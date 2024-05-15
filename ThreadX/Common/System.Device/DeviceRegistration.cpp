//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
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
static SDPin *SDChannels;
static int NumberOfSDChannels;


void CreatePinList(DevicePin *GPIOPins, int numberOfPins)
{
    GPIOPins = GPIOPins;
    NumberOfGPIOPins = numberOfPins;
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
void CreateSDChannelList(SDPin *BoardSDChannels, int BoardnumberOfSDChannels)
{
    SDChannels = BoardSDChannels;
    NumberOfSDChannels = BoardnumberOfSDChannels;
}

int NumberOfPins()
{
    return NumberOfGPIOPins;
}
bool IsValidPin(GPIO_PIN pinNumber)
{
    return (FindPin(pinNumber) != 0);
}
int FindPin(GPIO_PIN pinNumber)
{
    for (int i = 0; i < NumberOfGPIOPins; i++)
    {
        if (GPIOPins[i].pinNumber == pinNumber)
        {
            return i;
        }
    }
    return 0;
}
int FindPinWithFunctionAndChannel(DevicePinFunction dph, int deviceChannel)
{
    for (int i = 0; i < NumberOfPins(); i++)
    {
        if (GPIOPins[i].Function == dph && GPIOPins[i].DeviceFunctionChannelNumber == deviceChannel)
        {
            return i;
        }
    }
    return 0;
}
bool IsPinReserved(GPIO_PIN pinNumber)
{
    bool status = false;
    int pin = FindPin(pinNumber);
    if (pin != 0)
    {
        status = GPIOPins[pin].Reserved;
    }
    return status;
}
bool ReservePin(GPIO_PIN pinNumber, DevicePinFunction devicePinFunction, int deviceFunctionChannelNumber)
{
    bool status = false;
    int pin = FindPin(pinNumber);
    if (pin != 0)
    {
        if (!IsPinReserved(pinNumber))
        {
            GPIOPins[pin].Reserved = true;
            GPIOPins[pin].Function = devicePinFunction;
            GPIOPins[pin].DeviceFunctionChannelNumber = deviceFunctionChannelNumber;
            status = true;
        }
    }
    return status;
}
bool ReleasePin(GPIO_PIN pinNumber)
{
    bool status = false;
    int pin = FindPin(pinNumber);
    if (pin != 0)
    {
        if (IsPinReserved(pinNumber))
        {
            GPIOPins[FindPin(pinNumber)].Reserved = false;
            GPIOPins[pinNumber].Function = DevicePinFunction::NONE;
            GPIOPins[pin].DeviceFunctionChannelNumber = 0;
            status = true;
        }
    }
    return status;
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
bool IsValidI2CBus(int busIndex)
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

GpioParameter *GetPinParameters(GPIO_PIN pinNumber)
{
    GpioParameter *returnValue = {0};
    int pin = FindPin(pinNumber);
    if (pin != 0)
    {
        returnValue = (GpioParameter *)GPIOPins[pin].ParameterData;
    }
    return returnValue;
}
bool AddPinParameters(GPIO_PIN pinNumber, void *newParameters)
{
    bool status = false;
    int pin = FindPin(pinNumber);
    if (FindPin(pinNumber) != 0)
    {
        GPIOPins[pin].ParameterData = newParameters;
        status = true;
    }
    return status;
}
bool RemovePinParameters(GPIO_PIN pinNumber)
{
    bool status = false;
    int pin = FindPin(pinNumber);
    if (pin != 0)
    {
        DevicePin dp = GPIOPins[pin];
        if (dp.ParameterData != NULL)
        {
            platform_free(dp.ParameterData);
            dp.ParameterData = NULL;
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
