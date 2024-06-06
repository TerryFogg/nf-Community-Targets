#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "tx_api.h"

#define TX_TICKS_PER_MILLISEC(milliSecs)          ((milliSecs * TX_TIMER_TICKS_PER_SECOND) / 1000)
#define PLATFORM_DELAY(milliSecs)                 tx_thread_sleep(milliSecs / 10)
#define PLATFORM_MILLISECONDS_TO_TICKS(milliSecs) (milliSecs / (1000 / TX_TIMER_TICKS_PER_SECOND))

#define HAL_Time_CurrentSysTicks tx_time_get

#define GLOBAL_LOCK()   TX_INTERRUPT_SAVE_AREA TX_DISABLE
#define GLOBAL_UNLOCK() TX_RESTORE
#define NANOCLR_STOP()  CPU_Reset();

#define GLOBAL_LOCK_SOCKETS(x)
#define LPCSTR const char *


#define EVENTS_HEART_BEAT
