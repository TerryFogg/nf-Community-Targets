//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include "FileSystem.h"

typedef Library_nf_sys_io_filesystem_System_IO_NativeFileInfo NativeFileInfo;

HRESULT Library_nf_sys_io_filesystem_System_IO_NativeFindFile::GetFileInfo___STATIC__SystemIONativeFileInfo__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        UINT attributes;
        CLR_RT_HeapBlock *managedNativeFileInfo;
        CLR_RT_TypeDef_Index nativeFileInfoTypeDef;

        CLR_RT_HeapBlock_String *hbName = stack.Arg0().DereferenceString();

        FAULT_ON_NULL(hbName);
        {
            CLR_RT_HeapBlock &top = stack.PushValueAndClear();
            managedNativeFileInfo = top.Dereference();
            {
                // skip "{S:}, {R:} or {F:}"
                char *file_name = (char *)hbName->StringText() + 2;

                char MediaName = file_name[0];
                FX_MEDIA *media_ptr = GetMedia(MediaName);

                FAULT_ON_NULL(media_ptr);
                {
                    UINT status = fx_file_attributes_read(media_ptr, file_name, &attributes);
                    FileAttributes attrs =
                        (FileAttributes)((attributes & FX_READ_ONLY ? FileAttributes::FileAttributes_ReadOnly : 0) ||
                                         (attributes & FX_HIDDEN ? FileAttributes::FileAttributes_Hidden : 0) ||
                                         (attributes & FX_SYSTEM ? FileAttributes::FileAttributes_System : 0) ||
                                         (attributes & FX_ARCHIVE ? FileAttributes::FileAttributes_Archive : 0) ||
                                         (attributes & FX_DIRECTORY ? FileAttributes::FileAttributes_Directory : 0));

                    g_CLR_RT_TypeSystem.FindTypeDef("NativeFileInfo", "System.IO", nativeFileInfoTypeDef);
                    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, nativeFileInfoTypeDef));
                    managedNativeFileInfo = top.Dereference();

                    managedNativeFileInfo[NativeFileInfo::FIELD__Attributes].SetInteger((CLR_UINT32)attrs);
                    CLR_INT64 size =  (ULONG64)media_ptr->fx_media_bytes_per_sector * ((ULONG64)media_ptr->fx_media_sectors_per_cluster);
                    managedNativeFileInfo[NativeFileInfo::FIELD__Size].SetInteger(size);

                    // NOT SURE WHY IS REQUIRED, FOR NOW SET TO NULL
                    managedNativeFileInfo[NativeFileInfo::FIELD__FileName].SetObjectReference(NULL);
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

