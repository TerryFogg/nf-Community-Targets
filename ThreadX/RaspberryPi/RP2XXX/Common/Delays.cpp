//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Delays.h"
#include "board.h"

void InitializeDelayCounter()
{
}
void DelayCounter(int delayValue, int period)
{
}
void DelayMicroseconds(uint32_t microseconds)
{
    DelayCounter(microseconds, 1000000);
}
void DelayMilliseconds(uint32_t milliseconds)
{
    DelayCounter(milliseconds, 1000);
}
uint32_t GetCurrentMicroseconds(bool reset)
{
    return 1;
}
