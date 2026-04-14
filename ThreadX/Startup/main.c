//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "target_board.h"
#include <cyw43_country.h>

#if defined(DEBUG)
bool g_waitForDebuggerRequested = true;
#else
bool g_waitForDebuggerRequested = false;
#endif

int g_Country_Code = CYW43_COUNTRY_WORLDWIDE;

// This routine should be in the configuration block to be picked for the device configuration.
int GetCountryCode()
{
    return CYW43_COUNTRY_AUSTRALIA;
}

int main(void)
{
    Initialize_Board();

    if (GetCountryCode() != 0)
    {
        g_Country_Code = GetCountryCode();
    }
    tx_kernel_enter();
}
