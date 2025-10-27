// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "targetHAL.h"
#include <nanoCLR_Application.h>
#include "CLRNativeThreads.h"

#define DEFAULT_BYTE_POOL_SIZE     10000

TX_BYTE_POOL byte_pool_0;
uint8_t memory_area[DEFAULT_BYTE_POOL_SIZE];

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    // Create a byte memory pool from which to allocate the thread stacks
    const char *str = "byte pool 0";
    tx_byte_pool_create(&byte_pool_0, (char *)str, memory_area, DEFAULT_BYTE_POOL_SIZE);

    CreateReceiverThread();

    CreateCLRThread();
  //  CreateI2CThread();

#ifdef NETWORKING
    CreateNetworkThread();
#endif
#ifdef FILEX
      CreateFileThread();
#endif
#ifdef USB
      CreateUsbThread();
#endif

}
