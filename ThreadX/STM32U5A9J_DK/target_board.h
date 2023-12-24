#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "tx_port.h"
#include <nanoHAL_v2.h>
#include <nanoCLR_Headers.h>

#include "stm32h7xx.h"
#include "stm32h7xx_ll.h"
#include "stm32h743xx.h"

#define OEMSYSTEMINFOSTRING "nanoCLR running STM32U5A9J_DK"


//-----------------------
// Block Storagage
//-----------------------
// this device has 1 block storage devices

#define TARGET_BLOCKSTORAGE_COUNT 1

//--------------------------------------
// Display setup
//--------------------------------------
//
// LCD Display control pin
#define LCD_DISP_CTRL_PIN       LL_GPIO_PIN_10
#define LCD_DISP_CTRL_GPIO_PORT GPIOD

// LCD Display enable pin
#define LCD_DISP_EN_PIN       LL_GPIO_PIN_13
#define LCD_DISP_EN_GPIO_PORT GPIOE

// Back-light control pin
#define LCD_BL_CTRL_PIN       LL_GPIO_PIN_15
#define LCD_BL_CTRL_GPIO_PORT GPIOG

#define NANOCLR_AUDIO              FALSE
#define NANOCLR_ETHERNET           FALSE
#define NANOCLR_FDCAN              FALSE
#define NANOCLR_GRAPHICS_USING_SPI FALSE
#define NANOCLR_MICROSD            FALSE
#define NANOCLR_RTC                FALSE
#define NANOCLR_USB                FALSE
