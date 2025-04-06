//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "DeviceIO.h"

#pragma region System.Device.Gpio
GpioIO::GpioIO()
{
}
bool GpioIO::InitializePin(PinNameValue pinNameValue)
{
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

#pragma region System.Device.Adc
static ADC_Properties *mcuADC;
void AdcIO::SetupAdcList(ADC_Properties *boardAdcDefinitions)
{
    mcuADC = boardAdcDefinitions;
}
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
    return 3;
}
bool AdcIO::Initialize()
{
    adc_init();
    return true;
}
bool AdcIO::Open(CLR_INT32 adc_channel_number)
{
    if (!mcuADC[adc_channel_number].ADC_Initialized)
    {
        adc_gpio_init(mcuADC[adc_channel_number].adc);
        mcuADC[adc_channel_number].ADC_Initialized = true;
    }
    return true;
}
bool AdcIO::Dispose(CLR_INT32 adc_channel_number)
{
    if (mcuADC[adc_channel_number].ADC_Initialized)
    {
        GpioIO::SetLowPower(mcuADC[adc_channel_number].adc);
    }
    return true;
}
CLR_UINT16 AdcIO::Read(CLR_INT32 adc_channel_number)
{
    adc_select_input(adc_channel_number);
    return adc_read();
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

#pragma region System.Device.Dac
int DacIO::Resolution()
{
    // No DAC
    return 0;
}
int DacIO::ChannelCount()
{
    // No DAC
    return 0;
}
bool DacIO::Initialize(CLR_INT32 dac_channel_number)
{
    // No DAC
    (void)dac_channel_number;
    return false;
}
bool DacIO::Open(CLR_INT32 dac_channel_number)
{
    // No DAC
    (void)dac_channel_number;
    return false;
}
char *DacIO::DeviceSelector(int dac_channel_number)
{
    // No DAC
    (void)dac_channel_number;
    static char DeviceNameDummy[] = "NotSupported\0";
    return DeviceNameDummy;
}
void DacIO::Write(CLR_INT32 dac_channel_number, CLR_INT32 value)
{
    // No DAC
    (void)dac_channel_number;
    (void)value;
}
PinNameValue DacIO::ChannelToPin(CLR_INT32 dac_channel_number)
{
    // No DAC
    (void)dac_channel_number;
    return (PinNameValue)-1;
}
bool DacIO::Dispose(CLR_INT32 dac_channel_number)
{
    // No DAC
    (void)dac_channel_number;
    return true;
}
#pragma endregion

#pragma region System.Device.I2c
// Reserve 256 byte memory for the I2C slave.
// To write a series of bytes, the master first writes the memory address, followed by the data.
// The address is automatically incremented for each byte transferred, looping back to 0 upon reaching the end.
// Reading is done sequentially from the current memory address.
static struct
{
    uint8_t mem[256];
    uint8_t mem_address;
    bool mem_address_written;
} context;

static I2C_Properties *mcuI2C;
void I2cIO::SetupI2CList(I2C_Properties *boardMcuI2C)
{
    mcuI2C = boardMcuI2C;
}

// The handler is called from the I2C ISR, so it must complete quickly.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    switch (event)
    {
        case I2C_SLAVE_RECEIVE:
            if (!context.mem_address_written)
            {
                context.mem_address = i2c_read_byte_raw(i2c);
                context.mem_address_written = true;
            }
            else
            {
                context.mem[context.mem_address] = i2c_read_byte_raw(i2c);
                context.mem_address++;
            }
            break;
        case I2C_SLAVE_REQUEST:
            i2c_write_byte_raw(i2c, context.mem[context.mem_address]);
            context.mem_address++;
            break;
        case I2C_SLAVE_FINISH:
            context.mem_address_written = false;
            break;
        default:
            break;
    }
}
bool I2cIO::Initialize(
    CLR_INT32 I2C_deviceId,
    I2cBusSpeed I2C_speed,
    I2C_CONTROL_TYPE I2C_control_type,
    CLR_INT32 deviceAddress)
{
    if (!mcuI2C[I2C_deviceId].I2C_Initialized)
    {
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

        gpio_set_function(mcuI2C[I2C_deviceId].sda, GPIO_FUNC_I2C);
        gpio_pull_up(mcuI2C[I2C_deviceId].sda);
        gpio_set_function(mcuI2C[I2C_deviceId].scl, GPIO_FUNC_I2C);
        gpio_pull_up(mcuI2C[I2C_deviceId].scl);

        switch (I2C_control_type)
        {
            case I2C_CONTROL_TYPE::MASTER:
                i2c_init((i2c_inst *)(mcuI2C[I2C_deviceId].i2c_instance), baud);
                break;
            case I2C_CONTROL_TYPE::SLAVE:
                i2c_slave_init((i2c_inst *)(mcuI2C[I2C_deviceId].i2c_instance), deviceAddress, &i2c_slave_handler);
                break;
        }
        mcuI2C[I2C_deviceId].I2C_Initialized = true;
    }
    return true;
}
bool I2cIO::Dispose(CLR_INT32 I2C_deviceId)
{
    if (!mcuI2C[I2C_deviceId].I2C_Initialized)
    {
        i2c_deinit((i2c_inst *)(mcuI2C[I2C_deviceId].i2c_instance));
    }
    return true;
}
I2cTransferStatus I2cIO::Write(
    CLR_INT32 I2C_deviceId,
    CLR_INT32 slaveAddress,
    CLR_UINT8 *writeBuffer,
    CLR_INT32 writeSize,
    I2C_CONTROL_TYPE busType)
{
    I2cTransferStatus return_status = I2cTransferStatus_UnknownError;
    bool is_master = (busType == I2C_CONTROL_TYPE::MASTER) ? true : false;

    int returnValue = i2c_write_blocking(
        (i2c_inst *)(mcuI2C[I2C_deviceId].i2c_instance),
        slaveAddress,
        writeBuffer,
        writeSize,
        is_master); // true to keep master control of bus

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
    int returnValue =
        i2c_read_blocking((i2c_inst *)(mcuI2C[I2C_deviceId].i2c_instance), slaveAddress, readBuffer, readSize, false);

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

#pragma region System.Device.Pwm
static PWM_Properties *mcuPWM;
void PwmIO::SetupPwmList(PWM_Properties *boardPWMDefinitions)
{
    mcuPWM = boardPWMDefinitions;
}

bool PwmIO::Initialize(
    CLR_INT32 sliceNumber,
    CLR_INT32 timerId,
    CLR_INT32 pinNameValue,
    CLR_INT32 polarity,
    CLR_INT32 desiredFrequency,
    CLR_INT32 dutyCycle)
{
    if (!mcuPWM[sliceNumber].PWM_Initialized)
    {
        gpio_set_function(mcuPWM[sliceNumber].channelA, GPIO_FUNC_PWM);
        gpio_set_function(mcuPWM[sliceNumber].channelB, GPIO_FUNC_PWM);
    }
    if (polarity == 0)
    {
        pwm_set_output_polarity(sliceNumber, true, true);
    }
    else
    {
        pwm_set_output_polarity(sliceNumber, false, false);
    }
    CLR_INT32 slice_num = pwm_gpio_to_slice_num(mcuPWM[sliceNumber].channelA);

    // Set period of 4 cycles (0 to 3 inclusive)
    int wrapNumber = dutyCycle; // ??????????????????????
    pwm_set_wrap(slice_num, wrapNumber);
    pwm_set_enabled(slice_num, true);

    return true;
}
bool PwmIO::Dispose(CLR_INT32 sliceNumber)
{
    if (!mcuPWM[sliceNumber].PWM_Initialized)
    {
        gpio_set_function(mcuPWM[sliceNumber].channelA, GPIO_FUNC_PWM);
        gpio_set_function(mcuPWM[sliceNumber].channelB, GPIO_FUNC_PWM);
        mcuPWM[sliceNumber].PWM_Initialized = false;
    }
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
static USART_Properties *mcuUSART;
void SerialIO::SetupUsartList(USART_Properties *boardMcuUsart)
{
    mcuUSART = boardMcuUsart;
}
bool SerialIO::Initialize(CLR_INT32 usartDeviceNumber, CLR_INT32 baudrate)
{
    if (!mcuUSART[usartDeviceNumber].USART_Initialized)
    {
        uart_init(uart_get_instance(usartDeviceNumber), baudrate);
        gpio_set_function(mcuUSART[usartDeviceNumber].tx, GPIO_FUNC_UART);
        gpio_set_function(mcuUSART[usartDeviceNumber].rx, GPIO_FUNC_UART);
        mcuUSART[usartDeviceNumber].USART_Initialized = true;
    }
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
bool SerialIO::SetConfig(
    CLR_INT32 usartDeviceNumber,
    SerialMode serialMode,
    CLR_INT32 stopBits,
    CLR_INT32 dataBits,
    CLR_INT32 RequestedParity)
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
    (void)stack;
    (void)buffer;
    (void)length;
    (void)isNewAllocation;
    return 1;
}
bool SerialIO::SetMode(CLR_INT32 UsartDeviceNumber, CLR_INT32 mode)
{
    return true;
}
#pragma endregion

#pragma region System.Device.SPI

static SPI_Properties *mcuSPI;
void SpiIO::SetupSpiList(SPI_Properties *boardMcuSPI)
{
    mcuSPI = boardMcuSPI;
}
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
    (void)deviceId;
    return SPI_OP_STATUS::SPI_OP_COMPLETE;
}
#pragma endregion
