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
#include "stm32h7xx_hal_pwr.h"
#include "tx_api.h"
#include "tx_port.h"

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

#define FRAME_BUFFER_ADDRESS 0x70000000
#define ENABLE_LTDC          LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_LTDC)
#define LTDC_FORCE_RESET     LL_APB3_GRP1_ForceReset(LL_APB3_GRP1_PERIPH_LTDC)
#define LTDC_RELEASE_RESET   LL_APB3_GRP1_ReleaseReset(LL_APB3_GRP1_PERIPH_LTDC)

#define DMA2D_ENABLE        LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_DMA2D)
#define DMA2D_RESET         LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_DMA2D)
#define DMA2D_RELEASE_RESET LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_DMA2D)

#define USE_GPIOA
#define USE_GPIOB
#define USE_GPIOC
#define USE_GPIOD
#define USE_GPIOE
#define USE_GPIOG
#define USE_GPIOH

// LCD 16-bit interface pins
#define LCD_R0_PORT           GPIOE
#define LCD_R0                LL_GPIO_PIN_0
#define LCD_R0_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R1_PORT           GPIOH
#define LCD_R1                LL_GPIO_PIN_3
#define LCD_R1_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R2_PORT           GPIOH
#define LCD_R2                LL_GPIO_PIN_8
#define LCD_R2_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R3_PORT           GPIOH
#define LCD_R3                LL_GPIO_PIN_9
#define LCD_R3_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R4_PORT           GPIOH
#define LCD_R4                LL_GPIO_PIN_10
#define LCD_R4_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R5_PORT           GPIOH
#define LCD_R5                LL_GPIO_PIN_11
#define LCD_R5_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R6_PORT           GPIOE
#define LCD_R6                LL_GPIO_PIN_1
#define LCD_R6_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R7_PORT           GPIOE
#define LCD_R7                LL_GPIO_PIN_15
#define LCD_R7_ALTERNATE_MODE LL_GPIO_AF_14

#define LCD_G0_PORT           GPIOB
#define LCD_G0                LL_GPIO_PIN_1
#define LCD_G0_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G1_PORT           GPIOB
#define LCD_G1                LL_GPIO_PIN_0
#define LCD_G1_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G2_PORT           GPIOA
#define LCD_G2                LL_GPIO_PIN_6
#define LCD_G2_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G3_PORT           GPIOE
#define LCD_G3                LL_GPIO_PIN_11
#define LCD_G3_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G4_PORT           GPIOH
#define LCD_G4                LL_GPIO_PIN_15
#define LCD_G4_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G5_PORT           GPIOH
#define LCD_G5                LL_GPIO_PIN_4
#define LCD_G5_ALTERNATE_MODE LL_GPIO_AF_9
#define LCD_G6_PORT           GPIOC
#define LCD_G6                LL_GPIO_PIN_7
#define LCD_G6_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G7_PORT           GPIOD
#define LCD_G7                LL_GPIO_PIN_3
#define LCD_G7_ALTERNATE_MODE LL_GPIO_AF_14

#define LCD_B0_PORT           GPIOG
#define LCD_B0                LL_GPIO_PIN_14
#define LCD_B0_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B1_PORT           GPIOD
#define LCD_B1                LL_GPIO_PIN_0
#define LCD_B1_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B2_PORT           GPIOD
#define LCD_B2                LL_GPIO_PIN_6
#define LCD_B2_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B3_PORT           GPIOA
#define LCD_B3                LL_GPIO_PIN_8
#define LCD_B3_ALTERNATE_MODE LL_GPIO_AF_13
#define LCD_B4_PORT           GPIOE
#define LCD_B4                LL_GPIO_PIN_12
#define LCD_B4_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B5_PORT           GPIOA
#define LCD_B5                LL_GPIO_PIN_3
#define LCD_B5_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B6_PORT           GPIOB
#define LCD_B6                LL_GPIO_PIN_8
#define LCD_B6_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B7_PORT           GPIOB
#define LCD_B7                LL_GPIO_PIN_9
#define LCD_B7_ALTERNATE_MODE LL_GPIO_AF_14

#define LCD_BL_CTRL_PORT GPIOG
#define LCD_BL_CTRL_PIN  LL_GPIO_PIN_15

#define LCD_DISP_EN_PORT GPIOE
#define LCD_DISP_EN_PIN  LL_GPIO_PIN_13

#define LCD_DISP_CTRL_PIN  LL_GPIO_PIN_10
#define LCD_DISP_CTRL_PORT GPIOD

#define LCD_HSYNC_PORT GPIOC
#define LCD_HSYNC      LL_GPIO_PIN_6

#define LCD_VSYNC_PORT GPIOA
#define LCD_VSYNC      LL_GPIO_PIN_4

#define LCD_CLK_PORT GPIOG
#define LCD_CLK      LL_GPIO_PIN_7

#define LCD_INT_Port
#define LCD_INT_Pin

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


#ifdef __cplusplus
extern "C"
{
#endif

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

void Startup_Rtos(bool debuggerRequested);
void Initialize_Board();
void InitializeBoardPeripherals();
void Initialize_Board_LEDS();
void Initialize_Board_Buttons();
void InitializeDevicePins();
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
void BoardLed_ON(GPIO_TypeDef *gpio_port, uint32_t led);
void BoardLed_OFF(GPIO_TypeDef *gpio_port, uint32_t led);
void BoardLed_Toggle(GPIO_TypeDef *gpio_port, uint32_t led);
bool BoardUserButton_Pressed();
static inline uint32_t Get_SYSTICK();

    void LTDCClock_Config(void);
#ifdef __cplusplus
}
#endif
