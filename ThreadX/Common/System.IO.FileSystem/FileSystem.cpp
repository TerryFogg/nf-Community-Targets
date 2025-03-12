//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "FileSystem.h"
#include "File_Drivers.h"
#include <fx_api.h>
#include "nanoHAL.h"
#include "nf_errors_exceptions.h"

// FileX can support an unlimited number of physical media
// Each media instance has its own distinct memory area and associated driver specified on the fx_media_open API call.
// FileX maintains a logical sector cache  for each opened media.
//  The depth of the logical sector cache is determined by  the amount of memory supplied to FileX with the
//  fx_media_open API  call.
// FileX offers contiguous file support through  the API service fx_file_allocate to improve and make file access time
// deterministic. This routine takes the amount of memory requested and looks  for a series of adjacent clusters to
// satisfy the request.

static int NumberOfLogicalDrivers = 0;

void CombinePathAndName(char *outpath, char *path1, char *path2)
{
    strcat(outpath, path1);

    // Add "\" to path if required
    if (outpath[hal_strlen_s(outpath) - 1] != '\\')
    {
        strcat(outpath, "\\");
    }
    strcat(outpath, path2);
}
int FileXToClrResultCode(int error)
{
    int nanoCLR_File_Error;
    switch (error)
    {
        case FX_SUCCESS:
            nanoCLR_File_Error = 0;
            break;
        case FX_BOOT_ERROR:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_MEDIA_INVALID:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_FAT_READ_ERROR:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_NOT_FOUND:
            nanoCLR_File_Error = CLR_E_FILE_NOT_FOUND;
            break;
        case FX_NOT_A_FILE:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_ACCESS_ERROR:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_NOT_OPEN:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_FILE_CORRUPT:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_END_OF_FILE:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_NO_MORE_SPACE:
            nanoCLR_File_Error = CLR_E_OUT_OF_MEMORY;
            break;
        case FX_ALREADY_CREATED:
            nanoCLR_File_Error = CLR_E_PATH_ALREADY_EXISTS;
            break;
        case FX_INVALID_NAME:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_INVALID_PATH:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_NOT_DIRECTORY:
            nanoCLR_File_Error = CLR_E_DIRECTORY_NOT_FOUND;
            break;
        case FX_NO_MORE_ENTRIES:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_DIR_NOT_EMPTY:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_MEDIA_NOT_OPEN:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_INVALID_YEAR:
        case FX_INVALID_MONTH:
        case FX_INVALID_DAY:
        case FX_INVALID_HOUR:
        case FX_INVALID_MINUTE:
        case FX_INVALID_SECOND:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_PTR_ERROR:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_INVALID_ATTR:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_CALLER_ERROR:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_BUFFER_ERROR:
            nanoCLR_File_Error = CLR_E_BUFFER_TOO_SMALL;
            break;
        case FX_NOT_IMPLEMENTED:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_WRITE_PROTECT:
            nanoCLR_File_Error = CLR_E_UNAUTHORIZED_ACCESS;
            break;
        case FX_INVALID_OPTION:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_SECTOR_INVALID:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_IO_ERROR:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_NOT_ENOUGH_MEMORY:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_ERROR_FIXED:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_ERROR_NOT_FIXED:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_NOT_AVAILABLE:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_INVALID_CHECKSUM:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_READ_CONTINUE:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
        case FX_INVALID_STATE:
            nanoCLR_File_Error = CLR_E_FILE_IO;
            break;
    }
    return nanoCLR_File_Error;
}

#pragma region Linked list
struct Media_Device *nextMediaDevice = NULL;
struct FileHandle *nextOpenFile = NULL;

