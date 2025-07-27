// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <nanoCLR_Application.h>
#include <tx_api.h>
#include "board.h"
#include "CLRNativeThreads.h"

#define USBX_READ_STACK_SIZE  500
#define USBX_WRITE_STACK_SIZE 500

extern TX_BYTE_POOL byte_pool_0;


TX_THREAD ux_cdc_read_thread;
TX_THREAD ux_cdc_write_thread;
TX_EVENT_FLAGS_GROUP usbx_cdc_event_flag_group;
extern TX_BYTE_POOL byte_pool_0;

const char *str = "usbx_cdc_event_flag_group";

void CreateUsbThread()
{
    CHAR *pointer = TX_NULL;
    tx_event_flags_create(&usbx_cdc_event_flag_group, (char *)str);
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, USBX_READ_STACK_SIZE, TX_NO_WAIT);
    uint16_t status = tx_thread_create(
        &ux_cdc_read_thread,
        "cdc_acm_read_usbx_app_thread_entry",
        usbx_cdc_acm_read_thread_entry,
        1,
        pointer,
        USBX_READ_STACK_SIZE,
        20,
        20,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, USBX_WRITE_STACK_SIZE, TX_NO_WAIT);
    /* Create the usbx_cdc_acm_write_thread_entry thread */
    status = tx_thread_create(
        &ux_cdc_write_thread,
        "cdc_acm_write_usbx_app_thread_entry",
        usbx_cdc_acm_write_thread_entry,
        1,
        pointer,
        USBX_WRITE_STACK_SIZE,
        20,
        20,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

VOID usbx_cdc_acm_read_thread_entry(ULONG thread_input)
{
    uint8_t UserRxBufferFS[2048];
    uint8_t UserTxBufferFS[2048];
    ULONG senddataflag = 0;
    uint32_t UserTxBufPtrIn;
    uint32_t UserTxBufPtrOut;
/* Rx/TX flag */
#define TX_NEW_TRANSMITTED_DATA 0x02

    // Blocking read?
    // maybe need async read with event flag
    //  ux_device_class_cdc_acm_read(cdc_acm, (UCHAR *)UserRxBufferFS, 64, &actual_length);
    //  tx_event_flags_get(&usbx_cdc_event_flag_group, 0x1, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
}
VOID usbx_cdc_acm_write_thread_entry(ULONG thread_input)
{
    ULONG receivedataflag = 0;
    ULONG actual_length;
    ULONG buffptr;
    ULONG buffsize;

    // ux_device_class_cdc_acm_write(cdc_acm, (UCHAR *)(&UserTxBufferFS[buffptr]), buffsize, &actual_length);
}
