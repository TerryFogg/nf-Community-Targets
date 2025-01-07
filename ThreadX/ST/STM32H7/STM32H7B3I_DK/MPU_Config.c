//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "board.h"

void MPU_Config(uint32_t sdram_base_address, uint32_t sdram_region_size)
{
    LL_MPU_Disable();
    {
        // Configure the MPU as Strongly ordered for undefined regions
        // With the subregion value 0x87, the MPU will be enabled only for specific areas:
        // - On - chip peripheral address space
        // - External RAM
        // - Shared device space
        LL_MPU_ConfigRegion(
            LL_MPU_REGION_NUMBER0,
            0x87,
            0x00,
            LL_MPU_REGION_SIZE_4GB | 
            LL_MPU_REGION_NO_ACCESS | LL_MPU_ACCESS_NOT_BUFFERABLE | LL_MPU_ACCESS_NOT_CACHEABLE | 
            LL_MPU_ACCESS_SHAREABLE | LL_MPU_TEX_LEVEL0 | LL_MPU_INSTRUCTION_ACCESS_DISABLE);

        // Configure SDRAM as region 16MB for write through
        LL_MPU_ConfigRegion(
            LL_MPU_REGION_NUMBER1,
            0x00,
            sdram_base_address,
            sdram_region_size | LL_MPU_REGION_FULL_ACCESS | LL_MPU_ACCESS_NOT_BUFFERABLE | LL_MPU_ACCESS_CACHEABLE |
                LL_MPU_ACCESS_NOT_SHAREABLE | LL_MPU_TEX_LEVEL0 | LL_MPU_INSTRUCTION_ACCESS_ENABLE);
    }
    LL_MPU_Enable(LL_MPU_CTRL_PRIVILEGED_DEFAULT);
}