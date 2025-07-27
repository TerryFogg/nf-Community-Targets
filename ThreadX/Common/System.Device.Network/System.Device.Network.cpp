//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoHAL_ConfigurationManager.h"
#include "sys_net_native.h"
#include "System.Device.Network.h"
#include <System.Device.Wifi.h>
#include "ManagedThreadSupport.h"

#define NX_NAT_LOCAL_IPADR (IP_ADDRESS(192, 168, 2, 1))

// Stored configuration
//   1 wireless interface
//   1 ethernet interface
static HAL_Configuration_NetworkInterface wirelessInterface;
static HAL_Configuration_NetworkInterface ethernetInterface;
static HAL_Configuration_Wireless80211 wifiConfig;
static HAL_Configuration_WirelessAP wifiAP;

static NX_IP ip_ptr;

static bool socket_data_available = false;

NX_IP *ip_ptr;
NX_DHCP *dhcp_ptr;
NX_TCP_SOCKET *socket_ptr;
NX_SECURE_TLS_SESSION tls_session;
NX_PACKET *send_packet;
NX_PACKET *receive_packet;
static NX_SECURE_X509_CERT certificate;

#pragma region Local Functions

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

HRESULT GetSocketEntry(CLR_RT_StackFrame &stack, socket_entry_t *socket_entry)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *socket_info = stack.Arg0().Dereference();
        if (socket_info != NULL)
        {
            socket_entry =
                (socket_entry_t *)socket_info[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                    .NumericByRef()
                    .s4;
        }
        if ((int)socket_entry == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }
        NANOCLR_NOCLEANUP();
    }
}
int TranslateNXErrorToSocketError(CLR_RT_StackFrame &stack, int error)
{
    int translated_socket_error;
    switch (error)
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
void SetReturnStatus(CLR_RT_StackFrame &stack, CLR_INT32 errorCode)
{
    NATIVE_PROFILE_CLR_NETWORK();
    CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

    if ((Library_corlib_native_System_Exception::CreateInstance(
            res,
            g_CLR_RT_WellKnownTypes.m_SocketException,
            CLR_E_FAIL,
            &stack)) == S_OK)
    {
        res.Dereference()[Library_sys_net_native_System_Net_Sockets_SocketException::FIELD___errorCode].SetInteger(
            errorCode);
    }
}
void tcp_data_callback(NX_TCP_SOCKET *socket_ptr)
{
    socket_data_available = true;
}
void tcp_server_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port)
{
    socket_data_available = true;
}

#pragma endregion

#pragma region NetworkInterface
HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::
    InitializeNetworkInterfaceSettings___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        HAL_Configuration_NetworkInterface config;
        CLR_RT_HeapBlock *pConfig = stack.Arg0().Dereference();
        FAULT_ON_NULL(pConfig);

        CLR_UINT32 interfaceIndex = pConfig[FIELD___interfaceIndex].NumericByRefConst().u4;
        HAL_Configuration_NetworkInterface *pLocalConfig;

        if (!ConfigurationManager_GetConfigurationBlock(
                (void *)&config,
                DeviceConfigurationOption_Network,
                interfaceIndex))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
        _ASSERTE(config.StartupAddressMode > 0);

        if (config.StartupAddressMode == AddressMode_DHCP)
        {
            uint32_t ip_address;
            uint32_t network_mask;
            if (nx_ip_interface_address_get(ip_ptr, 0, &ip_address, &network_mask) != NX_SUCCESS)
            {
                return S_FALSE;
            }
            config.IPv4Address = ip_address;
            config.IPv4NetMask = network_mask;
            uint32_t gwAddress;
            if (nx_ip_gateway_address_get(ip_ptr, &gwAddress) != NX_SUCCESS)
            {
                return S_FALSE;
            }
            config.IPv4GatewayAddress = ip_address;
            UCHAR dnsIpString[4];
            UINT dnsSize;
            if (nx_dhcp_interface_user_option_retrieve(dhcp_ptr, 0, NX_DHCP_OPTION_DNS_SVR, dnsIpString, &dnsSize) !=
                NX_SUCCESS)
            {
                return S_FALSE;
            }
            config.IPv4DNSAddress1 = nx_dhcp_user_option_convert(dnsIpString);
            config.IPv4DNSAddress2 = 0;
        }

        // Setup the config with some defaults
        pConfig[FIELD___ipv4Address].SetInteger((CLR_UINT32)pLocalConfig->IPv4Address);
        pConfig[FIELD___ipv4GatewayAddress].SetInteger((CLR_UINT32)pLocalConfig->IPv4GatewayAddress);
        pConfig[FIELD___ipv4NetMask].SetInteger((CLR_UINT32)pLocalConfig->IPv4NetMask);
        pConfig[FIELD___ipv4dnsAddress1].SetInteger((CLR_UINT32)pLocalConfig->IPv4DNSAddress1);
        pConfig[FIELD___ipv4dnsAddress2].SetInteger((CLR_UINT32)pLocalConfig->IPv4DNSAddress2);

        // pConfig[FIELD___ipv6Address].SetInteger((CLR_UINT32)pLocalConfig->IPv6Address);
        // pConfig[FIELD___ipv6GatewayAddress].SetInteger((CLR_UINT32)pLocalConfig->IPv6GatewayAddress);
        // pConfig[FIELD___ipv6NetMask].SetInteger((CLR_UINT32)pLocalConfig->IPv6NetMask);
        // pConfig[FIELD___ipv6dnsAddress1].SetInteger((CLR_UINT32)pLocalConfig->IPv6DNSAddress1);
        // pConfig[FIELD___ipv6dnsAddress2].SetInteger((CLR_UINT32)pLocalConfig->IPv6DNSAddress2);

        pConfig[FIELD___automaticDns].SetInteger((CLR_UINT32)pLocalConfig->AutomaticDNS);
        pConfig[FIELD___networkInterfaceType].SetInteger((CLR_UINT32)pLocalConfig->InterfaceType);
        pConfig[FIELD___specificConfigId].SetInteger((CLR_UINT32)pLocalConfig->SpecificConfigId);
        pConfig[FIELD___startupAddressMode].SetInteger((CLR_UINT32)pLocalConfig->StartupAddressMode);

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
            pConfig[FIELD___macAddress],
            NETIF_MAX_HWADDR_LEN,
            g_CLR_RT_WellKnownTypes.m_UInt8));
        memcpy(
            pConfig[FIELD___macAddress].DereferenceArray()->GetFirstElement(),
            pLocalConfig->MacAddress,
            NETIF_MAX_HWADDR_LEN);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::UpdateConfiguration___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        HAL_Configuration_NetworkInterface *pLocalConfig;
        UpdateConfigurationResult updateResult = UpdateConfigurationResult_Failed;
        CLR_RT_HeapBlock *pConfig = stack.Arg0().Dereference();
        FAULT_ON_NULL(pConfig);

        CLR_UINT32 interfaceIndex = pConfig[FIELD___interfaceIndex].NumericByRefConst().u4;
        CLR_UINT32 updateFlags = stack.Arg1().NumericByRef().u4;
        CLR_RT_HeapBlock_Array *pMACAddress = pConfig[FIELD___macAddress].DereferenceArray();

        switch (interfaceIndex)
        {
            case INTERFACE_WIFI:
            {
                pLocalConfig = &wirelessInterface;
            }
            break;
            case INTERFACE_ETHERNET:
            {
                pLocalConfig = &ethernetInterface;
            }
            break;
        }

        if (pMACAddress != NULL)
        {
            memcpy(&pLocalConfig->MacAddress, pMACAddress->GetFirstElement(), NETIF_MAX_HWADDR_LEN);
        }

        pLocalConfig->IPv4Address = pConfig[FIELD___ipv4Address].NumericByRef().u4;
        pLocalConfig->IPv4NetMask = pConfig[FIELD___ipv4NetMask].NumericByRef().u4;
        pLocalConfig->IPv4GatewayAddress = pConfig[FIELD___ipv4GatewayAddress].NumericByRef().u4;
        pLocalConfig->IPv4DNSAddress1 = pConfig[FIELD___ipv4dnsAddress1].NumericByRef().u4;
        pLocalConfig->IPv4DNSAddress2 = pConfig[FIELD___ipv4dnsAddress2].NumericByRef().u4;

        pLocalConfig->IPv6Address[0] = (uint8_t)(pConfig[FIELD___ipv6Address].NumericByRef().u4 & 0xFF);
        pLocalConfig->IPv6Address[1] = (uint8_t)((pConfig[FIELD___ipv6Address].NumericByRef().u4 >> 8) & 0xFF);
        pLocalConfig->IPv6Address[2] = (uint8_t)((pConfig[FIELD___ipv6Address].NumericByRef().u4 >> 16) & 0xFF);
        pLocalConfig->IPv6Address[3] = (uint8_t)((pConfig[FIELD___ipv6Address].NumericByRef().u4 >> 24) & 0xFF);

        pLocalConfig->IPv6GatewayAddress[0] = (uint8_t)(pConfig[FIELD___ipv6GatewayAddress].NumericByRef().u4 & 0xFF);
        pLocalConfig->IPv6GatewayAddress[1] =
            (uint8_t)((pConfig[FIELD___ipv6GatewayAddress].NumericByRef().u4 >> 8) & 0xFF);
        pLocalConfig->IPv6GatewayAddress[2] =
            (uint8_t)((pConfig[FIELD___ipv6GatewayAddress].NumericByRef().u4 >> 16) & 0xFF);
        pLocalConfig->IPv6GatewayAddress[3] =
            (uint8_t)((pConfig[FIELD___ipv6GatewayAddress].NumericByRef().u4 >> 24) & 0xFF);

        pLocalConfig->IPv6GatewayAddress[0] = (uint8_t)(pConfig[FIELD___ipv6NetMask].NumericByRef().u4 & 0xFF);
        pLocalConfig->IPv6GatewayAddress[1] = (uint8_t)((pConfig[FIELD___ipv6NetMask].NumericByRef().u4 >> 8) & 0xFF);
        pLocalConfig->IPv6GatewayAddress[2] = (uint8_t)((pConfig[FIELD___ipv6NetMask].NumericByRef().u4 >> 16) & 0xFF);
        pLocalConfig->IPv6GatewayAddress[3] = (uint8_t)((pConfig[FIELD___ipv6NetMask].NumericByRef().u4 >> 24) & 0xFF);

        pLocalConfig->IPv6GatewayAddress[0] = (uint8_t)(pConfig[FIELD___ipv6dnsAddress1].NumericByRef().u4 & 0xFF);
        pLocalConfig->IPv6GatewayAddress[1] =
            (uint8_t)((pConfig[FIELD___ipv6dnsAddress1].NumericByRef().u4 >> 8) & 0xFF);
        pLocalConfig->IPv6GatewayAddress[2] =
            (uint8_t)((pConfig[FIELD___ipv6dnsAddress1].NumericByRef().u4 >> 16) & 0xFF);
        pLocalConfig->IPv6GatewayAddress[3] =
            (uint8_t)((pConfig[FIELD___ipv6dnsAddress1].NumericByRef().u4 >> 24) & 0xFF);

        pLocalConfig->IPv6GatewayAddress[0] = (uint8_t)(pConfig[FIELD___ipv6dnsAddress2].NumericByRef().u4 & 0xFF);
        pLocalConfig->IPv6GatewayAddress[1] =
            (uint8_t)((pConfig[FIELD___ipv6dnsAddress2].NumericByRef().u4 >> 8) & 0xFF);
        pLocalConfig->IPv6GatewayAddress[2] =
            (uint8_t)((pConfig[FIELD___ipv6dnsAddress2].NumericByRef().u4 >> 16) & 0xFF);
        pLocalConfig->IPv6GatewayAddress[3] =
            (uint8_t)((pConfig[FIELD___ipv6dnsAddress2].NumericByRef().u4 >> 24) & 0xFF);

        pLocalConfig->AutomaticDNS = pConfig[FIELD___automaticDns].NumericByRef().u4;
        pLocalConfig->InterfaceType = (NetworkInterfaceType)pConfig[FIELD___networkInterfaceType].NumericByRef().u4;
        pLocalConfig->SpecificConfigId = (CLR_UINT32)pConfig[FIELD___specificConfigId].NumericByRef().u4;
        pLocalConfig->StartupAddressMode = (AddressMode)pConfig[FIELD___startupAddressMode].NumericByRef().u4;
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::GetIsNetworkAvailable___STATIC__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        bool wifiIsAvailable = false;
        bool ethernetIsAvailable = false;
        bool networkIsAvailable = false;

