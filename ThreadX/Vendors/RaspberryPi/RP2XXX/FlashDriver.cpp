// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL.h>
#include "FlashDriver.h"
#include "hardware/flash.h"
#include "memory.h"
#include <string.h>

bool FlashDriver_InitializeDevice(void *context)
{
    (void)context;
    // nothing to do here
    return true;
}
bool FlashDriver_UninitializeDevice(void *context)
{
    (void)context;
    // nothing to do here
    return true;
}
DeviceBlockInfo *FlashDriver_GetDeviceInfo(void *context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;
    return config->BlockConfig.BlockDeviceInformation;
}
bool FlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
{
    (void)context;
    // Read each byte, if it fails the MCU will fault and not return
    uint8_t *cursor = (uint8_t *)startAddress;
    uint8_t *endAddress = (uint8_t *)(startAddress + numBytes);
    while (cursor < endAddress)
    {
        *buffer++ = *cursor++;
    }
    return true;
}
UpdateConfigurationResult FlashDriver_WriteConfig(unsigned char *configData, int length, int offset)
{
    // Erase the config block in flash which is a multiple of the flash block size, then write back the whole flash
    // block with the updated config block content from RAM
    UpdateConfigurationResult status = UpdateConfigurationResult_Failed;
    uint8_t *configSectorCopy = (uint8_t *)platform_malloc(__nanoConfig_size__);
    if (configSectorCopy != NULL)
    {
        // Copy the complete reserved config flash to RAM, update the RAM version
        // and then write it back to flash. 
        memcpy(configSectorCopy, &__nanoConfig_start__, __nanoConfig_size__);
        memcpy(configSectorCopy + offset, configData, length);

        if (FlashDriver_EraseBlock(NULL, (ByteAddress)&__nanoConfig_start__))
        {
            if (FlashDriver_Write(
                    NULL,
                    (uint32_t)&__nanoConfig_start__,
                    __nanoConfig_size__,
                    (unsigned char *)configSectorCopy,
                    true))
            {
                status = UpdateConfigurationResult_Success;
            }
        }
    } 
    return status;
}

bool FlashDriver_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;

    // TODO : If core1 running need to stop executing XIP as it causes issues

    uint32_t flashOffset = startAddress - XIP_BASE;
    int numberOfPages = numBytes / FLASH_PAGE_SIZE;

    GLOBAL_LOCK()
    {
        // By design, all data is aligned and multiple of Flash page size,
        // except the remaining bytes at the end of the stream
        if (numBytes < FLASH_PAGE_SIZE)
        {
            flash_range_program(flashOffset, buffer, numBytes);
        }
        else
        {
            for (int iPage = 0; iPage < numberOfPages; iPage++)
            {
                flash_range_program(flashOffset, buffer, FLASH_PAGE_SIZE);
                flashOffset += FLASH_PAGE_SIZE;
                buffer += FLASH_PAGE_SIZE;
            }
        }
    }
    GLOBAL_UNLOCK()

    return true;
}
bool FlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;
    // Assume multiple of 4 bytes for block size, which is a reasonable assumption for 32-bit MCU
    for (int i = 0; i < length / 4; ++i)
    {
        volatile uint32_t xx = *(uint32_t *)blockAddress;
        if (*(uint32_t *)blockAddress != 0xFFFFFFFF)
        {
            return false;
        }
        blockAddress += 4;
    }
    return true;
}
// On the RP2XXX using WINBOND flash, sector sizes are 4096 bytes with support to erase in 4KB, 32KB or 64KB erase
// sizes. All operations are aligned to, and in multiples of 4096 bytes
bool FlashDriver_EraseBlock(void *context, ByteAddress address)
{
    (void)context;
    // TODO : If core1 running need to stop executing XIP as it causes issues
    uint32_t flashOffset = address - XIP_BASE;
    GLOBAL_LOCK()
    {
        flash_range_erase(flashOffset, FLASH_ERASE_SIZE * 1024);
    }
    GLOBAL_UNLOCK()
    return true;
}
bool FlashDriver_Memset(void *, ByteAddress startAddress, unsigned char data, unsigned int numBytes)
{
    return false;
}
