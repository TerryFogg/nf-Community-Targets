
// Check that this file is included by the driver source and not compiled directly
#ifndef NX_DRIVER_SOURCE
#error This file is included by the driver source, not compiled directly.
#endif

// Include driver framework include file.
#include "nx_driver_framework.h"

NX_DRIVER_INFORMATION nx_driver_information;
NX_IP *ip_ptr;
NX_INTERFACE *interface_ptr;
UINT status;

static void nx_driver_framework_entry_default(NX_IP_DRIVER *driver_req_ptr);

#ifdef NX_DRIVER_ENABLE_DEFERRED
static void nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr);
#endif // NX_DRIVER_ENABLE_DEFERRED
static void nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr);
static void nx_driver_update_hardware_address(UCHAR hardware_address[6]);

// Define the pointers for the hardware implementation of this driver.
static UINT (*nx_driver_hardware_initialize)(NX_IP_DRIVER *driver_req_ptr) = NULL;
static UINT (*nx_driver_hardware_enable)(NX_IP_DRIVER *driver_req_ptr) = NULL;
static UINT (*nx_driver_hardware_disable)(NX_IP_DRIVER *driver_req_ptr) = NULL;
static UINT (*nx_driver_hardware_packet_send)(NX_PACKET *packet_ptr) = NULL;


static UINT (*nx_driver_hardware_multicast_join)(NX_IP_DRIVER *driver_req_ptr) = NULL;
static UINT (*nx_driver_hardware_multicast_leave)(NX_IP_DRIVER *driver_req_ptr) = NULL;
static UINT (*nx_driver_hardware_get_status)(NX_IP_DRIVER *driver_req_ptr) = NULL;
static void (*nx_driver_hardware_packet_transmitted)(void) = NULL;
static void (*nx_driver_hardware_packet_received)(void) = NULL;

// This is the default entry point processing for the NetX Driver.
// driver_req_ptr is the driver request from the IP layer.


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

            // Call the hardware-specific ethernet controller initialization.
            if (!nx_driver_hardware_initialize)
                status = NX_SUCCESS;
            else
                status = nx_driver_hardware_initialize(driver_req_ptr);

            // Determine if the request was successful.
            if (status == NX_SUCCESS)
            {
                nx_driver_information.nx_driver_information_ip_ptr = driver_req_ptr->nx_ip_driver_ptr;
                interface_ptr->nx_interface_ip_mtu_size = NX_DRIVER_MTU - NX_DRIVER_PHYSICAL_FRAME_SIZE;
                interface_ptr->nx_interface_address_mapping_needed = NX_TRUE;
                nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
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
            if (!nx_driver_hardware_enable)
                status = NX_SUCCESS;
            else
                status = nx_driver_hardware_enable(driver_req_ptr);
            if (status == NX_SUCCESS)
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
                return;
            }
            if (!nx_driver_hardware_disable)
                status = NX_SUCCESS;
            else
                status = nx_driver_hardware_disable(driver_req_ptr);
            if (status == NX_SUCCESS)
            {
                ip_ptr->nx_ip_driver_link_up = NX_FALSE;
                nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            break;

        case NX_LINK_ARP_SEND:
        case NX_LINK_ARP_RESPONSE_SEND:
        case NX_LINK_PACKET_BROADCAST:
        case NX_LINK_RARP_SEND:
        case NX_LINK_PACKET_SEND:
            NX_PACKET *packet_ptr;
            ULONG *ethernet_frame_ptr;

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
#ifdef FEATURE_NX_IPV6
            else if (packet_ptr->nx_packet_ip_version == NX_IP_VERSION_V6)
            {
                *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IPV6;
            }
#endif // FEATURE_NX_IPV6
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
            if (!nx_driver_hardware_packet_send)
            {
                status = NX_DRIVER_ERROR;
            }
            else
            {
                status = nx_driver_hardware_packet_send(packet_ptr);
            }
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

        case NX_LINK_MULTICAST_JOIN:

            if (!nx_driver_hardware_multicast_join)
            {
                status = NX_SUCCESS;
            }
            else
            {
                status = nx_driver_hardware_multicast_join(driver_req_ptr);
            }
            if (status != NX_SUCCESS)
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            break;

        case NX_LINK_MULTICAST_LEAVE:
            if (!nx_driver_hardware_multicast_leave)
            {
                status = NX_SUCCESS;
            }
            else
            {
                status = nx_driver_hardware_multicast_leave(driver_req_ptr);
            }
            if (status != NX_SUCCESS)
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            break;

        case NX_LINK_GET_STATUS:
            if (!nx_driver_hardware_get_status)
            {
                status = NX_SUCCESS;
            }
            else
            {
                status = nx_driver_hardware_get_status(driver_req_ptr);
            }
            if (status != NX_SUCCESS)
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            }
            break;

#ifdef NX_DRIVER_ENABLE_DEFERRED
        case NX_LINK_DEFERRED_PROCESSING:
        {

            // Process driver deferred requests.

            // Process a device driver function on behave of the IP thread.
            nx_driver_deferred_processing(driver_req_ptr);
            driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            break;
        }
#endif

        default:

            // Invalid driver request.

            // Return the unhandled command status.
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

            // Default to successful return.
            driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}

