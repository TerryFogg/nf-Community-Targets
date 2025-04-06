//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoCLR_Types.h"
#include "sys_dev_pwm_native.h"
#include "DeviceIO.h"
#include "Device.h"

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_INT32 channelId = stack.This()[FIELD___channelNumber].NumericByRef().s4;
        CLR_INT32 pinNumber = stack.This()[FIELD___pinNumber].NumericByRef().s4;
        CLR_INT32 timerId = stack.This()[FIELD___pwmTimer].NumericByRef().s4;
        CLR_INT32 polarity = stack.This()[FIELD___polarity].NumericByRef().s4;
        CLR_INT32 desiredFrequency = stack.This()[FIELD___frequency].NumericByRef().s4;
        CLR_INT32 dutyCycle = stack.This()[FIELD___dutyCycle].NumericByRef().s4;

        if (!PwmIO::Initialize(channelId, timerId, pinNumber, polarity, desiredFrequency, dutyCycle))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_INT32 channelId = stack.This()[FIELD___channelNumber].NumericByRef().s4;
        PwmIO::Dispose(channelId);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetActiveDutyCyclePercentage___VOID__R8(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        double percentage = stack.Arg1().NumericByRef().r8;
        if (percentage < 0 || percentage > 1.0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        CLR_INT32 dutyCycle = (CLR_INT32)(percentage * CONST_DutyCycleFactor);
        CLR_INT32 pinNumber = stack.This()[FIELD___pinNumber].NumericByRef().s4;

        CLR_UINT32 actual_duty_cycle = PwmIO::SetDutyCycle(pinNumber, dutyCycle);
        stack.This()[FIELD___dutyCycle].NumericByRef().u4 = actual_duty_cycle;
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_INT32 pinNumber = stack.This()[FIELD___pinNumber].NumericByRef().s4;
        CLR_INT32 timerId = stack.This()[FIELD___pwmTimer].NumericByRef().s4;
        PwmIO::Start(pinNumber, timerId);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_INT32 pinNumber = stack.This()[FIELD___pinNumber].NumericByRef().s4;
        CLR_INT32 timerId = stack.This()[FIELD___pwmTimer].NumericByRef().s4;
        PwmIO::Stop(pinNumber, timerId);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetDesiredFrequency___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        int timerId = stack.This()[FIELD___pwmTimer].NumericByRef().s4;
        int desiredFrequency = stack.Arg1().NumericByRef().s4;
        if (desiredFrequency <= 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        CLR_UINT32 actual_frequency = PwmIO::SetDutyCycle(timerId, desiredFrequency);
        stack.This()[FIELD___frequency].NumericByRef().s4 = actual_frequency;
        stack.SetResult_R8((double)actual_frequency);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::GetChannel___STATIC__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        // Get pin and potential TIM
        CLR_INT32 pin = stack.Arg0().NumericByRef().s4;
        CLR_INT32 timerId = stack.Arg1().NumericByRef().s4;
        CLR_INT32 channel = PwmIO::GetChannel(timerId, pin);
        stack.SetResult_I4(channel);
    }
    NANOCLR_NOCLEANUP();
}
