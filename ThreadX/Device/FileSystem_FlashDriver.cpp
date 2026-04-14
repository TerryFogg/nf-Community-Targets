//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "FileSystem.h"
#include "target_board.h"
#include "lx_api.h"
#include "memory.h"

static bool is_initialized = false;
uint8_t *flash_disk_memory = (uint8_t *)(uint32_t)&flash_disk_start_address;
unsigned int flash_size = (uint32_t)&flash_disk_size;
static LX_NAND_FLASH nand_flash;

bool File_System_FLASH_Initialize()
{
    FX_MEDIA *media = GetMedia('F');
    DriverFunc media_driver = GetMediaDriver('F');
    VOID *driver_info_ptr = NULL; //  Optional information pointer
    LX_NAND_FLASH *nand_flash;

    uint32_t status =
        fx_media_open(media, (char *)"FLASH", media_driver, driver_info_ptr, flash_disk_memory, flash_size);

    if (status != FX_SUCCESS)
    {
        uint32_t NumberOfFATs = 1;
        uint32_t DirectoryEntries = 32;
        uint32_t HiddenSectors = 0;
        uint32_t SectorsPerTrack = 1;
        uint32_t BytesPerSector = 128;
        uint32_t TotalSectors = (uint32_t)&flash_disk_size / BytesPerSector;
        uint32_t Heads = 1;
        uint32_t SectorsPerCluster = 1;
        uint32_t flash_size = (uint32_t)&flash_disk_size;
        // Ram disk total sectors must be an integral
        ASSERT(((uint32_t)&flash_disk_size % BytesPerSector) == 0);

        uint8_t *flash_disk_memory = (uint8_t *)(uint32_t)&flash_disk_start_address;

        FormatMedia(
            'R',
            flash_disk_memory,
            flash_size,
            NumberOfFATs,
            DirectoryEntries,
            HiddenSectors,
            SectorsPerTrack,
            BytesPerSector,
            TotalSectors,
            Heads,
            SectorsPerCluster,
            (char *)"MY_FLASH_DISK");
    }
    if (status == FX_SUCCESS)
    {
        is_initialized = true;
    }
    else
    {
        is_initialized = false;
    }
    return is_initialized;
}

void File_Flash_Driver(FX_MEDIA *media_ptr)
{
    int i;
    int status;
    uint8_t *source_buffer;
    uint8_t *destination_buffer;
    int logical_sector;

    DriverFunc media_driver = GetMediaDriver('F');
    FX_MEDIA *media = GetMedia('F');

    switch (media_ptr->fx_media_driver_request)
    {
        case FX_DRIVER_INIT:
        {
            lx_nand_flash_initialize();
            UINT status = fx_media_open(media, (char *)"MY_FLASH_DISK", media_driver, FX_NULL, flash_disk_memory, flash_size);

            /* With flash wear leveling, FileX should tell wear leveling when sectors are no longer in use.*/

            media_ptr->fx_media_driver_free_sector_update = FX_TRUE;

            {
                /* Open the NAND flash simulation.  */
                //status = _lx_nand_flash_open(
                //    media,
                //    "sim nand flash",
                //    _lx_nand_flash_simulator_initialize,
                //    lx_memory_buffer,
                //    sizeof(lx_memory_buffer));
            }
            /* Determine if the flash open was successful.  */
            if (status != LX_SUCCESS)
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                return;
            }
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }

        case FX_DRIVER_UNINIT:
        {

            /* There is nothing to do in this case for the RAM driver.  For actual
               devices some shutdown processing may be necessary.  */

            /* Close the NAND flash simulation.  */
            status = _lx_nand_flash_close(&nand_flash);

            /* Determine if the flash close was successful.  */
            if (status != LX_SUCCESS)
            {

                /* Return an I/O error to FileX.  */
                media_ptr->fx_media_driver_status = FX_IO_ERROR;

                return;
            }

            /* Successful driver request.  */
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }
        case FX_DRIVER_READ:
        {
            logical_sector = media_ptr->fx_media_driver_logical_sector;
            destination_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;
            for (i = 0; i < media_ptr->fx_media_driver_sectors; i++)
            {

                status = lx_nand_flash_sector_read(&nand_flash, logical_sector, destination_buffer);
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
            // Read the boot record and return to the caller.
            // Setup the destination buffer.
            destination_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;
            // Read boot sector from NAND flash.
            status = lx_nand_flash_sector_read(&nand_flash, 0, destination_buffer);
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

            // Loop to write sectors to flash.
            for (i = 0; i < media_ptr->fx_media_driver_sectors; i++)
            {
                status = lx_nand_flash_sector_write(&nand_flash, logical_sector, source_buffer);
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
            // Write the boot record and return to the caller.
            // Setup the source buffer.
            source_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;
            status = lx_nand_flash_sector_write(&nand_flash, 0, source_buffer);
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

            // Release sectors.
            for (i = 0; i < media_ptr->fx_media_driver_sectors; i++)
            {
                status = lx_nand_flash_sector_release(&nand_flash, logical_sector);
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
