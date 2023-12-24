//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "GraphicsMemoryHeap.h"
#include "targetHAL_board.h"
#include <hardware/spi.h>
#include <nanoCLR_Headers.h>
#include <nanoHAL_Graphics.h>
#include <target_BlockStorage.h>
#include <string.h>

extern DisplayInterface g_DisplayInterface;
extern "C" {
void Initialize_Graphics() 
{
  // Initialize graphics ram heap size to all available as defined in the
  // memory map
   g_GraphicsMemoryHeap.Initialize(0);

  DisplayInterfaceConfig displayConfig;


  #ifdef PICO_LCD_114
  // Index into array of pin values ( spiBus - 1) == 0

  displayConfig.Spi.spiBus = 1;
  displayConfig.Screen.width = 240;
  displayConfig.Screen.height = 135;
  displayConfig.Spi.chipSelect = 9;
  displayConfig.Spi.dataCommand = 8;
  displayConfig.Spi.backLight = 13;
  displayConfig.Spi.reset = 12;
#endif

#ifdef ROUND_DISPLAY
  // Index into array of pin values ( spiBus - 1) == 0
  displayConfig.Spi.spiBus = 1;
  clock = 10;
  MOSI = 11;
  displayConfig.Spi.chipSelect = 9;
  displayConfig.Spi.dataCommand = 8;
  displayConfig.Spi.reset = 12;
  displayConfig.Spi.backLight = 25;
  displayConfig.Screen.width = 240;
  displayConfig.Screen.height = 240;
#endif

#ifdef PICO_LCD_ERTFTM028
  // Index into array of pin values ( spiBus - 1) == 0

  memcpy(displayConfig.Name, "ERTFTM28", 8);
  displayConfig.Spi.spiBus = 0;
  clock = 18;
  MOSI = 19;
  displayConfig.Spi.chipSelect = 17;
  displayConfig.Spi.dataCommand = 16;
  displayConfig.Spi.reset = 27;
  displayConfig.Spi.backLight = 26;
  displayConfig.Screen.width = 320;
  displayConfig.Screen.height = 240;
#endif

  g_DisplayInterface.Initialize(displayConfig);
  g_DisplayDriver.Initialize();
}
}
