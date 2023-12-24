//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/util/queue.h"

typedef struct
{
    void *func;
    int32_t data;
} queue_entry_t;

#define Core1FunctionExecute(x)      multicore_fifo_push_blocking((uintptr_t)&x);
#define Core1FunctionExtraData(data) multicore_fifo_push_blocking((uintptr_t)&data);

int32_t function_Template(int32_t n);

void Core1_Entry_NonRTOS()
{
    while (1)
    {
        // Wait for information in the incoming FIFO

        // 1. Read a pointer to the function from the FIFO
        int32_t (*func)() = (int32_t(*)())multicore_fifo_pop_blocking();

        // 2. Read additional related data from the FIFO
        int32_t p = multicore_fifo_pop_blocking();

        // Run the function and return the result to the outgoing FIFO
        int32_t result = (*func)(p);
        multicore_fifo_push_blocking(result);
    }
}
