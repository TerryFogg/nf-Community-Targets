//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "board.h"

#ifdef SEMIHOSTING
#include "stdio.h"
extern void initialise_monitor_handles(void);
#endif

void test(void);

bool debuggerRequested = false;
int main(void)
{

#ifdef SEMIHOSTING
    initialise_monitor_handles();
    char buffer[255];
    printf("Please enter your name: \n");
    printf("\nAh, I know you, %s!\n", buffer);

#endif

    Initialize_Board();

#if defined(USER_BUTTON)
    debuggerRequested = BoardUserButton_Pressed();
#endif

    Startup_Rtos(debuggerRequested);
}
