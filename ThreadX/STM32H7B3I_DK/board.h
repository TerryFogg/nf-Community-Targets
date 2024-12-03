#pragma once

// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
#include <stdbool.h>

#include "tx_api.h"
#include "tx_port.h"
#include "target_platform.h"
#include "stm32h7xx.h"
#include "stm32h7xx_ll.h"

#include "stm32h7b3xxq.h"
#include "PinNames.h"

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

#pragma region Byte pool configuration and definitions
#define DEFAULT_BYTE_POOL_SIZE     16000
#define CLR_THREAD_STACK_SIZE      4096
#define RECEIVER_THREAD_STACK_SIZE 4096
#pragma endregion

#pragma region Peripheral clocks
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
#define ENABLE_CLOCK_ON_USART1     LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)
#define ENABLE_CLOCK_ON_DMA1       LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define ENABLE_CLOCK_ON_CRC        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC)
#pragma endregion

#pragma region Wire protocol USART
#define wpUSART_DMA_Receive_Buffer_size 2048
#define wpBAUD_RATE                     921600

#define wpUSART                         USART1
#define wpUSART_IRQn                    USART1_IRQn
#define wpUSART_IRQHANDLER()            void USART1_IRQHandler(void)
#define wpUSART_PERIPHERAL_CLOCK_ENABLE ENABLE_CLOCK_ON_USART1

#define wpUSART_GPIO_PORT                    GPIOA
#define wpUSART_RX_PIN                       LL_GPIO_PIN_10
#define wpUSART_TX_PIN                       LL_GPIO_PIN_9
#define wpUSART_GPIO_PERIPHERAL_CLOCK_ENABLE ENABLE_CLOCK_ON_PORT_GPIOA

#define wpDMA                               DMA1
#define wpDMA_ReceiveStreamInterrupt        DMA1_Stream0_IRQn
#define wpDMA_TransmitStreamInterrupt       DMA1_Stream1_IRQn
#define wpUSART_DMA_PERIPHERAL_CLOCK_ENABLE ENABLE_CLOCK_ON_DMA1

#define wpDMA_ReceiveStream              LL_DMA_STREAM_0
#define wpDMA_ReceiveMux                 LL_DMAMUX1_REQ_USART1_RX
#define wpDMA_ReceiveStream_IRQHandler() void DMA1_Stream0_IRQHandler(void)

#define wpDMA_TransmitStream              LL_DMA_STREAM_1
#define wpDMA_TransmitMux                 LL_DMAMUX1_REQ_USART1_TX
#define wpDMA_TransmitStream_IRQHandler() void DMA1_Stream1_IRQHandler(void)

#pragma endregion

#pragma region Display interface and controller setup parameters
#define LCD_WIDTH  480
#define LCD_HEIGHT 272

#define FRAME_BUFFER_ADDRESS 0xD0000000
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
#define USE_GPIOF
#define USE_GPIOG
#define USE_GPIOH
#define USE_GPIOI
#define USE_GPIOJ
#define USE_GPIOK

// LCD 16-bit interface pins
#define LCD_R0_PORT           GPIOI
#define LCD_R0                LL_GPIO_PIN_15
#define LCD_R0_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R1_PORT           GPIOJ
#define LCD_R1                LL_GPIO_PIN_0
#define LCD_R1_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R2_PORT           GPIOJ
#define LCD_R2                LL_GPIO_PIN_1
#define LCD_R2_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R3_PORT           GPIOJ
#define LCD_R3                LL_GPIO_PIN_2
#define LCD_R3_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R4_PORT           GPIOJ
#define LCD_R4                LL_GPIO_PIN_3
#define LCD_R4_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R5_PORT           GPIOJ
#define LCD_R5                LL_GPIO_PIN_4
#define LCD_R5_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R6_PORT           GPIOJ
#define LCD_R6                LL_GPIO_PIN_5
#define LCD_R6_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_R7_PORT           GPIOJ
#define LCD_R7                LL_GPIO_PIN_6
#define LCD_R7_ALTERNATE_MODE LL_GPIO_AF_14

