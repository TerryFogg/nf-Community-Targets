//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "DeviceIO.h"

#pragma region GPIO Pins
bool GpioIO::Initialize()
{
    return true;
}
bool GpioIO::InitializePin(PinNameValue pinNameValue)
{
    Device::ReservePin(
        (PinNameValue)TOUCH_INTERFACE_INTERRUPT,
        Device::DevicePinFunction::GPIO);

    // Sets pin as input, sets output to low and function as SIO
    int pinNumber = pinNameValue;
    gpio_init(pinNumber);
    return true;
}
bool GpioIO::Dispose(PinNameValue pinNameValue)
{
    int pinNumber = pinNameValue;
    GpioIO::InterruptRemove(pinNameValue);
    gpio_deinit(pinNumber);
    GpioIO::SetLowPower(pinNameValue);
    return true;
}
bool GpioIO::Read(PinNameValue pinNameValue)
{
    int pinNumber = pinNameValue;
    return gpio_get(pinNumber);
}
bool GpioIO::Write(PinNameValue pinNameValue, bool pinState)
{
    int pinNumber = pinNameValue;
    gpio_put(pinNumber, pinState);
    return true;
}
bool GpioIO::Toggle(PinNameValue pinNameValue)
{
    Write(pinNameValue, !Read(pinNameValue));
    return true;
}
static void gpio_wakup_callback(uint gpio, uint32_t events)
{ // Code to handle the interrupt
  // Interrupt callback has woken the MCU
  //  ....  continue
}
bool GpioIO::SetFunction(Device::DevicePin devicePin)
{
    bool status = false;
    int pinNumber = devicePin.pinNameValue;

    switch (devicePin.CurrentFunction)
    {
        case Device::DevicePinFunction::NONE:
            // basic input/output mode with output disabled
            gpio_set_function(pinNumber, GPIO_FUNC_NULL);
            status = true;
            break;
        case Device::DevicePinFunction::ADC:

            if (PinSupportsADC(pinNumber))
            {
                // Set pin to input (as far as SIO is concerned)
                gpio_set_function(pinNumber, GPIO_FUNC_SIO);
                gpio_disable_pulls(pinNumber);
                gpio_set_input_enabled(pinNumber, false);
                status = true;
            }
            else
            {
                status = false;
            }
            break;
        case Device::DevicePinFunction::CAN:
            // Not supported on this MCU
            status = false;
            break;
        case Device::DevicePinFunction::DAC:
            // Not supported on this MCU
            status = false;
            break;
        case Device::DevicePinFunction::GPIO:
            // basic input/output mode with output disabled
            gpio_set_function(pinNumber, GPIO_FUNC_NULL);
            status = true;
            break;
        case Device::DevicePinFunction::SPI:
            gpio_set_function(pinNumber, GPIO_FUNC_SPI);
            status = true;
            break;
        case Device::DevicePinFunction::COUNTER:
            // Not supported on this MCU
            status = false;
            break;
        case Device::DevicePinFunction::PWM:
            gpio_set_function(pinNumber, GPIO_FUNC_PWM);
            status = true;
            break;
        case Device::DevicePinFunction::SD:
            status = true;
            break;
        case Device::DevicePinFunction::TIMER:
            gpio_set_function(pinNumber, GPIO_FUNC_GPCK);
            status = true;
            break;
        case Device::DevicePinFunction::I2C:
            gpio_set_function(pinNumber, GPIO_FUNC_I2C);
            gpio_pull_up(pinNumber);
            status = true;
            break;
        case Device::DevicePinFunction::I2S:
            // Not supported on this MCU
            status = false;
            break;
        case Device::DevicePinFunction::USART:
            gpio_set_function(pinNumber, GPIO_FUNC_UART);
            status = true;
            break;
        case Device::DevicePinFunction::WAKEUP:
            // No formal pin wakeup function but we can set an interrupt to return from low power mode
            gpio_init(pinNumber);
            gpio_set_dir(pinNumber, GPIO_IN);
            gpio_pull_up(pinNumber);
            gpio_set_irq_enabled_with_callback(
                pinNumber,
                GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
                true,
                &gpio_wakup_callback);
            status = true;
            break;
        default:
            status = false;
            break;
    }
    return status;
}
bool GpioIO::SetLowPower(PinNameValue pinNameValue)
{
    int pinNumber = pinNameValue;
    gpio_deinit(pinNumber);
    gpio_disable_pulls(pinNumber);
    gpio_set_input_enabled(pinNumber, false);
    return true;
}
bool GpioIO::SetMode(PinNameValue pinNameValue, PinMode pinMode)
{
    bool status = false;
    int pinNumber = pinNameValue;

    gpio_init(pinNumber);
    switch (pinMode)
    {
        case PinMode_Input:
            gpio_set_dir(pinNumber, GPIO_IN);
            status = true;
            break;
        case PinMode_InputPullDown:
            gpio_pull_down(pinNumber);
            gpio_set_dir(pinNumber, GPIO_IN);
            status = true;
            break;
        case PinMode_InputPullUp:
            gpio_pull_up(pinNumber);
            gpio_set_dir(pinNumber, GPIO_IN);
            status = true;
            break;
        case PinMode_Output:
            gpio_set_dir(pinNumber, GPIO_OUT);
            status = true;
            break;
        case PinMode_OutputOpenDrain:
            gpio_set_dir(pinNumber, GPIO_OUT);
            status = true;
            break;
        case PinMode_OutputOpenDrainPullUp:
            gpio_set_dir(pinNumber, GPIO_OUT);
            status = true;
            break;
        case PinMode_OutputOpenSource:
            gpio_set_dir(pinNumber, GPIO_OUT);
            status = true;
            break;
        case PinMode_OutputOpenSourcePullDown:
            gpio_pull_down(pinNumber);
            gpio_set_dir(pinNumber, GPIO_OUT);
            status = true;
            break;
        default:
            status = false;
            break;
    }
    return status;
}
bool GpioIO::InterruptEnable(PinNameValue pinNameValue, GPIO_INT_EDGE events, void *interruptRoutine)
{
    bool enable = true;
    bool expectedState = false;
    int pinNumber = pinNameValue;

    int edge_events;
    // Translate events from nanoFramework to Pico
    switch (events)
    {
        case GPIO_INT_NONE:
            edge_events = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_LEVEL_LOW | GPIO_IRQ_EDGE_RISE | GPIO_IRQ_LEVEL_HIGH;
            break;
        case GPIO_INT_EDGE_LOW:
            edge_events = GPIO_IRQ_EDGE_FALL;
        case GPIO_INT_LEVEL_LOW:
            edge_events = GPIO_IRQ_LEVEL_LOW;
            break;
        case GPIO_INT_EDGE_HIGH:
            edge_events = GPIO_IRQ_EDGE_RISE;
            break;
        case GPIO_INT_LEVEL_HIGH:
            edge_events = GPIO_IRQ_LEVEL_HIGH;
            break;
        case GPIO_INT_EDGE_BOTH:
            edge_events = GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL;
            break;
    }

    if (interruptRoutine != NULL)
    {
        gpio_set_irq_enabled_with_callback((int)pinNameValue, edge_events, true, (gpio_irq_callback_t)interruptRoutine);
    }
    else
    {
        gpio_set_irq_enabled((int)pinNameValue, edge_events, true);
    }
    return true;
}
bool GpioIO::InterruptDisable(PinNameValue pinNameValue)
{
    int pinNumber = pinNameValue;
    uint32_t events = 0;
    bool disable = false;

    gpio_set_irq_enabled(pinNumber, events, disable);
    return false;
}
bool GpioIO::InterruptRemove(PinNameValue pinNameValue)
{
    int pinNumber = pinNameValue;

    GpioIO::InterruptDisable(pinNameValue);
    gpio_set_irq_enabled_with_callback(pinNumber, GPIO_IRQ_EDGE_RISE, false, NULL);
    return true;
}
#pragma endregion

