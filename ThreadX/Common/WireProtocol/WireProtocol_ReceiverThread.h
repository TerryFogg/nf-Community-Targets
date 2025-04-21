#pragma once

//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Declaration of RTOS thread
void ReceiverThread(void const *argument);

#ifdef __cplusplus
extern "C"
{
#endif
    void InitWireProtocolCommunications();
#ifdef __cplusplus
}
#endif
