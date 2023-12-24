//#include <src/common/pico_time/include/pico/time.h>

// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.

#include "Display.h"
#include "DisplayInterface.h"
#include "Graphics.h"
//#include "InternalFont.h"

//
// ST7789 is a 262,144-color single-chip SOC driver for a-TFT liquid crystal
// display with resolution of up to 240RGBx320 dots, comprising a 720-channel source
// driver, a 320-channel gate driver, 172,800 bytes GRAM for graphic display data
// of 240RGBx320 dots, and power supply circuit.
//
//
// NOTE: This code is a display for 240x132 which does not match the controller
// 240x320 For PORTRAIT and LANDSCAPE180 there are no problems because the
// remaining (320-240), 80 pixels are not displayed For Portrait180 and LANDSCAPE
// the (320-240), 80 Pixels are at the start and are visible. This causes an 80
// pixel offset that must be programmed for.
//

// 
// Using the default endian order for transferring bytes
// Normal (MSB first, default)
//

enum ST7789_CMD : CLR_UINT8
{
    NOP = 0x00,
    SOFTWARE_RESET = 0x01,
    Sleep_IN = 0x10,
    Sleep_OUT = 0x11,
    ST7789_PTLON = 0x12,
    Normal_Display_On = 0x13,
    Display_Inversion_Off = 0x20,
    Display_Inversion_On = 0x21,
    Display_OFF = 0x28,
    Display_ON = 0x29,
    Column_Address_Set = 0x2A,
    Row_Address_Set = 0x2B,
    Memory_Write = 0x2C,
    Memory_Read = 0x2E,
    Partial_Area = 0x30,
    Vertical_Scroll = 0x33,
    Memory_Access_Control = 0x36,
    Pixel_Format_Set = 0x3A,
    Memory_Write_Continue = 0x3C,
    Write_Display_Brightness = 0x51,
    Porch_Setting = 0xB2,
    Gate_Control = 0xB7,
    VCOMS_Setting = 0xBB,
    LCM_Control = 0xC0,
    VDV_VRH_Command_Enable = 0xC2,
    VRH_Set = 0xC3,
    VDV_Set = 0xC4,
    Frame_Rate_Control = 0xC6,
    Power_Control_1 = 0xD0,
    Positive_Voltage_Gamma = 0xE0,
    Negative_Voltage_Gamma = 0xE1,
};
enum ST7789_Orientation : CLR_UINT8
{
    MADCTL_MH = 0x04,  // sets the Horizontal Refresh, 0=Left-Right and 1=Right-Left
    MADCTL_BGR = 0x08, // Blue-Green-Red pixel order
    MADCTL_ML = 0x10,  // sets the Vertical Refresh, 0=Top-Bottom and 1=Bottom-Top
    MADCTL_MV = 0x20,  // sets the Row/Column Swap, 0=Normal and 1=Swapped
    MADCTL_MX = 0x40,  // sets the Column Order, 0=Left-Right and 1=Right-Left
    MADCTL_MY = 0x80   // sets the Row Order, 0=Top-Bottom and 1=Bottom-Top

};
extern DisplayInterface g_DisplayInterface;
struct DisplayDriver g_DisplayDriver;
int lcd_width;
int lcd_height;
int xx;
int y;
int stride;
int screenX;
int screenY;
CLR_UINT32 *ddata;


