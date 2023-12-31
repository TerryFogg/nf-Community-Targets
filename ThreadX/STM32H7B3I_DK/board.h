#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
// The following includes the specific STM32H7 chip defines
#include <nanoHAL_v2.h>
#include <nanoCLR_Headers.h>

#include "stm32h7xx.h"
#include "stm32h7xx_ll.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7b3xx.h"
#include "tx_api.h"
#include "tx_port.h"

// byte pool configuration and definitions
#define DEFAULT_BYTE_POOL_SIZE     16000
#define CLR_THREAD_STACK_SIZE      6000
#define RECEIVER_THREAD_STACK_SIZE 4096

#define TARGET_BLOCKSTORAGE_COUNT 1

//--------------------------------------
//
// Display setup
//

// LCD Display control pin
#define LCD_DISP_CTRL_PIN                LL_GPIO_PIN_2
#define LCD_DISP_CTRL_GPIO_PORT          GPIOA

// LCD Display enable pin
#define LCD_DISP_EN_PIN                LL_GPIO_PIN_7
#define LCD_DISP_EN_GPIO_PORT          GPIOK

// Back-light control pin
#define LCD_BL_CTRL_PIN                LL_GPIO_PIN_1
#define LCD_BL_CTRL_GPIO_PORT          

// LCD 16-bit interface pins
#define LCD_R0 LL_GPIO_PIN_2  // PE2
#define LCD_R1 LL_GPIO_PIN_2  // PE2
#define LCD_R2 LL_GPIO_PIN_15 // PE15
#define LCD_R3 LL_GPIO_PIN_8  // PD8
#define LCD_R4 LL_GPIO_PIN_9  // PD9
#define LCD_G0 LL_GPIO_PIN_14 // PF14
#define LCD_G1 LL_GPIO_PIN_15 // PF15
#define LCD_G2 LL_GPIO_PIN_9  // PE9
#define LCD_G3 LL_GPIO_PIN_10 // PE10
#define LCD_G4 LL_GPIO_PIN_11 // PE11
#define LCD_G5 LL_GPIO_PIN_12 // PE12
#define LCD_B0 LL_GPIO_PIN_4  // PE4
#define LCD_B1 LL_GPIO_PIN_13 // PF13
#define LCD_B2 LL_GPIO_PIN_14 // PD14
#define LCD_B3 LL_GPIO_PIN_15 // PD15
#define LCD_B4 LL_GPIO_PIN_0  // PD0


// Board Leds and buttons
#define LED_GPIO_PORT GPIOC
#define LED_GREEN     LL_GPIO_PIN_3
#define LED_RED       LL_GPIO_PIN_2

#define BUTTON_USER_GPIO_PORT GPIOC
#define BUTTON_USER_PIN       LL_GPIO_PIN_13

#define NANOCLR_AUDIO              FALSE
#define NANOCLR_ETHERNET           FALSE
#define NANOCLR_FDCAN              FALSE

#define NANOCLR_GRAPHICS_USING_SPI FALSE
#define NANOCLR_MICROSD            FALSE
#define NANOCLR_RTC                FALSE
#define NANOCLR_USB                FALSE

void Startup_Rtos();

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

