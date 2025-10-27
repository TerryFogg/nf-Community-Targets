#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoCLR_Types.h"

typedef struct
{
    CLR_INT32 request_id;
    CHAR request_type[20];
    CLR_UINT32 payload;
} FileRequest;

typedef struct
{
    CLR_INT32 request_id;
    CHAR request_type[20];
    CLR_UINT32 payload;
} NetworkRequest;

typedef struct
{
    CLR_INT32 request_id;
    CHAR request_type[20];
    CLR_UINT32 payload;
} I2CRequest;

typedef struct
{
    CLR_INT32 request_id;
    CHAR request_type[20];
    CLR_UINT32 payload;
} UsbRequest;

//#ifdef __cplusplus
//extern "C"
//{
//#endif
     void usbx_cdc_acm_read_thread_entry(CLR_UINT32 thread_input);
     void usbx_cdc_acm_write_thread_entry(CLR_UINT32 thread_input);

    void CreateCLRThread();
    void CreateReceiverThread();
    void CreateI2CThread();
    void CreateNetworkThread();
    void CreateUsbThread();
    void CreateFileThread();

//#ifdef __cplusplus
//}
//#endif
