//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include <nanoCLR_FileStream.h>
#include "FileSystem.h"

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::_ctor___VOID__STRING__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_String *hbPath;
        int32_t bufferSize;
        CLR_RT_HeapBlock *pThis = stack.This();
        CLR_RT_HeapBlock *pArgs = &(stack.Arg1());

        hbPath = pArgs[0].DereferenceString();
        FAULT_ON_NULL(hbPath);

        bufferSize = pArgs[1].NumericByRef().s4;

        if (bufferSize < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        NANOCLR_CHECK_HRESULT(CLR_RT_FileStream::CreateInstance(pThis[FIELD___fs], hbPath->StringText(), bufferSize));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Read___I4__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NANOCLR_SET_AND_LEAVE(ReadWriteHelper(stack, TRUE));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Write___I4__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        NANOCLR_SET_AND_LEAVE(ReadWriteHelper(stack, FALSE));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Seek___I8__I8__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int64_t byte_offset;
        uint32_t origin;
        int64_t position = 0;
        FX_FILE *file_ptr;

        CLR_RT_HeapBlock *pArgs = &(stack.Arg1());

        byte_offset = pArgs[0].NumericByRef().s8;
        origin = pArgs[1].NumericByRef().u4;

        CLR_RT_FileStream fs;
        CLR_RT_FileStream *pFs;
        // NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));
        {
            CLR_RT_HeapBlock_BinaryBlob *blob = stack.This()[FIELD___fs].DereferenceBinaryBlob();
            if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
            }
            pFs = (CLR_RT_FileStream *)blob->GetData();
        }
        // Arugment Validation
        if (origin > SEEKORIGIN_END)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        // What is file_ptr?
        UINT status = fx_file_seek(file_ptr, byte_offset);
        FX_FAIL_ON_ERROR_AND_LEAVE(status);

        stack.SetResult_I8(position);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Flush___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FX_MEDIA *media_ptr;

        CLR_RT_FileStream fs;
        CLR_RT_FileStream *pFs;
        // NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));
        {
            CLR_RT_HeapBlock_BinaryBlob *blob = stack.This()[FIELD___fs].DereferenceBinaryBlob();
            if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
            }
            pFs = (CLR_RT_FileStream *)blob->GetData();
        }

        UINT status = fx_media_flush(media_ptr);
        FX_FAIL_ON_ERROR_AND_LEAVE(status);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::GetLength___I8(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FX_FILE my_file;

        CLR_RT_FileStream fs;
        CLR_RT_FileStream *pFs;
        // NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));
        {
            CLR_RT_HeapBlock_BinaryBlob *blob = stack.This()[FIELD___fs].DereferenceBinaryBlob();
            if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
            }
            pFs = (CLR_RT_FileStream *)blob->GetData();
        }
        int64_t length = (int64_t)my_file.fx_file_current_file_size;
        stack.SetResult_I8(length);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::SetLength___VOID__I8(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pArgs = &(stack.Arg1());
        int64_t length = pArgs[0].NumericByRef().s8;
        FX_FILE file_ptr;
        CLR_RT_FileStream fs;
        CLR_RT_FileStream *pFs;
        // NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));
        {
            CLR_RT_HeapBlock_BinaryBlob *blob = stack.This()[FIELD___fs].DereferenceBinaryBlob();
            if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
            }
            pFs = (CLR_RT_FileStream *)blob->GetData();
        }

        if (length < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
        }
        UINT status = fx_file_truncate(&file_ptr, length);
        FX_FAIL_ON_ERROR_AND_LEAVE(status);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::
    GetStreamProperties___VOID__BYREF_BOOLEAN__BYREF_BOOLEAN__BYREF_BOOLEAN(CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_IO();
    NANOCLR_HEADER();
    FX_FILE my_file;
    {
        CLR_RT_HeapBlock *pArgs = &(stack.Arg1());
        CLR_RT_HeapBlock *hbCanRead = pArgs[0].Dereference();
        FAULT_ON_NULL(hbCanRead);
        {
            CLR_RT_HeapBlock *hbCanSeek = pArgs[2].Dereference();
            FAULT_ON_NULL(hbCanSeek);
            {
                CLR_RT_HeapBlock *hbCanWrite = pArgs[1].Dereference();

                CLR_RT_FileStream fs;
                CLR_RT_FileStream *pFs;
                // NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));
                {
                    CLR_RT_HeapBlock_BinaryBlob *blob = stack.This()[FIELD___fs].DereferenceBinaryBlob();
                    if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
                    }
                    pFs = (CLR_RT_FileStream *)blob->GetData();
                }
                FAULT_ON_NULL(hbCanWrite);
                {
                    bool canRead = (my_file.fx_file_open_mode & FX_OPEN_FOR_READ) != 0;
                    bool canWrite = (my_file.fx_file_open_mode & FX_OPEN_FOR_WRITE) != 0;
                    bool canSeek = true;

                    hbCanRead->SetBoolean(canRead);
                    hbCanWrite->SetBoolean(canWrite);
                    hbCanSeek->SetBoolean(canSeek);
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::Close___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // CLR_RT_FileStream *fs;
        CLR_RT_HeapBlock *pThis = stack.This();
        FX_FILE *file_ptr;

        FAULT_ON_NULL(stack.This());
        {
            UINT status = fx_file_close(file_ptr);
            FX_FAIL_ON_ERROR_AND_LEAVE(status);

            pThis[FIELD___fs].SetObjectReference(NULL);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFileStream::ReadWriteHelper(CLR_RT_StackFrame &stack, bool isRead)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *bufHB = NULL;
        CLR_RT_HeapBlock *nativeFileStreamHB = NULL;
        CLR_RT_FileStream fs;
        CLR_RT_FileStream *pFs;
        uint8_t *buffer;
        int32_t bufferLength;
        int32_t offset;
        int32_t count;
        CLR_RT_HeapBlock *timeoutHB;
        int64_t *timeoutTicks;
        int32_t bytesProcessed = 0;
        bool fRes;

        bufHB = stack.Arg1().DereferenceArray();
        FAULT_ON_NULL_ARG(bufHB);
        buffer = bufHB->GetFirstElement();
        bufferLength = (int32_t)bufHB->m_numOfElements;
        offset = stack.Arg2().NumericByRef().s4;
        count = stack.Arg3().NumericByRef().s4;
        timeoutHB = &(stack.Arg4());

        // NANOCLR_CHECK_HRESULT(GetFileStream(stack, fs));
        {
            CLR_RT_HeapBlock_BinaryBlob *blob = stack.This()[FIELD___fs].DereferenceBinaryBlob();
            if (!blob || blob->DataType() != DATATYPE_BINARY_BLOB_HEAD)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
            }
            pFs = (CLR_RT_FileStream *)blob->GetData();
        }

        // Argument Validation
        if (offset < 0 || count < 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
        }

        if (bufferLength - offset < count)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        if (timeoutHB->NumericByRef().s4 == 0)
        {
            // no timeout set, just use default timeout
            CLR_INT64 timeout = (isRead) ? pFs->GetReadTimeout() : pFs->GetWriteTimeout();
            timeoutHB->SetInteger((CLR_INT64)((timeout != 0) ? timeout : FS_DEFAULT_TIMEOUT));
        }

        NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(*timeoutHB, timeoutTicks));

        buffer += offset;

        // I/O is synchronous and does not require buffering or pinning
        if (pFs->GetBufferingStrategy() == SYNC_IO)
        {
            while (count > 0)
            {
                int processed;

                if (isRead)
                {
                    FX_FILE *file_ptr;
                    uint8_t *buffer_ptr = buffer;
                    UINT requests_size;
                    ULONG actual_size;
                    CLR_UINT32 status = fx_file_read(file_ptr, buffer_ptr, requests_size, &actual_size);
                    // NANOCLR_CHECK_HRESULT(fs->Read(buffer, count, &processed));
                }
                else
                {
                    FX_FILE *file_ptr;
                    uint8_t *buffer_ptr = buffer;
                    ULONG size;
                    CLR_UINT32 status = fx_file_write(file_ptr, buffer_ptr, size);
                    // NANOCLR_CHECK_HRESULT(fs->Write(buffer, count, &processed));
                }

                if (processed == 0)
                {
                    break;
                }

                bytesProcessed += processed;
                buffer += processed;
                count -= processed;
            }
        }
        else
        {
            // Push "bytesProcessed" onto the eval stack.
            if (stack.m_customState == 1)
            {
                stack.PushValueI4(0);

                switch (pFs->GetBufferingStrategy())
                {
                    case DIRECT_IO:
                        pFs->AssignStorage(buffer, count, NULL, 0);
                        bufHB->Pin();

                        break;

                    case SYSTEM_BUFFERED_IO:
                        nativeFileStreamHB = stack.This()[FIELD___fs].Dereference();
                        nativeFileStreamHB->Pin();

                        break;

                    // nothing to do for the other cases
                    default:
                        break;
                }

                stack.m_customState = 2;
            }

            bytesProcessed = stack.m_evalStack[1].NumericByRef().s4;

            buffer += bytesProcessed;
            count -= bytesProcessed;

            fRes = true;

            while (fRes && count > 0)
            {
                int processed;

                if (isRead)
                {
                    FX_FILE *file_ptr;
                    uint8_t *buffer_ptr = buffer;
                    UINT requests_size;
                    ULONG actual_size;
                    CLR_UINT32 status = fx_file_read(file_ptr, buffer_ptr, requests_size, &actual_size);
                    // NANOCLR_CHECK_HRESULT(fs->Read(buffer, count, &processed));
                }
                else
                {
                    FX_FILE *file_ptr;
                    uint8_t *buffer_ptr = buffer;
                    ULONG size;
                    CLR_UINT32 status = fx_file_write(file_ptr, buffer_ptr, size);
                    // NANOCLR_CHECK_HRESULT(fs->Write(buffer, count, &processed));
                }

                if (processed == 0)
                {
                    stack.m_evalStack[1].NumericByRef().s4 = bytesProcessed;

                    NANOCLR_CHECK_HRESULT(
                        g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_IO, fRes));
                }
                else if (processed < 0)
                {
                    // we've reached the end of the stream
                    break;
                }
                else
                {
                    buffer += processed;
                    bytesProcessed += processed;
                    count -= processed;
                }
            }

            // bytesProcessed
            stack.PopValue();
            // Timeout
            stack.PopValue();
        }

        stack.SetResult_I4(bytesProcessed);

        NANOCLR_CLEANUP();

        // we need to clean up if this is not rescheduled
        if (hr != CLR_E_THREAD_WAITING)
        {
            if (bufHB && bufHB->IsPinned())
            {
                bufHB->Unpin();
            }
            else if (nativeFileStreamHB && nativeFileStreamHB->IsPinned())
            {
                nativeFileStreamHB->Unpin();
            }
        }
    }
    NANOCLR_CLEANUP_END();
}
