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

#include "stm32h735xx.h"
#include "stm32h7xx.h"
#include "stm32h7xx_ll.h"
#include "tx_api.h"
#include "tx_port.h"

#define TARGET_BLOCKSTORAGE_COUNT 1

#pragma region Byte pool configuration and definitions
#define DEFAULT_BYTE_POOL_SIZE     16000
#define CLR_THREAD_STACK_SIZE      6000
#define RECEIVER_THREAD_STACK_SIZE 4096
#pragma endregion

#pragma region Peripheral clocks
#define ENABLE_PORT_GPIOA LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA)
#define ENABLE_PORT_GPIOB LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB)
#define ENABLE_PORT_GPIOC LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC)
#define ENABLE_PORT_GPIOD LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD)
#define ENABLE_PORT_GPIOE LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE)
#define ENABLE_PORT_GPIOF LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOF)
#define ENABLE_PORT_GPIOG LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG)
#define ENABLE_PORT_GPIOH LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOH)
#pragma endregion

#pragma region Wire protocol USART
#define wpUSART_DMA_Receive_Buffer_size 2048

#define wpUSART_DMA_Receive_Buffer_size 2048

#define wpBAUD_RATE 921600

#define wpUSART USART3
#define wpUSART_IRQn USART3_IRQn
#define wpUSART_IRQHANDLER() void USART3_IRQHandler(void)
#define wpUSART_PERIPHERAL_CLOCK_ENABLE                                        \
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)

#define wpUSART_GPIO_PORT GPIOD
#define wpUSART_RX_PIN LL_GPIO_PIN_9
#define wpUSART_TX_PIN LL_GPIO_PIN_8
#define wpUSART_GPIO_PERIPHERAL_CLOCK_ENABLE                                   \
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD)

#define wpDMA DMA1
#define wpDMA_ReceiveStreamInterrupt DMA1_Stream0_IRQn
#define wpDMA_TransmitStreamInterrupt DMA1_Stream1_IRQn
#define wpUSART_DMA_PERIPHERAL_CLOCK_ENABLE                                    \
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)

#define wpDMA_ReceiveStream LL_DMA_STREAM_0
#define wpDMA_ReceiveMux LL_DMAMUX1_REQ_USART3_RX
#define wpDMA_ReceiveStream_IRQHandler() void DMA1_Stream0_IRQHandler(void)

#define wpDMA_TransmitStream LL_DMA_STREAM_1
#define wpDMA_TransmitMux LL_DMAMUX1_REQ_USART3_TX
#define wpDMA_TransmitStream_IRQHandler() void DMA1_Stream1_IRQHandler(void)
#define wpUSART_DMA_Receive_Buffer_size   2048

#define wpBAUD_RATE 921600

#define wpUSART                         USART3
#define wpUSART_IRQn                    USART3_IRQn
#define wpUSART_IRQHANDLER()            void USART3_IRQHandler(void)
#define wpUSART_PERIPHERAL_CLOCK_ENABLE LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)

#define wpUSART_GPIO_PORT                    GPIOD
#define wpUSART_RX_PIN                       LL_GPIO_PIN_9
#define wpUSART_TX_PIN                       LL_GPIO_PIN_8
#define wpUSART_GPIO_PERIPHERAL_CLOCK_ENABLE LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD)

#define wpDMA                               DMA1
#define wpDMA_ReceiveStreamInterrupt        DMA1_Stream0_IRQn
#define wpDMA_TransmitStreamInterrupt       DMA1_Stream1_IRQn
#define wpUSART_DMA_PERIPHERAL_CLOCK_ENABLE LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)

#define wpDMA_ReceiveStream              LL_DMA_STREAM_0
#define wpDMA_ReceiveMux                 LL_DMAMUX1_REQ_USART3_RX
#define wpDMA_ReceiveStream_IRQHandler() void DMA1_Stream0_IRQHandler(void)

#define wpDMA_TransmitStream              LL_DMA_STREAM_1
#define wpDMA_TransmitMux                 LL_DMAMUX1_REQ_USART3_TX
#define wpDMA_TransmitStream_IRQHandler() void DMA1_Stream1_IRQHandler(void)

#pragma endregion

#pragma region Display interface and controller setup parameters
#define ENABLE_LTDC         LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_LTDC)
#define LTDC_FORCE_RESET    LL_APB3_GRP1_ForceReset(LL_APB3_GRP1_PERIPH_LTDC)
#define LTDC_RELEASE_RESET  LL_APB3_GRP1_ReleaseReset(LL_APB3_GRP1_PERIPH_LTDC)
#define DMA2D_ENABLE        LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_DMA2D)
#define DMA2D_RESET         LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_DMA2D)
#define DMA2D_RELEASE_RESET LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_DMA2D)

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
#define I2C_ENABLE_TOUCH_CLOCK        LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_I2C4)
#define I2C_TOUCH_FORCE_RESET         LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_I2C4)
#define I2C_TOUCH_FORCE_RELEASE_RESET LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_I2C4)
#pragma endregion

#pragma region Local board buttons and leds
#define LED_GPIO_PORT         GPIOC
#define LED_GREEN             LL_GPIO_PIN_3
#define LED_RED               LL_GPIO_PIN_2
#define BUTTON_USER_GPIO_PORT GPIOC
#define BUTTON_USER_PIN       LL_GPIO_PIN_13
#pragma endregion

#pragma region Flash SOC parameters and onboard external flash parameters

// FLASH_CR_PSIZE is a bit field in the FLASH_CR1 register of STM32 microcontrollers.
// It is used to configure the parallelism size of the flash memory interface
// The value of FLASH_CR_PSIZE determines the number of bytes to be programmed
// each time a write operation occurs to the flash memory
//  However,it is important to note that the existence of FLASH_CR_PSIZE bits in FLASH_CR1 register
//  is conditional and varies across different STM32 microcontroller models.
// For example, the STM32H7B3 microcontroller does not have FLASH_CR_PSIZE bits in its FLASH_CR1 register.
//

// Flash program/erase by 64 bits
#define FLASH_VOLTAGE_RANGE_4 FLASH_CR_PSIZE
#define FLASH_ERASE_SIZE      FLASH_VOLTAGE_RANGE_4

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

#define INVALIDATE_DCACHE SCB_CleanInvalidateDCache()

#define NANOCLR_AUDIO    FALSE
#define NANOCLR_ETHERNET FALSE
#define NANOCLR_FDCAN    FALSE

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

#ifdef __cplusplus
extern "C"
{
#endif
    void LTDCClock_Config(void);
#ifdef __cplusplus
}
#endif