// Check if wireless or ethernet network is available
#ifdef WIFI
        wifiIsAvailable = DeviceWifi::WifiUp();
#endif

#ifdef ETHERNET
        ethernetIsAvailable = DeviceEthernet::WifiUp();
#endif
        networkIsAvailable = (wifiIsAvailable || ethernetIsAvailable);
        stack.SetResult_Boolean(networkIsAvailable);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::GetNetworkInterfaceCount___STATIC__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int networkInterfaceCount = 0;
#ifdef WIFI
        networkInterfaceCount++;
#endif

#ifdef ETHERNET
        networkInterfaceCount++;
#endif
        stack.SetResult_I4(networkInterfaceCount);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::
    GetNetworkInterface___STATIC__SystemNetNetworkInformationNetworkInterface__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        {
            CLR_RT_HeapBlock *pConfig = stack.Arg0().Dereference();
            FAULT_ON_NULL(pConfig);
            CLR_UINT32 interfaceIndex = pConfig[FIELD___interfaceIndex].NumericByRefConst().u4;
            CLR_RT_HeapBlock &top = stack.PushValueAndClear();
            HAL_Configuration_NetworkInterface *pLocalConfig;

            switch (interfaceIndex)
            {
                case INTERFACE_WIFI:
                {
                    pLocalConfig = &wirelessInterface;
                }
                break;
                case INTERFACE_ETHERNET:
                {
                    pLocalConfig = &ethernetInterface;
                }
                break;
            }

            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_NetworkInterface));
            pConfig = top.Dereference();
            FAULT_ON_NULL(pConfig);

            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                pConfig[FIELD___macAddress],
                NETIF_MAX_HWADDR_LEN,
                g_CLR_RT_WellKnownTypes.m_UInt8));
            memcpy(
                pConfig[FIELD___macAddress].DereferenceArray()->GetFirstElement(),
                pLocalConfig->MacAddress,
                NETIF_MAX_HWADDR_LEN);

            pConfig[FIELD___ipv4Address].SetInteger((CLR_UINT32)pLocalConfig->IPv4Address);
            pConfig[FIELD___ipv4NetMask].SetInteger((CLR_UINT32)pLocalConfig->IPv4NetMask);
            pConfig[FIELD___ipv4GatewayAddress].SetInteger((CLR_UINT32)pLocalConfig->IPv4GatewayAddress);
            pConfig[FIELD___ipv4dnsAddress1].SetInteger((CLR_UINT32)pLocalConfig->IPv4DNSAddress1);
            pConfig[FIELD___ipv4dnsAddress2].SetInteger((CLR_UINT32)pLocalConfig->IPv4DNSAddress2);
            pConfig[FIELD___ipv6Address].SetInteger((CLR_UINT32)pLocalConfig->IPv6Address);
            pConfig[FIELD___ipv6NetMask].SetInteger((CLR_UINT32)pLocalConfig->IPv6NetMask);
            pConfig[FIELD___ipv6GatewayAddress].SetInteger((CLR_UINT32)pLocalConfig->IPv6GatewayAddress);
            pConfig[FIELD___ipv6dnsAddress1].SetInteger((CLR_UINT32)pLocalConfig->IPv6DNSAddress1);
            pConfig[FIELD___ipv6dnsAddress2].SetInteger((CLR_UINT32)pLocalConfig->IPv6DNSAddress2);
            pConfig[FIELD___networkInterfaceType].SetInteger((CLR_UINT32)pLocalConfig->InterfaceType);
            pConfig[FIELD___specificConfigId].SetInteger((CLR_UINT32)pLocalConfig->SpecificConfigId);
            pConfig[FIELD___automaticDns].SetInteger((CLR_UINT32)pLocalConfig->AutomaticDNS);
            pConfig[FIELD___startupAddressMode].SetInteger((CLR_UINT32)pLocalConfig->StartupAddressMode);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::
    IPV4AddressFromString___STATIC__I8__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // IPv4 max length is 15 characters
        LPCSTR ip_string = stack.Arg0().RecoverString();
        CLR_UINT64 ip_address = 0;
        int octets[4];

        // Parse the string into four octets
        if (sscanf(ip_string, "%d.%d.%d.%d", &octets[0], &octets[1], &octets[2], &octets[3]) == 4)
        {
            // Convert to ULONG format used by NetX Duo
            ip_address = ((CLR_UINT64)octets[0] << 24) | ((CLR_UINT64)octets[1] << 16) | ((CLR_UINT64)octets[2] << 8) |
                         (CLR_UINT64)octets[3];
        }
        stack.PushValue().SetInteger((CLR_UINT64)ip_address);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::
    IPV6AddressFromString___STATIC__SZARRAY_U2__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        LPCSTR ipString = stack.Arg0().RecoverString();
        uint16_t address[8];
        char *token;
        int index = 0;
        int emptyIndex = -1;

        while ((token = strtok((char *)ipString, ":")) != NULL)
        {
            ipString = NULL; // Use NULL for subsequent calls
            if (*token == '\0')
            {
                // "::" shorthand detected
                if (emptyIndex == -1)
                {
                    emptyIndex = index; // Store position of "::"
                }
                continue;
            }
            // Convert hex segment to 16-bit integer
            uint16_t value = (uint16_t)strtol(token, NULL, 16);
            address[index * 2] = value >> 8;       // High byte
            address[index * 2 + 1] = value & 0xFF; // Low byte
            index++;
        }
        // Handle "::" compression
        if (emptyIndex != -1)
        {
            // Number of zero-filled groups
            int shift = 8 - index;
            memmove(address + (emptyIndex + shift) * 2, address + emptyIndex * 2, (index - emptyIndex) * 2);
            memset(address + emptyIndex * 2, 0, shift * 2);
        }
        // Return array of uint16[8]
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(stack.PushValueAndClear(), 8, g_CLR_RT_WellKnownTypes.m_UInt16));
        {
            uint16_t *p = (uint16_t *)stack.TopValue().DereferenceArray()->GetFirstElement();
            memcpy(p, address, sizeof(address));
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_IPAddress::IPv4ToString___STATIC__STRING__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // Get address of CLR_UINT16 array with ipv4 address
        CLR_UINT16 *ipv4_address = (CLR_UINT16 *)stack.Arg0().DereferenceArray()->GetFirstElement();
        char ipv4_string[16];
        sprintf(ipv4_string, "%u.%u.%u.%u", ipv4_address[0], ipv4_address[1], ipv4_address[2], ipv4_address[3]);

        NANOCLR_CHECK_HRESULT(stack.SetResult_String(ipv4_string));
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_IPAddress::IPv6ToString___STATIC__STRING__SZARRAY_U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // Get address of CLR_UINT16 array with ipv6 address
        CLR_UINT16 *ipv6_address = (CLR_UINT16 *)stack.Arg0().DereferenceArray()->GetFirstElement();
        char ipv6_string[40];
        sprintf(
            ipv6_string,
            "%x:%x:%x:%x:%x:%x:%x:%x",
            (ipv6_address[0] << 8) | ipv6_address[1],
            (ipv6_address[2] << 8) | ipv6_address[3],
            (ipv6_address[4] << 8) | ipv6_address[5],
            (ipv6_address[6] << 8) | ipv6_address[7],
            (ipv6_address[8] << 8) | ipv6_address[9],
            (ipv6_address[10] << 8) | ipv6_address[11],
            (ipv6_address[12] << 8) | ipv6_address[13],
            (ipv6_address[14] << 8) | ipv6_address[15]);

        NANOCLR_CHECK_HRESULT(stack.SetResult_String(ipv6_string));
    }
    NANOCLR_NOCLEANUP();
}
#pragma endregion