#define LCD_G0_PORT           GPIOJ
#define LCD_G0                LL_GPIO_PIN_7
#define LCD_G0_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G1_PORT           GPIOJ
#define LCD_G1                LL_GPIO_PIN_8
#define LCD_G1_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G2_PORT           GPIOJ
#define LCD_G2                LL_GPIO_PIN_9
#define LCD_G2_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G3_PORT           GPIOJ
#define LCD_G3                LL_GPIO_PIN_10
#define LCD_G3_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G4_PORT           GPIOJ
#define LCD_G4                LL_GPIO_PIN_11
#define LCD_G4_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G5_PORT           GPIOK
#define LCD_G5                LL_GPIO_PIN_0
#define LCD_G5_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G6_PORT           GPIOK
#define LCD_G6                LL_GPIO_PIN_1
#define LCD_G6_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_G7_PORT           GPIOK
#define LCD_G7                LL_GPIO_PIN_2
#define LCD_G7_ALTERNATE_MODE LL_GPIO_AF_14

#define LCD_B0_PORT           GPIOJ
#define LCD_B0                LL_GPIO_PIN_12
#define LCD_B0_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B1_PORT           GPIOJ
#define LCD_B1                LL_GPIO_PIN_13
#define LCD_B1_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B2_PORT           GPIOJ
#define LCD_B2                LL_GPIO_PIN_14
#define LCD_B2_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B3_PORT           GPIOJ
#define LCD_B3                LL_GPIO_PIN_15
#define LCD_B3_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B4_PORT           GPIOK
#define LCD_B4                LL_GPIO_PIN_3
#define LCD_B4_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B5_PORT           GPIOK
#define LCD_B5                LL_GPIO_PIN_4
#define LCD_B5_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B6_PORT           GPIOK
#define LCD_B6                LL_GPIO_PIN_5
#define LCD_B6_ALTERNATE_MODE LL_GPIO_AF_14
#define LCD_B7_PORT           GPIOK
#define LCD_B7                LL_GPIO_PIN_6
#define LCD_B7_ALTERNATE_MODE LL_GPIO_AF_14

#define LCD_BL_CTRL_PORT GPIOA
#define LCD_BL_CTRL_PIN  LL_GPIO_PIN_1

#define LCD_DISP_EN_PORT GPIOK
#define LCD_DISP_EN_PIN  LL_GPIO_PIN_7

#define LCD_DISP_CTRL_PORT GPIOA
#define LCD_DISP_CTRL_PIN  LL_GPIO_PIN_2

#define LCD_HSYNC_PORT GPIOI
#define LCD_HSYNC      LL_GPIO_PIN_12

#define LCD_VSYNC_PORT GPIOI
#define LCD_VSYNC      LL_GPIO_PIN_13

#define LCD_CLK_PORT GPIOI
#define LCD_CLK      LL_GPIO_PIN_14

#define LCD_INT_Port // GPIOH
#define LCD_INT_Pin  // LL_GPIO_PIN_2

// Low level I2C for Display touch controller
#define I2C_ENABLE_TOUCH_CLOCK        LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_I2C4)
#define I2C_TOUCH_FORCE_RESET         LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_I2C4)
#define I2C_TOUCH_FORCE_RELEASE_RESET LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_I2C4)
#pragma endregion

#pragma region Local board buttons and leds
#define LED_GPIO_PORT GPIOG
#define LED3_RED      LL_GPIO_PIN_11
#define LED2_BLUE     LL_GPIO_PIN_2

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
#define FLASH_ERASE_SIZE FLASH_VOLTAGE_RANGE_4

// End Of Program on Bank 1 flag
// #define FLASH_FLAG_EOP_BANK1 FLASH_SR_EOP
#pragma endregion

#pragma region SDRAM setup
#define SDRAM_BANK_ADDR       ((uint32_t)0xD0000000)
#define SDRAM_RAM_REGION_SIZE LL_MPU_REGION_SIZE_32MB

