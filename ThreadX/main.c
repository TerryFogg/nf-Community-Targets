//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "board.h"

#if defined(DEBUG)
bool g_waitForDebuggerRequested = true;
#else
bool g_waitForDebuggerRequested = false;
#endif

int main(void)
{
    Initialize_Board();
    tx_kernel_enter();
}
