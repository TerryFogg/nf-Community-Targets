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
//#define SPI_MAX_TRANSFER_SIZE (320 * 2 * NUMBER_OF_LINES) // 240 pixels 2 words wide (16 bit colour)
#define SPI_MAX_TRANSFER_SIZE 960
#define MAX_SPI_BAUD_RATE 62500000

struct DisplayInterface g_DisplayInterface;
DisplayInterfaceConfig g_DisplayInterfaceConfig;

int clock = 10;
int MOSI = 11;

int lcdchipSelect;
int lcdDC;
int lcdReset;
int lcdBacklight;
int width = 240;
int height = 135;

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
  lcdchipSelect = config.Spi.chipSelect;
  lcdDC = config.Spi.dataCommand;
  lcdReset = config.Spi.reset;
  lcdBacklight = config.Spi.backLight;
  width = config.Screen.width;
  height = config.Screen.height;

  switch (config.Spi.spiBus) 
  {
  case 0:
    spi_port = spi0;
    break;
  case 1:
    spi_port = spi1;
    break;
  }

  spi_init(spi_port, MAX_SPI_BAUD_RATE);
  gpio_set_function(clock, GPIO_FUNC_SPI);
  gpio_set_function(MOSI, GPIO_FUNC_SPI);
  gpio_set_function(lcdchipSelect, GPIO_FUNC_SPI);

  // Configure GPIO's
  // ==============

  // Reset
  gpio_init(lcdReset);
  gpio_set_dir(lcdReset, GPIO_OUT);
  gpio_put(lcdReset, 1);

  // Data/Command
  gpio_init(lcdDC);
  gpio_set_dir(lcdDC, GPIO_OUT);
  gpio_put(lcdDC, 1);

  //    lcdchipSelect = config.Spi.chipSelect;
  //    gpio_init(lcdchipSelect);
  //    gpio_set_dir(lcdchipSelect, GPIO_OUT);
  //    gpio_put(lcdchipSelect, 1);

  gpio_init(lcdBacklight);
  gpio_set_dir(lcdBacklight, GPIO_OUT);
  gpio_put(lcdBacklight, 1);

  gpio_put(lcdReset, 1);
  PLATFORM_DELAY(100);
  gpio_put(lcdReset, 0);
  PLATFORM_DELAY(100);
  gpio_put(lcdReset, 1);
  PLATFORM_DELAY(100);

  // Setup DMA
  // We set the outbound DMA to transfer from a memory buffer to the SPI
  // transmit FIFO paced by the SPI TX FIFO DREQ
  // The default is for the read address to increment every element (in this
  // case 1 byte = DMA_SIZE_8) and for the write address to remain
  // unchanged.

  dma_tx = dma_claim_unused_channel(true);
  dma_tx_config = dma_channel_get_default_config(dma_tx);
  channel_config_set_transfer_data_size(&dma_tx_config, DMA_SIZE_8);
  channel_config_set_dreq(&dma_tx_config, spi_get_dreq(spi_port, true));
  dma_channel_configure(dma_tx, &dma_tx_config, &spi_get_hw(spi_port)->dr, NULL, 0, false);
  return;
}
void DisplayInterface::SetCommandMode(int mode)
{
  spiCommandMode = mode;
}
void DisplayInterface::GetTransferBuffer(CLR_UINT8 *&TransferBuffer,
                                         CLR_UINT32 &TransferBufferSize)
{
  TransferBuffer = spiBuffer;
  TransferBufferSize = sizeof(spiBuffer);
}

void DisplayInterface::ClearFrameBuffer()
{
  // Set screen to black
}

void DisplayInterface::WriteToFrameBuffer(CLR_UINT8 command, CLR_UINT8 data[],
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
    gpio_put(lcdBacklight, 1);
  } 
  else 
  {
    gpio_put(lcdBacklight, 0);
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
  
  gpio_put(lcdDC, GpioPinValue_Low);
  dma_channel_transfer_from_buffer_now(dma_tx, data, length);
}
void SendDataBytes(CLR_UINT8 *data, CLR_UINT32 length) {
  // Wait for all DMA writes to SPI 
  dma_channel_wait_for_finish_blocking(dma_tx);
  // And all SPI data written out of the Mcu
  while (spi_is_busy(spi_port))
    tight_loop_contents();

  gpio_put(lcdDC, GpioPinValue_High);
  dma_channel_transfer_from_buffer_now(dma_tx, data, length);
}
