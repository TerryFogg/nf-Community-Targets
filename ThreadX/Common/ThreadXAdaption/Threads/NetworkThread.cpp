// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "board.h"
#include "nanoCLR_Interop.h"
#include <nx_api.h>

#define Network_THREAD_STACK_SIZE 256
#define Network_THREAD_PRIORITY   5
#define NX_PACKET_POOL_SIZE       ((1536 + sizeof(NX_PACKET)) * 50)

extern void NetworkThread_Entry(uint32_t parameter);
void NetworkStartupThread(uint32_t parameter);

TX_THREAD NetworkWorkerThread;
extern TX_BYTE_POOL byte_pool_0;
ULONG packet_pool_area[NX_PACKET_POOL_SIZE / 4 + 4];
NX_PACKET_POOL nx_packet_pool_0;
TX_EVENT_FLAGS_GROUP eventsNetworkWorkerThread;

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
        NetworkStartupThread,
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

void NetworkStartupThread(uint32_t parameter)
{
  NetworkThread_Entry(parameter);
}
