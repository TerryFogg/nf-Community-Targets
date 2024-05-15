
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2021 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.

#include <stdarg.h>
#include <stdio.h>
#include "DisplayInterface.h"
#include "board.h"

#define DSI_TIMEOUT_VALUE ((uint32_t)100U)

#define LCD_WIDTH              480
#define LCD_HEIGHT             481
#define LCD_FRAME_BUFFER       0x200D0000
#define UTIL_LCD_COLOR_YELLOW  0xFFFFFF00UL
#define UTIL_LCD_COLOR_DARKRED 0xFF800000UL
#define UTIL_LCD_COLOR_BLACK   0xFF000000UL
#define UTIL_LCD_COLOR_BROWN   0xFFA52A2AUL
#define UTIL_LCD_COLOR_ORANGE  0xFFFFA500UL

#define LCD_COLOR_RGB565_BLACK 0x0000U
#define LCD_COLOR_RGB565_WHITE 0x1010U

#define DSI_DPHY_FRANGE_450MHZ_510MHZ   0x00000008U
#define DSI_DPHY_SLEW_HS_TX_SPEED       0x0000000EU
#define DSI_DPHY_VCO_FRANGE_800MHZ_1GHZ 0x00000001U
#define PHY_LP_OFFSSET_0_CLKP           0x00000000U

#define DSI_PLL_LOOP_FILTER_2000HZ_4400HZ 0x00000000U
#define DSI_PLL_CHARGE_PUMP_2000HZ_4400HZ 0x00000000U
#define DSI_HS_PREPARE_OFFSET2            0x00000002U
#define DSI_HS_PM_DISABLE                 0x00000000U
#define DSI_AUTO_CLK_LANE_CTRL_DISABLE    0x00000000U
#define DSI_TWO_DATA_LANES                1U
#define DSI_VID_MODE_BURST                2U
#define DSI_PLL_OUT_DIV2                  0x00000002U
#define DSI_PLL_IN_DIV4                   0x00000004U
#define DSI_RGB565                        0x00000000U

#define DSI_HSYNC_ACTIVE_HIGH       0x00000000U
#define DSI_VSYNC_ACTIVE_HIGH       0x00000000U
#define DSI_LP_COMMAND_DISABLE      0x00000000U
#define DSI_DATA_ENABLE_ACTIVE_HIGH 0x00000000U

#define LTDC_DEPOLARITY_AL       0x00000000U
#define LTDC_PCPOLARITY_IPC      0x00000000U
#define LTDC_BLENDING_FACTOR1_CA 0x00000400U
#define LTDC_BLENDING_FACTOR2_CA 0x00000005U
#define LTDC_PIXEL_FORMAT_RGB565 0x00000002U

#define DSI_DCS_SHORT_PKT_WRITE_P0 0x00000005U
#define DSI_DCS_SHORT_PKT_WRITE_P1 0x00000015U
#define DSI_DCS_LONG_PKT_WRITE     0x00000039U

DSI_TypeDef *DsiRegister = DSI;
LTDC_TypeDef *LtdcRegister = LTDC;
LTDC_Layer_TypeDef *LTDC_Layer_Register0 = (LTDC_Layer_TypeDef *)((uint32_t)LTDC + 0x84U);

struct DisplayInterface g_DisplayInterface;
// Frame buffer set externally
extern CLR_UINT32 Graphics_frame_buffer;

CLR_UINT32 Width;
CLR_UINT32 Height;

