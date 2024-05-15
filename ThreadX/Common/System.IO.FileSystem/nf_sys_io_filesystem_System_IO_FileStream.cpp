//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
//
// #include "nf_sys_io_filesystem.h"
// #include <nanoHAL_Windows_Storage.h>
#include "FileSystem.h"

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::OpenFileNative___VOID__STRING__STRING__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePath = stack.Arg1().RecoverString();
        const char *fileName = stack.Arg2().RecoverString();
        FileMode mode = (FileMode)(stack.Arg3().NumericByRef().s4);

        FAULT_ON_NULL(filePath);
        FAULT_ON_NULL(fileName);

        if (OpenFile((char *)filePath, (char *)fileName, mode))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::ReadNative___I4__STRING__STRING__I8__SZARRAY_U1__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePath = stack.Arg1().RecoverString();
        const char *fileName = stack.Arg2().RecoverString();
        CLR_INT64 position = stack.Arg3().NumericByRef().s8;
        CLR_INT32 length = stack.Arg5().NumericByRef().s4;
        CLR_RT_HeapBlock_Array *pArray = stack.Arg4().DereferenceArray();

        FAULT_ON_NULL(filePath);
        FAULT_ON_NULL(fileName);
        FAULT_ON_NULL_ARG(pArray);
        uint8_t *buffer = pArray->GetFirstElement();
        int readCount = ReadFile(filePath, fileName, buffer, position, length);
        stack.SetResult_I4(readCount);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::WriteNative___VOID__STRING__STRING__I8__SZARRAY_U1__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePath = stack.Arg1().RecoverString();
        const char *fileName = stack.Arg2().RecoverString();
        CLR_INT64 position = stack.Arg3().NumericByRef().s8;
        CLR_RT_HeapBlock_Array *pArray = stack.Arg4().DereferenceArray();
        const CLR_INT32 length = stack.Arg5().NumericByRef().s4;

        FAULT_ON_NULL(filePath);
        FAULT_ON_NULL(fileName);
        FAULT_ON_NULL_ARG(pArray);
        uint8_t *buffer = pArray->GetFirstElement();
        int writeCount = WriteFile(filePath, fileName, buffer, position, length);
        stack.SetResult_I4(writeCount);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_FileStream::GetLengthNative___I8__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        const char *filePath = stack.Arg1().RecoverString();
        const char *fileName = stack.Arg2().RecoverString();

        FAULT_ON_NULL(filePath);
        FAULT_ON_NULL(fileName);
        int64_t length = GetFileLength(filePath,fileName);
        stack.SetResult_I8(length);
    }
    NANOCLR_NOCLEANUP();
}
