//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "CLRNativeThreads.h"
#include <tx_api.h>



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
