//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "PinNames.h"
#include "DeviceIO.h"
#include "DeviceRegistration.h"
#include "board.h"
#include "SD_DoubleBufferedDMA.h"
#include "File_Drivers.h"
#include "Delays.h"

static DevicePin mcuPins[] = {
    // Arduino connector
    {PA0, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PA1, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1/ Arduino connector
    {PA4, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // EXT_I2C connector CN16
    {PA6, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1
    {PA11, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1/ Arduino connector
    {PA12, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Audio connector CN3
    {PB0, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1 (I2C1_SCL,AF4)
    {PB8, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1  (I2C1_SDA,AF4)/Audio connector CN3
    {PB9, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Audio connector CN3
    {PB12, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1/ Arduino connector
    {PB14, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1/ Arduino connector
    {PB15, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1/ Arduino connector
    {PC2, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1/ Arduino connector
    {PC3, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PC4, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1
    {PC6, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1/ Audio connector CN3
    {PC7, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1
    {PD3, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1/(I2C4_SCL) connector CN16/ Arduino connector
    {PD12, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1/(I2C4_SDA) connector CN16/ Arduino connector
    {PD13, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PE2, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1
    {PF8, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PF10, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // User Led LED2 - Blue
    {PG2, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // User Led LED1 - Red
    {PG11, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // STMOD+ connector P1
    {PH8, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PH9, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PH10, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PH11, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PH13, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PH14, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Audio connector CN3
    {PH15, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector/ Arduino connector
    {PI0, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Audio connector CN3
    {PI6, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PI7, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},
    // Arduino connector
    {PI9, false, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::NONE, 0},

    // SDMMC D0
    {PC8, true, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::SD, 0},
    // SDMMC D1
    {PC9, true, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::SD, 0},
    // SDMMC D2
    {PC10, true, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::SD, 0},
    // SDMMC D3
    {PC11, true, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::SD, 0},
    // SDMMC CLK
    {PC12, true, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::SD, 0},
    // SDMMC CMD
    {PC2, true, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::SD, 0},
    // SDMMC uSD detect
    {PI8, true, NULL, NULL, PinMode::PinMode_Input, DevicePinFunction::GPIO, 0}};

AdcPin BoardADCChannels[] = {{ADC1, 1, 1}, {ADC2, 11, 2}};
DacPin BoardDACChannels[] = {{DAC1, 1, 1}, {DAC2, 2, 2}};
I2cPin BoardI2CChannels[] = {{I2C1, 1, 4}, {I2C4, 1, 4}};
PwmPin BoardPWMChannels[] = {{TIM3, 3, 1}, {TIM4, 4, 1}};
SpiPin BoardSPIChannels[] = {{SPI1, 1, 1}, {SPI2, 1, 2}};
SerialPin BoardSerialChannels[] = {{USART1, 1, 1}, {USART2, 2, 2}};

static TX_SEMAPHORE txs;
static TX_SEMAPHORE rxs;

SDPin BoardSDChannels[] = {
    {.controllerId = SDMMC1,
     .controllerNumber = 1,
     .channelNumber = 1,
     .readType = File_Read_Type::FILE_NONE,
     .status = File_Status::FILE_READY,
     .RX_semaphore = rxs,
     .TX_semaphore = txs}};

void Initialize_Board()
{
    CPU_CACHE_Enable();
    SystemClock_Config();
    // Disabling FMC Bank1 ? To prevent this CortexM7  speculative read accesses on FMC bank1
    // it is recommended to disable it when it is not used
    FMC_Bank1_R->BTCR[0] &= ~FMC_BCRx_MBKEN;

    InitializeDelayCounter();
    Initialize64bit100nsTimer();
    Initialize_Board_LEDS();
    Initialize_Board_Buttons();
    Initialize_SDRAM(SDRAM_BANK_ADDR, SDRAM_RAM_REGION_SIZE);
    InitializeDevicePins();

    InitializeSD_Bus(
        SD1_BUSIndex,
        PC8,  // SDMMC D0
        PC9,  // SDMMC D1
        PC10, // SDMMC D2
        PC11, // SDMMC D3
        PC12, // SDMMC CLK
        PD2,  // SDMMC CMD
        PI8); // SDMMC uSD detect
}
void CPU_CACHE_Enable(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();
}
void Initialize_Board_LEDS()
{
    ENABLE_CLOCK_ON_PORT_GPIOG;

    LL_GPIO_InitTypeDef gpio_InitStruct;
    gpio_InitStruct.Pin = LED3_RED | LED2_BLUE;
    gpio_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    gpio_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOG, &gpio_InitStruct);
    LL_GPIO_SetOutputPin(GPIOG, LED3_RED);
    LL_GPIO_SetOutputPin(GPIOG, LED2_BLUE);
}
void Initialize_Board_Buttons()
{
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
    LL_GPIO_InitTypeDef gpio_InitStruct;
    gpio_InitStruct.Pin = BUTTON_USER_PIN;
    gpio_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    gpio_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(BUTTON_USER_GPIO_PORT, &gpio_InitStruct);
}
void Initialize64bit100nsTimer()
{

    // TODO : Not working yet
    // Although we setup a "nanosecond" timer, the real resolution is less than
    // 1ns. To get 1ns resolution would require a clock speed of 1GHz.
    //
    // For this implementation, 100ns is the smallest precision.

    // 32 bit counters used together to create a continuous 64 bit counter of
    // nanoseconds TIM 2  - the master, feeds TIM5 TIM 5  - the slave 2^64
    // nanosecond,  ~ 584 years before overflowing

    // In upcounting mode, the counter counts from 0 to the auto-reload value

    LL_RCC_ClocksTypeDef RCC_Clocks;
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);

    // Compute the prescaler value to have input to be 100ns pulses
    int32_t nanosecond100_per_second = 10000000;
    uint32_t prescaler_value = RCC_Clocks.SYSCLK_Frequency / nanosecond100_per_second;

    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);

    // In addition to all these clock sources, the timer should be clocked with
    // the APBx clock  (page 10. AN4013)
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetPrescaler(TIM2, prescaler_value);
    LL_TIM_EnableMasterSlaveMode(TIM2);

    // Update: the update event is selected as trigger output (TRGO).
    // This timer is used as a prescaler for a slave timer;
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_UPDATE);
    LL_TIM_DisableMasterSlaveMode(TIM2);

    LL_TIM_SetSlaveMode(TIM5, LL_TIM_SLAVEMODE_TRIGGER);
    LL_TIM_SetClockDivision(TIM5, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetCounterMode(TIM5, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetTriggerInput(TIM5, LL_TIM_TS_ITR0); // TRGO output from TIM2
    LL_TIM_EnableExternalClock(TIM5);

    TIM2->CNT = 0;
    TIM5->CNT = 0;
    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableCounter(TIM5);
}
uint64_t Get100nsTimerValue()
{
    return (int64_t)LL_TIM_GetCounter(TIM5) << 32 | LL_TIM_GetCounter(TIM2);
}
void BoardLed_OFF(GPIO_TypeDef *gpio_port, uint32_t led)
{
    LL_GPIO_ResetOutputPin(gpio_port, led);
};
void BoardLed_ON(GPIO_TypeDef *gpio_port, uint32_t led)
{
    LL_GPIO_SetOutputPin(gpio_port, led);
};
void BoardLed_Toggle(GPIO_TypeDef *gpio_port, uint32_t led)
{
    if ((gpio_port->ODR & led) == led)
    {
        gpio_port->BSRR = led << 16;
    }
    else
    {
        gpio_port->BSRR = led;
    }
}
bool BoardUserButton_Pressed()
{
    if (LL_GPIO_IsInputPinSet(BUTTON_USER_GPIO_PORT, BUTTON_USER_PIN))
    {
        return true;
    }
    else
    {
        return false;
    }
}
void InitializeDevicePins()
{
    CreatePinList(&mcuPins[0], ARRAY_LEN(mcuPins));
    CreateADCChannelList(BoardADCChannels, ARRAY_LEN(BoardADCChannels));
    CreateDACChannelList(BoardDACChannels, ARRAY_LEN(BoardDACChannels));
    CreateI2CChannelList(BoardI2CChannels, ARRAY_LEN(BoardI2CChannels));
    CreatePWMChannelList(BoardPWMChannels, ARRAY_LEN(BoardPWMChannels));
    CreateSPIChannelList(BoardSPIChannels, ARRAY_LEN(BoardSPIChannels));
    CreateSerialChannelList(BoardSerialChannels, ARRAY_LEN(BoardSerialChannels));
    CreateSDChannelList(BoardSDChannels, ARRAY_LEN(BoardSDChannels));
};