void DisplayInterface::Initialize(DisplayInterfaceConfig &config)
{
    // Base address of DSI,LTDC and LTDC Layer Host/Wrapper registers
    uint32_t FBStartAdress = LCD_FRAME_BUFFER;
    uint32_t TimeoutCkdiv = 1;
    uint32_t ClockLaneLP2HSTime = 40;
    uint32_t DataLaneHS2LPTime = 12;
    uint32_t DataLaneLP2HSTime = 23;
    uint32_t StopWaitTime = 7;
    uint32_t PacketSize = 480;
    uint32_t HorizontalSync = 1;
    uint32_t HorizontalLine = 1452;
    uint32_t HorizontalSyncActive = 6;
    uint32_t VerticalSyncActive = 1;
    uint32_t VerticalActive = 481;
    uint32_t VerticalSync = 0;
    uint32_t AccumulatedHBP = 2;
    uint32_t AccumulatedVBP = 12;
    uint32_t AccumulatedActiveW = 482;
    uint32_t AccumulatedActiveH = 493;
    uint32_t HorizontalBackPorch = 3;
    uint32_t VerticalBackPorch = 12;
    uint32_t VerticalFrontPorch = 50;

    uint32_t PLLNDIV = 125;
    uint32_t TotalWidth = 483;
    uint32_t TotalHeight = 543;
    uint32_t WindowX1 = LCD_WIDTH;
    uint32_t WindowY1 = LCD_HEIGHT;
    uint32_t ImageWidth = LCD_WIDTH;
    uint32_t ImageHeight = LCD_HEIGHT;

    uint32_t PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    uint32_t Alpha = 0xFF;
    uint32_t Blue = 0;
    uint32_t Green = 0;
    uint32_t Red = 0;

    Width = config.Screen.width;
    Height = config.Screen.height;

#pragma region Control Lines
    ENABLE_CLOCK_ON_PORT_GPIOD;
    ENABLE_CLOCK_ON_PORT_GPIOH;
    ENABLE_CLOCK_ON_PORT_GPIOI;

    LL_GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = DSI_RESET_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(DSI_RESET_PIN_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DSI_BackLight_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(DSI_BackLight_PIN_PORT, &GPIO_InitStruct);

    LL_GPIO_ResetOutputPin(DSI_RESET_PIN_PORT, DSI_RESET_PIN);
#pragma endregion

#pragma region DSI Setup

    // Disable DSI PLL and DSI
    CLEAR_BIT(DsiRegister->WRPCR, DSI_WRPCR_PLLEN);
    CLEAR_BIT(DsiRegister->CR, DSI_CR_EN);
    {

        // Enable the DSI clock
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_DSI);

        // Enable the DSI host
        SET_BIT(DsiRegister->CR, DSI_CR_EN);

        // Enable the DSI PLL
        SET_BIT(DsiRegister->WRPCR, DSI_WRPCR_PLLEN);

        DelayMilliseconds(1);

        // Enable the clock lane and the digital section of the D-PHY
        DsiRegister->PCTLR |= (DSI_PCTLR_CKE | DSI_PCTLR_DEN);

        // Set the TX escape clock division factor
        DsiRegister->CCR = 4;

        DelayMilliseconds(1);

        // Setup Dsi Clock source
        LL_RCC_SetDSIClockSource(LL_RCC_DSI_CLKSOURCE_PHY);

        DelayMilliseconds(11);
        LL_GPIO_SetOutputPin(DSI_RESET_PIN_PORT, DSI_RESET_PIN);
        DelayMilliseconds(150);

        DsiRegister->CCR &= ~DSI_CCR_TXECKDIV;

        // Bias power up
        DsiRegister->BCFGR |= DSI_BCFGR_PWRUP;

        // Requires min of 2ms delay
        DelayMilliseconds(2);

        // Set the PLL division factors
        DsiRegister->WRPCR &= ~(DSI_WRPCR_PLL_NDIV | DSI_WRPCR_PLL_IDF | DSI_WRPCR_PLL_ODF);
        DsiRegister->WRPCR |=
            ((PLLNDIV << DSI_WRPCR_PLL_NDIV_Pos) | (DSI_PLL_IN_DIV4 << DSI_WRPCR_PLL_IDF_Pos) |
             (DSI_PLL_OUT_DIV2 << DSI_WRPCR_PLL_ODF_Pos));

        // Set Band Control Frequency for clock lane
        DsiRegister->DPCBCR &= ~DSI_DPCBCR;
        DsiRegister->DPCBCR |= (DSI_DPHY_FRANGE_450MHZ_510MHZ << DSI_DPCBCR_Pos);

        // Set the slew rate for clock lane
        DsiRegister->DPCSRCR = DSI_DPHY_SLEW_HS_TX_SPEED;

        // Set Band Control Frequency for Data Lane0
        DsiRegister->DPDL0BCR &= ~DSI_DPDL0BCR;
        DsiRegister->DPDL0BCR = (DSI_DPHY_FRANGE_450MHZ_510MHZ << DSI_DPDL0BCR_Pos);

        // Set the slew rate for data Lane0
        DsiRegister->DPDL0SRCR = DSI_DPHY_SLEW_HS_TX_SPEED;

        // Set Band Control Frequency for Data Lane1
        DsiRegister->DPDL1BCR &= ~DSI_DPDL1BCR;
        DsiRegister->DPDL1BCR = (DSI_DPHY_FRANGE_450MHZ_510MHZ << DSI_DPDL1BCR_Pos);

        // Set Slew rate for data Lane1
        DsiRegister->DPDL1SRCR = DSI_DPHY_SLEW_HS_TX_SPEED;

        // Set HS prepare offset for data lanes
        DsiRegister->DPDL0HSOCR &= ~DSI_DPDL0HSOCR;
        DsiRegister->DPDL1HSOCR &= ~DSI_DPDL1HSOCR;

        DsiRegister->DPDL0HSOCR = (DSI_HS_PREPARE_OFFSET2 << DSI_DPDL0HSOCR_Pos);
        DsiRegister->DPDL1HSOCR = (DSI_HS_PREPARE_OFFSET2 << DSI_DPDL1HSOCR_Pos);

        // Set LPXO value for data lanes
        DsiRegister->DPDL0LPXOCR &= ~DSI_DPDL0LPXOCR;
        DsiRegister->DPDL1LPXOCR &= ~DSI_DPDL1LPXOCR;

        DsiRegister->DPDL0LPXOCR = PHY_LP_OFFSSET_0_CLKP;
        DsiRegister->DPDL1LPXOCR = PHY_LP_OFFSSET_0_CLKP;

        // Set the VCO Frequency
        DsiRegister->WRPCR &= ~(DSI_WRPCR_BC);
        DsiRegister->WRPCR |= (DSI_DPHY_VCO_FRANGE_800MHZ_1GHZ << DSI_WRPCR_BC_Pos);
        DsiRegister->WPTR &= ~(DSI_WPTR_LPF | DSI_WPTR_CP);

        // Set PLL Tuning
        DsiRegister->WPTR |= (DSI_PLL_LOOP_FILTER_2000HZ_4400HZ << DSI_WPTR_LPF_Pos);

        // Set PLL_Charge Value
        DsiRegister->WPTR |= (DSI_PLL_CHARGE_PUMP_2000HZ_4400HZ << DSI_WPTR_CP_Pos);

        // Enable the DSI PLL with delay
        SET_BIT(DsiRegister->WRPCR, DSI_WRPCR_PLLEN);
        (void)READ_BIT(DsiRegister->WRPCR, DSI_WRPCR_PLLEN);

        // Requires min of 400us delay before reading the PLLLS flag
        // 1ms delay is inserted that is the minimum HAL delay granularity
        DelayMilliseconds(1);

        while ((DsiRegister->WISR & DSI_WISR_PLLLS) == 0U)
        {
        };

        //************************** Set the PHY parameters **************************

        // D-PHY clock and digital enable
        DsiRegister->PCTLR |= (DSI_PCTLR_CKE | DSI_PCTLR_DEN);

        // Clock lane configuration
        DsiRegister->CLCR &= ~(DSI_CLCR_DPCC | DSI_CLCR_ACR);
        DsiRegister->CLCR |= (DSI_CLCR_DPCC | DSI_AUTO_CLK_LANE_CTRL_DISABLE);

        // Configure the number of active data lanes
        DsiRegister->PCONFR &= ~DSI_PCONFR_NL;
        DsiRegister->PCONFR |= DSI_TWO_DATA_LANES;

        //*********************** Set the DSI clock parameters ***********************
        // Set the TX escape clock division factor
        DsiRegister->CCR &= ~DSI_CCR_TXECKDIV;
        DsiRegister->CCR |= 4;

        //***************************** Error management ****************************
        // Disable all error interrupts and reset the Error Mask
        DsiRegister->IER[0U] = 0U;
        DsiRegister->IER[1U] = 0U;

        // DSI interrupt Init
        NVIC_SetPriority(DSI_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(DSI_IRQn);

        // Set the timeout clock division factor
        DsiRegister->CCR &= ~DSI_CCR_TOCKDIV;
        DsiRegister->CCR |= (TimeoutCkdiv << 8U);

        // High-speed transmission timeout
        DsiRegister->TCCR[0U] &= ~DSI_TCCR0_HSTX_TOCNT;

        // Low-power reception timeout
        DsiRegister->TCCR[0U] &= ~DSI_TCCR0_LPRX_TOCNT;

        // High-speed read timeout
        DsiRegister->TCCR[1U] &= ~DSI_TCCR1_HSRD_TOCNT;

        // Low-power read timeout
        DsiRegister->TCCR[2U] &= ~DSI_TCCR2_LPRD_TOCNT;

        // High-speed write timeout
        DsiRegister->TCCR[3U] &= ~DSI_TCCR3_HSWR_TOCNT;

        // High-speed write presp mode
        DsiRegister->TCCR[3U] &= ~DSI_TCCR3_PM;
        DsiRegister->TCCR[3U] |= DSI_HS_PM_DISABLE;

        // Low-speed write timeout
        DsiRegister->TCCR[4U] &= ~DSI_TCCR4_LPWR_TOCNT;

        // BTA timeout
        DsiRegister->TCCR[5U] &= ~DSI_TCCR5_BTA_TOCNT;

        // Insert LPX Offset into the timings
        // In Automatic Clock Lane control mode, the DSI Host can turn off the clock lane between two High - Speed
        // transmission. To do so, the DSI Host calculates the time required for the clock lane to change from HighSpeed
        // to Low - Power and from Low - Power to High - Speed.
        // These timings are configured by the HS2LP_TIME and LP2HS_TIME in the DSI Host Clock Lane Timer Configuration
        // Register(DSI_CLTCR). But the DSI Host is not calculating LP2HS_TIME + HS2LP_TIME but 2 x HS2LP_TIME.
        // Workaround : Configure HS2LP_TIME and LP2HS_TIME with the same value being the max of HS2LP_TIME or
        // LP2HS_TIME.
        uint32_t DataLaneLP2HSTimeWorkaround = (DataLaneLP2HSTime + (2 * PHY_LP_OFFSSET_0_CLKP));

        // Clock lane timer configuration
        DsiRegister->CLTCR &= ~(DSI_CLTCR_LP2HS_TIME | DSI_CLTCR_HS2LP_TIME);
        DsiRegister->CLTCR |= (ClockLaneLP2HSTime | (ClockLaneLP2HSTime << 16U));

        // Data lane timer configuration
        DsiRegister->DLTCR &= ~(DSI_DLTCR_LP2HS_TIME | DSI_DLTCR_HS2LP_TIME);
        DsiRegister->DLTCR |= (DataLaneLP2HSTimeWorkaround | (DataLaneHS2LPTime << 16U));
        DsiRegister->DLTRCR &= ~DSI_DLTRCR_MRD_TIME;

        // Configure the wait period to request HS transmission after a stop state
        DsiRegister->PCONFR &= ~DSI_PCONFR_SW_TIME;
        DsiRegister->PCONFR |= (StopWaitTime << 8U);

        // Set the DSI Host Protocol Configuration Register
        DsiRegister->PCR &=
            ~((DSI_PCR_CRCRXE | DSI_PCR_ECCRXE | DSI_PCR_BTAE | DSI_PCR_ETRXE | DSI_PCR_ETTXE | DSI_PCR_ETTXLPE));
        DsiRegister->PCR |= DSI_PCR_BTAE;

        // Select video mode by resetting CMDM and DSIM bits
        DsiRegister->MCR &= ~DSI_MCR_CMDM;
        DsiRegister->WCFGR &= ~DSI_WCFGR_DSIM;

        // Configure the video mode transmission type
        DsiRegister->VMCR &= ~DSI_VMCR_VMT;
        DsiRegister->VMCR |= DSI_VID_MODE_BURST;

        // Set the chunks number to be transmitted through the DSI link
        DsiRegister->VCCR &= ~DSI_VCCR_NUMC;

        // Set the size of the null packet to 0
        DsiRegister->VNPCR &= ~DSI_VNPCR_NPSIZE;

        // Select virtual channel 0 for the LTDC interface traffic
        DsiRegister->LVCIDR &= ~DSI_LVCIDR_VCID;

        // Configure the polarity of control signals
        DsiRegister->LPCR &= ~(DSI_LPCR_DEP | DSI_LPCR_VSP | DSI_LPCR_HSP);
        DsiRegister->LPCR |= (LTDC_DEPOLARITY_AL | LTDC_GCR_VSPOL | DSI_HSYNC_ACTIVE_HIGH);

        // Select the color coding for the host
        DsiRegister->LCOLCR &= ~DSI_LCOLCR_COLC;
        DsiRegister->LCOLCR |= DSI_RGB565;

        // Select the color coding for the wrapper
        DsiRegister->WCFGR &= ~DSI_WCFGR_COLMUX;
        DsiRegister->WCFGR |= (DSI_RGB565 << 1U);

        // Configure the command transmission mode
        DsiRegister->VMCR &= ~DSI_VMCR_LPCE;
        DsiRegister->VMCR |= DSI_LP_COMMAND_DISABLE;

        // Low power largest packet size
        DsiRegister->LPMCR &= ~DSI_LPMCR_LPSIZE;

        // Low power VACT largest packet size
        DsiRegister->LPMCR &= ~DSI_LPMCR_VLPSIZE;

        // Enable LP transition in HFP period
        DsiRegister->VMCR &= ~DSI_VMCR_LPHFPE;
        DsiRegister->VMCR |= DSI_VMCR_LPHFPE;

        // Enable LP transition in HBP period
        DsiRegister->VMCR &= ~DSI_VMCR_LPHBPE;
        DsiRegister->VMCR |= DSI_VMCR_LPHBPE;

        // Enable LP transition in VACT period
        DsiRegister->VMCR &= ~DSI_VMCR_LPVAE;
        DsiRegister->VMCR |= DSI_VMCR_LPVAE;

        // Enable LP transition in VFP period
        DsiRegister->VMCR &= ~DSI_VMCR_LPVFPE;
        DsiRegister->VMCR |= DSI_VMCR_LPVFPE;

        // Enable LP transition in VBP period
        DsiRegister->VMCR &= ~DSI_VMCR_LPVBPE;
        DsiRegister->VMCR |= DSI_VMCR_LPVBPE;

        // Enable LP transition in vertical sync period
        DsiRegister->VMCR &= ~DSI_VMCR_LPVSAE;
        DsiRegister->VMCR |= DSI_VMCR_LPVSAE;

        // Enable the request for an acknowledge response at the end of a frame
        DsiRegister->VMCR &= ~DSI_VMCR_FBTAAE;
        DsiRegister->VMCR |= DSI_VMCR_FBTAAE;

        // Configure the video packet size
        DsiRegister->VPCR &= ~DSI_VPCR_VPSIZE;
        DsiRegister->VPCR |= PacketSize;

        // Set the Horizontal Synchronization Active (HSA) in lane byte clock cycles
        DsiRegister->VHSACR &= ~DSI_VHSACR_HSA;
        DsiRegister->VHSACR |= HorizontalSyncActive;

        // Set the Horizontal Back Porch (HBP) in lane byte clock cycles
        DsiRegister->VHBPCR &= ~DSI_VHBPCR_HBP;
        DsiRegister->VHBPCR |= HorizontalBackPorch;

        // Set the total line time (HLINE=HSA+HBP+HACT+HFP) in lane byte clock cycles
        DsiRegister->VLCR &= ~DSI_VLCR_HLINE;
        DsiRegister->VLCR |= HorizontalLine;

        // Set the Vertical Synchronization Active (VSA)
        DsiRegister->VVSACR &= ~DSI_VVSACR_VSA;
        DsiRegister->VVSACR |= VerticalSyncActive;

        // Set the Vertical Back Porch (VBP)
        DsiRegister->VVBPCR &= ~DSI_VVBPCR_VBP;
        DsiRegister->VVBPCR |= VerticalBackPorch;

        // Set the Vertical Front Porch (VFP)
        DsiRegister->VVFPCR &= ~DSI_VVFPCR_VFP;
        DsiRegister->VVFPCR |= VerticalFrontPorch;

        // Set the Vertical Active period
        DsiRegister->VVACR &= ~DSI_VVACR_VA;
        DsiRegister->VVACR |= VerticalActive;

        // Update the GVCIDR register with the Tx and Rx Virtual channel ID of 0
        DsiRegister->GVCIDR &= ~(DSI_GVCIDR_VCIDRX | DSI_GVCIDR_VCIDTX);

#pragma endregion

#pragma region LTDC Setup
        // Peripheral clock enable
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_LTDCEN);

        // Configure the HS, VS, DE and PC polarity
        LtdcRegister->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
        LtdcRegister->GCR |= (uint32_t)(DSI_HSYNC_ACTIVE_HIGH | DSI_VSYNC_ACTIVE_HIGH | DSI_DATA_ENABLE_ACTIVE_HIGH |
                                        LTDC_PCPOLARITY_IPC);

        // Set Synchronization size
        LtdcRegister->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
        LtdcRegister->SSCR |= ((HorizontalSync << 16U) | VerticalSync);

        // Set Accumulated Back porch
        LtdcRegister->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
        LtdcRegister->BPCR |= ((AccumulatedHBP << 16U) | AccumulatedVBP);

        // Set Accumulated Active Width
        LtdcRegister->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
        LtdcRegister->AWCR |= ((AccumulatedActiveW << 16U) | AccumulatedActiveH);

        // Set Total Width
        LtdcRegister->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
        LtdcRegister->TWCR |= ((TotalWidth << 16U) | TotalHeight);

        // Set the background color value
        LtdcRegister->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
        LtdcRegister->BCCR |= (((uint32_t)(Red) << 16U) | ((uint32_t)(Green) << 8U) | Blue);

        // Configure the horizontal start and stop position
        LTDC_Layer_Register0->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
        LTDC_Layer_Register0->WHPCR =
            ((((LtdcRegister->BPCR & LTDC_BPCR_AHBP) >> 16U) + 1U) |
             ((WindowX1 + ((LtdcRegister->BPCR & LTDC_BPCR_AHBP) >> 16U)) << 16U));

        // Configure the vertical start and stop position
        LTDC_Layer_Register0->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
        LTDC_Layer_Register0->WVPCR =
            (((LtdcRegister->BPCR & LTDC_BPCR_AVBP) + 1U) |
             ((WindowY1 + (LtdcRegister->BPCR & LTDC_BPCR_AVBP)) << 16U));

        // Specifies the pixel format
        LTDC_Layer_Register0->PFCR &= ~LTDC_LxPFCR_PF;
        LTDC_Layer_Register0->PFCR = PixelFormat;

        // Configure the default color values
        LTDC_Layer_Register0->DCCR &=
            ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);
        LTDC_Layer_Register0->DCCR = (Blue | ((uint32_t)(Green) << 8U) | ((uint32_t)(Red) << 16U));

        // Specifies the constant alpha value
        LTDC_Layer_Register0->CACR &= ~LTDC_LxCACR_CONSTA;
        LTDC_Layer_Register0->CACR = Alpha;

        // Specifies the blending factors
        LTDC_Layer_Register0->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
        LTDC_Layer_Register0->BFCR = (LTDC_BLENDING_FACTOR1_CA | LTDC_BLENDING_FACTOR2_CA);

        // Configure the color frame buffer start address
        LTDC_Layer_Register0->CFBAR &= ~LTDC_LxCFBAR_CFBADD;
        LTDC_Layer_Register0->CFBAR = FBStartAdress;

        // Configure the color frame buffer pitch in byte
        LTDC_Layer_Register0->CFBLR &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
        LTDC_Layer_Register0->CFBLR = (((ImageWidth * 2) << 16U) | (WindowX1 * 2 + 3U));

        // Configure the frame buffer line number
        LTDC_Layer_Register0->CFBLNR &= ~LTDC_LxCFBLNR_CFBLNBR;
        LTDC_Layer_Register0->CFBLNR = ImageHeight;

        // Enable LTDC_Layer by setting LEN bit+-
        LTDC_Layer_Register0->CR |= (uint32_t)LTDC_LxCR_LEN;

        // Enable LTDC
        LtdcRegister->GCR |= LTDC_GCR_LTDCEN;

        // Set the Immediate Reload type
        LtdcRegister->SRCR = LTDC_SRCR_IMR;

        // LTCD Init
        NVIC_SetPriority(LTDC_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(LTDC_ER_IRQn);

#pragma endregion

#pragma region DMA2 Setup
        LL_DMA2D_InitTypeDef DMA2D_InitStruct;
        LL_DMA2D_StructInit(&DMA2D_InitStruct);
        DMA2D_InitStruct.Mode = LL_DMA2D_MODE_M2M;
        DMA2D_InitStruct.ColorMode = LL_DMA2D_OUTPUT_MODE_RGB565;
        DMA2D_InitStruct.NbrOfLines = 0x0U;
        DMA2D_InitStruct.NbrOfPixelsPerLines = 0x0U;
        DMA2D_InitStruct.LineOffset = 0x0U;
        DMA2D_InitStruct.OutputBlue = 0x0U;
        DMA2D_InitStruct.OutputGreen = 0x0U;
        DMA2D_InitStruct.OutputRed = 0x0U;
        DMA2D_InitStruct.OutputAlpha = 0x0U;
        DMA2D_InitStruct.OutputMemoryAddress = 0x0U;
        LL_DMA2D_Init(DMA2D, &DMA2D_InitStruct);
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2D);
        NVIC_SetPriority(DMA2D_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(DMA2D_IRQn);
#pragma endregion
    }
    // Enable the DSI host and DSI wrapper
    SET_BIT(DsiRegister->CR, DSI_CR_EN);
    SET_BIT(DsiRegister->WCR, DSI_WCR_DSIEN);

    return;
}