bool AddMediaDevice(char DeviceType, char DeviceUnit)
{
    if (FindMediaDevice(DeviceType, DeviceUnit) == NULL)
    {
        struct Media_Device *newNode = (struct Media_Device *)platform_malloc(sizeof(struct Media_Device));
        newNode->DeviceType = DeviceType;
        newNode->DeviceUnit = DeviceUnit;
        newNode->next = nextMediaDevice;
        NumberOfLogicalDrivers++;
        return true;
    }
    else
    {
        return false;
    }
}
Media_Device *FindMediaDevice(char DeviceType, char DeviceUnit)
{
    struct Media_Device *temp = nextMediaDevice;
    while (temp != NULL)
    {
        if (temp->DeviceType == DeviceType && temp->DeviceUnit == DeviceUnit)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}
bool RemoveMediaDevice(char DeviceType, char DeviceUnit)
{
    struct Media_Device *temp = nextMediaDevice;
    struct Media_Device *prev = NULL;

    // Find the node to be deleted
    while (temp != NULL && (temp->DeviceType != DeviceType && temp->DeviceUnit != DeviceUnit))
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL)
    {
        return false;
    }
    // Update pointers to remove the node
    if (prev == NULL)
    {
        nextMediaDevice = temp->next;
        NumberOfLogicalDrivers--;
    }
    else
    {
        prev->next = temp->next;
    }
    platform_free(temp);
    return true;
}
Media_Device *FindMediaDeviceFromPath(char *DeviceDirectoryAndFileName)
{
    char DeviceType = DeviceDirectoryAndFileName[0];
    char DeviceUnit = DeviceDirectoryAndFileName[1];
    return FindMediaDevice(DeviceType, DeviceUnit);
}
FX_FILE *CreateFileHandle()
{
    FX_FILE *newFileHandle = (FX_FILE *)platform_malloc(sizeof(FX_FILE));
    return newFileHandle;
}
bool AddOpenFileHandle(char *DeviceDirectoryAndFileName, FX_FILE *fileHandle)
{
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    if (sdm != NULL)
    {
        struct FileHandle *newNode = (struct FileHandle *)platform_malloc(sizeof(struct FileHandle));
        newNode->OpenFile = fileHandle;
        newNode->nextOpenFile = nextOpenFile;
        sdm->NumberOfOpenFiles++;
        return true;
    }
    else
    {
        return false;
    }
}
bool RemoveFileHandle(char *DeviceDirectoryAndFileName, FX_FILE *fileHandle)
{
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    if (sdm != NULL)
    {
        struct FileHandle *temp = nextOpenFile;
        struct FileHandle *prev = NULL;
        // Find the file handle to remove from its unique id
        while (temp != NULL && (temp->OpenFile->fx_file_id != fileHandle->fx_file_id))
        {
            prev = temp;
            temp = temp->nextOpenFile;
        }
        if (temp == NULL)
        {
            return false;
        }
        if (prev == NULL)
        {
            nextOpenFile = temp->nextOpenFile;
            sdm->NumberOfOpenFiles--;
        }
        else
        {
            prev->nextOpenFile = temp->nextOpenFile;
        }
        platform_free(temp);
        return true;
    }
    else
    {
        return false;
    }
}
#pragma endregion

#pragma region SD card
__attribute__((aligned(32))) uint32_t sd_media_memory[FILE_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)];

