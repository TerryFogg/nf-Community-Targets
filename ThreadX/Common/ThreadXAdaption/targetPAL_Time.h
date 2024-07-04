#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoHAL_Time.h"
#define MILLISECONDS_TO_SECONDS 1000

// const int compile time evaluation
#define NumberOf100nanosecondsPerTick  (TIME_CONVERSION__TO_SECONDS / TX_TIMER_TICKS_PER_SECOND)
#define NumberOfMillisecondsPerTick    (MILLISECONDS_TO_SECONDS / TX_TIMER_TICKS_PER_SECOND)


                 