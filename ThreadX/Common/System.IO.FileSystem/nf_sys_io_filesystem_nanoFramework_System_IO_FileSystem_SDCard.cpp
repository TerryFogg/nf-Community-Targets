//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_sys_io_filesystem.h"
#include "FileSystem.h"

HRESULT
Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::InitNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        bool useCardDetect = (bool)(stack.This()[FIELD___enableCardDetectPin].NumericByRef().s4);
        int cardDetectPin = (int)(stack.This()[FIELD___cardDetectPin].NumericByRef().s4);
        SDCard_SDInterfaceType cardType = (SDCard_SDInterfaceType)stack.This()[FIELD___sdCardType].NumericByRef().s4;
        SDCard_SDDataWidth dataWith = (SDCard_SDDataWidth)stack.This()[FIELD___dataWidth].NumericByRef().s4;

        switch (cardType)
        {
            case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Mmc:
                if (InitializeSDCard(cardType, cardDetectPin, useCardDetect, dataWith))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                    // NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
                }
                break;
            case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Spi:
                // TODO
                break;
            default:
                break;
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        (void)stack;
        // TODO
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::MountNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());

        if (stack.This()[FIELD___disposed].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        stack.This()[FIELD___mounted].NumericByRef().s4 = 0;

        SDCard_SDInterfaceType cardType = (SDCard_SDInterfaceType)stack.This()[FIELD___sdCardType].NumericByRef().s4;
        SDCard_SDDataWidth dataWith = (SDCard_SDDataWidth)stack.This()[FIELD___dataWidth].NumericByRef().s4;

        switch (cardType)
        {
            case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Mmc:
                if (MountSDCard(cardType,dataWith))
                {
                    stack.This()[FIELD___mounted].NumericByRef().s4 = 1;
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                    NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
                }
                break;
            case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Spi:
                // TODO
                break;
            default:
                break;
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::UnmountNative___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());

        if (stack.This()[FIELD___disposed].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }
        stack.This()[FIELD___mounted].NumericByRef().s4 = 0;
        SDCard_SDInterfaceType cardType = (SDCard_SDInterfaceType)stack.This()[FIELD___sdCardType].NumericByRef().s4;

        switch (cardType)
        {
            case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Mmc:
                if (UnMountSDCard(cardType))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                    NANOCLR_SET_AND_LEAVE(CLR_E_PIN_UNAVAILABLE);
                }
                break;
            case SDCard_SDInterfaceType::SDCard_SDInterfaceType_Spi:
                // TODO
                break;
            default:
                break;
        }
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}

__nfweak HRESULT Library_nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard::PollCardDetectNative___BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        if (stack.This()[FIELD___disposed].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }
        if (!stack.This()[FIELD___enableCardDetectPin].NumericByRef().s4)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
        int cardDetectPin = (int)stack.This()[FIELD___cardDetectPin].NumericByRef().s4;
        bool detected = PollDetectSDCard(cardDetectPin);
        stack.SetResult_Boolean(detected);
    }
    NANOCLR_CLEANUP();
    NANOCLR_CLEANUP_END();
}
