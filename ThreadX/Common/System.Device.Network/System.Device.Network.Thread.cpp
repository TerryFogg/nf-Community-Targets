//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


//  https://github.com/eclipse-threadx/rtos-docs/blob/main/rtos-docs/netx-duo/chapter5.md

// Use 1 nx_ip_create()
// Use multiple nx_ip_interface_attach() for additional interfaces.
// 
// This allows:
//  - Shared packet pool and socket space
//  - Unified routing and configuration
//  - Multiple interfaces (e.g. Ethernet + Wi-Fi) under one IP stack
// 
//Typical Use Cases:
// 
//  - Multi-homed devices (e.g. Ethernet + cellular)
//  - VLAN setups or virtual interfaces
//  - Bridging or routing between interfaces
//  - Shared stack with multiple interfaces
//  - Unified routing across interfaces
//
// After calling nx_ip_create() once, you can attach more interfaces using nx_ip_interface_attach().
//

#include "sys_net_native.h"
#include "System.Device.Wifi.h"
#include "nx_secure_tls_api.h"
#include <nx_api.h>
#include "tx_api.h"

#define NX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * 50)
#define QUEUE_SIZE          10

NX_PACKET_POOL pool_0;
ULONG packet_pool_area[NX_PACKET_POOL_SIZE / 4 + 4];
NX_IP ip_0;
ULONG ip_thread_stack[2 * 1024 / sizeof(ULONG)];
ULONG arp_space_area[512 / sizeof(ULONG)];
ULONG error_counter;
UINT status;

static bool socket_data_available = false;
TX_QUEUE Network_message_queue;
TX_EVENT_FLAGS_GROUP eventsNetworkWorkerThread;
void *first_unused_memory;

void NetworkThread_entry(ULONG arg)
{
    // Unused parameter
    (void)arg;
    TX_EVENT_FLAGS_GROUP wpReceivedEvent;
    ULONG actual_flags;

    NANOCLR_HEADER();
    {
        tx_event_flags_create(&eventsNetworkWorkerThread, (CHAR *)"WaitForRequestedOrPhysicalNetworkAccess");

        nx_system_initialize();

        status = nx_packet_pool_create(
            &pool_0,
            "NetX Main Packet Pool",
            1536,
            (ULONG *)(((int)packet_pool_area + 15) & ~15),
            NX_PACKET_POOL_SIZE);
        if (status == NX_SUCCESS)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // Create an IP instance.
        status = nx_ip_create(
            &ip_0,
            "NetX IP Instance 0",
            IP_ADDRESS(192, 168, 1, 139),
            0xFFFFFF00UL,
            &pool_0,
            nx_driver_pico_w,
            (UCHAR *)ip_thread_stack,
            sizeof(ip_thread_stack),
            1);
        if (status)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_);
        }

        // Enable ARP and supply ARP cache memory for IP Instance 0.
        status = nx_arp_enable(&ip_0, (void *)arp_space_area, sizeof(arp_space_area));

        if (status)
            error_counter++;

        status = nx_tcp_enable(&ip_0);
        if (status)
            error_counter++;

        status = nx_udp_enable(&ip_0);
        if (status)
            error_counter++;

        status = nx_icmp_enable(&ip_0);
        if (status)
            error_counter++;

        // Main loop for network operations
        while (1)
        {
            // Wait for the process network event to be set by the hardware or class library request
            tx_event_flags_get(&eventsNetworkWorkerThread, 0x1, TX_OR_CLEAR, &actual_flags, NX_WAIT_FOREVER);
        }
    }
    NANOCLR_NOCLEANUP();
}
