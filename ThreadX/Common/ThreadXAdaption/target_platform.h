#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "tx_port.h"
#include "tx_api.h"
#include "nx_api.h"
#include "nx_user.h"
#include "nxd_dhcp_client.h"
#include "nx_udp.h"
#include "nxd_dns.h"
#include "nx_crypto_ec.h"

#define GLOBAL_LOCK()   TX_INTERRUPT_SAVE_AREA TX_DISABLE
#define GLOBAL_UNLOCK() TX_RESTORE
#define NANOCLR_STOP()  CPU_Reset();

#define GLOBAL_LOCK_SOCKETS(x)
#define LPCSTR const char *


#define EVENTS_HEART_BEAT
