//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "ManagedThreadSupport.h"

bool RelinquishToOtherManagedThreads(CLR_RT_StackFrame &stack, CLR_INT64 timeoutMilliseconds, CLR_INT32 event_type)
{
    CLR_INT64 *timeout;
    bool eventResult = true;

    CLR_RT_HeapBlock hbTimeout;
    hbTimeout.SetInteger(timeoutMilliseconds * TIME_CONVERSION__TO_MILLISECONDS);
    stack.SetupTimeoutFromTicks(hbTimeout, timeout);

    // Wait here until native work completes and relinquish cpu to other c# threads
    while (eventResult)
    {
        g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, event_type, eventResult);
    }
    // Pop timeout heap block from stack
    stack.PopValue();
    return true;
}
