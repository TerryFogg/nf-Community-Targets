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

#define wpUSART_DMA_Receive_Buffer_size 2048

#define wpBAUD_RATE 921600

#define wpUSART                         USART1
#define wpUSART_IRQn                    USART1_IRQn
#define wpUSART_IRQHANDLER()            void USART1_IRQHandler(void)
#define wpUSART_PERIPHERAL_CLOCK_ENABLE LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)

#define wpUSART_GPIO_PORT                    GPIOA
#define wpUSART_RX_PIN                       LL_GPIO_PIN_10
#define wpUSART_TX_PIN                       LL_GPIO_PIN_9
#define wpUSART_GPIO_PERIPHERAL_CLOCK_ENABLE ENABLE_PORT_GPIOA
#define wpDMA                                GPDMA1

#define wpDMA_ReceiveStreamInterrupt        DMA1_Stream0_IRQn
#define wpDMA_TransmitStreamInterrupt       DMA1_Stream1_IRQn
#define wpUSART_DMA_PERIPHERAL_CLOCK_ENABLE LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)

#define wpDMA_ReceiveChannel             LL_DMA_CHANNEL_0
#define wpDMA_ReceiveStream_IRQHandler() void DMA1_Stream0_IRQHandler(void)

#define wpDMA_TransmitChannel             LL_DMA_CHANNEL_1
#define wpDMA_TransmitStream_IRQHandler() void DMA1_Stream1_IRQHandler(void)

void InitWireProtocolCommunications();
bool wp_WriteBytes(uint8_t *ptr, uint16_t size);
int wp_ReadBytes(uint8_t **ptr, uint32_t *size, uint32_t wait_time);
void wp_InitializeUsart(void);
void wp_DataReceived(void);
uint8_t wp_StartTransmitTransfer(void);
