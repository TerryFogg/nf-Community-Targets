//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Network.h"

static bool socket_data_available = false;

void SetReturnStatus(CLR_RT_StackFrame &stack, int errorCode, CLR_RT_TypeDef_Index messageType)
{
    CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;
    if ((Library_corlib_native_System_Exception::CreateInstance(res, messageType, CLR_E_FAIL, &stack)) == S_OK)
    {
        res.Dereference()[Library_sys_net_native_System_Net_Sockets_SocketException::FIELD___errorCode].SetInteger(
            errorCode);
    }
}
int TranslateNXStatusToBSDStatus(int NetXDuoError)
{
    int translated_socket_error;
    switch (NetXDuoError)
    {
        case NX_NO_PACKET:
            translated_socket_error = SOCK_EMFILE;
            break;
        case NX_UNDERFLOW:
            translated_socket_error = SOCK_ENOBUFS;
            break;
        case NX_OVERFLOW:
            translated_socket_error = SOCK_ENOBUFS;
            break;
        case NX_NO_MAPPING:
            translated_socket_error = SOCK_EADDRNOTAVAIL;
        case NX_DELETED:
            translated_socket_error = SOCK_EBADF;
            break;
        case NX_POOL_ERROR:
            translated_socket_error = SOCK_ENOMEM;
            break;
        case NX_PTR_ERROR:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_WAIT_ERROR:
            translated_socket_error = SOCK_EAGAIN;
            break;
        case NX_SIZE_ERROR:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_OPTION_ERROR:
            translated_socket_error = SOCK_ENOPROTOOPT;
            break;
        case NX_DELETE_ERROR:
            translated_socket_error = SOCK_EBADF;
            break;
        case NX_CALLER_ERROR:
            translated_socket_error = SOCK_EPERM;
            break;
        case NX_INVALID_PACKET:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_INVALID_SOCKET:
            translated_socket_error = SOCK_EBADF;
            break;
        case NX_NOT_ENABLED:
            translated_socket_error = SOCK_EPROTONOSUPPORT;
            break;
        case NX_ALREADY_ENABLED:
            translated_socket_error = SOCK_EALREADY;
            break;
        case NX_NO_MORE_ENTRIES:
            translated_socket_error = SOCK_ENOENT;
            break;
        case NX_ARP_TIMER_ERROR:
            translated_socket_error = SOCK_EIO;
            break;
        case NX_ENTRY_NOT_FOUND:
            translated_socket_error = SOCK_ENOENT;
            break;
        case NX_WAIT_ABORTED:
            translated_socket_error = SOCK_EWOULDBLOCK;
            break;
        case NX_IP_INTERNAL_ERROR:
            translated_socket_error = SOCK_EIO;
            break;
        case NX_IP_ADDRESS_ERROR:
            translated_socket_error = SOCK_EDESTADDRREQ;
            break;
        case NX_ALREADY_BOUND:
            translated_socket_error = SOCK_EADDRINUSE;
            break;
        case NX_PORT_UNAVAILABLE:
            translated_socket_error = SOCK_EADDRNOTAVAIL;
            break;
        case NX_NOT_BOUND:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_SOCKET_UNBOUND:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_NOT_CREATED:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_SOCKETS_BOUND:
            translated_socket_error = SOCK_EADDRINUSE;
            break;
        case NX_NO_RESPONSE:
            translated_socket_error = SOCK_ETIMEDOUT;
            break;
        case NX_POOL_DELETED:
            translated_socket_error = SOCK_ENOMEM;
            break;
        case NX_ALREADY_RELEASED:
            translated_socket_error = SOCK_EBADF;
            break;
        case NX_MAX_LISTEN:
            translated_socket_error = SOCK_ENOBUFS;
            break;
        case NX_DUPLICATE_LISTEN:
            translated_socket_error = SOCK_EADDRINUSE;
            break;
        case NX_NOT_CLOSED:
            translated_socket_error = SOCK_EALREADY;
            break;
        case NX_NOT_LISTEN_STATE:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_IN_PROGRESS:
            translated_socket_error = SOCK_EINPROGRESS;
            break;
        case NX_NOT_CONNECTED:
            translated_socket_error = SOCK_ENOTCONN;
            break;
        case NX_WINDOW_OVERFLOW:
            translated_socket_error = SOCK_EWOULDBLOCK;
            break;
        case NX_ALREADY_SUSPENDED:
            translated_socket_error = SOCK_EALREADY;
            break;
        case NX_DISCONNECT_FAILED:
            translated_socket_error = SOCK_ENOTCONN;
            break;
        case NX_STILL_BOUND:
            translated_socket_error = SOCK_EADDRINUSE;
            break;
        case NX_UNHANDLED_COMMAND:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_NOT_SUCCESSFUL:
            translated_socket_error = SOCK_EIO;
            break;
        case NX_NO_FREE_PORTS:
            translated_socket_error = SOCK_EADDRNOTAVAIL;
            break;
        case NX_INVALID_PORT:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_INVALID_RELISTEN:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_CONNECTION_PENDING:
            translated_socket_error = SOCK_EALREADY;
            break;
        case NX_TX_QUEUE_DEPTH:
            translated_socket_error = SOCK_EWOULDBLOCK;
            break;
        case NX_INVALID_INTERFACE:
            translated_socket_error = SOCK_ENODEV;
            break;
        case NX_INVALID_PARAMETERS:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_NOT_FOUND:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_CANNOT_START:
            translated_socket_error = SOCK_ENODEV;
            break;
        case NX_NO_INTERFACE_ADDRESS:
            translated_socket_error = SOCK_EADDRNOTAVAIL;
            break;
        case NX_INVALID_MTU_DATA:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_DUPLICATED_ENTRY:
            translated_socket_error = SOCK_EEXIST;
            break;
        case NX_PACKET_OFFSET_ERROR:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_OPTION_HEADER_ERROR:
            translated_socket_error = SOCK_EINVAL;
            break;
        case NX_TCPIP_OFFLOAD_ERROR:
            translated_socket_error = SOCK_EIO;
            break;
        case NX_NOT_IMPLEMENTED:
            translated_socket_error = SOCK_ENOSYS;
            break;
        case NX_NOT_SUPPORTED:
            translated_socket_error = SOCK_EOPNOTSUPP;
            break;
        default:
            translated_socket_error = SOCK_EPERM;
    }
    return translated_socket_error;
}
void tcp_data_callback(NX_TCP_SOCKET *socket_ptr)
{
    socket_data_available = true;
}
void tcp_server_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port)
{
    socket_data_available = true;
}