VOID media_close_callback(FX_MEDIA *media_ptr)
{
    (void)media_ptr;
    media_ptr->fx_media_driver_status = FX_NOT_OPEN;
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
int GetNumberOfLogicalDrives()
{
    return NumberOfLogicalDrivers;
}
char *GetLogicalDrive(int iRequestedDrive)
{
    static char LogicalDriveName[3];
    struct Media_Device *current = nextMediaDevice;
    int iDrive = 0;
    while (current != NULL)
    {
        if (iDrive == iRequestedDrive)
        {
            LogicalDriveName[0] = current->DeviceType;
            LogicalDriveName[1] = current->DeviceUnit;
            LogicalDriveName[2] = 0;
        }
        current = current->next;
        iDrive++;
    }
    return LogicalDriveName;
}
#pragma endregion

#pragma region Directories
char *GetDirectoryAndFileName(char *DeviceDirectoryAndFileName)
{
    (void)DeviceDirectoryAndFileName;
    static char DirectoryAndFileName[256];
    return DirectoryAndFileName;
}
char *GetDirectoryPath(char *DeviceDirectory)
{
    (void)DeviceDirectory;
    static char DirectoryPath[256];
    return DirectoryPath;
}
bool DirectoryExists(char *DeviceAndDirectory)
{
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceAndDirectory);
    char *DirectoryPath = GetDirectoryPath(DeviceAndDirectory);
    return (fx_directory_next_entry_find(&sdm->media, DirectoryPath) == FX_SUCCESS);
}
bool MoveDirectory(char *DeviceAndDirectorySrc, char *DirectoryDst)
{
    bool status = false;
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceAndDirectorySrc);
    char *DirectoryPathSrc = GetDirectoryPath(DeviceAndDirectorySrc);

    if (fx_directory_rename(&sdm->media, DirectoryPathSrc, DirectoryDst) == FX_SUCCESS)
    {
        status = true;
    }
    return status;
}
bool DeleteDirectory(char *DeviceAndDirectory)
{
    bool status = false;
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceAndDirectory);
    char *DirectoryPath = GetDirectoryPath(DeviceAndDirectory);
    if (fx_directory_delete(&sdm->media, DirectoryPath) == FX_SUCCESS)
    {
        status = true;
    }
    return status;
}
bool CreateDirectory(char *DeviceAndDirectory)
{
    bool status = false;
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceAndDirectory);
    char *DirectoryPath = GetDirectoryPath(DeviceAndDirectory);
    if (fx_directory_create(&sdm->media, DirectoryPath) == FX_SUCCESS)
    {
        status = true;
    }
    return status;
}
int GetDirectoryCount(char *DeviceAndDirectory)
{
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceAndDirectory);
    char *DirectoryPath = GetDirectoryPath(DeviceAndDirectory);
    int directorycount = 0;

    if (fx_directory_first_entry_find(&sdm->media, DirectoryPath) == FX_SUCCESS)
    {
        directorycount++;
        while (fx_directory_next_entry_find(&sdm->media, DirectoryPath) == FX_SUCCESS)
        {
            directorycount++;
        }
    }
    return directorycount;
}
char *GetDirectories(char *DeviceAndDirectoryPath, int DirectorySequenceNumber)
{
    // TODO - semaphore needed?
    // If using a non-local path, it is important to prevent (with a ThreadX semaphore, mutex, or priority level change)
    // other application threads from changing this directory while a directory traversal is taking place.
    // Otherwise, invalid results may be obtained.

    static char DirectoryName[256];
    memset(DirectoryName, sizeof(DirectoryName), 0);
    int iDirectoryIndex = 0;
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceAndDirectoryPath);
    char *TargetDirectoryPath = GetDirectoryPath(DeviceAndDirectoryPath);

    int status = fx_directory_default_set(&sdm->media, TargetDirectoryPath);
    if (status == FX_SUCCESS)
    {
        while (fx_directory_next_entry_find(&sdm->media, DirectoryName) == FX_SUCCESS)
        {
            if (DirectorySequenceNumber == iDirectoryIndex)
            {
                return DirectoryName;
            }
            iDirectoryIndex++;
        }
    }
    return NULL;
}
int GetFileCount(char *DeviceAndDirectory)
{
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceAndDirectory);
    char *DirectoryPath = GetDirectoryPath(DeviceAndDirectory);
    int directorycount = 0;

    if (fx_directory_first_entry_find(&sdm->media, DirectoryPath) == FX_SUCCESS)
    {
        directorycount++;
        while (fx_directory_next_entry_find(&sdm->media, DirectoryPath) == FX_SUCCESS)
        {
            directorycount++;
        }
    }
    return directorycount;
}
char *GetFiles(char *DirectoryAndFilePath, int fileSequenceNumber)
{
    (void)DirectoryAndFilePath;
    (void)fileSequenceNumber;

    static char FileName[260];
    return &FileName[0];
}
int GetDirectoryWriteTime(SYSTEMTIME *directoryTime, char *directoryPath)
{
    UINT attributes;
    ULONG size;
    UINT year;
    UINT month;
    UINT day;
    UINT hour;
    UINT minute;
    UINT second;

    Media_Device *sdm = FindMediaDeviceFromPath(directoryPath);

    UINT result = fx_directory_information_get(
        &sdm->media,
        directoryPath,
        &attributes,
        &size,
        &year,
        &month,
        &day,
        &hour,
        &minute,
        &second);
    int nanoCLRCode = FileXToClrResultCode(result);
    if (nanoCLRCode == 0)
    {
        directoryTime->wYear = year;
        directoryTime->wMonth = month;
        directoryTime->wDay = day;
        directoryTime->wHour = hour;
        directoryTime->wMinute = minute;
        directoryTime->wHour = hour;
        directoryTime->wSecond = second;
        directoryTime->wMilliseconds = 0;
    }
    return nanoCLRCode;
}
bool GetDirectoryAttributes(char *DirectoryName, UINT *attributes)
{
    Media_Device *sdm = FindMediaDeviceFromPath(DirectoryName);
    if (sdm != NULL)
    {
        UINT result = fx_directory_attributes_read(&sdm->media, DirectoryName, attributes);
        if (result == FX_SUCCESS)
        {
            return true;
        }
    }
    return false;
}
#pragma endregion