#pragma region NetworkInformation
HRESULT Library_sys_net_native_System_Net_NetworkInformation_IPGlobalProperties::
    GetIPAddress___STATIC__SystemNetIPAddress(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        HAL_Configuration_NetworkInterface config;

        CLR_RT_TypeDef_Index ipAddressTypeDef;
        CLR_RT_HeapBlock *ipAddressHbObj;
        CLR_RT_HeapBlock ipAddress;
        CLR_INT64 *pIPAddressRef;
        bool ipAddressValid = false;

        CLR_RT_HeapBlock &top = stack.PushValue();
        g_CLR_RT_TypeSystem.FindTypeDef("IPAddress", "System.Net", ipAddressTypeDef);
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(ipAddress, ipAddressTypeDef));
        ipAddressHbObj = ipAddress.Dereference();

        CLR_RT_HeapBlock &addressFieldRef = ipAddressHbObj[Library_sys_net_native_System_Net_IPAddress::FIELD__Address];
        pIPAddressRef = (CLR_INT64 *)&addressFieldRef.NumericByRef().s8;
        ipAddressHbObj[Library_sys_net_native_System_Net_IPAddress::FIELD___family].NumericByRef().s4 = SOCK_AF_INET;

#ifdef ACTIVE_INTERFACE_WIFI
        *pIPAddressRef = wirelessInterface.IPv4Address;
#elif ACTIVE_INTERFACE_ETHERNET
        *pIPAddressRef = ethernetInterface.IPv4Address;
