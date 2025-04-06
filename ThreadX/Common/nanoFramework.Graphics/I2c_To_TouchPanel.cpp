//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "TouchInterface.h"
#include "Device.h"
#include "DeviceIO.h"

TouchInterface g_TouchInterface;

static CLR_UINT8 I2C_READ_BUFFER[32];
static int m_TouchI2cBus;
static int m_TouchI2cSlaveAddress;

bool TouchInterface::Initialize(int TouchI2cBus, int TouchI2cSlaveAddress)
{
    m_TouchI2cBus = TouchI2cBus;
    m_TouchI2cSlaveAddress = TouchI2cSlaveAddress;
    I2cIO::Initialize(TouchI2cBus, I2cBusSpeed_StandardMode, I2C_CONTROL_TYPE::MASTER, TouchI2cSlaveAddress);
    return true;
}
CLR_UINT8 *TouchInterface::Write_Read(CLR_UINT8 *writeBuffer, CLR_UINT16 writeSize, CLR_UINT16 readSize)
{
    I2cTransferStatus writeStatus = I2cTransferStatus_UnknownError;
    I2cTransferStatus readStatus = I2cTransferStatus_UnknownError;

    writeStatus = I2cIO::Write(m_TouchI2cBus, m_TouchI2cSlaveAddress, writeBuffer, writeSize, I2C_CONTROL_TYPE::MASTER);
    if (writeStatus != I2cTransferStatus_FullTransfer)
    {
        return NULL;
    }
    if (readSize > 0)
    {
        I2cIO::Read(m_TouchI2cBus, m_TouchI2cSlaveAddress, I2C_READ_BUFFER, readSize);
    }
    if (readStatus != I2cTransferStatus_FullTransfer)
    {
        return I2C_READ_BUFFER;
    }
    else
    {
        return NULL;
    }
}