#pragma region Files
char *GetFileName(char *DeviceDirectoryAndFilename)
{
    (void)DeviceDirectoryAndFilename;
    static char FileName[256];
    return FileName;
}
bool FileExists(char *DeviceDirectoryAndFileName)
{
    FX_FILE my_file;
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    char *DirectoryAndFileName = GetDirectoryAndFileName(DeviceDirectoryAndFileName);
    UINT status = fx_file_open(&sdm->media, &my_file, DirectoryAndFileName, FX_OPEN_FOR_READ);
    if (status == FX_SUCCESS)
    {
        fx_file_close(&my_file);
        return true;
    }
    else if (status == FX_NOT_FOUND)
    {
        return false;
    }
    return false;
}
int CreateFile(char *DeviceDirectoryAndFileName)
{
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    char *DirectoryAndFileName = GetDirectoryAndFileName(DeviceDirectoryAndFileName);
    int result = fx_file_create(&sdm->media, DirectoryAndFileName);
    return FileXToClrResultCode(result);
}
bool MoveFile(char *DeviceDirectoryAndFileNameSrc, char *DeviceDirectoryAndFileNameDst)
{
    bool status = false;
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileNameSrc);
    char *DirectoryPathSrc = GetDirectoryPath(DeviceDirectoryAndFileNameSrc);
    char *DirectoryPathDst = GetDirectoryPath(DeviceDirectoryAndFileNameDst);

    if (fx_file_rename(&sdm->media, DirectoryPathSrc, DirectoryPathDst) == FX_SUCCESS)
    {
        status = true;
    }
    return status;
}
bool DeleteFile(char *DeviceDirectoryAndFileName)
{
    bool status = false;
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    char *DirectoryAndFilename = GetDirectoryAndFileName(DeviceDirectoryAndFileName);
    if (fx_directory_delete(&sdm->media, DirectoryAndFilename) == FX_SUCCESS)
    {
        status = true;
    }
    return status;
}
bool GetFileAttributes(char *DeviceDirectoryAndFileName, uint8_t *attributes)
{
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    char *DirectoryAndFilename = GetDirectoryAndFileName(DeviceDirectoryAndFileName);
    if (sdm != NULL)
    {
        return (fx_file_attributes_read(&sdm->media, DirectoryAndFilename, (UINT *)attributes) == FX_SUCCESS);
    }
    else
    {
        return false;
    }
}
bool SetFileAttributes(char *DeviceDirectoryAndFileName, uint8_t attributes)
{
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    char *DirectoryAndFilename = GetDirectoryAndFileName(DeviceDirectoryAndFileName);
    if (sdm != NULL)
    {
        return (fx_file_attributes_set(&sdm->media, DirectoryAndFilename, (UINT)attributes) == FX_SUCCESS);
    }
    else
    {
        return false;
    }
}
int GetFileWriteTime(SYSTEMTIME *fileTime, char *DeviceDirectoryAndFileName)
{
    UINT attributes;
    ULONG size;
    UINT year;
    UINT month;
    UINT day;
    UINT hour;
    UINT minute;
    UINT second;

    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    char *DirectoryAndFilename = GetDirectoryAndFileName(DeviceDirectoryAndFileName);

    UINT result = fx_directory_first_full_entry_find(
        &sdm->media,
        DirectoryAndFilename,
        &attributes,
        &size,
        &year,
        &month,
        &day,
        &hour,
        &minute,
        &second);
    int nanoCLRCode = FileXToClrResultCode(result);
    if (nanoCLRCode == 0)
    {
        fileTime->wYear = year;
        fileTime->wMonth = month;
        fileTime->wDay = day;
        fileTime->wHour = hour;
        fileTime->wMinute = minute;
        fileTime->wHour = hour;
        fileTime->wSecond = second;
        fileTime->wMilliseconds = 0;
    }
    return nanoCLRCode;
}
int GetDirectoryLength(char *DeviceDirectoryAndFileName, uint32_t *FileSize)
{
    UINT attributes;
    ULONG size;
    UINT year;
    UINT month;
    UINT day;
    UINT hour;
    UINT minute;
    UINT second;

    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    char *DirectoryAndFilename = GetDirectoryAndFileName(DeviceDirectoryAndFileName);

    UINT result = fx_directory_first_full_entry_find(
        &sdm->media,
        DirectoryAndFilename,
        &attributes,
        &size,
        &year,
        &month,
        &day,
        &hour,
        &minute,
        &second);
    int nanoCLRCode = FileXToClrResultCode(result);
    nanoCLRCode == 0 ? *FileSize = size : 0;
    return nanoCLRCode;
}
#pragma endregion

