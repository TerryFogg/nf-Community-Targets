// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "NetworkConfigurationManager.h"
#include <cstring>

NetworkConfiguration networkConfiguration;

bool ReadNetworkConfiguration()
{
    networkConfiguration.useDHCP = true;
    networkConfiguration.useICMP = true;
    networkConfiguration.initialIP = IP_ADDRESS(0, 0, 0, 0);
    networkConfiguration.initialGateway = IP_ADDRESS(0, 0, 0, 0);
    strncpy(networkConfiguration.ssid, "WiFi-848E3B_Ext", sizeof(networkConfiguration.ssid) - 1);
    strncpy(networkConfiguration.password, "cloudydiamond351", sizeof(networkConfiguration.password) - 1);
    return true;
}
