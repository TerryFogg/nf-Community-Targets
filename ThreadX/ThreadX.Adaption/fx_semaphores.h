#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "fx_api.h"

extern TX_SEMAPHORE sd_tx_semaphore;
extern TX_SEMAPHORE sd_rx_semaphore;

#define FX_SD_DEFAULT_SECTOR_SIZE 512

#define FX_SD_PRE_INIT(_media_ptr)                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((tx_semaphore_create(&sd_rx_semaphore, "sd rx transfer semaphore", 0) != TX_SUCCESS) ||                    \
            (tx_semaphore_create(&sd_tx_semaphore, "sd tx transfer semaphore", 0) != TX_SUCCESS))                      \
        {                                                                                                              \
            _media_ptr->fx_media_driver_status = FX_IO_ERROR;                                                          \
        }                                                                                                              \
    } while (0)
#define FX_SD_POST_DEINIT(_media_ptr)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        tx_semaphore_delete(&sd_rx_semaphore);                                                                         \
        tx_semaphore_delete(&sd_tx_semaphore);                                                                         \
    } while (0)
#define FX_SD_READ_CPLT_NOTIFY()                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        if (tx_semaphore_get(&sd_rx_semaphore, FX_STM32_SD_DEFAULT_TIMEOUT) != TX_SUCCESS)                             \
        {                                                                                                              \
            return FX_IO_ERROR;                                                                                        \
        }                                                                                                              \
    } while (0)
#define FX_SD_WRITE_CPLT_NOTIFY()                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if (tx_semaphore_get(&sd_tx_semaphore, FX_STM32_SD_DEFAULT_TIMEOUT) != TX_SUCCESS)                             \
        {                                                                                                              \
            return FX_IO_ERROR;                                                                                        \
        }                                                                                                              \
    } while (0)
