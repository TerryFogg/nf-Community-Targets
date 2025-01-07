//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "Display.h"
#include "DisplayInterface.h"
#include "Graphics.h"
#include "board.h"

struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;

void SendCommand(uint8_t NbrParams, ...);
int lcd_width;
int lcd_height;

//// Note to self
//// Something strange is happening and
//// static int counter fixes problem????????????
static int counter = 0;

bool DisplayDriver::Initialize()
{
    counter += 1;
    lcd_width = 480;
    lcd_height = 480;

    // CMD Mode
    SendCommand(4, 0xB9, 0xFF, 0x83, 0x79);

    // Set Pixel format - 1 Parameter
    SendCommand(1 + 1, 0x3A, 0x55);

    // Set power - 16 Parameters
    SendCommand(
        1 + 16,
        0xB1,
        0x44,
        0x1C,
        0x1C,
        0x37,
        0x57,
        0x90,
        0xD0,
        0xE2,
        0x58,
        0x80,
        0x38,
        0x38,
        0xF8,
        0x33,
        0x34,
        0x42);

    // Set display related register - 6 Parameters? but 9?
    SendCommand(1 + 9, 0xB2, 0x80, 0x14, 0x0C, 0x30, 0x20, 0x50, 0x11, 0x42, 0x1D);

    // Set display cycle timing - 10 Parameters
    SendCommand(1 + 10, 0xB4, 0x01, 0xAA, 0x01, 0xAF, 0x01, 0xAF, 0x10, 0xEA, 0x1C, 0xEA);

    // SETVCOM ?
    SendCommand(5, 0xC7, 00, 00, 00, 0xC0);

    // Set panel related register - 1 Parameter
    SendCommand(1 + 1, 0xCC, 0x02);

    // Set GIP timing -  28 Parameter?
    SendCommand(
        38,
        0xD2,
        0x00,
        0x07,
        0x00,
        0x00,
        0x00,
        0x08,
        0x08,
        0x32,
        0x10,
        0x01,
        0x00,
        0x01,
        0x03,
        0x72,
        0x03,
        0x72,
        0x00,
        0x08,
        0x00,
        0x08,
        0x33,
        0x33,
        0x05,
        0x05,
        0x37,
        0x05,
        0x05,
        0x37,
        0x0A,
        0x00,
        0x00,
        0x00,
        0x0A,
        0x00,
        0x01,
        0x00,
        0x0E);

    // Set GIP timing - 28 parameters?
    SendCommand(
        38,
        0xD3,
        0x00,
        0x07,
        0x00,
        0x00,
        0x00,
        0x08,
        0x08,
        0x32,
        0x10,
        0x01,
        0x00,
        0x01,
        0x03,
        0x72,
        0x03,
        0x72,
        0x00,
        0x08,
        0x00,
        0x08,
        0x33,
        0x33,
        0x05,
        0x05,
        0x37,
        0x05,
        0x05,
        0x37,
        0x0A,
        0x00,
        0x00,
        0x00,
        0x0A,
        0x00,
        0x01,
        0x00,
        0x0E);

    // Set forward GIP sequence - 35 parameter
    SendCommand(
        1 + 34,
        0xD5,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x19,
        0x19,
        0x18,
        0x18,
        0x18,
        0x18,
        0x19,
        0x19,
        0x01,
        0x00,
        0x03,
        0x02,
        0x05,
        0x04,
        0x07,
        0x06,
        0x23,
        0x22,
        0x21,
        0x20,
        0x18,
        0x18,
        0x18,
        0x18,
        0x00,
        0x00);

    // Set backward GIP sequence - 32 parameters
    SendCommand(
        1 + 32,
        0xD6,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x19,
        0x19,
        0x18,
        0x18,
        0x19,
        0x19,
        0x18,
        0x18,
        0x06,
        0x07,
        0x04,
        0x05,
        0x02,
        0x03,
        0x00,
        0x01,
        0x20,
        0x21,
        0x22,
        0x23,
        0x18,
        0x18,
        0x18,
        0x18);

    // Set gamma curve related setting - 42 Parameters
    SendCommand(
        1 + 42,
        0xE0,
        0x00,
        0x16,
        0x1B,
        0x30,
        0x36,
        0x3F,
        0x24,
        0x40,
        0x09,
        0x0D,
        0x0F,
        0x18,
        0x0E,
        0x11,
        0x12,
        0x11,
        0x14,
        0x07,
        0x12,
        0x13,
        0x18,
        0x00,
        0x17,
        0x1C,
        0x30,
        0x36,
        0x3F,
        0x24,
        0x40,
        0x09,
        0x0C,
        0x0F,
        0x18,
        0x0E,
        0x11,
        0x14,
        0x11,
        0x12,
        0x07,
        0x12,
        0x14,
        0x18);

    // Set VCOM voltage - 3 parameters
    SendCommand(1 + 3, 0xB6, 0x2C, 0x2C, 0x00);

    // Set register bank partition index BANK0
    SendCommand(1 + 1, 0xBD, 0x00);
    // Set DGC LUT - BANK 0 - 43 parameters
    SendCommand(
        1 + 42,
        0xC1,
        0x01,
        0x00,
        0x07,
        0x0F,
        0x16,
        0x1F,
        0x27,
        0x30,
        0x38,
        0x40,
        0x47,
        0x4E,
        0x56,
        0x5D,
        0x65,
        0x6D,
        0x74,
        0x7D,
        0x84,
        0x8A,
        0x90,
        0x99,
        0xA1,
        0xA9,
        0xB0,
        0xB6,
        0xBD,
        0xC4,
        0xCD,
        0xD4,
        0xDD,
        0xE5,
        0xEC,
        0xF3,
        0x36,
        0x07,
        0x1C,
        0xC0,
        0x1B,
        0x01,
        0xF1,
        0x34,
        0x00);

    // Set register bank partition index BANK1
    SendCommand(2, 0xBD, 0x01);
    // Set DGC LUT - BANK 0 - 42 parameters
    SendCommand(
        1 + 42,
        0xC1,
        0x00,
        0x08,
        0x0F,
        0x16,
        0x1F,
        0x28,
        0x31,
        0x39,
        0x41,
        0x48,
        0x51,
        0x59,
        0x60,
        0x68,
        0x70,
        0x78,
        0x7F,
        0x87,
        0x8D,
        0x94,
        0x9C,
        0xA3,
        0xAB,
        0xB3,
        0xB9,
        0xC1,
        0xC8,
        0xD0,
        0xD8,
        0xE0,
        0xE8,
        0xEE,
        0xF5,
        0x3B,
        0x1A,
        0xB6,
        0xA0,
        0x07,
        0x45,
        0xC5,
        0x37,
        0x00);

    // Set register bank partition index BANK2
    SendCommand(2, 0xBD, 0x02);
    // Set DGC LUT - BANK 0 - 42 parameters
    SendCommand(
        1 + 42,
        0xC1,
        0x00,
        0x09,
        0x0F,
        0x18,
        0x21,
        0x2A,
        0x34,
        0x3C,
        0x45,
        0x4C,
        0x56,
        0x5E,
        0x66,
        0x6E,
        0x76,
        0x7E,
        0x87,
        0x8E,
        0x95,
        0x9D,
        0xA6,
        0xAF,
        0xB7,
        0xBD,
        0xC5,
        0xCE,
        0xD5,
        0xDF,
        0xE7,
        0xEE,
        0xF4,
        0xFA,
        0xFF,
        0x0C,
        0x31,
        0x83,
        0x3C,
        0x5B,
        0x56,
        0x1E,
        0x5A);

    // Set register bank partition index BANK0
    SendCommand(2, 0xBD, 0x00);

    // Exit Sleep Mode - No Parameters ( 0 end)
    SendCommand(2, 0x11, 0x00);

    DelayMilliseconds(120);

    // Clear LCD_FRAME_BUFFER
    memset((uint32_t *)LCD_FRAME_BUFFER, 0x00, 0xFFFFF);

    // Set display on - No Parameters (0 end)
    SendCommand(2, 0x29, 0x00);

    DelayMilliseconds(120);


    SetupDisplayAttributes();
    SetDefaultOrientation();

    Clear();
    return 0;
}

