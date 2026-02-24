//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "FileSystem.h"
#include <fx_api.h>
#include "nanoHAL.h"
#include "memory.h"

// ThreadX integration: FileX uses ThreadX semaphores and timers.
// Make sure ThreadX is initialized before calling FileX functions.

// FileX can support an unlimited number of physical media
// Each media instance has its own distinct memory area and associated driver specified on the fx_media_open API call.
// FileX maintains a logical sector cache  for each opened media.
// The depth of the logical sector cache is determined by  the amount of memory supplied to FileX with the
// fx_media_open API  call.
// FileX offers contiguous file support through the API service fx_file_allocate to improve and make file access time
// deterministic. This routine takes the amount of memory requested and looks  for a series of adjacent clusters to
// satisfy the request.
//
// Volume formatting:
//     If the media is not yet formatted, use fx_media_format() before opening.
//
// Multiple volumes:
//     Each volume must be opened separately with its own control block and buffer.
// Partition access:
//
//     By default, FileX drivers access the first partition.
//     To access others, modify the driver to calculate offsets for additional partitions.
//

static FX_MEDIA sd_card_media;
static FX_MEDIA flash_media;
static FX_MEDIA ram_disk_media;

FX_MEDIA *GetMedia(char media_id)
{
    switch (media_id)
    {
#ifdef FILE_SYSTEM_SD
        case 'S':
            return &sd_card_media;
            break;
#endif
#ifdef FILE_SYSTEM_FLASH
        case 'F':
            return &flash_media;
            break;
#endif
#ifdef FILE_SYSTEM_RAM
        case 'R':
            return &ram_disk_media;
            break;
#endif
        default:
            return NULL;
    }
}
int GetMediaCount()
{
    int mediaCount = 0;
#ifdef FILE_SYSTEM_SD
    mediaCount++;
#endif
#ifdef FILE_SYSTEM_FLASH
    mediaCount++;
#endif
#ifdef FILE_SYSTEM_RAM
    mediaCount++;
#endif
    return mediaCount;
}

uint8_t *GetMediaAddress(char mediaName)
{
    switch (mediaName)
    {
#ifdef FILE_SYSTEM_FLASH
        case 'F':
            return (uint8_t *)flash_disk_start_address;
            break;
#endif
#ifdef FILE_SYSTEM_RAM
        case 'R':
            return (uint8_t *)ram_disk_start_address;
            break;
#endif
        default:
            return NULL;
    }
}

DriverFunc GetMediaDriver(char mediaName)
{
    switch (mediaName)
    {
#ifdef FILE_SYSTEM_SD
        case 'S':
            return &File_SD_Driver;
            break;
#endif
#ifdef FILE_SYSTEM_FLASH
        case 'F':
            return &File_Flash_Driver;
            break;
#endif
#ifdef FILE_SYSTEM_RAM
        case 'R':
            return &_fx_ram_driver;
            break;
#endif
        default:
            return NULL;
    }
}

UINT FormatMedia(
    char mediaName,
    uint8_t *media_address,
    unsigned int media_address_size,
    unsigned int number_of_fats,
    unsigned int directory_entries,
    unsigned int hidden_sectors,
    unsigned int sectors_per_track,
    unsigned int bytes_per_sector,
    unsigned int total_sectors,
    unsigned int Heads,
    unsigned int sectors_per_cluster,
    char *volume_name)
{
    UINT status;
    VOID *driver_info_ptr = NULL;   //  Optional information pointer

    DriverFunc media_driver = GetMediaDriver(mediaName);
    FX_MEDIA *media = GetMedia('R');
    status = fx_media_format(
        media,
        media_driver,
        driver_info_ptr,
        media_address,
        media_address_size,
        volume_name,
        number_of_fats,
        directory_entries,
        hidden_sectors,
        total_sectors,
        bytes_per_sector,
        sectors_per_cluster,
        Heads,
        sectors_per_track);

    return status;
}















