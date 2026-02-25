//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Network.h"

static HAL_Configuration_Wireless80211 Wireless80211_DefaultConfig = {
    // Marker
    { 'W', '8', '2', '1' },
    // Id
    0,
    AuthenticationType_Open,
    EncryptionType_None,
    RadioType__802_11n,
    // SSID
    { 0 },
    // Password
    { 0 },
    Wireless80211Configuration_ConfigurationOptions_None,
    // Rssi
};


HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::
    GetWireless82011ConfigurationCount___STATIC__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        stack.SetResult_I4(1);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::
    GetWireless82011Configuration___STATIC__SystemNetNetworkInformationWireless80211Configuration__I4(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        HAL_Configuration_Wireless80211 config = {};
        CLR_RT_HeapBlock *pConfig;
        CLR_UINT32 configurationIndex = stack.Arg0().NumericByRef().u4;
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        // create new object for configuration
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_Wireless80211Configuration));

        // load from stack
        pConfig = top.Dereference();
        FAULT_ON_NULL(pConfig);
        {
            pConfig[FIELD___id].SetInteger((CLR_UINT32)Wireless80211_DefaultConfig.Id);
            pConfig[FIELD___authentication].SetInteger((CLR_UINT32)Wireless80211_DefaultConfig.Authentication);
            pConfig[FIELD___encryption].SetInteger((CLR_UINT32)Wireless80211_DefaultConfig.Encryption);
            pConfig[FIELD___radio].SetInteger((CLR_UINT32)Wireless80211_DefaultConfig.Radio);
            pConfig[FIELD___options].SetInteger((CLR_UINT8)Wireless80211_DefaultConfig.Options);

            // Create managed strings and copy values from HAL structure
            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_String::CreateInstance(
                    pConfig[FIELD___password],
                    (const char *)Wireless80211_DefaultConfig.Password));
            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_String::CreateInstance(
                    pConfig[FIELD___ssid],
                    (const char *)Wireless80211_DefaultConfig.Ssid));
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::
    UpdateConfiguration___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pConfig = stack.Arg0().Dereference();
        _ASSERTE(pConfig != NULL);

        CLR_UINT32 configurationIndex = pConfig[FIELD___configurationIndex].NumericByRefConst().u4;

        CLR_RT_HeapBlock_String *hbPassword = NULL;
        CLR_RT_HeapBlock_String *hbSsid = NULL;
        CLR_UINT32 ssidLength;
        CLR_UINT32 passwordLength;

        Wireless80211_DefaultConfig.Id = (CLR_UINT32)pConfig[FIELD___id].NumericByRef().u4;
        Wireless80211_DefaultConfig.Authentication =
            (AuthenticationType)pConfig[FIELD___authentication].NumericByRef().u4;
        Wireless80211_DefaultConfig.Encryption = (EncryptionType)pConfig[FIELD___encryption].NumericByRef().u4;
        Wireless80211_DefaultConfig.Radio = (RadioType)pConfig[FIELD___radio].NumericByRef().u4;
        Wireless80211_DefaultConfig.Options =
            (Wireless80211Configuration_ConfigurationOptions)pConfig[FIELD___options].NumericByRef().u1;

        // the following ones are strings
        // make sure the terminators are there
        hbPassword = pConfig[FIELD___password].DereferenceString();
        FAULT_ON_NULL(hbPassword);
        {
            passwordLength = hal_strlen_s(hbPassword->StringText());
            if (passwordLength >= sizeof(Wireless80211_DefaultConfig.Password))
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            hal_strncpy_s(
                (char *)Wireless80211_DefaultConfig.Password,
                WIRELESS82011_CONFIG_MAX_PASSWORD_LEN,
                hbPassword->StringText(),
                passwordLength);

            hbSsid = pConfig[FIELD___ssid].DereferenceString();
            FAULT_ON_NULL(hbSsid);
            {
                ssidLength = hal_strlen_s(hbSsid->StringText());
                if (ssidLength >= sizeof(Wireless80211_DefaultConfig.Ssid))
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                hal_strncpy_s(
                    (char *)Wireless80211_DefaultConfig.Ssid,
                    WIRELESS82011_CONFIG_MAX_SSID_LEN,
                    hbSsid->StringText(),
                    ssidLength);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
