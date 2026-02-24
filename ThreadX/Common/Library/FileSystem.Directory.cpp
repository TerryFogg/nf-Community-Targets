//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include "FileSystem.h"

HRESULT Library_nf_sys_io_filesystem_System_IO_Directory::NativeGetChildren___STATIC__SZARRAY_STRING__STRING__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    // File reference is passed as rootName, rootNameLength, relativePath
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        {
            CLR_RT_HeapBlock &top = stack.PushValue();
            CLR_RT_HeapBlock_String *hbPath = stack.Arg0().DereferenceString();
            CLR_RT_HeapBlock *pathEntry;
            CLR_UINT32 itemsCount = 0;
            CLR_UINT32 status;
            char directory[FS_MAX_DIRECTORY_LENGTH];
            FAULT_ON_NULL(hbPath);
            {
                // First character is the Volume/Media identifier
                const char *pMediaName = hbPath->StringText();
                FX_MEDIA *pMedia = GetMedia(pMediaName[0]);
                FAULT_ON_NULL(pMedia)
                {
                    char *pDirectoryName = (char *)(pMediaName + 2);

                    // Count items to determine the size to allocate an array to return to managed code
                    status = fx_directory_default_set(pMedia, pDirectoryName);
                    {
                        status = fx_directory_first_entry_find(pMedia, directory);
                        while (status == FX_SUCCESS)
                        {
                            itemsCount++;
                            status = fx_directory_next_entry_find(pMedia, directory);
                        }
                    }

                    // Setup the array to return to managed code
                    NANOCLR_CHECK_HRESULT(
                        CLR_RT_HeapBlock_Array::CreateInstance(top, itemsCount, g_CLR_RT_WellKnownTypes.m_String));
                    pathEntry = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

                    // Restart scan to load the items
                    status = fx_directory_first_entry_find(pMedia, directory);
                    {
                        while (status == FX_SUCCESS)
                        {
                            // Set file full path in array of strings
                            NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*pathEntry, directory));
                            status = fx_directory_next_entry_find(pMedia, directory);
                            pathEntry++;
                        }
                    }
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
