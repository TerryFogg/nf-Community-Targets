// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <nanoCLR_Application.h>
#include <tx_api.h>
#include "board.h"
#include "target_platform.h"
#include <nanoHAL_v2.h>
#include "targetHAL.h"
#include "WireProtocol_ReceiverThread.h"
#include <WireProtocol_Message.h>

#ifdef FILEX
#include <fx_api.h>
#endif

void CLRStartupThread(uint32_t parameter);
void ReceiverThread_entry(uint32_t parameter);

#define RECEIVER_THREAD_PRIORITY 4
#define CLR_THREAD_PRIORITY      5

bool g_waitForDebuggerRequested;

TX_BYTE_POOL byte_pool_0;
uint8_t memory_area[DEFAULT_BYTE_POOL_SIZE] = {0};

TX_THREAD receiverThread;
uint32_t receiverThreadStack[RECEIVER_THREAD_STACK_SIZE / sizeof(uint32_t)];

TX_THREAD CLRThread;
uint32_t CLRThreadStack[CLR_THREAD_STACK_SIZE / sizeof(uint32_t)];

void Startup_Rtos(bool debuggerRequested)
{
    g_waitForDebuggerRequested = debuggerRequested;
    tx_kernel_enter();
}

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    uint16_t status;

    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEFAULT_BYTE_POOL_SIZE);

    // Create receiver thread
    status = tx_thread_create(
        &receiverThread,
        "Receiver Thread",
        ReceiverThread_entry,
        0,
        receiverThreadStack,
        RECEIVER_THREAD_STACK_SIZE,
        RECEIVER_THREAD_PRIORITY,
        RECEIVER_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }

    // Create CLR thread
    status = tx_thread_create(
        &CLRThread,
        "CLR_Thread",
        CLRStartupThread,
        g_waitForDebuggerRequested,
        CLRThreadStack,
        CLR_THREAD_STACK_SIZE,
        CLR_THREAD_PRIORITY,
        CLR_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {

#ifdef FILEX
        fx_system_initialize();
#endif

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

    //   HAL_Time_CurrentSysTicks

    nanoHAL_Initialize_C();
    ClrStartup(clrSettings);
}


extern WP_Message inboundMessage;

__attribute__((noreturn)) void ReceiverThread_entry(uint32_t parameter)
{
    (void)parameter;

    // NOTE: Don't call scheduler type calls in this module
    InitWireProtocolCommunications();
    tx_thread_sleep(50);

    WP_Message_Initialize(&inboundMessage);
    WP_Message_PrepareReception(&inboundMessage);

    // loop until thread receives a request to terminate
    while (true)
    {
        WP_Message_Process(&inboundMessage);
        tx_thread_relinquish();
    }
}

void WP_Message_PrepareReception_Platform()
{
    // empty on purpose, nothing to configure
}