#pragma region System.Device.ADC
int AdcIO::MaximumValue()
{
    return 4095;
}
int AdcIO::MinimumValue()
{
    return 0;
}
int AdcIO::Resolution()
{
    // 12 bit(8.7 ENOB)
    return 12;
}
int AdcIO::ChannelCount()
{
    return 4;
}

bool AdcIO::Initialize()
{
    adc_init();
    return true;
}
bool AdcIO::Dispose(CLR_INT32 adc_channel_number)
{
    PinNameValue pinNameValue = AdcIO::ChannelToPin(adc_channel_number);
    GpioIO::SetLowPower(pinNameValue);
    return true;
}
bool AdcIO::Open(CLR_INT32 adc_channel_number)
{
    // Channel 0 --> GPIO26
    // Channel 1 --> GPIO27
    // Channel 2 --> GPIO28
    // Channel 3 --> GPIO29
    // Channel 4 --> Temperature Sensor
    return (adc_channel_number >= 0 and adc_channel_number <= 4);
}
CLR_UINT16 AdcIO::Read(CLR_INT32 adc_channel_number)
{
    adc_select_input(adc_channel_number);
    return adc_read();
}
PinNameValue AdcIO::ChannelToPin(CLR_INT32 adc_channel_number)
{
    // Channel 0 --> GPIO26
    // Channel 1 --> GPIO27
    // Channel 2 --> GPIO28
    // Channel 3 --> GPIO29
    // Channel 4 --> Temperature Sensor
    PinNameValue pinNameValue;
    switch (adc_channel_number)
    {
        case 0:
            pinNameValue = GP26;
            break;
        case 1:
            pinNameValue = GP27;
            break;
        case 2:
            pinNameValue = GP28;
            break;
        case 3:
            pinNameValue = GP29;
            break;
        default:
            break;
    }
    return pinNameValue;
}
CLR_UINT16 AdcIO::IsModeSupported(AdcChannelMode requestedMode)
{
    bool modeSupported = false;
    if (requestedMode == AdcChannelMode::AdcChannelMode_SingleEnded)
    {
        modeSupported = true;
    }
    if (requestedMode == AdcChannelMode::AdcChannelMode_Differential)
    {
        modeSupported = false;
    }
    return modeSupported;
}
#pragma endregion

