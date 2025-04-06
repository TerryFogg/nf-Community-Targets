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
#include "SD_DoubleBufferedDMA.h"

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
    DisplayInterfaceConfig display_config;
    display_config.Screen.width = LCD_WIDTH;
    display_config.Screen.height = LCD_HEIGHT;

#if (INTERFACE_SPI == TRUE)
    display_config.spiBus = SPI_BUS;
    display_config.chipSelect = SPI_CS;
    display_config.dataCommand = LDC_DC;
    display_config.reset = LCD_RESET;
    display_config.backLight = LCD_BACKLIGHT;
#endif

#if (INTERFACE_VIDEO == TRUE)
    display_config.VideoDisplay.Frequency_Divider = VIDEO_FREQUENCY_DIVIDER;
    display_config.VideoDisplay.enable = LCD_ENABLE;
    display_config.VideoDisplay.control = LCD_CONTROL;
    display_config.VideoDisplay.backlight = LCD_BACKLIGHT;
    display_config.VideoDisplay.Horizontal_synchronization = VIDEO_HORIZONTAL_SYNC;
    display_config.VideoDisplay.Horizontal_back_porch = VIDEO_HORIZONTAL_BACK_PORCH;
    display_config.VideoDisplay.Horizontal_front_porch = VIDEO_HORIZONTAL_FRONT_PORCH;
    display_config.VideoDisplay.Vertical_synchronization = VIDEO_VERTICAL_SYNC;
    display_config.VideoDisplay.Vertical_back_porch = VIDEO_VERTICAL_BACK_PORCH;
    display_config.VideoDisplay.Vertical_front_porch = VIDEO_VERTICAL_FRONT_PORCH;
#endif

    //  TouchInterfaceConfig touch_config;
    //    touch_config.i2c_touch_screen_bus_initialize = NULL;
    //  touch_config.Address = 0x0070;
    // touch_config.I2c_bus_number = 0;

    //  g_TouchInterface.Initialize(touch_config);
    //  g_TouchDevice.Initialize(display_config.Screen.width, display_config.Screen.height,
    //  TranslateCoordinates::SWAP_X_Y); g_TouchPanelDriver.Initialize();

    g_GraphicsMemoryHeap.Initialize(0);
    g_DisplayInterface.Initialize(display_config);
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

    PalEvent_Initialize();

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
        __NOP();
    }
}
