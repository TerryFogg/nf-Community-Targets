// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <tx_api.h>
#include "board.h"
#include "CLRNativeThreads.h"

#define I2C_THREAD_PRIORITY   5
#define I2C_THREAD_STACK_SIZE 1000

extern TX_BYTE_POOL byte_pool_0;
extern bool g_waitForDebuggerRequested;

TX_THREAD I2CThread;

void CreateI2CThread()
{
    void *pointer = TX_NULL;
    const char *I2CThreadString = "I2C_Long_Running_Transactions_Thread";

    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, I2C_THREAD_STACK_SIZE, TX_NO_WAIT);

    uint16_t status = tx_thread_create(
        &I2CThread,
        (char *)I2CThreadString,
        I2CThread_Entry,
        0,
        pointer,
        I2C_THREAD_STACK_SIZE,
        I2C_THREAD_PRIORITY,
        I2C_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}
