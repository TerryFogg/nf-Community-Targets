﻿//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "PinNames.h"
#include "DeviceIO.h"
#include "DeviceRegistration.h"
#include "board.h"
#include "Delays.h"

#ifdef FileX
#include "File_Drivers.h"
#endif

DeviceRegistration::DevicePin mcuPins[] = {
    {GP0, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP1, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP2, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP3, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP4, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP5, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP6, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP7, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP8, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP9, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP10, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP11, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP12, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP13, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP14, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP15, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP16, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP17, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP18, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP19, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP20, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP21, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP22, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP23, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP24, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP25, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP26, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP27, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP28, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
    {GP29, false, NULL, NULL, PinMode::PinMode_Input, DeviceRegistration::DevicePinFunction::NONE, 0},
};

// 1 Analog Digital controller
// 4 Channels
// Channel 0 --> GPIO26
// Channel 1 --> GPIO27
// Channel 2 --> GPIO28
// Channel 3 --> GPIO29
// Channel 4 --> Temperature Sensor

static int AdcControllerReference = 1;
DeviceRegistration::AdcPin BoardADCChannels[] = {
    {&AdcControllerReference, 1, 1},
    {&AdcControllerReference, 1, 2},
    {&AdcControllerReference, 1, 3},
    {&AdcControllerReference, 1, 4}};
DeviceRegistration::I2cPin BoardI2CChannels[] = {{i2c0, 1, 1}, {i2c1, 2, 1}};

// PWM controllers are slice id's
static int PWMSlice1 = 1;
static int PWMSlice2 = 2;
static int PWMSlice3 = 3;
static int PWMSlice4 = 4;
static int PWMSlice5 = 5;
static int PWMSlice6 = 6;
static int PWMSlice7 = 7;
static int PWMSlice8 = 8;
void *pSlice1 = (void *)PWMSlice1;
void *pSlice2 = (void *)PWMSlice2;
void *pSlice3 = (void *)PWMSlice3;
void *pSlice4 = (void *)PWMSlice4;
void *pSlice5 = (void *)PWMSlice5;
void *pSlice6 = (void *)PWMSlice6;
void *pSlice7 = (void *)PWMSlice7;
void *pSlice8 = (void *)PWMSlice8;

DeviceRegistration::PwmPin BoardPWMChannels[] = {
    {pSlice1, 1, 1},
    {pSlice1, 1, 2},
    {pSlice2, 1, 1},
    {pSlice2, 1, 2},
    {pSlice3, 1, 1},
    {pSlice3, 1, 2},
    {pSlice4, 1, 1},
    {pSlice4, 1, 2},
    {pSlice5, 1, 1},
    {pSlice5, 1, 2},
    {pSlice6, 1, 1},
    {pSlice6, 1, 2},
    {pSlice7, 1, 1},
    {pSlice7, 1, 2},
    {pSlice8, 1, 1},
    {pSlice8, 1, 2},
};
DeviceRegistration::SpiPin BoardSPIChannels[] = {{spi0, 1, 1}, {spi1, 1, 1}};
DeviceRegistration::SerialPin BoardSerialChannels[] = {{uart0, 1, 1}, {uart1, 1, 1}};

void Initialize_Board()
{
    Initialize_Board_LEDS();
    Initialize64BitMicrosecondTimer();
    InitializeDevicePins();
}
void ResetPinList()
{
    for (int i = 0; i < sizeof(mcuPins) / sizeof(DeviceRegistration::DevicePin); i++)
    {
        mcuPins[i].Reserved = false;
        mcuPins[i].next = NULL;
        mcuPins[i].GpioParameterData = NULL;
        mcuPins[i].Mode = PinMode::PinMode_Input;
        mcuPins[i].CurrentFunction = DeviceRegistration::DevicePinFunction::NONE;
        mcuPins[i].DeviceFunctionChannelNumber = 0;
    }
}
void Initialize_Board_LEDS()
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}
void Initialize64BitMicrosecondTimer()
{
    // hardware_timer_init(timer_hw, 1, 1);
    // hardware_timer_set_hw_clock_ticks_per_us(timer_hw, 1);
    // hardware_timer_enable(timer_hw);
    return;
}
void BoardLed_ON()
{
    gpio_put(LED_PIN, LED_STATE_ON);
}
void BoardLed_OFF()
{
    gpio_put(LED_PIN, LED_STATE_OFF);
}
void BoardLed_Toggle()
{
    gpio_put(LED_PIN, !gpio_get(25));
}
bool BoardUserButton_Pressed()
{
    // TODO: Define one of the pins as a input for and add-on user button
    return false;
}
uint64_t ReadMicrosecondCounter()
{
    return time_us_64();
}

void InitializeDevicePins()
{
    DeviceRegistration::CreatePinList(&mcuPins[0], ARRAY_LEN(mcuPins));
    DeviceRegistration::CreateADCChannelList(BoardADCChannels, ARRAY_LEN(BoardADCChannels));
    DeviceRegistration::CreateI2CChannelList(BoardI2CChannels, ARRAY_LEN(BoardI2CChannels));
    DeviceRegistration::CreatePWMChannelList(BoardPWMChannels, ARRAY_LEN(BoardPWMChannels));
    DeviceRegistration::CreateSPIChannelList(BoardSPIChannels, ARRAY_LEN(BoardSPIChannels));
    DeviceRegistration::CreateSerialChannelList(BoardSerialChannels, ARRAY_LEN(BoardSerialChannels));
};
