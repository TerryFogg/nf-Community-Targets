#pragma once

// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.

#include <nanoHAL_v2.h>
#include <nanoCLR_Headers.h>

#include "target_platform.h"

// For the pico-sdk
#include "pico.h"

#ifdef NETWORKING_SUPPORT
#include "boards/pico2_w.h"
#elif
#include "boards/pico2.h"
#endif

#include "hardware/adc.h"
#include <hardware/gpio.h>
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "hardware/sync.h"
#include "hardware/timer.h"
#include "hardware/uart.h"

#include "pico/binary_info/code.h"
#include "pico/binary_info.h"
#include "pico/i2c_slave.h"
#include "pico/stdlib.h"

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

// Board Leds and buttons
#define LED_PIN       PICO_DEFAULT_LED_PIN
#define LED_STATE_OFF 0
#define LED_STATE_ON  1

#ifdef __cplusplus
extern "C"
{
#endif
    void Initialize_Board();
    void SetupPinList();
    void Initialize64BitMicrosecondTimer();
    void InitializeDevicePins();
    uint64_t ReadMicrosecondCounter();
    void SystemClock_Config();

#ifdef FILE_SYSTEM_RAM
    bool File_System_RAM_Initialize();
#endif

#ifdef FILE_SYSTEM_FLASH
    bool File_System_FLASH_Initialize();
#endif

#ifdef FILE_SYSTEM_SD
    bool File_System_SD_Initialize();
#endif

#ifdef __cplusplus
}
#endif
