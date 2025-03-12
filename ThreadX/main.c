//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "board.h"
typedef unsigned char CLR_UINT8;

#if defined(DEBUG)
bool debuggerRequested = true;
#else
bool debuggerRequested = false;
#endif

int main(void)
{
    debuggerRequested = true;
    Initialize_Board();
    Startup_Rtos(debuggerRequested);
}
