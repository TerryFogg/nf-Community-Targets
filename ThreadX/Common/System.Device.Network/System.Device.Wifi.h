#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "sys_dev_wifi_native.h"
#include "nanoHAL_Network.h"

#define TheSingleWirelessAdapter 1

typedef struct ScanReportRecordMatchesManagedCode
{
    uint8_t bssid[6];
    uint8_t ssid[33];
    uint8_t rssi;
    uint8_t authMode;
    uint8_t cypherType;
} ScanReportRecordMatchesManagedCode;

class DeviceWifi
{
  public:
    static const WifiConnectionStatus connectionWaiting = (WifiConnectionStatus)-99;
    static WifiConnectionStatus connectionStatus;
    static bool Initialize();
    static bool Dispose();
    static void Connect(HAL_Configuration_Wireless80211 wifiConfig);
    static void Disconnect(int index);
    static bool StartScan();
    static bool ScanActive();
    static int  NumberOfScanReportEntries();
    static bool GetScanReport(ScanReportRecordMatchesManagedCode *reportEntries);
    static bool WifiUp();
};
