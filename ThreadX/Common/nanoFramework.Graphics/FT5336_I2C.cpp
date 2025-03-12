// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.

// For a  FT63x6 by Focal Tech.
// It is important to note that the SDA and SCL must connect with a pull-high
// resistor respectively before you read/write I2C data
// Only supporting 1 point touch although the touch controller indicates support
// for 2 point touch

#include "TouchDevice.h"
#include "TouchInterface.h"

struct TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;
extern TouchPanelDriver g_TouchPanelDriver;
//static GPIO_PIN InterruptPin;

// NOTE: Portrait orientation of x,y notation
static int touchPanelWidthInPixels;
static int touchPanelHeightInPixels;
static TranslateCoordinates touchPanelTranslateType = TranslateCoordinates::NONE;
//
// Panel orientation is portrait for x and y notation
// -------  ... x
// |     |
// |     |
// |     |
// |     |
// -------
//        .
//        .
//        y

#define PRESS_DOWN 0
#define LIFT_UP    1

enum FT5336_CMD : CLR_UINT8
{
    DEV_MODE = 0x00,
    GEST_ID = 0x01,
    TD_STATUS = 0x02,
    P1_XH = 0x03,
    P1_XL = 0x04,
    P1_YH = 0x05,
    P1_YL = 0x06,
    P1_WEIGHT = 0x07,
    P1_MISC = 0x08,
    P2_XH = 0x09,
    P2_XL = 0x0A,
    P2_YH = 0x0B,
    P2_YL = 0x0C,
    P2_WEIGHT = 0x0D,
    P2_MISC = 0x0E,
    TH_GROUP = 0x80,
    TH_DIFF = 0x85,
    CTRL = 0x86,
    TIMEENTERMONITOR = 0x87,
    PERIODACTIVE = 0x88,
    PERIODMONITOR = 0x89,
    RADIAN_VALUE = 0x91,
    OFFSET_LEFT_RIGHT = 0x92,
    OFFSET_UP_DOWN = 0x93,
    DISTANCE_LEFT_RIGHT = 0x94,
    DISTANCE_UP_DOWN = 0x95,
    DISTANCE_ZOOM = 0x96,
    LIB_VER_H = 0xA1,
    LIB_VER_L = 0xA2,
    CIPHER = 0xA3,
    G_MODE = 0xA4,
    PWR_MODE = 0xA5,
    FIRMID = 0xA6,
    FOCALTECH_ID = 0xA8,
    RELEASE_CODE_ID = 0xAF,
    STATE = 0xBC,
};
enum FT5336_VALUES : CLR_UINT8
{
    ID_VALUE = 0x51,
    G_MODE_INTERRUPT_TRIGGER = 0x01,
    G_MODE_INTERRUPT_POLLING = 0x00,
    G_MODE_INTERRUPT_MASK = 0x03,
    G_MODE_INTERRUPT_SHIFT = 0x00,
    MSB_MASK = 0x0F,
    LSB_MASK = 0xFF
};
bool TouchDevice::Initialize(int width, int height, TranslateCoordinates translate_type)
{
    touchPanelWidthInPixels = width;
    touchPanelHeightInPixels = height;
    touchPanelTranslateType = translate_type;

    CLR_UINT8 id;
    // Check device type correct
    g_TouchInterface.Write_Register_Read_Data(FT5336_CMD::FOCALTECH_ID, 1, &id, 1);
    bool result = (id == ID_VALUE);
    if (result)
    {
        // Configured to interrupt on touch down and up but not each controller sampling period 
        g_TouchInterface.WriteCommand(FT5336_CMD::G_MODE, FT5336_VALUES::G_MODE_INTERRUPT_POLLING);
    }
    return result;
}

bool TouchDevice::Enable()
{
  // The FT5336 doesn't appear to have a enable/disable command ( at least not
  // what can be seen in the poor documentation)
  // Enable interrupt detection on the GPIO pin
    g_TouchInterface.Enable_TouchInterrupt();
    return TRUE;
}
bool TouchDevice::Disable()
{
  // The FT5336 doesn't appear to have a enable/disable command ( at least not
  // what can be seen in the poor documentation)
  // Disable interrupt detection on the GPIO pin
    g_TouchInterface.Disable_TouchInterrupt();
    return true;
}
bool TouchDevice::GetPoint(CLR_INT16 *x1, CLR_INT16 *y1)
{
    // The FT6x06 touch screen has its own processing so read on sample
    CLR_UINT8 touchRegister = FT5336_CMD::GEST_ID;
    CLR_UINT8 touchValue[7] = {0, 0, 0, 0, 0, 0, 0};
    CLR_UINT16 numberValuesExpected = 12;
    g_TouchInterface.Write_Register_Read_Data(touchRegister, 1, touchValue, numberValuesExpected);

    // The detected number of touch points 0,1,2 are valid for this controller

    CLR_INT32 numberOfTouchPoints = touchValue[1] & 0x0F;

    CLR_INT16 touchx1 = ((touchValue[2] & 0x0F) << 8) | touchValue[3];
    CLR_INT16 touchy1 = ((touchValue[4] & 0x0F) << 8) | (touchValue[5]);

    if (touchPanelTranslateType == TranslateCoordinates::NONE)
    {
        *x1 = touchx1;
        *y1 = touchy1;
    }
    else if (touchPanelTranslateType == TranslateCoordinates::SWAP_X_Y)
    {
        *x1 = touchy1;
        *y1 = touchx1;
    }
    // Upper 2 bits of high X1 coord is (touch Up/Down)
    //int touch1Event = (touchValue[2] & 0xC0) >> 6;

    // return (touch1Event == PRESS_DOWN);
    return (numberOfTouchPoints != 0);
}
