
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.

#include <nanoHAL_v2.h>
#include <stdbool.h>

extern uint32_t NATIVE_RAM;


#define RAM1_MEMORY_StartAddress   ((uint32_t)0x20000000)
#define RAM1_MEMORY_Size           ((uint32_t)0x00040000)
#define FLASH1_MEMORY_StartAddress ((uint32_t)0x10000000)
#define FLASH1_MEMORY_Size         ((uint32_t)0x00200000)

HAL_SYSTEM_CONFIG HalSystemConfig = {
	{true}, // HAL_DRIVER_CONFIG_HEADER Header;
	1,		// ConvertCOM_DebugHandle(1),
	0,		// ConvertCOM_DebugHandle(0),
	921600,
	0,		// STDIO = COM2 or COM1
	{RAM1_MEMORY_StartAddress, RAM1_MEMORY_Size},
	{FLASH1_MEMORY_StartAddress, FLASH1_MEMORY_Size} };


  HAL_TARGET_CONFIGURATION g_TargetConfiguration;