#endif

        bool validIPAddress = (*pIPAddressRef > 0);
        top.SetObjectReference(ipAddressHbObj);
        if (validIPAddress)
        {
            // set address field with IPAddress heap block object
            top.SetObjectReference(ipAddressHbObj);
        }
        else
        {
            // default to IP Any Address
            top.SetObjectReference(g_CLR_RT_TypeSystem.m_assemblies[ipAddressTypeDef.Assembly() - 1]
                                       ->GetStaticField(Library_sys_net_native_System_Net_IPAddress::FIELD_STATIC__Any)
                                       ->Dereference());
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::
    GetWireless82011ConfigurationCount___STATIC__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int wifiCount = 0;
#ifdef WIFI
        wifiCount++;
#endif
        stack.SetResult_I4(wifiCount);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::
    GetWireless82011Configuration___STATIC__SystemNetNetworkInformationWireless80211Configuration__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        HAL_Configuration_Wireless80211 config;
        CLR_RT_HeapBlock *pConfig;
        CLR_UINT32 configurationIndex = stack.Arg0().NumericByRef().u4;
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        NANOCLR_CLEAR(config);

        // load wireless 802.11 configuration from the storage
        if (!ConfigurationManager_GetConfigurationBlock(
                (void *)&config,
                DeviceConfigurationOption_Wireless80211Network,
                configurationIndex))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // create new object for configuration
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_Wireless80211Configuration));

        // load from stack
        pConfig = top.Dereference();
        FAULT_ON_NULL(pConfig);

        // fill in fields from config struct
        pConfig[FIELD___id].SetInteger((CLR_UINT32)config.Id);
        pConfig[FIELD___authentication].SetInteger((CLR_UINT32)config.Authentication);
        pConfig[FIELD___encryption].SetInteger((CLR_UINT32)config.Encryption);
        pConfig[FIELD___radio].SetInteger((CLR_UINT32)config.Radio);
        pConfig[FIELD___options].SetInteger((CLR_UINT8)config.Options);

        // the following ones are strings so a simple assignment isn't enough, need to create a managed string instance
        // and copy over make sure the terminators are there
        config.Password[WIRELESS82011_CONFIG_MAX_PASSWORD_LEN - 1] = 0;
        config.Ssid[WIRELESS82011_CONFIG_MAX_SSID_LEN - 1] = 0;
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___password], (const char *)config.Password));
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___ssid], (const char *)config.Ssid));
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::
    UpdateConfiguration___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        HAL_Configuration_Wireless80211 config;
        CLR_RT_HeapBlock *pConfig = stack.Arg0().Dereference();
        _ASSERTE(pConfig != NULL);

        CLR_UINT32 configurationIndex = pConfig[FIELD___configurationIndex].NumericByRefConst().u4;

        CLR_RT_HeapBlock_String *hbPassword = NULL;
        CLR_RT_HeapBlock_String *hbSsid = NULL;
        CLR_UINT32 ssidLength;
        CLR_UINT32 passwordLength;

        NANOCLR_CLEAR(config);

        config.Id = (CLR_UINT32)pConfig[FIELD___id].NumericByRef().u4;
        config.Authentication = (AuthenticationType)pConfig[FIELD___authentication].NumericByRef().u4;
        config.Encryption = (EncryptionType)pConfig[FIELD___encryption].NumericByRef().u4;
        config.Radio = (RadioType)pConfig[FIELD___radio].NumericByRef().u4;
        config.Options = (Wireless80211Configuration_ConfigurationOptions)pConfig[FIELD___options].NumericByRef().u1;

        // the following ones are strings
        // make sure the terminators are there
        hbPassword = pConfig[FIELD___password].DereferenceString();
        FAULT_ON_NULL(hbPassword);
        passwordLength = hal_strlen_s(hbPassword->StringText());
        if (passwordLength >= sizeof(config.Password))
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        hal_strncpy_s(
            (char *)config.Password,
            WIRELESS82011_CONFIG_MAX_PASSWORD_LEN,
            hbPassword->StringText(),
            passwordLength);

        hbSsid = pConfig[FIELD___ssid].DereferenceString();
        FAULT_ON_NULL(hbSsid);
        ssidLength = hal_strlen_s(hbSsid->StringText());
        if (ssidLength >= sizeof(config.Ssid))
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        hal_strncpy_s((char *)config.Ssid, WIRELESS82011_CONFIG_MAX_SSID_LEN, hbSsid->StringText(), ssidLength);

        // store configuration
        if ((ConfigurationManager_UpdateConfigurationBlock(
                 &config,
                 DeviceConfigurationOption_Wireless80211Network,
                 configurationIndex) == UpdateConfigurationResult_Failed))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::
    GetWirelessAPConfigurationCount___STATIC__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        stack.SetResult_I4(g_TargetConfiguration.WirelessAPConfigs->Count);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::
    GetWirelessAPConfiguration___STATIC__SystemNetNetworkInformationWirelessAPConfiguration__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        HAL_Configuration_WirelessAP config;
        CLR_RT_HeapBlock *pConfig;
        CLR_UINT32 configurationIndex = stack.Arg0().NumericByRef().u4;
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        NANOCLR_CLEAR(config);

        // load wireless AP configuration from the storage
        if (!ConfigurationManager_GetConfigurationBlock(
                (void *)&config,
                DeviceConfigurationOption_WirelessNetworkAP,
                configurationIndex))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        // create new object for configuration
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_WirelessAPConfiguration));

        // load from stack
        pConfig = top.Dereference();
        FAULT_ON_NULL(pConfig);

        // fill in fields from config struct
        pConfig[FIELD___apId].SetInteger((CLR_UINT32)config.Id);
        pConfig[FIELD___apAuthentication].SetInteger((CLR_UINT32)config.Authentication);
        pConfig[FIELD___apEncryption].SetInteger((CLR_UINT32)config.Encryption);
        pConfig[FIELD___apRadio].SetInteger((CLR_UINT32)config.Radio);
        pConfig[FIELD___options].SetInteger((CLR_UINT8)config.Options);
        pConfig[FIELD___apChannel].SetInteger((CLR_UINT8)config.Channel);
        pConfig[FIELD___apMaxConnections].SetInteger((CLR_UINT8)config.MaxConnections);

        // the following ones are strings so a simple assignment isn't enough, need to create a managed string instance
        // and copy over make sure the terminators are there
        config.Password[WIRELESS82011_CONFIG_MAX_PASSWORD_LEN - 1] = 0;
        config.Ssid[WIRELESS82011_CONFIG_MAX_SSID_LEN - 1] = 0;
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___apPassword], (const char *)config.Password));
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_String::CreateInstance(pConfig[FIELD___apSsid], (const char *)config.Ssid));

        NANOCLR_NOCLEANUP();
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::
    UpdateConfiguration___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        HAL_Configuration_WirelessAP config;
        CLR_RT_HeapBlock *pConfig = stack.Arg0().Dereference();
        _ASSERTE(pConfig != NULL);

        CLR_UINT32 configurationIndex = pConfig[FIELD___apConfigurationIndex].NumericByRefConst().u4;

        CLR_RT_HeapBlock_String *hbPassword = NULL;
        CLR_RT_HeapBlock_String *hbSsid = NULL;
        CLR_UINT32 ssidLength;
        CLR_UINT32 passwordLength;

        NANOCLR_CLEAR(config);

        config.Id = (CLR_UINT32)pConfig[FIELD___apId].NumericByRef().u4;
        config.Authentication = (AuthenticationType)pConfig[FIELD___apAuthentication].NumericByRef().u4;
        config.Encryption = (EncryptionType)pConfig[FIELD___apEncryption].NumericByRef().u4;
        config.Radio = (RadioType)pConfig[FIELD___apRadio].NumericByRef().u4;
        config.Options = (WirelessAPConfiguration_ConfigurationOptions)pConfig[FIELD___options].NumericByRef().u1;
        config.Channel = (uint8_t)pConfig[FIELD___apChannel].NumericByRef().u1;
        config.MaxConnections = (uint8_t)pConfig[FIELD___apMaxConnections].NumericByRef().u1;

        // the following ones are strings
        // make sure the terminators are there
        hbPassword = pConfig[FIELD___apPassword].DereferenceString();
        FAULT_ON_NULL(hbPassword);

        passwordLength = hal_strlen_s(hbPassword->StringText());
        if (passwordLength >= sizeof(config.Password))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        hal_strncpy_s(
            (char *)config.Password,
            WIRELESS82011_CONFIG_MAX_PASSWORD_LEN,
            hbPassword->StringText(),
            passwordLength);

        hbSsid = pConfig[FIELD___apSsid].DereferenceString();
        FAULT_ON_NULL(hbSsid);

        ssidLength = hal_strlen_s(hbSsid->StringText());

        if (ssidLength >= sizeof(config.Ssid))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        hal_strncpy_s((char *)config.Ssid, WIRELESS82011_CONFIG_MAX_SSID_LEN, hbSsid->StringText(), ssidLength);

        // store configuration
        if (ConfigurationManager_UpdateConfigurationBlock(
                &config,
                DeviceConfigurationOption_WirelessNetworkAP,
                configurationIndex) == UpdateConfigurationResult_Failed)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }

        NANOCLR_NOCLEANUP();
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::
    NativeGetConnectedClients___STATIC__SZARRAY_SystemNetNetworkInformationWirelessAPStation__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_TypeDef_Index apStationTypeDef;
        CLR_RT_HeapBlock *apStation;
        CLR_RT_HeapBlock *hbObj;

        uint16_t stationCount = 0;

        uint8_t mac[6];
        uint8_t rssi;
        uint32_t phyModes = 0;

        CLR_RT_HeapBlock &top = stack.PushValue();

        // Get index of station info required or if index == 0 then return all connected stations
        uint16_t index = (uint16_t)stack.Arg0().NumericByRef().u4;

        // find <WirelessAPStation> type, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef("WirelessAPStation", "System.Net.NetworkInformation", apStationTypeDef);

        // Count stations connected
        for (int x = 0; x < Network_Interface_Max_Stations(); x++)
        {
            if (index != 0 && x != index)
            {
                continue;
            }

            if (Network_Interface_Get_Station(x, mac, &rssi, &phyModes))
            {
                stationCount++;
            }
        }

        // Create an array of <WirelessAPStation>
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, stationCount, apStationTypeDef));

        // get a pointer to the first object in the array (which is of type <WirelessAPStation>)
        apStation = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

        // Create Array
        if (stationCount > 0)
        {
            for (int x = 0; x < Network_Interface_Max_Stations(); x++)
            {
                if (index != 0 && x != index)
                {
                    continue;
                }

                if (Network_Interface_Get_Station(x, mac, &rssi, &phyModes))
                {
                    // create an instance of <WirelessAPStation>
                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*apStation, apStationTypeDef));

                    // dereference the object in order to reach its fields
                    hbObj = apStation->Dereference();

                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(
                        hbObj[Library_sys_net_native_System_Net_NetworkInformation_WirelessAPStation::
                                  FIELD___macAddress],
                        6,
                        g_CLR_RT_WellKnownTypes.m_UInt8));
                    memcpy(
                        hbObj
                            [Library_sys_net_native_System_Net_NetworkInformation_WirelessAPStation::FIELD___macAddress]
                                .DereferenceArray()
                                ->GetFirstElement(),
                        mac,
                        6);

                    CLR_RT_HeapBlock &rssiFieldRef =
                        hbObj[Library_sys_net_native_System_Net_NetworkInformation_WirelessAPStation::FIELD___rssi];
                    CLR_INT8 *pRes2 = (CLR_INT8 *)&rssiFieldRef.NumericByRef().s1;
                    *pRes2 = rssi;

                    CLR_RT_HeapBlock &phyModesFieldRef =
                        hbObj[Library_sys_net_native_System_Net_NetworkInformation_WirelessAPStation::FIELD___phyModes];
                    CLR_INT32 *pRes3 = (CLR_INT32 *)&phyModesFieldRef.NumericByRef().s4;
                    *pRes3 = phyModes;

                    apStation++;
                }
            }
        }

        NANOCLR_NOCLEANUP();
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::
    NativeDeauthStation___STATIC__STRING__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint16_t index = (uint16_t)stack.Arg0().NumericByRef().u4;

        Network_Interface_Deauth_Station(index);

        NANOCLR_NOCLEANUP_NOLABEL();
    }
    NANOCLR_NOCLEANUP();
}

