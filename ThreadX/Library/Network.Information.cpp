//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Network.h"

HAL_Configuration_NetworkInterface network_Interface;

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

        *pIPAddressRef = network_Interface.IPv4Address;

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
