//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "sys_dev_pwm_native.h"
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <targetPAL.h>
#include <tx_api.h>
#include <tx_port.h>

HRESULT
Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID(
    CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    gpio_set_function(0, GPIO_FUNC_PWM);

    int desiredFrequency = pThis[FIELD___frequency].NumericByRef().s4;
    int dutyCycle = pThis[FIELD___dutyCycle].NumericByRef().s4;
    int pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;
    int timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    int channelId = pThis[FIELD___channelNumber].NumericByRef().s4;
    int polarity = pThis[FIELD___polarity].NumericByRef().s4;

    gpio_set_function(pinNumber, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pinNumber);

    if (polarity == 0) {
      pwm_set_output_polarity(slice_num, true, true);
    } else {
      pwm_set_output_polarity(slice_num, false, false);
    }

    // Set period of 4 cycles (0 to 3 inclusive)
    int wrapNumber = dutyCycle; // ??????????????????????
    pwm_set_wrap(slice_num, wrapNumber);

    pwm_set_enabled(slice_num, true);
  }
  NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::
    NativeSetDesiredFrequency___VOID__I4(CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    int desiredFrequency = stack.Arg1().NumericByRef().s4;
    if (desiredFrequency < 0) {
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    int timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    int wrapNumber = desiredFrequency; // ??????????????????????
    int slice_num = timerId;
    pwm_set_wrap(slice_num, wrapNumber);
  }
  NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::
    NativeSetActiveDutyCyclePercentage___VOID__R8(CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
  }
  NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID(
    CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    int timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    int slice_num = timerId;
    pwm_set_enabled(slice_num, true);
  }
  NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID(
    CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    int timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    int slice_num = timerId;
    pwm_set_enabled(slice_num, false);
  }
  NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID(
    CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    // Nothing to do
  }
  NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::
    GetChannel___STATIC__I4__I4__I4(CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    // Get pin and potential TIM
    int pin = stack.Arg0().NumericByRef().s4;
    int timerId = stack.Arg1().NumericByRef().s4;

    // Check if the combination is ok and set the result
    stack.SetResult_I4(-99); // ??????????????????
  }
  NANOCLR_NOCLEANUP();
}
