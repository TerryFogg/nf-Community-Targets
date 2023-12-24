//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include "FlashDriver_Stm32h7.h"

// map here the Block Storage Interface to the STM32 driver
IBlockStorageDevice STM32Flash_BlockStorageInterface =
{                          
    &STM32FlashDriver_InitializeDevice,
    &STM32FlashDriver_UninitializeDevice,
    &STM32FlashDriver_GetDeviceInfo,
    &STM32FlashDriver_Read,
    &STM32FlashDriver_Write,
    NULL,
    &STM32FlashDriver_IsBlockErased,    
    &STM32FlashDriver_EraseBlock,
    NULL,
    NULL
};
