//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

/*

   _STM32U5xx________________STLINK_3E_________      ___PC____
  |   __________________   ________            |    |         |
  |  |     USART       |   | USART |           |    |         |
  |  | (wpUSART_TX_PIN)|---|-RX    |           |    | Virtual |
  |  |                 |   |       | usb_dev_hs|<==>| Com     |
  |  | (wpUSART_RX_PIN)|---|-TX    |           |    | Port    |
  |  |_________________|   |_______|           |    |         |
  |____________________|   |___________________|    |_________|

*/

#include "wp_Communications.h"
#include "CircularBuffer.h"
#include <assert.h>
#include <tx_api.h>
#include "board.h"

// USART receive buffer for DMA - make sure it is in RAM accessible by the DMA controller used.
// Also, check alignment
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32)))
uint8_t wpUSART_DMA_Receive_Buffer[wpUSART_DMA_Receive_Buffer_size];
static_assert((sizeof(wpUSART_DMA_Receive_Buffer) % 32) == 0, "Must be a multiple by 32");

CircularBuffer_t ReceiveCircularBuffer;
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32))) uint8_t wp_ReceiveData[2048];

CircularBuffer_t TransmitCircularBuffer;
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32))) uint8_t wp_TransmitData[2048];

//  Length of currently active TX DMA transfer
volatile size_t usart_tx_dma_current_len;

TX_EVENT_FLAGS_GROUP wpReceivedBytesEvent;

