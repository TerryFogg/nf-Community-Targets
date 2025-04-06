//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoCLR_Types.h"
#include "sys_dev_i2c_native.h"
#include "DeviceIO.h"
#include "Device.h"

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_RT_HeapBlock *connectionSettings = stack.This()[FIELD___connectionSettings].Dereference();
        CLR_INT32 I2Cbus = connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;
        I2cBusSpeed I2cSpeed =
            (I2cBusSpeed)connectionSettings[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4;

        if (!I2cIO::Initialize(I2Cbus, I2cSpeed, I2C_CONTROL_TYPE::MASTER))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_RT_HeapBlock *connectionSettings;
        connectionSettings = stack.This()[FIELD___connectionSettings].Dereference();
        uint8_t deviceId = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;
        I2cIO::Dispose(deviceId);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::
    NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());

        uint8_t *readBuffer;
        uint8_t *writeBuffer;

        // create the return object (I2cTransferResult)
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult);
        CLR_RT_HeapBlock *result = top.Dereference();
        FAULT_ON_NULL(result);

        CLR_RT_HeapBlock *readSpanByte = stack.Arg2().Dereference();
        CLR_RT_HeapBlock *writeSpanByte = stack.Arg1().Dereference();

        CLR_RT_HeapBlock *connectionSettings = stack.This()[FIELD___connectionSettings].Dereference();
        CLR_RT_HeapBlock_Array *readData = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
        CLR_RT_HeapBlock_Array *writeData = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
        int slaveAddress = connectionSettings[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;

        int writeOffset = (writeSpanByte == NULL) ? -1 : writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
        int writeSize = (writeSpanByte == NULL) ? -1 : writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

        int readOffset = (readSpanByte == NULL) ? -1 : readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
        int readSize = (readSpanByte == NULL) ? -1 : readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

        bool IsWrite = (writeOffset >= 0 && writeSize > 0);
        bool IsRead = (readOffset >= 0 && readSize > 0);

        uint8_t deviceId = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        if (IsWrite)
        {
            // Allocate memory for a write buffer
            writeBuffer = (uint8_t *)platform_malloc(writeSize);
            {
                if (writeBuffer == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }
                // Take a local copy of the data from the managed heap
                memcpy(writeBuffer, (uint8_t *)writeData->GetElement(writeOffset), writeSize);
                I2cTransferStatus transferResult =
                    I2cIO::Write(deviceId, slaveAddress, writeBuffer, writeSize, I2C_CONTROL_TYPE::MASTER);
                result[I2cTransferResult::FIELD___status].SetInteger(transferResult);
                result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(0);
            }
            platform_free(writeBuffer);
        }

        if (IsRead)
        {
            // Allocate memory for a read buffer
            readBuffer = (uint8_t *)platform_malloc(readSize);
            {
                if (readBuffer == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }
                I2cTransferStatus transferResult = I2cIO::Read(deviceId, slaveAddress, readBuffer, readSize);
                if (transferResult != I2cTransferStatus::I2cTransferStatus_FullTransfer)
                {
                    result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(0);
                }
                else
                {
                    memcpy(readData->GetElement(readOffset), readBuffer, readSize);
                    result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_FullTransfer);
                    result[I2cTransferResult::FIELD___bytesTransferred].SetInteger((CLR_UINT32)(writeSize + readSize));
                    result[I2cTransferResult::FIELD___status].SetInteger(transferResult);
                }
            }
            platform_free(readBuffer);
        }
    }
    NANOCLR_NOCLEANUP();
}
