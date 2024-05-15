#pragma once
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nf_sys_io_filesystem.h"
#include <nanoHAL_Windows_Storage.h>

#pragma region SD card
bool InitializeSDCard(
    SDCard_SDInterfaceType cardType,
    int cardDetectPin,
    bool useCardDetect,
    SDCard_SDDataWidth dataWith);
bool DisposeSDCard(int cardType, int cardDetectPin, bool useCardDetect);
bool MountSDCard(SDCard_SDInterfaceType cardType, SDCard_SDDataWidth dataWith);
bool UnMountSDCard(SDCard_SDInterfaceType cardType);
bool PollDetectSDCard(int cardDetectPin);
void postManagedStorageEvent(bool pinState, uint32_t driveIndex);
void cardDetect_interrupt(GPIO_PIN Pin, bool pinState, void *pArg);
#pragma endregion

#pragma region Logical Drives
char *GetLogicalDrives(const char *logicalDrive, int driveListSequence);
#pragma endregion

#pragma region Folders/Directories
bool FolderExists(const char *folderPath);
bool MoveFolder(const char *folderPathSrc, const char *folderPathDest);
bool DeleteFolder(const char *folderPath);
bool CreateFolder(const char *folderPath);
int GetFolderCount(const char *folderPath);
char *GetFolders(const char *folderPath, int folderSequenceNumber);
char *GetFiles(const char *folderAndFilePath, int fileSequenceNumber);
SYSTEMTIME GetFolderWriteTime(const char *folderPath);
#pragma endregion

#pragma region Files
bool OpenFile(const char *filePath, const char *fileName, FileMode mode);
bool FileExists(const char *filePath, const char *fileName);
int CreateFile(const char *filePath, const char *fileName);
bool MoveFile(const char *filePathSrc, const char *filePathDest);
bool DeleteFile(const char *filePathAndfileName);
bool GetFileAttributes(const char *filePathAndName, uint8_t *attributes);
bool SetFileAttributes(const char *filePathAndName, uint8_t *attributes);
int ReadFile(const char *filePath, const char *fileName, uint8_t *buffer, int position, int length);
int WriteFile(const char *filePath, const char *fileName, uint8_t *buffer, int position, int length);
int64_t GetFileLength(const char *filePath, const char *fileName);
SYSTEMTIME GetFileWriteTime(const char *filePath);
#pragma endregion
