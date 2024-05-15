#pragma once
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <nanoHAL_v2.h>
#include "CPU_GPIO_decl.h"
#include "TX_API.h"
#include "DeviceIO.h"

#ifdef __cplusplus
extern "C"
{
#endif
    unsigned int countSetBits(unsigned int num);
    void CreatePinList(DevicePin *GPIOPins, int numberOfPins);
    void CreateADCChannelList(AdcPin *AdcChannels, int numberOfAdcChannels);
    void CreateDACChannelList(DacPin *DACChannels, int numberOfDACChannels);
    void CreateI2CChannelList(I2cPin *I2CChannels, int numberOfI2CChannels);
    void CreatePWMChannelList(PwmPin *PWMChannels, int numberOfPWMChannels);
    void CreateSPIChannelList(SpiPin *SPIChannels, int numberOfSPIChannels);
    void CreateSerialChannelList(SerialPin *SerialChannels, int numberOfSerialChannels);
    void CreateSDChannelList(SDPin *BoardSDChannels, int BoardnumberOfSDChannels);

    int FindPin(GPIO_PIN pinNumber);
    int FindPinWithFunctionAndChannel(DevicePinFunction dph, int deviceChannel);
    GpioParameter *GetPinParameters(GPIO_PIN pinNumber);
    int NumberOfPins();
    bool IsValidPin(GPIO_PIN pinNumber);
    bool IsPinReserved(GPIO_PIN pinNumber);
    bool ReservePin(GPIO_PIN pinNumber, DevicePinFunction dpf, int deviceFunctionChannelNumber);
    bool ReleasePin(GPIO_PIN pinNumber);
    bool AddPinParameters(GPIO_PIN pinNumber, void *newParameters);
    bool RemovePinParameters(GPIO_PIN pinNumber);
    bool IsValidOutputDriveMode(PinMode driveMode);
    bool IsValidInputDriveMode(PinMode driveMode);
    bool IsValidI2CBus(int busIndex);
    bool IsValidSDBus(int busIndex);

#ifdef __cplusplus
}
#endif
