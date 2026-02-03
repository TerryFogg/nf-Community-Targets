#pragma once
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <fx_api.h>
#include "nf_sys_io_filesystem.h"
#include "lx_api.h"
#include "nanoCLR_Headers.h"

#define FILE_DEFAULT_TIMEOUT     (10 * TX_TIMER_TICKS_PER_SECOND)
#define FILE_DEFAULT_SECTOR_SIZE 512
#define FILE_CURRENT_TIME()      tx_time_get()

// Macro to convert FX status to managed error and jump out via NANOCLR_SET_AND_LEAVE
#define FX_FAIL_ON_ERROR_AND_LEAVE(s)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((s) != FX_SUCCESS)                                                                                         \
        {                                                                                                              \
            NANOCLR_SET_AND_LEAVE(FxStatusToManagedStatus((s)));                                                       \
        }                                                                                                              \
    } while (0)

typedef void (*DriverFunc)(FX_MEDIA *media);

typedef enum
{
    FILE_NONE = 0,
    FILE_READ_SINGLE_BLOCK = 1,
    FILE_READ_MULTIPLE_BLOCK = 2,
    FILE_WRITE_SINGLE_BLOCK = 4,
    FILE_WRITE_MULTIPLE_BLOCK = 8,
    FILE_DMA = 16
} File_Read_Type;

typedef enum
{
    FILE_SUCCESS,
    FILE_ERROR,
    FILE_READ_ERROR,
    FILE_DATA_CRC_FAIL,
    FILE_DATA_TIMEOUT,
    FILE_OVERRUN,
    FILE_UNDERRUN,
    FILE_ADDRESS_OUT_OF_RANGE,
    FILE_WRITE_PROTECTED,
    FILE_ERASE_ERROR,
    FILE_RESET,
    FILE_READY,
    FILE_TIMEOUT,
    FILE_BUSY,
    FILE_PROGRAMMING,
    FILE_TRANSFER_OK,
    FILE_TRANSFER_BUSY,
    FILE_IDENTIFICATION,
    FILE_RECEIVING,
    FILE_SENDING,
    FILE_DISCONNECTED,
    FILE_STANDBY

} File_Status;

typedef enum
{
    // Normal Speed Card <12.5Mo/s , Spec Version 1.01
    CARD_NORMAL_SPEED = 1,
    // High Speed Card <25Mo/s , Spec version 2.00
    CARD_HIGH_SPEED = 2,
    // UHS-I SD Card <50Mo/s for SDR50, DDR5 Cardsand <104Mo/s for SDR104, Spec version 3.01
    CARD_ULTRA_HIGH_SPEED = 3
} SD_Card_Speed;

typedef enum
{
    // SD Standard Capacity <2Go
    CARD_SDSC = 0,
    // SD High Capacity <32Go, SD Extended Capacity <2To
    CARD_SDHC_SDXC = 1,
    CARD_SECURED = 2
} SD_Card_Type;

struct FileHandle
{
    struct FileHandle *nextOpenFile;
    FX_FILE *OpenFile;
};

struct Media_Device
{
    struct Media_Device *next;
    char DeviceType;
    char DeviceUnit;
    int SDPinDetectNumber;
    FX_MEDIA media;
    unsigned char media_memory[512];
    struct FileHandle nextFile;
    int NumberOfOpenFiles;
};

#ifdef __cplusplus
extern "C"
{
#endif
    void _fx_ram_driver(FX_MEDIA *media_ptr);
    void File_SD_Driver(FX_MEDIA *media_ptr);
    void File_Flash_Driver(FX_MEDIA *media_ptr);
    void File_Ram_Driver(FX_MEDIA *media_ptr);
    FX_MEDIA *GetMedia(char media_id);
    uint8_t *GetMediaAddress(char MediaName);
    int GetMediaCount();

    DriverFunc GetMediaDriver(char mediaName);

    UINT FxStatusToManagedStatus(UINT status);

    UINT FormatMedia(
        char mediaName,
        uint8_t *media_address,
        unsigned int media_address_size,
        unsigned int NumberOfFATs,
        unsigned int DirectoryEntries,
        unsigned int HiddenSectors,
        unsigned int SectorsPerTrack,
        unsigned int BytesPerSector,
        unsigned int TotalSectors,
        unsigned int Heads,
        unsigned int SectorsPerCluster,
        char *volumeName);

#ifdef USB_DEVICE_ENABLE
    //  void UsbFile_Driver(FX_MEDIA *media_ptr);
#endif

#ifdef __cplusplus
}
#endif

// Define a function pointer type

class FileIO
{
  private:
  public:
};
