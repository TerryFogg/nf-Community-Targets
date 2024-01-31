#include "TestGraphicsU5.h"
#include "board.h"

DMA2D_HandleTypeDef hdma2d;
DSI_HandleTypeDef hdsi;
LTDC_HandleTypeDef hltdc;
LCD_UTILS_Drv_t LCD_Driver;
static uint32_t ImageIndex = 0;
static const uint32_t *Images[] = {
    image_320x240_argb8888,
    (uint32_t *)life_augmented_argb8888,
};

// USER CODE END PV

void MX_GPIO_Init(void);
void MX_DMA2D_Init(void);
void MX_DSIHOST_DSI_Init(void);
void MX_LTDC_Init(void);
uint32_t SetPanelConfig(void);
void CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize);
int32_t LCD_FillRGBRect(
    uint32_t Instance,
    uint32_t Xpos,
    uint32_t Ypos,
    uint8_t *pData,
    uint32_t Width,
    uint32_t Height);
int32_t LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);
int32_t LCD_GetXSize(uint32_t Instance, uint32_t *Xsize);
int32_t LCD_GetYSize(uint32_t Instance, uint32_t *Ysize);
int32_t LCD_GetFormat(uint32_t Instance, uint32_t *Format);

int TestGraphicsU5(void)
{
    MX_GPIO_Init();
    MX_DMA2D_Init();
    MX_DSIHOST_DSI_Init();
    MX_LTDC_Init();

    SetPanelConfig();

    // Prepare area to display on LCD
    // Set UTIL_LCD functions
    LCD_Driver.DrawBitmap = 0;
    LCD_Driver.FillRGBRect = LCD_FillRGBRect;
    LCD_Driver.DrawHLine = 0;
    LCD_Driver.DrawVLine = 0;
    LCD_Driver.FillRect = LCD_FillRect;
    LCD_Driver.GetPixel = 0;
    LCD_Driver.SetPixel = 0;
    LCD_Driver.GetXSize = LCD_GetXSize;
    LCD_Driver.GetYSize = LCD_GetYSize;
    LCD_Driver.SetLayer = 0;
    LCD_Driver.GetFormat = LCD_GetFormat;

    UTIL_LCD_SetFuncDriver(&LCD_Driver);

    // Clear LCD
    UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);

    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetFont(&Font20);

    // Display title
    UTIL_LCD_FillRect(0, 70, LCD_WIDTH, 40, UTIL_LCD_COLOR_BLUE);
    UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"DSI_VideoMode_SingleBuffer", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)" example ", CENTER_MODE);

    while (1)
    {
        CopyBuffer((uint32_t *)Images[ImageIndex++], (uint32_t *)LCD_FRAME_BUFFER, 67, 140, IMAGE_WIDTH, IMAGE_HEIGHT);

        if (ImageIndex >= 2)
        {
            ImageIndex = 0;
        }

        // Wait some time before switching to next stage
        {
            PLATFORM_DELAY(2000);
        }
    }
}
void MX_DMA2D_Init(void)
{
    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
    hdma2d.Init.OutputOffset = 0;
    hdma2d.Init.BytesSwap = DMA2D_BYTES_REGULAR;
    hdma2d.Init.LineOffsetMode = DMA2D_LOM_PIXELS;
    hdma2d.LayerCfg[1].InputOffset = 0;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0xFF;
    hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
    hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_ConfigLayer(&hdma2d, 1);
}

