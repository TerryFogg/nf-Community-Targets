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

#ifdef __cplusplus
extern "C"
{
#endif

    class Device
    {
      public:
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
            SD = 8192,
            LCD = 16384
        } DevicePinFunction;
        typedef struct GpioParameter
        {
            bool callBack;
            uint8_t edgeTrigger;
            void *param;
            TX_TIMER debounceTimer;
            bool expectedState;
            uint32_t debounceMs;
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

        typedef struct I2C_Buses
        {
            void *bus;
            int busSpeed;
            int8_t I2cAddress[128];
            int8_t numberOfActiveDevices;
        } I2C_Bus;
        typedef enum I2c_Bus_Type
        {
            Master,
            Slave
        } I2c_Bus_Type;
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

        static unsigned int countSetBits(unsigned int num);
        static void CreatePinList(DevicePin *GPIOPins, int numberOfPins);
        static int NumberOfPins();
        static bool AddPinParameters(PinNameValue pinNameValue, void *newParameters);
        static bool ReservePin(
            PinNameValue pinNameValue,
            DevicePinFunction devicePinFunction,
            int deviceFunctionChannelNumber = 0);
        static bool IsValidPin(PinNameValue pinNameValue);
        static bool IsPinReserved(PinNameValue pinNameValue);
        static void ReleasePin(PinNameValue pinNameValue);
        static bool SetPinMode(PinNameValue pinNameValue, PinMode pinMode);
        static int FindPin(PinNameValue pinNameValue);
        static PinNameValue FindPinWithFunctionAndChannel(DevicePinFunction dph, int deviceChannel);
        static bool IsValidOutputDriveMode(PinMode driveMode);
        static bool IsValidInputDriveMode(PinMode driveMode);

        static DevicePin GetPin(PinNameValue pinNameValue);
        // bool AddPinParameters(PinNameValue pinNameValue, void *newParameters);
        static bool RemovePinParameters(PinNameValue pinNameValue);

        static void CreateADCChannelList(AdcPin *AdcChannels, int numberOfAdcChannels);
        static void CreateDACChannelList(DacPin *DACChannels, int numberOfDACChannels);
        static void CreateI2CChannelList(I2cPin *I2CChannels, int numberOfI2CChannels);
        static void CreatePWMChannelList(PwmPin *PWMChannels, int numberOfPWMChannels);
        static void CreateSPIChannelList(SpiPin *SPIChannels, int numberOfSPIChannels);
        static void CreateSerialChannelList(SerialPin *SerialChannels, int numberOfSerialChannels);
        static bool IsValidADCDevice(int deviceIndex);
        static bool IsValidDACDevice(int deviceIndex);
        static bool IsValidI2CDevice(int deviceIndex);
        static bool IsValidPWMDevice(int deviceIndex);
        static bool IsValidSDBus(int deviceIndex);
        static bool IsValidSerialBus(int deviceIndex);
        static bool IsValidSPIDevice(int deviceIndex);
        static bool IsValidOutputPin(PinNameValue pinNameValue);
        static bool IsValidInputPin(PinNameValue pinNameValue);

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
        static void CreateSDChannelList(SDPin *BoardSDChannels, int BoardnumberOfSDChannels);
#endif
    };

#ifdef __cplusplus
}
#endif
