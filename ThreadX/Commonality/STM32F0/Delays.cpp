//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Delays.h"
#include "board.h"

static uint32_t microsecondsPerTick;

void InitializeDelayCounter()
{
    LL_RCC_ClocksTypeDef RCC_Clocks;
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
    microsecondsPerTick = RCC_Clocks.SYSCLK_Frequency / 1000000;
    // Disable TRC
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
    // Enable TRC
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // Disable clock cycle counter
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
    // Enable  clock cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
void DelayCounter(int delayValue, int period)
{
    // This routine gives an approximate delay.
    // Used for small delays while waiting for hardware conditions to settle
    // NOTE:
    // The counter wraps after 32 bits,
    // so it is limited to 5-15 seconds depending on the MCU clock frequency
    LL_RCC_ClocksTypeDef RCC_Clocks;
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
    uint32_t ticksPerPeriod = RCC_Clocks.SYSCLK_Frequency / period;

    volatile uint32_t ticksToWait = ticksPerPeriod * delayValue;
    // Start the counter at zero to get the full 32 bits of counting
    volatile uint32_t start = (DWT->CYCCNT = 0);

    while (true)
    {
        if (DWT->CYCCNT - start >= ticksToWait)
        {
            break;
        }
    }
}
void DelayMicroseconds(uint32_t microseconds)
{
    DelayCounter(microseconds, 1000000);
}
void DelayMilliseconds(uint32_t milliseconds)
{
    DelayCounter(milliseconds, 1000);
}
void ResetCurrentMicroseconds()
{
    DWT->CYCCNT = 0;
}
uint32_t GetCurrentMicroseconds()
{
    return DWT->CYCCNT / microsecondsPerTick;
}