#pragma region DAC

int DacIO::Resolution()
{
    return 0;
}
int DacIO::ChannelCount()
{
    return 0;
}
bool DacIO::Initialize(CLR_INT32 dac_channel_number)
{
    // No DAC
    (void)dac_channel_number;
    return false;
}
bool DacIO::Open(CLR_INT32 controllerId, CLR_INT32 dac_channel_number)
{
    // No DAC
    (void)controllerId;
    (void)dac_channel_number;
    return false;
}
char *DacIO::DeviceSelector(int dac_channel_number)
{
    static char DeviceNameDummy[] = "NotSupported\0";
    switch (dac_channel_number)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
    }
    return DeviceNameDummy;
}
void DacIO::Write(CLR_INT32 dac_channel_number, CLR_INT32 value)
{
    (void)dac_channel_number;
    (void)value;
}
PinNameValue DacIO::ChannelToPin(CLR_INT32 dac_channel_number)
{
    return (PinNameValue)-1;
}
bool DacIO::Dispose(CLR_INT32 dac_channel_number)
{
    PinNameValue pinNameValue = DacIO::ChannelToPin(dac_channel_number);
    GpioIO::SetLowPower(pinNameValue);
    return true;
}
#pragma endregion

#pragma region System.Device.I2c

i2c_inst *Get_I2C_Instance(uint32_t busIndex)
{
    i2c_inst *i2c_instance = i2c_get_instance(busIndex);
    switch (busIndex)
    {
        case 0:
            return i2c0;
            break;
        case 1:
            return i2c1;
            break;
        default:
            // Only two I2C units ( 0 and 1)
            return NULL;
    }
}
bool I2cIO::Initialize(CLR_INT32 I2C_deviceId, I2cBusSpeed I2C_speed)
{
    i2c_inst_t *I2C_Instance = Get_I2C_Instance(I2C_deviceId);
    CLR_INT32 baud;
    switch (I2C_speed)
    {
        case I2cBusSpeed::I2cBusSpeed_StandardMode:
            baud = 100000;
            break;
        case I2cBusSpeed::I2cBusSpeed_FastMode:
            baud = 400000;
            break;
        case I2cBusSpeed::I2cBusSpeed_FastModePlus:
            baud = 1000000;
            break;
    }
    i2c_init(I2C_Instance, baud);

#ifdef TEST_CODE
    gpio_init(I2C_MASTER_SDA_PIN);
    gpio_set_function(I2C_MASTER_SDA_PIN, GPIO_FUNC_I2C);
    // pull-ups are already active on slave side, this is just a fail-safe in case the wiring is faulty
    gpio_pull_up(I2C_MASTER_SDA_PIN);

    gpio_init(I2C_MASTER_SCL_PIN);
    gpio_set_function(I2C_MASTER_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_MASTER_SCL_PIN);

    i2c_init(i2c1, I2C_BAUDRATE);
#endif
    return true;
}
bool I2cIO::Dispose(CLR_INT32 I2C_deviceId)
{
    i2c_inst *I2C_Instance = Get_I2C_Instance(I2C_deviceId);
    i2c_deinit(I2C_Instance);
    return true;
}
I2cTransferStatus I2cIO::Write(
    CLR_INT32 I2C_deviceId,
    CLR_INT32 slaveAddress,
    CLR_UINT8 *writeBuffer,
    CLR_INT32 writeSize)
{
    I2cTransferStatus return_status = I2cTransferStatus_UnknownError;
    i2c_inst *I2C_Instance = Get_I2C_Instance(I2C_deviceId);
    int returnValue = i2c_write_blocking(
        I2C_Instance,
        slaveAddress,
        writeBuffer,
        writeSize,
        false); // true to keep master control of bus

    if (returnValue == PICO_ERROR_TIMEOUT)
        return_status = I2cTransferStatus_ClockStretchTimeout;
    if (returnValue == PICO_ERROR_GENERIC)
        return_status = I2cTransferStatus_UnknownError;
    if (returnValue != writeSize)
        return_status = I2cTransferStatus_PartialTransfer;
    if (returnValue = writeSize)
        return_status = I2cTransferStatus_FullTransfer;
    return return_status;
}
I2cTransferStatus I2cIO::Read(CLR_INT32 I2C_deviceId, CLR_INT32 slaveAddress, CLR_UINT8 *readBuffer, CLR_INT32 readSize)
{
    I2cTransferStatus return_status = I2cTransferStatus_UnknownError;
    i2c_inst_t *I2C_Instance = Get_I2C_Instance(I2C_deviceId);
    int returnValue = i2c_read_blocking(I2C_Instance, slaveAddress, readBuffer, readSize, false);
    if (returnValue == PICO_ERROR_TIMEOUT)
        return_status = I2cTransferStatus_ClockStretchTimeout;
    if (returnValue == PICO_ERROR_GENERIC)
        return_status = I2cTransferStatus_UnknownError;
    if (returnValue != readSize)
        return_status = I2cTransferStatus_PartialTransfer;
    if (returnValue = readSize)
        return_status = I2cTransferStatus_FullTransfer;
    return return_status;
}
#pragma endregion

