//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Power.h>
#include <nanoHAL_v2.h>
#include "board.h"

extern void Reset_Handler();

uint32_t WakeupReasonStore;


/* Call the reset handler */
inline void CPU_Reset()
{
    NVIC_SystemReset();
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
