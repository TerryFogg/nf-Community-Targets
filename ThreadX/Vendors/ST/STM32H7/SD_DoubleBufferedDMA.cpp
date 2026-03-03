//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// No general propose DMA Configuration is needed, an Internal DMA for SDMMC Peripheral are used.

#include "SD_DoubleBufferedDMA.h"

extern Device::SDPin *SDChannels;
void SD_IRQHandler(int SDBUS_index);

const int blockSize = 512;
const int number_of_blocks = 4;
const uint32_t bufferSize = number_of_blocks * blockSize;

// SD1
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32))) uint32_t SD1DmaReadWriteBuffer0[bufferSize];
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32))) uint32_t SD1DmaReadWriteBuffer1[bufferSize];
uint32_t *pSD1DataBuffer0 = &SD1DmaReadWriteBuffer0[0];
uint32_t *pSD1DataBuffer1 = &SD1DmaReadWriteBuffer1[0];
static_assert((sizeof(SD1DmaReadWriteBuffer0) % 512) == 0, "Must be a multiple by 512");
static_assert((sizeof(SD1DmaReadWriteBuffer1) % 512) == 0, "Must be a multiple by 512");

// SD2
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32))) uint32_t SD2DmaReadWriteBuffer0[bufferSize];
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32))) uint32_t SD2DmaReadWriteBuffer1[bufferSize];
uint32_t *pSD2DataBuffer0 = &SD2DmaReadWriteBuffer0[0];
uint32_t *pSD2DataBuffer1 = &SD2DmaReadWriteBuffer1[0];
static_assert((sizeof(SD2DmaReadWriteBuffer0) % 512) == 0, "Must be a multiple by 512");
static_assert((sizeof(SD2DmaReadWriteBuffer1) % 512) == 0, "Must be a multiple by 512");

