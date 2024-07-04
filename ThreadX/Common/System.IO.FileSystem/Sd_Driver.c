//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "File_Drivers.h"
#include "board.h"
#include <targetHAL_Time.h>

typedef signed int CLR_INT32;

// NOTE: 
//------
// media_ptr->fx_media_driver_info is used to store the SD bus number to support multiple SD cards
// 

UINT _fx_partition_offset_calculate(
    void *partition_sector,
    UINT partition,
    ULONG *partition_start,
    ULONG *partition_size);

#define FX_SD_DEFAULT_TIMEOUT (10 * TX_TIMER_TICKS_PER_SECOND)

bool check_sd_status(CLR_INT32 SDBus_index)
{
    CLR_INT32 start = HAL_Time_CurrentSysTicks();

    while (HAL_Time_CurrentSysTicks() - start < FX_SD_DEFAULT_TIMEOUT)
    {
        if (GetCardState(SDBus_index))
        {
            return true;
        }
    }
    return false;
}

void Sd_Driver(FX_MEDIA *media_ptr)
{
    ULONG partition_start;
    ULONG partition_size;

    CLR_INT32 SDBus_index = (CLR_INT32)media_ptr->fx_media_driver_info;

    if (check_sd_status(SDBus_index) != 0)
    {
        media_ptr->fx_media_driver_status = FX_IO_ERROR;
        return;
    }

    // Process the driver request specified in the media control block
    switch (media_ptr->fx_media_driver_request)
    {
        case FX_DRIVER_INIT:
        {
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
            uint32_t startBlock = media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors;
            uint32_t numberOfBlocks = media_ptr->fx_media_driver_sectors;
            uint32_t total_blocks = media_ptr->fx_media_total_sectors;
            uint32_t card_type = media_ptr->fx_media_FAT_type;
            if (ReadSD(SDBus_index, card_type, total_blocks, startBlock, numberOfBlocks))
            {
                media_ptr->fx_media_driver_status = FX_SUCCESS;
            }
            else
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
            }
            break;
        }
        case FX_DRIVER_WRITE:
        {
            uint32_t startBlock = media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors;
            uint32_t numberOfBlocks = media_ptr->fx_media_driver_sectors;
            uint32_t total_blocks = media_ptr->fx_media_total_sectors;
            uint32_t card_type = media_ptr->fx_media_FAT_type;
            if (WriteSD(SDBus_index, card_type, total_blocks, startBlock, numberOfBlocks))
            {
                media_ptr->fx_media_driver_status = FX_SUCCESS;
            }
            else
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
            }
            break;
        }
        case FX_DRIVER_FLUSH:
        {
            // Return driver success
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
            uint32_t startBlock = media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors;
            uint32_t numberOfBlocks = media_ptr->fx_media_driver_sectors;
            uint32_t total_blocks = media_ptr->fx_media_total_sectors;
            uint32_t card_type = media_ptr->fx_media_FAT_type;
            if (ReadSD(SDBus_index, card_type, total_blocks, startBlock, numberOfBlocks))
            {
                media_ptr->fx_media_driver_status = FX_SUCCESS;
                break;
            }
            // Check if the sector 0 is the actual boot sector, otherwise calculate the offset into it.
            // Please note that this should belong to higher level of MW to do this check and it is here
            // as a temporary work solution */
            partition_start = 0;
            if (_fx_partition_offset_calculate(
                    media_ptr->fx_media_driver_buffer,
                    0,
                    &partition_start,
                    &partition_size) != FX_SUCCESS)
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                break;
            }
            if (partition_start)
            {
                if (check_sd_status(SDBus_index) != 0)
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                    break;
                }
                uint32_t startBlock = media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors;
                uint32_t numberOfBlocks = media_ptr->fx_media_driver_sectors;
                uint32_t total_blocks = media_ptr->fx_media_total_sectors;
                uint32_t card_type = media_ptr->fx_media_FAT_type;
                if (ReadSD(SDBus_index, card_type, total_blocks, startBlock, numberOfBlocks))
                {
                    media_ptr->fx_media_driver_status = FX_SUCCESS;
                    break;
                }
            }
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_BOOT_WRITE:
        {
            uint32_t startBlock = media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors;
            uint32_t numberOfBlocks = media_ptr->fx_media_driver_sectors;
            uint32_t total_blocks = media_ptr->fx_media_total_sectors;
            uint32_t card_type = media_ptr->fx_media_FAT_type;
            if (WriteSD(SDBus_index, card_type, total_blocks, startBlock, numberOfBlocks))
            {
                media_ptr->fx_media_driver_status = FX_SUCCESS;
                break;
            }
            default:
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                break;
            }
        }
            return;
    }
}
