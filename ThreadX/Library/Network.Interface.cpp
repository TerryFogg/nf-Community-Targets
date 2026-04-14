//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Network.h"
#include "NetworkInterfaceAdapter.h"

static HAL_Configuration_NetworkInterface network_Interface;
static HAL_Configuration_NetworkInterface ethernetInterface;
static HAL_Configuration_WirelessAP wifiAP;

HRESULT Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::
    InitializeNetworkInterfaceSettings___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NX_DHCP *dhcp_ptr;
        HAL_Configuration_NetworkInterface config;
        CLR_RT_HeapBlock *pConfig = stack.Arg0().Dereference();
        FAULT_ON_NULL(pConfig);
        NX_IP *ip_ptr;

        CLR_UINT32 interfaceIndex = pConfig[FIELD___interfaceIndex].NumericByRefConst().u4;
        HAL_Configuration_NetworkInterface *pLocalConfig;

        // if (!ConfigurationManager_GetConfigurationBlock(
        //         (void *)&config,
        //         DeviceConfigurationOption_Network,
        //         interfaceIndex))
        //{
        //     NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        // }
        //_ASSERTE(config.StartupAddressMode > 0);

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

        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(
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
                pLocalConfig = &network_Interface;
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
        wifiIsAvailable = Wifi::WifiUp();
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
                    pLocalConfig = &network_Interface;
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

            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_Array::CreateInstance(
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
