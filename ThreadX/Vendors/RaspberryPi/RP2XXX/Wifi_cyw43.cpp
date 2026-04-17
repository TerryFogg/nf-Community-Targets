//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "network.h"
#include <tx_api.h>
#include <nx_driver_framework.h>
#include <NetworkConfigurationManager.h>

extern NX_DRIVER_INFORMATION nx_driver_information;
extern int g_Country_Code;
extern TX_BYTE_POOL byte_pool_0;
extern NetworkConfiguration networkConfiguration;

void Poll_cyw43_Thread(ULONG parameter);

#ifdef __cplusplus
extern "C"
{
#endif

#include "cyw43.h"
#include <pico/cyw43_arch.h>
#include <cyw43_ll.h>
#include "NetworkInterfaceAdapter.h"

    struct pbuf;
    uint16_t pbuf_copy_partial(const struct pbuf *p, void *dataptr, uint16_t len, uint16_t offset)
    {
        /* Stub function as it will never hit. */
        return 0;
    }

#ifdef __cplusplus
}
#endif

TX_THREAD TX_PollingThread;
extern UCHAR nx_driver_pico_w_link_up;
int scan_results_callback(void *env, const cyw43_ev_scan_result_t *result);
WifiConnectionStatus connectionStatus;
static const cyw43_ev_scan_result_t *scan_results;

void CreatePollingThread()
{
    void *pointer = TX_NULL;
    int POLL_THREAD_STACK_SIZE = 2000;
    int POLL_THREAD_PRIORITY = 5-1;
    ULONG parameter = 0;

    UINT status = tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, POLL_THREAD_STACK_SIZE, TX_NO_WAIT);

    status = tx_thread_create(
        &TX_PollingThread,
        (char *)"Polling cyw43",
        Poll_cyw43_Thread,
        parameter,
        pointer,
        POLL_THREAD_STACK_SIZE,
        POLL_THREAD_PRIORITY,
        POLL_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

void Poll_cyw43_Thread(ULONG parameter)
{
    // ISSUE:
    // Because polling mode only makes network/driver progress when you call cyw43_arch_poll(),
    // the poll interval becomes a major contributor to network "reaction time."

    // TODO: Change to interrupt driven
    // --------------------------------
    // ISSUE:
    // The Pico SDK has a background mode but it is tightly coupled to lwIP,
    // and you cannot simply "turn lwIP off" and keep the background machinery while substituting NetX Duo.
    //
    // A future option (to be written) is to use the cyw43_driver (low-level) and handle the SPI and interrupt
    // handling.
    // ------------------------------------------------
    NX_IP *ip_ptr = nx_driver_information.nx_driver_information_ip_ptr;
    volatile int linkstatus;

    for (;;)
    {
        // Obtain the IP internal mutex, poll and release
        tx_mutex_get(&(ip_ptr->nx_ip_protection), TX_WAIT_FOREVER);
        {
            cyw43_arch_poll();
  //          linkstatus = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
        }
        tx_mutex_put(&(ip_ptr->nx_ip_protection));
        tx_thread_sleep(1);
    }
}
// Start the Wifi, default power management
bool Wifi::Initialize(uint8_t *mac)
{
    if (cyw43_arch_init() != PICO_ERROR_GENERIC)
    {
        cyw43_arch_enable_sta_mode();
   //     cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_STA, true, g_Country_Code);
        cyw43_hal_get_mac(0, mac);
        return true;
    }
    else
    {
        return false;
    }
}
bool Wifi::Enable()
{
    bool isEnabled =
        (cyw43_arch_wifi_connect_timeout_ms(
             networkConfiguration.initialSSID,
             networkConfiguration.initialSSIDPassword,
             WIFI_AUTH_TYPE,
             WIFI_CONNECT_TIMEOUT) == PICO_OK);

    CreatePollingThread();
    return isEnabled;
}
bool Wifi::Send(uint8_t *buffer, int length)
{
    int status = cyw43_send_ethernet(&cyw43_state, 0, length, buffer, false);
    if (status != 0)
    {
        return false;
    }
    return true;
}
bool Wifi::Receive()
{
    return true;
}
bool Wifi::MultiCastJoin()
{
    return true;
}
bool Wifi::MultiCastLeave()
{
    return true;
}
bool Wifi::DriverStatus()
{
    return true;
}
bool Wifi::Disconnect(HAL_Configuration_Wireless80211 wifiConfig)
{
    cyw43_arch_deinit();
    return true;
}
bool Wifi::Dispose()
{
    cyw43_arch_deinit();
    return true;
}

bool Wifi::WifiUp()
{
    // Only supports 'Client interface STA mode'             -
    int result = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    return (result == 1);
}
bool Wifi::StartScan()
{
    cyw43_wifi_scan_options_t scan_options = {0};
    int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_results_callback);
    return (err == 1);
}
bool Wifi::ScanActive()
{
    return cyw43_wifi_scan_active(&cyw43_state);
}

