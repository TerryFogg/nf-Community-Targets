#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "fx_api.h"
#include "lx_api.h"
#include "nanoCLR_Headers.h"

#define FX_SRAM_DISK_BASE_ADDRESS D1_AXISRAM_BASE
#define FX_SRAM_DISK_SIZE         (32 * 1024)
#define UNKNOWN_DRIVER_ID         0xFFFFFFFF
#define FILE_DEFAULT_TIMEOUT      (10 * TX_TIMER_TICKS_PER_SECOND)
#define FILE_DEFAULT_SECTOR_SIZE  512
#define FILE_CURRENT_TIME()       tx_time_get()

extern TX_SEMAPHORE sd_tx_semaphore;
extern TX_SEMAPHORE sd_rx_semaphore;

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

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(FILEX_SYSTEM_SD)
    void Sd_Driver(FX_MEDIA *media_ptr);
    bool GetCardState(int SDBus_index);
    bool ReadSD(
        int SDBUS_index,
        SD_Card_Type card_type,
        int total_blocks,
        int start_block,
        int number_of_blocks);
    bool WriteSD(
        int SDBUS_index,
        SD_Card_Type card_type,
        int total_blocks,
        int start_block,
        int number_of_blocks);
#endif

#if defined(FILEX_SYSTEM_NAND)
    void Nand_Driver(FX_MEDIA *media_ptr);
    void ReadNAND(
        int NANDStore_index,
        File_Read_Type read_type,
        int total_blocks,
        int start_block,
        int number_of_blocks);
    void WriteNAND(
        int NANDStore_index,
        int total_blocks,
        int start_block,
        int number_of_blocks);
#endif

#if defined(FILEX_SYSTEM_NOR)
    void Nor_Driver(FX_MEDIA *media_ptr);
    void ReadNOR(
        int NORStore_index,
        File_Read_Type read_type,
        int total_blocks,
        int start_block,
        int number_of_blocks);
    void WriteNOR(
        int NORStore_index,
        File_Read_Type read_type,
        int total_blocks,
        uint8_t *pData,
        int start_block,
        int number_of_blocks);
#endif

#if defined(FILEX_SYSTEM_RAM)
    void Ram_Driver( FX_MEDIA *media_ptr);
    void ReadRAM(
        int RAMStore_index,
        int total_blocks,
        int start_block,
        int number_of_blocks);
    void WriteRAM(
        int RAMStore_index,
        int total_blocks,
        int start_block,
        int number_of_blocks);
#endif

#ifdef __cplusplus
}
#endif
