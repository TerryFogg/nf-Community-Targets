//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "DeviceRegistration.h"

DevicePin *GPIOPins;
static int NumberOfGPIOPins;

static AdcPin *ADCChannels;
static int NumberOfADCChannels;
static DacPin *DACChannels;
static int NumberOfDACChannels;
static I2cPin *I2CChannels;
static int NumberOfI2CChannels;
static PwmPin *PWMChannels;
static int NumberOfPWMChannels;
static SerialPin *SerialChannels;
static int NumberOfSerialChannels;
static SpiPin *SpiChannels;
static int NumberOfSpiChannels;

void CreatePinList(DevicePin *mcuGPIOPinList, int numberOfPins)
{
    GPIOPins = mcuGPIOPinList;
    NumberOfGPIOPins = numberOfPins;
}
bool AddPinParameters(PinNameValue pinNameValue, void *newParameters)
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

void CreateADCChannelList(AdcPin *BoardAdcChannels, int BoardnumberOfAdcChannels)
{
    ADCChannels = BoardAdcChannels;
    NumberOfADCChannels = BoardnumberOfAdcChannels;
}
void CreateDACChannelList(DacPin *BoardDACChannels, int BoardnumberOfDACChannels)
{
    DACChannels = BoardDACChannels;
    NumberOfDACChannels = BoardnumberOfDACChannels;
}
void CreateI2CChannelList(I2cPin *BoardI2CChannels, int BoardnumberOfI2CChannels)
{
    I2CChannels = BoardI2CChannels;
    NumberOfI2CChannels = BoardnumberOfI2CChannels;
}
void CreatePWMChannelList(PwmPin *BoardPWMChannels, int BoardnumberOfPWMChannels)
{
    PWMChannels = BoardPWMChannels;
    NumberOfPWMChannels = BoardnumberOfPWMChannels;
}
void CreateSPIChannelList(SpiPin *BoardSpiChannels, int BoardnumberOfSpiChannels)
{
    SpiChannels = BoardSpiChannels;
    NumberOfSpiChannels = BoardnumberOfSpiChannels;
}
void CreateSerialChannelList(SerialPin *BoardSerialChannels, int BoardnumberOfSerialChannels)
{
    SerialChannels = BoardSerialChannels;
    NumberOfSerialChannels = BoardnumberOfSerialChannels;
}
bool IsValidPin(PinNameValue pinNameValue)
{
    return (FindPin(pinNameValue) != 0);
}
int NumberOfPins()
{
    return NumberOfGPIOPins;
}
int FindPin(PinNameValue pinNameValue)
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
PinNameValue FindPinWithFunctionAndChannel(DevicePinFunction dph, int deviceChannel)
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
bool IsPinReserved(PinNameValue pinNameValue)
{
    bool status = false;
    int pinIndex = FindPin(pinNameValue);
    if (pinIndex != 0)
    {
        status = GPIOPins[pinIndex].Reserved;
    }
    return status;
}
bool ReservePin(PinNameValue pinNameValue)
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
bool ReleasePin(PinNameValue pinNameValue)
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
bool IsValidOutputPin(PinNameValue pinNameValue)
{
    return IsValidOutputDriveMode(GPIOPins[FindPin(pinNameValue)].Mode);
}
bool IsValidInputPin(PinNameValue pinNameValue)
{
    return IsValidInputDriveMode(GPIOPins[FindPin(pinNameValue)].Mode);
}
bool IsValidOutputDriveMode(PinMode driveMode)
{
    return (
        driveMode == PinMode_Output || driveMode == PinMode_OutputOpenDrain ||
        driveMode == PinMode_OutputOpenDrainPullUp || driveMode == PinMode_OutputOpenSource ||
        driveMode == PinMode_OutputOpenSourcePullDown);
}
bool IsValidInputDriveMode(PinMode driveMode)
{
    return (driveMode == PinMode_Input || driveMode == PinMode_InputPullDown || driveMode == PinMode_InputPullUp);
}
bool IsValidADCBus(int busIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfADCChannels; iChannel++)
    {
        if (ADCChannels->controllerNumber == busIndex)
        {
            status = true;
        }
        ADCChannels++;
    }
    return status;
}
bool IsValidDACBus(int busIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfDACChannels; iChannel++)
    {
        if (DACChannels->controllerNumber == busIndex)
        {
            status = true;
        }
        DACChannels++;
    }
    return status;
}
bool IsValidI2CDevice(int busIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfI2CChannels; iChannel++)
    {
        if (I2CChannels->controllerNumber == busIndex)
        {
            status = true;
        }
        I2CChannels++;
    }
    return status;
}
bool IsValidPWMBus(int busIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfPWMChannels; iChannel++)
    {
        if (PWMChannels->controllerNumber == busIndex)
        {
            status = true;
        }
        PWMChannels++;
    }
    return status;
}
bool IsValidSpiBus(int busIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfSpiChannels; iChannel++)
    {
        if (SpiChannels->controllerNumber == busIndex)
        {
            status = true;
        }
        SpiChannels++;
    }
    return status;
}
bool IsValidSerialBus(int busIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfSerialChannels; iChannel++)
    {
        if (SerialChannels->controllerNumber == busIndex)
        {
            status = true;
        }
        SerialChannels++;
    }
    return status;
}
bool SetPinMode1(PinNameValue pinNameValue, PinMode pinMode)
{
    int pinIndex = FindPin(pinNameValue);
    GPIOPins[pinIndex].Mode = pinMode;
    return true;
}
bool GetPinMode(PinNameValue pinNameValue, PinMode *pinMode)
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
bool SetPinFunction(PinNameValue pinNameValue, DevicePinFunction devicePinFunction)
{
    int pinIndex = FindPin(pinNameValue);
    GPIOPins[pinIndex].CurrentFunction = devicePinFunction;
    return true;
}
bool GetPinFunction(PinNameValue pinNameValue, DevicePinFunction *devicePinFunction)
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
GpioParameter *GetPinParameters(PinNameValue pinNameValue)
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
bool RemovePinParameters(PinNameValue pinNameValue)
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
unsigned int countSetBits(unsigned int num)
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
static SDPin *SDChannels;
static int NumberOfSDChannels;
void CreateSDChannelList(SDPin *BoardSDChannels, int BoardnumberOfSDChannels)
{
    SDChannels = BoardSDChannels;
    NumberOfSDChannels = BoardnumberOfSDChannels;
}
bool IsValidSDBus(int busIndex)
{
    bool status = false;
    for (int iChannel = 0; iChannel < NumberOfSDChannels; iChannel++)
    {
        if (SDChannels->controllerNumber == busIndex)
        {
            status = true;
        }
        SDChannels++;
    }
    return status;
}
#endif
