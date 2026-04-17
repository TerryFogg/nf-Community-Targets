#pragma once

// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <nanoHAL.h>
#include <string.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct NetworkConfiguration
    {
        // bool enableDHCP;       // Whether to use DHCP or static IP
        // bool enableDNS;        // Whether to use DNS
        ULONG initialIP;              // Fixed IP address (if not using DHCP)
        ULONG initialGateway;         // Gateway IP address
        uint8_t macAddress[6];        // MAC address
        char initialSSID[64];         // WiFi SSID
        char initialSSIDPassword[64]; // WiFi password
    } NetworkConfiguration;

    bool ReadNetworkConfiguration();
    void NetworkStartup();

#ifdef __cplusplus
}
#endif
