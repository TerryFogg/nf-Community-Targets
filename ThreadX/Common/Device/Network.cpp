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

#include "pico/cyw43_arch.h"
#include "Wifi.h"

static NX_IP IpInstance;
static NX_DHCP DhcpInstance;
static NX_DNS DnsInstance;
static NX_PACKET_POOL Network_packet_pool;

// Size of packet pool for a typical embedded product
//  2 TCP connections /  Periodic DNS  /  Occasional HTTP
// ( 2 * 10) + 15 = ~35 packets, allow 48 packets for safety
#define PACKET_COUNT        48
#define PACKET_PAYLOAD_SIZE 1536
#define PACKET_POOL_SIZE    ((PACKET_PAYLOAD_SIZE + sizeof(NX_PACKET)) * PACKET_COUNT)
static UCHAR packet_pool_memory[PACKET_POOL_SIZE];

#define IP_THREAD_STACK_SIZE 3076
static ULONG ip_thread_stack[IP_THREAD_STACK_SIZE / sizeof(ULONG)];

// Sizing of ARP cache for simple IoT device for IP↔MAC mappings
// Gateway /  DHCP  /  1 cloud server  / 6–8 entries
// ~1–2 KB ARP cache
#define ARP_CACHE_SIZE 2048
static UCHAR arp_cache[ARP_CACHE_SIZE];

#define INITIAL_IP_ADDRESS   IP_ADDRESS(0, 0, 0, 0)
#define INITIAL_NETWORK_MASK 0xFFFFFF00UL
#define CONTROL_BLOCK_SIZE   sizeof(NX_IP)

NX_DRIVER_INFORMATION nx_driver_information;
NX_IP *ip_ptr;
NX_INTERFACE *interface_ptr;
ULONG actual_status;
TX_THREAD nx_driver_pico_w_thread;
NX_PACKET *packet_ptr;
ULONG *ethernet_frame_ptr;
UCHAR _nx_driver_buffer[1514];

TX_EVENT_FLAGS_GROUP wifi_events;

ULONG flags;

void NetworkThread_entry(uint32_t parameter)
{

    // The following blocks and returns after the network is usable
    // Status : NX_IP_ADDRESS_RESOLVED
    NetXDuoStartup();

    while (1)
    {
        tx_event_flags_get(
            &wifi_events,
            EVT_WIFI_RX | EVT_WIFI_LINK_UP | EVT_WIFI_LINK_DOWN,
            TX_OR_CLEAR,
            &flags,
            TX_WAIT_FOREVER);
    }
    //{
    //    NX_IP *ip_ptr = nx_driver_information.nx_driver_information_ip_ptr;
    //    NX_PARAMETER_NOT_USED(thread_input);
    //    for (;;)
    //    {
    //        if (nx_driver_information.nx_driver_information_ip_ptr->nx_ip_driver_link_up)
    //        {
    //            // Obtain the IP internal mutex before processing the IP event.
    //            tx_mutex_get(&(ip_ptr->nx_ip_protection), TX_WAIT_FOREVER);
    //            cyw43_arch_poll();

    //            // Release the IP internal mutex before processing the IP event.
    //            tx_mutex_put(&(ip_ptr->nx_ip_protection));
    //        }
    //        // Sleep some ticks to next loop
    //        tx_thread_sleep(NX_DRIVER_THREAD_INTERVAL);
    //    }
    //}
}

// This is called from a "Network Thread" because it blocks until the network is up
bool NetXDuoStartup()
{
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
        INITIAL_IP_ADDRESS,
        INITIAL_NETWORK_MASK,
        &Network_packet_pool,
        nx_driver_framework_entry_default,
        ip_thread_stack,
        IP_THREAD_STACK_SIZE,
        CONTROL_BLOCK_SIZE);

    // Create a table in RAM to store the IPv4 address  →  Ethernet MAC address
    nx_arp_enable(&IpInstance, (void *)arp_cache, ARP_CACHE_SIZE);

    // Enable ICMP for use by ping and error reporting
    // Helps TCP behave correctly under failure, essential for diagnostics
    nx_icmp_enable(&IpInstance);

    // At this point :
    // The IP stack exists
    // The link driver is initialized
    // IPv4 control protocols are ready
    // No application traffic can flow yet

    // Enable the standard protocols
    nx_udp_enable(&IpInstance);
    nx_tcp_enable(&IpInstance);

    // Create a DHCP client instance and attache it to an IP instance, then start it
    nx_dhcp_create(&DhcpInstance, &IpInstance, (char *)"DHCP");
    nx_dhcp_start(&DhcpInstance);

    // Wait for the IP to become usable   (BLOCKING)
    nx_ip_status_check(&IpInstance, NX_IP_ADDRESS_RESOLVED, &actual_status, NX_WAIT_FOREVER);

    // If your code ever contains a hostname string, you need DNS.
    // DNS uses UDP and typically consumes :
    // 1 TX packet(DNS query)
    // 1 RX packet(DNS response)
    // 2 - 4 packets with retries
    nx_dns_create(&DnsInstance, &IpInstance, (UCHAR *)"DNS Client");
    // nx_dns_server_add(&DnsInstance, dns_server_ip);

    return true;
}