bool DisplayDriver::Initialize() {

  lcd_width = 240;
  lcd_height = 135;

  SetupDisplayAttributes();

  g_DisplayInterface.SendCommand(1, SOFTWARE_RESET);
  OS_DELAY(10);

  g_DisplayInterface.SendCommand(2, Pixel_Format_Set, 0x55);
  g_DisplayInterface.SendCommand(6, Porch_Setting, 0x0c, 0x0c, 0x00, 0x33, 0x33);

  // Internal LCD Voltage generator settings
  g_DisplayInterface.SendCommand(2, Gate_Control, 0x35);
  g_DisplayInterface.SendCommand(2, VCOMS_Setting, 0x19);
  g_DisplayInterface.SendCommand(2, LCM_Control, 0x2C);
  g_DisplayInterface.SendCommand(2, VDV_VRH_Command_Enable, 0x01);
  g_DisplayInterface.SendCommand(2, VRH_Set, 0x12);
  g_DisplayInterface.SendCommand(2, VDV_Set, 0x20);

  g_DisplayInterface.SendCommand(2, Frame_Rate_Control, 0x0F);
  g_DisplayInterface.SendCommand(3, Power_Control_1, 0xA4, 0xA1);
  g_DisplayInterface.SendCommand(15, Positive_Voltage_Gamma, 0xD0, 0x04, 0x0D,0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D,0x0B, 0x1F, 0x23);
  g_DisplayInterface.SendCommand(15, Negative_Voltage_Gamma, 0xD0, 0x04, 0x0C,0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F,0x1F, 0x20, 0x23);

  SetDefaultOrientation();
  Clear();

  g_DisplayInterface.SendCommand(1, Display_Inversion_On);
  g_DisplayInterface.SendCommand(1, Sleep_OUT);

  g_DisplayInterface.SendCommand(1, Display_ON);
  //    OS_DELAY(50);
  //    g_DisplayInterface.SendCommand(1, NOP); // End of sequence
  //    OS_DELAY(20);

  g_DisplayInterface.DisplayBacklight(true);

  return true;
}
void DisplayDriver::SetupDisplayAttributes()
{
    // Define the LCD/TFT resolution
    Attributes.LongerSide = lcd_width;
    Attributes.ShorterSide = lcd_height;
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
      g_DisplayInterface.SendCommand(2, Memory_Access_Control, MADCTL_BGR);
      break;
    case DisplayOrientation_Portrait180:
      Attributes.Height = Attributes.ShorterSide;
      Attributes.Width = Attributes.LongerSide;
      g_DisplayInterface.SendCommand(2, Memory_Access_Control,(MADCTL_MY | MADCTL_MX | MADCTL_BGR));
      break;
    case DisplayOrientation_Landscape:
        Attributes.Height = Attributes.ShorterSide;
        Attributes.Width = Attributes.LongerSide;
        g_DisplayInterface.SendCommand(2, Memory_Access_Control,(MADCTL_MX | MADCTL_MV | MADCTL_ML));
        break;
    case DisplayOrientation_Landscape180:
      Attributes.Height = Attributes.ShorterSide;
      Attributes.Width = Attributes.LongerSide;
      g_DisplayInterface.SendCommand(2, Memory_Access_Control, (MADCTL_MY | MADCTL_BGR));
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
    return true;
}
void DisplayDriver::PowerSave(PowerSaveState powerState)
{
    switch (powerState)
    {
    default:
        // illegal fall through to Power on
    case PowerSaveState::NORMAL:
        g_DisplayInterface.SendCommand(3, Sleep_IN, 0x00, 0x00); // leave sleep mode
        break;
    case PowerSaveState::SLEEP:
        g_DisplayInterface.SendCommand(3, Sleep_IN, 0x00, 0x01); // enter sleep mode
        break;
    }
    return;
}
void DisplayDriver::Clear()
{
    // Clear the ST7789 controller frame
    SetWindow(0, 0, Attributes.Width - 1, Attributes.Height - 1);

    // Clear the transfer buffer
    memset(Attributes.TransferBuffer, 0, Attributes.TransferBufferSize);

    int totalBytesToClear = Attributes.Width * Attributes.Height * 2;
    int fullTransferBuffersCount = totalBytesToClear / Attributes.TransferBufferSize;
    int remainderTransferBuffer = totalBytesToClear % Attributes.TransferBufferSize;

    CLR_UINT8 command = Memory_Write;
    for (int i = 0; i < fullTransferBuffersCount; i++)
    {
        g_DisplayInterface.WriteToFrameBuffer(command, Attributes.TransferBuffer, Attributes.TransferBufferSize);
        command = Memory_Write_Continue;
    }

    if (remainderTransferBuffer > 0)
    {
      PLATFORM_DELAY(10);
      g_DisplayInterface.WriteToFrameBuffer(command, Attributes.TransferBuffer, remainderTransferBuffer);
    }
}
void DisplayDriver::DisplayBrightness(CLR_INT16 brightness)
{
    _ASSERTE(brightness >= 0 && brightness <= 100);
    g_DisplayInterface.SendCommand(2, Write_Display_Brightness, (CLR_UINT8)brightness);
    return;
}
bool DisplayDriver::SetWindow(CLR_INT16 x1, CLR_INT16 y1, CLR_INT16 x2, CLR_INT16 y2)
{

    x1 = x1 + 40;
    x2 = x2 + 40;
    y1 = y1 + 53;
    y2 = y2 + 53;

    CLR_UINT8 tx0 = (x1 >> 8) & 0xFF;
    CLR_UINT8 tx1 = x1 & 0xFF;
    CLR_UINT8 tx2 = (x2 >> 8) & 0xFF;
    CLR_UINT8 tx3 = x2 & 0xFF;

    CLR_UINT8 ty0 = (y1 >> 8) & 0xFF;
    CLR_UINT8 ty1 = y1 & 0xFF;
    CLR_UINT8 ty2 = (y2 >> 8) & 0xFF;
    CLR_UINT8 ty3 = y2 & 0xFF;

    CLR_UINT8 Column_Address_Set_Data[4];
    CLR_UINT8 Row_Address_Set_Data[4];

    Column_Address_Set_Data[0] = tx0; //  0;                  // (x1 >> 8) & 0xFF;
    Column_Address_Set_Data[1] = tx1; //  0x28; (40)              // x1 & 0xFF;
    Column_Address_Set_Data[2] = tx2; //  0x01;               // (x2 >> 8) & 0xFF;
    Column_Address_Set_Data[3] = tx3; //  0x17;               // x2 & 0xFF;
    Row_Address_Set_Data[0] = ty0;    //  0x00;                               // (y1 >> 8) & 0xFF;
    Row_Address_Set_Data[1] = ty1;    //  0x35;                               // y1 & 0xFF;
    Row_Address_Set_Data[2] = ty2;    //  0x00;                                               // (y2 >> 8) & 0xFF;
    Row_Address_Set_Data[3] = ty3;    //  0xbb; // y2 & 0xFF;

    g_DisplayInterface.SendCommand(
        5,
        Column_Address_Set,
        Column_Address_Set_Data[0],
        Column_Address_Set_Data[1],
        Column_Address_Set_Data[2],
        Column_Address_Set_Data[3]);

    g_DisplayInterface.SendCommand(
        5,
        Row_Address_Set,
        Row_Address_Set_Data[0],
        Row_Address_Set_Data[1],
        Row_Address_Set_Data[2],
        Row_Address_Set_Data[3]);

    return true;
}


