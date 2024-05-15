//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "sys_dev_pwm_native.h"
#include "board.h"
#include "DeviceIO.h"
#include "DeviceRegistration.h"

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        (void)stack;
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
HRESULT
Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetDesiredFrequency___VOID__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        uint32_t pwmID = pThis[FIELD___pwmTimer].NumericByRef().s4;
        int32_t desiredFrequency = stack.Arg1().NumericByRef().s4;

        if (int actualFrequency = SetDesiredFrequency(pwmID, desiredFrequency))
        {
            pThis[FIELD___frequency].NumericByRef().s4 = actualFrequency;
            stack.SetResult_R8((double)actualFrequency);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetActiveDutyCyclePercentage___VOID__R8(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int pwmId = pThis[FIELD___pwmTimer].NumericByRef().s4;
        int channelId = pThis[FIELD___channelNumber].NumericByRef().s4;
        double dutyCycle = stack.Arg1().NumericByRef().r8;
        double actualDutyCycle = 0.0;
        if ((actualDutyCycle = SetDutyCycle(pwmId, channelId,dutyCycle)))
        {
            pThis[FIELD___dutyCycle].NumericByRef().u4 = actualDutyCycle;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int pwmID = pThis[FIELD___pwmTimer].NumericByRef().s4;
        uint32_t dutyCycle = pThis[FIELD___dutyCycle].NumericByRef().u4;
        int channelId = pThis[FIELD___channelNumber].NumericByRef().s4;
        int pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;

        StartPwm(pwmID, dutyCycle, channelId, pinNumber);
    }

    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int pwmID = pThis[FIELD___pwmTimer].NumericByRef().s4;
        int channelId = pThis[FIELD___channelNumber].NumericByRef().s4;
        StopPwm(pwmID, channelId);
    }

    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;
    NANOCLR_HEADER();
    {
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::GetChannel___STATIC__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int pin = stack.Arg0().NumericByRef().s4;
        int pwmID = stack.Arg1().NumericByRef().s4;
        bool result = CheckChannel(pwmID, pin);
        stack.SetResult_I4(result);
    }
    NANOCLR_NOCLEANUP();
}
