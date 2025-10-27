// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <tx_api.h>
#include "board.h"
#include <nanoCLR_Application.h>
#include "WireProtocol_ReceiverThread.h"
#include <WireProtocol_Message.h>


#define RECEIVER_THREAD_PRIORITY   5
#define RECEIVER_THREAD_STACK_SIZE 5000
extern TX_BYTE_POOL byte_pool_0;

TX_THREAD receiverThread;
void wpReceiverThread_entry(uint32_t parameter);
    
void CreateReceiverThread()
{
    void *pointer = TX_NULL;
    const char *receiverThreadString = "Receiver Thread";
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, RECEIVER_THREAD_STACK_SIZE, TX_NO_WAIT);
    uint16_t status = tx_thread_create(
        &receiverThread,
        (char *)receiverThreadString,
        wpReceiverThread_entry,
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
