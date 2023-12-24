//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_adc_native.h"
#include <hardware/gpio.h>
#include "hardware/adc.h"
#include <tx_api.h>
#include <tx_port.h>

// 5 Adc channels, 4 on pins 1 temperature sensor
// Channel 0 = GPIO26
// Channel 1 = GPIO27
// Channel 2 = GPIO28
// Channel 3 = GPIO29 
// Channel 4 - onboard temperature sensor
#define ADC_CHANNEL_COUNT 5

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::NativeInit___VOID(
    CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    adc_init();
  }
  NANOCLR_NOCLEANUP();
}

HRESULT
Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeReadValue___I4(
    CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();

  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    int channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;
    adc_select_input(channelNumber);
    int32_t AdcReadValue = (int32_t)adc_read();
    stack.SetResult_I4(AdcReadValue);
  }
  NANOCLR_NOCLEANUP();
};

HRESULT
Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::
    NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    // Nothing to do
  }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::
    NativeOpenChannel___VOID__I4(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    int channel = stack.Arg1().NumericByRef().s4;
    adc_gpio_init(channel);
  }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::
      NativeGetChannelCount___I4(CLR_RT_StackFrame &stack) 
{
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    stack.SetResult_I4(ADC_CHANNEL_COUNT);
  }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::
    NativeGetMaxValue___I4(CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    // 12 bit(8.7 ENOB)
    stack.SetResult_I4(4095);
  }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::
    NativeGetMinValue___I4(CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    stack.SetResult_I4(0);
  }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::
    NativeIsChannelModeSupported___BOOLEAN__I4(CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    // Requested mode
    int mode = stack.Arg1().NumericByRef().s4;
    // Single ended mode for now
    stack.SetResult_Boolean((mode == (int)AdcChannelMode_SingleEnded));
  }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcController::
    NativeGetResolutionInBits___I4(CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);
    stack.SetResult_I4(12);
  }
  NANOCLR_NOCLEANUP();
}
