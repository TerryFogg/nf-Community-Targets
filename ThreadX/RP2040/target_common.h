#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <target_os.h>

// RAM base address
#define RAM1_MEMORY_StartAddress        ((uint32_t)0x20000000)
// RAM size 
#define RAM1_MEMORY_Size                ((uint32_t)0x00040000)

// FLASH base address
#define FLASH1_MEMORY_StartAddress      ((uint32_t)0x10000000)
// FLASH size
#define FLASH1_MEMORY_Size              ((uint32_t)0x00200000)

#define TARGETNAMESTRING "RASPBERRYPI_PICO"
#define PLATFORMNAMESTRING "RP2040"
#define PLATFORM_HAS_RNG       TRUE

