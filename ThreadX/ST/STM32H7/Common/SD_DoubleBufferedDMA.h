#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "DeviceIO.h"

#define HAL_SD_MODULE_ENABLED true

#define SD1_BUSIndex 1
#define SD2_BUSIndex 2


#ifdef __cplusplus
extern "C"
{
#endif
    void InitializeSD_Bus(
        int SDBus_index,
        PinNameValue SDMMC_D0,
        PinNameValue SDMMC_D1,
        PinNameValue SDMMC_D2,
        PinNameValue SDMMC_D3,
        PinNameValue SDMMC_CLK,
        PinNameValue SDMMC_CMD,
        PinNameValue SDMMC_DETECT);
#ifdef __cplusplus
}
#endif
