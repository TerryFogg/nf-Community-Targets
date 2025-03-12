//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include <nanoHAL_Windows_Storage.h>
#include "FileSystem.h"


HRESULT Library_nf_sys_io_filesystem_System_IO_File::ExistsNative___STATIC__BOOLEAN__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        char *folderPath = (char *)stack.Arg0().RecoverString();
        char *fileName = (char *)stack.Arg0().RecoverString();
        char *FolderAndfile = NULL;

        FAULT_ON_NULL_ARG(folderPath);
        FAULT_ON_NULL_ARG(fileName);

        CombinePathAndName(FolderAndfile, folderPath, fileName);
        bool exists = FileExists(FolderAndfile);
        stack.SetResult_Boolean(exists);
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::MoveNative___STATIC__VOID__STRING__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        char *filePathSrc = (char *)stack.Arg0().RecoverString();
        char *filePathDest = (char *)stack.Arg1().RecoverString();
        FAULT_ON_NULL_ARG(filePathDest);
        FAULT_ON_NULL_ARG(filePathSrc);

        if (!MoveFile(filePathSrc, filePathDest))
        {
            // invalid path
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::DeleteNative___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        char *FolderAndfile = (char *)stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(FolderAndfile);

        if (!DeleteFile(FolderAndfile))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::GetAttributesNative___STATIC__U1__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        char *FolderAndfile = (char *)stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(FolderAndfile);
        CLR_UINT8 attributes = 0xFF;

        if (!GetFileAttributes(FolderAndfile, &attributes))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        stack.SetResult_U1(attributes);
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::SetAttributesNative___STATIC__VOID__STRING__U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        char *FolderAndfile = (char *)stack.Arg0().RecoverString();
        CLR_UINT8 attributes = stack.Arg1().NumericByRef().u1;
        FAULT_ON_NULL_ARG(FolderAndfile);

        if (!SetFileAttributes(FolderAndfile, attributes))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        stack.SetResult_U1(attributes);
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_File::GetLastWriteTimeNative___STATIC__SystemDateTime__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        SYSTEMTIME fileTime;
        char *FolderAndfile = (char *)stack.Arg0().RecoverString();
        FAULT_ON_NULL_ARG(FolderAndfile);
        GetFileWriteTime(&fileTime, FolderAndfile);
        CLR_RT_HeapBlock &ref = stack.PushValue();
        CLR_INT64 *pRes = Library_corlib_native_System_DateTime::NewObject(ref);
        FAULT_ON_NULL(pRes);
        *pRes = HAL_Time_ConvertFromSystemTime(&fileTime);
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}