#pragma endregion

#pragma region NetworkSecurity

HRESULT Library_sys_net_native_System_Net_Security_CertificateManager::
    AddCaCertificateBundle___STATIC__BOOLEAN__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint32_t certificateSize;
        uint32_t allocationSize;
        HAL_Configuration_X509CaRootBundle *caBundle = NULL;

        CLR_RT_HeapBlock_Array *arrayCA = stack.Arg0().DereferenceArray();

        // check for empty array
        FAULT_ON_NULL(arrayCA);

        certificateSize = (int)arrayCA->m_numOfElements;

        // build a HAL_Configuration_X509CaRootBundle
        // because certificate length is variable need to compute required memory
        // header
        allocationSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        // certificate
        allocationSize += certificateSize;

        // allocate memory
        caBundle = (HAL_Configuration_X509CaRootBundle *)platform_malloc(allocationSize);
        // sanity check
        FAULT_ON_NULL(caBundle);

        // fill in struct
        caBundle->CertificateSize = certificateSize;

        // copy from get a pointer to the the binary data for the certificate
        memcpy(caBundle->Certificate, arrayCA->GetFirstElement(), certificateSize);

        if (g_TargetConfiguration.CertificateStore->Count == 0)
        {
            // not found, add the certificate bundle
            // we only support one CA root bundle, so this is fixed to 0
            // block size doesn't matter
            // offset is 0 and done flag is true because this is being stored in a single chunk
            if (ConfigurationManager_StoreConfigurationBlock(
                    caBundle,
                    DeviceConfigurationOption_X509CaRootBundle,
                    0,
                    certificateSize,
                    0,
                    true) != TRUE)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }
        else
        {
            // update the configuration block
            // we only support one CA root bundle, so this is fixed to 0
            if (ConfigurationManager_UpdateConfigurationBlock(
                    caBundle,
                    DeviceConfigurationOption_X509CaRootBundle,
                    0) == UpdateConfigurationResult_Failed)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
        }

        // reach here, we should be OK
        stack.SetResult_Boolean(TRUE);

        NANOCLR_CLEANUP();

        if (caBundle != NULL)
        {
            platform_free(caBundle);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureServerInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

        CLR_RT_TypeDef_Index x509Certificate2TypeDef;

        CLR_INT32 sslContext = -1;
        CLR_INT32 sslMode = stack.Arg0().NumericByRef().s4;
        CLR_INT32 sslVerify = stack.Arg1().NumericByRef().s4;
        CLR_RT_HeapBlock *hbCert = stack.Arg2().Dereference();
        CLR_RT_HeapBlock *caCert = stack.Arg3().Dereference();
        bool useDeviceCertificate = (bool)stack.Arg4().NumericByRef().u1;
        CLR_RT_HeapBlock_Array *arrCert = NULL;
        CLR_RT_HeapBlock_Array *privateKey = NULL;
        CLR_UINT8 *sslCert = NULL;
        volatile int result;
        uint8_t *pk = NULL;
        const char *pkPassword = NULL;
        CLR_UINT32 pkPasswordLength = 0;

        if (hbCert != NULL)
        {
            g_CLR_RT_TypeSystem.FindTypeDef(
                "X509Certificate2",
                "System.Security.Cryptography.X509Certificates",
                x509Certificate2TypeDef);

            arrCert = hbCert[X509Certificate::FIELD___certificate].DereferenceArray();
            arrCert->Pin();

            // there is a client certificate, find if it's a X509Certificate2
            if (hbCert->ObjectCls().Type() == x509Certificate2TypeDef.Type())
            {
                // get private key
                privateKey = hbCert[X509Certificate2::FIELD___privateKey].DereferenceArray();

                // grab the first element, if there is a private key
                if (privateKey)
                {
                    pk = privateKey->GetFirstElement();

                    // get password field
                    CLR_RT_HeapBlock *passwordHb = hbCert[X509Certificate2::FIELD___password].Dereference();

                    // get password length, if there is a password
                    if (passwordHb)
                    {
                        pkPassword = passwordHb->StringText();
                        pkPasswordLength = hal_strlen_s(pkPassword);
                    }
                }
            }

            // get certificate
            sslCert = arrCert->GetFirstElement();
        }

        result =
            (SSL_ServerInit(
                 sslMode,
                 sslVerify,
                 (const char *)sslCert,
                 sslCert == NULL ? 0 : arrCert->m_numOfElements,
                 pk,
                 pk == NULL ? 0 : privateKey->m_numOfElements,
                 pkPassword,
                 pkPasswordLength,
                 sslContext,
                 useDeviceCertificate)
                 ? 0
                 : -1);

        NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));

        if (caCert != NULL)
        {
            arrCert = caCert[X509Certificate::FIELD___certificate].DereferenceArray();

            // If arrCert == NULL then the certificate is an X509Certificate2 which uses a certificate handle
            if (arrCert == NULL)
            {
                arrCert = caCert[X509Certificate::FIELD___handle].DereferenceArray();
                FAULT_ON_NULL(arrCert);

                sslCert = arrCert->GetFirstElement();

                // pass the certificate handle as the data parameter
                result =
                    (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
            }
            else
            {

                arrCert->Pin();

                sslCert = arrCert->GetFirstElement();

                result =
                    (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
            }
        }

        stack.SetResult_I4(sslContext);

        NANOCLR_CLEANUP();

        if (FAILED(hr) && (sslContext != -1))
        {
            SSL_ExitContext(sslContext);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureClientInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

        CLR_RT_TypeDef_Index x509Certificate2TypeDef;

        CLR_INT32 sslContext = -1;
        CLR_INT32 sslMode = stack.Arg0().NumericByRef().s4;
        CLR_INT32 sslVerify = stack.Arg1().NumericByRef().s4;
        CLR_RT_HeapBlock *hbCert = stack.Arg2().Dereference();
        CLR_RT_HeapBlock *caCert = stack.Arg3().Dereference();
        bool useDeviceCertificate = (bool)stack.Arg4().NumericByRef().u1;
        CLR_RT_HeapBlock_Array *arrCert = NULL;
        CLR_RT_HeapBlock_Array *privateKey = NULL;
        CLR_UINT8 *sslCert = NULL;
        volatile int result;
        uint8_t *pk = NULL;
        const char *pkPassword = NULL;
        CLR_UINT32 pkPasswordLength = 0;

        if (hbCert != NULL)
        {
            g_CLR_RT_TypeSystem.FindTypeDef(
                "X509Certificate2",
                "System.Security.Cryptography.X509Certificates",
                x509Certificate2TypeDef);

            arrCert = hbCert[X509Certificate::FIELD___certificate].DereferenceArray();
            arrCert->Pin();

            // there is a client certificate, find if it's a X509Certificate2
            if (hbCert->ObjectCls().Type() == x509Certificate2TypeDef.Type())
            {
                // get private key
                privateKey = hbCert[X509Certificate2::FIELD___privateKey].DereferenceArray();

                // grab the first element, if there is a private key
                if (privateKey)
                {
                    pk = privateKey->GetFirstElement();

                    // get password field
                    CLR_RT_HeapBlock *passwordHb = hbCert[X509Certificate2::FIELD___password].Dereference();

                    // get password length, if there is a password
                    if (passwordHb)
                    {
                        pkPassword = passwordHb->StringText();
                        pkPasswordLength = hal_strlen_s(pkPassword);
                    }
                }
            }

            // get certificate
            sslCert = arrCert->GetFirstElement();
        }

        result =
            (SSL_ClientInit(
                 sslMode,
                 sslVerify,
                 (const char *)sslCert,
                 sslCert == NULL ? 0 : arrCert->m_numOfElements,
                 pk,
                 pk == NULL ? 0 : privateKey->m_numOfElements,
                 pkPassword,
                 pkPasswordLength,
                 sslContext,
                 useDeviceCertificate)
                 ? 0
                 : -1);

        NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));

        if (caCert != NULL)
        {
            arrCert = caCert[X509Certificate::FIELD___certificate].DereferenceArray();

            // If arrCert == NULL then the certificate is an X509Certificate2 which uses a certificate handle
            if (arrCert == NULL)
            {
                arrCert = caCert[X509Certificate::FIELD___handle].DereferenceArray();
                FAULT_ON_NULL(arrCert);

                sslCert = arrCert->GetFirstElement();

                // pass the certificate handle as the data parameter
                result =
                    (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
            }
            else
            {

                arrCert->Pin();

                sslCert = arrCert->GetFirstElement();

                result =
                    (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

                NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
            }
        }

        stack.SetResult_I4(sslContext);

        NANOCLR_CLEANUP();

        if (FAILED(hr) && (sslContext != -1))
        {
            SSL_ExitContext(sslContext);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureAccept___STATIC__VOID__I4__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

        int result = 0;
        bool fRes = true;
        CLR_INT64 *timeout;
        CLR_RT_HeapBlock hbTimeout;

        CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
        CLR_INT32 timeout_ms = -1; // wait forever

        CLR_RT_HeapBlock *socket_info = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket_info);
        socket_entry_t *socket_entry =
            (socket_entry_t *)socket_info[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle]
                .NumericByRef()
                .s4;
        FAULT_ON_NULL(socket_entry);

        // Because we could have been a rescheduled call due to a prior call that would have blocked, we need to see
        // if our handle has been shutdown before continuing.
        if ((int)socket_entry == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
        {
            ThrowError(stack, CLR_E_OBJECT_DISPOSED);
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }

        // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
        hbTimeout.SetInteger((CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS);

        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

        // first make sure we have data to read or ability to write
        while (true)
        {
            nx_tcp_server_socket_accept(socket_ptr, NX_WAIT_FOREVER);
            if (result == SOCK_EWOULDBLOCK || result == SOCK_TRY_AGAIN)
            {
                // non-blocking - allow other threads to run while we wait for socket activity
                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));
            }
            else
            {
                break;
            }
        }

        stack.PopValue(); // Timeout

        NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureConnect___STATIC__VOID__I4__STRING__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int result;

        bool fRes = true;
        CLR_INT64 *timeout;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        CLR_RT_HeapBlock *hb = stack.Arg1().DereferenceString();
        FAULT_ON_NULL_ARG(hb);

        CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
        CLR_RT_HeapBlock hbTimeout;

        const char *szName = hb->StringText();

        // Infinite Timeout
        hbTimeout.SetInteger((CLR_INT64)-1);

        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

        while (true)
        {
            // result = SSL_Connect(handle, szName, sslContext);
            //  Connect to the server
            nx_tcp_client_socket_connect(socket_ptr, IP_ADDRESS(192, 168, 1, 100), SERVER_PORT, NX_WAIT_FOREVER);

            if (result == SOCK_EWOULDBLOCK || result == SOCK_TRY_AGAIN)
            {
                // non-blocking - allow other threads to run while we wait for socket activity
                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));
                if (result < 0)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        stack.PopValue(); // Timeout

        NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureRead___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
#define BUFFER_SIZE 1024
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT32 totReadWrite;
        bool fRes = true;
        CLR_INT64 *timeout;
        int result = 0;
        CLR_UINT8 *buffer;

        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        FAULT_ON_NULL(arrData);

        CLR_INT32 offset = stack.Arg2().NumericByRef().s4;
        CLR_INT32 timeout_ms = stack.Arg4().NumericByRef().s4;

        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        CLR_INT32 count = stack.Arg3().NumericByRef().s4;
        if (count == 0)
        {
            stack.SetResult_I4(0);
            NANOCLR_SET_AND_LEAVE(S_OK);
        }

        hbTimeout.SetInteger((CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS);
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));
        //
        // Push "totReadWrite" onto the eval stack.
        //
        if (stack.m_customState == 1)
        {
            stack.PushValueI4(0);

            stack.m_customState = 2;
        }
        totReadWrite = stack.m_evalStack[1].NumericByRef().s4;
        buffer = arrData->GetElement(offset + totReadWrite);
        count -= totReadWrite;
        if ((offset + count + totReadWrite) > (int)arrData->m_numOfElements)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }
        while (count > 0)
        {
            // first make sure we have data to read or ability to write
            while (fRes)
            {
                // check SSL_DataAvailable() in case SSL has already read and buffered socket data
                result = SSL_DataAvailable(handle);

                if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                {
                    break;
                }

                result = Library_sys_net_native_System_Net_Sockets_NativeSocket::Helper__SelectSocket(handle, 0);

                if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                {
                    break;
                }

                // non-blocking - allow other threads to run while we wait for socket activity
                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));

                // timeout expired
                if (!fRes)
                {
                    result = SOCK_SOCKET_ERROR;

                    ThrowError(stack, SOCK_ETIMEDOUT);

                    NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
                }
            }

            // socket is in the excepted state, so let's bail out
            if (SOCK_SOCKET_ERROR == result)
            {
                break;
            }

            UINT status = nx_secure_tls_session_receive(&tls_session, &receive_packet, NX_WAIT_FOREVER);
            if (status == NX_SUCCESS)
            {
            }

            //    result = SSL_Read(handle, (char *)buffer, count);

            // if (result == SSL_RESULT__WOULD_BLOCK)
            //{
            //     continue;
            // }

            // ThrowOnError expects anything other than 0 to be a failure - so return 0 if we don't have an error
            if (result <= 0)
            {
                break;
            }

            buffer += result;
            totReadWrite += result;
            count -= result;

            // read is non-blocking if we have any data
            if (totReadWrite > 0)
            {
                break;
            }

            stack.m_evalStack[1].NumericByRef().s4 = totReadWrite;
        }

        stack.PopValue(); // totReadWrite
        stack.PopValue(); // Timeout

        if (result < 0)
        {
            NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
        }

        stack.SetResult_I4(totReadWrite);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_net_native_System_Net_Security_SslNative::SecureWrite___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NX_PACKET **packet_ptr_ptr;

        CLR_RT_HeapBlock hbTimeout;
        CLR_INT32 totReadWrite;
        bool fRes = true;
        CLR_INT64 *timeout;
        int result = 0;
        CLR_UINT8 *buffer;

