// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <tx_api.h>
#include "board.h"
#include "CLRNativeThreads.h"
#include "System.Device.Network.h"

#define Network_THREAD_STACK_SIZE 256
#define Network_THREAD_PRIORITY   5

TX_THREAD NetworkWorkerThread;
extern TX_BYTE_POOL byte_pool_0;

void CreateNetworkThread()
{

    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, Network_THREAD_STACK_SIZE, TX_NO_WAIT);
    tx_event_flags_create(&networkRequestEvent, "networkProcessRequest");

    uint16_t status = tx_thread_create(
        &NetworkWorkerThread,
        (CHAR *)"Network_Worker_Thread",
        NetworkThread_Entry,
        0,
        pointer,
        Network_THREAD_STACK_SIZE,
        Network_THREAD_PRIORITY,
        Network_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