void InitializeSD_Bus(
    int SDBus_index,
    PinNameValue SDMMC_D0,
    PinNameValue SDMMC_D1,
    PinNameValue SDMMC_D2,
    PinNameValue SDMMC_D3,
    PinNameValue SDMMC_CLK,
    PinNameValue SDMMC_CMD,
    PinNameValue SDMMC_DETECT)
{
    SDMMC_TypeDef *SDMMC=NULL;
    SDMMC_InitTypeDef SDMMCInit;

    Device::SetPinFunction(SDMMC_D0, DevicePinFunction::SD);
    Device::SetPinFunction(SDMMC_D1, DevicePinFunction::SD);
    Device::SetPinFunction(SDMMC_D2, DevicePinFunction::SD);
    Device::SetPinFunction(SDMMC_D3, DevicePinFunction::SD);
    Device::SetPinFunction(SDMMC_CLK, DevicePinFunction::SD);
    Device::SetPinFunction(SDMMC_CMD, DevicePinFunction::SD);
    Device::SetPinFunction(SDMMC_DETECT, DevicePinFunction::GPIO);

    switch (SDBus_index)
    {
        case 1:
            SDMMC = SDMMC1;
            LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_SDMMC1);
            SDMMCInit = {
                .ClockEdge = SDMMC_CLOCK_EDGE_RISING,
                .ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE,
                .BusWide = SDMMC_BUS_WIDE_4B,
                .HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE,
                .ClockDiv = 0};
            SDMMC->IDMABASE0 = (uint32_t)pSD1DataBuffer0;
            SDMMC->IDMABASE1 = (uint32_t)pSD1DataBuffer1;
            SDMMC->IDMABSIZE = bufferSize;
            NVIC_SetPriority(SDMMC1_IRQn, 0);
            NVIC_EnableIRQ(SDMMC1_IRQn);
            break;
        case 2:
            // TODO - Test SDMMC2 to be done on a supported platform
            SDMMC = SDMMC2;
            LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_SDMMC2);
            SDMMCInit = {
                .ClockEdge = SDMMC_CLOCK_EDGE_RISING,
                .ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE,
                .BusWide = SDMMC_BUS_WIDE_4B,
                .HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE,
                .ClockDiv = 0};
            SDMMC->IDMABASE0 = (uint32_t)pSD2DataBuffer0;
            SDMMC->IDMABASE1 = (uint32_t)pSD2DataBuffer1;
            SDMMC->IDMABSIZE = bufferSize;
            NVIC_SetPriority(SDMMC2_IRQn, 0);
            NVIC_EnableIRQ(SDMMC2_IRQn);
            break;
    }
    SDMMC_Init(SDMMC, SDMMCInit);
    SDMMC_PowerState_ON(SDMMC);

    //  At this stage, you can perform SD read/write/erase operations after SD card initialization
    return;
}
void SDMMC1_IRQHandler(void)
{
    SD_IRQHandler(0);
}
void SDMMC2_IRQHandler(void)
{
    SD_IRQHandler(1);
}
File_Status STM32ErrorToFileStatus(uint32_t status)
{
    File_Status return_value = File_Status::FILE_ERROR;
    switch (status)
    {
        case SDMMC_ERROR_NONE:
            return_value = File_Status::FILE_SUCCESS;
            break;
        case SDMMC_ERROR_ADDR_OUT_OF_RANGE:
        case SDMMC_ERROR_BLOCK_LEN_ERR:
            return_value = File_Status::FILE_ADDRESS_OUT_OF_RANGE;
            break;
        case SDMMC_ERROR_ADDR_MISALIGNED:
        case SDMMC_ERROR_ERASE_SEQ_ERR:
        case SDMMC_ERROR_AKE_SEQ_ERR:
        case SDMMC_ERROR_CC_ERR:
        case SDMMC_ERROR_GENERAL_UNKNOWN_ERR:
        case SDMMC_ERROR_ILLEGAL_CMD:
            return_value = File_Status::FILE_ERROR;
            break;
        case SDMMC_ERROR_CARD_ECC_DISABLED:
        case SDMMC_ERROR_CARD_ECC_FAILED:
        case SDMMC_ERROR_CMD_CRC_FAIL:
        case SDMMC_ERROR_COM_CRC_FAILED:
            return_value = File_Status::FILE_DATA_CRC_FAIL;
            break;
        case SDMMC_ERROR_CID_CSD_OVERWRITE:
        case SDMMC_ERROR_CMD_RSP_TIMEOUT:
            return_value = File_Status::FILE_DATA_TIMEOUT;
            break;
        case SDMMC_ERROR_ERASE_RESET:
        case SDMMC_ERROR_WP_ERASE_SKIP:
        case SDMMC_ERROR_BAD_ERASE_PARAM:
            return_value = File_Status::FILE_ERASE_ERROR;
            break;
        case SDMMC_ERROR_STREAM_READ_UNDERRUN:
            return_value = File_Status::FILE_UNDERRUN;
            break;
        case SDMMC_ERROR_STREAM_WRITE_OVERRUN:
            return_value = File_Status::FILE_OVERRUN;
            break;
        case SDMMC_ERROR_LOCK_UNLOCK_FAILED:
        case SDMMC_ERROR_WRITE_PROT_VIOLATION:
            return_value = File_Status::FILE_WRITE_PROTECTED;
            break;
        default:
            return_value = File_Status::FILE_ERROR;
            break;
    }
    return return_value;
}
bool GetCardState(int SDBus_index)
{
    uint32_t resp1 = 0;
    uint32_t errorstate = 0;
    const int SD_CARD_TRANSFER = 4;

    SDMMC_TypeDef *SDMMCx = (SDMMC_TypeDef *)SDChannels[SDBus_index].controllerId;
    if ((errorstate = SDMMC_CmdSendStatus(SDMMCx, resp1)) != SDMMC_ERROR_NONE)
    {
        SDChannels[SDBus_index].status = File_Status::FILE_ERROR;
        return false;
    }
    uint32_t cardstate = ((resp1 >> 9U) & 0x0FU);
    return cardstate == SD_CARD_TRANSFER ? true : false;
}
void SD_TxCpltCallback(int SDBUS_index)
{

    tx_semaphore_put(&SDChannels[SDBUS_index].RX_semaphore);
}
void SD_RxCpltCallback(int SDBUS_index)
{
    tx_semaphore_put(&SDChannels[SDBUS_index].TX_semaphore);
}
void SDRead_DMACallback(int SDBUS_index, int bufferIndex)
{
    UNUSED(SDBUS_index);
    UNUSED(bufferIndex);
}
void SDWrite_DMACallback(int SDBUS_index, int bufferIndex)
{
    UNUSED(SDBUS_index);
    UNUSED(bufferIndex);
}
bool ReadSD(int SDBUS_index, SD_Card_Type card_type, int total_blocks, int start_block, int number_of_blocks)
{
    uint32_t stm32Error;
    SDMMC_TypeDef *SDMMCx = (SDMMC_TypeDef *)SDChannels[SDBUS_index].controllerId;

    if (start_block + number_of_blocks > total_blocks)
    {
        SDChannels[SDBUS_index].status = File_Status::FILE_ADDRESS_OUT_OF_RANGE;
        return false;
    }
    if (SDMMCx->IDMABSIZE == 0U || SDMMCx->IDMABASE0 == 0U || SDMMCx->IDMABASE1 == 0U)
    {
        SDChannels[SDBUS_index].status = File_Status::FILE_ERROR;
        return false;
    }

    // Initialize data control register
    SDMMCx->DCTRL = 0U;
    __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_DATA_FLAGS);

    if (card_type != SD_Card_Type::CARD_SDHC_SDXC)
    {
        start_block *= 512U;
    }
    // Configure the SD DPSM (Data Path State Machine)
    SDMMC_DataInitTypeDef config = {
        .DataTimeOut = SDMMC_DATATIMEOUT,
        .DataLength = (uint32_t)number_of_blocks * FILE_DEFAULT_SECTOR_SIZE,
        .DataBlockSize = SDMMC_DATABLOCK_SIZE_512B,
        .TransferDir = SDMMC_TRANSFER_DIR_TO_SDMMC,
        .TransferMode = SDMMC_TRANSFER_MODE_BLOCK,
        .DPSM = SDMMC_DPSM_DISABLE};
    (void)SDMMC_ConfigData(SDMMCx, &config);

    SDMMCx->DCTRL |= SDMMC_DCTRL_FIFORST;

    __SDMMC_CMDTRANS_ENABLE(SDMMCx);

    SDMMCx->IDMACTRL = SDMMC_ENABLE_IDMA_DOUBLE_BUFF0;

    // Read Multi Block command
    SDChannels[SDBUS_index].readType =
        (File_Read_Type)(File_Read_Type::FILE_READ_MULTIPLE_BLOCK | File_Read_Type::FILE_DMA);
    stm32Error = SDMMC_CmdReadMultiBlock(SDMMCx, start_block);

    SDChannels[SDBUS_index].status = STM32ErrorToFileStatus(stm32Error);
    if (SDChannels[SDBUS_index].status != File_Status::FILE_SUCCESS)
    {
        __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_FLAGS);
        SDChannels[SDBUS_index].readType = File_Read_Type::FILE_NONE;
        return false;
    }

    // Enable transfer interrupts
    __SDMMC_ENABLE_IT(
        SDMMCx,
        (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_RXOVERR | SDMMC_IT_DATAEND | SDMMC_IT_IDMABTC));
    SDChannels[SDBUS_index].status = File_Status::FILE_SUCCESS;
    return true;
}
bool WriteSD(int SDBUS_index, SD_Card_Type card_type, int total_blocks, int start_block, int number_of_blocks)
{
    uint32_t add = start_block;
    uint32_t stm32Error;
    SDMMC_TypeDef *SDMMCx = (SDMMC_TypeDef *)SDChannels[SDBUS_index].controllerId;

    // Write block(s) from a specified address in a card. The Data transfer is managed by DMA mode.
    if ((start_block + number_of_blocks) > total_blocks)
    {
        SDChannels[SDBUS_index].status = File_Status::FILE_ADDRESS_OUT_OF_RANGE;
        return false;
    }

    if (SDMMCx->IDMABSIZE == 0U || SDMMCx->IDMABASE0 == 0U || SDMMCx->IDMABASE1 == 0U)
    {
        SDChannels[SDBUS_index].status = File_Status::FILE_ERROR;
        return false;
    }

    // Initialize data control register
    SDMMCx->DCTRL = 0U;
    if (card_type != SD_Card_Type::CARD_SDHC_SDXC)
    {
        add *= 512U;
    }
    // Configure the SD DPSM (Data Path State Machine)
    SDMMC_DataInitTypeDef config = {
        .DataTimeOut = SDMMC_DATATIMEOUT,
        .DataLength = (uint32_t)number_of_blocks * FILE_DEFAULT_SECTOR_SIZE,
        .DataBlockSize = SDMMC_DATABLOCK_SIZE_512B,
        .TransferDir = SDMMC_TRANSFER_DIR_TO_CARD,
        .TransferMode = SDMMC_TRANSFER_MODE_BLOCK,
        .DPSM = SDMMC_DPSM_DISABLE};
    (void)SDMMC_ConfigData(SDMMCx, &config);

    __SDMMC_CMDTRANS_ENABLE(SDMMCx);

    SDMMCx->IDMACTRL = SDMMC_ENABLE_IDMA_DOUBLE_BUFF0;

    /* Write Blocks in DMA Mode */
    SDChannels[SDBUS_index].readType =
        (File_Read_Type)(File_Read_Type::FILE_WRITE_MULTIPLE_BLOCK | File_Read_Type::FILE_DMA);
    stm32Error = SDMMC_CmdWriteMultiBlock(SDMMCx, add);

    if ((SDChannels[SDBUS_index].status = STM32ErrorToFileStatus(stm32Error)) != File_Status::FILE_SUCCESS)
    {
        __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_FLAGS);
        SDChannels[SDBUS_index].readType = File_Read_Type::FILE_NONE;
        return false;
    }
    __SDMMC_ENABLE_IT(
        SDMMCx,
        (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR | SDMMC_IT_DATAEND | SDMMC_IT_IDMABTC));
    SDChannels[SDBUS_index].status = File_Status::FILE_SUCCESS;
    return true;
}
void SD_IRQHandler(int SDBUS_index)
{
    SDMMC_TypeDef *SDMMCx = (SDMMC_TypeDef *)SDChannels[SDBUS_index].controllerId;
    uint32_t stm32Error = 0;
    File_Read_Type current_read_type = SDChannels[SDBUS_index].readType;

    // Data end (data counter, SDIDCOUNT, is zero)
    if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_DATAEND) != 0)
    {
        __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_DATAEND);
        __SDMMC_DISABLE_IT(
            SDMMCx,
            SDMMC_IT_DATAEND | SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR | SDMMC_IT_RXOVERR |
                SDMMC_IT_TXFIFOHE | SDMMC_IT_RXFIFOHF);
        __SDMMC_DISABLE_IT(SDMMCx, SDMMC_IT_IDMABTC);
        __SDMMC_CMDTRANS_DISABLE(SDMMCx);

        if ((current_read_type & File_Read_Type::FILE_DMA) != 0U)
        {
            SDMMCx->DLEN = 0;
            SDMMCx->DCTRL = 0;
            SDMMCx->IDMACTRL = SDMMC_DISABLE_IDMA;
            // Stop Transfer for Write Multi blocks or Read Multi blocks
            if (((current_read_type & File_Read_Type::FILE_READ_MULTIPLE_BLOCK) != 0U) ||
                ((current_read_type & File_Read_Type::FILE_WRITE_MULTIPLE_BLOCK) != 0U))
            {
                stm32Error = SDMMC_CmdStopTransfer(SDMMCx);
                if ((SDChannels[SDBUS_index].status = STM32ErrorToFileStatus(stm32Error)) == File_Status::FILE_SUCCESS)
                {
                    return;
                }
            }
            SDChannels[SDBUS_index].readType = File_Read_Type::FILE_NONE;

            if (((current_read_type & File_Read_Type::FILE_WRITE_MULTIPLE_BLOCK) != 0U))
            {
                SD_TxCpltCallback(SDBUS_index);
            }
            if (((current_read_type & File_Read_Type::FILE_READ_MULTIPLE_BLOCK) != 0U))
            {
                SD_RxCpltCallback(SDBUS_index);
            }
        }
        else
        {
            // Nothing to do
        }
    }
    // Check if there is an SD error
    else if (
        __SDMMC_GET_FLAG(
            SDMMCx,
            SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_RXOVERR | SDMMC_FLAG_TXUNDERR) != 0)
    {
        // Data CRC Fail Interrupt Enable
        if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_IT_DCRCFAIL) != 0)
        {
            SDChannels[SDBUS_index].status = File_Status::FILE_DATA_CRC_FAIL;
        }
        // Data TimeOut Interrupt Enable
        if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_IT_DTIMEOUT) != 0)
        {
            SDChannels[SDBUS_index].status = File_Status::FILE_DATA_TIMEOUT;
        }
        // Receive FIFO overrun
        if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_IT_RXOVERR) != 0)
        {
            SDChannels[SDBUS_index].status = File_Status::FILE_OVERRUN;
        }
        // Transmit FIFO underrun
        if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_IT_TXUNDERR) != 0)
        {
            SDChannels[SDBUS_index].status = File_Status::FILE_UNDERRUN;
        }

        // Clear All flags, disable all interrupts
        __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_DATA_FLAGS);
        __SDMMC_DISABLE_IT(
            SDMMCx,
            SDMMC_IT_DATAEND | SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR | SDMMC_IT_RXOVERR);

        __SDMMC_CMDTRANS_DISABLE(SDMMCx);
        SDMMCx->DCTRL |= SDMMC_DCTRL_FIFORST;
        SDMMCx->CMD |= SDMMC_CMD_CMDSTOP;
        stm32Error |= SDMMC_CmdStopTransfer(SDMMCx);

        SDMMCx->CMD &= ~(SDMMC_CMD_CMDSTOP);
        __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_DABORT);
        if ((current_read_type & File_Read_Type::FILE_DMA) != 0U)
        {
            if ((SDChannels[SDBUS_index].status = STM32ErrorToFileStatus(stm32Error)) != File_Status::FILE_SUCCESS)
                // Disable Internal DMA
                __SDMMC_DISABLE_IT(SDMMCx, SDMMC_IT_IDMABTC);
            SDMMCx->IDMACTRL = SDMMC_DISABLE_IDMA;
            return;
        }
        else
        {
            // Do Nothing
        }
    }
    // DMA buffer transfer complete
    else if (__SDMMC_GET_FLAG(SDMMCx, SDMMC_FLAG_IDMABTC) != 0)
    {
        __SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_IDMABTC);
        if (READ_BIT(SDMMCx->IDMACTRL, SDMMC_IDMA_IDMABACT) == 0U)
        {
            // Current buffer is buffer0, Transfer complete for buffer1
            if ((SDChannels[SDBUS_index].readType & File_Read_Type::FILE_READ_MULTIPLE_BLOCK) == 0U)
            {
                SDRead_DMACallback(SDBUS_index, 1);
            }
            else
            {
                SDWrite_DMACallback(SDBUS_index, 1);
            }
        }
        else
        {
            // Current buffer is buffer1, Transfer complete for buffer0
            if ((SDChannels[SDBUS_index].readType & File_Read_Type::FILE_READ_MULTIPLE_BLOCK) == 0U)
            {
                SDRead_DMACallback(SDBUS_index, 0);
            }
            else
            {
                SDWrite_DMACallback(SDBUS_index, 0);
            }
        }
    }
}

/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    (void)GPIO_Pin;
    // ULONG s_msg = CARD_STATUS_CHANGED;

    // if (GPIO_Pin == SD_DETECT_Pin)
    //{
    //     tx_queue_send(&tx_msg_queue, &s_msg, TX_NO_WAIT);
    // }
}
