//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "board.h"
#include "DeviceIO.h"
#include "Device.h"

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

struct I2C_Buses IC2_Bus[] = {
#if defined(I2C1)
    {I2C1, I2C_BUSSPEED_NOT_SET, {0}, 0},
    #endif
#if defined(I2C2)
    {I2C2, I2C_BUSSPEED_NOT_SET, {0}, 0},
#endif
#if defined(I2C3)
    {I2C3, I2C_BUSSPEED_NOT_SET, {0}, 0},
#endif
#if defined(I2C4)
    {I2C4, I2C_BUSSPEED_NOT_SET, {0}, 0},
#endif
#if defined(I2C5)
    {I2C5, I2C_BUSSPEED_NOT_SET, {0}, 0},
#endif
#if defined(I2C6)
    {I2C6, I2C_BUSSPEED_NOT_SET, {0}, 0}
#endif
};

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        CLR_RT_HeapBlock *connectionSettings = pThis[FIELD___connectionSettings].Dereference();
        CLR_INT32 busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;
        I2cBusSpeed busSpeed =
            (I2cBusSpeed)connectionSettings[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4;

        int slaveaddress = 0;
        if (!Initialize_Native_I2C(busIndex, busSpeed, Master, slaveaddress))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());
        CLR_RT_HeapBlock *connectionSettings = stack.This()[FIELD___connectionSettings].Dereference();
        CLR_INT32 busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        if (IsValidI2CBus(busIndex))
        {
            if (IC2_Bus[busIndex].numberOfActiveDevices > 0)
            {
                IC2_Bus[busIndex].numberOfActiveDevices--;
                if (IC2_Bus[busIndex].numberOfActiveDevices == 0)
                {
                    if (!DeInitialize_Native_I2C(busIndex))
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                    }
                }
            }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::
    NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());

        //CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        CLR_RT_HeapBlock *readSpanByte = stack.Arg2().Dereference();
        CLR_RT_HeapBlock *writeSpanByte = stack.Arg1().Dereference();

        CLR_RT_HeapBlock *connectionSettings = stack.This()[FIELD___connectionSettings].Dereference();
        CLR_RT_HeapBlock_Array *readData = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
        CLR_RT_HeapBlock_Array *writeData = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
        int slaveAddress = connectionSettings[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;
        uint8_t busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        bool WriteI2C = (writeSpanByte != NULL && writeData != NULL);
        bool ReadI2C = (readSpanByte != 0 && readData != NULL);
        uint8_t writeSize = (writeData == NULL) ? 0 : writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
        //uint8_t writeOffset = (writeData == NULL) ? 0 : writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
        uint8_t readSize = (readData == NULL) ? 0 : readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
        //uint8_t readOffset = (readData == NULL) ? 0 : readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;

        if (IsValidI2CBus(busIndex))
        {
            if (WriteI2C && writeSize > 0)
            {
                if (!I2CWriteData(busIndex, (uint8_t *)writeData, writeSize, slaveAddress))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                }
            }
            if (ReadI2C && readSize > 0)
            {
                if (!I2CReadData(busIndex, (uint8_t *)readData, readSize, slaveAddress))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
