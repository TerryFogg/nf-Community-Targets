//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nanoCLR_Types.h"
#include "sys_dev_i2c_native.h"
#include "Device.IO.h"
#include "ManagedThreadSupport.h"

static bool WorkerThreadCreated = false;
static I2c_Transaction I2CTransaction[NUMBER_I2C_BUSES];

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
    // NOTE: Important
    //
    // If the I2C transaction is long running then the transaction is handled by a thread and
    // this C# thread reliquishes to other c# threads and waits on Event "Event_I2cMaster"
    // This code does not safely allow further I2C transactions to be sent while waiting
    //
    NANOCLR_HEADER();
    {
        ULONG actual_flags;
        CLR_RT_HeapBlock hbTimeout;
        FAULT_ON_NULL(stack.This());
        {
            CLR_RT_HeapBlock &top = stack.PushValueAndClear();
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult);
            CLR_RT_HeapBlock *result = top.Dereference();
            CLR_RT_HeapBlock *conSettings =
                stack.This()[Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::FIELD___connectionSettings]
                    .Dereference();
            FAULT_ON_NULL(result);
            {

                CLR_RT_HeapBlock *readSpan = stack.Arg2().Dereference();
                CLR_RT_HeapBlock *writeSpan = stack.Arg1().Dereference();
                CLR_INT32 BusId = (uint8_t)conSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;
                CLR_INT32 slaveAddress = conSettings[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;
                CLR_RT_HeapBlock_Array *readData = readSpan[SpanByte::FIELD___array].DereferenceArray();
                CLR_RT_HeapBlock_Array *writeData = writeSpan[SpanByte::FIELD___array].DereferenceArray();
                CLR_INT32 writeOffset = (writeSpan == NULL) ? 0 : writeSpan[SpanByte::FIELD___start].NumericByRef().s4;
                CLR_INT32 writeSize = (writeSpan == NULL) ? 0 : writeSpan[SpanByte::FIELD___length].NumericByRef().s4;
                CLR_INT32 readOffset = (readSpan == NULL) ? 0 : readSpan[SpanByte::FIELD___start].NumericByRef().s4;
                CLR_INT32 readSize = (readSpan == NULL) ? 0 : readSpan[SpanByte::FIELD___length].NumericByRef().s4;
                bool IsWrite = writeSize > 0;
                bool IsRead = readSize > 0;

                CLR_INT32 transactionTime = I2cIO::GetByteTime(BusId) * (writeSize + readSize + 1);
                bool isLongRunningTransaction = transactionTime > CLR_RT_Thread::c_TimeQuantum_Milliseconds;

                // Setup the request
                // -----------------
                I2CTransaction[BusId].busId = BusId;
                I2CTransaction[BusId].slaveAddress = slaveAddress;
                I2CTransaction[BusId].writeOffset = writeOffset;
                I2CTransaction[BusId].writeSize = writeSize;
                I2CTransaction[BusId].readOffset = readOffset;
                I2CTransaction[BusId].readSize = readSize;
                I2CTransaction[BusId].IsWrite = IsWrite;
                I2CTransaction[BusId].IsRead = IsRead;

                if (IsWrite)
                {
                    // Allocate some local memory and take a local copy of the data from the managed heap
                    I2CTransaction[BusId].writeBuffer = (uint8_t *)platform_malloc(I2CTransaction[BusId].writeSize);
                    if (I2CTransaction[BusId].writeBuffer != NULL)
                    {
                        memcpy(
                            I2CTransaction[BusId].writeBuffer,
                            (uint8_t *)writeData->GetElement(I2CTransaction[BusId].writeOffset),
                            I2CTransaction[BusId].writeSize);
                    }
                    else
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                    }
                }

                if (IsRead)
                {
                    // Allocate some local memory to read from I2C device
                    I2CTransaction[BusId].readBuffer = (uint8_t *)platform_malloc(I2CTransaction[BusId].readSize);
                    if (I2CTransaction[BusId].readBuffer == NULL)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                    }
                }

                if (isLongRunningTransaction)
                {
                    // Request the transfer via the asynchronous IO thread
                    GLOBAL_LOCK();
                    {
                        tx_queue_send(&AsynchronousIOQueue, (void *)&I2CTransaction, TX_WAIT_FOREVER);
                        // Allow other C# threads to run while waiting for asynchronous IO to complete
                        RelinquishToOtherManagedThreads(stack, transactionTime, Event_I2cMaster);
                    }
                    GLOBAL_UNLOCK();
                }
                else
                // Execute immediately and return
                {
                    I2cIO::Execute(&I2CTransaction[BusId]);
                }

                // Return result after long running or immediate I2C transaction
                memcpy(
                    readData->GetElement(I2CTransaction[BusId].readOffset),
                    I2CTransaction[BusId].readBuffer,
                    I2CTransaction[BusId].readSize);
                result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2CTransaction[BusId].status);
                result[I2cTransferResult::FIELD___bytesTransferred].SetInteger(
                    (CLR_UINT32)I2CTransaction[BusId].bytesTransferred);

                // Free up allocated memory
                if (I2CTransaction[BusId].writeBuffer != NULL)
                {
                    platform_free(I2CTransaction[BusId].writeBuffer);
                }
                if (I2CTransaction[BusId].readBuffer != NULL)
                {
                    platform_free(I2CTransaction[BusId].readBuffer);
                }
            }
        }
    }
    NANOCLR_NOCLEANUP();
}
