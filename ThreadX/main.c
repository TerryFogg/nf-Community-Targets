//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "board.h"

bool g_waitForDebuggerRequested = false;
int main(void)
{
    Initialize_Board();
    g_waitForDebuggerRequested = BoardUserButton_Pressed();
    Startup_Rtos();
}