#ifdef NX_DRIVER_ENABLE_DEFERRED
//************************************************************************
//
//  FUNCTION                                               RELEASE
//
//    nx_driver_deferred_processing                       PORTABLE C
//                                                           6.x
//  AUTHOR
//
//    Andres Mlinar, Microsoft Corporation
//
//  DESCRIPTION
//
//    This function processing the deferred ISR action within the context
//    of the IP thread.
//
//  INPUT
//
//    driver_req_ptr                        Driver command from the IP
//                                            thread
//
//  OUTPUT
//
//    None
//
//  CALLS
//
//    nx_driver_packet_transmitted         Clean up after transmission
//    nx_driver_packet_received            Process a received packet
//
//  CALLED BY
//
//    Driver entry function
//
//  RELEASE HISTORY
//
//    DATE              NAME                      DESCRIPTION
//
//  xx-xx-xxxx     Andres Mlinar            Initial Version 6.x
//
//************************************************************************
static void nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr)
{

    TX_INTERRUPT_SAVE_AREA

    ULONG deferred_events;

    // Disable interrupts.
    TX_DISABLE

    // Pickup deferred events.
    deferred_events = nx_driver_information.nx_driver_information_deferred_events;
    nx_driver_information.nx_driver_information_deferred_events = 0;

    // Restore interrupts.
    TX_RESTORE

    // Check for a transmit complete event.
    if (deferred_events & NX_DRIVER_DEFERRED_PACKET_TRANSMITTED)
    {

        // Process transmitted packet(s).
        if (nx_driver_hardware_packet_transmitted)
            nx_driver_hardware_packet_transmitted();
    }

    // Check for received packet.
    if (deferred_events & NX_DRIVER_DEFERRED_PACKET_RECEIVED)
    {

        // Process received packet(s).
        if (nx_driver_hardware_packet_received)
            nx_driver_hardware_packet_received();
    }

    // Mark request as successful.
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
}
#endif // NX_DRIVER_ENABLE_DEFERRED

//************************************************************************
//
//  FUNCTION                                               RELEASE
//
//    nx_driver_transfer_to_netx                          PORTABLE C
//                                                           6.x
//  AUTHOR
//
//    Andres Mlinar, Microsoft Corporation
//
//  DESCRIPTION
//
//    This function processing incoming packets.  This routine would
//    be called from the driver-specific receive packet processing
//    function nx_driver_hardware_packet_received.
//
//  INPUT
//
//    ip_ptr                                Pointer to IP protocol block
//    packet_ptr                            Packet pointer
//
//  OUTPUT
//
//    Error indication
//
//  CALLS
//
//    _nx_ip_packet_receive                 NetX IP packet receive
//    _nx_ip_packet_deferred_receive        NetX IP packet receive
//    _nx_arp_packet_deferred_receive       NetX ARP packet receive
//    _nx_rarp_packet_deferred_receive      NetX RARP packet receive
//    _nx_packet_release                    Release packet
//
//  CALLED BY
//
//    nx_driver_hardware_packet_received    Driver packet receive function
//
//  RELEASE HISTORY
//
//    DATE              NAME                      DESCRIPTION
//
//  xx-xx-xxxx     Andres Mlinar            Initial Version 6.x
//
//************************************************************************
static void nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr)
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
        // Note:  The length reported by some Ethernet hardware includes
        //  bytes after the packet as well as the Ethernet header.  In some
        //  cases, the actual packet length after the Ethernet header should
        //  be derived from the length in the IP header (lower 16 bits of
        //  the first 32-bit word).

        // Clean off the Ethernet header.
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_PHYSICAL_FRAME_SIZE;

        // Adjust the packet length.
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_PHYSICAL_FRAME_SIZE;

        // Route to the ip receive function.
#ifdef NX_DRIVER_ENABLE_DEFERRED
        _nx_ip_packet_deferred_receive(ip_ptr, packet_ptr);
#else
        _nx_ip_packet_receive(ip_ptr, packet_ptr);
#endif // NX_DRIVER_ENABLE_DEFERRED
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

//************************************************************************
//
//  FUNCTION                                               RELEASE
//
//    nx_driver_update_hardware_address                   PORTABLE C
//                                                           6.x
//  AUTHOR
//
//    Andres Mlinar, Microsoft Corporation
//
//  DESCRIPTION
//
//    This function updates the hardware address kept by the driver.
//    The hardware address is used when constructing send packets and to
//    filter incoming packets.
//
//  INPUT
//
//    hardware_address                      The new hardware address
//
//  OUTPUT
//
//    None
//
//  CALLS
//
//    None
//
//  CALLED BY
//
//    nx_driver_initialize                  Driver initialization function
//
//  RELEASE HISTORY
//
//    DATE              NAME                      DESCRIPTION
//
//  xx-xx-xxxx     Andres Mlinar            Initial Version 6.x
//
//************************************************************************
void nx_driver_update_hardware_address(UCHAR hardware_address[6])
{

    NX_INTERFACE *interface_ptr;

    // Setup interface pointer.
    interface_ptr = nx_driver_information.nx_driver_information_interface;

    // Setup the physical address of this IP instance.
    // Increment the physical address lsw to simulate multiple nodes hanging on the ethernet.
    interface_ptr->nx_interface_physical_address_msw = (ULONG)((hardware_address[0] << 8) | (hardware_address[1]));
    interface_ptr->nx_interface_physical_address_lsw =
        (ULONG)((hardware_address[2] << 24) | (hardware_address[3] << 16) | (hardware_address[4] << 8) |
                (hardware_address[5]));
}
