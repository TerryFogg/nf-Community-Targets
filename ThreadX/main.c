//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "board.h"

    void test(void);

bool debuggerRequested = false;
int main(void)
{
    Initialize_Board();

#if defined(USER_BUTTON)
    debuggerRequested = BoardUserButton_Pressed();
#endif

    Startup_Rtos(debuggerRequested);
}
