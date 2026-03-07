#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nanoCLR_Types.h"

void CreateCLRThread();
void CLRThread_Entry(uint32_t parameter);

void CreateReceiverThread();
void ReceiverThread_entry(uint32_t parameter);

void CreateNetworkThread();
void NetworkThread_entry(uint32_t parameter);

void CreateAsynchronousIOThread();
void DeviceIOAsynchronousThread_Entry(uint32_t parameter);

