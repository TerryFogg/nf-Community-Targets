//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <sys_dev_wifi_native.h>
#include "Wifi.h"

extern "C"
{
#include "cyw43.h"
#include <cyw43_ll.h>
}
static HAL_Configuration_Wireless80211 wifiConfig;
extern WifiConnectionStatus connectionStatus;

HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // Initialization is done at power up and located in the NetXDuo framework driver "switch" statement.
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        Wifi::Dispose();
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeSetDeviceName___VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeFindWirelessAdapters___STATIC__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *array;
        CLR_UINT8 *arrayOfIndexes;
        int interfaceCount = 1;
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        HAL_Configuration_NetworkInterface *netInterfaceConfig =
            (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));
        if (netInterfaceConfig == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }
        // 1st pass: Find the number of wireless adapters
        for (int index = 0; index < g_TargetConfiguration.NetworkInterfaceConfigs->Count; index++)
        {
            if (!ConfigurationManager_GetConfigurationBlock(
                    netInterfaceConfig,
                    DeviceConfigurationOption_Network,
                    index))
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            }
            if (netInterfaceConfig->InterfaceType == NetworkInterfaceType_Wireless80211)
            {
                interfaceCount++;
            }
        }
        // Build array with indexes of Wireless interfaces
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(top, interfaceCount, g_CLR_RT_WellKnownTypes.m_UInt8));
        array = top.DereferenceArray();
        arrayOfIndexes = array->GetFirstElement();

        // 2nd pass: grab the index of the of the wireless adapters
        for (int index = 0; index < g_TargetConfiguration.NetworkInterfaceConfigs->Count; index++)
        {
            // if (!ConfigurationManager_GetConfigurationBlock(
            //         netInterfaceConfig,
            //         DeviceConfigurationOption_Network,
            //         index))
            //{
            //     NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
            // }
            if (netInterfaceConfig->InterfaceType == NetworkInterfaceType_Wireless80211)
            {
                *arrayOfIndexes = index;
                arrayOfIndexes++;
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::
    NativeConnect___SystemDeviceWifiWifiConnectionStatus__STRING__STRING__SystemDeviceWifiWifiReconnectionKind(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int netIndex;
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT64 *timeout;
        hbTimeout.SetInteger((CLR_INT64)20000 * TIME_CONVERSION__TO_MILLISECONDS);
        bool eventResult = true;
        HAL_Configuration_Wireless80211 *wirelessConfig;
        int reconnectionKind = stack.Arg3().NumericByRef().s4;

        FAULT_ON_NULL(stack.This());
        {
            const char *szSsid = stack.Arg1().RecoverString();
            FAULT_ON_NULL(szSsid);
            {
                const char *szPassPhase = stack.Arg2().RecoverString();
                FAULT_ON_NULL(szPassPhase);
                {
                    if (stack.This()[Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::FIELD___disposedValue]
                            .NumericByRef()
                            .u1 != 0)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
                    }
                    else
                    {
                        netIndex = stack.This()[FIELD___networkInterface].NumericByRefConst().s4;
                    }
                    wirelessConfig = ConfigurationManager_GetWirelessConfigurationFromId(netIndex);

                    if (reconnectionKind & NETWORK_CONNECT_RECONNECT)
                    {
                        wifiConfig.Options | Wireless80211Configuration_ConfigurationOptions_AutoConnect;
                    }
                    hal_strcpy_s((char *)wifiConfig.Ssid, sizeof(wifiConfig.Ssid), szSsid);
                    hal_strcpy_s((char *)wifiConfig.Password, sizeof(wifiConfig.Password), szPassPhase);

                    // Initiate a Connect then reliquish thread while waiting for completion with timeout.
                    // The WaitEvents will return earlier than timeout value if
                    // 'Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION)' translated to wait for event 'Event_Wifi_Station'
                    stack.SetupTimeoutFromTicks(hbTimeout, timeout);
                    Wifi::Connect(wifiConfig);
                    if (connectionStatus == WifiConnectionStatus::WifiConnectionStatus_NetworkNotAvailable)
                    {
                        // Non-blocking wait allowing other threads to run while we wait for the wifi to connect
                        g_CLR_RT_ExecutionEngine
                            .WaitEvents(stack.m_owningThread, *timeout, Event_Wifi_Station, eventResult);
                    }
                    if (!eventResult)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
                    }
                    stack.SetResult_I4(eventResult);
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeDisconnect___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        Wifi::Disconnect(TheSingleWirelessAdapter);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::NativeScanAsync___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        HAL_Configuration_NetworkInterface configNetworkInterface;
        HAL_Configuration_Wireless80211 configWireless80211NetworkInterface;
        CLR_UINT32 adapterIndex = stack.This()[FIELD___networkInterface].NumericByRefConst().s4;

        // This code base only supports 1 wireless adapter
        adapterIndex = TheSingleWirelessAdapter;
        if (!Wifi::StartScan())
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_wifi_native_System_Device_Wifi_WifiAdapter::GetNativeScanReport___SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock hbTimeout;
        CLR_INT64 *timeout;
        hbTimeout.SetInteger((CLR_INT64)20000 * TIME_CONVERSION__TO_MILLISECONDS);
        bool eventResult = true;

        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        CLR_RT_HeapBlock_Array *array;
        CLR_UINT8 *buf;
        uint16_t number = 0;
        int rlen = 2;

        // Initiate a Scan then reliquish thread while waiting for completion with timeout.
        // The WaitEvents will return earlier than timeout value if
        // 'Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION)' translated to wait for event 'Event_Wifi_Station'
        Wifi::StartScan();
        stack.SetupTimeoutFromTicks(hbTimeout, timeout);
        g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Wifi_Station, eventResult);
        if (!eventResult)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
        // Return the report in a byte array for analysis
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, rlen, g_CLR_RT_WellKnownTypes.m_UInt8));
        array = top.DereferenceArray();
        buf = array->GetFirstElement();
    }
    NANOCLR_NOCLEANUP();
}
