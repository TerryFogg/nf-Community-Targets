//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "GraphicsMemoryHeap.h"
#include "board.h"
#include <nanoCLR_Headers.h>
#include <nanoHAL_Graphics.h>

extern TouchPanelDriver g_TouchPanelDriver;

extern "C"
{
    void Initialize_Graphics()
    {
        // Initialize graphics ram heap size to all available as defined in the memory map
        g_GraphicsMemoryHeap.Initialize(0);

        DisplayInterfaceConfig display_config;
        display_config.Screen.width = 480;
        display_config.Screen.height = 480;

        display_config.VideoDisplay.Frequency_Divider = 5;
        display_config.VideoDisplay.enable = LL_GPIO_PIN_13;
        display_config.VideoDisplay.control = LL_GPIO_PIN_10;
        display_config.VideoDisplay.backlight = LL_GPIO_PIN_15;
        display_config.VideoDisplay.Horizontal_synchronization = 41;
        display_config.VideoDisplay.Horizontal_back_porch = 13;
        display_config.VideoDisplay.Horizontal_front_porch = 32;
        display_config.VideoDisplay.Vertical_synchronization = 10;
        display_config.VideoDisplay.Vertical_back_porch = 2;
        display_config.VideoDisplay.Vertical_front_porch = 2;
        g_DisplayInterface.Initialize(display_config);
        g_DisplayDriver.Initialize();

        //  TouchInterfaceConfig touch_config;
        //    touch_config.i2c_touch_screen_bus_initialize = NULL;
        //  touch_config.Address = 0x0070;
        // touch_config.I2c_bus_number = 0;

        //  g_TouchInterface.Initialize(touch_config);
        //  g_TouchDevice.Initialize(display_config.Screen.width, display_config.Screen.height,
        //  TranslateCoordinates::SWAP_X_Y); g_TouchPanelDriver.Initialize();
    }
}
