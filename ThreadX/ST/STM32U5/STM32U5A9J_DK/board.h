#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <nanoCLR_Headers.h>

#include "stm32u5a9xx.h"
#include "stm32u5xx.h"
#include "core_cm33.h"
#include "stm32u5xx_ll.h"
#include "tx_api.h"
#include "tx_port.h"
#include "core_cm33.h"
#include "stm32u5xx_hal_cortex.h"
#include "target_platform.h"

//#include <stdarg.h>
//#include <stdio.h>
//#include <nanoCLR_Interop.h>
////#include "stm32u5xx_hal_conf.h"
//#include "stm32u5xx_hal.h"

#define TARGET_BLOCKSTORAGE_COUNT 1

#pragma region Peripheral clocks
#define ENABLE_CLOCK_ON_PORT_GPIOA LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define ENABLE_CLOCK_ON_PORT_GPIOB LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define ENABLE_CLOCK_ON_PORT_GPIOC LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC)
#define ENABLE_CLOCK_ON_PORT_GPIOD LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD)
#define ENABLE_CLOCK_ON_PORT_GPIOE LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE)
#define ENABLE_CLOCK_ON_PORT_GPIOF LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF)
#define ENABLE_CLOCK_ON_PORT_GPIOG LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG)
#define ENABLE_CLOCK_ON_PORT_GPIOH LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH)
#define ENABLE_CLOCK_ON_PORT_GPIOI LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOI)
#pragma endregion

#pragma region Wire protocol USART
#define wpUSART_DMA_Receive_Buffer_size 2048

#define wpBAUD_RATE 921600

#define wpUSART                         USART1
#define wpUSART_IRQn                    USART1_IRQn
#define wpUSART_IRQHANDLER()            USART1_IRQHandler(void)
#define wpUSART_PERIPHERAL_CLOCK_ENABLE LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)

#define wpUSART_GPIO_PORT                    GPIOA
#define wpUSART_RX_PIN                       LL_GPIO_PIN_10
#define wpUSART_TX_PIN                       LL_GPIO_PIN_9
#define wpUSART_GPIO_PERIPHERAL_CLOCK_ENABLE ENABLE_CLOCK_ON_PORT_GPIOA

#define wpGPDMA                       GPDMA1
#define wpGDMA_TRANSMIT_CHANNEL       LL_DMA_CHANNEL_0
#define wpGPDMA_TRANSMIT_IRQ          GPDMA1_Channel0_IRQn
#define wpGPDMA_TRANSMIT_IRQHANDLER() GPDMA1_Channel0_IRQHandler()
#define wpGDMA_RECEIVE_CHANNEL        LL_DMA_CHANNEL_1
#define wpGPDMA_RECEIVE_IRQ           GPDMA1_Channel1_IRQn
#define wpGPDMA_RECEIVE_IRQHANDLER()  GPDMA1_Channel1_IRQHandler()

#define wpGPDMA_REQUEST LL_GPDMA1_REQUEST_USART1_TX

#pragma endregion

#pragma region Byte pool configuration and definitions
#define DEFAULT_BYTE_POOL_SIZE     16000
#define CLR_THREAD_STACK_SIZE      6000
#define RECEIVER_THREAD_STACK_SIZE 4096
#pragma endregion

#pragma region Display interface and controller setup parameters
#define DSI_RESET_PIN      LL_GPIO_PIN_5
#define DSI_RESET_PIN_PORT         GPIOD

#define DSI_BackLight_PIN          LL_GPIO_PIN_6
#define DSI_BackLight_PIN_PORT     GPIOI

#define LCD_WIDTH                  480
#define LCD_HEIGHT                 481
#define LCD_FRAME_BUFFER           0x200D0000
#define UTIL_LCD_COLOR_YELLOW      0xFFFFFF00UL
#define UTIL_LCD_COLOR_DARKRED     0xFF800000UL
#define UTIL_LCD_COLOR_BLACK       0xFF000000UL
#define UTIL_LCD_COLOR_BROWN       0xFFA52A2AUL
#define UTIL_LCD_COLOR_ORANGE      0xFFFFA500UL

#define DSI_DCS_SHORT_PKT_WRITE_P0 0x00000005U
#define DSI_DCS_SHORT_PKT_WRITE_P1 0x00000015U
#define DSI_DCS_LONG_PKT_WRITE     0x00000039U


#define ENABLE_LTDC LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_LTDC)

#define LTDC_FORCE_RESET   LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_LTDC)
#define LTDC_RELEASE_RESET LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_LTDC)

#define DMA2D_ENABLE        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2D)
#define DMA2D_RESET         LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_DMA2D)
#define DMA2D_RELEASE_RESET LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_DMA2D)

// LCD 16-bit interface pins
#define LCD_R0      LL_GPIO_PIN_0
#define LCD_R0_PORT GPIOE
#define LCD_R1      LL_GPIO_PIN_3
#define LCD_R1_PORT GPIOH
#define LCD_R2      LL_GPIO_PIN_8
#define LCD_R2_PORT GPIOH
#define LCD_R3      LL_GPIO_PIN_9
#define LCD_R3_PORT GPIOH
#define LCD_R4      LL_GPIO_PIN_10
#define LCD_R4_PORT GPIOH
#define LCD_R5      LL_GPIO_PIN_11
#define LCD_R5_PORT GPIOH
#define LCD_R6      LL_GPIO_PIN_1
#define LCD_R6_PORT GPIOE
#define LCD_R7      LL_GPIO_PIN_15
#define LCD_R7_PORT GPIOE