bool Wifi::GetAccessPointMaxStations()
{
    // At the time of writing this code
    // The CYW43 wireless driver in the Pico SDK does not support multiple simultaneous access points.
    return 1;
}
bool Wifi::GetAccessPointMaxAssociations(int station)
{
    // At the time of writing this code
    // The CYW43 wireless driver in the Pico SDK does not support multiple simultaneous access points.
    // Ignore station number

    // Get maximum number of associated clients
    int num_stas;
    cyw43_wifi_ap_get_max_stas(&cyw43_state, &num_stas);
    return num_stas;
}
bool Wifi::GetAccessPointInformation(int station, int number_associations, access_point_connected_clients_t *apInfo)
{
    // At the time of writing this code
    // The CYW43 wireless driver in the Pico SDK does not support multiple simultaneous access points.
    // The CYW43 firmware and driver do not expose per-client RSSI metrics when the device is acting as an AP.

    uint8_t *list_of_macs = (uint8_t *)platform_malloc(number_associations * sizeof(access_point_connected_clients_t));
    int num_stas;
    cyw43_wifi_ap_get_stas(&cyw43_state, &num_stas, list_of_macs);

    for (int i = 0; i < number_associations; i++)
    {
        memcpy(apInfo->mac, list_of_macs, 6);
        apInfo->phyModes = WIRELESS_PROTOCOL(-1);
        apInfo->rssi = 0;
        apInfo++;
        list_of_macs += 6;
    }
    platform_free(list_of_macs);
    return true;
}
void Wifi::Connect(HAL_Configuration_Wireless80211 wifiConfig)
{
    size_t ssid_len = 64;
    const uint8_t *ssid = wifiConfig.Ssid;
    size_t key_len = 32;
    const uint8_t *key = wifiConfig.Password;
    const uint32_t channel = CYW43_CHANNEL_NONE;

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

    cyw43_wifi_join(&cyw43_state, ssid_len, ssid, key_len, key, AuthenticationType, NULL, channel);
    Events_Set(SYSTEM_EVENT_FLAG_WIFI_STATION);
}
int Wifi::NumberOfScanReportEntries()
{
    return 1;
}
bool Wifi::GetScanReport(ScanReportRecordMatchesManagedCode *reportEntries)
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

// Overrides of TCP/IP stack callbacks customised for NetXDuo
int cyw43_tcpip_link_status(cyw43_t *self, int itf)
{
    if (nx_driver_pico_w_link_up)
        return CYW43_LINK_UP;
    else
        return cyw43_wifi_link_status(self, itf);
}
void cyw43_cb_tcpip_init(cyw43_t *self, int itf)
{
    if (!nx_driver_pico_w_link_up)
        return;
    nx_driver_information.nx_driver_information_ip_ptr->nx_ip_driver_link_up = NX_TRUE;
    _nx_ip_driver_link_status_event(
        nx_driver_information.nx_driver_information_ip_ptr,
        nx_driver_information.nx_driver_information_interface->nx_interface_index);
}
void cyw43_cb_tcpip_deinit(cyw43_t *self, int itf)
{
    if (!nx_driver_pico_w_link_up)
        return;
    nx_driver_information.nx_driver_information_ip_ptr->nx_ip_driver_link_up = NX_FALSE;
    _nx_ip_driver_link_status_event(
        nx_driver_information.nx_driver_information_ip_ptr,
        nx_driver_information.nx_driver_information_interface->nx_interface_index);
}
void cyw43_cb_tcpip_set_link_up(cyw43_t *self, int itf)
{
    nx_driver_pico_w_link_up = NX_TRUE;
}
void cyw43_cb_tcpip_set_link_down(cyw43_t *self, int itf)
{
    nx_driver_pico_w_link_up = NX_FALSE;
}
void cyw43_cb_process_ethernet(void *cb_data, int itf, size_t len, const uint8_t *buf)
{
    NX_PACKET *packet_ptr;
    if (!nx_driver_pico_w_link_up)
        return;

    /* Allocate a packet to receive data. */
    if (nx_packet_allocate(
            nx_driver_information.nx_driver_information_packet_pool_ptr,
            &packet_ptr,
            NX_RECEIVE_PACKET,
            NX_NO_WAIT))
    {

        /* No packet available. Just return. */
        return;
    }

    /* Adjust packet starting address to align IP header to 4 bytes boundary*/
    packet_ptr->nx_packet_prepend_ptr += 2;
    packet_ptr->nx_packet_append_ptr += 2;

    /* Append data to packet_ptr. */
    if (nx_packet_data_append(
            packet_ptr,
            (VOID *)buf,
            len,
            nx_driver_information.nx_driver_information_packet_pool_ptr,
            NX_NO_WAIT))
    {

        /* Error, release the packet. */
        nx_packet_release(packet_ptr);
        return;
    }

    /* Everything is OK, transfer the packet to NetX.  */
    nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, packet_ptr);
}
