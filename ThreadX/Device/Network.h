#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoCLR_Types.h"
#include "sys_net_native.h"
#include "nanoPAL_Sockets.h"
#include <nx_secure_tls.h>

#define NETWORK_INFINITE_TIMEOUT ((CLR_INT64) - 1)

#define INTERFACE_WIFI     0
#define INTERFACE_ETHERNET 1

#define AF_UNSPEC 0 // Unspecified.
#define AF_NS     1 // Local to host (pipes, portals).
#define AF_INET   2 // IPv4 socket (UDP, TCP, etc)
#define AF_INET6  3 // IPv6 socket (UDP, TCP, etc)
#define AF_PACKET 4 // Raw Packet type (Link Layer packets)

#define EVT_WIFI_RX        (1u << 0)
#define EVT_WIFI_LINK_UP   (1u << 1)
#define EVT_WIFI_LINK_DOWN (1u << 2)


#define FAULT_IF_NX_CALL_UNSUCCESSFUL(status, stack)                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((status) != NX_SUCCESS)                                                                                    \
        {                                                                                                              \
            SetReturnStatus((stack), SOCK_EINVAL, g_CLR_RT_WellKnownTypes.m_NetworkInterface);                         \
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);                                                                         \
        }                                                                                                              \
    } while (0)

#define NX_DRIVER_PHYSICAL_HEADER_REMOVE(p)                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        p->nx_packet_prepend_ptr += NX_DRIVER_PHYSICAL_FRAME_SIZE;                                                     \
        p->nx_packet_length -= NX_DRIVER_PHYSICAL_FRAME_SIZE;                                                          \
    } while (0)

// Shortcut name
typedef Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate X509Certificate;
typedef Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2 X509Certificate2;

enum socket_type
{
    SOCKET_TYPE_TCP,
    SOCKET_TYPE_UDP
};
struct socket_entry_t
{
    socket_type type;
    union {
        NX_TCP_SOCKET *tcp_socket;
        NX_UDP_SOCKET *udp_socket;
    };
    NX_SECURE_TLS_SESSION TLSSession;
};

#define SOCK_SUCCESS 0 // Successful operation
#define SOCK_EPERM  1  // Operation not permitted
#define SOCK_ENOENT 2  // No such file or directory
#define SOCK_EIO    5  // I/O error
#define SOCK_EAGAIN 11 // Try again
#define SOCK_ENOMEM 12 // Out of memory
#define SOCK_EEXIST 17 // File exists
#define SOCK_ENODEV 19 // No such device
#define SOCK_ENOSYS 38 // Function not implemented
#define EOPNOTSUPP  95 // Operation not supported on transport endpoint

#define Rdm       4
#define Seqpacket 5

int TranslateNXStatusToBSDStatus(int NetXDuoError);
void SetReturnStatus(CLR_RT_StackFrame &stack, int errorCode, CLR_RT_TypeDef_Index messageType);
void tcp_data_callback(NX_TCP_SOCKET *socket_ptr);
void tcp_server_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port);
