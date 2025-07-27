//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "System.Device.Wifi.h"
#include <cyw43_country.h>
#include "cyw43.h"
#include <cyw43_ll.h>

// Note: cyw43_state
// Appears to be a global variable to maintain the current state

int scan_results_callback(void *env, const cyw43_ev_scan_result_t *result);

static const cyw43_ev_scan_result_t *scan_results;

CLR_UINT32 itfClientInterfaceSTAMode = CYW43_ITF_STA;
CLR_UINT32 itfAccessPointInterfaceMode = CYW43_ITF_AP;

// Start the Wifi, default power management
bool DeviceWifi::Initialize()
{
    // This can take 100-200ms minimum
    cyw43_init(&cyw43_state);
    int itf = CYW43_ITF_STA;
    bool up = true;
    uint32_t country = CYW43_COUNTRY_WORLDWIDE;
    cyw43_wifi_set_up(&cyw43_state, itf, up, country);
    return true;
}
bool DeviceWifi::Dispose()
{
    // Stops TCP/IP interfaces, power of the WLAN chip
    cyw43_deinit(&cyw43_state);
    return true;
}
bool DeviceWifi::WifiUp()
{
    // Only supports 'Client interface STA mode'             -
    int result = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    return (result == 1);
}
/// Support for 'Client interface STA mode' only
void DeviceWifi::Connect(HAL_Configuration_Wireless80211 wifiConfig)
{
    CLR_UINT32 AuthenticationType;
    switch (wifiConfig.Authentication)
    {
        case AuthenticationType::AuthenticationType_None:
        case AuthenticationType::AuthenticationType_Open:
            AuthenticationType = CYW43_AUTH_OPEN;
            break;
        case AuthenticationType::AuthenticationType_WPA:
            AuthenticationType = CYW43_AUTH_WPA_TKIP_PSK;
            break;
        case AuthenticationType::AuthenticationType_WPA2:
            AuthenticationType = CYW43_AUTH_WPA2_AES_PSK;
            break;
        // Not supported
        // Windows now
        case AuthenticationType::AuthenticationType_WCN:
        // Vunerable
        case AuthenticationType::AuthenticationType_WEP:
        case AuthenticationType::AuthenticationType_EAP:
        case AuthenticationType::AuthenticationType_PEAP:
        case AuthenticationType::AuthenticationType_Shared:
            AuthenticationType = -1;
            break;
            // Future support ?
            // CYW43_AUTH_WPA2_MIXED_PSK    WPA2/WPA mixed authorisation
            // CYW43_AUTH_WPA3_SAE_AES_PSK  WPA3 AES authorisation
            // CYW43_AUTH_WPA3_WPA2_AES_PSK WPA2/WPA3 authorisation
    }

    size_t ssid_len = 64;
    const uint8_t *ssid = wifiConfig.Ssid;
    size_t key_len = 32;
    const uint8_t *key = wifiConfig.Password;
    const uint32_t channel = CYW43_CHANNEL_NONE;

    cyw43_wifi_join(&cyw43_state, ssid_len, ssid, key_len, key, AuthenticationType, NULL, channel);
    Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION);

}
void DeviceWifi::Disconnect(int index)
{
    WifiConnectionStatus::WifiConnectionStatus_UnspecifiedFailure;
}
bool DeviceWifi::StartScan()
{
    cyw43_wifi_scan_options_t scan_options = {0};
    int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_results_callback);
    return (err == 1);
}
bool DeviceWifi::ScanActive()
{
    return cyw43_wifi_scan_active(&cyw43_state);
}
int DeviceWifi::NumberOfScanReportEntries()
{
    return 1;
}
bool DeviceWifi::GetScanReport(ScanReportRecordMatchesManagedCode *reportEntries)
{
    const uint8_t unknown = 0;
    reportEntries = (ScanReportRecordMatchesManagedCode *)platform_malloc(
        sizeof(ScanReportRecordMatchesManagedCode) * NumberOfScanReportEntries());

    for (int i = 0; i < NumberOfScanReportEntries(); i++)
    {
        reportEntries->authMode = scan_results->auth_mode;
        for (int bs = 0; bs < 5; bs++)
        {
            reportEntries->bssid[bs] = scan_results->bssid[bs];
        }
        reportEntries->cypherType = scan_results->channel;
        reportEntries->rssi = scan_results->rssi;
        for (int s = 0; s < sizeof(scan_results->ssid); s++)
        {
            if (s < scan_results->ssid_len)
            {
                reportEntries->ssid[s] = scan_results->ssid[s];
            }
            reportEntries->ssid[s] = 0;
        }
        reportEntries->cypherType = unknown;
    }
    return true;
}
int scan_results_callback(void *env, const cyw43_ev_scan_result_t *result)
{
    if (result)
    {
        scan_results = result;
        Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION);
    }
    return 0;
}
