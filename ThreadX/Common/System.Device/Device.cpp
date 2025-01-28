//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "Device.h"

#define NOT_FOUND -1

static Device::DevicePin *GPIOPins;
static int NumberOfGPIOPins;

static Device::AdcPin *ADCChannels;
static int NumberOfADCChannels;
static Device::DacPin *DACChannels;
static int NumberOfDACChannels;
static Device::I2cPin *I2CChannels;
static int NumberOfI2CChannels;
static Device::PwmPin *PWMChannels;
static int NumberOfPWMChannels;
static Device::SerialPin *SerialChannels;
static int NumberOfSerialChannels;
static Device::SpiPin *SpiChannels;
static int NumberOfSpiChannels;

void Device::CreatePinList(DevicePin *mcuGPIOPinList, int numberOfPins)
{
    GPIOPins = mcuGPIOPinList;
    NumberOfGPIOPins = numberOfPins;
}
bool Device::AddPinParameters(PinNameValue pinNameValue, void *newParameters)
{
    bool status = false;
    int pin = FindPin(pinNameValue);
    if (FindPin(pinNameValue) != 0)
    {
        GPIOPins[pin].GpioParameterData = (GpioParameter *)newParameters;
        status = true;
    }
    return status;
}
void Device::CreateADCChannelList(AdcPin *BoardAdcChannels, int BoardnumberOfAdcChannels)
{
    ADCChannels = BoardAdcChannels;
    NumberOfADCChannels = BoardnumberOfAdcChannels;
}
void Device::CreateDACChannelList(DacPin *BoardDACChannels, int BoardnumberOfDACChannels)
{
    DACChannels = BoardDACChannels;
    NumberOfDACChannels = BoardnumberOfDACChannels;
}
void Device::CreateI2CChannelList(I2cPin *BoardI2CChannels, int BoardnumberOfI2CChannels)
{
    I2CChannels = BoardI2CChannels;
    NumberOfI2CChannels = BoardnumberOfI2CChannels;
}
void Device::CreatePWMChannelList(PwmPin *BoardPWMChannels, int BoardnumberOfPWMChannels)
{
    PWMChannels = BoardPWMChannels;
    NumberOfPWMChannels = BoardnumberOfPWMChannels;
}
void Device::CreateSPIChannelList(SpiPin *BoardSpiChannels, int BoardnumberOfSpiChannels)
{
    SpiChannels = BoardSpiChannels;
    NumberOfSpiChannels = BoardnumberOfSpiChannels;
}
void Device::CreateSerialChannelList(SerialPin *BoardSerialChannels, int BoardnumberOfSerialChannels)
{
    SerialChannels = BoardSerialChannels;
    NumberOfSerialChannels = BoardnumberOfSerialChannels;
}
bool Device::IsValidPin(PinNameValue pinNameValue)
{
    return (FindPin(pinNameValue) != NOT_FOUND);
}
int Device::NumberOfPins()
{
    return NumberOfGPIOPins;
}
int Device::FindPin(PinNameValue pinNameValue)
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
PinNameValue Device::FindPinWithFunctionAndChannel(DevicePinFunction dph, int deviceChannel)
{
    for (int pinIndex = 0; pinIndex < NumberOfPins(); pinIndex++)
    {
        if (GPIOPins[pinIndex].CurrentFunction == dph &&
            GPIOPins[pinIndex].DeviceFunctionChannelNumber == deviceChannel)
        {
            return GPIOPins[pinIndex].pinNameValue;
        }
    }
    return (PinNameValue)0;
}
bool Device::IsPinReserved(PinNameValue pinNameValue)
{
    int pinIndex = FindPin(pinNameValue);
    return GPIOPins[pinIndex].Reserved;
}
bool Device::ReservePin(
    PinNameValue pinNameValue,
    DevicePinFunction devicePinFunction,
    int deviceFunctionChannelNumber)
{
    bool status = false;
    int pinIndex = FindPin(pinNameValue);
    if (!IsPinReserved(pinNameValue))
    {
        GPIOPins[pinIndex].Reserved = true;
        GPIOPins[pinIndex].Mode = (PinMode)NOT_SET;
        GPIOPins[pinIndex].CurrentFunction = devicePinFunction;
        GPIOPins[pinIndex].DeviceFunctionChannelNumber = deviceFunctionChannelNumber;
        status = true;
    }
    return status;
}
Device::DevicePin Device::GetPin(PinNameValue pinNameValue)
{
    int pinIndex = FindPin(pinNameValue);
    return GPIOPins[pinIndex];
}

