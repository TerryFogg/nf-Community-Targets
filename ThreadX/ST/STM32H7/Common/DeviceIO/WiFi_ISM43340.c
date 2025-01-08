
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "WiFi_ISM43340.h"

SPI_HandleTypeDef hspi3;
UART_HandleTypeDef huart1;
uint8_t txBuffer[TX_BUFFER_SIZE];
uint8_t rxBuffer[RX_BUFFER_SIZE];
WIFI_HandleTypeDef hwifi;
char ssid[] = "HSPP";
char passphrase[] = "michel11";
__IO FlagStatus cmdDataReady = 0;

InitializeWiFi()
{
#pragma region GPIO setup
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, WIFI_RESET_Pin | WIFI_NSS_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : WIFI_RESET_Pin WIFI_NSS_Pin */
    GPIO_InitStruct.Pin = WIFI_RESET_Pin | WIFI_NSS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pin : WIFI_CMD_DATA_READY_Pin */
    GPIO_InitStruct.Pin = WIFI_CMD_DATA_READY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WIFI_CMD_DATA_READY_GPIO_Port, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
#pragma endregion

#pragma region Setup WiFi to MCU interface
    hspi3.Instance = SPI3;
    hspi3.Init.Mode = SPI_MODE_MASTER;
    hspi3.Init.Direction = SPI_DIRECTION_2LINES;
    hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
    hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi3.Init.NSS = SPI_NSS_SOFT;
    hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi3.Init.CRCPolynomial = 7;
    hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(&hspi3) != HAL_OK)
    {
        Error_Handler();
    }

#pragma endregion

#pragma region Setup WiFi to MCU interface
    hwifi.handle = &hspi3;
    hwifi.ssid = ssid;
    hwifi.passphrase = passphrase;
    hwifi.securityType = WPA_MIXED;
    hwifi.DHCP = SET;
    hwifi.ipStatus = IP_V4;
    hwifi.transportProtocol = WIFI_TCP_PROTOCOL;
    hwifi.port = 8080;
    WIFI_Init(&hwifi);
#pragma endregion
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

    if (GPIO_Pin == WIFI_CMD_DATA_READY_Pin)
    {
        cmdDataReady = HAL_GPIO_ReadPin(WIFI_CMD_DATA_READY_GPIO_Port, WIFI_CMD_DATA_READY_Pin);
    }
}

void ISM43340()
{
    WIFI_StatusTypeDef WIFI_SPI_Receive(WIFI_HandleTypeDef * hwifi, char *buffer, uint16_t size)
    {
        uint16_t cnt = 0;
        memset(buffer, '\0', size); // Erase buffer

        while (WIFI_IS_CMDDATA_READY())
        {
            // Fill buffer as long there is still space
            if ((cnt > (size - 2)) ||
                (HAL_SPI_Receive(hwifi->handle, (uint8_t *)buffer + cnt, 1, WIFI_TIMEOUT) != HAL_OK))
            {
                Error_Handler();
            }
            cnt += 2;
        }

        // Trim padding chars from data
        trimstr(buffer, size, (char)WIFI_RX_PADDING);

        return WIFI_OK;
    }
    WIFI_StatusTypeDef WIFI_SPI_Transmit(WIFI_HandleTypeDef * hwifi, char *buffer, uint16_t size)
    {

        char bTx[(size / 2) * 2 + 1]; // Make a buffer that has an even amount of bytes (even is meant for the chars
                                      // excluding \0)
        snprintf(bTx, size, buffer);  // Copy buffer in bTx

        if (!(size % 2))
            strcat(bTx, (char)WIFI_TX_PADDING); // If buffer had an odd amount of bytes, append a filler char to bTx

        if (HAL_SPI_Transmit(hwifi->handle, (uint8_t *)bTx, size / 2, WIFI_TIMEOUT) !=
            HAL_OK) // size must be halved since 16bits are sent via SPI
        {
            Error_Handler();
        }

        return WIFI_OK;
    }
    WIFI_StatusTypeDef WIFI_Init(WIFI_HandleTypeDef * hwifi)
    {

        int msgLength = 0;

        WIFI_RESET_MODULE();
        WIFI_ENABLE_NSS();

        while (!WIFI_IS_CMDDATA_READY())
            ;

        if (WIFI_SPI_Receive(hwifi, wifiRxBuffer, WIFI_RX_BUFFER_SIZE) != WIFI_OK)
            Error_Handler();

        if (strcmp(wifiRxBuffer, WIFI_MSG_POWERUP))
            Error_Handler();

        WIFI_DISABLE_NSS();

        msgLength = sprintf(wifiTxBuffer, "Z3=0\r");
        WIFI_SendATCommand(hwifi, wifiTxBuffer, WIFI_TX_BUFFER_SIZE, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);

        msgLength = sprintf(wifiTxBuffer, "Z0\r");
        WIFI_SendATCommand(hwifi, wifiTxBuffer, WIFI_TX_BUFFER_SIZE, wifiRxBuffer, WIFI_RX_BUFFER_SIZE);
        printf("Answer reset:\n %s", wifiRxBuffer);

        return WIFI_OK;
    }

    WIFI_StatusTypeDef
        WIFI_SendATCommand(WIFI_HandleTypeDef * hwifi, char *bCmd, uint16_t sizeCmd, char *bRx, uint16_t sizeRx)
    {

        while (!WIFI_IS_CMDDATA_READY())
        {
        };
        WIFI_ENABLE_NSS();

        if (WIFI_SPI_Transmit(hwifi, bCmd, sizeCmd) != WIFI_OK)
        {
            Error_Handler();
        }
        WIFI_DISABLE_NSS();

        while (!WIFI_IS_CMDDATA_READY())
        {
        };
        WIFI_ENABLE_NSS();
        if (WIFI_SPI_Receive(hwifi, bRx, sizeRx) != WIFI_OK)
        {
            Error_Handler();
        }
        if (WIFI_IS_CMDDATA_READY())
        {
            // If CMDDATA_READY is still high, then the buffer is too small for the data
            Error_Handler(); 
        }
        WIFI_DISABLE_NSS();
        return WIFI_OK;
    }


}