void nx_driver_framework_entry_default(NX_IP_DRIVER *driver_req_ptr)
{
    switch (driver_req_ptr->nx_ip_driver_command)
    {
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

            uint8_t *mac;

            Wifi::Initialize(mac);
            cyw43_arch_init();
            cyw43_arch_enable_sta_mode();
            cyw43_hal_get_mac(0, mac);

            // nx_driver_update_hardware_address(mac);
            {
                NX_INTERFACE *interface_ptr;
                interface_ptr = nx_driver_information.nx_driver_information_interface;
                interface_ptr->nx_interface_physical_address_msw = (ULONG)((mac[0] << 8) | (mac[1]));
                interface_ptr->nx_interface_physical_address_lsw =
                    (ULONG)((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | (mac[5]));
            }

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
            cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, WIFI_AUTH_TYPE, WIFI_CONNECT_TIMEOUT);
            tx_thread_reset(&nx_driver_pico_w_thread);
            tx_thread_resume(&nx_driver_pico_w_thread);
            nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_LINK_ENABLED;
            driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            ip_ptr->nx_ip_driver_link_up = NX_TRUE;
            break;

        case NX_LINK_DISABLE:
            ip_ptr = driver_req_ptr->nx_ip_driver_ptr;
            if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                return;
            }
            //  ? Reset all sockets here ?
            tx_thread_suspend(&nx_driver_pico_w_thread);
            tx_thread_terminate(&nx_driver_pico_w_thread);
            ip_ptr->nx_ip_driver_link_up = NX_FALSE;
            nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;
            driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            break;

        case NX_LINK_ARP_SEND:
        case NX_LINK_ARP_RESPONSE_SEND:
        case NX_LINK_PACKET_BROADCAST:
        case NX_LINK_RARP_SEND:
        case NX_LINK_PACKET_SEND:

            ip_ptr = driver_req_ptr->nx_ip_driver_ptr;
            if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                nx_packet_transmit_release(driver_req_ptr->nx_ip_driver_packet);
                return;
            }
            packet_ptr = driver_req_ptr->nx_ip_driver_packet;
            packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr - NX_DRIVER_PHYSICAL_FRAME_SIZE;
            packet_ptr->nx_packet_length = packet_ptr->nx_packet_length + NX_DRIVER_PHYSICAL_FRAME_SIZE;

            // adjust by 2 to get 32-bit word alignment.
            ethernet_frame_ptr = (ULONG *)(packet_ptr->nx_packet_prepend_ptr - 2);
            *ethernet_frame_ptr = driver_req_ptr->nx_ip_driver_physical_address_msw;
            *(ethernet_frame_ptr + 1) = driver_req_ptr->nx_ip_driver_physical_address_lsw;
            *(ethernet_frame_ptr + 2) =
                (ip_ptr->nx_ip_arp_physical_address_msw << 16) | (ip_ptr->nx_ip_arp_physical_address_lsw >> 16);
            *(ethernet_frame_ptr + 3) = (ip_ptr->nx_ip_arp_physical_address_lsw << 16);
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

            // Endian swapping if NX_LITTLE_ENDIAN is defined.
            NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr));
            NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 1));
            NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 2));
            NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 3));

            // Determine if the packet exceeds the driver's MTU.
            if (packet_ptr->nx_packet_length > NX_DRIVER_MTU)
            {
                // This packet exceeds the size of the driver's MTU. Simply throw it away!
                // Remove the Ethernet header.
                NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                nx_packet_transmit_release(packet_ptr);
                return;
            }
            // Transmit the packet through the Ethernet controller low level access routine.
            int status;
            ULONG bytes_copied;
            if (packet_ptr->nx_packet_next)
            {
                if (packet_ptr->nx_packet_length > sizeof(_nx_driver_buffer))
                {
                    // Buffer too small to hold packet, release the packet
                    NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
                    nx_packet_transmit_release(packet_ptr);
                }

                // Extract data from packet to contigous buffer
                if (nx_packet_data_retrieve(packet_ptr, _nx_driver_buffer, &bytes_copied))
                {
                    // Error, release the packet
                    nx_packet_release(packet_ptr);
                }
                status = cyw43_send_ethernet(&cyw43_state, 0, bytes_copied, _nx_driver_buffer, false);
            }
            else
            {
                status = cyw43_send_ethernet(
                    &cyw43_state,
                    0,
                    packet_ptr->nx_packet_length,
                    packet_ptr->nx_packet_prepend_ptr,
                    false);
            }
            NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
            nx_packet_transmit_release(packet_ptr);

            if (status != NX_SUCCESS)
            {
                NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                nx_packet_transmit_release(packet_ptr);
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            break;

        case NX_LINK_GET_STATUS:
            driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            break;

        case NX_LINK_MULTICAST_JOIN:
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
            break;

        case NX_LINK_MULTICAST_LEAVE:
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
            break;

        default:
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
    }
}

int cyw43_tcpip_link_status(cyw43_t *self, int itf)
{
    if (true)
    {
        return CYW43_LINK_UP;
    }
    else
    {
        return cyw43_wifi_link_status(self, itf);
    }
}
