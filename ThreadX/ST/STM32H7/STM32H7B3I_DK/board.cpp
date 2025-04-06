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

#ifdef FILEX
#include "File_Drivers.h"
#endif
#include "Delays.h"

static DeviceGpioPin *mcuPins;

void SetupPinList()
{
    mcuPins[0] = {PA0, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[1] = {PA0, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[2] = {PA1, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[3] = {PA2, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[4] = {PA3, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[5] = {PA4, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[6] = {PA5, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[7] = {PA6, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[8] = {PA7, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[9] = {PA8, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[10] = {PA9, true, NULL, PinMode::PinMode_Input, DevicePinFunction::USART1_TX};
    mcuPins[11] = {PA10, true, NULL, PinMode::PinMode_Input, DevicePinFunction::USART1_RX};
    mcuPins[12] = {PA11, true, NULL, PinMode::PinMode_Input, DevicePinFunction::CAN};
    mcuPins[13] = {PA12, true, NULL, PinMode::PinMode_Input, DevicePinFunction::CAN};
    mcuPins[14] = {PA13, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[15] = {PA14, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[16] = {PA15, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[17] = {PB0, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[18] = {PB1, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[19] = {PB2, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[20] = {PB3, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[21] = {PB4, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[22] = {PB5, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[23] = {PB6, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[24] = {PB7, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[25] = {PB8, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[26] = {PB9, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[27] = {PB10, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[28] = {PB11, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[29] = {PB12, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[30] = {PB13, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[31] = {PB14, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[32] = {PB15, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[33] = {PC0, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[34] = {PC1, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[35] = {PC2, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[36] = {PC3, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[37] = {PC4, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[38] = {PC5, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[39] = {PC6, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[40] = {PC7, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[41] = {PC8, true, NULL, PinMode::PinMode_Input, DevicePinFunction::SD};
    mcuPins[42] = {PC9, true, NULL, PinMode::PinMode_Input, DevicePinFunction::SD};
    mcuPins[43] = {PC10, true, NULL, PinMode::PinMode_Input, DevicePinFunction::SD};
    mcuPins[44] = {PC11, true, NULL, PinMode::PinMode_Input, DevicePinFunction::SD};
    mcuPins[45] = {PC12, true, NULL, PinMode::PinMode_Input, DevicePinFunction::SD};
    mcuPins[46] = {PC13, true, NULL, PinMode::PinMode_Input, DevicePinFunction::BUTTON};
    mcuPins[47] = {PC14, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[48] = {PC15, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[49] = {PD0, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[50] = {PD1, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[51] = {PD2, true, NULL, PinMode::PinMode_Input, DevicePinFunction::SD};
    mcuPins[51] = {PD3, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[53] = {PD4, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[54] = {PD5, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[55] = {PD6, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[56] = {PD7, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[57] = {PD8, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[58] = {PD9, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[59] = {PD10, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[60] = {PD11, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[61] = {PD12, true, NULL, PinMode::PinMode_Input, DevicePinFunction::I2C4_SDA};
    mcuPins[62] = {PD13, true, NULL, PinMode::PinMode_Input, DevicePinFunction::I2C4_SCL};
    mcuPins[63] = {PD14, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[64] = {PD15, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[65] = {PE0, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[66] = {PE1, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[67] = {PE2, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[68] = {PE3, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[69] = {PE4, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[70] = {PE5, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[71] = {PE6, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[72] = {PE7, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[73] = {PE8, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[74] = {PE9, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[75] = {PE10, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[76] = {PE11, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[77] = {PE12, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[78] = {PE13, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[79] = {PE14, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[80] = {PE15, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[81] = {PF0, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[82] = {PF1, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[83] = {PF2, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[84] = {PF3, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[85] = {PF4, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[86] = {PF5, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[87] = {PF6, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[88] = {PF7, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[89] = {PF8, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[90] = {PF9, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[90] = {PF10, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[91] = {PF11, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[92] = {PF12, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[93] = {PF13, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[94] = {PF14, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[95] = {PF15, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[96] = {PG0, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[97] = {PG1, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[98] = {PG2, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LED};
    mcuPins[99] = {PG3, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[100] = {PG4, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[101] = {PG5, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[102] = {PG6, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[103] = {PG7, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[104] = {PG8, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[105] = {PG9, true, NULL, PinMode::PinMode_Input, DevicePinFunction::MEMORY_INTERFACE};
    mcuPins[106] = {PG10, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[107] = {PG11, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LED};
    mcuPins[108] = {PG12, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[109] = {PG13, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[110] = {PG14, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[111] = {PG15, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[112] = {PH0, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[113] = {PH1, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[114] = {PH2, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[115] = {PH3, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[116] = {PH4, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[117] = {PH5, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[118] = {PH6, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[119] = {PH7, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[120] = {PH8, true, NULL, PinMode::PinMode_Input, DevicePinFunction::CAN};
    mcuPins[121] = {PH9, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[122] = {PH10, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[123] = {PH11, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[124] = {PH12, true, NULL, PinMode::PinMode_Input, DevicePinFunction::USB};
    mcuPins[125] = {PH13, true, NULL, PinMode::PinMode_Input, DevicePinFunction::USART4_TX};
    mcuPins[126] = {PH14, true, NULL, PinMode::PinMode_Input, DevicePinFunction::USART4_RX};
    mcuPins[127] = {PH15, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[128] = {PI0, true, NULL, PinMode::PinMode_Input, DevicePinFunction::SPI};
    mcuPins[129] = {PI1, true, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[130] = {PI2, true, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[131] = {PI3, true, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[132] = {PI4, true, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[133] = {PI5, true, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[134] = {PI6, true, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[135] = {PI7, true, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[136] = {PI8, true, NULL, PinMode::PinMode_Input, DevicePinFunction::SD};
    mcuPins[137] = {PI9, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[138] = {PI10, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[139] = {PI11, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[140] = {PI12, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[141] = {PI13, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[142] = {PI14, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[143] = {PI15, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[144] = {PJ0, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[145] = {PJ1, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[146] = {PJ2, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[147] = {PJ3, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[148] = {PJ4, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[149] = {PJ5, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[150] = {PJ6, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[151] = {PJ7, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[152] = {PJ8, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[153] = {PJ9, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[154] = {PJ10, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[155] = {PJ11, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[156] = {PJ12, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[157] = {PJ13, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[158] = {PJ14, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[159] = {PJ15, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[160] = {PK0, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[161] = {PK1, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[162] = {PK2, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[163] = {PK3, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[164] = {PK4, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[165] = {PK5, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[166] = {PK6, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
    mcuPins[167] = {PK7, true, NULL, PinMode::PinMode_Input, DevicePinFunction::LCD};
}
I2C_Pin_Definition i2cPins[] = {
    {1, PB7, PB8, LL_GPIO_AF_4},
    {2, PB10, PB11, LL_GPIO_AF_4},
    {3, PH8, PH7, LL_GPIO_AF_4},
    {4, PD13, PD12, LL_GPIO_AF_4}};

#ifdef FILEX
Device::SDPin BoardSDChannels[] = {
    {.controllerId = SDMMC1,
     .controllerNumber = 1,
     .channelNumber = 1,
     .readType = File_Read_Type::FILE_NONE,
     .status = File_Status::FILE_READY,
     .RX_semaphore = rxs,
     .TX_semaphore = txs}};
#endif

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
    // InitializeSD_Bus(
    //     SD1_BUSIndex,
    //     PC8,  // SDMMC D0
    //     PC9,  // SDMMC D1
    //     PC10, // SDMMC D2
    //     PC11, // SDMMC D3
    //     PC12, // SDMMC CLK
    //     PD2,  // SDMMC CMD
    //     PI8); // SDMMC uSD detect
}
void InitializeBoardPeripherals()
{
    Initialize_Board_LEDS();
    Initialize_Board_Buttons();
    Device::CreatePinList(&mcuPins[0], ARRAY_LEN(mcuPins));
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
I2C_Pin_Definition *GetI2cBusPins()
{
    return i2cPins;
}