void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = lcd_width;
    Attributes.ShorterSide = lcd_height;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    g_DisplayInterface.DisplayBacklight(true);
}

bool DisplayDriver::ChangeOrientation(DisplayOrientation orientation)
{
    switch (orientation)
    {
        case DisplayOrientation_Portrait:
            Attributes.Height = Attributes.LongerSide;
            Attributes.Width = Attributes.ShorterSide;

            break;
        case DisplayOrientation_Portrait180:
            Attributes.Height = Attributes.LongerSide;
            Attributes.Width = Attributes.ShorterSide;
            // Landscape only at the moment
            break;
        case DisplayOrientation_Landscape:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            break;
        case DisplayOrientation_Landscape180:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            break;
    }
    return true;
}

void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(DisplayOrientation_Landscape);
}

bool DisplayDriver::Uninitialize()
{
    Clear();
    return TRUE;
}

void DisplayDriver::PowerSave(PowerSaveState powerState)
{
    switch (powerState)
    {
        default:
            // Illegal fall through to Power on
        case PowerSaveState::NORMAL:
            //        ??
            break;
        case PowerSaveState::SLEEP:
            //       ??
            break;
    }
    return;
}
void DisplayDriver::Clear()
{
    // Clear Frame buffer
    g_DisplayInterface.ClearFrameBuffer();
}

