//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "TouchInterface.h"
#include "DeviceRegistration.h"
#include "DeviceIO.h"

TouchInterface gTouchInterface;

static CLR_UINT8 I2C_READ_BUFFER[32];

bool TouchInterface::Initialize()
{
    PinNameValue i2c_sda = (PinNameValue)TOUCH_INTERFACE_I2C_SDA;
    PinNameValue i2c_scl = (PinNameValue)TOUCH_INTERFACE_I2C_SCL;

    // Reserve the pins
    DeviceRegistration::ReservePin(i2c_sda, DeviceRegistration::DevicePinFunction::I2C, TOUCH_INTERFACE_BUS);
    DeviceRegistration::ReservePin(i2c_scl, DeviceRegistration::DevicePinFunction::I2C, TOUCH_INTERFACE_BUS);

    // Setup the Pins
    GpioIO::SetFunction(DeviceRegistration::GetPin(i2c_sda));
    GpioIO::SetFunction(DeviceRegistration::GetPin(i2c_scl));

    I2cIO::Initialize(TOUCH_INTERFACE_BUS, I2cBusSpeed::I2cBusSpeed_StandardMode);
    return true;
}

static CLR_UINT8 readbuffer[12];
CLR_UINT8 *TouchInterface::Write_Read(CLR_UINT8 *writeBuffer, CLR_UINT16 writeSize, CLR_UINT16 readSize)
{
    I2cTransferStatus writeStatus = I2cIO::Write(TOUCH_INTERFACE_BUS, TOUCH_INTERFACE_SLAVE_ADDRESS, writeBuffer, writeSize);
    I2cTransferStatus readStatus =
        I2cIO::Read(TOUCH_INTERFACE_BUS, TOUCH_INTERFACE_SLAVE_ADDRESS, I2C_READ_BUFFER, readSize);

    return I2C_READ_BUFFER;
}
