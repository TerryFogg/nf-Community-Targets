//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NetworkIO.h"

static VOID NetworkIO::Entry(NX_IP_DRIVER *driver_req_ptr)
{

    /* Default to successful return.  */
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;

    /* Process according to the driver request type in the IP control block.  */
    switch (driver_req_ptr->nx_ip_driver_command)
    {
        case NX_LINK_PACKET_SEND:
        {

            /* Process packet send requests.  */
            nx_driver_packet_send(driver_req_ptr);
            break;
        }
        case NX_LINK_INITIALIZE:
        {

            /* Process link initialize requests.  */
            nx_driver_initialize(driver_req_ptr);
            break;
        }
        case NX_LINK_ENABLE:
        {

            /* Process link enable requests.  */
            nx_driver_enable(driver_req_ptr);
            break;
        }
        case NX_LINK_DISABLE:
        {

            /* Process link disable requests.  */
            nx_driver_disable(driver_req_ptr);
            break;
        }
        case NX_LINK_PACKET_BROADCAST:
        {

            /* Process packet send requests.  */
            nx_driver_packet_send(driver_req_ptr);
            break;
        }
        case NX_LINK_ARP_SEND:
        {

            /* Process packet send requests.  */
            nx_driver_packet_send(driver_req_ptr);
            break;
        }
        case NX_LINK_ARP_RESPONSE_SEND:
        {
            /* Process packet send requests.  */
            nx_driver_packet_send(driver_req_ptr);
            break;
        }
        case NX_LINK_RARP_SEND:
        {

            /* Process packet send requests.  */
            nx_driver_packet_send(driver_req_ptr);
            break;
        }
        case NX_LINK_MULTICAST_JOIN:
        {

            /* Process multicast join requests.  */
            nx_driver_multicast_join(driver_req_ptr);
            break;
        }
        case NX_LINK_MULTICAST_LEAVE:
        {

            /* Process multicast leave requests.  */
            nx_driver_multicast_leave(driver_req_ptr);
            break;
        }
        case NX_LINK_GET_STATUS:
        {

            /* Process get status requests.  */
            nx_driver_get_status(driver_req_ptr);
            break;
        }
        case NX_LINK_GET_SPEED:
        {
            break;
        }
        case NX_LINK_GET_DUPLEX_TYPE:
        {
            break;
        }
        case NX_LINK_GET_ERROR_COUNT:
        {
            break;
        }
        case NX_LINK_GET_RX_COUNT:
        {
            break;
        }
        case NX_LINK_GET_TX_COUNT:
        {
            break;
        }
        case NX_LINK_GET_ALLOC_ERRORS:
        {
            break;
        }
        case NX_LINK_UNINITIALIZE:
        {
            break;
        }
        case NX_LINK_DEFERRED_PROCESSING:
        {
            break;
        }
        case NX_LINK_INTERFACE_ATTACH:
        {

            /* Process link interface attach requests.  */
            nx_driver_interface_attach(driver_req_ptr);
            break;
        }
        case NX_LINK_SET_PHYSICAL_ADDRESS:
        {
            break;
        }
        case NX_INTERFACE_CAPABILITY_GET:
        {

            /* Process get capability requests.  */
            nx_driver_capability_get(driver_req_ptr);
            break;
        }
        case NX_INTERFACE_CAPABILITY_SET:
        {

            /* Process set capability requests.  */
            nx_driver_capability_set(driver_req_ptr);
            break;
        }
        case NX_LINK_INTERFACE_DETACH:
        {
            break;
        }
        case NX_LINK_FACTORY_ADDRESS_GET:
        {
            break;
        }
        case NX_LINK_RX_ENABLE:
        {
            break;
        }
        case NX_LINK_RX_DISABLE:
        {
            break;
        }
        case NX_LINK_6LOWPAN_COMMAND:
        {
            break;
        }
        case NX_LINK_GET_INTERFACE_TYPE:
        {
            break;
        }
        case NX_LINK_USER_COMMAND:
        {
            break;
        }
        default:

            /* Invalid driver request.  */

            /* Return the unhandled command status.  */
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

            /* Default to successful return.  */
            driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}
