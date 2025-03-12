//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>
#include "FlashDriver.h"
#include "memory.h"

BlockStorageDevice Device_BlockStorage;

// Block Size is the selected erase size for the flash chip
const BlockRange BlockRange1[] = {
    {BlockRange_BLOCKTYPE_CODE, (uint32_t)(uint32_t *)&native_block_start, (uint32_t)(uint32_t *)&native_block_end},
    {BlockRange_BLOCKTYPE_DEPLOYMENT,
     (uint32_t)(uint32_t *)&deployment_block_start,
     (uint32_t)(uint32_t *)&deployment_block_end},
    {BlockRange_BLOCKTYPE_CONFIG, (uint32_t)(uint32_t *)&config_block_start, (uint32_t)(uint32_t *)&config_block_end}};

const BlockRegionInfo BlockRegions[] = {
    {
        (BlockRegionAttribute_ProgramWidthIs128bits),
        (uint32_t)(uint32_t *)&native_flash_start_address, // start address for block region
        (uint32_t)(uint32_t *)&total_flash_blocks,         // total number of blocks in this region
        (uint32_t)(uint32_t *)&erase_block_size,           // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange1),                 // size
        BlockRange1,                                       // Address
    },
};

const DeviceBlockInfo Device_BlockInfo = {
    (MediaAttribute_SupportsXIP),       // Flash memory is XIP
    2,                                  // ??????????  UINT32 BytesPerSector
    ARRAYSIZE_CONST_EXPR(BlockRegions), // UINT32 NumRegions;
    (BlockRegionInfo *)BlockRegions,    // const BlockRegionInfo* pRegions;
};

MEMORY_MAPPED_NOR_BLOCK_CONFIG Device_BlockStorageConfig = {
    {
        // BLOCK_CONFIG
        {
            0,     // GPIO_PIN             Pin;
            false, // BOOL                 ActiveState;
        },
        (DeviceBlockInfo *)&Device_BlockInfo, // BlockDeviceinfo
    },
    {
        // CPU_MEMORY_CONFIG
        0,                                       // UINT8  CPU_MEMORY_CONFIG::ChipSelect;
        true,                                    // UINT8  CPU_MEMORY_CONFIG::ReadOnly;
        0,                                       // UINT32 CPU_MEMORY_CONFIG::WaitStates;
        0,                                       // UINT32 CPU_MEMORY_CONFIG::ReleaseCounts;
        16,                                      // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        (uint32_t)(uint32_t *)&flash_start,      // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        (uint32_t)(uint32_t *)&total_flash_size, // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,                                       // UINT8  CPU_MEMORY_CONFIG::XREADYEnable
        0,                                       // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead
        0,                                       // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },
    0, // UINT32 ChipProtection;
    0, // UINT32 ManufacturerCode;
    0, // UINT32 DeviceCode;
};

// map here the Block Storage Interface
IBlockStorageDevice Flash_BlockStorageInterface = {
    &FlashDriver_InitializeDevice,
    &FlashDriver_UninitializeDevice,
    &FlashDriver_GetDeviceInfo,
    &FlashDriver_Read,
    &FlashDriver_Write,
    &FlashDriver_Memset,
    &FlashDriver_IsBlockErased,
    &FlashDriver_EraseBlock,
    NULL,
    NULL};

void BlockStorage_AddDevices()
{
    BlockStorageList_AddDevice(
        (BlockStorageDevice *)&Device_BlockStorage,
        &Flash_BlockStorageInterface,
        &Device_BlockStorageConfig,
        false);
}
