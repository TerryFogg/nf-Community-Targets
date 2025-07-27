// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <fx_api.h>
#include <tx_api.h>
#include "board.h"
#include "CLRNativeThreads.h"

// fx_system_initialize();

#define FileX_THREAD_STACK_SIZE 256
#define FileX_THREAD_PRIORITY   5
TX_THREAD FileXThread;
TX_EVENT_FLAGS_GROUP eventsFileXWorkerThread;
extern TX_BYTE_POOL byte_pool_0;

#define QUEUE_SIZE 10

TX_QUEUE File_message_queue;

void CreateFileThread()
{
    void *pointer = TX_NULL;

    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, FileX_THREAD_STACK_SIZE, TX_NO_WAIT);

    tx_queue_create(
        &File_message_queue,
        "File Message Queue",
        sizeof(Request),
        first_unused_memory,
        QUEUE_SIZE * sizeof(Request));

    uint16_t status = tx_thread_create(
        &FileXThread,
        (CHAR *)"File_Thread",
        FileThread_entry,
        0,
        pointer,
        FileX_THREAD_STACK_SIZE,
        FileX_THREAD_PRIORITY,
        FileX_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}
