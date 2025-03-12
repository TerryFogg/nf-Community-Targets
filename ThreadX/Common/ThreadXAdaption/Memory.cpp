//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include <tx_api.h>
#include "memory.h"

// the byte pool for memory allocation is defined in main()
extern TX_BYTE_POOL byte_pool_0;

void HeapLocation(unsigned char * & baseAddress, unsigned int& sizeInBytes)
{
    baseAddress = (unsigned char*)                           &HeapBegin;
    sizeInBytes = (unsigned int)((size_t)&HeapEnd - (size_t)&HeapBegin);
}

void *platform_malloc(size_t size)
{
    uint8_t *pointer = (uint8_t *)TX_NULL;
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, size, TX_NO_WAIT);
    return pointer;
}

void platform_free(void *ptr)
{
    tx_byte_release(ptr);
}