void MX_DSIHOST_DSI_Init(void)
{
    DSI_PLLInitTypeDef PLLInit = {0};
    DSI_HOST_TimeoutTypeDef HostTimeouts = {0};
    DSI_PHY_TimerTypeDef PhyTimings = {0};
    DSI_VidCfgTypeDef VidCfg = {0};

    hdsi.Instance = DSI;
    hdsi.Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;
    hdsi.Init.TXEscapeCkdiv = 4;
    hdsi.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
    hdsi.Init.PHYFrequencyRange = DSI_DPHY_FRANGE_450MHZ_510MHZ;
    hdsi.Init.PHYLowPowerOffset = PHY_LP_OFFSSET_0_CLKP;
    PLLInit.PLLNDIV = 125;
    PLLInit.PLLIDF = DSI_PLL_IN_DIV4;
    PLLInit.PLLODF = DSI_PLL_OUT_DIV2;
    PLLInit.PLLVCORange = DSI_DPHY_VCO_FRANGE_800MHZ_1GHZ;
    PLLInit.PLLChargePump = DSI_PLL_CHARGE_PUMP_2000HZ_4400HZ;
    PLLInit.PLLTuning = DSI_PLL_LOOP_FILTER_2000HZ_4400HZ;
    HAL_DSI_Init(&hdsi, &PLLInit);
    HostTimeouts.TimeoutCkdiv = 1;
    HostTimeouts.HighSpeedTransmissionTimeout = 0;
    HostTimeouts.LowPowerReceptionTimeout = 0;
    HostTimeouts.HighSpeedReadTimeout = 0;
    HostTimeouts.LowPowerReadTimeout = 0;
    HostTimeouts.HighSpeedWriteTimeout = 0;
    HostTimeouts.HighSpeedWritePrespMode = DSI_HS_PM_DISABLE;
    HostTimeouts.LowPowerWriteTimeout = 0;
    HostTimeouts.BTATimeout = 0;
    HAL_DSI_ConfigHostTimeouts(&hdsi, &HostTimeouts);
    PhyTimings.ClockLaneHS2LPTime = 11;
    PhyTimings.ClockLaneLP2HSTime = 40;
    PhyTimings.DataLaneHS2LPTime = 12;
    PhyTimings.DataLaneLP2HSTime = 23;
    PhyTimings.DataLaneMaxReadTime = 0;
    PhyTimings.StopWaitTime = 7;
    HAL_DSI_ConfigPhyTimer(&hdsi, &PhyTimings);
    HAL_DSI_ConfigFlowControl(&hdsi, DSI_FLOW_CONTROL_BTA);
    HAL_DSI_ConfigErrorMonitor(&hdsi, HAL_DSI_ERROR_NONE);
    VidCfg.ColorCoding = DSI_RGB888;
    VidCfg.LooselyPacked = DSI_LOOSELY_PACKED_DISABLE;
    VidCfg.Mode = DSI_VID_MODE_BURST;
    VidCfg.PacketSize = 480;
    VidCfg.NumberOfChunks = 0;
    VidCfg.NullPacketSize = 0;
    VidCfg.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
    VidCfg.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
    VidCfg.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
    VidCfg.HorizontalSyncActive = 6;
    VidCfg.HorizontalBackPorch = 3;
    VidCfg.HorizontalLine = 1452;
    VidCfg.VerticalSyncActive = 1;
    VidCfg.VerticalBackPorch = 12;
    VidCfg.VerticalFrontPorch = 50;
    VidCfg.VerticalActive = 481;
    VidCfg.LPCommandEnable = DSI_LP_COMMAND_DISABLE;
    VidCfg.LPLargestPacketSize = 0;
    VidCfg.LPVACTLargestPacketSize = 0;
    VidCfg.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;
    VidCfg.LPHorizontalBackPorchEnable = DSI_LP_HBP_ENABLE;
    VidCfg.LPVerticalActiveEnable = DSI_LP_VACT_ENABLE;
    VidCfg.LPVerticalFrontPorchEnable = DSI_LP_VFP_ENABLE;
    VidCfg.LPVerticalBackPorchEnable = DSI_LP_VBP_ENABLE;
    VidCfg.LPVerticalSyncActiveEnable = DSI_LP_VSYNC_ENABLE;
    VidCfg.FrameBTAAcknowledgeEnable = DSI_FBTAA_ENABLE;
    HAL_DSI_ConfigVideoMode(&hdsi, &VidCfg);
    HAL_DSI_SetGenericVCID(&hdsi, 0);
}

void MX_LTDC_Init(void)
{
    LTDC_LayerCfgTypeDef pLayerCfg = {0};
    hltdc.Instance = LTDC;
    hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AH;
    hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AH;
    hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hltdc.Init.HorizontalSync = 1;
    hltdc.Init.VerticalSync = 0;
    hltdc.Init.AccumulatedHBP = 2;
    hltdc.Init.AccumulatedVBP = 12;
    hltdc.Init.AccumulatedActiveW = 482;
    hltdc.Init.AccumulatedActiveH = 493;
    hltdc.Init.TotalWidth = 483;
    hltdc.Init.TotalHeigh = 543;
    hltdc.Init.Backcolor.Blue = 0;
    hltdc.Init.Backcolor.Green = 0;
    hltdc.Init.Backcolor.Red = 0;
    HAL_LTDC_Init(&hltdc);
    pLayerCfg.WindowX0 = 0;
    pLayerCfg.WindowX1 = LCD_WIDTH;
    pLayerCfg.WindowY0 = 0;
    pLayerCfg.WindowY1 = LCD_HEIGHT;
    pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    pLayerCfg.Alpha = 0xFF;
    pLayerCfg.Alpha0 = 0;
    pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
    pLayerCfg.FBStartAdress = LCD_FRAME_BUFFER;
    pLayerCfg.ImageWidth = LCD_WIDTH;
    pLayerCfg.ImageHeight = LCD_HEIGHT;
    pLayerCfg.Backcolor.Blue = 0;
    pLayerCfg.Backcolor.Green = 0;
    pLayerCfg.Backcolor.Red = 0;
    HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0);
}