#pragma region System.Device.I2c.Slave
CLR_INT32 I2cIO_Slave::TimeoutMaximum()
{
    return 10000;
}
CLR_INT32 I2cIO_Slave::Timeout()
{
    return 100;
}
bool I2cIO_Slave::Initialize(CLR_INT32 I2C_deviceId, CLR_INT32 deviceAddress)
{
    i2c_slave_handler_t handler;
    i2c_inst_t *I2C_Instance = Get_I2C_Instance(I2C_deviceId);
    i2c_slave_init(I2C_Instance, deviceAddress, handler);

#ifdef TEST_CODE
    gpio_set_function(I2C_SLAVE_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SLAVE_SCL_PIN, GPIO_FUNC_I2C);

    gpio_init(I2C_SLAVE_SDA_PIN);
    gpio_init(I2C_SLAVE_SCL_PIN);

    gpio_pull_up(I2C_SLAVE_SDA_PIN);
    gpio_pull_up(I2C_SLAVE_SCL_PIN);

    i2c_init(I2C_Instance, I2C_BAUDRATE);
    i2c_slave_init(I2C_Instance, I2C_SLAVE_ADDRESS, &i2c_slave_handler);
    irq_set_exclusive_handler(num, I2C_deviceId == 0 ? i2c0_slave_irq_handler : i2c1_slave_irq_handler);
    irq_set_enabled(num, true);
#endif
    return true;
}
bool I2cIO_Slave::Dispose(CLR_INT32 I2C_deviceId)
{
    i2c_inst_t *I2C_Instance = Get_I2C_Instance(I2C_deviceId);
    i2c_slave_deinit(I2C_Instance);

#ifdef TEST_CODE

    irq_set_enabled(num, false);
    irq_remove_handler(num, i2c_slave_handler);

    i2c_hw_t *hw = i2c_get_hw(i2c);
    hw->intr_mask = I2C_IC_INTR_MASK_RESET;
    i2c_set_slave_mode(I2C_Instance, false, 0);
#endif

    return true;
}
I2cTransferStatus I2cIO_Slave::Write(
    CLR_INT32 I2C_deviceId,
    CLR_UINT8 *writeBuffer,
    CLR_INT32 writeSize,
    CLR_INT32 timeoutMilliseconds,
    CLR_INT32 *readCount)
{
    uint8_t deviceAddress;
    I2cTransferStatus return_status = I2cTransferStatus::I2cTransferStatus_UnknownError;
    i2c_inst_t *I2C_Instance = Get_I2C_Instance(I2C_deviceId);
    int i2cWriteResult = i2c_write_blocking(I2C_Instance, deviceAddress, writeBuffer, writeSize, true);
    if (i2cWriteResult == PICO_ERROR_TIMEOUT || i2cWriteResult == PICO_ERROR_GENERIC)
    {
        return_status = I2cTransferStatus::I2cTransferStatus_UnknownError;
    }
    else
    {
        return_status = I2cTransferStatus::I2cTransferStatus_FullTransfer;
    }
    return return_status;
}
I2cTransferStatus I2cIO_Slave::Read(
    CLR_INT32 I2C_deviceId,
    CLR_UINT8 *readBuffer,
    CLR_INT32 readSize,
    CLR_INT32 timeoutMilliseconds,
    CLR_INT32 *readCount)
{
    I2cTransferStatus return_status = I2cTransferStatus::I2cTransferStatus_UnknownError;
    uint8_t deviceAddress;
    i2c_inst_t *I2C_Instance = Get_I2C_Instance(I2C_deviceId);
    int i2cReadResult = i2c_read_blocking(
        I2C_Instance,
        deviceAddress,
        readBuffer,
        readSize,
        true); // true to keep master control of bus

    if (i2cReadResult == PICO_ERROR_TIMEOUT || i2cReadResult == PICO_ERROR_GENERIC)
    {
        return_status = I2cTransferStatus::I2cTransferStatus_UnknownError;
    }
    else
    {
        return_status = I2cTransferStatus::I2cTransferStatus_FullTransfer;
    }
    return return_status;
}

