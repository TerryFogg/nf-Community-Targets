//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <WireProtocol.h>
#include <nanoHAL_v2.h>
#include <wp_Communications.h>
#include <tx_api.h>
#include "WireProtocol_HAL_Interface.h"


WP_Message inboundMessage;

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    if (*size != 0) 
    {
        size_t read = wp_ReadBytes(ptr, size, TX_WAIT_FOREVER);
        *ptr += read;
        *size -= read;
    }
    else
    {
        // Do nothing
        // Required for the PING case where payload is 0
    }
}
uint8_t WP_TransmitMessage(WP_Message *message)
{
    wp_WriteBytes((uint8_t *)&message->m_header, sizeof(message->m_header));
    if (message->m_header.m_size && message->m_payload)
    {
        wp_WriteBytes(message->m_payload, message->m_header.m_size);
    }
    return true;
}

void WP_Message_PrepareReception_Platform()
{
    // empty on purpose, nothing to configure
}