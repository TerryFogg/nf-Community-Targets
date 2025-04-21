//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "PinNames.h"
#include "DeviceIO.h"
#include "Device.h"
#include "board.h"
#include "Delays.h"

#ifdef FileX
#include "File_Drivers.h"
#endif
#include <hardware/structs/xip.h>
static DeviceGpioPin mcuPins[28];
static ADC_Properties mcuADC[] = {{false, 0, GP26}, {false, 1, GP27}, {false, 2, GP28}};
static I2C_Properties mcuI2C[] = {{false, i2c0, GP8, GP9}, {false, i2c1, GP14, GP15}};
static PWM_Properties mcuPWM[] =
    {{false, 0, GP0, GP1}, {false, 1, GP2, GP3}, {false, 2, GP4, GP5}, {false, 3, GP6, GP7}, {false, 4, GP8, GP9}};
static SPI_Properties mcuSPI[] = {{false, spi0, GP19, GP16, GP18, GP17}, {false, spi1, GP10, GP7}};
static USART_Properties mcuUSART[] = {{false, uart0, GP0, GP1}, {false, uart1, GP6, GP7}};

void SetupPinList()
{
    mcuPins[0] = {GP1, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[1] = {GP2, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[2] = {GP3, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[3] = {GP4, false, NULL, PinMode::PinMode_Input, DevicePinFunction::I2C0_SDA};
    mcuPins[4] = {GP5, false, NULL, PinMode::PinMode_Input, DevicePinFunction::I2C0_SCL};
    mcuPins[5] = {GP6, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[6] = {GP7, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[7] = {GP8, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[8] = {GP9, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[9] = {GP10, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[10] = {GP11, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[11] = {GP12, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[12] = {GP13, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[13] = {GP14, false, NULL, PinMode::PinMode_Input, DevicePinFunction::I2C1_SDA};
    mcuPins[14] = {GP15, false, NULL, PinMode::PinMode_Input, DevicePinFunction::I2C1_SCL};
    mcuPins[15] = {GP16, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[16] = {GP17, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[17] = {GP18, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[18] = {GP19, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[19] = {GP20, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[20] = {GP21, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[21] = {GP22, false, NULL, PinMode::PinMode_Input, DevicePinFunction::LOW_POWER};
    mcuPins[22] = {GP23, false, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[23] = {GP24, false, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[24] = {GP25, false, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};
    mcuPins[25] = {GP26, false, NULL, PinMode::PinMode_Input, DevicePinFunction::ADC_0};
    mcuPins[26] = {GP27, false, NULL, PinMode::PinMode_Input, DevicePinFunction::ADC_1};
    mcuPins[27] = {GP28, false, NULL, PinMode::PinMode_Input, DevicePinFunction::ADC_2};
    mcuPins[28] = {GP29, false, NULL, PinMode::PinMode_Input, DevicePinFunction::WIFI_INTERFACE};

    Device::CreatePinList(&mcuPins[0], ARRAY_LEN(mcuPins));

    AdcIO::SetupAdcList(mcuADC);
    I2cIO::SetupI2CList(mcuI2C);
    PwmIO::SetupPwmList(mcuPWM);
    SpiIO::SetupSpiList(mcuSPI);
    SerialIO::SetupUsartList(mcuUSART);
};
void Initialize_Board()
{
    SetupPinList();
    Initialize64BitMicrosecondTimer();
#ifdef PSRAM
    InitializePSRAM();
#endif
}
void Initialize64BitMicrosecondTimer()
{
    // hardware_timer_init(timer_hw, 1, 1);
    // hardware_timer_set_hw_clock_ticks_per_us(timer_hw, 1);
    // hardware_timer_enable(timer_hw);
    return;
}
uint64_t ReadMicrosecondCounter()
{
    return time_us_64();
}
void InitializePSRAM()
{
    gpio_set_function(47, GPIO_FUNC_XIP_CS1); // Set GPIO47 as CS for PSRAM
    xip_ctrl_hw->ctrl |= XIP_CTRL_WRITABLE_M1_BITS;

    // Enable writable memory[_{{{CITATION{{{_1{RP2350: How to enable XIP PSRAM? - // Raspberry Pi
    // Forums](https://forums.raspberrypi.com/viewtopic.php?t=375109)
}
