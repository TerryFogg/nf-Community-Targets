#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdbool.h>
#include <nanoPAL_BlockStorage.h>
#include "board.h"

#ifdef __cplusplus
extern "C"
{
#endif
    bool FlashDriver_InitializeDevice(void *);
    bool FlashDriver_UninitializeDevice(void *);
    DeviceBlockInfo *FlashDriver_GetDeviceInfo(void *);
    bool FlashDriver_Read(void *, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer);
    bool FlashDriver_Write(
        void *,
        ByteAddress startAddress,
        unsigned int numBytes,
        unsigned char *buffer,
        bool readModifyWrite);
    bool FlashDriver_IsBlockErased(void *, ByteAddress blockAddress, unsigned int length);
    bool FlashDriver_EraseBlock(void *, ByteAddress address);
    bool FlashUnlock();
    bool FlashLock();
    void WriteFlashWord(uint32_t bank, uint32_t *aligned_dest_addr, uint32_t *buffer_256bits);
    bool WaitForLastOperation( );
    uint32_t GetPage(uint32_t Address);
    uint32_t GetBank(uint32_t Address);

#ifdef __cplusplus
}
#endif
