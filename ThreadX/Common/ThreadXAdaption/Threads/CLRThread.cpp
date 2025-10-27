// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <tx_api.h>
#include "board.h"
#include <nanoCLR_Application.h>
#include "CLRNativeThreads.h"

#define CLR_THREAD_PRIORITY   5
#define CLR_THREAD_STACK_SIZE 3000

extern TX_BYTE_POOL byte_pool_0;
extern bool g_waitForDebuggerRequested;

void CLRStartupThread(uint32_t parameter);
TX_THREAD CLRThread;


void CreateCLRThread()
{
    void *pointer = TX_NULL;
    const char *clrThreadString = "CLR_Thread";

    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, CLR_THREAD_STACK_SIZE, TX_NO_WAIT);

    uint16_t status = tx_thread_create(
        &CLRThread,
        (char *)clrThreadString,
        CLRStartupThread,
        g_waitForDebuggerRequested,
        pointer,
        CLR_THREAD_STACK_SIZE,
        CLR_THREAD_PRIORITY,
        CLR_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

void CLRStartupThread(uint32_t parameter)
{
    bool userRequestedWaitForDebugger = (bool)parameter;

    // CLR settings to launch CLR thread
    CLR_SETTINGS clrSettings;
    (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));
    clrSettings.MaxContextSwitches = 50;
    clrSettings.EnterDebuggerLoopAfterExit = true;
    clrSettings.WaitForDebugger = userRequestedWaitForDebugger;

    nanoHAL_Initialize();
    ClrStartup(clrSettings);
}
