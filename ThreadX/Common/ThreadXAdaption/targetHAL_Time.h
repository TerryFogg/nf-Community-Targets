#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "tx_api.h"

#define HAL_Time_CurrentSysTicks tx_time_get
#define PLATFORM_DELAY(milliSeconds) tx_thread_sleep((milliSeconds * TX_TIMER_TICKS_PER_SECOND) / 1000);

#ifdef __cplusplus
extern "C"
{
#endif
    // Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
    inline __attribute__((always_inline)) uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
    {
        // need to convert RTOS ticks to 100 nanoseconds
        return sysTicks * TX_TIMER_TICKS_PER_SECOND * 1000;
    }
#ifdef __cplusplus
}
#endif
