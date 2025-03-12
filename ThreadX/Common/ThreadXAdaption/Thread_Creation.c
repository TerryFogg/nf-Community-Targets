// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include <nanoCLR_Application.h>
#include <tx_api.h>
#include "board.h"
#include "target_platform.h"
#include <nanoHAL_v2.h>
#include "targetHAL.h"
#include "WireProtocol_ReceiverThread.h"
#include <WireProtocol_Message.h>

#ifdef FILEX
#include <fx_api.h>
#endif

void CLRStartupThread(uint32_t parameter);
void ReceiverThread_entry(uint32_t parameter);
void CreateReceiverThread();
void CreateCLRThread();
void CreateUsbXThreads();
void usbx_cdc_acm_read_thread_entry(ULONG thread_input);
void usbx_cdc_acm_write_thread_entry(ULONG thread_input);

#define RECEIVER_THREAD_PRIORITY 5
#define CLR_THREAD_PRIORITY      5

bool g_waitForDebuggerRequested;

TX_BYTE_POOL byte_pool_0;

uint8_t memory_area[DEFAULT_BYTE_POOL_SIZE];
TX_THREAD receiverThread;
TX_THREAD CLRThread;
TX_THREAD ux_cdc_read_thread;
TX_THREAD ux_cdc_write_thread;
TX_EVENT_FLAGS_GROUP usbx_cdc_event_flag_group;

void Startup_Rtos(bool debuggerRequested)
{
    g_waitForDebuggerRequested = debuggerRequested;
    tx_event_flags_create(&usbx_cdc_event_flag_group, "usbx_cdc_event_flag_group");
    tx_kernel_enter();
}

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    // Create a byte memory pool from which to allocate the thread stacks
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEFAULT_BYTE_POOL_SIZE);

    CreateReceiverThread();
    CreateCLRThread();
    // CreateUsbXThreads();

#ifdef FILEX
    fx_system_initialize();
#endif
}

#pragma region Threads
void CreateReceiverThread()
{
    CHAR *pointer = TX_NULL;
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, RECEIVER_THREAD_STACK_SIZE, TX_NO_WAIT);
    uint16_t status = tx_thread_create(
        &receiverThread,
        "Receiver Thread",
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
void CreateCLRThread()
{
    CHAR *pointer = TX_NULL;
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, CLR_THREAD_STACK_SIZE, TX_NO_WAIT);
    uint16_t status = tx_thread_create(
        &CLRThread,
        "CLR_Thread",
        CLRStartupThread,
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

#pragma region Thread startup programs

void CLRStartupThread(uint32_t parameter)
{
    bool userRequestedWaitForDebugger = (bool)parameter;

    // CLR settings to launch CLR thread
    CLR_SETTINGS clrSettings;
    (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));
    clrSettings.MaxContextSwitches = 50;
    clrSettings.EnterDebuggerLoopAfterExit = true;
    clrSettings.WaitForDebugger = userRequestedWaitForDebugger;

    nanoHAL_Initialize_C();
    ClrStartup(clrSettings);
}
void ReceiverThread_entry(uint32_t parameter)
{
    (void)parameter;
    extern WP_Message inboundMessage;

    // NOTE: Don't call scheduler type calls in this module
    InitWireProtocolCommunications();
    tx_thread_sleep(50);

    WP_Message_Initialize(&inboundMessage);
    WP_Message_PrepareReception(&inboundMessage);

    // loop until thread receives a request to terminate
    while (true)
    {
        WP_Message_Process(&inboundMessage);
        tx_thread_relinquish();
    }
}
#pragma endregion

#ifdef USING_USBX
void CreateUsbXThreads()
{
#define USBX_READ_STACK_SIZE  500
#define USBX_WRITE_STACK_SIZE 500

    CHAR *pointer = TX_NULL;
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
#endif
