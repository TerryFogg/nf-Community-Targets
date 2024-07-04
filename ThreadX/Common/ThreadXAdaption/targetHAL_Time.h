#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "tx_api.h"

#define HAL_Time_CurrentSysTicks tx_time_get
#define PLATFORM_DELAY(milliSeconds) tx_thread_sleep(CPU_MillisecondsToTicks(milliSeconds))

