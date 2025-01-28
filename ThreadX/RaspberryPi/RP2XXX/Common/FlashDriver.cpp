﻿// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetHAL.h>
#include "FlashDriver.h"
#include "hardware/flash.h"
#include <tx_port.h>
#include "pico/multicore.h"

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
bool FlashDriver_Read(void *context, ByteAddress startAddress,
                      unsigned int numBytes, unsigned char *buffer)
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
bool FlashDriver_Write(void *context, ByteAddress startAddress,
                       unsigned int numBytes, unsigned char *buffer,
                       bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;

    // TODO : If core1 running need to stop executing XIP as it causes issuew

    uint32_t flashOffset = startAddress - XIP_BASE;
    int numberOfPages = numBytes / FLASH_PAGE_SIZE;
    
    GLOBAL_LOCK()
    
    // By design, all data is aligned and multiple of Flash page size, 
    // except the remaining bytes as the end of the stream
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

    GLOBAL_UNLOCK()
    
    return true;
}
bool FlashDriver_IsBlockErased(void *context, ByteAddress blockAddress,
                               unsigned int length)
{
    return false;
}
// On the RP2XXX, sector sizes are 4096 bytes and all operations are aligned to and in multiples of 4096 bytes
// with support to erase in 4KB, 32KB or 64KB erase sizes
// The support comes from the WINBOND flash chips used

bool FlashDriver_EraseBlock(void *context, ByteAddress address)
{
 
  // TODO : If core1 running need to stop executing XIP as it causes issues
  
  uint32_t flashOffset = address - XIP_BASE;

  GLOBAL_LOCK()

  // Use 4KB size blocks
  flash_range_erase(flashOffset, FLASH_SECTOR_SIZE);

  // Use 64KB size blocks
//  flash_range_erase(flashOffset, FLASH_BLOCK_SIZE);

  GLOBAL_UNLOCK()

  return true;
}