void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
}

void DisplayDriver::BitBlt(int x, int y, int width, int height, int stride, int screenX, int screenY, CLR_UINT32 data[])
{
    (void)stride;
    (void)screenX;
    (void)screenY;

    CLR_INT32 dataSize = width * height * 2;
    if (dataSize == (Attributes.Width * Attributes.Height * 2))
    {
        // Full screen
        g_DisplayInterface.WriteToFrameBuffer(0, (CLR_UINT8 *)data, dataSize, 0);
    }
    else
    {
        // Partial bitblt
        CLR_UINT16 *p16data = (CLR_UINT16 *)&data[0];
        CLR_UINT32 srcOffset = (y * (CLR_UINT32)Attributes.Width) + x;
        p16data += srcOffset;
        dataSize = width * 2;
        CLR_UINT32 targetOffset = srcOffset; // Target 16bit offset in frame
        while (height--)
        {
            g_DisplayInterface.WriteToFrameBuffer(0, (CLR_UINT8 *)p16data, dataSize, targetOffset);
            p16data += Attributes.Width;      // Next display row in data[]
            targetOffset += Attributes.Width; // Next offset in target frame
        }
    }
}

CLR_UINT32 DisplayDriver::PixelsPerWord()
{
    return (32 / Attributes.BitsPerPixel);
}
CLR_UINT32 DisplayDriver::WidthInWords()
{
    return ((Attributes.Width + (PixelsPerWord() - 1)) / PixelsPerWord());
}
CLR_UINT32 DisplayDriver::SizeInWords()
{
    return (WidthInWords() * Attributes.Height);
}
CLR_UINT32 DisplayDriver::SizeInBytes()
{
    return (SizeInWords() * sizeof(CLR_UINT32));
}

void SendCommand(uint8_t NbrParams, ...)
{
    va_list ap;
    va_start(ap, NbrParams);
    volatile uint8_t command;
    DSI_TypeDef *DsiRegister = DSI;

    // Wait for Command FIFO Empty
    while ((DsiRegister->GPSR & DSI_GPSR_CMDFE) == 0U)
    {
    };

    switch (NbrParams)
    {
        case 1:
        {
            command = va_arg(ap, int);
            DsiRegister->GHCR = (DSI_DCS_SHORT_PKT_WRITE_P1 | (command << 8U));
            break;
        }
        case 2:
        {
            command = va_arg(ap, int);
            volatile uint8_t parameter = va_arg(ap, int);
            DsiRegister->GHCR = (DSI_DCS_SHORT_PKT_WRITE_P1 | (command << 8U) | (parameter << 16U));
            break;
        }
        default:
        {
            // Set the DCS code on payload byte 1, and the other parameters on the write FIFO command
            uint32_t fifoword = va_arg(ap, int);
            uint32_t nbBytes = (NbrParams < 3U) ? NbrParams : 3U;
            for (uint8_t count = 0U; count < nbBytes; count++)
            {
                uint8_t parameter = va_arg(ap, int);
                fifoword |= parameter << (8U + (8U * count));
            }
            DsiRegister->GPDR = fifoword;

            uint32_t uicounter = NbrParams - nbBytes;
            while (uicounter != 0U) // Set the Next parameters on the write FIFO command
            {
                nbBytes = (uicounter < 4U) ? uicounter : 4U;
                fifoword = 0U;
                for (uint8_t count = 0U; count < nbBytes; count++)
                {
                    uint8_t parameter = va_arg(ap, int);
                    fifoword |= parameter << (8U * count);
                }
                DsiRegister->GPDR = fifoword;
                uicounter -= nbBytes;
            }
            // Update the DSI packet header with new information
            DsiRegister->GHCR =
                (DSI_DCS_LONG_PKT_WRITE | (((NbrParams + 1U) & 0x00FFU) << 8U) |
                 ((((NbrParams + 1U) & 0xFF00U) >> 8U) << 16U));
        }
    }
}