void Device::ReleasePin(PinNameValue pinNameValue)
{
    int pinIndex = FindPin(pinNameValue);
    if (IsPinReserved(pinNameValue))
    {
        GPIOPins[pinIndex].Reserved = false;
        GPIOPins[pinIndex].CurrentFunction = DevicePinFunction::NONE;
        GPIOPins[pinIndex].DeviceFunctionChannelNumber = 0;
    }
}
bool Device::IsValidOutputPin(PinNameValue pinNameValue)
{
    return IsValidOutputDriveMode(GPIOPins[FindPin(pinNameValue)].Mode);
}
bool Device::IsValidInputPin(PinNameValue pinNameValue)
{
    return IsValidInputDriveMode(GPIOPins[FindPin(pinNameValue)].Mode);
}
bool Device::IsValidOutputDriveMode(PinMode driveMode)
{
    return (
        driveMode == PinMode_Output || driveMode == PinMode_OutputOpenDrain ||
        driveMode == PinMode_OutputOpenDrainPullUp || driveMode == PinMode_OutputOpenSource ||
        driveMode == PinMode_OutputOpenSourcePullDown);
}
bool Device::IsValidInputDriveMode(PinMode driveMode)
{
    return (driveMode == PinMode_Input || driveMode == PinMode_InputPullDown || driveMode == PinMode_InputPullUp);
}
bool Device::IsValidADCDevice(int deviceIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfADCChannels; iChannel++)
    {
        if (ADCChannels->controllerNumber == deviceIndex)
        {
            status = true;
        }
        ADCChannels++;
    }
    return status;
}
bool Device::IsValidDACDevice(int deviceIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfDACChannels; iChannel++)
    {
        if (DACChannels->controllerNumber == deviceIndex)
        {
            status = true;
        }
        DACChannels++;
    }
    return status;
}
bool Device::IsValidI2CDevice(int deviceIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfI2CChannels; iChannel++)
    {
        if (I2CChannels->controllerNumber == deviceIndex)
        {
            status = true;
        }
        I2CChannels++;
    }
    return status;
}
bool Device::IsValidPWMDevice(int deviceIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfPWMChannels; iChannel++)
    {
        if (PWMChannels->controllerNumber == deviceIndex)
        {
            status = true;
        }
        PWMChannels++;
    }
    return status;
}
bool Device::IsValidSPIDevice(int deviceIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfSpiChannels; iChannel++)
    {
        if (SpiChannels->controllerNumber == deviceIndex)
        {
            status = true;
        }
        SpiChannels++;
    }
    return status;
}
bool Device::IsValidSerialBus(int deviceIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfSerialChannels; iChannel++)
    {
        if (SerialChannels->controllerNumber == deviceIndex)
        {
            status = true;
        }
        SerialChannels++;
    }
    return status;
}
bool Device::SetPinMode(PinNameValue pinNameValue, PinMode pinMode)
{
    int pinIndex = FindPin(pinNameValue);
    GPIOPins[pinIndex].Mode = pinMode;
    return true;
}
// bool AddPinParameters(PinNameValue pinNameValue, GpioParameter *newParameters)
//{
//     bool status = false;
//     int pinIndex = FindPin(pinNameValue);
//     if (FindPin(pinNameValue) != 0)
//     {
//         GPIOPins[pinIndex].GpioParameterData = newParameters;
//         status = true;
//     }
//     return status;
// }
bool Device::RemovePinParameters(PinNameValue pinNameValue)
{
    bool status = false;
    int pinIndex = FindPin(pinNameValue);
    if (pinIndex != 0)
    {
        DevicePin dp = GPIOPins[pinIndex];
        if (dp.GpioParameterData != NULL)
        {
            platform_free(dp.GpioParameterData);
            dp.GpioParameterData = NULL;
        }
        status = true;
    }
    return status;
}
unsigned int Device::countSetBits(unsigned int num)
{
    unsigned int count = 0;
    while (num)
    {
        count += num & 1;
        num >>= 1;
    }
    return count;
}

#ifdef FILEX
static Device::SDPin *SDChannels;
static int NumberOfSDChannels;
void Device::CreateSDChannelList(SDPin *BoardSDChannels, int BoardnumberOfSDChannels)
{
    SDChannels = BoardSDChannels;
    NumberOfSDChannels = BoardnumberOfSDChannels;
}
bool Device::IsValidSDBus(int deviceIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfSDChannels; iChannel++)
    {
        if (SDChannels->controllerNumber == deviceIndex)
        {
            status = true;
        }
        SDChannels++;
    }
    return status;
}
#endif
