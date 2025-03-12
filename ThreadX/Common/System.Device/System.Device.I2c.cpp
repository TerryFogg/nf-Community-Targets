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
        CLR_INT32 deviceId = connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        if (Device::IsValidI2CDevice(deviceId))
        {
            I2cBusSpeed I2cSpeed =
                (I2cBusSpeed)connectionSettings[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4;
            I2cIO::Initialize(deviceId, I2cSpeed);
        }
        else
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

        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        CLR_RT_HeapBlock *result = top.Dereference();
        FAULT_ON_NULL(result);

        uint8_t *writeBuffer;
        uint8_t *readBuffer;

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

        if (!Device::IsValidI2CDevice(deviceId))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        if (IsWrite)
        {
            // Take a copy of the data from the managed heap to local store ( future?, maybe execute this on an
            // async thread)
            if ((writeBuffer = (uint8_t *)platform_malloc(writeSize)) != NULL)
            {
                memcpy(writeBuffer, (uint8_t *)writeData->GetElement(writeOffset), writeSize);
                I2cTransferStatus transferResult = I2cIO::Write(deviceId, slaveAddress, writeBuffer, writeSize);
                platform_free(writeBuffer);
                result[I2cTransferResult::FIELD___status].SetInteger(transferResult);
                result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(0);
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
                I2cTransferStatus transferResult = I2cIO::Read(deviceId, slaveAddress, readBuffer, readSize);
                memcpy(readData->GetElement(readOffset), readBuffer, readSize);
                result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2cTransferStatus_FullTransfer);
                result[I2cTransferResult::FIELD___bytesTransferred].SetInteger((CLR_UINT32)(writeSize + readSize));
                platform_free(readBuffer);
                result[I2cTransferResult::FIELD___status].SetInteger(transferResult);
                result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(0);
            }
            else
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