#pragma region File Stream

int OpenFile(char *DeviceDirectoryAndFileName, FileMode mode)
{
    FX_FILE openFile;
    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    char *DirectoryAndFileName = GetDirectoryAndFileName(DeviceDirectoryAndFileName);

    int FILE_MODE;
    switch (mode)
    {
        case FileMode::FileMode_Append:
            FILE_MODE = FX_OPEN_FOR_WRITE;
            break;
        case FileMode::FileMode_Create:
            FILE_MODE = FX_OPEN_FOR_WRITE;
            break;
        case FileMode::FileMode_CreateNew:
            FILE_MODE = FX_OPEN_FOR_WRITE;
            break;
        case FileMode::FileMode_Open:
            FILE_MODE = FX_OPEN_FOR_WRITE;
            break;
        case FileMode::FileMode_OpenOrCreate:
            FILE_MODE = FX_OPEN_FOR_WRITE;
            break;
        case FileMode::FileMode_Truncate:
            FILE_MODE = FX_OPEN_FOR_WRITE;
            break;
    }

    UINT status = fx_file_open(&sdm->media, &openFile, DirectoryAndFileName, FILE_MODE);

    return FileXToClrResultCode(status);
}
int ReadFile(char *DeviceDirectoryAndFileName, uint8_t *buffer, int position, int length)
{
    (void)buffer;
    (void)position;
    (void)DeviceDirectoryAndFileName;

    FX_FILE file;
    CHAR localBuffer[512];
    ULONG requestedBytes = (ULONG)length;
    ULONG actualBytes = 0;

    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    (void)sdm;
    char *DirectoryAndFileName = GetDirectoryAndFileName(DeviceDirectoryAndFileName);
    (void)DirectoryAndFileName;
    int result = fx_file_read(&file, localBuffer, requestedBytes, &actualBytes);
    return FileXToClrResultCode(result);
}
int WriteFile(char *DeviceDirectoryAndFileName, uint8_t *buffer, int position, int length)
{
    (void)buffer;
    (void)position;
    (void)DeviceDirectoryAndFileName;
    (void)length;

    FX_FILE file;
    CHAR localBuffer[512];

    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    (void)sdm;
    char *DirectoryAndFileName = GetDirectoryAndFileName(DeviceDirectoryAndFileName);
    (void)DirectoryAndFileName;
    int result = fx_file_write(&file, localBuffer, sizeof(buffer));
    return FileXToClrResultCode(result);
}
int GetFileLength(char *DeviceDirectoryAndFileName, uint32_t *size)
{
    UINT attributes;
    ULONG fileSize;
    UINT year;
    UINT month;
    UINT day;
    UINT hour;
    UINT minute;
    UINT second;

    Media_Device *sdm = FindMediaDeviceFromPath(DeviceDirectoryAndFileName);
    char *DirectoryAndFilename = GetDirectoryAndFileName(DeviceDirectoryAndFileName);

    UINT result = fx_directory_first_full_entry_find(
        &sdm->media,
        DirectoryAndFilename,
        &attributes,
        &fileSize,
        &year,
        &month,
        &day,
        &hour,
        &minute,
        &second);

    *size = fileSize;

    return FileXToClrResultCode(result);
}
#pragma endregion
