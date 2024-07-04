//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "DeviceRegistration.h"

DeviceRegistration::DevicePin *GPIOPins;
static int NumberOfGPIOPins;

static DeviceRegistration::AdcPin *ADCChannels;
static int NumberOfADCChannels;
static DeviceRegistration::DacPin *DACChannels;
static int NumberOfDACChannels;
static DeviceRegistration::I2cPin *I2CChannels;
static int NumberOfI2CChannels;
static DeviceRegistration::PwmPin *PWMChannels;
static int NumberOfPWMChannels;
static DeviceRegistration::SerialPin *SerialChannels;
static int NumberOfSerialChannels;
static DeviceRegistration::SpiPin *SpiChannels;
static int NumberOfSpiChannels;

void DeviceRegistration::CreatePinList(DevicePin *mcuGPIOPinList, int numberOfPins)
{
    GPIOPins = mcuGPIOPinList;
    NumberOfGPIOPins = numberOfPins;
}
bool DeviceRegistration::AddPinParameters(PinNameValue pinNameValue, void *newParameters)
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

void DeviceRegistration::CreateADCChannelList(AdcPin *BoardAdcChannels, int BoardnumberOfAdcChannels)
{
    ADCChannels = BoardAdcChannels;
    NumberOfADCChannels = BoardnumberOfAdcChannels;
}
void DeviceRegistration::CreateDACChannelList(DacPin *BoardDACChannels, int BoardnumberOfDACChannels)
{
    DACChannels = BoardDACChannels;
    NumberOfDACChannels = BoardnumberOfDACChannels;
}
void DeviceRegistration::CreateI2CChannelList(I2cPin *BoardI2CChannels, int BoardnumberOfI2CChannels)
{
    I2CChannels = BoardI2CChannels;
    NumberOfI2CChannels = BoardnumberOfI2CChannels;
}
void DeviceRegistration::CreatePWMChannelList(PwmPin *BoardPWMChannels, int BoardnumberOfPWMChannels)
{
    PWMChannels = BoardPWMChannels;
    NumberOfPWMChannels = BoardnumberOfPWMChannels;
}
void DeviceRegistration::CreateSPIChannelList(SpiPin *BoardSpiChannels, int BoardnumberOfSpiChannels)
{
    SpiChannels = BoardSpiChannels;
    NumberOfSpiChannels = BoardnumberOfSpiChannels;
}
void DeviceRegistration::CreateSerialChannelList(SerialPin *BoardSerialChannels, int BoardnumberOfSerialChannels)
{
    SerialChannels = BoardSerialChannels;
    NumberOfSerialChannels = BoardnumberOfSerialChannels;
}
bool DeviceRegistration::IsValidPin(PinNameValue pinNameValue)
{
    return (FindPin(pinNameValue) >= 0);
}
int DeviceRegistration::NumberOfPins()
{
    return NumberOfGPIOPins;
}
int DeviceRegistration::FindPin(PinNameValue pinNameValue)
{
    for (int pinIndex = 0; pinIndex < NumberOfGPIOPins; pinIndex++)
    {
        if (GPIOPins[pinIndex].pinNameValue == pinNameValue)
        {
            return pinIndex;
        }
    }
    return -1;
}
PinNameValue DeviceRegistration::FindPinWithFunctionAndChannel(DevicePinFunction dph, int deviceChannel)
{
    for (int pinIndex = 0; pinIndex < NumberOfPins(); pinIndex++)
    {
        if (GPIOPins[pinIndex].CurrentFunction == dph && GPIOPins[pinIndex].DeviceFunctionChannelNumber == deviceChannel)
        {
            return GPIOPins[pinIndex].pinNameValue;
        }
    }
    return (PinNameValue)0;
}
bool DeviceRegistration::IsPinReserved(PinNameValue pinNameValue)
{
    bool status = false;
    int pinIndex = FindPin(pinNameValue);
    if (pinIndex != 0)
    {
        status = GPIOPins[pinIndex].Reserved;
    }
    return status;
}
bool DeviceRegistration::ReservePin(PinNameValue pinNameValue)
{
    bool status = false;
    int pinIndex = FindPin(pinNameValue);
    if (pinIndex != 0)
    {
        if (!IsPinReserved(pinNameValue))
        {
            GPIOPins[pinIndex].Reserved = true;
            GPIOPins[pinIndex].Mode = (PinMode)NOT_SET;
            GPIOPins[pinIndex].CurrentFunction = DevicePinFunction::NONE;
            GPIOPins[pinIndex].DeviceFunctionChannelNumber = 0;
            status = true;
        }
    }
    return status;
}
bool DeviceRegistration::ReleasePin(PinNameValue pinNameValue)
{
    bool status = false;
    int pinIndex = FindPin(pinNameValue);
    if (pinIndex != 0)
    {
        if (IsPinReserved(pinNameValue))
        {
            GPIOPins[pinIndex].Reserved = false;
            GPIOPins[pinIndex].CurrentFunction = DevicePinFunction::NONE;
            GPIOPins[pinIndex].DeviceFunctionChannelNumber = 0;
            status = true;
        }
    }
    return status;
}
bool DeviceRegistration::IsValidOutputPin(PinNameValue pinNameValue)
{
    return IsValidOutputDriveMode(GPIOPins[FindPin(pinNameValue)].Mode);
}
bool DeviceRegistration::IsValidInputPin(PinNameValue pinNameValue)
{
    return IsValidInputDriveMode(GPIOPins[FindPin(pinNameValue)].Mode);
}
bool DeviceRegistration::IsValidOutputDriveMode(PinMode driveMode)
{
    return (
        driveMode == PinMode_Output || driveMode == PinMode_OutputOpenDrain ||
        driveMode == PinMode_OutputOpenDrainPullUp || driveMode == PinMode_OutputOpenSource ||
        driveMode == PinMode_OutputOpenSourcePullDown);
}
bool DeviceRegistration::IsValidInputDriveMode(PinMode driveMode)
{
    return (driveMode == PinMode_Input || driveMode == PinMode_InputPullDown || driveMode == PinMode_InputPullUp);
}
bool DeviceRegistration::IsValidADCDevice(int deviceIndex)
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
bool DeviceRegistration::IsValidDACDevice(int deviceIndex)
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
bool DeviceRegistration::IsValidI2CDevice(int deviceIndex)
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
bool DeviceRegistration::IsValidPWMDevice(int deviceIndex)
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
bool DeviceRegistration::IsValidSPIDevice(int deviceIndex)
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
bool DeviceRegistration::IsValidSerialBus(int deviceIndex)
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
bool DeviceRegistration::SetPinMode(PinNameValue pinNameValue, PinMode pinMode)
{
    int pinIndex = FindPin(pinNameValue);
    GPIOPins[pinIndex].Mode = pinMode;
    return true;
}
bool DeviceRegistration::GetPinMode(PinNameValue pinNameValue, PinMode *pinMode)
{
    bool status = false;
    int pinIndex = FindPin(pinNameValue);
    if (pinIndex != -1)
    {
        *pinMode = GPIOPins[pinIndex].Mode;
        status = true;
    }
    return status;
}
bool DeviceRegistration::SetPinFunction(PinNameValue pinNameValue, DevicePinFunction devicePinFunction)
{
    int pinIndex = FindPin(pinNameValue);
    GPIOPins[pinIndex].CurrentFunction = devicePinFunction;
    return true;
}
bool DeviceRegistration::GetPinFunction(PinNameValue pinNameValue, DevicePinFunction *devicePinFunction)
{
    bool status = false;
    int pinIndex = FindPin(pinNameValue);
    if (pinIndex != -1)
    {
        *devicePinFunction = GPIOPins[pinIndex].CurrentFunction;
        status = true;
    }
    return status;
}
DeviceRegistration::GpioParameter *DeviceRegistration::GetPinParameters(PinNameValue pinNameValue)
{
    GpioParameter *returnValue = NULL; //{0};
    int pinIndex = FindPin(pinNameValue);
    if (pinIndex != 0)
    {
        returnValue = GPIOPins[pinIndex].GpioParameterData;
    }
    return returnValue;
}
//bool AddPinParameters(PinNameValue pinNameValue, GpioParameter *newParameters)
//{
//    bool status = false;
//    int pinIndex = FindPin(pinNameValue);
//    if (FindPin(pinNameValue) != 0)
//    {
//        GPIOPins[pinIndex].GpioParameterData = newParameters;
//        status = true;
//    }
//    return status;
//}
bool DeviceRegistration::RemovePinParameters(PinNameValue pinNameValue)
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
unsigned int DeviceRegistration::countSetBits(unsigned int num)
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
static DeviceRegistration::SDPin *SDChannels;
static int NumberOfSDChannels;
void DeviceRegistration::CreateSDChannelList(SDPin *BoardSDChannels, int BoardnumberOfSDChannels)
{
    SDChannels = BoardSDChannels;
    NumberOfSDChannels = BoardnumberOfSDChannels;
}
bool DeviceRegistration::IsValidSDBus(int deviceIndex)
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
