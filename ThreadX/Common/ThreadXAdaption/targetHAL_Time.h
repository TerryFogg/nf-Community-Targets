#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "tx_api.h"

#define HAL_Time_CurrentSysTicks     tx_time_get
#define PLATFORM_DELAY(milliSeconds) tx_thread_sleep((milliSeconds * TX_TIMER_TICKS_PER_SECOND) / 1000);
#define TIME_CONVERSION__TO_MILLISECONDS 10000

#define NUMBER_100NANO_SECONDS_PER_TICK TIME_CONVERSION__TO_MILLISECONDS*(1000 / TX_TIMER_TICKS_PER_SECOND);

#ifdef __cplusplus
extern "C"
{
#endif
    // Converts RTOS ticks to .NET ticks (100 nanoseconds)
    inline __attribute__((always_inline)) uint64_t HAL_Time_SysTicksToTime(uint64_t rtosTicks)
    {
        return rtosTicks * NUMBER_100NANO_SECONDS_PER_TICK;
    }
#ifdef __cplusplus
}
#endif
