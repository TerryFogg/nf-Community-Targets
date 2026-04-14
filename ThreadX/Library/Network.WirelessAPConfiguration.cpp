//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Network.h"
#include "nanoHAL_v2.h"
#include "nanoCLR_Interop.h"

extern "C"
{
#include "cyw43.h"
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

    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::
    UpdateConfiguration___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
    }
    NANOCLR_NOCLEANUP();
}


HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::
    NativeGetConnectedClients___STATIC__SZARRAY_SystemNetNetworkInformationWirelessAPStation__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::
    NativeDeauthStation___STATIC__STRING__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint16_t index = (uint16_t)stack.Arg0().NumericByRef().u4;

        cyw43_wifi_set_up(&cyw43_state,0, false, 0);

        NANOCLR_NOCLEANUP_NOLABEL();
    }
    NANOCLR_NOCLEANUP();
}
