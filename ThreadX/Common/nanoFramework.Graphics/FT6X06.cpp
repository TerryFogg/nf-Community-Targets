// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.

// For a  FT63x6 by Focal Tech.
// It is important to note that the SDA and SCL must connect with a pull-high
// resistor respectively before you read/write I2C data
// Only supporting 1 point touch although the touch controller indicates support
// for 2 point touch

#include "TouchDevice.h"
#include "TouchInterface.h"
#include <Device.h>
#include <DeviceIO.h>

struct TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

enum FT6X06_CMD : CLR_UINT8
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
enum FT6X06_VALUES : CLR_UINT8
{
    ID_VALUE = 0x11,
    G_MODE_INTERRUPT_TRIGGER = 0x01,
    G_MODE_INTERRUPT_POLLING = 0x00,
    G_MODE_INTERRUPT_MASK = 0x03,
    G_MODE_INTERRUPT_SHIFT = 0x00,
    MSB_MASK = 0x0F,
    LSB_MASK = 0xFF
};

static GPIO_INTERRUPT_SERVICE_ROUTINE touchInterrupt = NULL;
void gpio_callback(uint gpio, uint32_t events)
{
    // The interrupt edge (from pinState) can be used for the Touch Down(falling edge) and Touch Up(Rising edge)
    bool pinState = (events == 4) ? true : false;
    if (touchInterrupt != NULL)
    {
        touchInterrupt(gpio, pinState, NULL);
    }
}
bool TouchDevice::Initialize()
{
    // Time of starting to report point after resetting minimum time
    //  300 milliseconds
    // NOTE: Can run this initialize later 
    PLATFORM_DELAY(310);

    // Check device type correct
    CLR_UINT8 registerCommand = FT6X06_CMD::FOCALTECH_ID;
    CLR_UINT8 *id = g_TouchInterface.Write_Read(&registerCommand, 1, 1);

    // Set mode to either Interrupt Polling mode or Interrupt Trigger mode
    registerCommand = FT6X06_CMD::G_MODE;
    bool result = (*id == ID_VALUE);
    if (result)
    {
        uint8_t set_interrupt_mode[2]{FT6X06_CMD::G_MODE, FT6X06_VALUES::G_MODE_INTERRUPT_POLLING};
        if (result)
        {
            // Configured to interrupt on touch down and up but not each controller sampling period
            g_TouchInterface.Write_Read(set_interrupt_mode, 2, 0);
        }
    }
    Device::ReservePin((PinNameValue)TOUCH_INTERFACE_INTERRUPT, Device::DevicePinFunction::GPIO);
    GpioIO::InitializePin((PinNameValue)TOUCH_INTERFACE_INTERRUPT);
    Device::SetPinMode((PinNameValue)TOUCH_INTERFACE_INTERRUPT, PinMode_Input);
    GpioIO::SetMode((PinNameValue)TOUCH_INTERFACE_INTERRUPT, PinMode_Input);

    Device::GpioParameter *newGpioParameter = (Device::GpioParameter *)platform_malloc(sizeof(Device::GpioParameter));
    memset(newGpioParameter, 0, sizeof(Device::GpioParameter));
    Device::AddPinParameters((PinNameValue)TOUCH_INTERFACE_INTERRUPT, newGpioParameter);
    newGpioParameter->callBack = true;
    newGpioParameter->edgeTrigger = GPIO_INT_EDGE_HIGH;

    return result;
}
bool TouchDevice::Enable(GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc)
{
    touchInterrupt = touchIsrProc;
    GpioIO::InterruptEnable((PinNameValue)TOUCH_INTERFACE_INTERRUPT, GPIO_INT_EDGE_BOTH, (void *)gpio_callback);
    return TRUE;
}
bool TouchDevice::Disable()
{
    GpioIO::InterruptDisable((PinNameValue)TOUCH_INTERFACE_INTERRUPT);
    return true;
}
TouchPointDevice TouchDevice::GetPoint()
{
    // The FT6x06 touch controller does its own processing, averaging etc
    CLR_UINT8 registerCommand = FT6X06_CMD::DEV_MODE;
    CLR_UINT8 *touchValues = g_TouchInterface.Write_Read(&registerCommand, 1, 7);

    CLR_INT16 touchx1 = ((touchValues[3] & 0x0F) << 8) | touchValues[4];
    CLR_INT16 touchy1 = ((touchValues[5] & 0x0F) << 8) | (touchValues[6]);

    TouchPointDevice tp;
#if (TOUCH_INVERT_X == true)
    tp.x = TOUCH_INTERFACE_WIDTH - touchx1;
#else
    tp.x = touchx1;
#endif
#if (TOUCH_INVERT_Y == true)
    tp.y = TOUCH_INTERFACE_HEIGHT - touchy1;
#else
    tp.y = touchy1;
#endif
    return tp;
}
