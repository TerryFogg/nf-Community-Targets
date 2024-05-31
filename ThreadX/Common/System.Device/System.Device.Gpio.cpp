//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "sys_dev_gpio_native_target.h"
#include "sys_dev_gpio_native.h"
#include <targetPAL.h>
#include <corlib_native.h>
#include "nf_rt_events_native.h"
#include "sys_dev_gpio_native.h"
#include "DeviceIO.h"
#include "DeviceRegistration.h"
#include "tx_api.h"

typedef Library_sys_dev_gpio_native_System_Device_Gpio_PinValue PinValue;
typedef Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin GpioPin;
typedef Library_sys_dev_gpio_native_System_Device_Gpio_GpioPinBundle GpioPinBundle;

CLR_INT32 NearestThreadxTicks(int value);
void DebounceTimerCallback(ULONG pinNumber);
void Gpio_Interrupt_ISR(GPIO_PIN pin, bool pinState, void *pArg);
#pragma region GPIO Pin
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::NativeInit___BOOLEAN__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.Arg1().NumericByRef().s4;
        if (IsValidPin(pinNumber) && ReservePin(pinNumber))
        {
            GpioIO::Initialize(pinNumber);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        if (IsValidPin(pinNumber))
        {
            GpioParameter *gpioParameters = GetPinParameters(pinNumber);
            if (gpioParameters->debounceTimer.tx_timer_id != 0)
            {
                tx_timer_delete(&gpioParameters->debounceTimer);
            }
            RemovePinParameters(pinNumber);
            GpioIO::Dispose(pinNumber);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::Toggle___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        bool callbackRequested = stack.This()[FIELD___callbacks].Dereference() != NULL;
        if (IsValidPin(pinNumber) && IsValidOutputPin(pinNumber))
        {
            uint32_t pinValue = GpioIO::Toggle(pinNumber) ? 1 : 0;
            if (callbackRequested)
            {
                PostManagedEvent(EVENT_GPIO, 0, (uint16_t)pinNumber, pinValue);
            }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::
    NativeIsPinModeSupported___BOOLEAN__SystemDeviceGpioPinMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        PinMode driveMode = (PinMode)stack.Arg1().NumericByRef().s4;
        if (IsValidPin(pinNumber))
        {
            stack.SetResult_Boolean(IsValidOutputDriveMode(driveMode) || IsValidInputDriveMode(driveMode));
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::NativeSetPinMode___VOID__SystemDeviceGpioPinMode(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);

        PinNameValue pinNameValue = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        bool callbacksRegistered = stack.This()[FIELD___callbacks].Dereference() != NULL;
        PinMode pinMode = (PinMode)stack.Arg1().NumericByRef().s4;
        if (IsValidPin(pinNameValue) && IsValidInputDriveMode(pinMode))
        {
            CLR_INT64 *ptrDebounceValue =
                Library_corlib_native_System_TimeSpan::GetValuePtr(stack.This()[FIELD___debounceTimeout]);
            FAULT_ON_NULL(ptrDebounceValue);
            CLR_UINT64 debounceTimeoutMilliseconds = NearestThreadxTicks(*ptrDebounceValue);

            SetPinMode1(pinNameValue, pinMode);
            GpioIO::SetMode(pinNameValue, pinMode);
            GpioParameter *gpioParameter = GetPinParameters(pinNameValue);
            if (IsPinReserved(pinNameValue))
            {
                bool HasPreviousTimer = (gpioParameter->debounceTimer.tx_timer_id != TX_CLEAR_ID);
                if (HasPreviousTimer)
                {
                    tx_timer_delete(&gpioParameter->debounceTimer);
                }
                RemovePinParameters(pinNameValue);
                GpioIO::InterruptRemove(pinNameValue);
            }

            GpioParameter *newGpioParameter = (GpioParameter *)platform_malloc(sizeof(GpioParameter));
            newGpioParameter->isrPtr = &Gpio_Interrupt_ISR;
            newGpioParameter->debounceMs = TX_TICKS_PER_MILLISEC(debounceTimeoutMilliseconds);
            newGpioParameter->waitingForDebounceToExpire = false;
            newGpioParameter->param = NULL;
            newGpioParameter->edgeTrigger = GPIO_INT_EDGE_BOTH;

            if (debounceTimeoutMilliseconds > 0)
            {
                newGpioParameter->waitingForDebounceToExpire = true;
                tx_timer_create(
                    &newGpioParameter->debounceTimer,
                    (char *)"GPIO debounce timer",
                    DebounceTimerCallback,
                    (ULONG)pinNameValue,
                    0,
                    1,
                    TX_AUTO_ACTIVATE);
            }
            newGpioParameter->expectedState = GpioIO::InterruptEnable(
                pinNameValue,
                GPIO_INT_EDGE_BOTH,
                callbacksRegistered ? &Gpio_Interrupt_ISR : NULL);
            AddPinParameters(pinNameValue, newGpioParameter);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        stack.This()[FIELD___pinMode].NumericByRef().s4 = pinMode;
    }
    NANOCLR_NOCLEANUP();
}
// from the main version   ?// protect this from GC so that the callback is where it's supposed to
// from the main version   ?CLR_RT_ProtectFromGC gc(*stack.This());

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::NativeSetDebounceTimeout___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        CLR_INT64 *debounceValue =
            Library_corlib_native_System_TimeSpan::GetValuePtr(stack.This()[FIELD___debounceTimeout]);
        FAULT_ON_NULL(debounceValue);
        if (IsValidPin(pinNumber))
        {
            GpioParameter *gpioParameter = GetPinParameters(pinNumber);
            if (gpioParameter != NULL)
            {
                gpioParameter->debounceMs = *debounceValue / TIME_CONVERSION__TO_MILLISECONDS;
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::Read___SystemDeviceGpioPinValue(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        CLR_RT_HeapBlock &top = stack.PushValue();
        CLR_RT_TypeDef_Index pinValueTypeDef;
        CLR_RT_HeapBlock *hbObj;
        CLR_UINT8 pinValue;
        PinNameValue pinNumber = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        if (IsValidPin(pinNumber))
        {
            pinValue = GpioIO::Read(pinNumber) ? 1 : 0;
            g_CLR_RT_TypeSystem.FindTypeDef("PinValue", "System.Device.Gpio", pinValueTypeDef);
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, pinValueTypeDef));
            hbObj = top.Dereference();
            hbObj[Library_sys_dev_gpio_native_System_Device_Gpio_PinValue::FIELD___value].NumericByRef().u1 = pinValue;
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::WriteNative___VOID__SystemDeviceGpioPinValue(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        bool callbackRequested = stack.This()[FIELD___callbacks].Dereference() != NULL;
        bool pinState = stack.This()[PinValue::FIELD___value].NumericByRef().u1;
        if (IsValidPin(pinNumber) && IsValidOutputPin(pinNumber))
        {
            GpioIO::Write(pinNumber, pinState);
            if (callbackRequested)
            {
                PostManagedEvent(EVENT_GPIO, 0, (uint16_t)pinNumber, pinState);
            }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::NativeSetAlternateFunction___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        DevicePinFunction function = (DevicePinFunction)stack.Arg1().NumericByRef().s4;
        if (IsValidPin(pinNumber))
        {
            GpioIO::SetFunction(pinNumber, function);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
#pragma endregion

#pragma region GPIO Controller
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::get_PinCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        stack.SetResult_I4(NumberOfPins());
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::
    IsPinModeSupported___BOOLEAN__I4__SystemDeviceGpioPinMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinMode driveMode;
        driveMode = (PinMode)stack.Arg2().NumericByRef().s4;
        stack.SetResult_Boolean(IsValidOutputDriveMode(driveMode) || IsValidInputDriveMode(driveMode));
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::IsPinOpen___BOOLEAN__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.Arg1().NumericByRef().s4;
        stack.SetResult_Boolean(IsPinReserved(pinNumber));
    }
    NANOCLR_NOCLEANUP();
}
HRESULT
Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::SetPinMode___VOID__I4__SystemDeviceGpioPinMode(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.Arg1().NumericByRef().s4;
        PinMode pinMode = (PinMode)stack.Arg2().NumericByRef().s4;
        GpioIO::SetMode(pinNumber, pinMode);
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::NativeRead___U1__I4(CLR_RT_StackFrame &stack)
{
    // Reads the current value of a pin.
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.Arg1().NumericByRef().s4;

        if (IsValidPin(pinNumber))
        {
            bool pinState = GpioIO::Read(pinNumber);
            stack.SetResult_I4(pinState);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::NativeWrite___VOID__I4__U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);
        PinNameValue pinNumber = (PinNameValue)stack.Arg1().NumericByRef().s4;
        bool pinState = (GpioPinValue)stack.Arg2().NumericByRef().u1;
        if (IsValidPin(pinNumber))
        {
            GpioIO::Write(pinNumber, pinState);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}
#pragma endregion

#pragma region Local support code
void Gpio_Interrupt_ISR(GPIO_PIN pinNumber, bool pinState, void *pArg)
{
    (void)pArg;
    PostManagedEvent(EVENT_GPIO, 0, pinNumber, pinState);
}
CLR_INT32 NearestThreadxTicks(int milliseconds)
{
    return (milliseconds + 9) / TX_TIMER_TICKS_PER_SECOND;
}
void DebounceTimerCallback(ULONG callBackValue)
{
    PinNameValue pinNameValue = (PinNameValue)callBackValue;
    GpioParameter *gpioParameter = GetPinParameters(pinNameValue);
    if (gpioParameter != NULL)
    {
        CLR_UINT16 pinValue = GpioIO::Read(pinNameValue);

        if (pinValue == gpioParameter->expectedState)
        {
            gpioParameter->isrPtr(pinValue, pinValue, gpioParameter->param);
            if (gpioParameter->edgeTrigger == GPIO_INT_EDGE_BOTH)
            {
                gpioParameter->expectedState ^= 1;
            }
        }
        gpioParameter->waitingForDebounceToExpire = false;
    }
}
#pragma endregion
