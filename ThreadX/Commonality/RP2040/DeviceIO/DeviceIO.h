#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "board.h"
#include "sys_dev_i2c_native.h"
#include "sys_dev_adc_native.h"
#include <nanoHAL_v2.h>
#include "CPU_GPIO_decl.h"

#include "File_Drivers.h"

#define I2C_BUSSPEED_NOT_SET -1
typedef struct I2C_Buses
{
//    I2C_TypeDef *bus;
    int busSpeed;
    int8_t I2cAddress[128];
    int8_t numberOfActiveDevices;
} I2C_Bus;

typedef enum DevicePinFunction
{
    NONE,
    GPIO,
    GPIO_INPUT,
    GPIO_OUTPUT,
    ADC,
    CAN,
    FDCAN,
    DAC,
    SPI,
    COUNTER,
    PWM,
    TIMER,
    TIMER_LOW_POWER,
    I2C_SCL,
    I2C_SDA,
    I2S,
    USART_TX,
    USART_RX,
    USART_LOW_POWER_TX,
    USART_LOW_POWER_RX,
    WAKEUP,
    SD
} DevicePinFunction;

typedef enum I2c_Bus_Type
{
    Master,
    Slave
} I2c_Bus_Type;

typedef struct GpioParameter
{
//    TX_TIMER debounceTimer;
    GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;
    uint32_t debounceMs;
    uint8_t edgeTrigger;
    void *param;
    bool expectedState;
    bool waitingForDebounceToExpire;
} GpioParameter;

typedef struct DevicePin
{
    GPIO_PIN pinNumber;
    bool Reserved;
    struct DevicePins *next;
    void *ParameterData;
    DevicePinFunction Function;
    int DeviceFunctionChannelNumber;
} DevicePin;

typedef struct AdcPin
{
    //ADC_TypeDef *controllerId;
    int controllerNumber;
    int channelNumber;
} AdcPin;
typedef struct DacPin
{
    //DAC_TypeDef *controllerId;
    int controllerNumber;
    int channelNumber;
} DacPin;
typedef struct I2cPin
{
    //I2C_TypeDef *controllerId;
    int controllerNumber;
    int channelNumber;
} I2cPin;
typedef struct PwmPin
{
    //TIM_TypeDef *controllerId;
    int controllerNumber;
    int channelNumber;
} PwmPin;
typedef struct SpiPin
{
    //SPI_TypeDef *controllerId;
    int controllerNumber;
    int channelNumber;
} SpiPin;
typedef struct SerialPin
{
    //USART_TypeDef *controllerId;
    int controllerNumber;
    int channelNumber;
} SerialPin;
typedef struct SDPin
{
    //SDMMC_TypeDef *controllerId;
    int controllerNumber;
    int channelNumber;
    File_Read_Type readType;
    File_Status status;
    TX_SEMAPHORE RX_semaphore;
    TX_SEMAPHORE TX_semaphore;
} SDPin;

typedef struct SerialChannel
{
    //USART_TypeDef *controllerId;
    char newLine;
} SerialChannel;

#ifdef __cplusplus
extern "C"
{
#endif
    void InitializeDevicePins();
    int GetI2CPin_AlternateFunctionNumber(GPIO_PIN GPIOPortByNumber, int pinNumber, DevicePinFunction Dpf);
    uint32_t I2C_GetTiming(I2cBusSpeed busSpeed);

    bool SetPinLowPower(GPIO_PIN pinNumber);
    bool SetGpioDriveMode(GPIO_PIN pinNumber, PinMode driveMode);
    bool SetPinState(GPIO_PIN pinNumber, GpioPinValue pinState);
    bool TogglePinState(GPIO_PIN pinNumber);
    bool SetPinFunction(GPIO_PIN pinNumber, DevicePinFunction alternateFunction, int optional = 0);
    GpioPinValue GetPinState(GPIO_PIN pinNumber);
    bool EnablePinInterrupt(int pinNumber, GpioParameter *gpioParameter);
    bool RemovePinInterrupt(GPIO_PIN pinNumber);

    void EXTI0_IRQHandler(void);
    void EXTI1_IRQHandler(void);
    void EXTI2_IRQHandler(void);
    void EXTI3_IRQHandler(void);
    void EXTI4_IRQHandler(void);
    void EXTI9_5_IRQHandler(void);
    void EXTI15_10_IRQHandler(void);

#pragma region I2C
    bool Initialize_Native_I2C(int busIndex, I2cBusSpeed busSpeed, I2c_Bus_Type busType, uint8_t SlaveAddress);
    bool DeInitialize_Native_I2C(int busIndex);
    bool I2CWriteData(int busIndex, uint8_t *writeData, int writeSize, uint8_t slaveAddress);
    bool I2CReadData(int busIndex, uint8_t *readData, int readSize, uint8_t slaveAddress);
#pragma endregion

#pragma region ADC
    void InitADC();
    void OpenAdcChannel(int AdcChannel);
    int GetAdcChannelCount();
    int GetAdcSupportedMaxValue();
    int GetAdcSupportedMinValue();
    int GetAdcSupportedResolution();
    uint16_t AdcReadValue(int channelNumber);
    bool IsAdcModeSupported(AdcChannelMode mode);
    int ConvertChannelIndexToAdcInstance(int channelIndex);
#pragma endregion

#pragma region DAC
    void InitDacChannel(int dacIndex);
    void OpenDacChannel(int dacIndex, int dacChannel);
    void StopDac(int dacIndex, int channel);
    void DAC_Start_SoftwareTriggered(int channel);
    void WriteDacValue(int dacIndex, int channel, uint16_t value);
    int GetDacChannelCount(int dacIndex);
    int GetDacSupportedResolution(int dacIndex);
#pragma endregion

#pragma region PWM
    int SetDesiredFrequency(int pwmID, int desiredFrequency);
    double SetDutyCycle(int pwmID, int channelId, double dutyCycle);
    void StartPwm(int pwmID, double dutyCycle, int channelId, int pinNumber);
    void StopPwm(int pwmID, int channelId);
    bool CheckChannel(int pwmID, int channelId);
#pragma endregion

#pragma region Serial
    void InitUSART(int UsartDeviceNumber);
    void DeInitUSART(int UsartDeviceNumber);
    int SerialReadBytes(int UsartDeviceNumber, uint8_t *data, int bytesRequested);
    int GetSerialDeviceBufferIndex(int SerialDevice);
    void SetSerialDeviceSignalsInverted(int UsartDeviceNumber, bool InvertSignals);
    bool IsSerialSignalInverted(int UsartDeviceNumber);
    int SerialBytesAvailable(int UsartDeviceNumber);
    int SerialReadBytes(int UsartDeviceNumber, uint8_t *data, int bytesRequested);
    int SerialReadLine(int UsartDeviceNumber, const char *newLineCharacters, int newLineLength, uint8_t *&line);
    bool SerialWriteBytes(int UsartDeviceNumber, uint8_t *data, int size);
    void SerialSetStopBits(int UsartDeviceNumber, int RequestedStopBits);
    void SerialSetBaudRate(int UsartDeviceNumber, int RequestedBaudRate);
    void SerialSetWatchCharacter(int UsartDeviceNumber, uint8_t watchCharacter);
    int SerialSetThreshold(int UsartDeviceNumber, int RequestedThreshold);


#pragma endregion

#ifdef __cplusplus
}
#endif
