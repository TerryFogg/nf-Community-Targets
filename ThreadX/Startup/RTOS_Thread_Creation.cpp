// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "targetHAL.h"
#include "nanoCLR_Application.h"
#include "CLRNativeThreads.h"
#include <tx_api.h>

#define RECEIVER_THREAD_STACK_SIZE 2048
#define CLR_THREAD_STACK_SIZE      4096
#define NETWORK_THREAD_STACK_SIZE  8000
#define MALLOC_BYTES_AVAILABLE     90000

// TODO  - Fix duplicate
#define POLL_THREAD_STACK_SIZE 2000

#define CLR_THREAD_PRIORITY      5
#define RECEIVER_THREAD_PRIORITY 5
#define NETWORK_THREAD_PRIORITY  5

#define DEFAULT_BYTE_POOL_SIZE                                                                                         \
    (CLR_THREAD_STACK_SIZE + RECEIVER_THREAD_STACK_SIZE + NETWORK_THREAD_STACK_SIZE + POLL_THREAD_STACK_SIZE +         \
     MALLOC_BYTES_AVAILABLE)

TX_BYTE_POOL byte_pool_0;
static uint8_t byte_pool_memory_area[DEFAULT_BYTE_POOL_SIZE];

TX_THREAD TX_CLRThread;
TX_THREAD TX_receiverThread;
TX_THREAD TX_AsynchronousIOThread;
TX_THREAD TX_networkThread;

extern bool g_waitForDebuggerRequested;
extern uint32_t g_networkThreadCountryCode;

void tx_application_define(void *first_unused_memory)
{
    // Memory pool for thread stacks and platform_malloc()
    tx_byte_pool_create(&byte_pool_0, (char *)"byte pool 0", byte_pool_memory_area, DEFAULT_BYTE_POOL_SIZE);

    CreateReceiverThread();
    CreateCLRThread();
}
void CLRThread(ULONG parameter)
{
    CLR_SETTINGS clrSettings = {0};
    bool userRequestedWaitForDebugger = (bool)parameter;
    clrSettings.MaxContextSwitches = 50;
    clrSettings.EnterDebuggerLoopAfterExit = true;
    clrSettings.WaitForDebugger = userRequestedWaitForDebugger;
    nanoHAL_Initialize();
    ClrStartup(clrSettings);
}
void CreateCLRThread()
{
    void *pointer = TX_NULL;
    UINT status = tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, CLR_THREAD_STACK_SIZE, TX_NO_WAIT);

    status = tx_thread_create(
        &TX_CLRThread,
        (char *)"CLR_Thread",
        CLRThread,
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
void CreateReceiverThread()
{
    void *pointer = TX_NULL;
    UINT status = tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, RECEIVER_THREAD_STACK_SIZE, TX_NO_WAIT);
    ULONG parameter = 0;

    status = tx_thread_create(
        &TX_receiverThread,
        (char *)"Receiver Thread",
        ReceiverThread,
        parameter,
        pointer,
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
    return;
}

// ??? Not implemented yet, but we need to create the thread so that the CLR can use it for async I/O operations
void CreateAsynchronousIOThread()
{
    void *pointer = TX_NULL;
    int ASYNCHRONOUS_IO_THREAD_STACK_SIZE = 256;
    int ASYNCHRONOUS_IO_THREAD_PRIORITY = 5;
    ULONG parameter = 0;

    UINT status = tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, ASYNCHRONOUS_IO_THREAD_STACK_SIZE, TX_NO_WAIT);
    status = tx_thread_create(
        &TX_AsynchronousIOThread,
        (char *)"Asynchronous_IO_Worker_Thread",
        AsynchronousIOThread,
        parameter,
        pointer,
        ASYNCHRONOUS_IO_THREAD_STACK_SIZE,
        ASYNCHRONOUS_IO_THREAD_PRIORITY,
        ASYNCHRONOUS_IO_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}
