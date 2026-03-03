//
// Copyright(c).NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nx_driver_pico_w.h"
#include "Wifi.h"
#include "nx_driver_framework.h"
#include "pico/cyw43_arch.h"

NX_DRIVER_INFORMATION nx_driver_information;
NX_IP *ip_ptr;
NX_INTERFACE *interface_ptr;
UINT status;
UCHAR _nx_driver_buffer[1514];
TX_THREAD nx_driver_pico_w_thread;
UCHAR nx_driver_pico_w_thread_stack[NX_DRIVER_STACK_SIZE];
bool nx_driver_pico_w_link_up;
UCHAR mac[6];
UINT priority = 0;
NX_PACKET *packet_ptr;
ULONG *ethernet_frame_ptr;

void _nx_driver_thread_entry(ULONG thread_input)
{
    NX_IP *ip_ptr = nx_driver_information.nx_driver_information_ip_ptr;
    NX_PARAMETER_NOT_USED(thread_input);
    for (;;)
    {
        if (nx_driver_information.nx_driver_information_ip_ptr->nx_ip_driver_link_up)
        {
            // Obtain the IP internal mutex before processing the IP event.
            tx_mutex_get(&(ip_ptr->nx_ip_protection), TX_WAIT_FOREVER);
            cyw43_arch_poll();

            // Release the IP internal mutex before processing the IP event.
            tx_mutex_put(&(ip_ptr->nx_ip_protection));
        }
        // Sleep some ticks to next loop
        tx_thread_sleep(NX_DRIVER_THREAD_INTERVAL);
    }
}
int cyw43_tcpip_link_status(cyw43_t *self, int itf)
{
    if (nx_driver_pico_w_link_up)
    {
        return CYW43_LINK_UP;
    }
    else
    {
        return cyw43_wifi_link_status(self, itf);
    }
}
void cyw43_cb_tcpip_init(cyw43_t *self, int itf)
{
    if (nx_driver_pico_w_link_up)
    {
        nx_driver_information.nx_driver_information_ip_ptr->nx_ip_driver_link_up = NX_TRUE;
        _nx_ip_driver_link_status_event(
            nx_driver_information.nx_driver_information_ip_ptr,
            nx_driver_information.nx_driver_information_interface->nx_interface_index);
    }
}
void cyw43_cb_tcpip_deinit(cyw43_t *self, int itf)
{
    if (nx_driver_pico_w_link_up)
    {
        nx_driver_information.nx_driver_information_ip_ptr->nx_ip_driver_link_up = NX_FALSE;
        _nx_ip_driver_link_status_event(
            nx_driver_information.nx_driver_information_ip_ptr,
            nx_driver_information.nx_driver_information_interface->nx_interface_index);
    }
}
void cyw43_cb_tcpip_set_link_up(cyw43_t *self, int itf)
{
    nx_driver_pico_w_link_up = true;
}
void cyw43_cb_tcpip_set_link_down(cyw43_t *self, int itf)
{
    nx_driver_pico_w_link_up = false;
}
void cyw43_cb_process_ethernet(void *cb_data, int itf, size_t len, const uint8_t *buf)
{
    NX_PACKET *packet_ptr;
    if (!nx_driver_pico_w_link_up)
    {
        return;
    }
    // Allocate a packet to receive data
    if (nx_packet_allocate(
            nx_driver_information.nx_driver_information_packet_pool_ptr,
            &packet_ptr,
            NX_RECEIVE_PACKET,
            NX_NO_WAIT))
    {
        return;
    }
    // Align IP header to 4 bytes boundary
    packet_ptr->nx_packet_prepend_ptr += 2;
    packet_ptr->nx_packet_append_ptr += 2;
    if (nx_packet_data_append(
            packet_ptr,
            (VOID *)buf,
            len,
            nx_driver_information.nx_driver_information_packet_pool_ptr,
            NX_NO_WAIT))
    {
        // Error, release the packet
        nx_packet_release(packet_ptr);
        return;
    }
    // Everything is OK, transfer the packet to NetX
    nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, packet_ptr);
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

            cyw43_arch_init();
            cyw43_arch_enable_sta_mode();
            cyw43_hal_get_mac(0, mac);
            nx_driver_update_hardware_address(mac);
            // Get priority of IP thread
            tx_thread_info_get(
                tx_thread_identify(),
                NX_NULL,
                NX_NULL,
                NX_NULL,
                &priority,
                NX_NULL,
                NX_NULL,
                NX_NULL,
                NX_NULL);

            // Create the driver thread
            // The priority of network thread is lower than IP thread
            tx_thread_create(
                &nx_driver_pico_w_thread,
                (char *)"Driver Thread",
                _nx_driver_thread_entry,
                0,
                nx_driver_pico_w_thread_stack,
                NX_DRIVER_STACK_SIZE,
                priority + 1,
                priority + 1,
                TX_NO_TIME_SLICE,
                TX_DONT_START);

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
void nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr)
{
    USHORT packet_type;
    // Set the interface for the incoming packet.
    packet_ptr->nx_packet_ip_interface = nx_driver_information.nx_driver_information_interface;

    // Pickup the packet header to determine where the packet needs to be sent.
    packet_type = (USHORT)(((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 12))) << 8) |
                  ((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 13)));

    // Route the incoming packet according to its ethernet type.
    if (packet_type == NX_DRIVER_ETHERNET_IP || packet_type == NX_DRIVER_ETHERNET_IPV6)
    {
        // Note:  The length reported by some Ethernet hardware includes bytes after the packet as well as the Ethernet
        // header.
        //  In some cases, the actual packet length after the Ethernet header should be derived from the length in the
        //  IP header
        // (lower 16 bits of the first 32-bit word).

        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_PHYSICAL_FRAME_SIZE;

        // Adjust the packet length.
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_PHYSICAL_FRAME_SIZE;

        // Route to the ip receive function.
        _nx_ip_packet_receive(ip_ptr, packet_ptr);
    }
    else if (packet_type == NX_DRIVER_ETHERNET_ARP)
    {
        // Clean off the Ethernet header.
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_PHYSICAL_FRAME_SIZE;

        // Adjust the packet length.
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_PHYSICAL_FRAME_SIZE;

        // Route to the ARP receive function.
        _nx_arp_packet_deferred_receive(ip_ptr, packet_ptr);
    }
    else if (packet_type == NX_DRIVER_ETHERNET_RARP)
    {
        // Clean off the Ethernet header.
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_PHYSICAL_FRAME_SIZE;

        // Adjust the packet length.
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_PHYSICAL_FRAME_SIZE;

        // Route to the RARP receive function.
        _nx_rarp_packet_deferred_receive(ip_ptr, packet_ptr);
    }
    else
    {
        // Invalid ethernet header... release the packet.
        nx_packet_release(packet_ptr);
    }
}
void nx_driver_update_hardware_address(UCHAR hardware_address[6])
{
    NX_INTERFACE *interface_ptr;
    interface_ptr = nx_driver_information.nx_driver_information_interface;
    interface_ptr->nx_interface_physical_address_msw = (ULONG)((hardware_address[0] << 8) | (hardware_address[1]));
    interface_ptr->nx_interface_physical_address_lsw =
        (ULONG)((hardware_address[2] << 24) | (hardware_address[3] << 16) | (hardware_address[4] << 8) |
                (hardware_address[5]));
}
UINT _nx_driver_packet_send(NX_PACKET *packet_ptr)
{
    int status;
    ULONG bytes_copied;
    if (packet_ptr->nx_packet_next)
    {
        if (packet_ptr->nx_packet_length > sizeof(_nx_driver_buffer))
        {
            // Buffer too small to hold packet, release the packet
            NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
            nx_packet_transmit_release(packet_ptr);
            return NX_DRIVER_ERROR;
        }
        // Extract data from packet to contigous buffer
        if (nx_packet_data_retrieve(packet_ptr, _nx_driver_buffer, &bytes_copied))
        {
            // Error, release the packet
            nx_packet_release(packet_ptr);
            return NX_DRIVER_ERROR;
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
    if (status)
    {
        return NX_DRIVER_ERROR;
    }
    return NX_SUCCESS;
}
