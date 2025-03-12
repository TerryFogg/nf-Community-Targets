#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <target_os.h>

/////////////////////////////////////////////////////////////////////////////////////////
// The following addresses and sizes should be filled in according to the SoC data-sheet
// they also must be coherent with what's in the linker file for nanoBooter and nanoCLR

#define RAM1_MEMORY_StartAddress        ((uint32_t)0x24000000)
#define RAM1_MEMORY_Size                ((uint32_t)0x00100000)
#define FLASH1_MEMORY_StartAddress      ((uint32_t)0x08000000)
#define FLASH1_MEMORY_Size              ((uint32_t)0x00200000)
