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
int lcd_width;
int lcd_height;

bool DisplayDriver::Initialize()
{
    lcd_width = 480;
    lcd_height = 272;

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
