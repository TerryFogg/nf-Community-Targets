//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_dac_native.h"
#include "DeviceIO.h"

HRESULT
Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        // TODO: Is channel a parameter here?
        CLR_INT32 dac_channel_number = stack.Arg1().NumericByRef().s4;
        CLR_INT32 controllerId = stack.This()[FIELD___controllerId].NumericByRef().s4;
        (void)controllerId;
        (void)dac_channel_number;
        if (!DacIO::Initialize(dac_channel_number))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeDispose___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int dac_channel_Number = pThis[FIELD___channelNumber].NumericByRef().s4;

        if (!DacIO::Dispose(dac_channel_Number))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_INT32 controllerId = stack.This()[FIELD___controllerId].NumericByRef().s4;
        int dac_channel_number = stack.Arg1().NumericByRef().s4;
        if (!DacIO::Open(controllerId, dac_channel_number))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }
    }
    NANOCLR_NOCLEANUP();
};
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetChannelCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        stack.SetResult_I4(DacIO::ChannelCount());
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetResolutionInBits___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        stack.SetResult_I4(DacIO::Resolution());
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_dev_dac_native_System_Device_Dac_DacController::GetDeviceSelector___STATIC__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        int dac_channel_Number = stack.Arg1().NumericByRef().s4;
        char *deviceName = DacIO::DeviceSelector(dac_channel_Number);

        if (deviceName == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }
        else
        {
            stack.SetResult_String(deviceName);

        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeWriteValue___VOID__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        int dac_channel_Number = stack.Arg1().NumericByRef().s4;
        uint16_t value = stack.Arg1().NumericByRef().u2;
        DacIO::Write(dac_channel_Number, value);
    }
    NANOCLR_NOCLEANUP();
}

