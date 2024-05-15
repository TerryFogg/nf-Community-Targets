//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nanoCLR_Interop.h"
#include "sys_io_ser_native.h"
#include "board.h"
#include "DeviceIO.h"
#include "DeviceRegistration.h"

// An Events_Get clears the event
#define CLEAR_SERIAL_IN_EVENT Events_Get(SYSTEM_EVENT_FLAG_COM_IN)

int GetUsartDeviceNumber(CLR_RT_StackFrame &stack)
{
    int returnValue = 0;
    if (stack.This() != NULL)
    {
        bool IsDisposed =
            stack.This()[Library_sys_io_ser_native_System_IO_Ports_SerialPort::FIELD___disposed].NumericByRef().u1 != 0;
        if (!IsDisposed)
        {
            returnValue = ((int)stack.This()[Library_sys_io_ser_native_System_IO_Ports_SerialPort::FIELD___portIndex]
                               .NumericByRef()
                               .s4);
        }
    }
    return returnValue;
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            InitUSART(UsartDeviceNumber);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            DeInitUSART(UsartDeviceNumber);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_BytesToRead___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            int bytesAvailable = SerialBytesAvailable(UsartDeviceNumber);
            stack.SetResult_U4(bytesAvailable);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::set_InvertSignalLevels___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            bool InvertedSignal = (bool)stack.Arg1().NumericByRef().u1;
            SetSerialDeviceSignalsInverted(UsartDeviceNumber, InvertedSignal);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_InvertSignalLevels___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            bool result = IsSerialSignalInverted(UsartDeviceNumber);
            stack.SetResult_Boolean(result);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::Read___I4__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock hbTimeout;
        CLR_RT_HeapBlock_Array *dataBuffer;
        uint8_t *data = NULL;
        uint32_t length;
        uint32_t count = 0;
        uint32_t offset = 0;
        uint32_t bytesToRead = 0;
        uint32_t bytesRead = 0;

        CLR_INT32 timeout;
        int64_t *timeoutTicks;
        bool eventResult;

        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            timeout = stack.This()[FIELD___readTimeout].NumericByRef().s4;
            hbTimeout.SetInteger((CLR_INT64)timeout * TIME_CONVERSION__TO_MILLISECONDS);
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

            dataBuffer = stack.Arg1().DereferenceArray();
            FAULT_ON_NULL_ARG(dataBuffer);

            offset = stack.Arg2().NumericByRef().s4;
            count = stack.Arg3().NumericByRef().s4;

            bool ValidateParametersOnInitialCall = (stack.m_customState == 1);
            if (ValidateParametersOnInitialCall)
            {
                length = dataBuffer->m_numOfElements;
                if ((offset > length) || (count > length) || (offset + count > length))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                }
            }
            data = dataBuffer->GetElement(offset);
            bytesToRead = SerialBytesAvailable(UsartDeviceNumber);
            if (bytesToRead > 0)
            {
                eventResult = false;
                CLEAR_SERIAL_IN_EVENT;
            }
            else
            {
                if (ValidateParametersOnInitialCall)
                {
                    eventResult = false;
                    CLEAR_SERIAL_IN_EVENT;
                    // Delay reading from the buffer
                    bytesToRead = 0;
                }
            }
            while (eventResult)
            {
                if (ValidateParametersOnInitialCall)
                {
                    if (bytesToRead > 0)
                    {
                        eventResult = false;
                    }
                    else
                    {
                        stack.m_customState = 2;
                    }
                }
                else
                {
                    // wait for event
                    NANOCLR_CHECK_HRESULT(
                        g_CLR_RT_ExecutionEngine
                            .WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortIn, eventResult));
                    if (!eventResult)
                    {
                        NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
                    }
                }
            }
            if (bytesToRead > 0)
            {
                // pop the requested bytes from the ring buffer
                bytesRead = SerialReadBytes(UsartDeviceNumber, data, bytesToRead);
            }
            // pop "hbTimeout" heap block from stack
            stack.PopValue();
            // return how many bytes were read
            stack.SetResult_U4(bytesRead);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::ReadExisting___STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint8_t *buffer = NULL;
        uint32_t bufferLength;
        CLR_RT_HeapBlock &top = stack.PushValue();

        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            bufferLength = SerialBytesAvailable(UsartDeviceNumber);
            if (bufferLength)
            {
                // Create a read buffer to store the input data
                if ((buffer = (uint8_t *)platform_malloc(bufferLength)) == NULL)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
                }
                SerialReadBytes(UsartDeviceNumber, buffer, bufferLength);
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(top, (const char *)buffer, bufferLength));
                platform_free(buffer);
            }
            else
            {
                // create an empty <string>
                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(top, (const char *)NULL));
            }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::ReadLine___STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock hbTimeout;
        uint8_t *line = NULL;
        const char *newLine;
        uint32_t newLineLength;
        int64_t *timeoutTicks;
        bool eventResult = true;
        CLR_INT32 timeout;

        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            timeout = stack.This()[FIELD___readTimeout].NumericByRef().s4;
            hbTimeout.SetInteger((CLR_INT64)timeout * TIME_CONVERSION__TO_MILLISECONDS);
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

            bool ValidateParametersOnInitialCall = (stack.m_customState == 1);
            if (ValidateParametersOnInitialCall)
            {
                newLine = stack.This()[FIELD___newLine].RecoverString();
                newLineLength = hal_strlen_s(newLine);

                bool IsLineRead = SerialReadLine(UsartDeviceNumber, newLine, newLineLength, line);

                if (IsLineRead)
                {
                    eventResult = false;
                }

                {
                    newLineLength--;
                    // set new line char as the last one in the string
                    // only if this one is found it will have a chance of the others being there
                    stack.m_customState = 2;
                }
            }

            while (eventResult)
            {
                // wait for event
                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine
                        .WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortIn, eventResult));
                if (eventResult)
                {
                    //GetLineFromRxBuffer(stack.This(), &(palUart->RxRingBuffer), line);
                    break;
                }
                else
                {
                    // event timeout
                    NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
                }
            }

            // pop "hbTimeout" heap block from stack
            stack.PopValue();

            // return how many bytes were read
            stack.SetResult_String((const char *)line);

            // free memory, if needed
            if (line != NULL)
            {
                platform_free(line);
            }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::Write___VOID__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock_Array *dataBuffer;
        CLR_RT_HeapBlock hbTimeout;
        int64_t *timeoutTicks;
        bool eventResult = true;

        uint8_t *data;
        int32_t length = 0;
        int32_t count = 0;
        int32_t offset = 0;

        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            // setup timeout
            hbTimeout.SetInteger(
                (CLR_INT64)stack.This()[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

            // perform parameter validation and setup TX operation
            if (stack.m_customState == 1)
            {
                // dereference the data buffer from the argument
                dataBuffer = stack.Arg1().DereferenceArray();
                offset = stack.Arg2().NumericByRef().s4;
                count = stack.Arg3().NumericByRef().s4;

                // get the size of the buffer
                length = dataBuffer->m_numOfElements;

                // check parameters
                FAULT_ON_NULL_ARG(dataBuffer);

                if ((offset > length) || (count > length))
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
                }

                if (offset + count > length)
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
                }

                // get a the pointer to the array by using the offset
                data = dataBuffer->GetElement(offset);

                // push onto the eval stack how many bytes are being pushed to the UART
                stack.PushValueI4(count);

                SerialWriteBytes(UsartDeviceNumber, data, count);
                // bump custom state
                stack.m_customState = 2;
            }

            while (eventResult)
            {
                // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine
                        .WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));

                if (eventResult)
                {
                    // event occurred
                    // get from the eval stack how many bytes were buffered to Tx
                    count = stack.m_evalStack[1].NumericByRef().s4;

                    // done here
                    break;
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
                }
            }

            // pop "count" heap block from stack
            stack.PopValue();

            // pop "hbTimeout" heap block from stack
            stack.PopValue();

            stack.SetResult_U4(count);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeConfig___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            // Check RS485 mode is not selected as currently not supported
            if ((SerialMode)stack.This()[FIELD___mode].NumericByRef().s4 != SerialMode_Normal)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
            }
            int RequestedStopBits = stack.This()[FIELD___stopBits].NumericByRef().s4;
            int RequestedBaudRate = (int)stack.This()[FIELD___baudRate].NumericByRef().s4;

            SerialSetStopBits(UsartDeviceNumber, RequestedStopBits);
            SerialSetBaudRate(UsartDeviceNumber,RequestedBaudRate);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeSetWatchChar___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            uint8_t watchCharacter = (uint8_t)stack.This()[FIELD___watchChar].NumericByRef().u1;
            SerialSetWatchCharacter(UsartDeviceNumber,watchCharacter);
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeWriteString___VOID__STRING__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock hbTimeout;
        int64_t *timeoutTicks;
        bool eventResult = true;

        bool isNewAllocation = false;
        char *buffer = NULL;
        uint32_t bufferLength = 0;
        int32_t length = 0;

        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            if (stack.Arg1().RecoverString() == NULL)
            {
                stack.SetResult_U4(0);
                NANOCLR_SET_AND_LEAVE(S_OK);
            }

            // setup timeout
            hbTimeout.SetInteger(
                (CLR_INT64)stack.This()[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
            NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

            if (stack.m_customState == 1)
            {
                // get buffer to output
                NANOCLR_CHECK_HRESULT(SetupWriteLine(stack, &buffer, &bufferLength, &isNewAllocation));
                stack.PushValueI4(bufferLength);
                stack.PushValueI4(isNewAllocation ? 1 : 0);
                SerialWriteBytes(UsartDeviceNumber, (uint8_t *)&buffer, bufferLength);
                stack.m_customState = 2;
            }

            while (eventResult)
            {
                // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
                NANOCLR_CHECK_HRESULT(
                    g_CLR_RT_ExecutionEngine
                        .WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));
                if (eventResult)
                {
                    // event occurred
                    // pop "isNewAllocation" from the eval stack
                    isNewAllocation = stack.m_evalStack[2].NumericByRef().s4 == 1 ? true : false;

                    // get from the eval stack how many bytes were buffered to Tx
                    length = stack.m_evalStack[1].NumericByRef().s4;

                    // done here
                    break;
                }
                else
                {
                    NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
                }
            }

            // pop "length" heap block from stack
            stack.PopValue();

            // pop "isNewAllocation" heap block from stack
            stack.PopValue();

            // pop "hbTimeout" heap block from stack
            stack.PopValue();

            stack.SetResult_U4(length);

            // free memory, if it was allocated
            if (isNewAllocation)
            {
                //platform_free(palUart->TxBuffer);
            }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeReceivedBytesThreshold___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int UsartDeviceNumber;
        if ((UsartDeviceNumber = GetUsartDeviceNumber(stack)) > 0)
        {
            int32_t RequestedThreshold = (int32_t)stack.Arg1().NumericByRef().s4;
            if (RequestedThreshold <= 0)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
            }
            int32_t thresholdSet = SerialSetThreshold(UsartDeviceNumber, RequestedThreshold);
            stack.This()[FIELD___receivedBytesThreshold].NumericByRef().s4 = thresholdSet;
        //    // fake call to event handler in case port is open and the new threshold was set
        //    // to a value lower than the bytes that are already available
        //    if (stack.This()[FIELD___opened].NumericByRef().u1 &&
        //        (uint32_t)RequestedThreshold <= palUart->RxRingBuffer.Length())
        //    {
        //        PostManagedEvent(EVENT_SERIAL, 0, portIndex, SerialData_Chars);
        //    }
        }
        else
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // declare the device selector string whose max size is "COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8," + terminator
        // and init with the terminator
        static char deviceSelectorString[] = {

            "COM1,"
            "COM2,"
            "COM3,"
            "COM4,"
            "COM5,"
            "COM6,"
            "COM7,"
            "COM8,"
            "COM9,"
            "COM10"};

        if (deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] == ',')
        {
            deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';
        }
        stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
