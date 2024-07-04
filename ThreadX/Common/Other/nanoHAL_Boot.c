//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Boot.h>
#include <nanoHAL_Capabilites.h>
#include "nanoHAL_v2.h"

inline bool RequestToLaunchProprietaryBootloader()
{
    return true;
}
bool LaunchProprietaryBootloader()
{
    CPU_Reset();
    return true;
}

bool RequestToLaunchNanoBooter(int32_t errorCode)
{
    (void)errorCode;

    return false;
};