void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // GPIO Ports Clock Enable
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    // Configure GPIO pin Output Level
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);

    // Configure GPIO pin : PD5
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // Configure GPIO pin : PI6
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
}

void CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize)
{

    uint32_t destination = (uint32_t)pDst + (y * 480 + x) * 4;
    uint32_t source = (uint32_t)pSrc;

    hdma2d.Init.OutputOffset = 480 - xsize;
    if (HAL_DMA2D_Init(&hdma2d) == HAL_OK)
    {
        if (HAL_DMA2D_Start(&hdma2d, source, destination, xsize, ysize) == HAL_OK)
        {
            // Polling For DMA transfer
            HAL_DMA2D_PollForTransfer(&hdma2d, 100);
        }
    }
}

uint32_t SetPanelConfig(void)
{

    if (HAL_DSI_Start(&hdsi) != HAL_OK)
        return 1;

    // CMD Mode
    uint8_t InitParam1[3] = {0xFF, 0x83, 0x79};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 3, 0xB9, InitParam1) != HAL_OK)
        return 1;

    // SETPOWER
    uint8_t InitParam3[16] =
        {0x44, 0x1C, 0x1C, 0x37, 0x57, 0x90, 0xD0, 0xE2, 0x58, 0x80, 0x38, 0x38, 0xF8, 0x33, 0x34, 0x42};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 16, 0xB1, InitParam3) != HAL_OK)
        return 2;

    // SETDISP
    uint8_t InitParam4[9] = {0x80, 0x14, 0x0C, 0x30, 0x20, 0x50, 0x11, 0x42, 0x1D};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 9, 0xB2, InitParam4) != HAL_OK)
        return 3;

    // Set display cycle timing
    uint8_t InitParam5[10] = {0x01, 0xAA, 0x01, 0xAF, 0x01, 0xAF, 0x10, 0xEA, 0x1C, 0xEA};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 10, 0xB4, InitParam5) != HAL_OK)
        return 4;

    // SETVCOM
    uint8_t InitParam60[4] = {00, 00, 00, 0xC0};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, 0xC7, InitParam60) != HAL_OK)
        return 5;

    // Set Panel Related Registers
    if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xCC, 0x02) != HAL_OK)
        return 6;

    if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xD2, 0x77) != HAL_OK)
        return 7;

    uint8_t InitParam50[37] = {0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x08, 0x32, 0x10, 0x01, 0x00, 0x01, 0x03,
                               0x72, 0x03, 0x72, 0x00, 0x08, 0x00, 0x08, 0x33, 0x33, 0x05, 0x05, 0x37, 0x05,
                               0x05, 0x37, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x0E};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 37, 0xD3, InitParam50) != HAL_OK)
        return 8;

    uint8_t InitParam51[34] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x18, 0x18,
                               0x18, 0x18, 0x19, 0x19, 0x01, 0x00, 0x03, 0x02, 0x05, 0x04, 0x07, 0x06,
                               0x23, 0x22, 0x21, 0x20, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 34, 0xD5, InitParam51) != HAL_OK)
        return 9;

    uint8_t InitParam52[35] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x18,
                               0x18, 0x19, 0x19, 0x18, 0x18, 0x06, 0x07, 0x04, 0x05, 0x02, 0x03,
                               0x00, 0x01, 0x20, 0x21, 0x22, 0x23, 0x18, 0x18, 0x18, 0x18};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 35, 0xD6, InitParam52) != HAL_OK)
        return 10;

    // SET GAMMA
    uint8_t InitParam8[42] = {0x00, 0x16, 0x1B, 0x30, 0x36, 0x3F, 0x24, 0x40, 0x09, 0x0D, 0x0F, 0x18, 0x0E, 0x11,
                              0x12, 0x11, 0x14, 0x07, 0x12, 0x13, 0x18, 0x00, 0x17, 0x1C, 0x30, 0x36, 0x3F, 0x24,
                              0x40, 0x09, 0x0C, 0x0F, 0x18, 0x0E, 0x11, 0x14, 0x11, 0x12, 0x07, 0x12, 0x14, 0x18};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 42, 0xE0, InitParam8) != HAL_OK)
        return 11;

    uint8_t InitParam44[3] = {0x2C, 0x2C, 00};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 3, 0xB6, InitParam44) != HAL_OK)
        return 12;

    if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xBD, 0x00) != HAL_OK)
        return 13;

    uint8_t InitParam14[] = {0x01, 0x00, 0x07, 0x0F, 0x16, 0x1F, 0x27, 0x30, 0x38, 0x40, 0x47, 0x4E, 0x56, 0x5D, 0x65,
                             0x6D, 0x74, 0x7D, 0x84, 0x8A, 0x90, 0x99, 0xA1, 0xA9, 0xB0, 0xB6, 0xBD, 0xC4, 0xCD, 0xD4,
                             0xDD, 0xE5, 0xEC, 0xF3, 0x36, 0x07, 0x1C, 0xC0, 0x1B, 0x01, 0xF1, 0x34, 0x00};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 42, 0xC1, InitParam14) != HAL_OK)
        return 14;

    if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xBD, 0x01) != HAL_OK)
        return 15;

    uint8_t InitParam15[] = {0x00, 0x08, 0x0F, 0x16, 0x1F, 0x28, 0x31, 0x39, 0x41, 0x48, 0x51, 0x59, 0x60, 0x68,
                             0x70, 0x78, 0x7F, 0x87, 0x8D, 0x94, 0x9C, 0xA3, 0xAB, 0xB3, 0xB9, 0xC1, 0xC8, 0xD0,
                             0xD8, 0xE0, 0xE8, 0xEE, 0xF5, 0x3B, 0x1A, 0xB6, 0xA0, 0x07, 0x45, 0xC5, 0x37, 0x00};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 42, 0xC1, InitParam15) != HAL_OK)
        return 16;

    if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xBD, 0x02) != HAL_OK)
        return 17;

    uint8_t InitParam20[42] = {0x00, 0x09, 0x0F, 0x18, 0x21, 0x2A, 0x34, 0x3C, 0x45, 0x4C, 0x56, 0x5E, 0x66, 0x6E,
                               0x76, 0x7E, 0x87, 0x8E, 0x95, 0x9D, 0xA6, 0xAF, 0xB7, 0xBD, 0xC5, 0xCE, 0xD5, 0xDF,
                               0xE7, 0xEE, 0xF4, 0xFA, 0xFF, 0x0C, 0x31, 0x83, 0x3C, 0x5B, 0x56, 0x1E, 0x5A, 0xFF};
    if (HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 42, 0xC1, InitParam20) != HAL_OK)
        return 18;

    if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xBD, 0x00) != HAL_OK)
        return 19;

    // Exit Sleep Mode
    if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P0, 0x11, 0x00) != HAL_OK)
        return 20;

    PLATFORM_DELAY(120);

    // Clear LCD_FRAME_BUFFER
    memset((uint32_t *)LCD_FRAME_BUFFER, 0x00, 0xFFFFF);

    // Display On
    if (HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P0, 0x29, 0x00) != HAL_OK)
        return 21;

    PLATFORM_DELAY(120);

    // All setting OK
    return 0;
}

