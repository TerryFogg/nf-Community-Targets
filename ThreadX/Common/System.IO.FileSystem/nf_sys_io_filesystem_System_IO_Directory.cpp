//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include "FileSystem.h"

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::ExistsNative___STATIC__BOOLEAN__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        char *DeviceAndDirectoryPath = (char *)stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(DeviceAndDirectoryPath);
        bool exists = DirectoryExists(DeviceAndDirectoryPath);
        stack.SetResult_Boolean(exists);
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::MoveNative___STATIC__VOID__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        char *folderPathSrc = (char *)stack.Arg0().RecoverString();
        char *folderPathDest = (char *)stack.Arg1().RecoverString();
        FAULT_ON_NULL_ARG(folderPathSrc);
        FAULT_ON_NULL_ARG(folderPathDest);
        if (!MoveDirectory(folderPathSrc, folderPathDest))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_FOUND);
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::DeleteNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
         char *directoryPath = (char *)stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(directoryPath);
         if (!DeleteDirectory(directoryPath))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_EMPTY);
            // Test for not found? and what else?
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::CreateNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        char *directoryPath = (char *)stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(directoryPath);
        if (CreateDirectory(directoryPath))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_PATH_ALREADY_EXISTS);
            // CLR_E_DIRECTORY_NOT_FOUND?
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetFilesNative___STATIC__SZARRAY_STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *filePathEntry;
        char *directoryPath = (char *)stack.Arg0().RecoverString();
        CLR_RT_HeapBlock &filePaths = stack.PushValue();
        FAULT_ON_NULL_ARG(directoryPath);

        int directoryCount = 0;
         //GetFileCount(directoryPath);
        int fileCount = 0;
        if (fileCount > 0)
        {
            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_Array::CreateInstance(filePaths, fileCount, g_CLR_RT_WellKnownTypes.m_String));
            filePathEntry = (CLR_RT_HeapBlock *)filePaths.DereferenceArray()->GetFirstElement();
            for (int iDirectoryEntryNumber = 0; iDirectoryEntryNumber < directoryCount; iDirectoryEntryNumber++)
            {
                char *str = GetDirectories(directoryPath, iDirectoryEntryNumber);
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*filePathEntry, str));
            }
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetDirectoriesNative___STATIC__SZARRAY_STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *filePathEntry;
        char *directoryPath = (char *)stack.Arg0().RecoverString();
        CLR_RT_HeapBlock &filePaths = stack.PushValue();
        FAULT_ON_NULL_ARG(directoryPath);

        int directoryCount = GetDirectoryCount(directoryPath);
        int fileCount = 0;
        if (fileCount > 0)
        {
            NANOCLR_CHECK_HRESULT(
                CLR_RT_HeapBlock_Array::CreateInstance(filePaths, fileCount, g_CLR_RT_WellKnownTypes.m_String));
            filePathEntry = (CLR_RT_HeapBlock *)filePaths.DereferenceArray()->GetFirstElement();
            for (int iDirectoryEntryNumber = 0; iDirectoryEntryNumber < directoryCount; iDirectoryEntryNumber++)
            {
                char *str = GetDirectories(directoryPath, iDirectoryEntryNumber);
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*filePathEntry, str));
            }
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetLogicalDrivesNative___STATIC__SZARRAY_STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *storageFolder;
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        int NumberOfLogicalDrives = GetNumberOfLogicalDrives();
        storageFolder = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(top, NumberOfLogicalDrives, g_CLR_RT_WellKnownTypes.m_String));
        for (int iRequestedDrive = 0; iRequestedDrive < NumberOfLogicalDrives; iRequestedDrive++)
        {
            char *LogicalDriveUnit = GetLogicalDrive(iRequestedDrive);
            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*storageFolder, LogicalDriveUnit));
            storageFolder++;
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::GetLastWriteTimeNative___STATIC__SystemDateTime__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        char *folderPath = (char *)stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(folderPath);

        // get the date time details and return it on Stack as DateTime object
        SYSTEMTIME fileInfoTime;
        //= GetFolderWriteTime(folderPath);

        CLR_RT_HeapBlock &ref = stack.PushValue();

        CLR_INT64 *pRes = Library_corlib_native_System_DateTime::NewObject(ref);
        FAULT_ON_NULL(pRes);

        *pRes = HAL_Time_ConvertFromSystemTime(&fileInfoTime);
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}
