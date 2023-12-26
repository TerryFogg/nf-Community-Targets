#pragma once

// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.

#include <nanoHAL_v2.h>
#include <nanoCLR_Headers.h>
#include "tx_port.h"
#include "tx_api.h"
#include "boards/pico.h"


// Board Leds and buttons
#define LED_PIN       PICO_DEFAULT_LED_PIN
#define LED_STATE_OFF 0
#define LED_STATE_ON  1


// byte pool configuration and definitions
#define DEFAULT_BYTE_POOL_SIZE     10000
#define CLR_THREAD_STACK_SIZE      3000
#define RECEIVER_THREAD_STACK_SIZE 1800

#define TARGET_BLOCKSTORAGE_COUNT 1

void Startup_Rtos();


void Initialize_Board();
void InitializeSerialOverUSB();
void Initialize_RTC();
void Initialize_DWT_Counter();
void USBD_Clock_Config(void);
void Initialize_Board_LEDS_And_Buttons();
void Initialize_64bit_timer();
void CPU_CACHE_Enable(void);
void MPU_Config(void);
void SystemClock_Config();
void BoardLed_ON(uint32_t led);
void BoardLed_OFF(uint32_t led);
void BoardLed_Toggle(uint32_t led);
bool BoardUserButton_Pressed();
static inline uint32_t Get_SYSTICK();

// DWT is connected to the system clock
static inline void DWT_Delay_us(volatile uint32_t microsecond_delay)
{

    //// TODO - At boot time not relevant, but Important  WRAP AROUND counter !!!! to fix
    // LL_RCC_ClocksTypeDef RCC_Clocks;
    // LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
    // uint32_t initial_microseconds = DWT->CYCCNT;
    // uint32_t tick_rate = RCC_Clocks.SYSCLK_Frequency / 1000000;
    // microsecond_delay *= tick_rate;
    // while ((DWT->CYCCNT - initial_microseconds) < microsecond_delay - tick_rate)
    //     ;
}

static inline uint32_t Get_SYSTICK()
{
    return tx_time_get();
}

#ifdef __cplusplus
extern "C"
{
#endif
    void LTDCClock_Config(void);
#ifdef __cplusplus
}
#endif

// ========================
// PICO board Leds
// ========================

// ===============================
// PICO board push button
// ===============================
