//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "sys_dev_adc_native.h"
#include <tx_api.h>
#include <tx_port.h>
#include "DeviceIO.h"


HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;
    NANOCLR_HEADER();
    {
        // No parameters passed, ADC is handled in OpenChannel call
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeOpenChannel___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int channelNumber = stack.Arg1().NumericByRef().s4;
        OpenAdcChannel(channelNumber);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeReadValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;
        uint16_t AdcValue = AdcReadValue(channelNumber);
        stack.SetResult_I4(AdcValue);
    }
    NANOCLR_NOCLEANUP();
};
HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;
    NANOCLR_HEADER();
    {
        // all required initialization for ADC is handled in OpenChannel call
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetChannelCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int value = GetAdcChannelCount();
        stack.SetResult_I4(value);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMaxValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int value = GetAdcSupportedMaxValue();
        stack.SetResult_I4(value);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetMinValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int value = GetAdcSupportedMinValue();
        stack.SetResult_I4(value);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeIsChannelModeSupported___BOOLEAN__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        AdcChannelMode mode = (AdcChannelMode)stack.Arg1().NumericByRef().s4;
        bool IsSupported = IsAdcModeSupported(mode);
        stack.SetResult_Boolean(IsSupported);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeGetResolutionInBits___I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
         int value = GetAdcSupportedResolution();
        stack.SetResult_I4(value);
    }
    NANOCLR_NOCLEANUP();
}
