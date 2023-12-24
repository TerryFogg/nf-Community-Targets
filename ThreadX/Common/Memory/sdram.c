

#define SDRAM_INSTANCES_NBR 1U
#define SDRAM_DEVICE_ADDR   0xD0000000U
#define SDRAM_DEVICE_SIZE   0x800000U

/* MDMA definitions for SDRAM DMA transfer */
#define SDRAM_MDMAx_CLK_ENABLE  __HAL_RCC_MDMA_CLK_ENABLE
#define SDRAM_MDMAx_CLK_DISABLE __HAL_RCC_MDMA_CLK_DISABLE
#define SDRAM_MDMAx_CHANNEL     MDMA_Channel0
#define SDRAM_MDMAx_IRQn        MDMA_IRQn
#define SDRAM_MDMA_IRQHandler   MDMA_IRQHandler

/**
 * @brief  FMC SDRAM Mode definition register defines
 */
#define SDRAM_MODEREG_BURST_LENGTH_1             0x0000U
#define SDRAM_MODEREG_BURST_LENGTH_2             0x0001U
#define SDRAM_MODEREG_BURST_LENGTH_4             0x0002U
#define SDRAM_MODEREG_BURST_LENGTH_8             0x0004U
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      0x0000U
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     0x0008U
#define SDRAM_MODEREG_CAS_LATENCY_2              0x0020U
#define SDRAM_MODEREG_CAS_LATENCY_3              0x0030U
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    0x0000U
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED 0x0000U
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     0x0200U


int32_t BSP_SDRAM_Init(uint32_t Instance);
int32_t BSP_SDRAM_SendCmd(uint32_t Instance, FMC_SDRAM_CommandTypeDef *SdramCmd);
HAL_StatusTypeDef MX_SDRAM_BANK2_Init(SDRAM_HandleTypeDef *hsdram, uint32_t RowBitsNumber, uint32_t MemoryDataWidth);
void BSP_SDRAM_IRQHandler(uint32_t Instance);
static void SDRAM_MspInit(SDRAM_HandleTypeDef *hSdram);

