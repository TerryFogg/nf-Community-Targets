// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "NetworkConfigurationManager.h"
#include <cstring>

extern NetworkConfiguration networkConfiguration;

bool ReadNetworkConfiguration()
{
    networkConfiguration.initialIP = IP_ADDRESS(0, 0, 0, 0);
    networkConfiguration.initialGateway = IP_ADDRESS(0, 0, 0, 0);
    strncpy(networkConfiguration.initialSSID, "WiFi-848E3B", sizeof(networkConfiguration.initialSSID) - 1);
    strncpy(
        networkConfiguration.initialSSIDPassword,
        "cloudydiamond351",
        sizeof(networkConfiguration.initialSSIDPassword) - 1);
    return true;
}
