//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "System.Device.Network.h"
#include "ManagedThreadSupport.h"
#include "System.Device.Wifi.h"

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::socket___STATIC__I4__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 family_requested = stack.Arg0().NumericByRef().s4;
        CLR_INT32 family = stack.Arg0().NumericByRef().s4;
        CLR_INT32 type = stack.Arg1().NumericByRef().s4;
        CLR_INT32 protocol = stack.Arg2().NumericByRef().s4;

        UINT status;
        NX_PACKET *response_packet;
        ULONG round_trip_time;
        NX_PACKET *packet;
        NX_IP_STRUCT igmp_nx_struct;
        NX_IP ip_ptr;
        ULONG destination_ip = IP_ADDRESS(192, 168, 1, 1);
        socket_entry_t *socket_entry;

        switch (family_requested)
        {
            case SOCK_AF_UNSPEC:
                family = AF_UNSPEC;
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;

            case SOCK_AF_INET:
                family = AF_INET;
                break;

            case SOCK_AF_INET6:
                family = AF_INET6;
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;

            default:
                family = AF_UNSPEC;
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
                // family = AF_NS          - Local to host (pipes, portals).
                // family = AF_PACKET 4    - Raw Packet type (Link Layer packets)
        }

        switch (type)
        {
            case SOCK_SOCK_STREAM:
                break;
            case SOCK_SOCK_DGRAM:
                break;
            case SOCK_SOCK_RAW:
                break;
            case Rdm:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
            case Seqpacket:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
        }

        switch (protocol)
        {
            case SOCK_IPPROTO_TCP:
                socket_entry->tcp_socket = (NX_TCP_SOCKET *)platform_malloc(sizeof(NX_TCP_SOCKET));
                nx_tcp_socket_create(
                    &ip_ptr,
                    socket_entry->tcp_socket,
                    (char *)"TCP Socket",
                    NX_IP_NORMAL,
                    NX_FRAGMENT_OKAY,
                    NX_IP_TIME_TO_LIVE,
                    200, // NX_TCP_WINDOW_SIZE,
                    NULL,
                    NULL);
                nx_tcp_enable(&ip_ptr);
                break;

            case SOCK_IPPROTO_UDP:
                socket_entry->udp_socket = (NX_UDP_SOCKET *)platform_malloc(sizeof(NX_UDP_SOCKET));
                status = nx_udp_socket_create(
                    &ip_ptr,
                    socket_entry->udp_socket,
                    (char *)"Socket 0",
                    NX_IP_NORMAL,
                    NX_FRAGMENT_OKAY,
                    0x80,
                    5);
                nx_udp_enable(&ip_ptr);
                break;

            // Internet Control Message Protocol
            // NOTE: Enabled but not used in NETXDUO in a socket format
            case SOCK_IPPROTO_ICMP:
                nx_icmp_enable(&ip_ptr);
                break;

                // NOTE: Enabled but not used in NETXDUO in a socket format
            case SOCK_IPPROTO_IGMP:
                nx_igmp_enable(&ip_ptr);
                break;

            case SOCK_IPPROTO_RAW:
                nx_ip_raw_packet_enable(&ip_ptr);
                break;
            case SOCK_IPPROTO_IP:
            case SOCK_IPPROTO_IPV4:
            case SOCK_IPPROTO_PUP:
            case SOCK_IPPROTO_IDP:
            case SOCK_IPPROTO_IPV6:
            case SOCK_IPPROTO_IPv6RoutingHeader:
            case SOCK_IPPROTO_IPv6FragmentHeader:
            case SOCK_IPPROTO_RDP:
            case SOCK_IPPROTO_GRE:
            case SOCK_IPPROTO_IPSecEncapsulatingSecurityPayload:
            case SOCK_IPPROTO_IPSecAuthenticationHeader:
            case SOCK_IPPROTO_IcmpV6:
            case SOCK_IPPROTO_IPv6NoNextHeader:
            case SOCK_IPPROTO_IPv6DestinationOptions:
            case SOCK_IPPROTO_ND:
            case SOCK_IPPROTO_OSPF:
            case SOCK_IPPROTO_TPACKET:
            case SOCK_IPPROTO_SPX:
            case SOCK_IPPROTO_SPXII:
            case SOCK_SOL_SOCKET:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
        }
        if (status != NX_SUCCESS)
        {
            TranslateNXErrorToSocketError(stack, status);
        }
        stack.SetResult_I4((CLR_UINT32)socket_entry);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::bind___STATIC__VOID__OBJECT__SystemNetEndPoint(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        SOCK_sockaddr sock_addr;
        UINT sock_addr_len;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        NANOCLR_CHECK_HRESULT(MarshalSockAddress(&sock_addr, sock_addr_len, stack.Arg1()));

        // sock_addr.sa_family = AF_INET;
        // sock_addr.sa_data;

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_UDP:
                // socket_entry->tcp_socket->nx_tcp_socket_connect_ip.nxd_ip_address.v4 =
                //(ULONG)sock_addr.sa_data.ip_addr;
                status = nx_udp_socket_bind(
                    (NX_UDP_SOCKET *)socket_entry->tcp_socket,
                    socket_entry->tcp_socket->nx_tcp_socket_port,
                    2 * NX_IP_PERIODIC_RATE);
                break;
            case SOCKET_TYPE_TCP:
                //
                status = nx_tcp_client_socket_bind(
                    (NX_TCP_SOCKET *)socket_entry->udp_socket,
                    socket_entry->tcp_socket->nx_tcp_socket_port,
                    2 * NX_IP_PERIODIC_RATE);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::
    connect___STATIC__VOID__OBJECT__SystemNetEndPoint__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        SOCK_sockaddr addr;
        CLR_UINT32 addrLen = sizeof(addr);
        bool fThrowOnWouldBlock = false;
        ULONG server_ip = 0;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        NANOCLR_CHECK_HRESULT(MarshalSockAddress(&addr, addrLen, stack.Arg1()));
        fThrowOnWouldBlock = (stack.Arg2().NumericByRefConst().s4 != 0);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                status = nx_tcp_client_socket_connect(
                    (NX_TCP_SOCKET *)socket_entry->tcp_socket,
                    server_ip,
                    12345,
                    NX_WAIT_FOREVER);
                break;
            case SOCKET_TYPE_UDP:
                // UDP sockets do not connect like TCP sockets, so we just return not implemented here.
                NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Sockets_NativeSocket::send___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT64 *timeout;
        CLR_UINT8 *buf;
        bool fRes = true;
        CLR_INT32 ret = 0;
        SOCK_sockaddr addr;
        CLR_UINT32 addrLen = sizeof(addr);

        NX_PACKET *packet_ptr;
        ULONG ipaddress;
        ULONG port;
        ULONG wait_option;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        FAULT_ON_NULL(arrData);

        CLR_INT32 totReadWrite = stack.m_evalStack[1].NumericByRef().s4;
        CLR_UINT32 offset = stack.Arg2().NumericByRef().u4;
        CLR_UINT32 count = stack.Arg3().NumericByRef().u4;
        CLR_INT32 flags = stack.Arg4().NumericByRef().s4;
        CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

        if (offset + count > arrData->m_numOfElements)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }
        buf = arrData->GetElement(offset + totReadWrite);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                status = nx_tcp_socket_send((NX_TCP_SOCKET *)socket_entry->tcp_socket, packet_ptr, wait_option);
                break;
            case SOCKET_TYPE_UDP:
                status = nx_udp_socket_send((NX_UDP_SOCKET *)socket_entry->udp_socket, packet_ptr, ipaddress, port);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);

        stack.SetResult_I4(totReadWrite);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Sockets_NativeSocket::recv___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        CLR_UINT32 offset = stack.Arg2().NumericByRef().u4;
        CLR_UINT32 count = stack.Arg3().NumericByRef().u4;
        CLR_INT32 flags = stack.Arg4().NumericByRef().s4;
        CLR_RT_HeapBlock hbTimeout;

        NX_PACKET *packet_ptr;
        ULONG ipaddress;
        ULONG port;
        ULONG wait_option;

        CLR_INT64 *timeout;
        CLR_UINT8 *buf;
        bool fRes = true;
        CLR_INT32 totReadWrite;
        CLR_INT32 ret = 0;

        FAULT_ON_NULL(arrData);

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                status = nx_tcp_socket_receive((NX_TCP_SOCKET *)socket_entry->tcp_socket, &packet_ptr, wait_option);
                break;
            case SOCKET_TYPE_UDP:
                status = nx_udp_socket_receive((NX_UDP_SOCKET *)socket_entry->udp_socket, &packet_ptr, ipaddress);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);

        stack.SetResult_I4(totReadWrite);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::close___STATIC__I4__OBJECT(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        NX_TCP_SOCKET tcp_socket;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                status = nx_tcp_socket_disconnect(socket_entry->tcp_socket, NX_WAIT_FOREVER);
                nx_tcp_socket_delete(socket_entry->tcp_socket);
                platform_free(socket_entry->tcp_socket);
                Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
                break;
            case SOCKET_TYPE_UDP:
                nx_udp_socket_delete(socket_entry->udp_socket);
                nx_udp_socket_unbind(socket_entry->udp_socket);
                platform_free(socket_entry->udp_socket);
                Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::listen___STATIC__VOID__OBJECT__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NX_IP ip_ptr;

        NX_TCP_SOCKET tcp_socket;
        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        CLR_INT16 port = stack.Arg0().NumericByRef().s4;
        CLR_INT16 queue_size = stack.Arg1().NumericByRef().s4;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        CLR_INT32 backlog = stack.Arg1().NumericByRef().s4;

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                break;
                status = nx_tcp_server_socket_listen(
                    &ip_ptr,
                    port,
                    socket_entry->tcp_socket,
                    queue_size,
                    tcp_server_listen_callback);
            case SOCKET_TYPE_UDP:
                // UDP sockets do not connect like TCP sockets, so we just return not implemented here.
                NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::accept___STATIC__I4__OBJECT(CLR_RT_StackFrame &stack)
{

    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                nx_tcp_server_socket_accept(socket_entry->tcp_socket, NX_WAIT_FOREVER);
                break;
            case SOCKET_TYPE_UDP:
                // UDP sockets do not accept like TCP sockets, so we just return not implemented here.
                NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::
    getaddrinfo___STATIC__VOID__STRING__BYREF_STRING__BYREF_SZARRAY_SZARRAY_U1(CLR_RT_StackFrame &stack)
{

    // NetXDuo does not abstract address resolution into a unified structure like addrinfo.
    // You handle IP and port separately.

    NANOCLR_HEADER();
    {
        NX_IP ip_ptr;

        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        NX_DNS client_dns;
        UCHAR *host_name;
        ULONG *host_address_ptr;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        status = nx_dns_create(&client_dns, &ip_ptr, (UCHAR *)"DNS Client");
        nx_dns_server_add(&client_dns, IP_ADDRESS(192, 168, 1, 1));
        unsigned char ip_address[4] = {0, 0, 0, 0};
        status = nx_dns_host_by_name_get(&client_dns, host_name, host_address_ptr, NX_IP_PERIODIC_RATE);

        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::shutdown___STATIC__VOID__OBJECT__I4__BYREF_I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;

        NX_TCP_SOCKET tcp_socket;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                nx_tcp_socket_disconnect(socket_entry->tcp_socket, NX_WAIT_FOREVER);
                nx_tcp_socket_delete(socket_entry->tcp_socket);
                break;
            case SOCKET_TYPE_UDP:
                nx_udp_socket_unbind(socket_entry->udp_socket);
                nx_udp_socket_delete(socket_entry->udp_socket);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }

    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::
    sendto___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__SystemNetEndPoint(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        NX_PACKET *packet_ptr_ptr;
        ULONG ipaddress;
        ULONG port;

        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        CLR_UINT32 offset = stack.Arg2().NumericByRef().u4;
        CLR_UINT32 count = stack.Arg3().NumericByRef().u4;
        CLR_INT32 flags = stack.Arg4().NumericByRef().s4;
        CLR_RT_HeapBlock hbTimeout;

        CLR_INT64 *timeout;
        CLR_UINT8 *buf;
        bool fRes = true;
        CLR_INT32 totReadWrite;
        CLR_INT32 ret = 0;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        FAULT_ON_NULL(arrData);

        if (offset + count > arrData->m_numOfElements)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                nx_tcp_socket_send(socket_entry->tcp_socket, packet_ptr_ptr, NX_WAIT_FOREVER);
                break;
            case SOCKET_TYPE_UDP:
                nx_udp_socket_send(socket_entry->udp_socket, packet_ptr_ptr, ipaddress, port);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);

        stack.SetResult_I4(totReadWrite);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::
    recvfrom___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__BYREF_SystemNetEndPoint(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;

        NX_PACKET *packet_ptr_ptr;
        ULONG ipaddress;
        ULONG port;

        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        CLR_UINT32 offset = stack.Arg2().NumericByRef().u4;
        CLR_UINT32 count = stack.Arg3().NumericByRef().u4;
        CLR_INT32 flags = stack.Arg4().NumericByRef().s4;
        CLR_RT_HeapBlock hbTimeout;

        CLR_INT64 *timeout;
        CLR_UINT8 *buf;
        bool fRes = true;
        CLR_INT32 totReadWrite;
        CLR_INT32 ret = 0;

        FAULT_ON_NULL(arrData);

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        if (offset + count > arrData->m_numOfElements)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }
        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                nx_tcp_socket_receive(socket_entry->tcp_socket, &packet_ptr_ptr, NX_WAIT_FOREVER);
                // copy packet data to buffer
                break;
            case SOCKET_TYPE_UDP:
                nx_udp_socket_receive(socket_entry->udp_socket, &packet_ptr_ptr, ipaddress);
                break;
            default:
                break;
        }

        if (status == NX_SUCCESS)
        {
            // copy packet data to buffer
            // TODO: Handle offset and count correctly
            memcpy(arrData->GetElement(offset + totReadWrite), packet_ptr_ptr->nx_packet_prepend_ptr, count);
            nx_packet_release(packet_ptr_ptr);
        }
        else
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);

        stack.SetResult_I4(totReadWrite);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::
    getpeername___STATIC__VOID__OBJECT__BYREF_SystemNetEndPoint(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;

        NX_TCP_SOCKET tcp_socket;
        NX_UDP_SOCKET udp_socket;
        NX_PACKET *packet;
        ULONG peer_ip;
        ULONG peer_port;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                status = nx_tcp_socket_peer_info_get(socket_entry->tcp_socket, &peer_ip, &peer_port);
                break;
            case SOCKET_TYPE_UDP:
                // UDP sockets do not have peers like TCP sockets, so we just return not implemented here.
                NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::
    getsockname___STATIC__VOID__OBJECT__BYREF_SystemNetEndPoint(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        NX_TCP_SOCKET tcp_socket;
        NX_UDP_SOCKET udp_socket;
        NX_PACKET *packet;
        ULONG peer_ip;
        ULONG peer_port;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                socket_entry->tcp_socket->nx_tcp_socket_name;
                break;
            case SOCKET_TYPE_UDP:
                socket_entry->udp_socket->nx_udp_socket_name;
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Sockets_NativeSocket::getsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        UINT socket_option_level;
        UINT socket_option_name;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        switch (socket_option_level)
        {
            // Indicates socket options apply to the socket itself
            case SOCK_SOL_SOCKET:
                break;
            // Apply to sockets created for the IPv4 address family (AF_INET)
            case SOCK_IPPROTO_IP:
                break;
            // Apply to sockets created for the IPv6 address family (AF_INET6)
            case SOCK_IPPROTO_IPV6:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
                break;
                // Apply to sockets created for the IPv4 and IPv6 address families (AF_INET and AF_INET6)
                // with the protocol parameter to the socket function specified as TCP (IPPROTO_TCP)
            case SOCK_IPPROTO_TCP:
                break;
            // apply to sockets created for the IPv4 and IPv6 address families (AF_INET and AF_INET6)
            //  with the protocol parameter to the socket function specified as UDP (IPPROTO_UDP)
            case SOCK_IPPROTO_UDP:
                break;
            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        switch (socket_option_name)
        {
            // Retrieves and clears pending socket error
            case SOCK_SOCKO_ERROR:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
            case SOCK_SOCKO_KEEPALIVE:
                // Check if keep alive is built into the NetX Duo library
                if (socket_entry->type == SOCKET_TYPE_TCP)
                // Has keep alive been built int
                {
#ifdef NX_ENABLE_TCP_KEEPALIVE
                    bool keep_alive_result = socket_entry->tcp_socket->nx_tcp_socket_keepalive_enabled;
#elseif
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
#endif
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
                }
                break;
            case SOCK_SOCKO_RECEIVETIMEOUT:
                socket_entry->tcp_socket->nx_tcp_socket_timeout;
                socket_entry->udp_socket->nx_udp_socket_time_to_live;
                break;
            // Useful for servers that restart frequently or bind to wildcard addresses.
            case SOCK_SOCKO_REUSEADDRESS:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
            case SOCK_SOCKO_LINGER:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
            case SOCK_IPO_MULTICAST_IF:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Sockets_NativeSocket::setsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        UINT socket_option_level;
        UINT socket_option_name;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        switch (socket_option_level)
        {
            // Indicates socket options apply to the socket itself
            case SOCK_SOL_SOCKET:
                break;
            // Apply to sockets created for the IPv4 address family (AF_INET)
            case SOCK_IPPROTO_IP:
                break;
            // Apply to sockets created for the IPv6 address family (AF_INET6)
            case SOCK_IPPROTO_IPV6:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
                break;
                // Apply to sockets created for the IPv4 and IPv6 address families (AF_INET and AF_INET6)
                // with the protocol parameter to the socket function specified as TCP (IPPROTO_TCP)
            case SOCK_IPPROTO_TCP:
                break;
            // apply to sockets created for the IPv4 and IPv6 address families (AF_INET and AF_INET6)
            //  with the protocol parameter to the socket function specified as UDP (IPPROTO_UDP)
            case SOCK_IPPROTO_UDP:
                break;
            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        switch (socket_option_name)
        {
            // Retrieves and clears pending socket error
            case SOCK_SOCKO_ERROR:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
            case SOCK_SOCKO_KEEPALIVE:
                // Check if keep alive is built into the NetX Duo library
                if (socket_entry->type == SOCKET_TYPE_TCP)
                // Has keep alive been built int
                {
#ifdef NX_ENABLE_TCP_KEEPALIVE
                    bool keep_alive_result = socket_entry->tcp_socket->nx_tcp_socket_keepalive_enabled;
#elseif
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
#endif
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
                }
                break;
            case SOCK_SOCKO_RECEIVETIMEOUT:
                socket_entry->tcp_socket->nx_tcp_socket_timeout;
                socket_entry->udp_socket->nx_udp_socket_time_to_live;
                break;
            // Useful for servers that restart frequently or bind to wildcard addresses.
            case SOCK_SOCKO_REUSEADDRESS:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
            case SOCK_SOCKO_LINGER:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
            case SOCK_IPO_MULTICAST_IF:
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::poll___STATIC__BOOLEAN__OBJECT__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        SOCK_sockaddr addr;
        CLR_UINT32 addrLen = sizeof(addr);
        bool fThrowOnWouldBlock = false;
        ULONG server_ip = 0;

        NX_TCP_SOCKET *socket_ptr;
        ULONG *tcp_packets_sent;
        ULONG *tcp_bytes_sent;
        ULONG *tcp_packets_received;
        ULONG *tcp_bytes_received;
        ULONG *tcp_retransmit_packets;
        ULONG *tcp_packets_queued;
        ULONG *tcp_checksum_errors;
        ULONG *tcp_socket_state;
        ULONG *tcp_transmit_queue_depth;
        ULONG *tcp_transmit_window;
        ULONG *tcp_receive_window;

        ULONG *udp_packets_sent;
        ULONG *udp_bytes_sent;
        ULONG *udp_packets_received;
        ULONG *udp_bytes_received;
        ULONG *udp_retransmit_packets;
        ULONG *udp_packets_queued;
        ULONG *udp_receive_packets_dropped;
        ULONG *udp_checksum_errors;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        NANOCLR_CHECK_HRESULT(MarshalSockAddress(&addr, addrLen, stack.Arg1()));
        fThrowOnWouldBlock = (stack.Arg2().NumericByRefConst().s4 != 0);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                status = nx_tcp_socket_info_get(
                    socket_entry->tcp_socket,
                    tcp_packets_sent,
                    tcp_bytes_sent,
                    tcp_packets_received,
                    tcp_bytes_received,
                    tcp_retransmit_packets,
                    tcp_packets_queued,
                    tcp_checksum_errors,
                    tcp_socket_state,
                    tcp_transmit_queue_depth,
                    tcp_transmit_window,
                    tcp_receive_window);
                break;
            case SOCKET_TYPE_UDP:
                status = nx_udp_socket_info_get(
                    socket_entry->udp_socket,
                    udp_packets_sent,
                    udp_bytes_sent,
                    udp_packets_received,
                    udp_bytes_received,
                    udp_packets_queued,
                    udp_receive_packets_dropped,
                    udp_checksum_errors);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::ioctl___STATIC__VOID__OBJECT__U4__BYREF_U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_INT32 status;
        CLR_INT32 returnStatus = SOCK_SUCCESS;
        SOCK_sockaddr addr;
        CLR_UINT32 addrLen = sizeof(addr);
        bool fThrowOnWouldBlock = false;
        ULONG server_ip = 0;

        NX_TCP_SOCKET *socket_ptr;
        ULONG *tcp_packets_sent;
        ULONG *tcp_bytes_sent;
        ULONG *tcp_packets_received;
        ULONG *tcp_bytes_received;
        ULONG *tcp_retransmit_packets;
        ULONG *tcp_packets_queued;
        ULONG *tcp_checksum_errors;
        ULONG *tcp_socket_state;
        ULONG *tcp_transmit_queue_depth;
        ULONG *tcp_transmit_window;
        ULONG *tcp_receive_window;

        ULONG *udp_packets_sent;
        ULONG *udp_bytes_sent;
        ULONG *udp_packets_received;
        ULONG *udp_bytes_received;
        ULONG *udp_retransmit_packets;
        ULONG *udp_packets_queued;
        ULONG *udp_receive_packets_dropped;
        ULONG *udp_checksum_errors;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        NANOCLR_CHECK_HRESULT(MarshalSockAddress(&addr, addrLen, stack.Arg1()));
        fThrowOnWouldBlock = (stack.Arg2().NumericByRefConst().s4 != 0);

        switch (socket_entry->type)
        {
            case SOCKET_TYPE_TCP:
                status = nx_tcp_socket_info_get(
                    socket_entry->tcp_socket,
                    tcp_packets_sent,
                    tcp_bytes_sent,
                    tcp_packets_received,
                    tcp_bytes_received,
                    tcp_retransmit_packets,
                    tcp_packets_queued,
                    tcp_checksum_errors,
                    tcp_socket_state,
                    tcp_transmit_queue_depth,
                    tcp_transmit_window,
                    tcp_receive_window);
                break;
            case SOCKET_TYPE_UDP:
                status = nx_udp_socket_info_get(
                    socket_entry->udp_socket,
                    udp_packets_sent,
                    udp_bytes_sent,
                    udp_packets_received,
                    udp_bytes_received,
                    udp_packets_queued,
                    udp_receive_packets_dropped,
                    udp_checksum_errors);
                break;
            default:
                break;
        }
        if (status != NX_SUCCESS)
        {
            returnStatus = TranslateNXErrorToSocketError(stack, status);
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        SetReturnStatus(stack, returnStatus);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::MarshalSockAddress(
    struct SOCK_sockaddr *addrDst,
    CLR_UINT32 &addrLenDst,
    const CLR_RT_HeapBlock &blkEndPointAddress)
{

    (void)addrDst;
    (void)addrLenDst;
    (void)blkEndPointAddress;

    return 0;
}

CLR_INT32 Library_sys_net_native_System_Net_Sockets_NativeSocket::Helper__SelectSocket(CLR_INT32 handle, CLR_INT32 mode)
{
    (void)handle;
    (void)mode;
    return 0;
}
