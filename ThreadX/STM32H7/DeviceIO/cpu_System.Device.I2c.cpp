//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nanoCLR_Interop.h"
#include "nanoCLR_Types.h"
#include "sys_dev_i2c_native.h"
#include "low_level_i2c.h"

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

// NOTE:
// STM32H735G-DK development kit can only have I2C4 as the I2C device.
// The Pins of I2C1, I2C2, I2C3, I2C5 are used and defined for other features on
// the development board
//

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;
    // Already done
    return S_OK;
}
HRESULT
Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;
    // Nothing to do
    return S_OK;
}
HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::
    NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    (void)stack;
    // Nothing to do
    return S_OK;
}
