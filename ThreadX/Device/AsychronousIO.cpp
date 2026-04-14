//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <tx_api.h>
#include <tx_port.h>
#include "Device.IO.h"

TX_EVENT_FLAGS_GROUP asynchronous_io_events_group;
TX_QUEUE AsynchronousIOQueue;

TX_EVENT_FLAGS_GROUP asynchronous_io_flags;
uint8_t *RequestData;

#define QUEUE_SIZE 10
void AsynchronousIOThread(ULONG parameter)
{
    ULONG actual_flags;
    UINT status;
    CHAR *pointer = (CHAR *)TX_NULL;

    tx_event_flags_create(&asynchronous_io_flags, (CHAR *)"asynchronous io flags");

    tx_queue_create(
        &AsynchronousIOQueue,
        (CHAR *)"asynchronous IO Queue",
        sizeof(uint8_t *),
        RequestData,
        QUEUE_SIZE * sizeof(uint8_t *));
    do
    {
        // Wait for a command
        tx_event_flags_get(
            &asynchronous_io_flags,
            REQUEST_NETWORK | REQUEST_I2C | REQUEST_USB | REQUEST_FILEX,
            TX_OR,
            &actual_flags,
            TX_WAIT_FOREVER);

        tx_queue_receive(&AsynchronousIOQueue, RequestData, TX_WAIT_FOREVER);

        if (actual_flags & REQUEST_NETWORK)
        {
        }
        else if (actual_flags & REQUEST_I2C)
        {
            I2cIO::Execute((I2c_Transaction *)RequestData);
            Events_Set(SYSTEM_EVENT_FLAG_I2C_MASTER);

        }
        else if (actual_flags & REQUEST_FILEX)
        {
           // FILE_IO::Execute((I2c_Transaction *)RequestData);
            Events_Set(SYSTEM_EVENT_FLAG_STORAGE_IO);
        }
        else if (actual_flags & REQUEST_USB)
        {
        }
    } while (true);
}