#define BUFFER_SIZE 1024

        UCHAR receive_buffer[BUFFER_SIZE];
        UINT bytes_received;
        UINT status;

        CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
        FAULT_ON_NULL(arrData);

        CLR_INT32 offset = stack.Arg2().NumericByRef().s4;
        CLR_INT32 timeout_ms = stack.Arg4().NumericByRef().s4;
        CLR_RT_HeapBlock *socket_info = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket_info);
        socket_entry_t *socket_entry;
        GetSocketEntry(stack, socket_entry);

        CLR_INT32 count = stack.Arg3().NumericByRef().s4;
        if (count == 0)
        {
            stack.SetResult_I4(0);
            NANOCLR_SET_AND_LEAVE(S_OK);
        }
        hbTimeout.SetInteger((CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS);
        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));
        //
        // Push "totReadWrite" onto the eval stack.
        //
        if (stack.m_customState == 1)
        {
            stack.PushValueI4(0);

            stack.m_customState = 2;
        }
        totReadWrite = stack.m_evalStack[1].NumericByRef().s4;
        buffer = arrData->GetElement(offset + totReadWrite);
        count -= totReadWrite;
        if ((offset + count + totReadWrite) > (int)arrData->m_numOfElements)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
        }
        while (count > 0)
        {
            // first make sure we have data to read or ability to write
            while (fRes)
            {
                // check SSL_DataAvailable() in case SSL has already read and buffered socket data
                result = SSL_DataAvailable(handle);

                if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                {
                    break;
                }

                result = Library_sys_net_native_System_Net_Sockets_NativeSocket::Helper__SelectSocket(handle, 1);

                if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                {
                    break;
                }

                // non-blocking - allow other threads to run while we wait for socket activity
                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));

                // timeout expired
                if (!fRes)
                {
                    result = SOCK_SOCKET_ERROR;

                    ThrowError(stack, SOCK_ETIMEDOUT);

                    NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
                }
            }

            // socket is in the excepted state, so let's bail out
            if (SOCK_SOCKET_ERROR == result)
            {
                break;
            }

            // Send data securely
            UINT status = nx_secure_tls_session_send(&tls_session, &send_packet, NX_WAIT_FOREVER);
            if (status == NX_SUCCESS)
            {
            }

            // ThrowOnError expects anything other than 0 to be a failure - so return 0 if we don't have an error
            if (result <= 0)
            {
                break;
            }

            buffer += result;
            totReadWrite += result;
            count -= result;

            stack.m_evalStack[1].NumericByRef().s4 = totReadWrite;
        }

        stack.PopValue(); // totReadWrite
        stack.PopValue(); // Timeout

        if (result < 0)
        {
            NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
        }

        stack.SetResult_I4(totReadWrite);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureCloseSocket___STATIC__I4__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

        UINT status;
        CLR_INT32 handle;

        CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket);

        handle = socket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle].NumericByRef().s4;
        FAULT_ON_NULL(handle);

        // End the TLS session
        status = nx_secure_tls_session_end(&tls_session, NX_WAIT_FOREVER);
        if (status == NX_SUCCESS)
        {
        }

        // Disconnect the TCP socket
        status = nx_tcp_socket_disconnect(socket_ptr, NX_WAIT_FOREVER);
        if (status == NX_SUCCESS)
        {

            // Delete the TLS session
            nx_secure_tls_session_delete(&tls_session);

            stack.SetResult_I4(status);
        }
        NANOCLR_NOCLEANUP();
    }
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::ExitSecureContext___STATIC__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT status;
        CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
        FAULT_ON_NULL(sslContext);

        status = nx_secure_tls_session_end(&tls_session, NX_WAIT_FOREVER);
        if (status == NX_SUCCESS)
        {
        }
        nx_secure_tls_session_delete(&tls_session);
        stack.SetResult_I4(status);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_Security_SslNative::DataAvailable___STATIC__I4__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NX_SECURE_TLS_SESSION tls_session;
        NX_PACKET *packet;
        UCHAR buffer[1024];
        UINT bytes_received;
        UINT status;

        int result;
        CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
        FAULT_ON_NULL(socket);

        CLR_INT32 handle =
            socket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle].NumericByRef().s4;

        NX_PACKET *packet;
        UINT status = nx_tcp_socket_receive(tcp_socket_ptr, &packet, NX_NO_WAIT);
        if (status == NX_SUCCESS)
        {
            // Data is available in 'packet'
            // Remember to release the packet when done: nx_packet_release(packet);
        }
        else if (status == NX_NO_PACKET)
        {
            // No data available
        }

        stack.SetResult_I4(result);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate::
    ParseCertificate___STATIC__VOID__SZARRAY_U1__BYREF_STRING__BYREF_STRING__BYREF_SystemDateTime__BYREF_SystemDateTime(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        X509Certificate x;
        CLR_INT64 *val;
        SYSTEMTIME st;
        NX_SECURE_X509_CERT certificate;
        UINT status;

        CLR_RT_HeapBlock_Array *arrData = stack.Arg0().DereferenceArray();
        FAULT_ON_NULL(arrData);
        certificate.nx_secure_x509_certificate_data = arrData->GetFirstElement();

        CLR_RT_HeapBlock hbIssuer;
        CLR_RT_ProtectFromGC gc1(hbIssuer);

        CLR_RT_HeapBlock hbSubject;
        CLR_RT_ProtectFromGC gc2(hbSubject);

        // Load the certificate
        status = nx_secure_x509_certificate_initialize(
            &certificate,
            test_device_cert_der,
            test_device_cert_der_len,
            NX_NULL,
            0,
            test_device_cert_key_der,
            test_device_cert_key_der_len,
            NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER);

        if (status == NX_SUCCESS)
        {
        }
        //// from test netxduo code
        status = _nx_secure_tls_local_certificate_add(&tls_session, &certificate);
        //// ??????????

        // fill in the various fields of the certificate class
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(
            hbIssuer,
            certificate.nx_secure_x509_issuer.nx_secure_x509_common_name));
        NANOCLR_CHECK_HRESULT(hbIssuer.StoreToReference(stack.Arg1(), 0));

        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_String::CreateInstance(hbSubject, certificate.nx_secure_x509_subject_identifier));
        NANOCLR_CHECK_HRESULT(hbSubject.StoreToReference(stack.Arg2(), 0));

        st.wYear = cert.EffectiveDate.year;
        st.wMonth = cert.EffectiveDate.month;
        st.wDay = cert.EffectiveDate.day;
        st.wHour = cert.EffectiveDate.hour;
        st.wMinute = cert.EffectiveDate.minute;
        st.wSecond = cert.EffectiveDate.second;
        st.wMilliseconds = cert.EffectiveDate.msec;

        val = Library_corlib_native_System_DateTime::GetValuePtr(stack.Arg3());
        *val = HAL_Time_ConvertFromSystemTime(&st);

        st.wYear = cert.ExpirationDate.year;
        st.wMonth = cert.ExpirationDate.month;
        st.wDay = cert.ExpirationDate.day;
        st.wHour = cert.ExpirationDate.hour;
        st.wMinute = cert.ExpirationDate.minute;
        st.wSecond = cert.ExpirationDate.second;
        st.wMilliseconds = cert.ExpirationDate.msec;

        val = Library_corlib_native_System_DateTime::GetValuePtr(stack.Arg4());
        *val = HAL_Time_ConvertFromSystemTime(&st);
    }
    NANOCLR_NOCLEANUP();
}
#pragma endregion

