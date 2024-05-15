// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nanoCLR_Types.h"
#include "sys_dev_dac_native.h"
#include "DeviceIO.h"

#pragma region DacController
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int controllerId = pThis[FIELD___controllerId].NumericByRef().s4;
        switch (controllerId)
        {
            case 1:
            case 2:
            case 3:
                InitDacChannel(controllerId);
                break;
            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int dacChannel = stack.Arg1().NumericByRef().s4;
        int controllerId = pThis[FIELD___controllerId].NumericByRef().s4;
        OpenDacChannel(controllerId, dacChannel);
    }
    NANOCLR_NOCLEANUP();
};
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetChannelCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int controllerId = pThis[FIELD___controllerId].NumericByRef().s4;
        int value = GetDacChannelCount(controllerId);
        stack.SetResult_I4(value);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::NativeGetResolutionInBits___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int controllerId = pThis[FIELD___controllerId].NumericByRef().s4;
        int value = GetDacSupportedResolution(controllerId);
        stack.SetResult_I4(value);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacController::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // because the caller is expecting a result to be returned
        // we need set a return result in the stack argument using the appropriate SetResult according to the variable
        // type (a string here)
        stack.SetResult_String("????");
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
#pragma endregion

#pragma region DacChannel
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeWriteValue___VOID__U2(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;
        CLR_RT_HeapBlock *dacController = pThis[FIELD___dacController].Dereference();
        int controllerId =
            dacController[Library_sys_dev_dac_native_System_Device_Dac_DacController::FIELD___controllerId]
                .NumericByRef()
                .s4;
        uint16_t dacValue = stack.Arg1().NumericByRef().u2;

        WriteDacValue(controllerId, channelNumber, dacValue);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_dac_native_System_Device_Dac_DacChannel::NativeDispose___VOID__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // get disposeController
        bool disposeController = (bool)stack.Arg0().NumericByRef().u1;
        (void)disposeController;
        int channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;
        int controllerId = 0; // ????????
        switch (controllerId)
        {
            case 1:
            case 2:
            case 3:
                StopDac(controllerId, channelNumber);
                break;
            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
#pragma endregion