#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "board.h"
#include <stdbool.h>

// TLINK - V3E supports the Virtual COM port(VCP)
// by default.Communication between the target MCU and ST - LINK MCU is enabled on USART1(PA9 / PA10)


void InitWireProtocolCommunications();
bool wp_WriteBytes(uint8_t *ptr, uint16_t size);
int wp_ReadBytes(uint8_t **ptr, uint32_t *size, uint32_t wait_time);
void wp_InitializeUsart(void);
void wp_DataReceived(void);
uint8_t wp_StartTransmitTransfer(void);