#pragma region NetworkSockets

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
                    "TCP Socket",
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
                    "Socket 0",
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
                socket_entry->tcp_socket->nx_tcp_socket_connect_ip.nxd_ip_address.v4 = (ULONG)sock_addr.sa_data.ip_addr;
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
        CLR_UINT32 status;

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
    NANOCLR_HEADER();
    {
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
        UINT status = nx_dns_host_by_name_get(&client_dns, host_name, host_address_ptr, NX_IP_PERIODIC_RATE);

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
        UINT status;

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
        UINT status;

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
#pragma endregion

#pragma region NetworkCryptography
HRESULT Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2::
    DecodePrivateKeyNative___STATIC__VOID__SZARRAY_U1__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *keyData = stack.Arg0().DereferenceArray();
        CLR_UINT8 *keyBuffer = keyData->GetFirstElement();

        const char *password = NULL;
        CLR_UINT32 passwordLength = 0;

        CLR_RT_HeapBlock *passwordHb = stack.Arg1().DereferenceString();
        if (passwordHb != NULL)
        {
            password = passwordHb->StringText();
            passwordLength = hal_strlen_s(password);
        }

        UINT status;
        NX_SECURE_TLS_SESSION tls_session;
        UCHAR private_key[] = {/* DER-encoded private key bytes */};
        UINT private_key_length = sizeof(private_key);

        NX_CRYPTO_EC *curve;
        UCHAR *output;
        ULONG output_length_in_byte;
        ULONG *actual_output_length;
        HN_UBASE *scratch;

        status =
            _nx_crypto_ec_key_pair_stream_generate(curve, output, output_length_in_byte, actual_output_length, scratch);

        if (SSL_DecodePrivateKey(
                (const unsigned char *)keyBuffer,
                keyData->m_numOfElements,
                (const unsigned char *)password,
                passwordLength) < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
#pragma endregion

#pragma region Network Thread
void FileThread_entry(ULONG parameter)
{

    ULONG received_message;
    while (true)
    {
        tx_queue_receive(&message_queue, &received_message, TX_WAIT_FOREVER);
    }
}

#pragma endregion

#pragma region Network Processing

#define VALID_INTERFACE                   1
#define NX_MAX_RAM_INTERFACES             1
#define NX_RAM_DRIVER_MAX_MCAST_ADDRESSES 1
ULONG simulated_address_msw = 0x0011;
ULONG simulated_address_lsw = 0x22334456;

typedef struct MAC_ADDRESS_STRUCT
{
    ULONG nx_mac_address_msw;
    ULONG nx_mac_address_lsw;
} MAC_ADDRESS;
typedef struct _nx_ram_network_driver_instance_type
{
    UINT nx_ram_network_driver_in_use;
    UINT nx_ram_network_driver_id;
    NX_INTERFACE *nx_ram_driver_interface_ptr;
    NX_IP *nx_ram_driver_ip_ptr;
    MAC_ADDRESS nx_ram_driver_mac_address;
    MAC_ADDRESS nx_ram_driver_mcast_address[NX_RAM_DRIVER_MAX_MCAST_ADDRESSES];
} _nx_ram_network_driver_instance_type;

static _nx_ram_network_driver_instance_type nx_ram_driver[NX_MAX_RAM_INTERFACES];

VOID Network_Request_Processing(NX_IP_DRIVER *driver_req_ptr)
{
    UINT status;
    UINT i = 0;
    UINT interface_index;
    ULONG *ethernet_frame_ptr;
    NX_IP *ip_ptr;
    NX_INTERFACE *interface_ptr;
    NX_PACKET *packet_ptr;

    ip_ptr = driver_req_ptr->nx_ip_driver_ptr;
    interface_ptr = driver_req_ptr->nx_ip_driver_interface;
    interface_index = interface_ptr->nx_interface_index;

    // Process according to the driver request type in the IP control block
    switch (driver_req_ptr->nx_ip_driver_command)
    {
        // This request is invoked from the NetX Duo to the device driver, allowing the driver to associate the
        // driver instance with the corresponding IP instance and the physical interface instance within the IP.
        case NX_LINK_INTERFACE_ATTACH:

            if (VALID_INTERFACE)
            {
                nx_ram_driver[0].nx_ram_network_driver_in_use = 1;
                nx_ram_driver[0].nx_ram_network_driver_id = i;
                nx_ram_driver[0].nx_ram_driver_interface_ptr = interface_ptr;
                nx_ram_driver[0].nx_ram_driver_ip_ptr = ip_ptr;
                nx_ram_driver[0].nx_ram_driver_mac_address.nx_mac_address_msw = simulated_address_msw;
                nx_ram_driver[0].nx_ram_driver_mac_address.nx_mac_address_lsw = simulated_address_lsw + i;
            }
            else
            {
                driver_req_ptr->nx_ip_driver_status = NX_INVALID_INTERFACE;
            }
            break;
        case NX_LINK_INTERFACE_DETACH:
            memset(&(nx_ram_driver[i]), 0, sizeof(_nx_ram_network_driver_instance_type));
            driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
            break;
        // After the application calls nx_ip_create, the IP helper thread sends a driver request with the command
        // set to NX_LINK_INITIALIZE to the driver to initialize its physical network interface.
        case NX_LINK_INITIALIZE:
            NX_INTERFACE *interface_ptr = driver_req_ptr->nx_ip_driver_interface;
            nx_driver_information.nx_driver_information_ip_ptr = NX_NULL;
            nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_NOT_INITIALIZED;
            nx_driver_information.nx_driver_information_packet_pool_ptr = ip_ptr->nx_ip_default_packet_pool;
            nx_driver_information.nx_driver_information_deferred_events = 0;

            if (!nx_driver_hardware_initialize)
            {
                status = NX_SUCCESS;
            }
            else
            {
                status = nx_driver_hardware_initialize(driver_req_ptr);
            }
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
        case NX_LINK_UNINITIALIZE:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        // An IPv4 or IPv6 packet is being transmitted,
        case NX_LINK_PACKET_SEND:
            break;
        case NX_LINK_PACKET_BROADCAST:
            break;
        case NX_LINK_ARP_SEND:
            break;
        case NX_LINK_ARP_RESPONSE_SEND:
            break;
        case NX_LINK_RARP_SEND:
            if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;

                nx_packet_transmit_release(driver_req_ptr->nx_ip_driver_packet);
                return;
            }
            // Place the ethernet frame at the front of the packet, adjust the prepend pointer and adjust the
            // packet length
            packet_ptr = driver_req_ptr->nx_ip_driver_packet;
            packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr - NX_DRIVER_PHYSICAL_FRAME_SIZE;
            packet_ptr->nx_packet_length = packet_ptr->nx_packet_length + NX_DRIVER_PHYSICAL_FRAME_SIZE;

            // Setup the ethernet frame pointer to build the ethernet frame.
            // Backup another 2 bytes to get 32-bit word alignment.
            ethernet_frame_ptr = (ULONG *)(packet_ptr->nx_packet_prepend_ptr - 2);
            *ethernet_frame_ptr = driver_req_ptr->nx_ip_driver_physical_address_msw;
            *(ethernet_frame_ptr + 1) = driver_req_ptr->nx_ip_driver_physical_address_lsw;
            *(ethernet_frame_ptr + 2) =
                (ip_ptr->nx_ip_arp_physical_address_msw << 16) | (ip_ptr->nx_ip_arp_physical_address_lsw >> 16);
            *(ethernet_frame_ptr + 3) = (ip_ptr->nx_ip_arp_physical_address_lsw << 16);

            // Set up the frame type field in the Ethernet harder
            if ((driver_req_ptr->nx_ip_driver_command == NX_LINK_ARP_SEND) ||
                (driver_req_ptr->nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND))
            {
                *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_ARP;
            }
            else if (driver_req_ptr->nx_ip_driver_command == NX_LINK_RARP_SEND)
            {
                *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_RARP;
            }
            else if (packet_ptr->nx_packet_ip_version == NX_IP_VERSION_V6)
            {
                *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IPV6;
            }
            else
            {
                *(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IP;
            }
            // Determine if the packet exceeds the driver's MTU
            if (packet_ptr->nx_packet_length > NX_DRIVER_MTU)
            {
                // Packet exceeds the size of the driver's MTU
                NX_DRIVER_PHYSICAL_HEADER_REMOVE(packet_ptr);
                nx_packet_transmit_release(packet_ptr);
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                return;
            }
            // Transmit the packet through the network controller low level access routine
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


        // The IP helper thread enables the physical network by setting the driver command to NX_LINK_ENABLE
        // in the driver request and sending the request to the network driver.
        // This happens shortly after the IP helper thread completes the initialization request.
        case NX_LINK_ENABLE:
            UINT status;
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
            {
                status = NX_SUCCESS;
            }
            else
            {
                status = nx_driver_hardware_enable(driver_req_ptr);
            }
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

        // NX_LINK_DISABLE is called after the deletion of an IP instance or an application may issue
        // this command in order to temporarily disable the link in order to save power.
        case NX_LINK_DISABLE:
            if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED)
            {
                driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
                return;
            }
            if (!nx_driver_hardware_disable)
            {
                status = NX_SUCCESS;
            }
            else
            {
                status = nx_driver_hardware_disable(driver_req_ptr);
            }
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
            break;

        // This request is made with the nx_igmp_multicast_interface join and nx_ipv4_multicast_interface_join
        // service in IPv4, nxd_ipv6_multicast_interface_join service in IPv6, and various operation required by
        // IPv6. The network driver takes the supplied multicast group address and sets up the physical media to
        // accept incoming packets from that multicast group address.
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

        // This request is invoked by explicitly calling the nx_igmp_multicast_interface_leave or
        // nx_ipv4_multicast_interface_leave services in IPv4, nxd_ipv6_multicast_interface_leave service in IPv6,
        // or by various internal NetX Duo operations required for IPv6.
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

        // Called by nx_ip_interface_status_check service for any interface on the host
        case NX_LINK_GET_STATUS:
            UINT status;

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

        // This request is made from the IP helper thread in response to the driver calling the
        // _nx_ip_driver_deferred_processing routine from a transmit or receive ISR. This allows the driver ISR to
        // defer the packet receive and transmit processing to the IP helper thread and thus reduce the amount to
        // process in the ISR
        case NX_LINK_DEFERRED_PROCESSING:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        case NX_LINK_SET_PHYSICAL_ADDRESS:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        //  Depending upon the level of hardware checksum support from the hardware,
        //  the device driver needs to inform the IP instance which hardware feature is enabled.This way,
        //  the IP instance is aware of the hardware feature,
        //  and offload as much computation to the hardware as possible.
        case NX_INTERFACE_CAPABILITY_SET:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
            break;

        case NX_INTERFACE_CAPABILITY_GET:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
            break;

        // This request is made from within the nx_ip_driver_direct_command service
        case NX_LINK_GET_SPEED:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
            break;

        // This request is made from within the nx_ip_driver_direct_command service.
        case NX_LINK_GET_DUPLEX_TYPE:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
            break;

        // This request is made from within the nx_ip_driver_direct_command service
        case NX_LINK_GET_ERROR_COUNT:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
            break;

        // This request is made from within the nx_ip_driver_direct_command service
        case NX_LINK_GET_RX_COUNT:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        // This request is made from within the nx_ip_driver_direct_command service
        case NX_LINK_GET_TX_COUNT:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        // This request is made from within the nx_ip_driver_direct_command service
        case NX_LINK_GET_ALLOC_ERRORS:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        case NX_LINK_FACTORY_ADDRESS_GET:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        case NX_LINK_RX_ENABLE:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        case NX_LINK_RX_DISABLE:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        case NX_LINK_6LOWPAN_COMMAND:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        case NX_LINK_GET_INTERFACE_TYPE:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        // This request is made from within the nx_ip_driver_direct_command service.
        // The driver processes the application specific user commands.
        case NX_LINK_USER_COMMAND:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;

        default:
            *(driver_req_ptr->nx_ip_driver_return_ptr) = NX_INTERFACE_TYPE_UNKNOWN;
            driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}
#pragma endregion
