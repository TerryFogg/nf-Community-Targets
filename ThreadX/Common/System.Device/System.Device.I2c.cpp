//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoCLR_Types.h"
#include "sys_dev_i2c_native.h"
#include "DeviceIO.h"
#include "Device.h"

void CreateI2CWorkerThread();
void I2CWorkingThread_entry(uint32_t parameter);
void RelinquishToOtherManagedThreads(CLR_RT_StackFrame &stack, int64_t estimatedTransactionTimeMilliseconds);

extern TX_EVENT_FLAGS_GROUP eventsI2CWorkerThread;
static bool WorkerThreadCreated = false;
static I2c_Transaction I2CTransaction;

typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings I2cConnectionSettings;
typedef Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult I2cTransferResult;
typedef Library_corlib_native_System_SpanByte SpanByte;

HRESULT Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        FAULT_ON_NULL(stack.This());

        if (!WorkerThreadCreated)
        {
            CreateI2CWorkerThread();
            WorkerThreadCreated = true;
        }

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

        // Check if another thread has a transaction in progress
        if (I2CTransaction.InProgress)
        {
            // Wait for previous thread to finished
            RelinquishToOtherManagedThreads(stack, I2CTransaction.estimatedTransactionTimeMilliseconds);
        }

        bool eventResult = true;
        CLR_RT_HeapBlock hbTimeout;
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult);
        CLR_RT_HeapBlock *result = top.Dereference();
        FAULT_ON_NULL(result);

        CLR_RT_HeapBlock *readSpanByte = stack.Arg2().Dereference();
        CLR_RT_HeapBlock *writeSpanByte = stack.Arg1().Dereference();
        CLR_RT_HeapBlock *connectionSettings =
            stack.This()[Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice::FIELD___connectionSettings]
                .Dereference();
        I2CTransaction.deviceId = (uint8_t)connectionSettings[I2cConnectionSettings::FIELD___busId].NumericByRef().s4;
        I2CTransaction.slaveAddress =
            connectionSettings[I2cConnectionSettings::FIELD___deviceAddress].NumericByRef().s4;
        CLR_RT_HeapBlock_Array *readData = readSpanByte[SpanByte::FIELD___array].DereferenceArray();
        CLR_RT_HeapBlock_Array *writeData = writeSpanByte[SpanByte::FIELD___array].DereferenceArray();
        I2CTransaction.writeOffset =
            (writeSpanByte == NULL) ? 0 : writeSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
        I2CTransaction.writeSize =
            (writeSpanByte == NULL) ? 0 : writeSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
        I2CTransaction.readOffset =
            (readSpanByte == NULL) ? 0 : readSpanByte[SpanByte::FIELD___start].NumericByRef().s4;
        I2CTransaction.readSize = (readSpanByte == NULL) ? 0 : readSpanByte[SpanByte::FIELD___length].NumericByRef().s4;
        I2CTransaction.IsWrite = (I2CTransaction.writeOffset >= 0 && I2CTransaction.writeSize > 0);
        I2CTransaction.IsRead = (I2CTransaction.readOffset >= 0 && I2CTransaction.readSize > 0);
        I2CTransaction.estimatedTransactionTimeMilliseconds =
            I2cIO::GetByteTime(I2CTransaction.deviceId) * (I2CTransaction.writeSize + I2CTransaction.readSize + 1);

        CLR_INT64 *timeout;

        if (I2CTransaction.IsWrite)
        {
            if ((I2CTransaction.writeBuffer = (uint8_t *)platform_malloc(I2CTransaction.writeSize)) == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
            // Take a local copy of the data from the managed heap
            memcpy(
                I2CTransaction.writeBuffer,
                (uint8_t *)writeData->GetElement(I2CTransaction.writeOffset),
                I2CTransaction.writeSize);
        }
        if (I2CTransaction.IsRead)
        {
            // Allocate memory for a read buffer
            if ((I2CTransaction.readBuffer = (uint8_t *)platform_malloc(I2CTransaction.readSize)) == NULL)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
            }
        }
        I2CTransaction.InProgress = true;
        I2CTransaction.bytesTransferred = 0;
        I2CTransaction.status = 0;

        {
            // Start the transfer by activating the worker thread
            tx_event_flags_set(&eventsI2CWorkerThread, 0x1, TX_OR);
        }
        if (I2CTransaction.longRunningTransaction)
        {
            RelinquishToOtherManagedThreads(stack, I2CTransaction.estimatedTransactionTimeMilliseconds);
        }
        else
        {
            // Wait for transfer to complete and continue
            ULONG actual_flags;
            tx_event_flags_get(&eventsI2CWorkerThread, 0x02, TX_AND_CLEAR, &actual_flags, TX_WAIT_FOREVER);
        }

        if (I2CTransaction.status == I2cTransferStatus::I2cTransferStatus_FullTransfer)
        {
            if (I2CTransaction.IsRead)
            {
                memcpy(
                    readData->GetElement(I2CTransaction.readOffset),
                    I2CTransaction.readBuffer,
                    I2CTransaction.readSize);
            }
        }
        result[I2cTransferResult::FIELD___status].SetInteger((CLR_UINT32)I2CTransaction.status);
        result[I2cTransferResult::FIELD___bytesTransferred].SetInteger((CLR_UINT32)I2CTransaction.bytesTransferred);

        if (I2CTransaction.writeBuffer != NULL)
        {
            platform_free(I2CTransaction.writeBuffer);
        }
        if (I2CTransaction.readBuffer != NULL)
        {
            platform_free(I2CTransaction.readBuffer);
        }
        // Reset all values to 0 and set the InProgress to false
        I2CTransaction = {0};
        I2CTransaction.InProgress = false;
    }
    NANOCLR_NOCLEANUP();
}
void RelinquishToOtherManagedThreads(CLR_RT_StackFrame &stack,int64_t timeoutMilliseconds)
{
    CLR_INT64 *timeout;
    bool eventResult = true;

    CLR_RT_HeapBlock hbTimeout;
    hbTimeout.SetInteger(timeoutMilliseconds * TIME_CONVERSION__TO_MILLISECONDS);
    stack.SetupTimeoutFromTicks(hbTimeout, timeout);

    // Wait here until native work completes and reliquish cpu to other c# threads
    while (eventResult)
    {
        g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_I2cMaster, eventResult);
    }
    // Pop timeout heap block from stack
    stack.PopValue();
}
void I2CWorkingThread_entry(ULONG parameter)
{
    ULONG actual_flags;
    // Loop continually, process is resumed when a write is required by setting the flag
    do
    {
        tx_event_flags_get(&eventsI2CWorkerThread, 0x1, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
        {
            // Set transfer result I2cTransferStatus_FullTransfer, it will updated by the result of the write if
            // requested
            I2CTransaction.status = I2cTransferStatus::I2cTransferStatus_FullTransfer;

            if (I2CTransaction.IsWrite)
            {
                I2CTransaction.status = I2cIO::Write(
                    I2CTransaction.deviceId,
                    I2CTransaction.slaveAddress,
                    I2CTransaction.writeBuffer,
                    I2CTransaction.writeSize,
                    I2C_CONTROL_TYPE::MASTER);
            }
            if (I2CTransaction.IsRead && (I2CTransaction.status == I2cTransferStatus::I2cTransferStatus_FullTransfer))
            {
                I2CTransaction.status = I2cIO::Read(
                    I2CTransaction.deviceId,
                    I2CTransaction.slaveAddress,
                    I2CTransaction.readBuffer,
                    I2CTransaction.readSize);
            }

            if (I2CTransaction.longRunningTransaction)
            {
                //  For long running transactions, the I2C requestor has setup to wait for event and returned to other
                //  c# threads if available
                Events_Set(SYSTEM_EVENT_FLAG_I2C_MASTER);
            }
            else
            {
                // For short transactions, the I2C requestor is waiting on this flag to complete and return
                tx_event_flags_set(&eventsI2CWorkerThread, 0x02, TX_OR);
            }
        }
    } while (true);
}