#define A0_GPIO_Port  GPIOF
#define A0_Pin        LL_GPIO_PIN_0
#define A1_GPIO_Port  GPIOF
#define A1_Pin        LL_GPIO_PIN_1
#define A2_GPIO_Port  GPIOF
#define A2_Pin        LL_GPIO_PIN_2
#define A3_GPIO_Port  GPIOF
#define A3_Pin        LL_GPIO_PIN_3
#define A4_GPIO_Port  GPIOF
#define A4_Pin        LL_GPIO_PIN_4
#define A5_GPIO_Port  GPIOF
#define A5_Pin        LL_GPIO_PIN_5
#define A6_GPIO_Port  GPIOF
#define A6_Pin        LL_GPIO_PIN_12
#define A7_GPIO_Port  GPIOF
#define A7_Pin        LL_GPIO_PIN_13
#define A8_GPIO_Port  GPIOF
#define A8_Pin        LL_GPIO_PIN_14
#define A9_GPIO_Port  GPIOF
#define A9_Pin        LL_GPIO_PIN_15
#define A10_GPIO_Port GPIOG
#define A10_Pin       LL_GPIO_PIN_0
#define A11_GPIO_Port GPIOG
#define A11_Pin       LL_GPIO_PIN_1
#define A14_GPIO_Port GPIOG
#define A14_Pin       LL_GPIO_PIN_4
#define A15_GPIO_Port GPIOG
#define A15_Pin       LL_GPIO_PIN_5
#define D0_GPIO_Port  GPIOD
#define D0_Pin        LL_GPIO_PIN_14
#define D1_GPIO_Port  GPIOD
#define D1_Pin        LL_GPIO_PIN_15
#define D2_GPIO_Port  GPIOD
#define D2_Pin        LL_GPIO_PIN_0
#define D3_GPIO_Port  GPIOD
#define D3_Pin        LL_GPIO_PIN_1
#define D4_GPIO_Port  GPIOE
#define D4_Pin        LL_GPIO_PIN_7
#define D5_GPIO_Port  GPIOE
#define D5_Pin        LL_GPIO_PIN_8
#define D6_GPIO_Port  GPIOE
#define D6_Pin        LL_GPIO_PIN_9
#define D7_GPIO_Port  GPIOE
#define D7_Pin        LL_GPIO_PIN_10
#define D8_GPIO_Port  GPIOE
#define D8_Pin        LL_GPIO_PIN_11
#define D9_GPIO_Port  GPIOE
#define D9_Pin        LL_GPIO_PIN_12
#define D10_GPIO_Port GPIOE
#define D10_Pin       LL_GPIO_PIN_13
#define D11_GPIO_Port GPIOE
#define D11_Pin       LL_GPIO_PIN_14
#define D12_GPIO_Port GPIOE
#define D12_Pin       LL_GPIO_PIN_15
#define D13_GPIO_Port GPIOD
#define D13_Pin       LL_GPIO_PIN_8
#define D14_GPIO_Port GPIOD
#define D14_Pin       LL_GPIO_PIN_9
#define D15_GPIO_Port GPIOD
#define D15_Pin       LL_GPIO_PIN_10

#define FMC_NBL0_GPIO_Port GPIOE
#define FMC_NBL0_Pin       LL_GPIO_PIN_0
#define FMC_NBL1_GPIO_Port GPIOE
#define FMC_NBL1_Pin       LL_GPIO_PIN_1

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
// static inline bool OSPI2_WaitUntilState(uint32_t Flag, FlagStatus State)
//{
//     int loopCounter = 10;                         // 10 loops, 10 microseconds maximum
//     while (READ_BIT(OCTOSPI2->SR, Flag) != State) // Wait until flag is in expected state
//     {
//         DWT_Delay_us(1); // Wait a microsecond;
//         loopCounter--;
//         if (loopCounter == 0)
//         {
//             return false;
//         }
//     }
//     return true;
// }
#pragma endregion

#pragma region Prototypes

#define INVALIDATE_DCACHE SCB_CleanInvalidateDCache()
    void Startup_Rtos(bool debuggerRequested);
    void Initialize_Board();
    void InitializeBoardPeripherals();
    void InitializeDevicePins();
    void Initialize_DWT_Counter();
    void Initialize_Board_LEDS();
    void Initialize_Board_Buttons();
    void Initialize_SDRAM(uint32_t base_address, uint32_t sdram_region_size);
    void Initialize_64bit_timer();
    void CPU_CACHE_Enable(void);
    void MPU_Config(uint32_t sdram_base_address, uint32_t sdram_region_size);
    void SystemClock_Config();
    void BoardLed_ON(GPIO_TypeDef *gpio_port, uint32_t led);
    void BoardLed_OFF(GPIO_TypeDef *gpio_port, uint32_t led);
    void BoardLed_Toggle(GPIO_TypeDef *gpio_port, uint32_t led);
    bool BoardUserButton_Pressed();
    void PeriphCommonClock_Config(void);
    void LTDCClock_Config(void);

    uint64_t Get100nsTimerValue();
    void Initialize64bit100nsTimer();

#ifdef __cplusplus
}
#endif

#pragma endregion
