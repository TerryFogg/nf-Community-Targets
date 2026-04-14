//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "network.h"
#include <nx_api.h>
#include <nxd_dhcp_client.h>
#include <nxd_dns.h>
#include "nx_driver_framework.h"
#include "NetworkInterfaceAdapter.h"
#include "NetworkConfigurationManager.h"

extern TX_BYTE_POOL byte_pool_0;
extern TX_THREAD TX_networkThread;

#define IP_THREAD_STACK_SIZE 3076
static ULONG ip_thread_stack[IP_THREAD_STACK_SIZE / sizeof(ULONG)];

#define IP_PRIORITY          5

static NX_IP IpInstance;
static NX_DHCP DhcpClient;
static NX_DNS DnsInstance;
static NX_PACKET_POOL Network_packet_pool;
static UCHAR ip_address[4];
static UCHAR network_mask[4];
UCHAR nx_driver_pico_w_link_up;

// Size of packet pool for a typical embedded product
//  2 TCP connections /  Periodic DNS  /  Occasional HTTP
// ( 2 * 10) + 15 = ~35 packets, allow 48 packets for safety
#define PACKET_COUNT        48
#define PACKET_PAYLOAD_SIZE 1536
#define PACKET_POOL_SIZE    ((PACKET_PAYLOAD_SIZE + sizeof(NX_PACKET)) * PACKET_COUNT)

// Sizing of ARP cache for simple IoT device for IP↔MAC mappings
// Gateway /  DHCP  /  1 cloud server  / 6–8 entries
// ~1–2 KB ARP cache
#define ARP_CACHE_SIZE 2048

NX_DRIVER_INFORMATION nx_driver_information;
NX_IP *ip_ptr;
NX_INTERFACE *interface_ptr;
ULONG actual_status;
NX_PACKET *packet_ptr;
ULONG *ethernet_frame_ptr;
void *arp_cache = TX_NULL;
void *packet_pool_memory = TX_NULL;
UCHAR *_nx_driver_buffer;

#define DRIVER_CACHE_BUFFER 1514

TX_EVENT_FLAGS_GROUP native_cyw43_events;

ULONG flags;
extern int g_Country_Code;
extern NetworkConfiguration networkConfiguration;

void NetworkThread(ULONG parameter)
{
    (void)parameter;
    volatile UINT status;
    NX_IP *ip_ptr = nx_driver_information.nx_driver_information_ip_ptr;

    status = tx_byte_allocate(&byte_pool_0, (VOID **)&arp_cache, ARP_CACHE_SIZE, TX_NO_WAIT);
    status = tx_byte_allocate(&byte_pool_0, (VOID **)&_nx_driver_buffer, DRIVER_CACHE_BUFFER, TX_NO_WAIT);
    status = tx_byte_allocate(&byte_pool_0, (VOID **)&packet_pool_memory, PACKET_POOL_SIZE, TX_NO_WAIT);

    // Initialize NetX Duo
    // The background thread created by nx_ip_create()
    // will call nx_driver_framework_entry_default() which will call into the physical network interface

    tx_event_flags_create(&native_cyw43_events, (char *)"CLR Wifi event");

    // The service nx_system_initialize must be called before any other NetX Duo service is called.
    nx_system_initialize();

    // This creates the central memory source for all network packets used by the stack.
    nx_packet_pool_create(
        &Network_packet_pool,
        (char *)"Main Packet Pool",
        PACKET_PAYLOAD_SIZE,
        packet_pool_memory,
        PACKET_POOL_SIZE);

    // Allocate memory from the network byte pool and creates and initializes a full IPv4/IPv6 networking context,
    // including protocol state, internal threads, and the connection to your network driver.
    // Use DHCP or set then real IP address to the interface at a later call

    nx_ip_create(
        &IpInstance,
        (char *)"NetX IP Instance 0",
        networkConfiguration.initialIP,
        networkConfiguration.initialGateway,
        &Network_packet_pool,
        nx_driver_framework_entry_default,
        ip_thread_stack,
        IP_THREAD_STACK_SIZE,
        IP_PRIORITY);

    // Create a table in RAM to store the IPv4 address  →  Ethernet MAC address
    status = nx_arp_enable(&IpInstance, arp_cache, ARP_CACHE_SIZE);

    // Enable ICMP for use by ping and error reporting,essential for diagnostics
    status = nx_icmp_enable(&IpInstance);

    // Enable the standard protocols
    status = nx_udp_enable(&IpInstance);
    status = nx_tcp_enable(&IpInstance);

    // Create a DHCP client instance and attach it to an IP instance and start
    status = nx_dhcp_create(&DhcpClient, &IpInstance, (char *)"DHCP Client");
    status = nx_dhcp_start(&DhcpClient);

    // Wait for the IP to become usable   (This is a blocking call)
    status = nx_ip_status_check(&IpInstance, NX_IP_ADDRESS_RESOLVED, &actual_status, NX_WAIT_FOREVER);

    // DNS:
    // If your code ever contains a hostname string, you need DNS.
    // DNS uses UDP and typically consumes :
    // 1 TX packet(DNS query)
    // 1 RX packet(DNS response)
    // 2 - 4 packets with retries
    status = nx_dns_create(&DnsInstance, &IpInstance, (UCHAR *)"DNS Client");
    // nx_dns_server_add(&DnsInstance, dns_server_ip);

    for (;;)
    {
        // Wait for events from the Wi-Fi driver, such as packet reception or link status changes.
        tx_event_flags_get(&native_cyw43_events, 0xFFFFFFFF, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);
        // Process received packets or handle link status changes based on the flags.
        if (flags & 0x01)
        {
            // Handle packet reception
            // This typically involves calling nx_ip_packet_receive() to get the packet and then processing it.
        }
        if (flags & 0x02)
        {
            // Handle link status change
            // This may involve checking the link status and updating the IP instance accordingly.
        }
    }
}