void InitWireProtocolCommunications()
{
    wp_InitializeUsart();

    wp_InitializeBuffer(&TransmitCircularBuffer, wp_TransmitData, sizeof(wp_TransmitData));
    wp_InitializeBuffer(&ReceiveCircularBuffer, wp_ReceiveData, sizeof(wp_ReceiveData));

    // Create event based data synchronization
    tx_event_flags_create(&wpReceivedBytesEvent, "wpReceiveDataEvent");
}
void wp_InitializeUsart(void)
{
#pragma region Setup GPIO used by the USART
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    wpUSART_GPIO_PERIPHERAL_CLOCK_ENABLE;
    GPIO_InitStruct.Pin = wpUSART_TX_PIN | wpUSART_RX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(wpUSART_GPIO_PORT, &GPIO_InitStruct);
#pragma endregion

#pragma region Setup USART
    // Configure WireProtocol wpUSART
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_USART_Disable(wpUSART);
    {
        wpUSART_PERIPHERAL_CLOCK_ENABLE;
        USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
        USART_InitStruct.BaudRate = wpBAUD_RATE;
        USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
        USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
        USART_InitStruct.Parity = LL_USART_PARITY_NONE;
        USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
        USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
        USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
        LL_USART_DisableOneBitSamp(wpUSART);
        LL_USART_Init(wpUSART, &USART_InitStruct);
        LL_USART_SetTXFIFOThreshold(wpUSART, LL_USART_FIFOTHRESHOLD_7_8);
        LL_USART_SetRXFIFOThreshold(wpUSART, LL_USART_FIFOTHRESHOLD_7_8);
        LL_USART_EnableFIFO(wpUSART);
        LL_USART_ConfigAsyncMode(wpUSART);
        LL_USART_EnableDMAReq_RX(wpUSART);
        LL_USART_EnableDMAReq_TX(wpUSART);
        LL_USART_EnableIT_IDLE(wpUSART);
        NVIC_SetPriority(wpUSART_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(wpUSART_IRQn);
        //LL_USART_EnableIT_TC(wpUSART);
        LL_USART_EnableDMAReq_TX(wpUSART);
#pragma endregion

#pragma region Setup UART DMA functionality
        LL_DMA_InitTypeDef dmaReceive = {0};
        LL_DMA_InitTypeDef dmaTransmit = {0};

        // Enable GPDMA1 Clock
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPDMA1);

        LL_DMA_StructInit(&dmaReceive);
        dmaReceive.Request = LL_GPDMA1_REQUEST_USART1_RX;
        dmaReceive.SrcAddress = LL_USART_DMA_GetRegAddr(wpUSART, LL_USART_DMA_REG_DATA_RECEIVE);
        dmaReceive.DestAddress = (uint32_t)wpUSART_DMA_Receive_Buffer;
        dmaReceive.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
        dmaReceive.SrcIncMode = LL_DMA_SRC_FIXED;
        dmaReceive.DestIncMode = LL_DMA_SRC_INCREMENT;
        dmaReceive.Priority = LL_DMA_HIGH_PRIORITY;
        LL_DMA_Init(wpGPDMA, wpGDMA_RECEIVE_CHANNEL, &dmaReceive);
        LL_DMA_EnableChannelPrivilege(wpGPDMA, LL_DMA_CHANNEL_1);

        LL_DMA_StructInit(&dmaTransmit);
        dmaTransmit.Request = LL_GPDMA1_REQUEST_USART1_TX;
        dmaTransmit.SrcAddress = (uint32_t)&wpUSART_DMA_Receive_Buffer;
        dmaTransmit.DestAddress = LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_TRANSMIT);
        dmaTransmit.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
        dmaTransmit.SrcIncMode = LL_DMA_SRC_INCREMENT;
        dmaTransmit.DestIncMode = LL_DMA_DEST_FIXED;
        LL_DMA_Init(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL, &dmaTransmit);
        LL_DMA_EnableChannelPrivilege(wpGPDMA, LL_DMA_CHANNEL_0);

        NVIC_SetPriority(wpGPDMA_TRANSMIT_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(wpGPDMA_TRANSMIT_IRQ);
        NVIC_SetPriority(wpGPDMA_RECEIVE_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(wpGPDMA_RECEIVE_IRQ);
        // Enable transmit transfer complete interrupt
        LL_DMA_EnableIT_TC(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL);
        LL_DMA_EnableChannel(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL);

        // Enable the following interrupts for receive
        LL_DMA_EnableIT_HT(wpGPDMA, wpGDMA_RECEIVE_CHANNEL);
        LL_DMA_EnableIT_TC(wpGPDMA, wpGDMA_RECEIVE_CHANNEL);
        LL_DMA_EnableIT_USE(wpGPDMA, wpGDMA_RECEIVE_CHANNEL);
        LL_DMA_EnableIT_ULE(wpGPDMA, wpGDMA_RECEIVE_CHANNEL);
        LL_DMA_EnableIT_DTE(wpGPDMA, wpGDMA_RECEIVE_CHANNEL);
        LL_DMA_EnableChannel(wpGPDMA, wpGDMA_RECEIVE_CHANNEL);
#pragma endregion
    }
    LL_USART_Enable(wpUSART);
    // Wait for UART initialization to complete
    while (!LL_USART_IsActiveFlag_TEACK(wpUSART) || !LL_USART_IsActiveFlag_REACK(wpUSART))
    {
    }
}
int wp_ReadBytes(uint8_t **ptr, uint32_t *size, uint32_t wait_time)
{
    ULONG actual_flags;
    uint32_t requestedSize = *size;
    tx_event_flags_get(&wpReceivedBytesEvent, 0x1, TX_OR_CLEAR, &actual_flags, wait_time);
    ULONG read = wp_ReadBuffer(&ReceiveCircularBuffer, *ptr, requestedSize);
    return read;
}
bool wp_WriteBytes(uint8_t *ptr, uint16_t size)
{
    // SCB_CleanInvalidateDCache();
    wp_WriteBuffer(&TransmitCircularBuffer, ptr, size);
    //  SCB_CleanInvalidateDCache();
    wp_StartTransmitTransfer();
    return true;
}
void wp_DataReceived(void)
{
    static size_t old_position;

    // Calculate current position in buffer and check for new data available

    int length_data_received = LL_DMA_GetBlkDataLength(wpGPDMA, wpGDMA_RECEIVE_CHANNEL);
    //    int length_data_received = LL_DMA_GetDataLength(wpGPDMA, wpDMA_ReceiveChannel);
    size_t position = ARRAY_LEN(wpUSART_DMA_Receive_Buffer) - length_data_received;

    if (position != old_position)
    {
        // SCB_CleanInvalidateDCache_by_Addr((uint32_t *)&wpUSART_DMA_Receive_Buffer,
        // sizeof(wpUSART_DMA_Receive_Buffer));

        if (position > old_position)
        {
            wp_WriteBuffer(&ReceiveCircularBuffer, &wpUSART_DMA_Receive_Buffer[old_position], position - old_position);
        }
        else
        {
            // Processing is done in "overflow" mode..
            wp_WriteBuffer(
                &ReceiveCircularBuffer,
                &wpUSART_DMA_Receive_Buffer[old_position],
                ARRAY_LEN(wpUSART_DMA_Receive_Buffer) - old_position);

            if (position > 0)
            {
                wp_WriteBuffer(&ReceiveCircularBuffer, &wpUSART_DMA_Receive_Buffer[0], position);
            }
        }
        old_position = position;
        tx_event_flags_set(&wpReceivedBytesEvent, 0x1, TX_OR);
    }
}
uint8_t wp_StartTransmitTransfer(void)
{
    uint8_t started = 0;
    size_t length;
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    // Use temporary values in case they are changed during operations
    size_t w = TransmitCircularBuffer.w;
    size_t r = TransmitCircularBuffer.r;
    if (w > r)
    {
        length = w - r;
    }
    else if (r > w)
    {
        length = TransmitCircularBuffer.size - r;
    }
    else
    {
        length = 0;
    }

    if (usart_tx_dma_current_len == 0 && (usart_tx_dma_current_len = length) > 0)
    {
        // Wait for any previous DMA to finish
        // Wait for any previous DMA / stream 1 transfers to complete
        while (LL_DMA_IsActiveFlag_TC(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL))
        {
        };
        // Disable channel if enabled
        LL_DMA_DisableChannel(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL);

        // Clear all flags
        LL_DMA_ClearFlag_TC(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL);
        LL_DMA_ClearFlag_HT(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL);

        // Prepare DMA data and length
        LL_DMA_SetBlkDataLength(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL, usart_tx_dma_current_len);
        LL_DMA_SetSrcAddress(
            wpGPDMA,
            wpGDMA_TRANSMIT_CHANNEL,
            (uint32_t)&TransmitCircularBuffer.buffer[TransmitCircularBuffer.r]);

        LL_DMA_EnableChannel(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL);
        started = 1;
    }
    __set_PRIMASK(primask);
    return started;
}
void wpDMA_ReceiveChannel_IRQHandler()
{
    // Check half-transfer complete interrupt
    if (LL_DMA_IsEnabledIT_HT(wpGPDMA, wpGDMA_RECEIVE_CHANNEL) &&
        LL_DMA_IsActiveFlag_HT(wpGPDMA, wpGDMA_RECEIVE_CHANNEL))
    {
        LL_DMA_ClearFlag_HT(wpGPDMA, wpGDMA_RECEIVE_CHANNEL);
        wp_DataReceived();
    }

    // Check transfer-complete interrupt
    if (LL_DMA_IsEnabledIT_TC(wpGPDMA, wpGDMA_RECEIVE_CHANNEL) &&
        LL_DMA_IsActiveFlag_TC(wpGPDMA, wpGDMA_RECEIVE_CHANNEL))
    {
        LL_DMA_ClearFlag_TC(wpGPDMA, wpGDMA_RECEIVE_CHANNEL);
        wp_DataReceived();
    }
}
void wpDMA_TransmitChannel_IRQHandler()
{
    if (LL_DMA_IsEnabledIT_TC(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL) &&
        LL_DMA_IsActiveFlag_TC(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL))
    {
        LL_DMA_ClearFlag_TC(wpGPDMA, wpGDMA_TRANSMIT_CHANNEL);

        size_t number_bytes_waiting = wp_BufferBytesWaiting(&TransmitCircularBuffer);
        TransmitCircularBuffer.r += BUF_MIN(usart_tx_dma_current_len, number_bytes_waiting);
        if (TransmitCircularBuffer.r >= TransmitCircularBuffer.size)
        {
            TransmitCircularBuffer.r -= TransmitCircularBuffer.size;
        }
        usart_tx_dma_current_len = 0;
        wp_StartTransmitTransfer();
    }
}
void wpUSART_IRQHANDLER()
{

    if (LL_USART_IsActiveFlag_PE(wpUSART) || LL_USART_IsActiveFlag_FE(wpUSART) || LL_USART_IsActiveFlag_NE(wpUSART) ||
        LL_USART_IsActiveFlag_ORE(wpUSART) || LL_USART_IsActiveFlag_PE(wpUSART))
    {
        // Clear Parity,Framing,Noise,Overrun errors
        LL_USART_ClearFlag_PE(wpUSART);
        LL_USART_ClearFlag_FE(wpUSART);
        LL_USART_ClearFlag_NE(wpUSART);
        LL_USART_ClearFlag_ORE(wpUSART);

        // Initialize Circular buffer for Receive after an error
        wp_InitializeBuffer(&ReceiveCircularBuffer, wp_ReceiveData, sizeof(wp_ReceiveData));
    }
    // Check for IDLE line interrupt
    else if (LL_USART_IsActiveFlag_IDLE(wpUSART))
    {
        LL_USART_ClearFlag_IDLE(wpUSART);
        wp_DataReceived();
    }
}
