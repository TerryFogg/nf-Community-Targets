//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "File_Drivers.h"
#include "board.h"

static bool is_initialized = false;
VOID Ram_Driver(FX_MEDIA *media_ptr)
{
    uint8_t *source_buffer;
    uint8_t *destination_buffer;
    int bytes_per_sector;

    uint32_t RAMStore_index = (uint32_t)media_ptr->fx_media_driver_info;

    switch (media_ptr->fx_media_driver_request)
    {
        case FX_DRIVER_INIT:
        {
            _fx_utility_memory_set((uint8_t *)FX_SRAM_DISK_BASE_ADDRESS, '\0', FX_SRAM_DISK_SIZE);
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_UNINIT:
        {
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_READ:
        {
            source_buffer = ((uint8_t *)FX_SRAM_DISK_BASE_ADDRESS) +
                            ((media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors) *
                             media_ptr->fx_media_bytes_per_sector);
            _fx_utility_memory_copy(
                source_buffer,
                media_ptr->fx_media_driver_buffer,
                media_ptr->fx_media_driver_sectors * media_ptr->fx_media_bytes_per_sector);
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_WRITE:
        {
            destination_buffer = (uint8_t *)FX_SRAM_DISK_BASE_ADDRESS +
                                 ((media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors) *
                                  media_ptr->fx_media_bytes_per_sector);
            _fx_utility_memory_copy(
                media_ptr->fx_media_driver_buffer,
                destination_buffer,
                media_ptr->fx_media_driver_sectors * media_ptr->fx_media_bytes_per_sector);
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_FLUSH:
        {
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_ABORT:
        {
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_BOOT_READ:
        {
            source_buffer = (uint8_t *)FX_SRAM_DISK_BASE_ADDRESS;
            bytes_per_sector = _fx_utility_16_unsigned_read(&source_buffer[FX_BYTES_SECTOR]);
            if (bytes_per_sector > media_ptr->fx_media_memory_size)
            {
                media_ptr->fx_media_driver_status = FX_BUFFER_ERROR;
                break;
            }
            _fx_utility_memory_copy(source_buffer, media_ptr->fx_media_driver_buffer, bytes_per_sector);
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_BOOT_WRITE:
        {
            destination_buffer = (uint8_t *)FX_SRAM_DISK_BASE_ADDRESS;
            _fx_utility_memory_copy(
                media_ptr->fx_media_driver_buffer,
                destination_buffer,
                media_ptr->fx_media_bytes_per_sector);
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        default:
        {
            media_ptr->fx_media_driver_status = FX_IO_ERROR;
            break;
        }
    }
}
