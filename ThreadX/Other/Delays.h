#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
    void InitializeDelayCounter();
    void DelayCounter(int delayValue, int period);
    void DelayMicroseconds(uint32_t microseconds);
    void DelayMilliseconds(uint32_t milliseconds);
    void ResetCurrentMicroseconds();
    uint32_t GetCurrentMicroseconds();
#ifdef __cplusplus
}
#endif
