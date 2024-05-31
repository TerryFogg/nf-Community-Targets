#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Types.h>
#include "TX_API.h"
#ifdef FILEX
#include "File_Drivers.h"
#endif

#include "PinNames.h"

#define NOT_SET -1

#define FAULT_ON_OBJECT_DISPOSED(disposedState)                                                                        \
    if (disposedState != 0)                                                                                            \
    NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED)

typedef struct I2C_Buses
{
    void *bus;
    int busSpeed;
    int8_t I2cAddress[128];
    int8_t numberOfActiveDevices;
} I2C_Bus;
typedef enum DevicePinFunction
{
    NONE = 1,
    GPIO = 2,
    ADC = 4,
    DAC = 8,
    SPI = 16,
    PWM = 32,
    I2C = 64,
    USART = 128,
    CAN = 256,
    COUNTER = 512,
    TIMER = 1024,
    I2S = 2048,
    WAKEUP = 4096,
    SD = 8192
} DevicePinFunction;
typedef enum I2c_Bus_Type
{
    Master,
    Slave
} I2c_Bus_Type;
typedef struct GpioParameter
{
    TX_TIMER debounceTimer;
    GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;
    uint32_t debounceMs;
    uint8_t edgeTrigger;
    void *param;
    bool expectedState;
    bool waitingForDebounceToExpire;
} GpioParameter;
typedef struct DevicePin
{
    PinNameValue pinNameValue;
    bool Reserved;
    struct DevicePins *next;
    GpioParameter *GpioParameterData;
    PinMode Mode;
    DevicePinFunction CurrentFunction;
    int DeviceFunctionChannelNumber;
} DevicePin;
typedef struct AdcPin
{
    void *controllerId;
    int controllerNumber;
    int channelNumber;
} AdcPin;
typedef struct DacPin
{
    void *controllerId;
    int controllerNumber;
    int channelNumber;
} DacPin;
typedef struct I2cPin
{
    void *controllerId;
    int controllerNumber;
    int channelNumber;
} I2cPin;
typedef struct PwmPin
{
    void *controllerId;
    int controllerNumber;
    int channelNumber;
} PwmPin;
typedef struct SpiPin
{
    void *controllerId;
    int controllerNumber;
    int channelNumber;
} SpiPin;
typedef struct SerialPin
{
    void *controllerId;
    int controllerNumber;
    int channelNumber;
} SerialPin;
typedef struct SerialChannel
{
    void *controllerId;
    char newLine;
} SerialChannel;

#ifdef __cplusplus
extern "C"
{
#endif
    unsigned int countSetBits(unsigned int num);
    void CreatePinList(DevicePin *GPIOPins, int numberOfPins);
    int NumberOfPins();
    bool AddPinParameters(PinNameValue pinNameValue, void *newParameters);
    bool ReservePin(PinNameValue pinNameValue);
    bool IsValidPin(PinNameValue pinNameValue);
    bool IsPinReserved(PinNameValue pinNameValue);
    bool ReleasePin(PinNameValue pinNameValue);
    bool SetPinMode1(PinNameValue pinNameValue, PinMode pinMode);
    bool GetPinMode(PinNameValue pinNameValue, PinMode *pinMode);
    bool SetPinFunction(PinNameValue pinNameValue, DevicePinFunction devicePinFunction);
    bool GetPinFunction(PinNameValue pinNameValue, DevicePinFunction *devicePinFunction);
    int FindPin(PinNameValue pinNameValue);
    PinNameValue FindPinWithFunctionAndChannel(DevicePinFunction dph, int deviceChannel);
    bool IsValidOutputDriveMode(PinMode driveMode);
    bool IsValidInputDriveMode(PinMode driveMode);

    GpioParameter *GetPinParameters(PinNameValue pinNameValue);
    // bool AddPinParameters(PinNameValue pinNameValue, void *newParameters);
    bool RemovePinParameters(PinNameValue pinNameValue);

    void CreateADCChannelList(AdcPin *AdcChannels, int numberOfAdcChannels);
    void CreateDACChannelList(DacPin *DACChannels, int numberOfDACChannels);
    void CreateI2CChannelList(I2cPin *I2CChannels, int numberOfI2CChannels);
    void CreatePWMChannelList(PwmPin *PWMChannels, int numberOfPWMChannels);
    void CreateSPIChannelList(SpiPin *SPIChannels, int numberOfSPIChannels);
    void CreateSerialChannelList(SerialPin *SerialChannels, int numberOfSerialChannels);
    bool IsValidI2CDevice(int busIndex);
    bool IsValidPWMBus(int busIndex);
    bool IsValidSDBus(int busIndex);
    bool IsValidSerialBus(int busIndex);
    bool IsValidOutputPin(PinNameValue pinNameValue);
    bool IsValidInputPin(PinNameValue pinNameValue);

#if defined(FILEX_SYSTEM_SD)
    typedef struct SDPin
    {
        void *controllerId;
        int controllerNumber;
        int channelNumber;
        File_Read_Type readType;
        File_Status status;
        TX_SEMAPHORE RX_semaphore;
        TX_SEMAPHORE TX_semaphore;
    } SDPin;
    void CreateSDChannelList(SDPin *BoardSDChannels, int BoardnumberOfSDChannels);
#endif

#ifdef __cplusplus
}
#endif
