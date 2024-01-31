#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include <stdbool.h>
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_dma.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_usart.h"


void InitWireProtocolCommunications();
bool wp_WriteBytes(uint8_t *ptr, uint16_t size);
int wp_ReadBytes(uint8_t **ptr, uint32_t *size, uint32_t wait_time);
void wp_InitializeUsart(void);
void wp_DataReceived(void);
uint8_t wp_StartTransmitTransfer(void);
