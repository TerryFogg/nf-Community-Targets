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

#include "stm32h743xx.h"
#include "stm32h7xx.h"
#include "stm32h7xx_ll.h"
#include "stm32h7xx_hal_pwr.h"
#include "tx_api.h"
#include "tx_port.h"

#define TARGET_BLOCKSTORAGE_COUNT 1

#pragma region Byte pool configuration and definitions
#define DEFAULT_BYTE_POOL_SIZE     16000
#define CLR_THREAD_STACK_SIZE      6000
#define RECEIVER_THREAD_STACK_SIZE 4096
#pragma endregion

#pragma region Peripheral clocks definitions
#define ENABLE_CLOCK_ON_PORT_GPIOA LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA)
#define ENABLE_CLOCK_ON_PORT_GPIOB LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB)
#define ENABLE_CLOCK_ON_PORT_GPIOC LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC)
#define ENABLE_CLOCK_ON_PORT_GPIOD LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD)
#define ENABLE_CLOCK_ON_PORT_GPIOE LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE)
#define ENABLE_CLOCK_ON_PORT_GPIOF LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOF)
#define ENABLE_CLOCK_ON_PORT_GPIOG LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG)
#define ENABLE_CLOCK_ON_PORT_GPIOH LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOH)
#define ENABLE_CLOCK_ON_USART3     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)
#define ENABLE_CLOCK_ON_DMA1       LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#pragma endregion

#pragma region Wire protocol USART
#define wpUSART_DMA_Receive_Buffer_size 2048
#define wpBAUD_RATE                     921600

#define wpUSART                         USART3
#define wpUSART_IRQn                    USART3_IRQn
#define wpUSART_IRQHANDLER()            void USART3_IRQHandler(void)
#define wpUSART_PERIPHERAL_CLOCK_ENABLE ENABLE_CLOCK_ON_USART3

#define wpUSART_GPIO_PORT                    GPIOD
#define wpUSART_RX_PIN                       LL_GPIO_PIN_8
#define wpUSART_TX_PIN                       LL_GPIO_PIN_9
#define wpUSART_GPIO_PERIPHERAL_CLOCK_ENABLE ENABLE_CLOCK_ON_PORT_GPIOD

#define wpDMA                               DMA1
#define wpDMA_ReceiveStreamInterrupt        DMA1_Stream0_IRQn
#define wpDMA_TransmitStreamInterrupt       DMA1_Stream1_IRQn
#define wpUSART_DMA_PERIPHERAL_CLOCK_ENABLE ENABLE_CLOCK_ON_DMA1

#define wpDMA_ReceiveStream              LL_DMA_STREAM_0
#define wpDMA_ReceiveMux                 LL_DMAMUX1_REQ_USART3_RX
#define wpDMA_ReceiveStream_IRQHandler() void DMA1_Stream0_IRQHandler(void)

#define wpDMA_TransmitStream              LL_DMA_STREAM_1
#define wpDMA_TransmitMux                 LL_DMAMUX1_REQ_USART3_TX
#define wpDMA_TransmitStream_IRQHandler() void DMA1_Stream1_IRQHandler(void)

#pragma endregion

#pragma region Local board buttons and leds
// GPIOB
#define LED1_GREEN LL_GPIO_PIN_0
#define LED3_RED   LL_GPIO_PIN_14

// GPIOE
#define LED2_YELLOW LL_GPIO_PIN_1

// GPIOC
#define BUTTON_USER_PIN       LL_GPIO_PIN_13
#define BUTTON_USER_GPIO_PORT GPIOC

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
void Initialize_DWT_Counter();
void Initialize_Board_LEDS_And_Buttons();
void Initialize_SDRAM(uint32_t base_address, uint32_t sdram_region_size);
void Initialize_64bit_timer();
void CPU_CACHE_Enable(void);
void SystemClock_Config();
void BoardLed_ON(GPIO_TypeDef *gpio_port, uint32_t led);
void BoardLed_OFF(GPIO_TypeDef *gpio_port, uint32_t led);
void BoardLed_Toggle(GPIO_TypeDef *gpio_port, uint32_t led);
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
