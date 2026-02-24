//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// FileX RAM/FLASH structures look like the following:
//
// Physical Sector              Contents
//     0                         Boot record
//     1                         FAT Area Start
//     +FAT Sectors              Root Directory Start
//     +Directory Sectors        Data Sector Start

#include "target_board.h"
#include "memory.h"
#include "FileSystem.h"

static bool is_initialized = false;


#define return_if_fail(p)                                                                                              \
    if (!(p))                                                                                                          \
    {                                                                                                                  \
        is_initialized = false;                                                                                        \
        return is_initialized;                                                                                         \
    }

static FX_MEDIA ram_disk_media;

bool File_System_RAM_Initialize()
{
    int status;
    if (!is_initialized)
    {
        is_initialized = true;

        unsigned int NumberOfFATs = 1;
        unsigned int DirectoryEntries = 32;
        unsigned int HiddenSectors = 0;
        unsigned int SectorsPerTrack = 1;
        unsigned int BytesPerSector = 128;
        unsigned int TotalSectors = (uint32_t)&ram_disk_size / BytesPerSector;
        unsigned int Heads = 1;
        unsigned int SectorsPerCluster = 1;
        unsigned int ram_size = (uint32_t)&ram_disk_size;
        // Ram disk total sectors must be an integral
        ASSERT(((uint32_t)&ram_disk_size % BytesPerSector) == 0);

        uint8_t *ram_disk_memory = (uint8_t *)(uint32_t)&ram_disk_start_address;

        FormatMedia(
            'R',
            ram_disk_memory,
            ram_size,
            NumberOfFATs,
            DirectoryEntries,
            HiddenSectors,
            SectorsPerTrack,
            BytesPerSector,
            TotalSectors,
            Heads,
            SectorsPerCluster,
            (char *)"MY_RAM_DISK");
    }
#if false
    {
        /* Open the ram_disk.  */
        status = fx_media_open(
            &ram_disk_media,
            (char *)"RAM DISK",
            _fx_ram_driver,
            ram_disk_memory,
            cache_buffer,
            CACHE_SIZE);
        return_if_fail(status == FX_SUCCESS);

        // create a value
        status = fx_file_create(&ram_disk_media, (char *)"TEST.TXT");
        return_if_fail(status == FX_SUCCESS);

        ULONG write_value;
        static FX_FILE my_file;

        // write a value
        status = fx_file_write(&my_file, (void *)&write_value, sizeof(ULONG));
        return_if_fail(status == FX_SUCCESS);

        // Invalidate cache
        status = fx_media_cache_invalidate(&ram_disk_media);
        return_if_fail(status == FX_SUCCESS);

        // Read value back
        ULONG read_value = 0;
        ULONG actual;
        status = fx_file_read(&my_file, (void *)&read_value, sizeof(ULONG), &actual);

        // Pickup the available bytes in the media
        ULONG available_bytes;
        status = fx_media_space_available(&ram_disk_media, &available_bytes);
        if ((status != FX_SUCCESS) || (available_bytes < sizeof(ULONG)))
        {
            is_initialized = false;
            return is_initialized;
        }

        // Create a series of directories
        status = fx_directory_create(&ram_disk_media, (char *)"/A0");
        status = fx_directory_create(&ram_disk_media, (char *)"/A0/A1");
        return_if_fail(status == FX_SUCCESS);

        // Close the file
        status = fx_file_close(&my_file);
        return_if_fail(status == FX_SUCCESS);
    }
#endif

    return is_initialized;
}
