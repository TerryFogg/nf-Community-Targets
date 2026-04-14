//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include "FileSystem.h"
#include "memory.h"

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::Delete___STATIC__VOID__STRING__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_String *hbName = stack.Arg0().DereferenceString();

        FAULT_ON_NULL(hbName);
        {
            CLR_RT_HeapBlock &top = stack.PushValueAndClear();
            bool recursive = stack.Arg1().NumericByRef().u1 != 0;
            char *fileOrDirectoryName = (char *)hbName->StringText() + 2;

            // skip "{S:}, {R:} or {F:}"
            char mediaName = hbName->StringText()[0];
            FX_MEDIA *media_ptr = GetMedia(mediaName);
            FAULT_ON_NULL(media_ptr);
            {
                UINT attributes;
                UINT status = fx_file_attributes_read(media_ptr, fileOrDirectoryName, &attributes);
                FX_FAIL_ON_ERROR_AND_LEAVE(status);
                bool IsFile = attributes & FX_DIRECTORY ? false : true;

                if (recursive)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
                }
                if (IsFile)
                {
                    if (fx_file_delete(media_ptr, fileOrDirectoryName) != FX_SUCCESS)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
                    }
                }
                else
                {
                    // Check if directory is empty and delete only if it is
                    if (fx_directory_first_entry_find(media_ptr, fileOrDirectoryName) != FX_NO_MORE_ENTRIES)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_DIRECTORY_NOT_EMPTY);
                    }
                    if (fx_directory_delete(media_ptr, fileOrDirectoryName) != FX_SUCCESS)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
                    }
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::Move___STATIC__BOOLEAN__STRING__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_String *hbName1 = stack.Arg0().DereferenceString();
        CLR_RT_HeapBlock_String *hbName2 = stack.Arg1().DereferenceString();

        FAULT_ON_NULL(hbName1);
        FAULT_ON_NULL(hbName2);
        {
            char mediaName1 = hbName1->StringText()[0];
            char mediaName2 = hbName1->StringText()[0];
            if (mediaName1 != mediaName2)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
            }
            else
            {
                FX_MEDIA *media_ptr = GetMedia(mediaName1);
                FAULT_ON_NULL(media_ptr);
                {
                    // skip "{S:}, {R:} or {F:}"
                    char *fileOrDirectoryName1 = (char *)hbName1->StringText() + 2;
                    char *fileOrDirectoryName2 = (char *)hbName2->StringText() + 2;

                    UINT status;
                    status = fx_file_rename(media_ptr, fileOrDirectoryName1, fileOrDirectoryName2);
                    FX_FAIL_ON_ERROR_AND_LEAVE(status);
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::CreateDirectory___STATIC__VOID__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_String *hbName = stack.Arg0().DereferenceString();

        FAULT_ON_NULL(hbName);
        {
            char mediaName = hbName->StringText()[0];
            FX_MEDIA *media_ptr = GetMedia(mediaName);
            FAULT_ON_NULL(media_ptr);
            {
                // skip "{S:}, {R:} or {F:}"
                char *fileOrDirectoryName = (char *)hbName->StringText() + 2;
                if (hal_strlen_s(fileOrDirectoryName) >= FX_MAXIMUM_PATH)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_PATH_TOO_LONG);
                }
                UINT status = fx_directory_create(media_ptr, fileOrDirectoryName);
                FX_FAIL_ON_ERROR_AND_LEAVE(status);
                //      stack.SetResult_Boolean(false);
                // stack.SetResult_Boolean(true);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::GetAttributes___STATIC__U4__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT attributes;
        CLR_RT_HeapBlock *managedNativeFileInfo;
        CLR_RT_TypeDef_Index nativeFileInfoTypeDef;

        CLR_RT_HeapBlock_String *hbName = stack.Arg0().DereferenceString();

        FAULT_ON_NULL(hbName);
        {
            {
                // skip "{S:}, {R:} or {F:}"
                char *file_name = (char *)hbName->StringText() + 2;

                char MediaName = file_name[0];
                FX_MEDIA *media_ptr = GetMedia(MediaName);
                FAULT_ON_NULL(media_ptr);
                {
                    UINT status = fx_file_attributes_read(media_ptr, file_name, &attributes);
                    if (status != FX_SUCCESS)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
                    }
                    FileAttributes attrs =
                        (FileAttributes)((attributes & FX_READ_ONLY ? FileAttributes::FileAttributes_ReadOnly : 0) ||
                                         (attributes & FX_HIDDEN ? FileAttributes::FileAttributes_Hidden : 0) ||
                                         (attributes & FX_SYSTEM ? FileAttributes::FileAttributes_System : 0) ||
                                         (attributes & FX_ARCHIVE ? FileAttributes::FileAttributes_Archive : 0) ||
                                         (attributes & FX_DIRECTORY ? FileAttributes::FileAttributes_Directory : 0));

                    stack.SetResult_U4(attributes);
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::SetAttributes___STATIC__VOID__STRING__U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_String *hbName = stack.Arg0().DereferenceString();
        UINT managedAttributes = stack.Arg1().NumericByRef().u4;
        UINT fxAttributes = 0;
        FAULT_ON_NULL(hbName);
        {
            // skip "{S:}, {R:} or {F:}"
            char *file_name = (char *)hbName->StringText() + 2;

            char MediaName = file_name[0];
            FX_MEDIA *media_ptr = GetMedia(MediaName);
            FAULT_ON_NULL(media_ptr);
            {
                if (managedAttributes & FileAttributes::FileAttributes_ReadOnly)
                {
                    fxAttributes | FX_READ_ONLY;
                }
                if (managedAttributes & FileAttributes::FileAttributes_Hidden)
                {
                    fxAttributes | FX_HIDDEN;
                }
                if (managedAttributes & FileAttributes::FileAttributes_System)
                {
                    fxAttributes | FX_SYSTEM;
                }
                if (managedAttributes & FileAttributes::FileAttributes_Archive)
                {
                    fxAttributes | FX_ARCHIVE;
                }
                UINT status = fx_file_attributes_set(media_ptr, file_name, fxAttributes);
                if (status != FX_SUCCESS)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_FILE_IO);
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

// Support for FAT16/FAT32 for small to medium embedded devices where compatibility and low memory footprint matter.
HRESULT Library_nf_sys_io_filesystem_System_IO_NativeIO::Format___STATIC__VOID__STRING__STRING__U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_String *pmediaName = stack.Arg0().DereferenceString();
        CLR_RT_HeapBlock_String *pFileSystemName = stack.Arg1().DereferenceString();
        uint32_t parameters = stack.Arg2().NumericByRef().u4;
        unsigned int media_address_size;

        FAULT_ON_NULL(pmediaName);
        {
            char MediaName = *(char *)pmediaName;
            FX_MEDIA *media_ptr = GetMedia(MediaName);
            uint8_t *MediaAddress = GetMediaAddress(MediaName);
            FAULT_ON_NULL(media_ptr);
            {
                UINT status = FormatMedia(
                    *media_ptr->fx_media_name,
                    MediaAddress,
                    media_address_size,
                    media_ptr->fx_media_number_of_FATs,
                    media_ptr->fx_media_root_directory_entries,
                    media_ptr->fx_media_hidden_sectors,
                    media_ptr->fx_media_sectors_per_track,
                    media_ptr->fx_media_bytes_per_sector,
                    media_ptr->fx_media_total_sectors,
                    media_ptr->fx_media_heads,
                    media_ptr->fx_media_sectors_per_cluster,
                    &MediaName);

                FX_FAIL_ON_ERROR_AND_LEAVE(status);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

UINT FxStatusToManagedStatus(UINT status)
{
    CLR_INT32 ClrFileStatus;
    switch (status)
    {
        case FX_BOOT_ERROR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_MEDIA_INVALID:
            ClrFileStatus = CLR_E_VOLUME_NOT_FOUND;
            break;
        case FX_FAT_READ_ERROR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_NOT_FOUND:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_NOT_A_FILE:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_ACCESS_ERROR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_NOT_OPEN:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_FILE_CORRUPT:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_END_OF_FILE:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_NO_MORE_SPACE:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_ALREADY_CREATED:
            ClrFileStatus = CLR_E_PATH_ALREADY_EXISTS;
            break;
        case FX_INVALID_NAME:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_PATH:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_NOT_DIRECTORY:
            ClrFileStatus = CLR_E_DIRECTORY_NOT_FOUND;
            break;
        case FX_NO_MORE_ENTRIES:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_DIR_NOT_EMPTY:
            ClrFileStatus = CLR_E_DIRECTORY_NOT_EMPTY;
            break;
        case FX_MEDIA_NOT_OPEN:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_YEAR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_MONTH:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_DAY:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_HOUR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_MINUTE:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_SECOND:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_PTR_ERROR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_ATTR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_CALLER_ERROR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_BUFFER_ERROR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_NOT_IMPLEMENTED:
            ClrFileStatus = CLR_E_NOT_SUPPORTED;
            break;
        case FX_WRITE_PROTECT:
            ClrFileStatus = CLR_E_UNAUTHORIZED_ACCESS;
            break;
        case FX_INVALID_OPTION:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_SECTOR_INVALID:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_IO_ERROR:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_NOT_ENOUGH_MEMORY:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_ERROR_FIXED:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_ERROR_NOT_FIXED:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_NOT_AVAILABLE:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_CHECKSUM:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_READ_CONTINUE:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        case FX_INVALID_STATE:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
        default:
            ClrFileStatus = CLR_E_FILE_IO;
            break;
    }

    return ClrFileStatus;
}
