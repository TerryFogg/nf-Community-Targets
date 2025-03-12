//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "board.h"

void Initialize_Board()
{
    CPU_CACHE_Enable();
    SystemClock_Config();
    // Disabling FMC Bank1 ? To prevent this CortexM7  speculative read accesses on FMC bank1
    // it is recommended to disable it when it is not used
    // Counter used for microsecond delays (blocking)
    FMC_Bank1_R->BTCR[0] &= ~FMC_BCRx_MBKEN;

    Initialize_DWT_Counter();
    InitializeBoardPeripherals();
    Initialize_OCTOSPI2_Hyperam();
}
void InitializeBoardPeripherals()
{
    Initialize_Board_LEDS();
    Initialize_Board_Buttons();
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
    // Enable I-Cache
    SCB_EnableICache();

    // Enable D-Cache
    SCB_EnableDCache();
}
void Initialize_Board_LEDS()
{
    // LED's
    ENABLE_CLOCK_ON_PORT_GPIOC;
    LL_GPIO_InitTypeDef gpio_InitStruct;
    gpio_InitStruct.Pin = LED_GREEN | LED_RED;
    gpio_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    gpio_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(LED_GPIO_PORT, &gpio_InitStruct);

    // Turn them off
    LL_GPIO_SetOutputPin(LED_GPIO_PORT, LED_GREEN);
    LL_GPIO_SetOutputPin(LED_GPIO_PORT, LED_RED);

}
void Initialize_Board_Buttons()
{
    // USER button
    ENABLE_CLOCK_ON_PORT_GPIOC;
    LL_GPIO_InitTypeDef gpio_InitStruct;

    gpio_InitStruct.Pin = BUTTON_USER_PIN;
    gpio_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    gpio_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(LED_GPIO_PORT, &gpio_InitStruct);

    //    LL_EXTI_EnableRisingTrig_0_31(BUTTON_USER_PIN);
    //    LL_EXTI_EnableIT_0_31(BUTTON_USER_PIN);
}
void Initialize_DWT_Counter()
{
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // Disable TRC
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  // Enable TRC
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;            // Disable clock cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;             // Enable  clock cycle counter
    DWT->CYCCNT = 0;                                 // Reset the clock cycle counter value
}
void Initialize_64bit_timer()
{
    // Although we setup a "nanosecond" timer, the real resolution is less than
    // 1ns. To get 1ns resolution would require a clock speed of 1GHz.
    //
    // For this implementation, 100ns is the smallest precision.

    // Not all counters on the STM32H7 are 32 bit
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
    LL_TIM_SetTriggerInput(TIM5, LL_TIM_TS_ITR1); // TRGO output from TIM2
    LL_TIM_EnableExternalClock(TIM5);

    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableCounter(TIM5);
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
};
