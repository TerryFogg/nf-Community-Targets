#pragma once

//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoCLR_Types.h"
#include "nanoCLR_Runtime.h"

bool RelinquishToOtherManagedThreads(CLR_RT_StackFrame &stack, CLR_INT64 timeoutMilliseconds, CLR_INT32 event_type);
