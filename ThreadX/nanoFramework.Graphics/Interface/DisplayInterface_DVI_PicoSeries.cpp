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

struct DisplayInterface g_DisplayInterface;
DisplayInterfaceConfig g_DisplayInterfaceConfig;

// Display Interface
void DisplayInterface::Initialize(DisplayInterfaceConfig &config)
{
    (void)config;
    return;
}
void DisplayInterface::GetTransferBuffer(CLR_UINT8 *&TransferBuffer, CLR_UINT32 &TransferBufferSize)
{
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
    return;
}
void DisplayInterface::SendCommand(CLR_UINT8 arg_count, ...)
{
}
void DisplayInterface::DisplayBacklight(bool on) // true = on
{
    //if (on)
    //{
    //    gpio_put(LCD_BACKLIGHT, 1);
    //}
    //else
    //{
    //    gpio_put(LCD_BACKLIGHT, 0);
    //}
    //return;
}
void SendCommandBytes(CLR_UINT8 *data, CLR_UINT32 length)
{
}
void SendDataBytes(CLR_UINT8 *data, CLR_UINT32 length)
{
}


#pragma region TMDS encoder (Transition Minimized Differential Signaling)
 //The TMDS encoder performs 8b / 10b encoding,
 //   - Takes 8 - bit video data(like pixel color values) and converts it into a 10 - bit code.
 //   - This encoding minimizes the number of transitions(changes from 0 to 1 or vice versa) to reduce
 //     EMI(electromagnetic interference)
 //   - Maintains DC balance, which helps with reliable clock recovery and signal stability over long cables.
 //   - Red,Green,Blue channels are encoded separately.
 //   - Control signals : Like horizontal and vertical sync, are also encoded during blanking intervals.


#pragma endregion
