//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "sys_io_ser_native.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "CircularBuffer.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1

CircularBuffer_t ReceiveCircularBuffer;
uint8_t ReceiveData[512];
CircularBuffer_t TransmitCircularBuffer;
uint8_t TransmitData[256];
uint8_t DMA_Receive_Buffer[256];



void on_uart_rx() {
  while (uart_is_readable(UART_ID)) {
    uint8_t charReceived = uart_getc(UART_ID);
    WriteCircularBuffer(&ReceiveCircularBuffer,&charReceived, 1);
    Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
  }
}
void on_uart_tx(uint8_t *data) {
  if (uart_is_writable(UART_ID)) {
    uart_putc_raw(UART_ID,data);
  }
  WriteCircularBuffer(&ReceiveCircularBuffer, &charReceived, 1);
  Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
  }
}
  
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeInit___VOID(CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    InitializeCircularBuffer(&TransmitCircularBuffer, &TransmitData, sizeof(TransmitData));
    InitializeCircularBuffer(&ReceiveCircularBuffer, &ReceiveData,sizeof(ReceiveData));

    
    
    uart_init(UART_ID, BAUD_RATE);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID, false);
    uart_set_translate_crlf(UART_ID, false);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);
    }

    static size_t old_position;

    // Calculate current position in buffer and check for new data available
    int length_data_received;
    size_t position =
        ARRAY_LEN(DMA_Receive_Buffer) - length_data_received;

    if (position != old_position) {

      if (position > old_position) {
        WriteCircularBuffer(&ReceiveCircularBuffer,
                            &DMA_Receive_Buffer[old_position],
                       position - old_position);
      } else {
        // Processing is done in "overflow" mode..
        WriteCircularBuffer(&ReceiveCircularBuffer,
                            &DMA_Receive_Buffer[old_position],
                            ARRAY_LEN(DMA_Receive_Buffer) - old_position);

        if (position > 0) {
          WriteCircularBuffer(&ReceiveCircularBuffer, &DMA_Receive_Buffer[0],
                         position);
        }
      }
      old_position = position;
      tx_event_flags_set(&wpReceivedBytesEvent, 0x1, TX_OR);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT
Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeDispose___VOID(
    CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  {
    // Nothing to do
  }
  NANOCLR_NOCLEANUP();
}



HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_InvertSignalLevels___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    (void)stack;
    {
      NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED); 
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::set_InvertSignalLevels___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    (void)stack;

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();
}

HRESULT
Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_BytesToRead___I4(
    CLR_RT_StackFrame &stack) {
  NANOCLR_HEADER();
  { NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub()); }
  NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::Read___I4__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::ReadExisting___STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::ReadLine___STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::Write___VOID__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    { 
      uart_write_blocking(UART_ID, &pre, 1); 
    }
    NANOCLR_NOCLEANUP();
}


HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeConfig___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeSetWatchChar___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeWriteString___VOID__STRING__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
      uart_puts(UART_ID, "\nHello, uart interrupts\n"); 
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeReceivedBytesThreshold___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

static HRESULT SetupWriteLine(CLR_RT_StackFrame &stack, char **buffer, uint32_t *length, bool *isNewAllocation)
{
    (void)stack;
    (void)buffer;
    (void)length;
    (void)isNewAllocation;

    NANOCLR_HEADER();
    NANOCLR_NOCLEANUP();
}

static HRESULT PerformWriteOperation(const char *buffer, int32_t offset, int32_t count)
{
    (void)buffer;
    (void)offset;
    (void)count;

    NANOCLR_HEADER();
    NANOCLR_NOCLEANUP();
}
