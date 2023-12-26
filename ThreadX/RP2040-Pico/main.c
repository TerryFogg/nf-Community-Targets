//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "targetHAL_board.h"
#include "pico/multicore.h"

bool g_waitForDebuggerRequested = false;
extern void Startup_Rtos();
extern void Core1_Entry_NonRTOS();

int main(void) {
  Initialize_Board();
  g_waitForDebuggerRequested = BoardUserButton_Pressed();
 // multicore_launch_core1(Core1_Entry_NonRTOS);
  Startup_Rtos();
}

