//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "targetHAL.h"
#include "WireProtocol_ReceiverThread.h"
#include <WireProtocol_Message.h>
#include <tx_api.h>
#include "startup_threads.h"

__attribute__((noreturn)) void ReceiverThread_entry(uint32_t parameter)
{
    (void)parameter;

    // NOTE: Don't call scheduler type calls in this module
    InitWireProtocolCommunications(); 
    tx_thread_sleep(50);

    WP_Message_PrepareReception();
    
    // loop until thread receives a request to terminate
    while (true) 
    {
        WP_Message_Process();
        tx_thread_relinquish();
    }
}

void WP_Message_PrepareReception_Platform()
{
    // empty on purpose, nothing to configure
}
