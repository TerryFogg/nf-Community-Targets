//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Power.h>
#include <nanoHAL_v2.h>
#include "board.h"

inline void CPU_Reset()
{
    volatile uint32_t *aircr = (volatile uint32_t *)0xE000ED0C;
    *aircr = (0x05FA << 16) | 0x1 << 2;
    for (;;) /* wait until reset */
    {
        __asm__ volatile("nop");
    }
}

inline bool CPU_IsSoftRebootSupported()
{
    return true;
}

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{
    (void)level;
    (void)wakeEvents;
}

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    switch (powerLevel)
    {
    case PowerLevel__Off:
        break;
    default:
        // all the other power modes are unsupported here
        break;
    }
}

