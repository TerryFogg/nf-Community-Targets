// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <nanoCLR_Application.h>
#include <nanoHAL_v2.h>
#include <nanoPAL_BlockStorage.h>
#include <targetHAL.h>

void CLRStartupThread(uint32_t parameter)
{

    bool userRequestedWaitForDebugger = (bool)parameter;

    // CLR settings to launch CLR thread
    CLR_SETTINGS clrSettings;
    (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));
    clrSettings.MaxContextSwitches = 50;
    clrSettings.EnterDebuggerLoopAfterExit = true;
    clrSettings.WaitForDebugger = userRequestedWaitForDebugger;

    nanoHAL_Initialize_C();
    ClrStartup(clrSettings);
}
