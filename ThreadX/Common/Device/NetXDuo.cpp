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
// Typical Use Cases:
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
#include "CLRNativeThreads.h"
#include "nx_driver_pico_w.h"
#include <nx_api.h>

#define NX_PACKET_SIZE      1536
#define NX_PACKET_POOL_SIZE NX_PACKET_SIZE * 8
#define NX_ARP_SPACE_SIZE  1024

#define NX_DHCP_SERVER_IP_ADDRESS_0 IP_ADDRESS(10, 0, 0, 1)
#define START_IP_ADDRESS_LIST_0     IP_ADDRESS(10, 0, 0, 10)
#define END_IP_ADDRESS_LIST_0       IP_ADDRESS(10, 0, 0, 19)

#define NX_DHCP_SUBNET_MASK_0     IP_ADDRESS(255, 255, 255, 0)
#define NX_DHCP_DEFAULT_GATEWAY_0 IP_ADDRESS(10, 0, 0, 1)
#define NX_DHCP_DNS_SERVER_0      IP_ADDRESS(10, 0, 0, 1)

static NX_IP IP0;
static NX_PACKET_POOL Network_packet_pool_0;
//NX_DHCP dhcp_client;
uint8_t* pointer;
uint8_t *pArp_space;


bool NetworkStartup()
{
    UINT status;

    nx_system_initialize();

    pointer = (uint8_t *)platform_malloc(NX_PACKET_POOL_SIZE);
    status = nx_packet_pool_create(&Network_packet_pool_0, (char *)"NetX Main Packet Pool", 1024, pointer, NX_PACKET_POOL_SIZE);
    if (status != NX_SUCCESS)
    {
        return false;
    }
    status = nx_ip_create(
        &IP0,
        (char *)"NetX IP Instance 0",
        IP_ADDRESS(192, 168, 1, 139),
        0xFFFFFF00UL,
        &Network_packet_pool_0,
        nx_driver_framework_entry_default,
        pointer,
        2048,
        1);
    if (status != NX_SUCCESS)
    {
        return false;
    }

    pArp_space = (uint8_t *)platform_malloc(NX_ARP_SPACE_SIZE);
    status = nx_arp_enable(&IP0, (void *)pArp_space, NX_ARP_SPACE_SIZE);
    if (status != NX_SUCCESS)
    {
        return false;
    }

    status = nx_udp_enable(&IP0);
    if (status != NX_SUCCESS)
    {
        return false;
    }

    status = nx_tcp_enable(&IP0);
    if (status != NX_SUCCESS)
    {
        return false;
    }

    status = nx_icmp_enable(&IP0);
    if (status != NX_SUCCESS)
    {
        return false;
    }

    return true;
}

//void NetworkThread_Entry(uint32_t parameter)
//{
//    (void)parameter;
//
//    TX_EVENT_FLAGS_GROUP wpReceivedEvent;
//    ULONG actual_flags;
//
//    nx_system_initialize();
//
//    NANOCLR_HEADER();
//    {
//
//        // Main loop for network operations
//        while (1)
//        {
//            // Wait for the process network event to be set by the hardware or class library request
//            tx_event_flags_get(&eventsNetworkWorkerThread, 0x1, TX_OR_CLEAR, &actual_flags, NX_WAIT_FOREVER);
//        }
//    }
//    NANOCLR_CLEANUP();
//}
