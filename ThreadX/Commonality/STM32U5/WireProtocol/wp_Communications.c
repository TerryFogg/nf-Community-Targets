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
#include "wp_CircularBuffer.h"
#include <assert.h>
#include <tx_api.h>

// USART receive buffer for DMA - make sure it is in RAM accessible by the DMA controller used.
// Also, check alignment
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32)))
uint8_t wpUSART_DMA_Receive_Buffer[wpUSART_DMA_Receive_Buffer_size];
static_assert((sizeof(wpUSART_DMA_Receive_Buffer) % 32) == 0, "Must be a multiple by 32");

CircularBuffer_t ReceiveCircularBuffer;
uint8_t wp_ReceiveData[2048];

CircularBuffer_t TransmitCircularBuffer;
uint8_t wp_TransmitData[2048];

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
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Peripheral clock enable
    wpUSART_PERIPHERAL_CLOCK_ENABLE;
    wpUSART_GPIO_PERIPHERAL_CLOCK_ENABLE;
    wpUSART_DMA_PERIPHERAL_CLOCK_ENABLE;

    // UART TX/RX GPIO pin configuration and clock
    GPIO_InitStruct.Pin = wpUSART_TX_PIN | wpUSART_RX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // wpUSART Receive Initialize
    LL_DMA_SetPeriphRequest(wpDMA, wpDMA_ReceiveChannel, LL_GPDMA1_REQUEST_USART1_RX);
    LL_DMA_SetPeriphRequest(wpDMA, wpDMA_TransmitChannel, LL_GPDMA1_REQUEST_USART1_TX);


    LL_DMA_InitTypeDef dmaReceive;
    dmaReceive.SrcAddress = LL_USART_DMA_GetRegAddr(wpUSART, LL_USART_DMA_REG_DATA_RECEIVE);
    dmaReceive.DestAddress = &wpUSART_DMA_Receive_Buffer;
    dmaReceive.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    dmaReceive.BlkHWRequest = LL_DMA_HWREQUEST_SINGLEBURST;
    dmaReceive.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD; //?
    dmaReceive.SrcBurstLength = 1;                         // ?
    dmaReceive.DestBurstLength = 1;                        // ?
    dmaReceive.SrcDataWidth = LL_DMA_SRC_DATAWIDTH_BYTE;
    dmaReceive.DestDataWidth = LL_DMA_SRC_DATAWIDTH_BYTE;
    dmaReceive.SrcIncMode = LL_DMA_SRC_FIXED;
    dmaReceive.DestIncMode = LL_DMA_SRC_INCREMENT;
    dmaReceive.Priority = LL_DMA_HIGH_PRIORITY;
    dmaReceive.BlkDataLength = 0; //?
    dmaReceive.BlkRptCount = 0;
    dmaReceive.TriggerMode = LL_DMA_TRIGM_BLK_TRANSFER;         //?
    dmaReceive.TriggerPolarity = LL_DMA_TRIG_POLARITY_FALLING;  //?
    dmaReceive.TriggerSelection = LL_GPDMA1_TRIGGER_EXTI_LINE0; //?
    dmaReceive.Request = LL_GPDMA1_REQUEST_USART1_RX;
    dmaReceive.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;      //?
    dmaReceive.DestHWordExchange = LL_DMA_DEST_HALFWORD_PRESERVE; //?
    dmaReceive.DestByteExchange = LL_DMA_DEST_BYTE_PRESERVE;
    dmaReceive.SrcByteExchange = LL_DMA_SRC_BYTE_PRESERVE;
    dmaReceive.SrcAllocatedPort = LL_DMA_SRC_ALLOCATED_PORT0;         //?
    dmaReceive.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0;       //?
    dmaReceive.LinkAllocatedPort = LL_DMA_LINK_ALLOCATED_PORT0;       //?
    dmaReceive.LinkStepMode = LL_DMA_LSM_FULL_EXECUTION;              //?
    dmaReceive.SrcAddrUpdateMode = LL_DMA_BURST_SRC_ADDR_INCREMENT;   //?
    dmaReceive.DestAddrUpdateMode = LL_DMA_BURST_DEST_ADDR_INCREMENT; //?
    dmaReceive.SrcAddrOffset = 0;                                     //  only for 2D addressing channels
    dmaReceive.DestAddrOffset = 0;                                    //  only for 2D addressing channels
    dmaReceive.BlkRptSrcAddrUpdateMode = 0;                           //  only for 2D addressing channels
    dmaReceive.BlkRptDestAddrUpdateMode = 0;                          //  only for 2D addressing channels
    dmaReceive.BlkRptSrcAddrOffset = 0;                               //  only for 2D addressing channels
    dmaReceive.BlkRptDestAddrOffset = 0;                              //  only for 2D addressing channels
    dmaReceive.LinkedListBaseAddr = LL_DMA_CHANNEL_0;
    dmaReceive.LinkedListAddrOffset = 0;
    LL_DMA_Init(wpDMA, wpDMA_ReceiveChannel, &dmaReceive);


    LL_DMA_InitTypeDef dmaTransmit;
    dmaTransmit.SrcAddress = LL_USART_DMA_GetRegAddr(wpUSART, LL_USART_DMA_REG_DATA_RECEIVE);
    dmaTransmit.DestAddress = &wpUSART_DMA_Receive_Buffer;
    dmaTransmit.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dmaTransmit.BlkHWRequest = LL_DMA_HWREQUEST_SINGLEBURST;
    dmaTransmit.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD; //?
    dmaTransmit.SrcBurstLength = 1;                         // ?
    dmaTransmit.DestBurstLength = 1;                        // ?
    dmaTransmit.SrcDataWidth = LL_DMA_SRC_DATAWIDTH_BYTE;
    dmaTransmit.DestDataWidth = LL_DMA_SRC_DATAWIDTH_BYTE;
    dmaTransmit.SrcIncMode = LL_DMA_SRC_INCREMENT;
    dmaTransmit.DestIncMode = LL_DMA_SRC_FIXED;
    dmaTransmit.Priority = LL_DMA_HIGH_PRIORITY;
    dmaTransmit.BlkDataLength = 0; //?
    dmaTransmit.BlkRptCount = 0;
    dmaTransmit.TriggerMode = LL_DMA_TRIGM_BLK_TRANSFER;         //?
    dmaTransmit.TriggerPolarity = LL_DMA_TRIG_POLARITY_FALLING;  //?
    dmaTransmit.TriggerSelection = LL_GPDMA1_TRIGGER_EXTI_LINE0; //?
    dmaTransmit.Request = LL_GPDMA1_REQUEST_USART1_RX;
    dmaTransmit.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;      //?
    dmaTransmit.DestHWordExchange = LL_DMA_DEST_HALFWORD_PRESERVE; //?
    dmaTransmit.DestByteExchange = LL_DMA_DEST_BYTE_PRESERVE;
    dmaTransmit.SrcByteExchange = LL_DMA_SRC_BYTE_PRESERVE;
    dmaTransmit.SrcAllocatedPort = LL_DMA_SRC_ALLOCATED_PORT0;         //?
    dmaTransmit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0;       //?
    dmaTransmit.LinkAllocatedPort = LL_DMA_LINK_ALLOCATED_PORT0;       //?
    dmaTransmit.LinkStepMode = LL_DMA_LSM_FULL_EXECUTION;              //?
    dmaTransmit.SrcAddrUpdateMode = LL_DMA_BURST_SRC_ADDR_INCREMENT;   //?
    dmaTransmit.DestAddrUpdateMode = LL_DMA_BURST_DEST_ADDR_INCREMENT; //?
    dmaTransmit.SrcAddrOffset = 0;                                     //  only for 2D addressing channels
    dmaTransmit.DestAddrOffset = 0;                                    //  only for 2D addressing channels
    dmaTransmit.BlkRptSrcAddrUpdateMode = 0;                           //  only for 2D addressing channels
    dmaTransmit.BlkRptDestAddrUpdateMode = 0;                          //  only for 2D addressing channels
    dmaTransmit.BlkRptSrcAddrOffset = 0;                               //  only for 2D addressing channels
    dmaTransmit.BlkRptDestAddrOffset = 0;                              //  only for 2D addressing channels
    dmaTransmit.LinkedListBaseAddr = LL_DMA_CHANNEL_0;
    dmaTransmit.LinkedListAddrOffset = 0;
    LL_DMA_Init(wpDMA, wpDMA_ReceiveChannel, &dmaTransmit);

    // Enable receive half transfer interrupt
    LL_DMA_EnableIT_HT(wpDMA, wpDMA_ReceiveChannel);

    // Enable receive transfer complete interrupt
    LL_DMA_EnableIT_TC(wpDMA, wpDMA_ReceiveChannel);

    // Enable transmit transfer complete interrupt
    LL_DMA_EnableIT_TC(wpDMA, wpDMA_TransmitChannel);

    // Configure WireProtocol wpUSART
    USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
    USART_InitStruct.BaudRate = wpBAUD_RATE;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

    // Required to make changes, LL_USART_Init doesn't work unless disabled
    LL_USART_Disable(wpUSART);
    LL_USART_Init(wpUSART, &USART_InitStruct);
    LL_USART_SetTXFIFOThreshold(wpUSART, LL_USART_FIFOTHRESHOLD_7_8);
    LL_USART_SetRXFIFOThreshold(wpUSART, LL_USART_FIFOTHRESHOLD_7_8);
    LL_USART_EnableFIFO(wpUSART);
    LL_USART_ConfigAsyncMode(wpUSART);
    LL_USART_EnableDMAReq_RX(wpUSART);
    LL_USART_EnableDMAReq_TX(wpUSART);
    LL_USART_EnableIT_IDLE(wpUSART);

    // USART interrupt, same priority as DMA channel
    NVIC_SetPriority(wpUSART_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(wpUSART_IRQn);

    LL_DMA_EnableStream(wpDMA, wpDMA_ReceiveChannel);
    LL_USART_Enable(wpUSART);

    // Polling wpUSART initialization
    while (!LL_USART_IsActiveFlag_TEACK(wpUSART) || !LL_USART_IsActiveFlag_REACK(wpUSART))
    {
    }
}
int wp_ReadBytes(uint8_t **ptr, uint32_t *size, uint32_t wait_time)
{
    ULONG actual_flags;
    uint32_t requestedSize = *size;
    tx_event_flags_get(&wpReceivedBytesEvent, 0x1, TX_OR_CLEAR, &actual_flags, wait_time);

    // Bytes have arrived try to read what was requested
    ULONG read = wp_ReadBuffer(&ReceiveCircularBuffer, *ptr, requestedSize);

    return read;
}
bool wp_WriteBytes(uint8_t *ptr, uint16_t size)
{
    SCB_CleanInvalidateDCache();
    wp_WriteBuffer(&TransmitCircularBuffer, ptr, size);
    SCB_CleanInvalidateDCache();

    wp_StartTransmitTransfer();
    return true;
}
void wp_DataReceived(void)
{
    static size_t old_position;

    // Calculate current position in buffer and check for new data available
    int length_data_received = LL_DMA_GetDataLength(wpDMA, wpDMA_ReceiveChannel);
    size_t position = ARRAY_LEN(wpUSART_DMA_Receive_Buffer) - length_data_received;

    if (position != old_position)
    {
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)&wpUSART_DMA_Receive_Buffer, sizeof(wpUSART_DMA_Receive_Buffer));

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
        while (LL_DMA_IsActiveFlag_TC1(wpDMA))
        {
        };
        // Disable channel if enabled
        LL_DMA_DisableStream(wpDMA, wpDMA_TransmitChannel);

        // Clear all flags
        LL_DMA_ClearFlag_TC1(wpDMA);
        LL_DMA_ClearFlag_HT1(wpDMA);
        LL_DMA_ClearFlag_TE1(wpDMA);
        LL_DMA_ClearFlag_DME1(wpDMA);
        LL_DMA_ClearFlag_FE1(wpDMA);

        // Prepare DMA data and length
        LL_DMA_SetDataLength(wpDMA, wpDMA_TransmitChannel, usart_tx_dma_current_len);
        LL_DMA_SetMemoryAddress(
            wpDMA,
            wpDMA_TransmitChannel,
            (uint32_t)&TransmitCircularBuffer.buffer[TransmitCircularBuffer.r]);

        LL_DMA_EnableStream(wpDMA, wpDMA_TransmitChannel);
        started = 1;
    }
    __set_PRIMASK(primask);
    return started;
}
wpDMA_ReceiveStream_IRQHandler()
{
    // Check half-transfer complete interrupt
    if (LL_DMA_IsEnabledIT_HT(wpDMA, wpDMA_ReceiveChannel) && LL_DMA_IsActiveFlag_HT0(wpDMA))
    {
        LL_DMA_ClearFlag_HT0(wpDMA);
        wp_DataReceived();
    }

    // Check transfer-complete interrupt
    if (LL_DMA_IsEnabledIT_TC(wpDMA, wpDMA_ReceiveChannel) && LL_DMA_IsActiveFlag_TC0(wpDMA))
    {
        LL_DMA_ClearFlag_TC0(wpDMA);
        wp_DataReceived();
    }
}
wpDMA_TransmitStream_IRQHandler()
{
    if (LL_DMA_IsEnabledIT_TC(wpDMA, wpDMA_TransmitChannel) && LL_DMA_IsActiveFlag_TC1(wpDMA))
    {
        LL_DMA_ClearFlag_TC1(wpDMA);

        size_t number_bytes_waiting = wp_BufferBytesWaiting(&TransmitCircularBuffer);
        TransmitCircularBuffer.r += BUF_MIN(usart_tx_dma_current_len, number_bytes_waiting);
        if (TransmitCircularBuffer.r >= TransmitCircularBuffer.size)
        {
            TransmitCircularBuffer.r -= TransmitCircularBuffer.size;
        }

        // Clear length variable
        usart_tx_dma_current_len = 0;

        // Start sending more data
        wp_StartTransmitTransfer();
    }
}
wpUSART_IRQHANDLER()
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

