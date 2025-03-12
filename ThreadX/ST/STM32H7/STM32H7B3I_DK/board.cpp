//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics.  All rights false.
// See LICENSE file in the project root for full license information.
//
#include "PinNames.h"
#include "DeviceIO.h"
#include "Device.h"
#include "board.h"
#include "SD_DoubleBufferedDMA.h"
#include "File_Drivers.h"
#include "Delays.h"

Device::DevicePin mcuPins[] = {

    // -----------------
    // Arduino connector
    // -----------------
    // A0 - ADC1 - channel 18
    {PA4, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // A1 - ADC12 - channel 4
    {PC4, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // A2 - ADC1 Channel 0
    {PA0_C, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // A3 - ADC1 - channel 1
    {PA1_C, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // A4 - ADC2 - channel 0
    {PC2_C, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // A5 - ADC2 - channel 1
    {PC3_C, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D0
    {PH14, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D1
    {PH13, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D2
    {PI9, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D3
    {PH9, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D4
    {PE2, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D5
    {PH11, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D6
    {PH10, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D7
    {PI10, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D8
    {PF10, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D9
    {PI7, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D10
    {PI0, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D11
    {PB15, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D12
    {PB14, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D13
    {PA12, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D14
    {PD13, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // D15
    {PD12, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},

    // -----------------------------
    // CN16 EXT_I2C connector pinout
    // -----------------------------
    // Pin1 - NC
    // Pin2 - GND
    // Pin3 - NC
    // Pin4 - VDD(3V3)
    // Pin5  -EX_RESET
    {PB6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::GPIO, 0},
    // Pin6 - I2C4_SCL
    {PD12, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::I2C, 0},
    // Pin7 - NC
    // Pin8 - I2C4_SDA
    {PD13, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::I2C, 0},

    // ----------------------------------
    // CN3: Audio extension board (DFSDM)
    // ----------------------------------
    // Pin1 - GND
    // Pin2 - 3V3
    // Pin3 /  Pin4 - duplicate
    {PB0, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin5
    {PB9, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin6
    {PB12, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin7
    {PC7, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin8 - NC
    // Pin9 - NC
    // Pin10 - DETECTn
    {PI6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin11 - NC
    // Pin12 - MEMS_LED
    {PH15, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin13 - NC
    // Pin14 - NC
    // Pin15 - NC
    // Pin16 - NC
    // Pin17 - NC
    // Pin18 - NC
    // Pin19 - 3V3
    // Pin20 - GND
    // ----------------------------------

    // ------------------------------
    //  CN4 microSD™ connector pinout
    // ------------------------------
    // Pin1 - D2
    {PC10, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::SD, 0},
    // Pin2 - D3
    {PC11, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::SD, 0},
    // Pin3 - CMD
    {PD2, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::SD, 0},
    // Pin4 - VDD(3V3)
    // Pin5 - CLK
    {PC12, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::SD, 0},
    // Pin6 - GND
    // Pin7 - D0
    {PC8, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::SD, 0},
    // Pin8 - D1
    {PC9, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::SD, 0},
    // Pin9 - GND
    // Pin10 uSD detect
    {PI8, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::GPIO, 0},
    // Pin11 - GND
    // Pin12 - GND
    // Pin13 - GND
    // Pin14 - GND

    // ------
    // STMod+
    // ------
    // Pin1
    {PA0, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin2
    {PD5, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin3
    {PD6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin4
    {PD4, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin5 - GND
    // Pin6 - 5V
    // Pin7
    {PD12, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::I2C, 0},
    // Pin8
    {PB15, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::SPI, 0},
    // Pin9
    {PB14, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::SPI, 0},
    // Pin10
    {PD13, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::I2C, 0},
    // Pin11
    {PC6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::GPIO, 0},
    // Pin12
    {PH8, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::GPIO, 0},
    // Pin13
    {PA4, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::ADC, 0},
    // Pin14
    {PF8, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::PWM, 0},
    // Pin15 - 5V
    // Pin16 - GND
    // Pin17
    {PC7, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::PWM, 0},
    // Pin18
    {PD3, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::GPIO, 0},
    // Pin19
    {PB9, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::GPIO, 0},
    // Pin20
    {PB8, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::GPIO, 0},
    
    // ---------------------
    // CN1 TFT LCD connector
    // ---------------------
    // Pin1 - GND
    // Pin2 - GND
    // Pin3
    {PI15, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin4
    {PJ7, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin5
    {PJ0, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin6
    {PJ8, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin7
    {PJ1, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin8
    {PJ9, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin9
    {PJ2, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin10
    {PJ10, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin11
    {PJ3, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin12
    {PJ11, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin13
    {PJ4, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin14
    {PK0, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin15
    {PJ5, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin16
    {PK1, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin17
    {PJ6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin18
    {PK2, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin19 - GND
    // Pin20 - GND
    // Pin21
    {PJ12, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin22
    {PK7, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin23
    {PJ13, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin24
    {PA2, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin25
    {PJ14, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin26
    {PI12, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin27
    {PJ15, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin28
    {PI13, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin29
    {PK3, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin30 - GND
    // Pin31
    {PK4, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin32
    {PI14, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin33
    {PK5, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin34 - GND
    // Pin35
    {PK6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin36 - NRST
    // Pin37 - GND
    // Pin38
    {PD13, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin39
    {PH2, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin40
    {PD12, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin41 - NC
    // Pin42 - NC
    // Pin43
    {PA1, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::LCD, 0},
    // Pin44 - NC
    // Pin45 - 5V
    // Pin46 - NC
    // Pin47 - GND
    // Pin48 - NC
    // Pin49 - GND
    // Pin50 - 3V3

    // ----------------------------------
    // CN7 camera module connector pinout
    // ----------------------------------
    // Pin1 -  GND
    // Pin2 -  NC 
    // Pin3 -  NC 
    // Pin4 -  DCMI_D0
    {PC6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin5 -  DCMI_D1 
    {PC7, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin6 -  DCMI_D2
    {PG10, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin7 -  DCMI_D3 
    {PC9, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin8 -  DCMI_D4 
    {PC11, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin9 -  DCMI_D5
    {PD3, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin10 - DCMI_D6
    {PB8, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin11 - DCMI_D7
    {PB9, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin12 - NC
    // Pin13 - NC
    // Pin14 - GND
    // Pin15 - DCMI_PIXCLK 
    {PA6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin16 - GND
    // Pin17 - DCMI_HSYNC
    {PA6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin18 - NC
    // Pin19 - DCMI_VSYNC
    {PA6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin20 - VDD (3V3)
    // Pin21 - CAMERA_CLK
    {PA6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin22 - NC
    // Pin23 - GND
    // Pin24 - NC
    // Pin25 - DCMI_PWR_EN
    {PA6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin26 - DCMI_NRST (NRST from MCU)
    {PA6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin27 - I2C4_SDA
    {PA6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin28 - I2C4_SCL
    {PA6, false, NULL, NULL, PinMode::PinMode_Input, Device::DevicePinFunction::NONE, 0},
    // Pin29 - GND
    // Pin30 - VDD(3V3)
};

Device::AdcPin BoardADCChannels[] = {{ADC1, 1, 1}, {ADC2, 11, 2}};
Device::DacPin BoardDACChannels[] = {{DAC1, 1, 1}, {DAC2, 2, 2}};
Device::I2cPin BoardI2CChannels[] = {{I2C1, 1, 4}, {I2C4, 1, 4}};
Device::PwmPin BoardPWMChannels[] = {{TIM3, 3, 1}, {TIM4, 4, 1}};
Device::SpiPin BoardSPIChannels[] = {{SPI1, 1, 1}, {SPI2, 1, 2}};
Device::SerialPin BoardSerialChannels[] = {{USART1, 1, 1}, {USART2, 2, 2}};

static TX_SEMAPHORE txs;
static TX_SEMAPHORE rxs;

Device::SDPin BoardSDChannels[] = {
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
    Initialize_SDRAM(SDRAM_BANK_ADDR, SDRAM_RAM_REGION_SIZE);
    //InitializeSD_Bus(
    //    SD1_BUSIndex,
    //    PC8,  // SDMMC D0
    //    PC9,  // SDMMC D1
    //    PC10, // SDMMC D2
    //    PC11, // SDMMC D3
    //    PC12, // SDMMC CLK
    //    PD2,  // SDMMC CMD
    //    PI8); // SDMMC uSD detect

}
void InitializeBoardPeripherals()
{
    Initialize_Board_LEDS();
    Initialize_Board_Buttons();
    InitializeDevicePins();
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
    Device::CreatePinList(&mcuPins[0], ARRAY_LEN(mcuPins));
    Device::CreateADCChannelList(BoardADCChannels, ARRAY_LEN(BoardADCChannels));
    Device::CreateDACChannelList(BoardDACChannels, ARRAY_LEN(BoardDACChannels));
    Device::CreateI2CChannelList(BoardI2CChannels, ARRAY_LEN(BoardI2CChannels));
    Device::CreatePWMChannelList(BoardPWMChannels, ARRAY_LEN(BoardPWMChannels));
    Device::CreateSPIChannelList(BoardSPIChannels, ARRAY_LEN(BoardSPIChannels));
    Device::CreateSerialChannelList(BoardSerialChannels, ARRAY_LEN(BoardSerialChannels));
    Device::CreateSDChannelList(BoardSDChannels, ARRAY_LEN(BoardSDChannels));
};