// A reference to this routine is passed in the nx_ip_create() earlier for
// pico-sdk routine _nx_ip_thread_entry to call
void nx_driver_framework_entry_default(NX_IP_DRIVER *driver_req_ptr)
{
    uint8_t mac[6];
    int status;
    ULONG bytes_copied;

    switch (driver_req_ptr->nx_ip_driver_command)
    {
        HAL_Configuration_Wireless80211 wifiConfig;
        case NX_LINK_INTERFACE_ATTACH:
            // Simple one-interface driver,additional logic is necessary for multiple port devices.
            nx_driver_information.nx_driver_information_interface = driver_req_ptr->nx_ip_driver_interface;
            driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            break;

        case NX_LINK_INITIALIZE:
            ip_ptr = driver_req_ptr->nx_ip_driver_ptr;
            interface_ptr = driver_req_ptr->nx_ip_driver_interface;
            nx_driver_information.nx_driver_information_ip_ptr = NX_NULL;
            nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_NOT_INITIALIZED;
            nx_driver_information.nx_driver_information_packet_pool_ptr = ip_ptr->nx_ip_default_packet_pool;
            nx_driver_information.nx_driver_information_deferred_events = 0;

            Wifi::Initialize(mac);

            // Update hardware address(mac)
            nx_driver_information.nx_driver_information_interface->nx_interface_physical_address_msw =
                (ULONG)((mac[0] << 8) | (mac[1]));
            nx_driver_information.nx_driver_information_interface->nx_interface_physical_address_lsw =
                (ULONG)((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | (mac[5]));

            nx_driver_information.nx_driver_information_ip_ptr = driver_req_ptr->nx_ip_driver_ptr;
            interface_ptr->nx_interface_ip_mtu_size = NX_DRIVER_MTU - NX_DRIVER_PHYSICAL_FRAME_SIZE;
            interface_ptr->nx_interface_address_mapping_needed = NX_TRUE;
            nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;
            driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            break;

        case NX_LINK_ENABLE:
            ip_ptr = driver_req_ptr->nx_ip_driver_ptr;
            if (nx_driver_information.nx_driver_information_state < NX_DRIVER_STATE_INITIALIZED)
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                return;
            }
            if (nx_driver_information.nx_driver_information_state >= NX_DRIVER_STATE_LINK_ENABLED)
            {
                driver_req_ptr->nx_ip_driver_status = NX_ALREADY_ENABLED;
                return;
            }

            if (Wifi::Enable())
            {
                nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_LINK_ENABLED;
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
                ip_ptr->nx_ip_driver_link_up = NX_TRUE;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            break;

        case NX_LINK_DISABLE:
            ip_ptr = driver_req_ptr->nx_ip_driver_ptr;
            if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            // TODO: Fix the wifiConfig
            else if (Wifi::Disconnect(wifiConfig))
            {
                ip_ptr->nx_ip_driver_link_up = NX_FALSE;
                nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            break;

        case NX_LINK_ARP_SEND:
        case NX_LINK_ARP_RESPONSE_SEND:
        case NX_LINK_PACKET_BROADCAST:
        case NX_LINK_RARP_SEND:
        case NX_LINK_PACKET_SEND:

            ip_ptr = driver_req_ptr->nx_ip_driver_ptr;
            if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
            {
                // Link is not up, simply free the packet.
                nx_packet_transmit_release(driver_req_ptr->nx_ip_driver_packet);
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                return;
            }

            // Place the ethernet frame at the front of the packet.
            packet_ptr = driver_req_ptr->nx_ip_driver_packet;
            packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr - NX_DRIVER_PHYSICAL_FRAME_SIZE;
            packet_ptr->nx_packet_length = packet_ptr->nx_packet_length + NX_DRIVER_PHYSICAL_FRAME_SIZE;

            // Adjust by 2 to get 32-bit word alignment.
            ethernet_frame_ptr = (ULONG *)(packet_ptr->nx_packet_prepend_ptr - 2);
            *ethernet_frame_ptr = driver_req_ptr->nx_ip_driver_physical_address_msw;
            *(ethernet_frame_ptr + 1) = driver_req_ptr->nx_ip_driver_physical_address_lsw;
            *(ethernet_frame_ptr + 2) =
                (ip_ptr->nx_ip_arp_physical_address_msw << 16) | (ip_ptr->nx_ip_arp_physical_address_lsw >> 16);
            *(ethernet_frame_ptr + 3) = (ip_ptr->nx_ip_arp_physical_address_lsw << 16);

            // Set up the frame type field in the Ethernet header
            if ((driver_req_ptr->nx_ip_driver_command == NX_LINK_ARP_SEND) ||
                (driver_req_ptr->nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND))
            {
                *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_ARP;
            }
            else if (driver_req_ptr->nx_ip_driver_command == NX_LINK_RARP_SEND)
            {
                *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_RARP;
            }
            else
            {
                *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IP;
            }

            // Endian swapping if NX_LITTLE_ENDIAN is defined, most ARM targets are little Endian
            NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr));
            NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 1));
            NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 2));
            NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 3));

            // Determine if the packet exceeds the driver's MTU.
            if (packet_ptr->nx_packet_length > NX_DRIVER_MTU)
            {
                NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                nx_packet_transmit_release(packet_ptr);
                return;
            }

            // Transmit the packet through the Network interface adapter low level access routine.
            if (packet_ptr->nx_packet_next)
            {
                if (packet_ptr->nx_packet_length > sizeof(_nx_driver_buffer))
                {
                    NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
                    nx_packet_transmit_release(packet_ptr);
                    status = NX_DRIVER_ERROR;
                    return;
                }

                // Extract data from packet to a contiguous buffer
                if (nx_packet_data_retrieve(packet_ptr, _nx_driver_buffer, &bytes_copied) != NX_SUCCESS)
                {
                    nx_packet_release(packet_ptr);
                    driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                }
                if (Wifi::Send(_nx_driver_buffer, bytes_copied))
                {
                    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
                }
                else
                {
                    driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                }
            }
            else
            {
                if (Wifi::Send(packet_ptr->nx_packet_prepend_ptr, packet_ptr->nx_packet_length))
                {
                    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
                }
                else
                {
                    driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                }
            }
            NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
            nx_packet_transmit_release(packet_ptr);

            break;

        case NX_LINK_GET_STATUS:
            if (Wifi::DriverStatus())
            {
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            break;

        case NX_LINK_MULTICAST_JOIN:
            if (Wifi::MultiCastJoin())
            {
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            break;
        case NX_LINK_MULTICAST_LEAVE:
            if (Wifi::MultiCastLeave())
            {
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            break;
        default:
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
    }
}

void nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr)
{
    USHORT packet_type;
    // Set the interface for the incoming packet.
    packet_ptr->nx_packet_ip_interface = nx_driver_information.nx_driver_information_interface;
    packet_type = (USHORT)(((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 12))) << 8) |
                  ((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 13)));

    if (packet_type == NX_DRIVER_ETHERNET_IP || packet_type == NX_DRIVER_ETHERNET_IPV6)
    {
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_PHYSICAL_FRAME_SIZE;
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_PHYSICAL_FRAME_SIZE;
        _nx_ip_packet_receive(ip_ptr, packet_ptr);
    }
    else if (packet_type == NX_DRIVER_ETHERNET_ARP)
    {
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_PHYSICAL_FRAME_SIZE;
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_PHYSICAL_FRAME_SIZE;
        _nx_arp_packet_deferred_receive(ip_ptr, packet_ptr);
    }
    else if (packet_type == NX_DRIVER_ETHERNET_RARP)
    {
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_PHYSICAL_FRAME_SIZE;
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_PHYSICAL_FRAME_SIZE;
        _nx_rarp_packet_deferred_receive(ip_ptr, packet_ptr);
    }
    else
    {
        // Invalid ethernet header... release the packet.
        nx_packet_release(packet_ptr);
    }
}
