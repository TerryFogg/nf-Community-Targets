#pragma once

// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.

#include <nanoHAL_v2.h>
#include <nanoCLR_Headers.h>

#include "tx_port.h"
#include "tx_api.h"
#include "target_platform.h"

#include "boards/pico2.h"

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

// byte pool configuration and definitions
#define DEFAULT_BYTE_POOL_SIZE     10000
#define CLR_THREAD_STACK_SIZE      3000
#define RECEIVER_THREAD_STACK_SIZE 5000

#ifdef __cplusplus
extern "C"
{
#endif
    void Startup_Rtos(bool debuggerRequested);
    void Initialize_Board();
    void SetupPinList();
    void Initialize64BitMicrosecondTimer();
    void InitializeDevicePins();
    uint64_t ReadMicrosecondCounter();
    void SystemClock_Config();
    void InitializePSRAM();

#ifdef __cplusplus
}
#endif