void DisplayDriver::BitBlt(int x, int y, int width, int height, int stride,
                           int screenX, int screenY, CLR_UINT32 data[]) {
  // 16 bit colour  RRRRRGGGGGGBBBBB mode 565

    ASSERT((x >= 0) && ((x + width) <= g_DisplayDriver.Attributes.Width));
    ASSERT((y >= 0) && ((y + height) <= g_DisplayDriver.Attributes.Height));

    g_DisplayDriver.SetWindow(x, y, (x + width - 1), (y + height - 1));

    CLR_UINT16 *StartOfLine_src = (CLR_UINT16 *)&data[0];

    // Position to offset in data[] for start of window
    CLR_UINT16 offset = (y * g_DisplayDriver.Attributes.Width) + x;
    StartOfLine_src += offset;

    CLR_UINT8 *transferBufferIndex = g_DisplayDriver.Attributes.TransferBuffer;
    CLR_UINT32 transferBufferCount = g_DisplayDriver.Attributes.TransferBufferSize;
    CLR_UINT8 command = Memory_Write;

    while (height--)
    {
        CLR_UINT16 *src;
        int xCount;

        src = StartOfLine_src;
        xCount = width;

        while (xCount--)
        {
            CLR_UINT16 data = *src++;
            // Swap bytes
            *transferBufferIndex++ = (data >> 8);
            *transferBufferIndex++ = data & 0xff;
            transferBufferCount -= 2;

            // Send over SPI if no room for another 2 bytes
            if (transferBufferCount < 1)
            {
                // Transfer buffer full, send it
                g_DisplayInterface.WriteToFrameBuffer(command, 
                                                      g_DisplayDriver.Attributes.TransferBuffer,
                                                     (g_DisplayDriver.Attributes.TransferBufferSize - transferBufferCount));

                // Reset transfer ptrs/count
                transferBufferIndex = g_DisplayDriver.Attributes.TransferBuffer;
                transferBufferCount = g_DisplayDriver.Attributes.TransferBufferSize;
                command = Memory_Write_Continue;
            }
        }

        // Next row in data[]
        StartOfLine_src += g_DisplayDriver.Attributes.Width;
    }

    // Send remaining data in transfer buffer to SPI
    if (transferBufferCount < g_DisplayDriver.Attributes.TransferBufferSize)
    {
        PLATFORM_DELAY(10);
        g_DisplayInterface.WriteToFrameBuffer( command, 
                                               g_DisplayDriver.Attributes.TransferBuffer,
                                             ( g_DisplayDriver.Attributes.TransferBufferSize - transferBufferCount));
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

