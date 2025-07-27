// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "System.Device.Network.h"

HAL_TARGET_CONFIGURATION g_TargetConfiguration;

void ConfigurationManager_Initialize()
{
    memset((void *)&g_TargetConfiguration, 0, sizeof(g_TargetConfiguration));

    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
}


