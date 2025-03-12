//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "File_Drivers.h"
#include "board.h"
#include "lx_api.h"

struct fx_lx_nand_driver_instance
{
    LX_NAND_FLASH flash_instance;
    char name[32];
    int id;
    uint32_t (*nand_driver_initialize)(LX_NAND_FLASH *);
    int initialized;
};

static struct fx_lx_nand_driver_instance fx_lx_nand_drivers[MAX_LX_NAND_DRIVERS] = {
    static struct fx_lx_nand_driver_instance *current_driver = NULL;
ULONG fx_lx_nand_driver_buffer
    [(7 * TOTAL_BLOCKS + 4 + 2 * (BYTES_PER_PHYSICAL_PAGE + SPARE_BYTES_PER_PAGE)) / sizeof(ULONG)];

static const int num_drivers = sizeof(fx_lx_nand_drivers) / sizeof(fx_lx_nand_drivers[0]);
static uint32_t find_driver_id(UINT driver_id)
{
    int i = 0;
    for (i = 0; i < num_drivers; i++)
    {
        if (fx_lx_nand_drivers[i].id == driver_id)
            return i;
    }

    return UNKNOWN_DRIVER_ID;
}
FileStatus Nand_Driver(FX_MEDIA *media_ptr)
{
    int i;
    int status;
    uint8_t *source_buffer;
    uint8_t *destination_buffer;
    int logical_sector;

    uint32_t NANDStore_index = (uint32_t)media_ptr->fx_media_driver_info;

    if (media_ptr->fx_media_driver_info == NULL)
    {
        i = UNKNOWN_DRIVER_ID;
    }
    else
    {
        i = find_driver_id((UINT)media_ptr->fx_media_driver_info);
    }
    if (i == UNKNOWN_DRIVER_ID)
    {
        media_ptr->fx_media_driver_status = FX_MEDIA_INVALID;
        return;
    }
    else
    {
        current_driver = &fx_lx_nand_drivers[i];
    }
    switch (media_ptr->fx_media_driver_request)
    {
        case FX_DRIVER_INIT:
        {
            if (current_driver->initialized == FX_FALSE)
            {
#ifdef FX_NAND_FORMAT_FLASH_BEFORE_OPEN
                // Format flash instance*/
                status = lx_nand_flash_format(
                    &current_driver->flash_instance,
                    current_driver->name,
                    current_driver->nand_driver_initialize,
                    fx_lx_nand_driver_buffer,
                    sizeof(fx_lx_nand_driver_buffer));
                if (status != LX_SUCCESS)
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                    return;
                }
#endif
                status = lx_nand_flash_open(
                    &current_driver->flash_instance,
                    current_driver->name,
                    current_driver->nand_driver_initialize,
                    fx_lx_nand_driver_buffer,
                    sizeof(fx_lx_nand_driver_buffer));
                // LevelX driver correctly initialized */
                if (status == LX_SUCCESS)
                {
                    current_driver->initialized = FX_TRUE;
                    media_ptr->fx_media_driver_status = FX_SUCCESS;
                    media_ptr->fx_media_driver_free_sector_update = FX_TRUE;
                }
                else
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                }
            }
            else
            {
                media_ptr->fx_media_driver_status = FX_SUCCESS;
            }
            break;
        }
        case FX_DRIVER_UNINIT:
        {
            status = lx_nand_flash_close(&current_driver->flash_instance);
            if (status == LX_SUCCESS)
            {
                media_ptr->fx_media_driver_status = FX_SUCCESS;
            }
            else
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
            }
            break;
        }
        case FX_DRIVER_READ:
        {
            logical_sector = media_ptr->fx_media_driver_logical_sector;
            destination_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;
            for (i = 0; i < media_ptr->fx_media_driver_sectors; i++)
            {
                status = lx_nand_flash_sector_read(&current_driver->flash_instance, logical_sector, destination_buffer);
                if (status != LX_SUCCESS)
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                    return;
                }
                logical_sector++;
                destination_buffer = destination_buffer + media_ptr->fx_media_bytes_per_sector;
            }
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_BOOT_READ:
        {
            // Read the boot record and return to the caller.  */
            // Setup the destination buffer.  */
            destination_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;
            // Read boot sector from NAND flash.  */
            status = lx_nand_flash_sector_read(&current_driver->flash_instance, 0, destination_buffer);
            if (status != LX_SUCCESS)
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                return;
            }
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_WRITE:
        {
            logical_sector = media_ptr->fx_media_driver_logical_sector;
            source_buffer = (uint8_t *)media_ptr->fx_media_driver_buffer;

            // Loop to write sectors to flash.  */
            for (i = 0; i < media_ptr->fx_media_driver_sectors; i++)
            {
                status = lx_nand_flash_sector_write(&current_driver->flash_instance, logical_sector, source_buffer);
                if (status != LX_SUCCESS)
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                    return;
                }
                logical_sector++;
                source_buffer = source_buffer + media_ptr->fx_media_bytes_per_sector;
            }
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_BOOT_WRITE:
        {
            // Write the boot record and return to the caller.  */
            // Setup the source buffer.  */
            source_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;
            status = lx_nand_flash_sector_write(&current_driver->flash_instance, 0, source_buffer);
            if (status != LX_SUCCESS)
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                return;
            }
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_RELEASE_SECTORS:
        {
            logical_sector = media_ptr->fx_media_driver_logical_sector;

            // Release sectors.  */
            for (i = 0; i < media_ptr->fx_media_driver_sectors; i++)
            {
                status = lx_nand_flash_sector_release(&current_driver->flash_instance, logical_sector);
                if (status != LX_SUCCESS)
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                    return;
                }
                logical_sector++;
            }
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
        default:
        {
            media_ptr->fx_media_driver_status = FX_IO_ERROR;
            break;
        }
    }
}
