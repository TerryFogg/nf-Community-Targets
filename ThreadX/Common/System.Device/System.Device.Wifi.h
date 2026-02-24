#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>
#include "nanoHAL_network.h"

#define TheSingleWirelessAdapter 1
#define ALL_ACCESS_POINT_STATIONS 0

#define WIFI_AUTH_TYPE            CYW43_AUTH_WPA2_AES_PSK
#define WIFI_CONNECT_TIMEOUT      10000 /* WIFI_CONNECT_TIMEOUT */
#define WIFI_SSID                 "test"
#define WIFI_PASSWORD             "test"
#define NX_DRIVER_STACK_SIZE      1024
#define NX_DRIVER_THREAD_INTERVAL (NX_IP_PERIODIC_RATE / 10)


typedef enum
{
    PHY802_11b = 1,
    // IEEE 802.11g  max 54 Mbit/s
    PHY802_11g = 2,
    // IEEE 802.11n  max 288.8 Mbit/s for 20mhz channel or 600 for 40Mhz
    PHY802_11n = 4,
    // Low rate enabled.
    PHY802_11lr = 8,
} WIRELESS_PROTOCOL;

typedef struct
{
    uint8_t mac[6];
    uint8_t rssi;
    WIRELESS_PROTOCOL phyModes;
} access_point_connected_clients_t;

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
    static bool Initialize();
    static bool Dispose();
    static void Connect(HAL_Configuration_Wireless80211 wifiConfig);
    static void Disconnect(int index);
    static bool StartScan();
    static bool ScanActive();
    static int NumberOfScanReportEntries();
    static bool GetScanReport(ScanReportRecordMatchesManagedCode *reportEntries);
    static bool WifiUp();
    static bool GetAccessPointMaxStations();
    static bool GetAccessPointMaxAssociations(int station);
    static bool GetAccessPointInformation(
        int station,
        int expected_number_associations,
        access_point_connected_clients_t *apConnectedClients);
};
