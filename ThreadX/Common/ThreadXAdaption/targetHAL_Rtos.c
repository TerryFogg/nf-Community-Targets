//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <tx_api.h>
#include "nanoHAL_Rtos.h"

void RtosYield()
{
    tx_thread_relinquish();
}
