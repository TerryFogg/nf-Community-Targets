
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "targetPAL.h"
#include "tx_api.h"
#include "tx_port.h"
#include "Math.h"
#include "board.h"
#include "DeviceIO.h"
#include "Device.h"

int NearestThreadxTickPeriod(int value);
bool RemovePreviousPinSetup(GPIO_PIN pinNumber);
void DebounceTimerCallback(ULONG pinNumber);
void GpioInterruptHandler(void *arg);

bool CPU_GPIO_Initialize()
{
    // Global Active Device pin list is set at boot time
    return true;
}
bool CPU_GPIO_Uninitialize()
{
    // What is Uninitialize mean/meant to do?
    // 2. This needs more thought on how to resume from some various states of sleep
    //
    return true;
}
bool CPU_GPIO_ReservePin(GPIO_PIN pinNumber, bool fReserve)
{
    bool status = false;
    if (fReserve)
    {
        status = ReservePin(pinNumber, DevicePinFunction::GPIO, 0);
    }
    else
    {
        status = ReleasePin(pinNumber);
    }
    return status;
}
void CPU_GPIO_SetPinState(GPIO_PIN pinNumber, GpioPinValue pinState)
{
    SetPinState(pinNumber, pinState);
}
void CPU_GPIO_TogglePinState(GPIO_PIN pinNumber)
{
    TogglePinState(pinNumber);
}
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, PinMode driveMode)
{
    return SetGpioDriveMode(pinNumber, driveMode);
}
bool CPU_GPIO_PinIsBusy(GPIO_PIN pinNumber)
{
    return IsPinReserved(pinNumber);
}
void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, PinMode driveMode, uint32_t  alternateFunction)
{
    (void)driveMode;
    // Functionality ONE:
    // Dispose and disable the pin ( Possibly rarely used)
    bool DisposeAndDisable = (alternateFunction == 0);
    if (DisposeAndDisable)
    {
        if (RemovePreviousPinSetup(pinNumber))
        {
            SetPinLowPower(pinNumber);
        }
        return;
    }

    // Functionality TWO:
    // Setup a pin for a specific function
    SetPinFunction(pinNumber, ( DevicePinFunction)alternateFunction, 0);
}
int32_t CPU_GPIO_GetPinCount()
{
    return NumberOfPins();
}
GpioPinValue CPU_GPIO_GetPinState(GPIO_PIN pinNumber)
{
    GpioPinValue pinValue = (GpioPinValue)-1;
    if (IsValidPin(pinNumber))
    {
        pinValue = GetPinState(pinNumber);
    }
    return pinValue;
}
int NearestThreadxTickPeriod(int value)
{
    if (value < 5)
        return 10;
    return (int)round((float)value / 10) * 10;
}
bool RemovePreviousPinSetup(GPIO_PIN pinNumber)
{
    bool status = false;
    GpioParameter *gpioParameter = GetPinParameters(pinNumber);
    if (gpioParameter != NULL && gpioParameter->isrPtr != NULL)
    {
        RemovePinInterrupt(pinNumber);
        RemovePinParameters(pinNumber);
    }
    return status;
}
bool CPU_GPIO_EnableInputPin(
    GPIO_PIN pinNumber,
    uint32_t debounceTimeMilliseconds,
    GPIO_INTERRUPT_SERVICE_ROUTINE pinISR,
    void *isrParam,
    GPIO_INT_EDGE intEdge,
    PinMode driveMode)
{
    bool interruptWithoutDebounce = (pinISR != NULL && debounceTimeMilliseconds == 0);
    bool interruptWithDebounce = (pinISR != NULL && debounceTimeMilliseconds != 0);

    bool status = false;
    if (IsValidInputDriveMode(driveMode))
    {
        GpioParameter *gpioParameter = (GpioParameter *)GetPinParameters(pinNumber);

        if (IsPinReserved(pinNumber))
        {
            bool HasPreviousTimer = (gpioParameter->debounceTimer.tx_timer_id != TX_CLEAR_ID);
            if (HasPreviousTimer)
            {
                tx_timer_delete(&gpioParameter->debounceTimer);
            }
            if (RemovePinInterrupt(pinNumber))
            {
                RemovePinParameters(pinNumber);
            }
        }
        CPU_GPIO_SetDriveMode(pinNumber, driveMode);

        // Initialize a new gpioParameter
        GpioParameter *newGpioParameter = (GpioParameter *)platform_malloc(sizeof(GpioParameter));
        AddPinParameters(pinNumber, newGpioParameter);

        newGpioParameter->edgeTrigger = intEdge;
        newGpioParameter->param = NULL;

        if (interruptWithoutDebounce)
        {
            newGpioParameter->isrPtr = pinISR;
            newGpioParameter->debounceMs = 0;
            newGpioParameter->waitingForDebounceToExpire = false;
            newGpioParameter->expectedState = GpioPinValue::GpioPinValue_High;
            newGpioParameter->param = (void *)isrParam;
        }
        else if (interruptWithDebounce)
        {
            newGpioParameter->isrPtr = pinISR;
            newGpioParameter->debounceMs = NearestThreadxTickPeriod(debounceTimeMilliseconds);
            newGpioParameter->waitingForDebounceToExpire = true;
            newGpioParameter->expectedState = GpioPinValue::GpioPinValue_Low;
            newGpioParameter->param = (void *)isrParam;
            tx_timer_create(
                &newGpioParameter->debounceTimer,
                (char *)"GPIO debounce timer",
                DebounceTimerCallback,
                pinNumber,
                0,
                1,
                TX_AUTO_ACTIVATE);
        }
        EnablePinInterrupt(pinNumber, newGpioParameter);
        status = true;
    }
    return status;
}
bool CPU_GPIO_EnableOutputPin(GPIO_PIN pinNumber, GpioPinValue InitialState, PinMode driveMode)
{
    bool status = false;
    if (IsValidOutputDriveMode(driveMode))
    {
        if (IsPinReserved(pinNumber))
        {
            if (RemovePinInterrupt(pinNumber))
            {
                status = RemovePinParameters(pinNumber);
            }
        }
        if (CPU_GPIO_SetDriveMode(pinNumber, driveMode))
        {
            CPU_GPIO_SetPinState(pinNumber, InitialState);
            status = true;
        }
    }
    return status;
}
uint32_t CPU_GPIO_GetPinDebounce(GPIO_PIN pinNumber)
{
    uint32_t debounceTime = 0;
    GpioParameter *gpioParameter = GetPinParameters(pinNumber);
    if (gpioParameter != NULL)
    {
        debounceTime = gpioParameter->debounceMs;
    }
    return debounceTime;
}
bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, uint32_t debounceTimeMilliseconds)
{
    bool result = false;
    GpioParameter *gpioParameter = GetPinParameters(pinNumber);
    if (gpioParameter != NULL)
    {
        gpioParameter->debounceMs = debounceTimeMilliseconds;
        result = true;
    }
    return result;
}
void GpioInterruptHandler(void *arg)
{
    DevicePin *dp = (DevicePin *)arg;
    GpioParameter *gpioParameter = GetPinParameters(dp->pinNumber);
    bool interruptWithoutDebounce = (gpioParameter->debounceMs = 0);

    if (interruptWithoutDebounce || gpioParameter->waitingForDebounceToExpire)
    {
        return;
    }
    // Signal the interrupt
    gpioParameter->isrPtr(dp->pinNumber, CPU_GPIO_GetPinState(dp->pinNumber), gpioParameter->param);

    // Restart debounce timer for next interrupt
    gpioParameter->waitingForDebounceToExpire = true;
    gpioParameter->expectedState = CPU_GPIO_GetPinState(dp->pinNumber);
    tx_timer_activate(&gpioParameter->debounceTimer);
}
void DebounceTimerCallback(ULONG pinNumber)
{
    GpioParameter *gpioParameter = GetPinParameters(pinNumber);
    if (gpioParameter != NULL)
    {
        bool currentPinState = CPU_GPIO_GetPinState(pinNumber);
        if (currentPinState == gpioParameter->expectedState)
        {
            gpioParameter->isrPtr(pinNumber, currentPinState, gpioParameter->param);

            if (gpioParameter->edgeTrigger == GPIO_INT_EDGE_BOTH)
            {
                gpioParameter->expectedState ^= 1;
            }
        }
        gpioParameter->waitingForDebounceToExpire = false;
    }
}
bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, PinMode driveMode)
{
    (void)pinNumber;
    // TODO: Improve based on pin number
    return (IsValidOutputDriveMode(driveMode) && IsValidInputDriveMode(driveMode));
}
