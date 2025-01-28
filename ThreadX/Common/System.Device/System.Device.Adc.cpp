//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_adc_native.h"
#include <tx_api.h>
#include <tx_port.h>
#include "DeviceIO.h"
#include "Device.h"

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        AdcIO::Initialize();
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        int adc_channel = stack.Arg1().NumericByRef().s4;
        AdcIO::Open(adc_channel);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetChannelCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        stack.SetResult_I4(AdcIO::ChannelCount());
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMaxValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        stack.SetResult_I4(AdcIO::MaximumValue());
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMinValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        stack.SetResult_I4(AdcIO::MinimumValue());
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        AdcChannelMode adcChannelMode = (AdcChannelMode)stack.Arg1().NumericByRef().s4;
        // TODO: Add adc channel number to the input parameters
        //       Assume some channels can be single/differential and others single only
        stack.SetResult_Boolean(AdcIO::IsModeSupported(adcChannelMode));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetResolutionInBits___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        stack.SetResult_I4(AdcIO::Resolution());
    }
    NANOCLR_NOCLEANUP();
}

HRESULT
Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeReadValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposed].NumericByRef().u1);
        int adc_channel_Number = (stack.This())[FIELD___channelNumber].NumericByRef().s4;
        int32_t AdcReadValue = (int32_t)AdcIO::Read(adc_channel_Number);
        stack.SetResult_I4(AdcReadValue);
    }
    NANOCLR_NOCLEANUP();
};

HRESULT
Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposed].NumericByRef().u1);
        int adc_channel_Number = stack.This()[FIELD___channelNumber].NumericByRef().s4;
        if (!AdcIO::Dispose(adc_channel_Number))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
        }
    }
    NANOCLR_NOCLEANUP();
}