#define LCD_G0      LL_GPIO_PIN_1
#define LCD_G0_PORT GPIOB
#define LCD_G1      LL_GPIO_PIN_0
#define LCD_G1_PORT GPIOB
#define LCD_G2      LL_GPIO_PIN_6
#define LCD_G2_PORT GPIOA
#define LCD_G3      LL_GPIO_PIN_11
#define LCD_G3_PORT GPIOE
#define LCD_G4      LL_GPIO_PIN_15
#define LCD_G4_PORT GPIOH
#define LCD_G5      LL_GPIO_PIN_4
#define LCD_G5_PORT GPIOH
#define LCD_G6      LL_GPIO_PIN_7
#define LCD_G6_PORT GPIOC
#define LCD_G7      LL_GPIO_PIN_3
#define LCD_G7_PORT GPIOD

#define LCD_B0      LL_GPIO_PIN_14
#define LCD_B0_PORT GPIOG
#define LCD_B1      LL_GPIO_PIN_0
#define LCD_B1_PORT GPIOD
#define LCD_B2      LL_GPIO_PIN_6
#define LCD_B2_PORT GPIOD
#define LCD_B3      LL_GPIO_PIN_8
#define LCD_B3_PORT GPIOA
#define LCD_B4      LL_GPIO_PIN_12
#define LCD_B4_PORT GPIOE
#define LCD_B5      LL_GPIO_PIN_3
#define LCD_B5_PORT GPIOA
#define LCD_B6      LL_GPIO_PIN_8
#define LCD_B6_PORT GPIOB
#define LCD_B7      LL_GPIO_PIN_9
#define LCD_B7_PORT GPIOB

#define LCD_CLK        LL_GPIO_PIN_7
#define LCD_CLK_PORT   GPIOG
#define LCD_HSYNC      LL_GPIO_PIN_6
#define LCD_HSYNC_PORT GPIOC
#define LCD_VSYNC      LL_GPIO_PIN_4
#define LCD_VSYNC_PORT GPIOA

#define LCD_DISP_CTRL_PIN  LL_GPIO_PIN_10
#define LCD_DISP_CTRL_PORT GPIOD

#define LCD_DISP_EN_PIN  LL_GPIO_PIN_13
#define LCD_DISP_EN_PORT GPIOE

#define LCD_BL_CTRL_PIN  LL_GPIO_PIN_15
#define LCD_BL_CTRL_PORT GPIOG

// Low level I2C for Display touch controller
#define I2C_ENABLE_TOUCH_CLOCK        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_I2C4)
#define I2C_TOUCH_FORCE_RESET         LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_I2C4)
#define I2C_TOUCH_FORCE_RELEASE_RESET LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_I2C4)

////
#define TS_INT_PIN       GPIO_PIN_8  // PE8
#define TS_INT_EXTI_IRQn EXTI8_IRQn


#pragma endregion

#pragma region Local board buttons and leds
#define LED_GPIO_PORT         GPIOE
#define LED_GREEN             LL_GPIO_PIN_0
#define LED_RED               LL_GPIO_PIN_1
#define BUTTON_USER_PIN                LL_GPIO_PIN_13
#define BUTTON_USER_GPIO_PORT          GPIOC
#define BUTTON_USER_GPIO_CLK_ENABLE()  ENABLE_CLOCK_ON_PORT_GPIOC
#define BUTTON_USER_EXTI_IRQn          EXTI13_IRQn

#pragma endregion

#pragma region Flash SOC parameters and onboard external flash parameters

// FLASH_CR_PSIZE is a bit field in the FLASH_CR1 register of STM32 microcontrollers.
// It is used to configure the parallelism size of the flash memory interface ¹
// The value of FLASH_CR_PSIZE determines the number of bytes to be programmed
// each time a write operation occurs to the flash memory
//  However,it is important to note that the existence of FLASH_CR_PSIZE bits in FLASH_CR1 register
//  is conditional and varies across different STM32 microcontroller models.
// For example, the STM32H7B3 microcontroller does not have FLASH_CR_PSIZE bits in its FLASH_CR1 register.
//

#define FLASH_ERASE_SIZE FLASH_VOLTAGE_RANGE_4

// End Of Program on Bank 1 flag
// #define FLASH_FLAG_EOP_BANK1 FLASH_SR_EOP
#pragma endregion

#pragma region Inline functions
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
#pragma endregion

#define INVALIDATE_DCACHE

void BoardLed_ON(uint32_t led);
void BoardLed_OFF(uint32_t led);
void BoardLed_Toggle(uint32_t led);
bool BoardUserButton_Pressed();
void CPU_CACHE_Enable(void);
void Initialize_Board();
void Initialize_Board_LEDS_And_Buttons();
void Initialize_DWT_Counter();
void Initialize_OCTOSPI2_Hyperam();
void Initialize_OPSPI_Flash();
void Initialize_RTC();
void MPU_Config(void);
void Startup_Rtos(bool debuggerRequested);
void USBD_Clock_Config(void);
void Initialize_64bit_timer();



void SystemClock_Config();
void SystemPower_Config(void);
void PeriphCommonClock_Config(void);


static inline uint32_t Get_SYSTICK();

#ifdef __cplusplus
extern "C"
{
#endif
    void LTDCClock_Config(void);
    int GetCurrentMilliseconds();
    void InitOneMillisecondTick();

#ifdef __cplusplus
}
#endif