void DisplayInterface::GetTransferBuffer(CLR_UINT8 *&TransferBuffer, CLR_UINT32 &TransferBufferSize)
{
    TransferBuffer = (CLR_UINT8 *)&Graphics_frame_buffer;
    TransferBufferSize = (Width * Height * 2);
}

void DisplayInterface::ClearFrameBuffer()
{
    while (LL_DMA2D_IsTransferOngoing(DMA2D))
        ; // DMA2D is asynchronous, we may return here from a previous call before it was finished

    // Setup DMA to clear the graphic frame buffer
    LL_DMA2D_SetMode(DMA2D, LL_DMA2D_MODE_R2M);
    LL_DMA2D_SetOutputColor(DMA2D, LCD_COLOR_RGB565_BLACK);
    LL_DMA2D_SetOutputMemAddr(DMA2D, (uint32_t)&Graphics_frame_buffer);
    LL_DMA2D_SetOutputMemAddr(DMA2D, LCD_FRAME_BUFFER);
    LL_DMA2D_SetLineOffset(DMA2D, 0);
    LL_DMA2D_ConfigSize(DMA2D, Height, Width);
   // DMA2D->OPFCCR = PIXEL_FORMAT_RGB565;

    // Start the transfer
    LL_DMA2D_Start(DMA2D);

}

