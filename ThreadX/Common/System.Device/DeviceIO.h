#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nanoCLR_Types.h"
#include "Device.h"
#include "board.h"

#include "sys_dev_adc_native.h"
#include "sys_dev_dac_native.h"
#include "sys_dev_gpio_native.h"
#include "sys_dev_i2c_native.h"
#include "sys_dev_i2c_slave_native.h"
#include "sys_dev_i2s_native.h"
#include "sys_dev_pwm_native.h"
#include "sys_io_ser_native.h"
#include "sys_dev_spi_native.h"
#include "sys_dev_usbstream_native.h"

#define PORT_INDEX(pinNumber) (pinNumber / 16)
#define PIN_INDEX(pinNumber)  pinNumber % 16

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

inline void Callback_EVENT_GPIO(GPIO_PIN pinNumber, bool pinValue)
{
    PostManagedEvent(EVENT_GPIO, 0, (uint16_t)pinNumber, pinValue);
}

class GpioIO
{
  private:
  public:
    static bool Initialize();
    static bool InitializePin(PinNameValue pinNumber);
    static bool Dispose(PinNameValue pinNumber);
    static bool Read(PinNameValue pinNumber);
    static bool Write(PinNameValue pinNumber, bool pinState);
    static bool Toggle(PinNameValue pinNumber);
    static bool SetFunction(Device::DevicePin);
    static bool SetLowPower(PinNameValue pinNumber);
    static bool SetMode(PinNameValue pinNumber, PinMode pinMode);
    static bool InterruptEnable(PinNameValue pinNumber, GPIO_INT_EDGE events, void *interruptRoutine = NULL);
    static bool InterruptDisable(PinNameValue pinNumber);
    static bool InterruptRemove(PinNameValue pinNumber);

    enum PinFunction
    {
        NONE = 1,
        ADC = 2,
        CAN = 3,
        COUNTER = 4,
        DAC = 5,
        GPIO = 6,
        I2C = 7,
        I2S = 8,
        PWM = 9,
        SD = 10,
        SPI = 11,
        TIMER = 12,
        USART = 13
    };
};

class AdcIO
{
  private:
  public:
    static int MaximumValue();
    static int MinimumValue();
    static int Resolution();
    static int ChannelCount();

    static bool Initialize();
    static bool Dispose(CLR_INT32 adc_channel_number);
    static bool Open(CLR_INT32 adc_channel_number);
    static CLR_UINT16 Read(CLR_INT32 adc_channel_number);
    static PinNameValue ChannelToPin(CLR_INT32 adc_channel_number);
    static CLR_UINT16 IsModeSupported(AdcChannelMode requestedMode);
    static CLR_UINT16 DifferentialModeSupport(CLR_INT32 adc_channel_number);
};

class DacIO
{
  private:
  public:
    static int Resolution();
    static int ChannelCount();
    static bool Initialize(CLR_INT32 dac_channel_number);
    static bool Open(CLR_INT32 controllerId, CLR_INT32 dac_channel_number);
    static char *DeviceSelector(int controllerId);
    static void Write(CLR_INT32 dac_channel_number, CLR_INT32 value);
    static PinNameValue ChannelToPin(CLR_INT32 dac_channel_number);
    static bool Dispose(CLR_INT32 dac_channel_number);
};

class I2cIO
{
  private:
  public:
    static bool Initialize(CLR_INT32 I2C_deviceId, I2cBusSpeed I2C_speed);
    static bool Dispose(CLR_INT32 I2C_deviceId);
    static I2cTransferStatus Write(
        CLR_INT32 I2C_deviceId,
        CLR_INT32 slaveAddress,
        CLR_UINT8 *writeBuffer,
        CLR_INT32 writeSize);

    static I2cTransferStatus Read(
        CLR_INT32 I2C_deviceId,
        CLR_INT32 slaveAddress,
        CLR_UINT8 *readBuffer,
        CLR_INT32 readSize);
};

