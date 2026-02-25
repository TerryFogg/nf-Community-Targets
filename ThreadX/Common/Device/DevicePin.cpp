//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "DevicePin.h"

#define NOT_FOUND -1

static DeviceGpioPin *GPIOPins;
static int NumberOfGPIOPins;

void DevicePin::CreatePinList(DeviceGpioPin *mcuGPIOPinList, int numberOfPins)
{
    GPIOPins = mcuGPIOPinList;
    NumberOfGPIOPins = numberOfPins;
}
bool DevicePin::IsValidPin(PinNameValue pinNameValue)
{
    return (FindPin(pinNameValue) != NOT_FOUND);
}
int DevicePin::NumberOfPins()
{
    return NumberOfGPIOPins;
}
int DevicePin::FindPin(PinNameValue pinNameValue)
{
    for (int pinIndex = 0; pinIndex < NumberOfGPIOPins; pinIndex++)
    {
        if (GPIOPins[pinIndex].pinNameValue == pinNameValue)
        {
            return pinIndex;
        }
    }
    return NOT_FOUND;
}
bool DevicePin::AddPinCallbackParameter(PinNameValue pinNameValue, void *newCallbackParameters)
{
    bool status = false;
    int pin = FindPin(pinNameValue);
    if (pin != NOT_FOUND)
    {
        GPIOPins[pin].GpioCallbackParameters = (GpioCallbackParameter *)newCallbackParameters;
        status = true;
    }
    return status;
}
bool DevicePin::RemovePinCallbackParameters(PinNameValue pinNameValue)
{
    bool status = false;
    int pinIndex = FindPin(pinNameValue);
    if (pinIndex != NOT_FOUND)
    {
        DeviceGpioPin dp = GPIOPins[pinIndex];
        if (dp.GpioCallbackParameters != NULL)
        {
            platform_free(dp.GpioCallbackParameters);
            dp.GpioCallbackParameters = NULL;
        }
        status = true;
    }
    return status;
}
bool DevicePin::IsPinReserved(PinNameValue pinNameValue)
{
    int pinIndex = FindPin(pinNameValue);
    return GPIOPins[pinIndex].Reserved;
}
bool DevicePin::ReservePin(PinNameValue pinNameValue)
{
    int pinIndex = FindPin(pinNameValue);
    if (!IsPinReserved(pinNameValue))
    {
        GPIOPins[pinIndex].Reserved = true;
        return true;
    }
    return false;
}
DeviceGpioPin DevicePin::GetPin(PinNameValue pinNameValue)
{
    int pinIndex = FindPin(pinNameValue);
    return GPIOPins[pinIndex];
}
void DevicePin::ReleasePin(PinNameValue pinNameValue)
{
    int pinIndex = FindPin(pinNameValue);
    if (IsPinReserved(pinNameValue))
    {
        GPIOPins[pinIndex].Reserved = false;
        GPIOPins[pinIndex].Function = DevicePinFunction::LOW_POWER;
    }
}
bool DevicePin::IsValidOutputPin(PinNameValue pinNameValue)
{
    return IsValidOutputDriveMode(GPIOPins[FindPin(pinNameValue)].Mode);
}
bool DevicePin::IsValidInputPin(PinNameValue pinNameValue)
{
    return IsValidInputDriveMode(GPIOPins[FindPin(pinNameValue)].Mode);
}
bool DevicePin::IsValidOutputDriveMode(PinMode driveMode)
{
    return (
        driveMode == PinMode_Output || driveMode == PinMode_OutputOpenDrain ||
        driveMode == PinMode_OutputOpenDrainPullUp || driveMode == PinMode_OutputOpenSource ||
        driveMode == PinMode_OutputOpenSourcePullDown);
}
bool DevicePin::IsValidInputDriveMode(PinMode driveMode)
{
    return (driveMode == PinMode_Input || driveMode == PinMode_InputPullDown || driveMode == PinMode_InputPullUp);
}
bool DevicePin::RegisterPinMode(PinNameValue pinNameValue, PinMode pinMode)
{
    int pinIndex = FindPin(pinNameValue);
    GPIOPins[pinIndex].Mode = pinMode;
    return true;
}
bool DevicePin::RegisterPinFunction(PinNameValue pinNameValue,DevicePinFunction function)
{
    int pinIndex = FindPin(pinNameValue);
    GPIOPins[pinIndex].Function = function;
    return true;
}

#ifdef FILEX
static DevicePin::SDPin *SDChannels;
static int NumberOfSDChannels;
void DevicePin::CreateSDChannelList(SDPin *BoardSDChannels, int BoardnumberOfSDChannels)
{
    SDChannels = BoardSDChannels;
    NumberOfSDChannels = BoardnumberOfSDChannels;
}
#endif

#ifdef SD_CARD
static DevicePin::SDPin *SDChannels;
static int NumberOfSDChannels;
void DevicePin::CreateSDChannelList(SDPin *BoardSDChannels, int BoardnumberOfSDChannels)
{
    SDChannels = BoardSDChannels;
    NumberOfSDChannels = BoardnumberOfSDChannels;
}
#endif