int32_t BSP_SDRAM_Init(uint32_t Instance)
{
    int32_t ret;
    static IS42S16800J_Context_t pRegMode;
    uint32_t row_bits_num, mem_bus_width;

    if (Instance >= SDRAM_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else
    {
        /* Msp SDRAM initialization */
        SDRAM_MspInit(&hsdram[Instance]);

        row_bits_num = FMC_SDRAM_ROW_BITS_NUM_12;
        mem_bus_width = FMC_SDRAM_MEM_BUS_WIDTH_16;

        if (MX_SDRAM_BANK2_Init(&hsdram[Instance], row_bits_num, mem_bus_width) != HAL_OK)
        {
            ret = BSP_ERROR_NO_INIT;
        }
        else
        {
            /* External memory mode register configuration */
            pRegMode.TargetBank = FMC_SDRAM_CMD_TARGET_BANK2;
            pRegMode.RefreshMode = IS42S16800J_AUTOREFRESH_MODE_CMD;
            pRegMode.RefreshRate = REFRESH_COUNT;
            pRegMode.BurstLength = IS42S16800J_BURST_LENGTH_1;
            pRegMode.BurstType = IS42S16800J_BURST_TYPE_SEQUENTIAL;
            pRegMode.CASLatency = IS42S16800J_CAS_LATENCY_2;
            pRegMode.OperationMode = IS42S16800J_OPERATING_MODE_STANDARD;
            pRegMode.WriteBurstMode = IS42S16800J_WRITEBURST_MODE_SINGLE;

            /* SDRAM initialization sequence */
            if (IS42S16800J_Init(&hsdram[Instance], &pRegMode) != IS42S16800J_OK)
            {
                ret = BSP_ERROR_COMPONENT_FAILURE;
            }
            else
            {
                ret = BSP_ERROR_NONE;
            }
        }
    }

    return ret;
}
__weak HAL_StatusTypeDef
    MX_SDRAM_BANK2_Init(SDRAM_HandleTypeDef *hSdram, uint32_t RowBitsNumber, uint32_t MemoryDataWidth)
{
    FMC_SDRAM_TimingTypeDef sdram_timing;

    /* SDRAM device configuration */
    hsdram->Instance = FMC_SDRAM_DEVICE;

    /* SDRAM handle configuration */
    hSdram->Init.SDBank = FMC_SDRAM_BANK2;
    hSdram->Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
    hSdram->Init.RowBitsNumber = RowBitsNumber;
    hSdram->Init.MemoryDataWidth = MemoryDataWidth;
    hSdram->Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hSdram->Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
    hSdram->Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hSdram->Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_3;
    hSdram->Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    hSdram->Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_2;

    /* tick = 1/(HCLK/3) = 1/93,3Mhz = 10 ns */
    /* Timing configuration for as SDRAM */
    sdram_timing.LoadToActiveDelay = 2;    /* Load Mode Register to Active  */
    sdram_timing.ExitSelfRefreshDelay = 7; /* Exit Self-refresh delay       */
    sdram_timing.SelfRefreshTime = 4;      /* Self refresh time             */
    sdram_timing.RowCycleDelay = 7;        /* Row cycle delay               */
    sdram_timing.WriteRecoveryTime = 2;    /* Recovery delay                */
    sdram_timing.RPDelay = 2;              /* Row precharge delay           */
    sdram_timing.RCDDelay = 2;             /* Row to column delay           */

    /* SDRAM controller initialization */
    if (HAL_SDRAM_Init(hSdram, &sdram_timing) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}
int32_t BSP_SDRAM_SendCmd(uint32_t Instance, FMC_SDRAM_CommandTypeDef *SdramCmd)
{
    int32_t ret;

    if (Instance >= SDRAM_INSTANCES_NBR)
    {
        ret = BSP_ERROR_WRONG_PARAM;
    }
    else if (IS42S16800J_Sendcmd(&hsdram[Instance], SdramCmd) != IS42S16800J_OK)
    {
        ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
        ret = BSP_ERROR_NONE;
    }

    return ret;
}
void BSP_SDRAM_IRQHandler(uint32_t Instance)
{
    HAL_MDMA_IRQHandler(hsdram[Instance].hmdma);
}
static void SDRAM_MspInit(SDRAM_HandleTypeDef *hSdram)
{
    static MDMA_HandleTypeDef mdma_handle;
    GPIO_InitTypeDef gpio_init_structure;

    /* Enable FMC clock */
    __HAL_RCC_FMC_CLK_ENABLE();

    /* Enable chosen DMAx clock */
    SDRAM_MDMAx_CLK_ENABLE();

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    /* Common GPIO configuration */
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF12_FMC;

    /* GPIOD configuration */
    gpio_init_structure.Pin =
        GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &gpio_init_structure);

    /* GPIOE configuration */
    gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                              GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);

    /* GPIOF configuration */
    gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                              GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &gpio_init_structure);

    /* GPIOG configuration */
    gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &gpio_init_structure);

    /* GPIOH configuration */
    gpio_init_structure.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOH, &gpio_init_structure);

    /* Configure common MDMA parameters */
    mdma_handle.Init.Request = MDMA_REQUEST_SW;
    mdma_handle.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
    mdma_handle.Init.Priority = MDMA_PRIORITY_HIGH;
    mdma_handle.Init.SourceInc = MDMA_SRC_INC_WORD;
    mdma_handle.Init.DestinationInc = MDMA_DEST_INC_WORD;
    mdma_handle.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
    mdma_handle.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
    mdma_handle.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
    mdma_handle.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
    mdma_handle.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
    mdma_handle.Init.BufferTransferLength = 128;
    mdma_handle.Init.SourceBlockAddressOffset = 0;
    mdma_handle.Init.DestBlockAddressOffset = 0;

    mdma_handle.Instance = SDRAM_MDMAx_CHANNEL;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hSdram, hmdma, mdma_handle);

    /* De-initialize the Stream for new transfer */
    (void)HAL_MDMA_DeInit(&mdma_handle);

    /* Configure the DMA Stream */
    (void)HAL_MDMA_Init(&mdma_handle);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(SDRAM_MDMAx_IRQn, BSP_SDRAM_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(SDRAM_MDMAx_IRQn);
}
