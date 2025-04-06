//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_gpio_native_target.h"
#include "sys_dev_gpio_native.h"
#include <targetPAL.h>
#include <corlib_native.h>
#include "nf_rt_events_native.h"
#include "DeviceIO.h"
#include "Device.h"
#include "tx_api.h"

typedef Library_sys_dev_gpio_native_System_Device_Gpio_PinValue PinValue;
typedef Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin GpioPin;
typedef Library_sys_dev_gpio_native_System_Device_Gpio_GpioPinBundle GpioPinBundle;

CLR_INT32 NearestThreadxTicks(int value);
void DebounceTimerCallback(ULONG pinNumber);

#pragma region GPIO Pin
HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::NativeInit___BOOLEAN__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        FAULT_ON_OBJECT_DISPOSED(stack.This()[FIELD___disposedValue].NumericByRef().u1);

        PinNameValue pinNumber = (PinNameValue)stack.Arg1().NumericByRef().s4;
        if (Device::IsValidPin(pinNumber) && Device::ReservePin(pinNumber))
        {
            GpioIO::InitializePin(pinNumber);
            stack.SetResult_Boolean(true);
        }
        else
        {
            stack.SetResult_Boolean(false);
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
        if (Device::IsValidPin(pinNumber))
        {
            DeviceGpioPin gpioPin = Device::GetPin(pinNumber);
            GpioCallbackParameter *gpioParameters = gpioPin.GpioCallbackParameters;
            if (gpioParameters->debounceTimer.tx_timer_id != 0)
            {
                // Delete existing debounce timer
                tx_timer_delete(&gpioParameters->debounceTimer);
            }
            Device::RemovePinCallbackParameters(pinNumber);
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
        if (Device::IsValidPin(pinNumber) && Device::IsValidOutputPin(pinNumber))
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
        if (Device::IsValidPin(pinNumber))
        {
            stack.SetResult_Boolean(
                Device::IsValidOutputDriveMode(driveMode) || Device::IsValidInputDriveMode(driveMode));
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

        if (Device::IsValidPin(pinNameValue) &&
            (Device::IsValidInputDriveMode(pinMode) || Device::IsValidOutputDriveMode(pinMode)))
        {
            CLR_INT64 *ptrDebounceValue =
                Library_corlib_native_System_TimeSpan::GetValuePtr(stack.This()[FIELD___debounceTimeout]);
            FAULT_ON_NULL(ptrDebounceValue);
            CLR_UINT64 debounceTimeoutTicks = NearestThreadxTicks(*ptrDebounceValue);

            DeviceGpioPin gpioPin = Device::GetPin(pinNameValue);
            GpioCallbackParameter *gpioParameter = gpioPin.GpioCallbackParameters;
            bool debounceRequested = debounceTimeoutTicks > 0;
            bool previouslyUsed = gpioParameter != NULL;

            if (previouslyUsed)
            {
                bool HasPreviousTimer = (gpioParameter->debounceTimer.tx_timer_id != TX_CLEAR_ID);
                if (HasPreviousTimer)
                {
                    tx_timer_delete(&gpioParameter->debounceTimer);
                }
                // Remove previous parameters
                Device::RemovePinCallbackParameters(pinNameValue);
                GpioIO::InterruptRemove(pinNameValue);
            }

            Device::RegisterPinMode(pinNameValue, pinMode);
            GpioIO::SetMode(pinNameValue, pinMode);

            if (debounceRequested || callbacksRegistered)
            {
                GpioCallbackParameter *newGpioParameter =
                    (GpioCallbackParameter *)platform_malloc(sizeof(GpioCallbackParameter));
                memset(newGpioParameter, 0, sizeof(GpioCallbackParameter));
                Device::AddPinCallbackParameter(pinNameValue, newGpioParameter);

                newGpioParameter->callBack = false;
                newGpioParameter->debounceMs = 0;
                newGpioParameter->edgeTrigger = GPIO_INT_NONE;
                if (debounceRequested)
                {
                    newGpioParameter->debounceMs = debounceTimeoutTicks;
                    tx_timer_create(
                        &newGpioParameter->debounceTimer,
                        (char *)"GPIO debounce timer",
                        DebounceTimerCallback,
                        (ULONG)pinNameValue,
                        0,
                        1,
                        TX_AUTO_ACTIVATE);
                    newGpioParameter->expectedState = GpioIO::Read(pinNameValue);
                }
                if (callbacksRegistered)
                {
                    newGpioParameter->callBack = true;
                    newGpioParameter->edgeTrigger = GPIO_INT_EDGE_BOTH;
                    GpioIO::InterruptEnable(pinNameValue, GPIO_INT_EDGE_BOTH);
                }
            }
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
        if (Device::IsValidPin(pinNumber))
        {
            DeviceGpioPin gpioPin = Device::GetPin(pinNumber);
            GpioCallbackParameter *gpioParameter = gpioPin.GpioCallbackParameters;

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
        PinNameValue pinNameValue = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        if (Device::IsValidPin(pinNameValue))
        {
            pinValue = GpioIO::Read(pinNameValue) ? 1 : 0;
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
        PinNameValue pinNameValue = (PinNameValue)stack.This()[FIELD___pinNumber].NumericByRefConst().s4;
        CLR_RT_HeapBlock *pinValue = stack.Arg1().Dereference();
        bool callbackRequested = stack.This()[FIELD___callbacks].Dereference() != NULL;
        bool pinState = (GpioPinValue)pinValue[PinValue::FIELD___value].NumericByRef().u1;

        if (Device::IsValidPin(pinNameValue) && Device::IsValidOutputPin(pinNameValue))
        {
            GpioIO::Write(pinNameValue, pinState);
            if (callbackRequested)
            {
                PostManagedEvent(EVENT_GPIO, 0, (uint16_t)pinNameValue, pinState);
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
        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
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
        stack.SetResult_I4(Device::NumberOfPins());
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
        stack.SetResult_Boolean(Device::IsValidOutputDriveMode(driveMode) || Device::IsValidInputDriveMode(driveMode));
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
        stack.SetResult_Boolean(Device::IsPinReserved(pinNumber));
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

        if (Device::IsValidPin(pinNumber))
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
        if (Device::IsValidPin(pinNumber))
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
CLR_INT32 NearestThreadxTicks(int milliseconds)
{
    return milliseconds;
}
void DebounceTimerCallback(ULONG callBackValue)
{
    PinNameValue pinNameValue = (PinNameValue)callBackValue;
    DeviceGpioPin gpioPin = Device::GetPin(pinNameValue);
    GpioCallbackParameter *gpioParameter = gpioPin.GpioCallbackParameters;

    if (gpioParameter != NULL)
    {
        CLR_UINT16 pinValue = GpioIO::Read(pinNameValue);

        if (pinValue == gpioParameter->expectedState)
        {
            if (gpioParameter->callBack)
            {
                Callback_EVENT_GPIO(pinValue, pinValue);
            }
            if (gpioParameter->edgeTrigger == GPIO_INT_EDGE_BOTH)
            {
                gpioParameter->expectedState ^= 1;
            }
        }
        gpioParameter->waitingForDebounceToExpire = false;
    }
}
#pragma endregion
