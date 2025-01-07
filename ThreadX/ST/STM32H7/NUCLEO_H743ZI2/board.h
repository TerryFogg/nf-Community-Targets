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

#pragma region Device Input/output
enum ArduinoPin : int
{
    Analog_0 = 33,  // PC0
    Analog_1 = 115, // PH2
    Analog_2 = 252, // PA0_C
    Analog_3 = 253, // PA1_C
    Analog_4 = 254, // PC2_C
    Analog_5 = 255, // PC3_C
    D0 = 32,        // PB15
    D1 = 31,        // PB14
    D2 = 100,       // PG3
    D3 = 1,         // PA0
    D4 = 101,       // PG4
    D5 = 79,        // PE14
    D6 = 64,        // PD15
    D7 = 102,       // PG5
    D8 = 68,        // PE3
    D9 = 24,        // PB7
    D10 = 87,       // PF6
    D11 = 90,       // PF9 - SPI5_MOSI
    D12 = 89,       // PF8 - SPI5_MISO
    D13 = 88,       // PF7 - SPI5_SCK
    D14 = 96,       // PF15 - I2C4_SDA
    D15 = 95        // PF14 - I2C4_SCL
};
#define PINS_PER_PORT  16
#define GPIO_MAX_PIN   sizeof(GpioPorts) * PINS_PER_PORT
#define GPIO_PORT(pin) (GpioPorts[pin / PINS_PER_PORT])
#define GPIO_PIN(pin)  (pin - (pin % PINS_PER_PORT) * PINS_PER_PORT)

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
#define ENABLE_CLOCK_ON_PORT_GPIOI LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOI)
#define ENABLE_CLOCK_ON_PORT_GPIOJ LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOJ)
#define ENABLE_CLOCK_ON_PORT_GPIOK LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOK)
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
#define wpUSART_RX_PIN                       LL_GPIO_PIN_9
#define wpUSART_TX_PIN                       LL_GPIO_PIN_8
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

#pragma region Display interface and controller setup parameters
#define LCD_WIDTH  480
#define LCD_HEIGHT 272
#pragma endregion


#pragma region Local board buttons and leds

#define LD1_PIN  LL_GPIO_PIN_0
#define LD1_PORT GPIOB

#define LD2_PIN  LL_GPIO_PIN_1
#define LD2_PORT GPIOE

#define LD3_PIN  LL_GPIO_PIN_14
#define LD3_PORT GPIOB

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


void Startup_Rtos(bool debuggerRequested);
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

