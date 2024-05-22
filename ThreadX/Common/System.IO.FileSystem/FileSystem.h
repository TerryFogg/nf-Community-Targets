#pragma once
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nf_sys_io_filesystem.h"
#include <nanoHAL_Windows_Storage.h>
#include <fx_api.h>

void CombinePathAndName(char *outpath, char *path1, char *path2);

#pragma region Storage Devices
struct FileHandle
{
    struct FileHandle *nextOpenFile;
    FX_FILE *OpenFile;
};

// "S1","S2" ...  SD cards
// "I1","I2" ...  SOC/Onboard flash NAND,NOR,EMC
// "R1","R2" ...  RAM disks
// "U1","U2" ...  USB disks
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
bool AddMediaDevice(char DeviceType, char DeviceUnit);
bool RemoveMediaDevice(char DeviceType, char DeviceUnit);
Media_Device *FindMediaDevice(char DeviceType, char DeviceUnit);
Media_Device *FindMediaDeviceFromPath(char *filePathAndName);
bool AddOpenFileHandle(char *DeviceDirectoryAndFileName, FX_FILE *fileHandle);
bool RemoveFileHandle(char *DeviceDirectoryAndFileName, FX_FILE *fileHandle);

#pragma endregion

#pragma region SD card
void postManagedStorageEvent(bool pinState, uint32_t driveIndex);
void cardDetect_interrupt(GPIO_PIN Pin, bool pinState, void *pArg);
VOID media_close_callback(FX_MEDIA *media_ptr);
#pragma endregion

#pragma region Logical Drives
int GetNumberOfLogicalDrives();
char *GetLogicalDrive(int iRequestedDrive);
#pragma endregion

#pragma region Directories
int GetDirectoryLength(char *DeviceDirectoryAndFileName, uint32_t *FileSize);
bool DirectoryExists(char *DeviceAndDirectory);
bool MoveDirectory(char *DeviceAndDirectorySrc, char *DeviceAndDirectoryDest);
bool DeleteDirectory(char *DeviceAndDirectory);
bool CreateDirectory(char *DeviceAndDirectory);
int GetDirectoryCount(char *DeviceAndDirectory);
char *GetDirectories(char *DeviceAndDirectory, int DirectorySequenceNumber);
char *GetFiles(char *DirectoryAndFilePath, int fileSequenceNumber);
int GetDirectoryWriteTime(SYSTEMTIME *directoryTime, char *directoryPath);
#pragma endregion

#pragma region Files
char *GetFileName(char *DeviceDirectoryAndFilename);
char *GetDirectoryAndFileName(char *DeviceDirectoryAndFileName);
char *GetDirectoryPath(char *DeviceDirectoryPath);
int OpenFile(char *DeviceDirectoryAndFileName, FileMode mode);
bool FileExists(char *DirectoryAndFileName);
int CreateFile(char *DirectoryAndFileName);
bool MoveFile(char *filePathSrc, char *filePathDest);
bool DeleteFile(char *DirectoryAndFileName);
bool GetFileAttributes(char *DeviceDirectoryAndFileName, uint8_t *attributes);
bool SetFileAttributes(char *DeviceDirectoryAndFileName, uint8_t attributes);
int ReadFile(char *DirectoryAndFileName, uint8_t *buffer, int position, int length);
int WriteFile(char *DirectoryAndFileName, uint8_t *buffer, int position, int length);
int GetFileWriteTime(SYSTEMTIME *fileTime, char *DeviceDirectoryAndFileName);
int GetFileLength(char *DeviceDirectoryAndFileName, ULONG *size);
#pragma endregion
