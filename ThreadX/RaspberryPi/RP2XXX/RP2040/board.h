#pragma once

// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.

#include <nanoHAL_v2.h>
#include <nanoCLR_Headers.h>

#include "tx_port.h"
#include "tx_api.h"
#include "target_platform.h"

#include "boards/pico.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"
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
#define RECEIVER_THREAD_STACK_SIZE 1800

#pragma region Display interface and controller setup parameters
#define LCD_WIDTH  240
#define LCD_HEIGHT 135
#pragma endregion

#ifdef __cplusplus
extern "C"
{
#endif
    void Startup_Rtos(bool debuggerRequested);
    void Initialize_Board();
    void ResetPinList();
    void Initialize64BitMicrosecondTimer();
    uint64_t ReadMicrosecondCounter();
    void InitializeDevicePins();
    void Initialize_Board_LEDS();
    void SystemClock_Config();
    void BoardLed_ON();
    void BoardLed_OFF();
    void BoardLed_Toggle();
    bool BoardUserButton_Pressed();
    bool PinSupportsADC(int pinNumber);

    static inline uint32_t Get_SYSTICK()
    {
        return tx_time_get();
    }
    void LTDCClock_Config(void);
#ifdef __cplusplus
}
#endif
