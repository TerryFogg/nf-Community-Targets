//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "FileSystem.h"
#include "File_Drivers.h"
#include <fx_api.h>

#pragma region SD card

/* Buffer for FileX FX_MEDIA sector cache. */
__attribute__((aligned(32))) uint32_t media_memory[FILE_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)];

bool InitializeSDCard(
    SDCard_SDInterfaceType cardType,
    int cardDetectPin,
    bool useCardDetect,
    SDCard_SDDataWidth dataWith)
{
    (void)cardType;
    (void)cardDetectPin;
    (void)useCardDetect;
    (void)dataWith;
    return true;
}
bool DisposeSDCard(int cardType, int cardDetectPin, bool useCardDetect)
{
    (void)cardType;
    (void)cardDetectPin;
    (void)useCardDetect;

    // Not used
    return true;
}
bool MountSDCard(SDCard_SDInterfaceType cardType, SDCard_SDDataWidth dataWith)
{
    (void)cardType;
    (void)dataWith;
    return false;
}
bool UnMountSDCard(SDCard_SDInterfaceType cardType)
{
    (void)cardType;
    return false;
}
bool PollDetectSDCard(int cardDetectPin)
{
    (void)cardDetectPin;
    return false;
}
void postManagedStorageEvent(bool pinState, uint32_t driveIndex)
{
    StorageEventType eventType =
        pinState ? StorageEventType_RemovableDeviceRemoval : StorageEventType_RemovableDeviceInsertion;
    PostManagedEvent(EVENT_STORAGE, 0, eventType, driveIndex);
}
void cardDetect_interrupt(GPIO_PIN Pin, bool pinState, void *pArg)
{
    (void)Pin;
    postManagedStorageEvent(pinState, (uint32_t)pArg);
}
#pragma endregion

#pragma region Logical Drives
char *GetLogicalDrives(const char *logicalDrive, int driveListSequence)
{
    static char logicalDriveName[4];
    (void)logicalDrive;
    (void)driveListSequence;

    return &logicalDriveName[0];
}
#pragma endregion

#pragma region Folders/Directories

bool FolderExists(const char *folderPath)
{
    (void)folderPath;
    bool status = false;
    return status;
}
bool MoveFolder(char *folderPathSrc, const char *folderPathDest)
{
    (void)folderPathSrc;
    (void)folderPathDest;
    bool status = false;
    return status;
}
bool DeleteFolder(const char *folderPath)
{
    (void)folderPath;
    bool status = false;
    return status;
}
bool CreateFolder(const char *folderPath)
{
    (void)folderPath;
    bool status = false;
    return status;
}
int GetFolderCount(const char *folderPath)
{
    (void)folderPath;
    return 1;
}
char *GetFolders(const char *folderPath, int folderSequenceNumber)
{
    static char FolderOrFileName[260];
    (void)folderPath;
    (void)folderSequenceNumber;
    return &FolderOrFileName[0];
}
char *GetFiles(const char *folderAndFilePath, int fileSequenceNumber)
{
    (void)folderAndFilePath;
    (void)fileSequenceNumber;

    static char FileName[260];
    return &FileName[0];
}
SYSTEMTIME GetFolderWriteTime(const char *folderpath)
{
    (void)folderpath;
    SYSTEMTIME x;
    x.wDay = 5;
    return x;
}

#pragma endregion

#pragma region Files
bool OpenFile(const char *filePath, const char *fileName, FileMode mode)
{
    (void)filePath;
    (void)mode;

    FX_MEDIA media;
    int result = fx_media_open(&media, (char *)fileName, Sd_Driver, 0, (VOID *)media_memory, sizeof(media_memory));
    switch (result)
    {
        case FX_INVALID_NAME:
        case FX_MEDIA_NOT_OPEN:
        case FX_WRITE_PROTECT:
        case FX_ALREADY_CREATED:
        case FX_INVALID_PATH:
            break;
        case FX_SUCCESS:
            break;
    }
    bool status = false;
    return status;
}
bool FileExists(const char *filePath, const char *fileName)
{
    (void)filePath;
    (void)fileName;
    bool status = false;
    return status;
}
int CreateFile(const char *filePath, const char *fileName)
{
    (void)filePath;
    FX_MEDIA media;

    int status = fx_file_create(&media, (char *)fileName);
    switch (status)
    {
        case FX_INVALID_NAME:
        case FX_MEDIA_NOT_OPEN:
        case FX_WRITE_PROTECT:
        case FX_ALREADY_CREATED:
        case FX_INVALID_PATH:
            break;
        case FX_SUCCESS:
            break;
    }
    return status;
}
bool MoveFile(const char *filePathSrc, const char *filePathDest)
{
    (void)filePathSrc;
    (void)filePathDest;
    bool status = false;
    return status;
}
bool DeleteFile(const char *filePathAndfileName)
{
    (void)filePathAndfileName;
    bool status = false;
    return status;
}
bool GetFileAttributes(const char *filePathAndName, uint8_t *attributes)
{
    (void)filePathAndName;
    (void)attributes;
    uint64_t file_write_time;
    file_write_time = 0;
    return file_write_time;
}
bool SetFileAttributes(const char *filePathAndName, uint8_t *attributes)
{
    (void)filePathAndName;
    (void)attributes;
    bool status = false;
    return status;
}
int ReadFile(const char *filePath, const char *fileName, uint8_t *buffer, int position, int length)
{
    (void)filePath;
    (void)fileName;
    (void)buffer;
    (void)position;
    (void)length;
    return 0;
}
int WriteFile(const char *filePath, const char *fileName, uint8_t *buffer, int position, int length)
{
    (void)filePath;
    (void)fileName;
    (void)buffer;
    (void)position;
    (void)length;
    return 0;
}
int64_t GetFileLength(const char *filePath, const char *fileName)
{
    (void)filePath;
    (void)fileName;
    return 0;
}
SYSTEMTIME GetFileWriteTime(const char *filePath)
{
    (void)filePath;

    SYSTEMTIME x;
    x.wDay = 5;
    return x;
}
#pragma endregion
