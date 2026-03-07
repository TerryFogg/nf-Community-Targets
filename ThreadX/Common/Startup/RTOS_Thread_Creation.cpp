// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "targetHAL.h"
#include <nanoCLR_Application.h>
#include "CLRNativeThreads.h"
#include <tx_api.h>
#include "network.h"

#define RECEIVER_THREAD_STACK_SIZE 16000
#define CLR_THREAD_STACK_SIZE      16000
#define NETWORK_THREAD_STACK_SIZE  1000
#define MALLOC_BYTES_AVAILABLE     8000

#define CLR_THREAD_PRIORITY      5
#define RECEIVER_THREAD_PRIORITY 5
#define NETWORK_THREAD_PRIORITY  5

#define DEFAULT_BYTE_POOL_SIZE (CLR_THREAD_STACK_SIZE + RECEIVER_THREAD_STACK_SIZE + MALLOC_BYTES_AVAILABLE)

TX_BYTE_POOL byte_pool_0;
static uint8_t byte_pool_memory_area[DEFAULT_BYTE_POOL_SIZE];

TX_THREAD CLRThread;
TX_THREAD receiverThread;
TX_THREAD AsynchronousIOThread;
TX_THREAD networkThread;

extern bool g_waitForDebuggerRequested;

void tx_application_define(void *first_unused_memory)
{
    // Memory pool for thread stacks and platform_malloc()
    tx_byte_pool_create(&byte_pool_0, (char *)"byte pool 0", byte_pool_memory_area, DEFAULT_BYTE_POOL_SIZE);

    CreateReceiverThread();
    CreateCLRThread();

#if (NETWORKING_SUPPORT == TRUE)
    CreateNetworkThread();
#endif
}

void CLRThread_Entry(uint32_t parameter)
{
    bool userRequestedWaitForDebugger = (bool)parameter;
    CLR_SETTINGS clrSettings = {0};
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
        &CLRThread,
        (char *)"CLR_Thread",
        CLRThread_Entry,
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

    status = tx_thread_create(
        &receiverThread,
        (char *)"Receiver Thread",
        ReceiverThread_entry,
        0,
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

void CreateNetworkThread()
{
    void *pointer = TX_NULL;
    UINT status = tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, NETWORK_THREAD_STACK_SIZE, TX_NO_WAIT);

    status = tx_thread_create(
        &networkThread,
        (char *)"Network Thread",
        NetworkThread_entry,
        0,
        pointer,
        NETWORK_THREAD_STACK_SIZE,
        NETWORK_THREAD_PRIORITY,
        NETWORK_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

void CreateAsynchronousIOThread()
{
    void *pointer = TX_NULL;
    int ASYNCHRONOUS_IO_THREAD_STACK_SIZE = 256;
    int ASYNCHRONOUS_IO_THREAD_PRIORITY = 5;

    UINT status = tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, ASYNCHRONOUS_IO_THREAD_STACK_SIZE, TX_NO_WAIT);
    status = tx_thread_create(
        &AsynchronousIOThread,
        (char *)"Asynchronous_IO_Worker_Thread",
        DeviceIOAsynchronousThread_Entry,
        0,
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