#pragma endregion

#pragma region System.Device.Pwm
bool PwmIO::Initialize(
    CLR_INT32 channelId,
    CLR_INT32 timerId,
    CLR_INT32 pinNameValue,
    CLR_INT32 polarity,
    CLR_INT32 desiredFrequency,
    CLR_INT32 dutyCycle)
{
    uint slice_num;
    gpio_set_function(pinNameValue, GPIO_FUNC_PWM);
    if (polarity == 0)
    {
        pwm_set_output_polarity(slice_num, true, true);
    }
    else
    {
        pwm_set_output_polarity(slice_num, false, false);
    }
    slice_num = pwm_gpio_to_slice_num(pinNameValue);

    // Set period of 4 cycles (0 to 3 inclusive)
    int wrapNumber = dutyCycle; // ??????????????????????
    pwm_set_wrap(slice_num, wrapNumber);
    pwm_set_enabled(slice_num, true);

    return true;
}
bool PwmIO::Dispose(CLR_INT32 I2C_deviceId)
{
    // TODO: Deactive PWM, set pins to low power
    // Get each pin and set to low power function
    return true;
}
CLR_UINT32 PwmIO::SetDutyCycle(CLR_INT32 pinNameValue, CLR_INT32 desiredFrequency)
{
    // TODO: calculate duty cycle
    return 1;
}
CLR_UINT32 PwmIO::Start(CLR_INT32 pinNameValue, CLR_INT32 timerId)
{
    CLR_INT32 slice_num = timerId;
    pwm_set_enabled(slice_num, true);
    return 1;
}
CLR_UINT32 PwmIO::Stop(CLR_INT32 pinNameValue, CLR_INT32 timerId)
{
    CLR_INT32 slice_num = timerId;
    pwm_set_enabled(slice_num, false);
    return 1;
}
CLR_UINT32 PwmIO::DesiredFrequency(CLR_INT32 timerId, CLR_INT32 desiredFrequency)
{
    int wrapNumber = desiredFrequency; // ??????????????????????
    int slice_num = timerId;
    pwm_set_wrap(slice_num, wrapNumber);
    return 1;
}
CLR_UINT32 PwmIO::GetChannel(CLR_INT32 timerId, CLR_INT32 pin_number)
{
    return 1;
}
#pragma endregion