int32_t LCD_FillRGBRect(
    uint32_t Instance,
    uint32_t Xpos,
    uint32_t Ypos,
    uint8_t *pData,
    uint32_t Width,
    uint32_t Height)
{
    uint32_t Xaddress = 0;
    uint32_t StartAddress = 0;
    uint32_t i;
    uint32_t j;

    // Set the start address
    StartAddress = (hltdc.LayerCfg[0].FBStartAdress + (4 * (Ypos * 480 + Xpos)));

    // Fill the rectangle
    for (i = 0; i < Height; i++)
    {
        Xaddress = StartAddress + (480 * 4 * i);
        for (j = 0; j < Width; j++)
        {
            *(__IO uint32_t *)(Xaddress) = *(uint32_t *)pData;
            pData += 4;
            Xaddress += 4;
        }
    }

    return 0;
}

int32_t LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
    uint32_t Xaddress = 0;
    uint32_t Startaddress = 0;
    uint32_t i;
    uint32_t j;

    // Get the rectangle start address
    Startaddress = (hltdc.LayerCfg[0].FBStartAdress + (4 * (Ypos * 480 + Xpos)));

    // Fill the rectangle
    for (i = 0; i < Height; i++)
    {
        Xaddress = Startaddress + (480 * 4 * i);
        for (j = 0; j < Width; j++)
        {
            *(__IO uint32_t *)(Xaddress) = Color;
            Xaddress += 4;
        }
    }

    return 0;
}

int32_t LCD_GetXSize(uint32_t Instance, uint32_t *Xsize)
{
    // Get the display Xsize
    *Xsize = 480U;
    return 0;
}

int32_t LCD_GetYSize(uint32_t Instance, uint32_t *Ysize)
{
    // Get the display Ysize
    *Ysize = 480U;
    return 0;
}

int32_t LCD_GetFormat(uint32_t Instance, uint32_t *Format)
{
    // Get pixel format supported by LCD
    *Format = LTDC_PIXEL_FORMAT_ARGB8888;
    return 0;
}

