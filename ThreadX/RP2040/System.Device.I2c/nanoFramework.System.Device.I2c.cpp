//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "sys_dev_i2c_native.h"
#include "hardware/gpio.h"

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

i2c_inst *Get_I2C_Instance(uint8_t &busIndex);

i2c_inst *Get_I2C_Instance(uint8_t &busIndex) 
{
  switch (busIndex) {
    case 0:
        // I2C0_SDA valid pins : 0,4,8,12,16,20,24,28
        // I2C0_SCL valid pins : 1,5,9,13,17,21,25,29
        return i2c0;
        break;
    case 1:
        // I2C1_SDA valid pins : 2,6,10,14,18,22,26
        // I2C1_SCL valid pins : 3,7,11,15,19,23,27
        #define GPIO6 6
        #define GPIO7 7 
        gpio_set_function(GPIO6, GPIO_FUNC_I2C);
        gpio_set_function(GPIO7, GPIO_FUNC_I2C);
        return i2c1;
        break;
    default:
        // Only two I2C units ( 0 and 1)
        return NULL;
    }
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {

      CLR_RT_HeapBlock *pThis = stack.This();
      FAULT_ON_NULL(pThis);

        CLR_RT_HeapBlock *connectionSettings = pThis[FIELD___connectionSettings].Dereference();
        
        uint8_t busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;
        i2c_inst_t *I2C_Instance = Get_I2C_Instance(busIndex);

        if (I2C_Instance != NULL) 
        {
            I2cBusSpeed I2cSpeed = (I2cBusSpeed)connectionSettings[I2cConnectionSettings::FIELD___busSpeed].NumericByRef().s4;
            int i2cSpeedBits;

            switch (I2cSpeed) 
            {
                case I2cBusSpeed_StandardMode:
                    i2cSpeedBits = 100 * 1000;
                    break;
                case I2cBusSpeed_FastMode:
                    i2cSpeedBits = 400 * 1000;
                    break;
                case I2cBusSpeed_FastModePlus:
                    i2cSpeedBits = 1000 * 1000;
                    break;
                default:
                    // Only supports the speeds above
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                    break;
            }
          i2c_init(I2C_Instance, i2cSpeedBits);
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
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        CLR_RT_HeapBlock *connectionSettings;
        connectionSettings = pThis[FIELD___connectionSettings].Dereference();
        uint8_t busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

        i2c_inst_t *I2C_Instance = Get_I2C_Instance(busIndex);
        if (I2C_Instance != NULL) 
        {
          i2c_deinit(I2C_Instance);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
      CLR_RT_HeapBlock *pThis = stack.This();
      FAULT_ON_NULL(pThis);

      CLR_RT_HeapBlock &top = stack.PushValueAndClear();
      CLR_RT_HeapBlock *readSpanByte = stack.Arg2().Dereference();
      CLR_RT_HeapBlock *writeSpanByte = stack.Arg1().Dereference();

      CLR_RT_HeapBlock *connectionSettings = pThis[FIELD___connectionSettings].Dereference();
      CLR_RT_HeapBlock_Array *readData = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
      CLR_RT_HeapBlock_Array *writeData = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
      int slaveAddress = connectionSettings[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;
      uint8_t busIndex = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;

      i2c_inst_t *I2C_Instance = Get_I2C_Instance(busIndex);
      if (I2C_Instance == NULL)
      {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
      }

      if (writeSpanByte != NULL && writeData != NULL)
      {
          int writeOffset = writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
          int writeSize = writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

          if (writeSize > 0)
          {
              // Take a copy of the data from the managed heap to local store ( future?, maybe execute this on an async thread)
              uint8_t *writeBuffer = (uint8_t *)platform_malloc(writeSize);
              if (writeBuffer == NULL)
              {
                  NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
              }
              memcpy(writeBuffer, (uint8_t *)writeData->GetElement(writeOffset), writeSize);
              int i2cWriteResult = i2c_write_blocking(I2C_Instance, slaveAddress, writeBuffer, writeSize, true); // true to keep master control of bus
              platform_free(writeBuffer);
              // need to allocate buffer from internal memory

              if (i2cWriteResult == PICO_ERROR_TIMEOUT || i2cWriteResult == PICO_ERROR_GENERIC)
              {
                  NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
              }
          }
      }

      if (readSpanByte != 0 && readData != NULL)
      {
          int readOffset = readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
          int readSize = readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;

          if (readSize > 0)
          {
              // Take a copy of the data from the managed heap to local store ( future?, maybe execute this on an async thread)
              uint8_t *readBuffer = (uint8_t *)platform_malloc(readSize);
              if (readBuffer == NULL)
              {
                  NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
              }
              // clear allocated buffer
              memset(readBuffer, 0, readSize);

              int i2cReadResult = i2c_read_blocking(I2C_Instance, slaveAddress, readBuffer, readSize, true); // true to keep master control of bus
              platform_free(readBuffer);

              if (i2cReadResult == PICO_ERROR_TIMEOUT || i2cReadResult == PICO_ERROR_GENERIC)
              {
                  NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
              }
          }
      }
    }

    NANOCLR_NOCLEANUP();
}