#pragma region System.IO.Ports

bool SerialIO::Initialize(CLR_INT32 usartDeviceNumber, CLR_INT32 baudrate)
{
    uart_init(uart_get_instance(usartDeviceNumber), baudrate);
    return true;
}
bool SerialIO::Dispose(CLR_INT32 usartDeviceNumber)
{
    uart_deinit(uart_get_instance(usartDeviceNumber));
    return true;
}
CLR_INT32 SerialIO::BytesAvailable(CLR_INT32 usartDeviceNumber)
{
    uart_inst_t *uart = uart_get_instance(usartDeviceNumber);
    (void)uart;
    return 1;
}
bool SerialIO::ReadBytes(CLR_INT32 usartDeviceNumber, CLR_UINT8 *data, CLR_INT32 length)
{
    (void)length;

    uart_inst_t *uart = uart_get_instance(usartDeviceNumber);
    (void)uart;
    return true;
}
bool SerialIO::ReadLine(CLR_INT32 usartDeviceNumber, char *newLine, CLR_UINT8 length, char *line)
{
    (void)newLine;
    (void)line;
    (void)length;

    uart_inst_t *uart = uart_get_instance(usartDeviceNumber);
    (void)uart;
    return true;
}
bool SerialIO::SetSignalLevels(CLR_INT32 usartDeviceNumber, bool inversion)
{
    (void)inversion;

    uart_inst_t *uart = uart_get_instance(usartDeviceNumber);
    (void)uart;
    return true;
}
bool SerialIO::GetSignalLevels(CLR_INT32 usartDeviceNumber)
{
    // bool result = IsSerialSignalInverted(UsartDeviceNumber);
    return true;
}
bool SerialIO::WriteBytes(CLR_INT32 usartDeviceNumber, CLR_UINT8 *data, CLR_INT32 count)
{
    (void)data;
    (void)count;
    uart_inst_t *uart = uart_get_instance(usartDeviceNumber);
    (void)uart;

    return true;
}
bool SerialIO::SetWatchCharacter(CLR_INT32 usartDeviceNumber, CLR_UINT8 watch_character)
{
    uart_inst_t *uart = uart_get_instance(usartDeviceNumber);
    (void)watch_character;
    return true;
}
bool SerialIO::SetReceiveThreshold(CLR_INT32 usartDeviceNumber, CLR_INT32 threshold)
{
    uart_inst_t *uart = uart_get_instance(usartDeviceNumber);
    (void)threshold;
    return 1;
}
char *SerialIO::GetDevice(CLR_INT32 usartDeviceNumber)
{
    uart_inst_t *uart = uart_get_instance(usartDeviceNumber);
    (void)uart;
    static char DeviceSelector[1];
    DeviceSelector[0] = ' ';
    return DeviceSelector;
}
bool SerialIO::InvertSignalLevels(CLR_INT32 usartDeviceNumber, bool InvertSignal)
{
    (void)InvertSignal;

    uart_inst_t *uart = uart_get_instance(usartDeviceNumber);
    (void)uart;
    return true;
}
bool SerialIO::SetBaudRate(CLR_INT32 usartDeviceNumber, CLR_INT32 baudRate)
{
    int actualBaudRate = uart_set_baudrate(uart_get_instance(usartDeviceNumber), baudRate);
    return true;
}
bool SerialIO::SetConfig(CLR_INT32 usartDeviceNumber, CLR_INT32 stopBits, CLR_INT32 dataBits, CLR_INT32 RequestedParity)
{
    uart_parity_t parity = (uart_parity_t)RequestedParity;
    uart_set_format(uart_get_instance(usartDeviceNumber), dataBits, stopBits, parity);
    return true;
}
bool SerialIO::SetHandshake(CLR_INT32 usartDeviceNumber, Handshake handshake)
{
    bool cts = false;
    bool rts = false;
    bool status = false;

    switch (handshake)
    {
        case Handshake::Handshake_None:
            status = true;
            break;
        case Handshake_XOnXOff:
            break;
        case Handshake::Handshake_RequestToSend:
            cts = true;
            rts = true;
            status = true;
            break;
        case Handshake::Handshake_RequestToSendXOnXOff:
            status = true;
            break;
    }

    uart_set_hw_flow(uart_get_instance(usartDeviceNumber), cts, rts);
    return status;
}
HRESULT SerialIO::SetupWriteLine(CLR_RT_StackFrame &stack, char **buffer, uint32_t *length, bool *isNewAllocation)
{
    UNUSED(stack);
    UNUSED(buffer);
    UNUSED(length);
    UNUSED(isNewAllocation);
    return 1;
}

