#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nanoCLR_Types.h"

void CreateCLRThread();
void CLRThread(ULONG parameter);

void CreateReceiverThread();
void ReceiverThread(ULONG parameter);

void CreateAsynchronousIOThread();
void AsynchronousIOThread(ULONG parameter);

