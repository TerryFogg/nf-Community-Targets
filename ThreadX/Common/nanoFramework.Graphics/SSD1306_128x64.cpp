//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Graphics.h"
#include "DisplayInterface.h"
#include "Display.h"

/*
SSD1306 ar small mono display of either 128x64 or 128x32
The interface is I2C
*/
struct DisplayDriver g_DisplayDriver;
extern DisplayInterface g_DisplayInterface;

enum SSD1306_CMD : CLR_UINT8
{
    Set_Memory_Addressing_Mode = 0x20,
    Set_Column_Address = 0x21,
    Set_Page_address = 0x22,
    Memory_Write = 0x40,
    Set_Contrast = 0x81,
    Charge_Pump = 0x8D,
    Set_Segment_remap127 = 0xA1,
    Set_Normal = 0xA6,
    Set_Inversion = 0xA6,
    Display_OFF = 0xAE,
    Display_ON = 0xAF,
    Set_COM_Scan_x = 0xC8
};
enum _Orientation : CLR_UINT8
{
    MADCTL_MH = 0x04, // sets the Horizontal Refresh, 0=Left-Right and 1=Right-Left
    MADCTL_ML = 0x10, // sets the Vertical Refresh, 0=Top-Bottom and 1=Bottom-Top
    MADCTL_MV = 0x20, // sets the Row/Column Swap, 0=Normal and 1=Swapped
    MADCTL_MX = 0x40, // sets the Column Order, 0=Left-Right and 1=Right-Left
    MADCTL_MY = 0x80, // sets the Row Order, 0=Top-Bottom and 1=Bottom-Top
    MADCTL_BGR = 0x08 // Blue-Green-Red pixel order, 0 = RGB, 1 = BGR
};
bool DisplayDriver::Initialize()
{
    SetupDisplayAttributes();
    g_DisplayInterface.SendCommand(2, SSD1306_CMD::Charge_Pump, 0x14);
    g_DisplayInterface.SendCommand(2, SSD1306_CMD::Set_Memory_Addressing_Mode, 0x00);
    SetDefaultOrientation();
    Clear();
    g_DisplayInterface.SendCommand(1, SSD1306_CMD::Display_ON);
    return true;
}
void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.Width = LCD_WIDTH;
    Attributes.Height = LCD_HEIGHT;
    Attributes.LongerSide = LCD_WIDTH;
    Attributes.ShorterSide = LCD_HEIGHT;
    Attributes.Orientation = DisplayOrientation_Landscape;
    Attributes.PowerSave = PowerSaveState::NORMAL;
    Attributes.BitsPerPixel = 16;
    g_DisplayInterface.GetTransferBuffer(Attributes.TransferBuffer, Attributes.TransferBufferSize);
    return;
}
bool DisplayDriver::ChangeOrientation(DisplayOrientation orientation)
{
    switch (orientation)
    {
        case DisplayOrientation_Portrait:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            break;
        case DisplayOrientation_Portrait180:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            break;
        case DisplayOrientation_Landscape:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            g_DisplayInterface.SendCommand(1, SSD1306_CMD::Set_Segment_remap127); // Flip the display horizontally
            g_DisplayInterface.SendCommand(1, SSD1306_CMD::Set_COM_Scan_x);       // Flip the display vertically
            break;
        case DisplayOrientation_Landscape180:
            Attributes.Height = Attributes.ShorterSide;
            Attributes.Width = Attributes.LongerSide;
            break;
            break;
    }
    return true;
}
void DisplayDriver::SetDefaultOrientation()
{
    ChangeOrientation(DisplayOrientation::DisplayOrientation_Landscape);
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
        case PowerSaveState::NORMAL:
            break;
        case PowerSaveState::SLEEP:
            break;
        default:
            break;
    }
    return;
}
void DisplayDriver::Clear()
{
    SetWindow(0, 0, Attributes.Width - 1, Attributes.Height - 1);
    memset(Attributes.TransferBuffer, 0, Attributes.TransferBufferSize);
    for (int row = 0; row < Attributes.Height / 8; row++)
    {
        g_DisplayInterface.WriteToFrameBuffer(Memory_Write, Attributes.TransferBuffer, Attributes.Width);
    }
}
void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
    return;
}
bool DisplayDriver::SetWindow(CLR_INT16 x1, CLR_INT16 y1, CLR_INT16 x2, CLR_INT16 y2)
{
    g_DisplayInterface.SendCommand(3, Set_Column_Address, x1, x2);
    g_DisplayInterface.SendCommand(3, Set_Page_address, (y1 / 8), (y2 / 8));
    return true;
}
void DisplayDriver::BitBlt(
    int srcX,
    int srcY,
    int width,
    int height,
    int stride,
    int screenX,
    int screenY,
    CLR_UINT32 data[])
{

    // 16 bit colour  RRRRRGGGGGGBBBBB mode 565

    ASSERT((x >= 0) && ((x + width) <= g_DisplayDriver.Attributes.Width));
    ASSERT((y >= 0) && ((y + height) <= g_DisplayDriver.Attributes.Height));

    CLR_UINT16 *pui16Data = (CLR_UINT16 *)data;

    // Round y position down as we can only deal with rows of 8 at a time
    int offset = srcY % 8;
    srcY -= offset;

    // Increase height for change in y
    height += offset;

    // Round up height to multiple of 8
    height = (height + 7) & 0xfffffff8;

    // Find position in buffer for start of data in window
    pui16Data += srcX + (srcY * stride);

    // Set window for bitblt
    SetWindow(screenX, screenY, screenX + width - 1, screenY + height - 1);

    CLR_INT16 firstPageToUpdate = srcY / 8;
    CLR_INT16 lastPageToUpdate = (srcY + height - 1) / 8;

    CLR_UINT8 *pui8Buf = Attributes.TransferBuffer;
    CLR_UINT32 numberOfBytesPerTransfer = width;

    for (int page = firstPageToUpdate; page <= lastPageToUpdate; page++)
    {
        // Convert internal bitmap to dispay Page of 8 lines
        memset(pui8Buf, 0, 128);
        uint8_t mask = 0x01;

        // For each line in a page
        for (uint iy = 0; iy < 8; iy++)
        {
            CLR_UINT16 *pData = pui16Data;

            // for each pixel of width
            for (int i = 0; i < width; i++)
            {
                if (*pData++)
                {
                    pui8Buf[i] = pui8Buf[i] | mask;
                }
            }
            mask <<= 1;
            pui16Data += stride; // Next row
        }
        g_DisplayInterface.WriteToFrameBuffer(Memory_Write, Attributes.TransferBuffer, numberOfBytesPerTransfer);
    }
    return;
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
