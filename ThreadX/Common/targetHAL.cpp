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
#include <nanoHAL_Graphics.h>
#include <nanoHAL_v2.h>
#include "board.h"

bool g_fDoNotUninitializeDebuggerPort = false;

#if (TOUCH_DISPLAY_SUPPORT == TRUE)
#include "TouchPanel.h"
#include "TouchInterface.h"
extern TouchPanel g_TouchPanel;
extern TouchInterface g_TouchInterface;
extern TouchDevice g_TouchDevice;
#endif

void nanoHAL_Initialize()
{
    unsigned char *heapStart = NULL;
    unsigned int heapSize = 0;

    HAL_CONTINUATION::InitializeList();
    HAL_COMPLETION::InitializeList();

    HeapLocation(heapStart, heapSize);
    memset(heapStart, 0, heapSize);
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();
    BlockStorageList_InitializeDevices();
    Events_Initialize();
    SetupPinList();

#if (NANOCLR_GRAPHICS == TRUE)
    DisplayInterfaceConfig displayConfig = {0};
    g_GraphicsMemoryHeap.Initialize(0);
    g_DisplayInterface.Initialize(displayConfig);
    g_DisplayDriver.Initialize();
#endif

#if (TOUCH_DISPLAY_SUPPORT == TRUE)
    g_TouchInterface.Initialize(TOUCH_INTERFACE_BUS, TOUCH_INTERFACE_SLAVE_ADDRESS);
    g_TouchDevice.Initialize(
        TOUCH_INTERFACE_INTERRUPT,
        TOUCH_INTERFACE_WIDTH,
        TOUCH_INTERFACE_HEIGHT,
        TOUCH_INVERT_X,
        TOUCH_INVERT_Y);
    g_TouchPanel.Initialize();
#endif

}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
    (void)isPoweringDown;
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

bool Target_CanChangeMacAddress()
{
    return false;
}


void ConfigurationManager_GetOemModelSku(char *model, size_t modelSkuSize)
{
    memset(model, 0, modelSkuSize);
}

void ConfigurationManager_GetModuleSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    memset(serialNumber, 0, serialNumberSize);
}

void ConfigurationManager_GetSystemSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    memset(serialNumber, 0, serialNumberSize);
}
