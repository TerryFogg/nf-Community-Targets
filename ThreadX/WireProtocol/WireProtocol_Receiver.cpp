//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <WireProtocol_ReceiverThread.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>

void ReceiverThread(ULONG parameter)
{
    (void)parameter;
    extern WP_Message inboundMessage;

    InitWireProtocolCommunications();
    PLATFORM_DELAY(50);

    WP_Message_Initialize(&inboundMessage);
    WP_Message_PrepareReception();

    // loop until thread receives a request to terminate
    while (true)
    {
        WP_Message_Process();
        tx_thread_relinquish();
    }
}
