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

void NetworkThread_Entry(uint32_t parameter);
VOID _nx_ram_network_driver(NX_IP_DRIVER *driver_req_ptr);

void CreateCLRThread();
void CreateReceiverThread();
void CreateFileXThread(void *first_unused_memory);
void CreateI2CThread();
void CreateNetworkThread();
void CreateUsbThread();

void CLRThread_Entry(uint32_t parameter);
void ReceiverThread_entry(uint32_t parameter);
void I2CThread_Entry(ULONG parameter);
void NetworkThread_Entry(uint32_t parameter);
//void UsbThread_Entry(uint32_t parameter);

void usbx_cdc_acm_read_thread_entry(ULONG thread_input);
void usbx_cdc_acm_write_thread_entry(ULONG thread_input);
