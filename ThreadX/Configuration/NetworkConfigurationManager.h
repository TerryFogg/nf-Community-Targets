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
        bool useDHCP;          // Whether to use DHCP or static IP
        bool useICMP;          // Whether to enable ICMP (ping)
        ULONG initialIP;       // Fixed IP address (if not using DHCP)
        ULONG initialGateway;  // Gateway IP address
        char ssid[64];         // WiFi SSID
        char password[64];     // WiFi password
    } NetworkConfiguration;

    bool ReadNetworkConfiguration();

#ifdef __cplusplus
}
#endif
