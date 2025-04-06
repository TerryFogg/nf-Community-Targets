//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_i2c_slave_native.h"

#include "DeviceIO.h"
#include "Device.h"
#include <corlib_native.h>

typedef Library_corlib_native_System_SpanByte SpanByte;

HRESULT Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_INT32 I2Cbus = (stack.This())[FIELD___busId].NumericByRef().s4;
        CLR_INT32 I2cAddress = stack.This()[FIELD___deviceAddress].NumericByRef().s4;
        // TODO - Check on speed selection for slave I2C ?
        I2cBusSpeed I2cSpeed = I2cBusSpeed::I2cBusSpeed_StandardMode;
        if (!I2cIO::Initialize(I2Cbus, I2cSpeed, I2C_CONTROL_TYPE::SLAVE, I2cAddress))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_INT32 deviceId = (stack.This())[FIELD___busId].NumericByRef().s4;
        I2cIO::Dispose(deviceId);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice::
    NativeTransmit___I4__SystemSpanByte__SystemSpanByte__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());

        uint8_t *readBuffer = NULL;
        uint8_t *writeBuffer = NULL;
        CLR_INT32 bufferOffset = 0;
        CLR_INT32 bufferSize = 0;
        CLR_INT32 readCount;
        bool IsRead = false;
        bool IsWrite = false;
        CLR_INT32 transferResult;

        CLR_RT_HeapBlock_Array *readWorkingData;
        CLR_RT_HeapBlock_Array *writeWorkingData;
        CLR_RT_HeapBlock *readSpanByte = stack.Arg1().Dereference();
        CLR_RT_HeapBlock *writeSpanByte = stack.Arg2().Dereference();
        int64_t timeoutMilliseconds = stack.Arg3().NumericByRef().s4;

        CLR_INT32 writeOffset = (writeSpanByte == NULL) ? -1 : writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
        CLR_INT32 writeSize = (writeSpanByte == NULL) ? -1 : writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

        CLR_INT32 readOffset = (readSpanByte == NULL) ? -1 : readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
        CLR_INT32 readSize = (readSpanByte == NULL) ? -1 : readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

        writeWorkingData = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
        readWorkingData = readSpanByte[SpanByte::FIELD___array].DereferenceArray();

        IsWrite = (writeOffset >= 0 && writeSize > 0);
        IsRead = (readOffset >= 0 && readSize > 0);
        uint8_t deviceId = (uint8_t)(stack.This())[FIELD___busId].NumericByRef().s4;
        timeoutMilliseconds = (timeoutMilliseconds == -1) ? I2cIO::TimeoutMaximum : I2cIO::Timeout;

        if (IsWrite)
        {
            // Take a copy of the data from the managed heap to local store ( future?, maybe execute this on an
            // async thread)
            if ((writeBuffer = (uint8_t *)platform_malloc(writeSize)) != NULL)
            {
                memset(writeBuffer, 0, bufferSize);

                CLR_INT32 slaveAddress = 0;
                I2cTransferStatus transferResult =
                    I2cIO::Write(deviceId, slaveAddress, writeBuffer, writeSize, I2C_CONTROL_TYPE::MASTER);
                platform_free(writeBuffer);
                if (transferResult == I2cTransferStatus::I2cTransferStatus_FullTransfer)
                {
                    memcpy(writeWorkingData->GetElement(bufferOffset), writeBuffer, bufferSize);
                    stack.SetInteger(readCount);
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                }
            }
            else
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
        }
        if (IsRead)
        {
            // Take a copy of the data from the managed heap to local store ( future?, maybe execute this on an
            // async thread)
            if ((readBuffer = (uint8_t *)platform_malloc(readSize)) != NULL)
            {
                memset(readBuffer, 0, readSize);

                CLR_INT32 slaveAddress = 0;
                I2cTransferStatus transferResult =
                    I2cIO::Write(deviceId, slaveAddress, writeBuffer, writeSize, I2C_CONTROL_TYPE::MASTER);
                platform_free(readBuffer);
                if (transferResult == I2cTransferStatus::I2cTransferStatus_FullTransfer)
                {
                    memcpy(readWorkingData->GetElement(bufferOffset), readBuffer, bufferSize);
                    stack.SetInteger(readCount);
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                }
            }
            else
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
