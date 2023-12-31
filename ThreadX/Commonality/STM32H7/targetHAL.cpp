//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>
#include <nanoHAL_Time.h>
#include <nanoHAL_Types.h>
#include <target_platform.h>
#include <nanoPAL_Events.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>
#include <nanoHAL_Graphics.h>
#include <nanoHAL_v2.h>
#include "board.h"

extern bool g_waitForDebuggerRequested;
bool g_fDoNotUninitializeDebuggerPort = false;

extern "C"
{
    extern void Initialize_Graphics(void);
}

extern "C"
{
    void nanoHAL_Initialize_C()
    {
        nanoHAL_Initialize();
    }

    void nanoHAL_Uninitialize_C(bool isPoweringDown)
    {
        nanoHAL_Uninitialize(isPoweringDown);
    }
}

void nanoHAL_Initialize()
{
    // initialize global mutex
    // chMtxObjectInit(&interpreterGlobalMutex);

#if (NANOCLR_GRAPHICS == TRUE)
    Initialize_Graphics();
#endif

    PalEvent_Initialize();

    HAL_CONTINUATION::InitializeList();
    HAL_COMPLETION::InitializeList();

    BlockStorageList_Initialize();
    BlockStorage_AddDevices();
    BlockStorageList_InitializeDevices();
    unsigned char *heapStart = NULL;
    unsigned int heapSize = 0;

    ::HeapLocation(heapStart, heapSize);
    memset(heapStart, 0, heapSize);

    // ConfigurationManager_Initialize();

    Events_Initialize();

    CPU_GPIO_Initialize();

    // Initialise Network Stack
    Network_Initialize();
}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
    (void)isPoweringDown;
    SOCKETS_CloseConnections();
    BlockStorageList_UnInitializeDevices();
    CPU_GPIO_Uninitialize();
    Events_Uninitialize();
    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION::Uninitialize();
}

void HAL_AssertEx()
{
    __asm("BKPT #0\n");
    while (true)
    {
        __NOP();
    }
}