class I2cIO_Slave
{
  private:
  public:
    static CLR_INT32 TimeoutMaximum();
    static CLR_INT32 Timeout();
    static bool Initialize(CLR_INT32 I2C_deviceId, CLR_INT32 deviceAddress);
    static bool Dispose(CLR_INT32 I2C_deviceId);
    static I2cTransferStatus Write(
        CLR_INT32 I2C_deviceId,
        CLR_UINT8 *writeBuffer,
        CLR_INT32 writeSize,
        CLR_INT32 timeoutMilliseconds,
        CLR_INT32 *readCount);
    static I2cTransferStatus Read(
        CLR_INT32 I2C_deviceId,
        CLR_UINT8 *readBuffer,
        CLR_INT32 readSize,
        CLR_INT32 timeoutMilliseconds,
        CLR_INT32 *readCount);
};

class PwmIO
{
  private:
  public:
    static bool Initialize(
        CLR_INT32 channelId,
        CLR_INT32 timerId,
        CLR_INT32 pinNumber,
        CLR_INT32 polarity,
        CLR_INT32 desiredFrequency,
        CLR_INT32 dutyCycle);

    static bool Dispose(CLR_INT32 I2C_deviceId);
    static CLR_UINT32 SetDutyCycle(CLR_INT32 pinNumber, CLR_INT32 desiredFrequency);
    static CLR_UINT32 Start(CLR_INT32 pinNumber, CLR_INT32 timerId);
    static CLR_UINT32 Stop(CLR_INT32 pinNumber, CLR_INT32 timerId);
    static CLR_UINT32 DesiredFrequency(CLR_INT32 timerId, CLR_INT32 desiredFrequency);
    static CLR_UINT32 GetChannel(CLR_INT32 timerId, CLR_INT32 pin_number);
};

class SerialIO
{
  private:
  public:
    static bool Initialize(CLR_INT32 usartDeviceNumber, CLR_INT32 baudrate);
    static bool Dispose(CLR_INT32 usartDeviceNumber);
    static CLR_INT32 BytesAvailable(CLR_INT32 usartDeviceNumber);
    static bool ReadBytes(CLR_INT32 usartDeviceNumber, CLR_UINT8 *data, CLR_INT32 length);
    static bool ReadLine(CLR_INT32 usartDeviceNumber, char *newLine, CLR_UINT8 length, char *line);
    static bool SetSignalLevels(CLR_INT32 usartDeviceNumber, bool inversion);
    static bool GetSignalLevels(CLR_INT32 usartDeviceNumber);
    static bool WriteBytes(CLR_INT32 usartDeviceNumber, CLR_UINT8 *data, CLR_INT32 count);
    static bool SetWatchCharacter(CLR_INT32 usartDeviceNumber, CLR_UINT8 watch_character);
    static bool SetReceiveThreshold(CLR_INT32 usartDeviceNumber, CLR_INT32 threshold);
    static char *GetDevice(CLR_INT32 usartDeviceNumber);
    static bool InvertSignalLevels(CLR_INT32 usartDeviceNumber, bool InvertSignal);
    static bool SetBaudRate(CLR_INT32 usartDeviceNumber, CLR_INT32 baudRate);
    static bool SetConfig(
        CLR_INT32 usartDeviceNumber,
        CLR_INT32 stopBits,
        CLR_INT32 dataBits,
        CLR_INT32 RequestedParity);
    static bool SetHandshake(CLR_INT32 usartDeviceNumber, Handshake handshake);
    static bool SetMode(CLR_INT32 UsartDeviceNumber, CLR_INT32 mode);
    static HRESULT SetupWriteLine(CLR_RT_StackFrame &stack, char **buffer, uint32_t *length, bool *isNewAllocation);
};

class SpiIO
{
  private:
  public:
    static CLR_INT32 MaximumClockFrequency(CLR_INT32 requestClockFrequence);
    static CLR_INT32 MinimumClockFrequency(CLR_INT32 controllerID);

    static bool Initialize();
    static bool Dispose(CLR_INT32 deviceId);
    static bool WriteRead(
        CLR_INT32 deviceId,
        SPI_WRITE_READ_SETTINGS rws,
        CLR_UINT8 *writeData,
        CLR_UINT16 writeSize,
        CLR_UINT8 *readData,
        CLR_UINT16 readSize);
    static bool Open(SPI_DEVICE_CONFIGURATION spiConfig, CLR_UINT32 handle);
    static CLR_INT32 ByteTime();
    static SPI_OP_STATUS Completed(CLR_INT32 deviceId);
};
