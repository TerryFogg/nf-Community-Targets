//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "DisplayInterface.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "sys_dev_spi_native.h"
#include <nanoPAL.h>
#include <target_platform.h>

#define NUMBER_OF_LINES 2
#define SPI_MAX_TRANSFER_SIZE 960

#if PICO_RP2040
#define MAX_SPI_BAUD_RATE 62500000
#elif PICO_RP2350
#define MAX_SPI_BAUD_RATE 75000000
#endif

struct DisplayInterface g_DisplayInterface;
DisplayInterfaceConfig g_DisplayInterfaceConfig;

CLR_INT16 outputBufferSize;
CLR_UINT8 spiBuffer[SPI_MAX_TRANSFER_SIZE];
CLR_UINT8 spiCommandMode = 0; // 0 Command first byte, 1 = Command all bytes

static uint dma_tx;
static dma_channel_config dma_tx_config;

void SendCommandBytes(CLR_UINT8 *data, CLR_UINT32 length);
void SendDataBytes(CLR_UINT8 *data, CLR_UINT32 length);
spi_inst_t *spi_port;

// Display Interface
void DisplayInterface::Initialize(DisplayInterfaceConfig &config)
{
    (void)config;

    m_TouchI2cBus = TouchI2cBus;
    m_TouchI2cSlaveAddress = TouchI2cSlaveAddress;
    I2cIO::Initialize(TouchI2cBus, I2cBusSpeed_StandardMode, I2C_CONTROL_TYPE::MASTER, TouchI2cSlaveAddress);

    spi_init(spi_port, MAX_SPI_BAUD_RATE);
    gpio_set_function(SPI_CLK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CS, GPIO_FUNC_SPI);

    // Configure GPIO's
    // ==============

    // Data/Command
    gpio_init(LCD_DC);
    gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_put(LCD_DC, 1);

    gpio_init(LCD_BACKLIGHT);
    gpio_set_dir(LCD_BACKLIGHT, GPIO_OUT);
    // gpio_put(LCD_BACKLIGHT, 1);

    // Reset
    gpio_init(LCD_RESET);
    gpio_set_dir(LCD_RESET, GPIO_OUT);
    gpio_put(LCD_RESET, 1);
    PLATFORM_DELAY(10);
    gpio_put(LCD_RESET, 0);
    PLATFORM_DELAY(10);
    gpio_put(LCD_RESET, 1);
    PLATFORM_DELAY(10);

    // Setup DMA
    // We set the outbound DMA to transfer from a memory buffer to the SPI
    // transmit FIFO paced by the SPI TX FIFO DREQ
    // The default is for the read address to increment every element (in this
    // case 1 byte = DMA_SIZE_8) and for the write address to remain unchanged.

    dma_tx = dma_claim_unused_channel(true);
    dma_tx_config = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&dma_tx_config, DMA_SIZE_8);
    channel_config_set_dreq(&dma_tx_config, spi_get_dreq(spi_port, true));
    dma_channel_configure(dma_tx, &dma_tx_config, &spi_get_hw(spi_port)->dr, NULL, 0, false);
    return;
}
void DisplayInterface::GetTransferBuffer(CLR_UINT8 *&TransferBuffer, CLR_UINT32 &TransferBufferSize)
{
    TransferBuffer = spiBuffer;
    TransferBufferSize = sizeof(spiBuffer);
}
void DisplayInterface::ClearFrameBuffer()
{
    // Set screen to black
}
void DisplayInterface::WriteToFrameBuffer(
    CLR_UINT8 command,
    CLR_UINT8 data[],
    CLR_UINT32 dataCount,
    CLR_UINT32 frameOffset)
{
    (void)frameOffset;
    SendCommandBytes(&command, 1);
    SendDataBytes(data, dataCount);
    return;
}
void DisplayInterface::SendCommand(CLR_UINT8 arg_count, ...)
{
    va_list ap;
    va_start(ap, arg_count);

    // Parse arguments into parameters buffer
    CLR_UINT8 parameters[arg_count];
    for (int i = 0; i < arg_count; i++)
    {
        parameters[i] = va_arg(ap, int);
    }
    SendCommandBytes(&parameters[0], 1);
    if (arg_count > 1)
    {
        SendDataBytes(&parameters[1], arg_count - 1);
    }
}
void DisplayInterface::DisplayBacklight(bool on) // true = on
{
    if (on)
    {
        gpio_put(LCD_BACKLIGHT, 1);
    }
    else
    {
        gpio_put(LCD_BACKLIGHT, 0);
    }
    return;
}
void SendCommandBytes(CLR_UINT8 *data, CLR_UINT32 length)
{
    // Wait for all DMA writes to SPI
    dma_channel_wait_for_finish_blocking(dma_tx);

    // And all SPI data written out of the Mcu
    while (spi_is_busy(spi_port))
        tight_loop_contents();

    gpio_put(LCD_DC, GpioPinValue_Low);
    dma_channel_transfer_from_buffer_now(dma_tx, data, length);
}
void SendDataBytes(CLR_UINT8 *data, CLR_UINT32 length)
{
    // Wait for all DMA writes to SPI
    dma_channel_wait_for_finish_blocking(dma_tx);
    // And all SPI data written out of the Mcu
    while (spi_is_busy(spi_port))
        tight_loop_contents();

    gpio_put(LCD_DC, GpioPinValue_High);
    dma_channel_transfer_from_buffer_now(dma_tx, data, length);
}
