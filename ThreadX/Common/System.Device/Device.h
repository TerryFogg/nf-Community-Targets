#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Types.h>
#include "sys_dev_i2c_native.h"
#include "TX_API.h"
#ifdef FILEX
#include "File_Drivers.h"
#endif

#include "PinNames.h"
#define NOT_SET -1
#define FAULT_ON_OBJECT_DISPOSED(disposedState)                                                                        \
    if (disposedState != 0)                                                                                            \
    NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED)


enum DevicePinFunction
{
    LOW_POWER,
    GPIO,
    ADC0,
    ADC1,
    ADC2,
    ADC3,
    ADC4,
    ADC5,
    DAC,
    SPI,
    PWM,
    I2C0_SDA,
    I2C0_SCL,
    I2C1_SDA,
    I2C1_SCL,
    I2C2_SDA,
    I2C2_SCL,
    I2C3_SDA,
    I2C3_SCL,
    I2C4_SDA,
    I2C4_SCL,
    I2C5_SDA,
    I2C5_SCL,
    USART0_TX,
    USART0_RX,
    USART1_TX,
    USART1_RX,
    USART2_TX,
    USART2_RX,
    USART3_TX,
    USART3_RX,
    USART4_TX,
    USART4_RX,
    CAN,
    COUNTER,
    TIMER,
    I2S,
    WAKEUP,
    SD,
    LCD,
    LED,
    BUTTON,
    MEMORY_INTERFACE,
    WIFI_INTERFACE,
    USB,
    CAMERA
} ;

typedef struct GpioCallbackParameter
{
    bool callBack;
    uint8_t edgeTrigger;
    void *param;
    TX_TIMER debounceTimer;
    bool expectedState;
    uint32_t debounceMs;
    bool waitingForDebounceToExpire;
} GpioCallbackParameter;
typedef struct DeviceGpioPin
{
    PinNameValue pinNameValue;
    bool Reserved;
    GpioCallbackParameter *GpioCallbackParameters;
    PinMode Mode;
    DevicePinFunction Function;
    void *FunctionCode;
} DeviceGpioPin;
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

#ifdef __cplusplus
extern "C"
{
#endif
    class Device
    {
      public:
        static void CreatePinList(DeviceGpioPin *GpioPins, int numberOfPins);
        static int NumberOfPins();
        static bool ReservePin(PinNameValue pinNameValue);
        static bool IsPinReserved(PinNameValue pinNameValue);
        static bool IsValidPin(PinNameValue pinNameValue);
        static void ReleasePin(PinNameValue pinNameValue);

        static bool AddPinCallbackParameter(PinNameValue pinNameValue, void *newParameters);
        static bool RemovePinCallbackParameters(PinNameValue pinNameValue);

        static bool RegisterPinMode(PinNameValue pinNameValue, PinMode pinMode);
        static bool RegisterPinFunction(PinNameValue pinNameValue, DevicePinFunction function);

        static int FindPin(PinNameValue pinNameValue);
        static DeviceGpioPin GetPin(PinNameValue pinNameValue);
        static bool IsValidOutputDriveMode(PinMode driveMode);
        static bool IsValidInputDriveMode(PinMode driveMode);
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