bool SerialIO::SetMode(CLR_INT32 UsartDeviceNumber, CLR_INT32 mode)
{
    return true;
}
#pragma endregion

#pragma region System.Device.SPI
CLR_INT32 SpiIO::MaximumClockFrequency(CLR_INT32 controllerID)
{
    return 1;
}
CLR_INT32 SpiIO::MinimumClockFrequency(CLR_INT32 controllerID)
{
    return 1;
}
bool SpiIO::Initialize()
{
    return true;
}
bool SpiIO::Dispose(CLR_INT32 deviceId)
{
    return true;
}
bool SpiIO::WriteRead(
    CLR_INT32 deviceId,
    SPI_WRITE_READ_SETTINGS rws,
    CLR_UINT8 *writeData,
    CLR_UINT16 writeSize,
    CLR_UINT8 *readData,
    CLR_UINT16 readSize)
{
    return false;
}
bool SpiIO::Open(SPI_DEVICE_CONFIGURATION spiConfig, CLR_UINT32 handle)
{
    return false;
}
CLR_INT32 SpiIO::ByteTime()
{
    return 1;
}
SPI_OP_STATUS SpiIO::Completed(CLR_INT32 deviceId)
{
    UNUSED(deviceId);
    return SPI_OP_STATUS::SPI_OP_COMPLETE;
}

#pragma endregion
