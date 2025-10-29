// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "targetHAL.h"
#include <nanoCLR_Application.h>
#include "CLRNativeThreads.h"
#include <tx_api.h>
#include "board.h"

#define RECEIVER_THREAD_PRIORITY 5
#define RECEIVER_THREAD_STACK_SIZE 5000

#define CLR_THREAD_PRIORITY        5
#define CLR_THREAD_STACK_SIZE      3000

#define I2C_THREAD_PRIORITY   5
#define I2C_THREAD_STACK_SIZE 1000

#define Network_THREAD_STACK_SIZE 256
#define Network_THREAD_PRIORITY   5

#define FileX_THREAD_STACK_SIZE 256
#define FileX_THREAD_PRIORITY   5

#define USBX_READ_STACK_SIZE  500
#define USBX_WRITE_STACK_SIZE 500

#define NX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * 50)

#define DEFAULT_BYTE_POOL_SIZE 10000
uint8_t memory_area[DEFAULT_BYTE_POOL_SIZE];

TX_BYTE_POOL byte_pool_0;

TX_THREAD CLRThread;

TX_THREAD receiverThread;

TX_THREAD I2CThread;
TX_EVENT_FLAGS_GROUP eventsI2CWorkerThread;

TX_THREAD NetworkWorkerThread;
TX_EVENT_FLAGS_GROUP eventsNetworkWorkerThread;

TX_THREAD FileXThread;
TX_EVENT_FLAGS_GROUP eventsFileXWorkerThread;

ULONG packet_pool_area[NX_PACKET_POOL_SIZE / 4 + 4];
NX_PACKET_POOL nx_packet_pool_0;

TX_THREAD ux_cdc_read_thread;
TX_THREAD ux_cdc_write_thread;
TX_EVENT_FLAGS_GROUP usbx_cdc_event_flag_group;

#define FILEX_QUEUE_SIZE 10
TX_QUEUE File_message_queue;
FileRequest Request;


extern TX_BYTE_POOL byte_pool_0;
extern bool g_waitForDebuggerRequested;

void tx_application_define(void *first_unused_memory)
{
    // Create a byte memory pool from which to allocate the thread stacks
    const char *str = "byte pool 0";
    tx_byte_pool_create(&byte_pool_0, (char *)str, memory_area, DEFAULT_BYTE_POOL_SIZE);

    CreateReceiverThread();

    CreateCLRThread();

    CreateI2CThread();

#ifdef NETWORKING_SUPPORT
    CreateNetworkThread();
#endif

#ifdef FILE_SYSTEM_SUPPORT
    CreateFileXThread(first_unused_memory);
#endif

#ifdef USB_SUPPORT
    CreateUsbThread();
#endif
}

void CreateCLRThread()
{
    void *pointer = TX_NULL;
    const char *clrThreadString = "CLR_Thread";

    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, CLR_THREAD_STACK_SIZE, TX_NO_WAIT);

    uint16_t status = tx_thread_create(
        &CLRThread,
        (char *)clrThreadString,
        CLRThread_Entry,
        g_waitForDebuggerRequested,
        pointer,
        CLR_THREAD_STACK_SIZE,
        CLR_THREAD_PRIORITY,
        CLR_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

void CLRThread_Entry(uint32_t parameter)
{
    bool userRequestedWaitForDebugger = (bool)parameter;
    CLR_SETTINGS clrSettings = {0};
    clrSettings.MaxContextSwitches = 50;
    clrSettings.EnterDebuggerLoopAfterExit = true;
    clrSettings.WaitForDebugger = userRequestedWaitForDebugger;
    nanoHAL_Initialize();
    ClrStartup(clrSettings);
}

void CreateReceiverThread()
{
    void *pointer = TX_NULL;
    const char *receiverThreadString = "Receiver Thread";
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, RECEIVER_THREAD_STACK_SIZE, TX_NO_WAIT);
    uint16_t status = tx_thread_create(
        &receiverThread,
        (char *)receiverThreadString,
        ReceiverThread_entry,
        0,
        pointer,
        RECEIVER_THREAD_STACK_SIZE,
        RECEIVER_THREAD_PRIORITY,
        RECEIVER_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
    return;
}

void FilexThread_Entry(uint32_t parameter)
{
}

void CreateI2CThread()
{
    void *pointer = TX_NULL;

    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, I2C_THREAD_STACK_SIZE, TX_NO_WAIT);

    uint16_t status = tx_thread_create(
        &I2CThread,
        (char *)"I2C_Long_Running_Transactions_Thread",
        I2CThread_Entry,
        0,
        pointer,
        I2C_THREAD_STACK_SIZE,
        I2C_THREAD_PRIORITY,
        I2C_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

void CreateFileXThread(void *first_unused_memory)
{
    void *pointer = TX_NULL;

    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, FileX_THREAD_STACK_SIZE, TX_NO_WAIT);

    tx_queue_create(
        &File_message_queue,
        (CHAR *)"File Message Queue",
        sizeof(Request),
        first_unused_memory,
        FILEX_QUEUE_SIZE * sizeof(Request));

    uint16_t status = tx_thread_create(
        &FileXThread,
        (CHAR *)"FileX_Thread",
        FilexThread_Entry,
        0,
        pointer,
        FileX_THREAD_STACK_SIZE,
        FileX_THREAD_PRIORITY,
        FileX_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

void CreateNetworkThread()
{
    UINT status;
    void *pointer = TX_NULL;

    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, Network_THREAD_STACK_SIZE, TX_NO_WAIT);
    tx_event_flags_create(&eventsNetworkWorkerThread, (CHAR *)"WaitForRequestedOrPhysicalNetworkAccess");

    status = nx_packet_pool_create(
        &nx_packet_pool_0,
        (char *)"NetX Main Packet Pool",
        1536,
        (ULONG *)(((int)packet_pool_area + 15) & ~15),
        NX_PACKET_POOL_SIZE);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
    status = tx_thread_create(
        &NetworkWorkerThread,
        (char *)"Network_Worker_Thread",
        NetworkThread_Entry,
        0,
        pointer,
        Network_THREAD_STACK_SIZE,
        Network_THREAD_PRIORITY,
        Network_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }
}

void CreateUsbThread()
{
    void *pointer = TX_NULL;

    tx_event_flags_create(&usbx_cdc_event_flag_group, (char *)"usbx_cdc_event_flag_group");
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, USBX_READ_STACK_SIZE, TX_NO_WAIT);
    uint16_t status = tx_thread_create(
        &ux_cdc_read_thread,
        (CHAR *)"cdc_acm_read_usbx_app_thread_entry",
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
        (CHAR *)"cdc_acm_write_usbx_app_thread_entry",
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

