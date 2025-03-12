//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <Debugger.h>
#include <WireProtocol_MonitorCommands.h>
#include "FlashDriver.h"

int AccessMemory(uint32_t location, uint32_t lengthInBytes, uint8_t *buffer, int32_t mode, uint32_t *errorCode)
{
    *errorCode = AccessMemoryErrorCode_NoError;

    switch (mode)
    {
        case AccessMemory_Write:
            return FlashDriver_Write(NULL, location, lengthInBytes, buffer, true);

        case AccessMemory_Erase:
            return FlashDriver_EraseBlock(NULL, location);

        case AccessMemory_Check:
            *(unsigned int *)buffer = SUPPORT_ComputeCRC((uint32_t *)location, lengthInBytes, 0);
            // done here
            return true;

        case AccessMemory_Read:
            FlashDriver_Read(NULL, location, lengthInBytes, buffer);
            return true;

        default:
            *errorCode = AccessMemoryErrorCode_RequestedOperationFailed;
            return false;
    }
}
 int Monitor_Reboot(WP_Message *message)
{
     Monitor_Reboot_Command *cmd = (Monitor_Reboot_Command *)message->m_payload;

     WP_ReplyToCommand(message, true, false, NULL, 0);

     if (cmd != NULL)
     {
         CPU_Reset();
     }
     return true;
 }
