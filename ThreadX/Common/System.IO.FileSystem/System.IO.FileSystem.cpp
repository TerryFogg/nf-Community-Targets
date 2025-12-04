//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "System.IO.FileSystem.h"
#include "CLRNativeThreads.h"
#include <fx_api.h>
#include "nanoHAL.h"
#include "nf_errors_exceptions.h"
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

// "S" for SD card
// "F" for Flash
// "R" for RAM disk
static FX_MEDIA SD_CARD_MEDIA;
static FX_MEDIA FLASH_MEDIA;
static FX_MEDIA RAM_MEDIA;

FX_MEDIA *GetMedia(char media_id)
{
    switch (media_id)
    {
#ifdef FILE_SYSTEM_SD
        case 'S':
            return &SD_CARD_MEDIA;
            break;
#endif
#ifdef FILE_SYSTEM_FLASH
        case 'F':
            return &FLASH_MEDIA;
            break;
#endif
#ifdef FILE_SYSTEM_RAM
        case 'R':
            return &RAM_MEDIA;
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
            return (uint8_t *)ram_disk_start_address;
            break;
#endif
        default:
            return NULL;
    }
}

void (*GetMediaDriver(char mediaName))(FX_MEDIA *media)
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
    uint8_t *MediaAddress,
    unsigned int NumberOfFATs,
    unsigned int DirectoryEntries,
    unsigned int HiddenSectors,
    unsigned int SectorsPerTrack,
    unsigned int BytesPerSector,
    unsigned int TotalSectors,
    unsigned int Heads,
    unsigned int SectorsPerCluster,
    char *volumeName)
{
    UINT status;
    static uint8_t *ram_disk_memory = (uint8_t *)ram_disk_start_address;

    FX_MEDIA *media_ptr = GetMedia(mediaName);

    
// After (preferred)
    using fx_media_driver_t = void (*)(FX_MEDIA *);
    fx_media_driver_t pDriver = GetMediaDriver(mediaName);

    int cache_size = 128 * 128;

    uint8_t *cache_buffer = (uint8_t *)platform_malloc(cache_size);
    {
        status = fx_media_format(
            media_ptr,
            pDriver,
            MediaAddress,
            cache_buffer,
            cache_size,
            volumeName,
            NumberOfFATs,
            DirectoryEntries,
            HiddenSectors,
            TotalSectors,
            BytesPerSector,
            SectorsPerCluster,
            Heads,
            SectorsPerTrack);
    }
    platform_free(cache_buffer);

    return status;
}