void DisplayInterface::WriteToFrameBuffer(
    CLR_UINT8 command,
    CLR_UINT8 data[],
    CLR_UINT32 dataCount,
    CLR_UINT32 frameOffset)
{
    (void)command;
    (void)dataCount;
    (void)frameOffset;

    while (LL_DMA2D_IsTransferOngoing(DMA2D))
        ; // DMA2D is asynchronous, so we may return here from a previous call before it was finished

    // Setup DMA to transfer data to the graphic frame buffer
    LL_DMA2D_SetMode(DMA2D, LL_DMA2D_MODE_M2M);
    LL_DMA2D_FGND_SetMemAddr(DMA2D, (uint32_t)data);
    LL_DMA2D_SetOutputMemAddr(DMA2D, (uint32_t)&Graphics_frame_buffer);
    LL_DMA2D_ConfigSize(DMA2D, 272, 480);
    LL_DMA2D_SetLineOffset(DMA2D, 0);

    // Enable the transfer complete
    LL_DMA2D_EnableIT_TC(DMA2D);

    // Start the transfer
    LL_DMA2D_Start(DMA2D);

    INVALIDATE_DCACHE;
}

void DisplayInterface::DisplayBacklight(bool on)
{
    (void)on;
}

void DisplayInterface::SendCommand(CLR_UINT8 NbrParams, ...)
{
    (void)NbrParams;
}

int32_t BSP_LCD_SetBrightness(uint32_t Instance, uint32_t Brightness)
{
    (void)Instance;
    (void)Brightness;

    // TIM_SET_COMPARE(&hlcd_tim, LCD_TIMx_CHANNEL, 2U * Brightness);
    return 1;
}

int32_t BSP_LCD_GetBrightness(uint32_t Instance, uint32_t *Brightness)
{
    (void)Instance;
    (void)Brightness;
    return 1;
}
