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

#if (TOUCH_DISPLAY_SUPPORT == TRUE)
#include "TouchPanel.h"
#include "TouchInterface.h"
extern TouchPanel g_TouchPanel;
extern TouchInterface g_TouchInterface;
extern TouchDevice g_TouchDevice;
#endif

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
    unsigned char *heapStart = NULL;
    unsigned int heapSize = 0;

    InitializeBoardPeripherals();

#if (NANOCLR_GRAPHICS == TRUE)
    DisplayInterfaceConfig displayConfig = {0};
    g_GraphicsMemoryHeap.Initialize(0);
    g_DisplayInterface.Initialize(displayConfig);
    g_DisplayDriver.Initialize();
#endif

#if (TOUCH_DISPLAY_SUPPORT == TRUE)
    g_TouchInterface.Initialize();
    g_TouchDevice.Initialize();
    g_TouchPanel.Initialize();
#endif

    HAL_CONTINUATION::InitializeList();
    HAL_COMPLETION::InitializeList();

    BlockStorageList_Initialize();
    BlockStorage_AddDevices();
    BlockStorageList_InitializeDevices();

    ::HeapLocation(heapStart, heapSize);
    memset(heapStart, 0, heapSize);

    Events_Initialize();
    Network_Initialize();
}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
    (void)isPoweringDown;
    SOCKETS_CloseConnections();
    BlockStorageList_UnInitializeDevices();

    Events_Uninitialize();
    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION::Uninitialize();
}

void HAL_AssertEx()
{
    __asm("BKPT #0\n");
    while (true)
    {
        /*nop*/
    }
}
