//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include "FileSystem.h"

typedef Library_nf_sys_io_filesystem_System_IO_NativeIO NativeIO;

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::Refresh___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        CLR_RT_HeapBlock_String *hbName = stack.This()[FIELD___name].DereferenceString();

        const char *pMediaName = hbName->StringText();
        FX_MEDIA *pMedia = GetMedia(pMediaName[0]);

        CLR_RT_HeapBlock_String::CreateInstance(hbName[FIELD___name], pMedia->fx_media_id);
        hbName[FIELD___driveType].SetInteger(DriveType_Fixed);
        hbName[FIELD___totalSize].SetInteger((int64_t)pMedia->fx_media_memory_size);
        hbName[FIELD___volumeIndex].SetInteger((CLR_UINT32)pMedia->fx_media_id);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::DriveInfoNative___VOID__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // This is a duplicate of the _ctor method, its called from the C# constructor
        CLR_RT_HeapBlock *pThis = stack.This();
        CLR_RT_HeapBlock_String *hbName = stack.Arg1().DereferenceString();

        const char *pMediaName = hbName->StringText();
        FX_MEDIA *pMedia = GetMedia(pMediaName[0]);

        CLR_RT_HeapBlock_String::CreateInstance(hbName[FIELD___name], pMedia->fx_media_id);
        hbName[FIELD___driveType].SetInteger(DriveType_Fixed);
        hbName[FIELD___totalSize].SetInteger((int64_t)pMedia->fx_media_memory_size);
        hbName[FIELD___volumeIndex].SetInteger((CLR_UINT32)pMedia->fx_media_id);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::_ctor___VOID__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        CLR_RT_HeapBlock_String *hbName = stack.Arg1().DereferenceString();

        const char *pMediaName = hbName->StringText();
        FX_MEDIA *pMedia = GetMedia(pMediaName[0]);

        CLR_RT_HeapBlock_String::CreateInstance(hbName[FIELD___name], pMedia->fx_media_id);
        hbName[FIELD___driveType].SetInteger(DriveType_Fixed);
        hbName[FIELD___totalSize].SetInteger((int64_t)pMedia->fx_media_memory_size);
        hbName[FIELD___volumeIndex].SetInteger((CLR_UINT32)pMedia->fx_media_id);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::GetFileSystems___STATIC__SZARRAY_STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock &ret = stack.PushValueAndClear();
        CLR_RT_HeapBlock *fsNames = (CLR_RT_HeapBlock *)ret.DereferenceArray()->GetFirstElement();

        int installedFileSystems = GetMediaCount();
        NANOCLR_CHECK_HRESULT(
            CLR_RT_HeapBlock_Array::CreateInstance(ret, installedFileSystems, g_CLR_RT_WellKnownTypes.m_String));

#ifdef FILE_SYSTEM_SD
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*fsNames, "S:"));
        fsNames++;
#endif
#ifdef FILE_SYSTEM_FLASH
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*fsNames, "F:"));
        fsNames++;
#endif
#ifdef FILE_SYSTEM_RAM
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(*fsNames, "R:"));
        fsNames++;
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::MountRemovableVolumes___STATIC__VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // Not used, mount removable SD cards in the SD Mount function
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_sys_io_filesystem_System_IO_DriveInfo::GetDrivesNative___STATIC__SZARRAY_SystemIODriveInfo(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        CLR_RT_TypeDef_Index driveInfoTypeDef;
        g_CLR_RT_TypeSystem.FindTypeDef("DriveInfo", "System.IO", driveInfoTypeDef);
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, GetMediaCount(), driveInfoTypeDef));
        CLR_RT_HeapBlock *hbVolumes = (CLR_RT_HeapBlock *)top.DereferenceArray()->GetFirstElement();

        FX_MEDIA *pMedia;
        // SD Card
        {
            pMedia = GetMedia('S');
            hbVolumes[FIELD___driveType].SetInteger(DriveType_Removable);
            hbVolumes[FIELD___totalSize].SetInteger((int64_t)pMedia->fx_media_memory_size);
            hbVolumes[FIELD___volumeIndex].SetInteger(0);
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(hbVolumes[0], driveInfoTypeDef));
        }
        // Internal Flash
        {
            pMedia = GetMedia('F');
            hbVolumes[FIELD___driveType].SetInteger(DriveType_Fixed);
            hbVolumes[FIELD___totalSize].SetInteger((int64_t)pMedia->fx_media_memory_size);
            hbVolumes[FIELD___volumeIndex].SetInteger(0);
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(hbVolumes[1], driveInfoTypeDef));
        }
        // RAM Disk
        {
            pMedia = GetMedia('R');
            hbVolumes[FIELD___driveType].SetInteger(DriveType_Ram);
            hbVolumes[FIELD___totalSize].SetInteger((int64_t)pMedia->fx_media_memory_size);
            hbVolumes[FIELD___volumeIndex].SetInteger(2);
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(hbVolumes[2], driveInfoTypeDef));
        }
    }
    NANOCLR_NOCLEANUP();
}
