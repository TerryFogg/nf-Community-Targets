#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "tx_port.h"

// The following includes the specific STM32H7 chip defines
#include "tx_port.h"
#include <nanoHAL_v2.h>
#include <nanoCLR_Headers.h>

#include "stm32h7xx.h"
#include "stm32h7xx_ll.h"
#include "stm32h7b3xx.h"

#define OEMSYSTEMINFOSTRING "nanoCLR running STM32H7B3I-DK"


//-----------------------
// Block Storagage
//-----------------------
// this device has 1 block storage devices

#define TARGET_BLOCKSTORAGE_COUNT 1

//--------------------------------------
//
// Display setup
//
// LCD Display control pin
#define LCD_DISP_CTRL_PIN                GPIO_PIN_2
#define LCD_DISP_CTRL_PULL               GPIO_NOPULL
#define LCD_DISP_CTRL_GPIO_PORT          GPIOA
#define LCD_DISP_CTRL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define LCD_DISP_CTRL_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()

// LCD Display enable pin
#define LCD_DISP_EN_PIN                GPIO_PIN_7
#define LCD_DISP_EN_PULL               GPIO_NOPULL
#define LCD_DISP_EN_GPIO_PORT          GPIOK
#define LCD_DISP_EN_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOK_CLK_ENABLE()
#define LCD_DISP_EN_GPIO_CLK_DISABLE() __HAL_RCC_GPIOK_CLK_DISABLE()

// Back-light control pin
#define LCD_BL_CTRL_PIN                GPIO_PIN_1
#define LCD_BL_CTRL_GPIO_PORT          GPIOA
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define LCD_BL_CTRL_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()

// Definition for LCD Timer used to control the Brightness
#define LCD_TIMx                 TIM2
#define LCD_TIMx_CLK_ENABLE()    __HAL_RCC_TIM2_CLK_ENABLE()
#define LCD_TIMx_CLK_DISABLE()   __HAL_RCC_TIM2_CLK_DISABLE()
#define LCD_TIMx_CHANNEL         TIM_CHANNEL_2
#define LCD_TIMx_CHANNEL_AF      GPIO_AF1_TIM2
#define LCD_TIMX_PERIOD_VALUE    ((uint32_t)200)
#define LCD_TIMX_PRESCALER_VALUE ((uint32_t)27)

#define NANOCLR_AUDIO              FALSE
#define NANOCLR_ETHERNET           FALSE
#define NANOCLR_FDCAN              FALSE
#define NANOCLR_GRAPHICS_USING_SPI FALSE
#define NANOCLR_MICROSD            FALSE
#define NANOCLR_RTC                FALSE
#define NANOCLR_USB                FALSE

void Initialize_Board();
void Initialize_OCTOSPI2_Hyperam();
void Initialize_OPSPI_Flash();
void Initialize_RTC();
void Initialize_DWT_Counter();
void USBD_Clock_Config(void);
void Initialize_Board_LEDS_And_Buttons();
void Initialize_64bit_timer();
void CPU_CACHE_Enable(void);
void MPU_Config(void);
void SystemClock_Config();
void Startup_Rtos();
void BoardLed_ON(uint32_t led);
void BoardLed_OFF(uint32_t led);
void BoardLed_Toggle(uint32_t led);
bool BoardUserButton_Pressed();
static inline uint32_t Get_SYSTICK();

// DWT is connected to the system clock
static inline void DWT_Delay_us(volatile uint32_t microsecond_delay)
{

    // TODO - At boot time not relevant, but Important  WRAP AROUND counter !!!! to fix
    LL_RCC_ClocksTypeDef RCC_Clocks;
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
    uint32_t initial_microseconds = DWT->CYCCNT;
    uint32_t tick_rate = RCC_Clocks.SYSCLK_Frequency / 1000000;
    microsecond_delay *= tick_rate;
    while ((DWT->CYCCNT - initial_microseconds) < microsecond_delay - tick_rate)
        ;
}

static inline uint32_t Get_SYSTICK()
{
    return tx_time_get();
}

static inline bool OSPI2_WaitUntilState(uint32_t Flag, FlagStatus State)
{
    int loopCounter = 10;                         // 10 loops, 10 microseconds maximum
    while (READ_BIT(OCTOSPI2->SR, Flag) != State) // Wait until flag is in expected state
    {
        DWT_Delay_us(1); // Wait a microsecond;
        loopCounter--;
        if (loopCounter == 0)
        {
            return false;
        }
    }
    return true;
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
// STM327B3I-DK board Leds
// ========================

// #define LED_GPIO_PORT GPIOC
// #define LED_GREEN     LL_GPIO_PIN_3
// #define LED_RED       LL_GPIO_PIN_2

// ===============================
// STM327B3I-DK board push button
// ===============================

// #define BUTTON_USER_GPIO_PORT GPIOC
// #define BUTTON_USER_PIN       LL_GPIO_PIN_13
