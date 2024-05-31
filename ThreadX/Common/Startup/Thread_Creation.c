// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <nanoCLR_Application.h>
#include <tx_api.h>
#include "startup_threads.h"
#include "board.h"
#include "tx_adaption.h"
#ifdef FILEX
 #include <fx_api.h>
#endif


#define RECEIVER_THREAD_PRIORITY   4
#define CLR_THREAD_PRIORITY        5

 bool g_waitForDebuggerRequested;

TX_BYTE_POOL byte_pool_0;
uint8_t memory_area[DEFAULT_BYTE_POOL_SIZE];

TX_THREAD receiverThread;
TX_THREAD CLRThread;

uint32_t receiverThreadStack[RECEIVER_THREAD_STACK_SIZE / sizeof(uint32_t)];
uint32_t CLRThreadStack[CLR_THREAD_STACK_SIZE / sizeof(uint32_t)];


void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    uint16_t status;
    CHAR *pointer = TX_NULL;

    // Create a byte memory pool from which to allocate the thread stacks.
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEFAULT_BYTE_POOL_SIZE);

    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, RECEIVER_THREAD_STACK_SIZE, TX_NO_WAIT);
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, CLR_THREAD_STACK_SIZE, TX_NO_WAIT);

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
void Startup_Rtos(bool debuggerRequested)
{
    g_waitForDebuggerRequested = debuggerRequested;
    tx_kernel_enter();
}
