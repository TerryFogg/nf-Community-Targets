#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

typedef struct
{
    ULONG request_id;
    CHAR request_type[20];
    ULONG payload;
} FileRequest;

typedef struct
{
    ULONG request_id;
    CHAR request_type[20];
    ULONG payload;
} NetworkRequest;

typedef struct
{
    ULONG request_id;
    CHAR request_type[20];
    ULONG payload;
} I2CRequest;

typedef struct
{
    ULONG request_id;
    CHAR request_type[20];
    ULONG payload;
} UsbRequest;



void FileThread_Entry(ULONG parameter);
void I2CThread_Entry(ULONG parameter);
void NetworkThread_Entry(ULONG parameter);
void UsbThread_Entry(ULONG parameter);

void usbx_cdc_acm_read_thread_entry(ULONG thread_input);
void usbx_cdc_acm_write_thread_entry(ULONG thread_input);


void CreateCLRThread();
void CreateReceiverThread();
void CreateI2CThread();

void CreateUsbThread();
void CreateNetworkThread();
void CreateFileThread();
