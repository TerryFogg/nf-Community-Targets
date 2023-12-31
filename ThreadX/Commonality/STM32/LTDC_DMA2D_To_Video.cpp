//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// -------------------------------------------------------
// For the STM32
// Directly drive a LCD TFT using the LTDC controller

// Timing and setting for the RK043FN48H LCD driver

#include <stdarg.h>
#include <stdio.h>
#include "DisplayInterface.h"
#include <nanoCLR_Interop.h>
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_dma.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_dma2d.h"
#include "board.h"

#define LCD_COLOR_RGB565_BLACK 0x0000U

#define LCD_LAYER_0_ADDRESS         0x70000000U
#define PIXEL_FORMAT_RGB565         0x00000002U
#define LTDC_BLENDING_FACTOR1_PAxCA 0x00000600U
#define LTDC_BLENDING_FACTOR2_PAxCA 0x00000007U
#define LTDC_HSPOLARITY_AL          0x00000000U
#define LTDC_VSPOLARITY_AL          0x00000000U
#define LTDC_DEPOLARITY_AL          0x00000000U
#define LTDC_PCPOLARITY_IPC         0x00000000U
#define LTDC_IT_TE                  LTDC_IER_TERRIE
#define LTDC_IT_FU                  LTDC_IER_FUIE
#define MAX_LAYER                   2U

struct DisplayInterface g_DisplayInterface;
// Framebuffer set externally
extern CLR_UINT32 Graphics_frame_buffer;

CLR_UINT32 Width;
CLR_UINT32 Height;

void SetLcdPin(GPIO_TypeDef *GpioPort, CLR_UINT32 GpioPin, CLR_UINT32 AlternateFunction)
{
    // LTDC Pins configuration
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = AlternateFunction;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pin = GpioPin;
    LL_GPIO_Init(GpioPort, &GPIO_InitStruct);
}

void DisplayInterface::Initialize(DisplayInterfaceConfig &config)
{
    Width = config.Screen.width;
    Height = config.Screen.height;

    LTDCClock_Config();

    // Enable THE CLOCKS
    LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_LTDC);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOH);

    // LTDC Pins configuration
    SetLcdPin(LCD_R0_PORT, LCD_R0, LL_GPIO_AF_14);   // lcd r0
    SetLcdPin(LCD_R1_PORT, LCD_R1, LL_GPIO_AF_14);   // lcd r1
    SetLcdPin(LCD_R2_PORT, LCD_R2, LL_GPIO_AF_14);   // lcd r2
    SetLcdPin(LCD_R3_PORT, LCD_R3, LL_GPIO_AF_14);   // lcd r3
    SetLcdPin(LCD_R4_PORT, LCD_R4, LL_GPIO_AF_14);   // lcd r4
    SetLcdPin(LCD_R5_PORT, LCD_R5, LL_GPIO_AF_14);   // lcd r5
    SetLcdPin(LCD_R6_PORT, LCD_R6, LL_GPIO_AF_14);   // lcd r6
    SetLcdPin(LCD_R7_PORT, LCD_R7, LL_GPIO_AF_14);   // lcd r7

    SetLcdPin(LCD_B0_PORT, LCD_B0, LL_GPIO_AF_14);   // lcd b0
    SetLcdPin(LCD_B1_PORT, LCD_B1, LL_GPIO_AF_14);   // lcd b1
    SetLcdPin(LCD_B2_PORT, LCD_B2, LL_GPIO_AF_14);   // lcd b2
    SetLcdPin(LCD_B3_PORT, LCD_B3, LL_GPIO_AF_13);   // lcd b3
    SetLcdPin(LCD_B4_PORT, LCD_B4, LL_GPIO_AF_14);   // lcd b4
    SetLcdPin(LCD_B5_PORT, LCD_B5, LL_GPIO_AF_14);   // lcd b5
    SetLcdPin(LCD_B6_PORT, LCD_B6, LL_GPIO_AF_14);   // lcd b6
    SetLcdPin(LCD_B7_PORT, LCD_B7, LL_GPIO_AF_14);   // lcd b7

    SetLcdPin(LCD_G0_PORT, LCD_G0, LL_GPIO_AF_14);   // lcd g0
    SetLcdPin(LCD_G1_PORT, LCD_G1, LL_GPIO_AF_14);   // lcd g1
    SetLcdPin(LCD_G2_PORT, LCD_G2, LL_GPIO_AF_14);   // lcd g2
    SetLcdPin(LCD_G3_PORT, LCD_G3, LL_GPIO_AF_14);   // lcd g3
    SetLcdPin(LCD_G4_PORT, LCD_G4, LL_GPIO_AF_9);    // lcd g4
    SetLcdPin(LCD_G5_PORT, LCD_G5, LL_GPIO_AF_9);    // lcd g5
    SetLcdPin(LCD_G6_PORT, LCD_G6, LL_GPIO_AF_14);   // lcd g6
    SetLcdPin(LCD_G7_PORT, LCD_G7, LL_GPIO_AF_14);   // lcd g7

    SetLcdPin(LCD_VSYNC_PORT, LCD_VSYNC, LL_GPIO_AF_14);
    SetLcdPin(LCD_HSYNC_PORT, LCD_HSYNC, LL_GPIO_AF_14);
    SetLcdPin(LCD_CLK_PORT, LCD_CLK, LL_GPIO_AF_14);
    SetLcdPin(LCD_DISP_CTRL_PORT, LCD_DISP_CTRL_PIN, LL_GPIO_AF_14);


    // Set Enable, Control and Backlight pins to output mode
    LL_GPIO_InitTypeDef GPIO_InitStructLCD;
    GPIO_InitStructLCD.Mode = LL_GPIO_OUTPUT_PUSHPULL; // Output for all pins

    GPIO_InitStructLCD.Pin = config.VideoDisplay.backlight;
    LL_GPIO_Init(LCD_BL_CTRL_PORT, &GPIO_InitStructLCD); // Backlight

    GPIO_InitStructLCD.Pin = config.VideoDisplay.enable;
    LL_GPIO_Init(LCD_DISP_EN_PORT, &GPIO_InitStructLCD); // Enable

    GPIO_InitStructLCD.Pin = config.VideoDisplay.control;
    LL_GPIO_Init(LCD_DISP_CTRL_PORT, &GPIO_InitStructLCD); // Control

        // Initialize the LTDC
    LL_APB3_GRP1_ForceReset(LL_APB3_GRP1_PERIPH_LTDC); // Toggle Sw reset of LTDC IP
    LL_APB3_GRP1_ReleaseReset(LL_APB3_GRP1_PERIPH_LTDC);
    
    LL_GPIO_ResetOutputPin(LCD_DISP_EN_PORT, LCD_DISP_EN_PIN);
    LL_GPIO_SetOutputPin(LCD_DISP_CTRL_PORT, LCD_DISP_CTRL_PIN);
    LL_GPIO_SetOutputPin(LCD_DISP_CTRL_PORT, LCD_BL_CTRL_PIN);

    LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_DMA2D); // Enable the DMA2D clock
    LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_DMA2D);  // Toggle Sw reset of DMA2D IP
    LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_DMA2D);

    LL_DMA2D_SetOutputColorMode(DMA2D, LL_DMA2D_OUTPUT_MODE_RGB565); // Configure the DMA2D Color Mode
    LL_DMA2D_FGND_SetAlpha(DMA2D, 0xFF);                             // Always write opaque
    LL_DMA2D_FGND_SetColorMode(DMA2D, LL_DMA2D_INPUT_MODE_RGB565);   // Foreground layer format is RGB565 (16 bpp)

    // Configure the HS, VS, DE and PC polarity
    LTDC->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
    LTDC->GCR |= (uint32_t)(LTDC_HSPOLARITY_AL | LTDC_VSPOLARITY_AL | LTDC_DEPOLARITY_AL | LTDC_PCPOLARITY_IPC);

    // Set Synchronization size
    LTDC->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
    LTDC->SSCR |=
        (((config.VideoDisplay.Horizontal_synchronization - 1U) << 16U) |
         (config.VideoDisplay.Vertical_synchronization - 1U));

    // Set Accumulated Back porch
    LTDC->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
    LTDC->BPCR |=
        (((config.VideoDisplay.Horizontal_synchronization + (config.VideoDisplay.Horizontal_back_porch - 11U) - 1U)
          << 16U) |
         (config.VideoDisplay.Vertical_synchronization + config.VideoDisplay.Vertical_back_porch - 1U));

    // Set Accumulated Active Width
    LTDC->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
    LTDC->AWCR |=
        (((config.VideoDisplay.Horizontal_synchronization + Width + config.VideoDisplay.Horizontal_back_porch - 1U)
          << 16U) |
         (config.VideoDisplay.Vertical_synchronization + Height + config.VideoDisplay.Vertical_back_porch - 1U));

    // Set Total Width
    LTDC->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
    LTDC->TWCR |=
        (((config.VideoDisplay.Horizontal_synchronization + Width + (config.VideoDisplay.Horizontal_back_porch - 11U) +
           config.VideoDisplay.Horizontal_front_porch - 1U)
          << 16U) |
         (config.VideoDisplay.Vertical_synchronization + Height + config.VideoDisplay.Vertical_back_porch +
          config.VideoDisplay.Vertical_front_porch - 1U));

    // Set the background color value
    LTDC->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
    LTDC->BCCR |= (0 | 0 | 0);

    LTDC->IER |= (LTDC_IT_TE | LTDC_IT_FU); // Enable the Transfer Error and FIFO underrun interrupts
    LTDC->GCR |= LTDC_GCR_LTDCEN;           // Enable LTDC by setting LTDCEN bit

    // Configure up a default LTDC Layer 0
    // We don't use the layer blending feature, all the 'smarts' are performed by
    // code on the graphics buffer before we DMA2D it to the graphics frame buffer
    //
    // Configure the LTDC Layer
    //-------------------
    LTDC_Layer_TypeDef *Layer0 = (LTDC_Layer_TypeDef *)((uint32_t)LTDC + 0x84U);

    // Configure the horizontal start and stop position
    Layer0->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);

    Layer0->WHPCR =
        ((0 + ((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16U) + 1U) | ((Width + ((LTDC->BPCR & LTDC_BPCR_AHBP) >> 16U)) << 16U));

    // Configure the vertical start and stop position
    Layer0->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
    Layer0->WVPCR = ((0 + (LTDC->BPCR & LTDC_BPCR_AVBP) + 1U) | ((Height + (LTDC->BPCR & LTDC_BPCR_AVBP)) << 16U));

    // Specifies the pixel format
    Layer0->PFCR &= ~(LTDC_LxPFCR_PF);
    Layer0->PFCR = (PIXEL_FORMAT_RGB565);

    // Configure the default color values
    Layer0->DCCR &= ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);
    Layer0->DCCR = (0 | 0 | 0 | 0);

    // Specifies the constant alpha value
    Layer0->CACR &= ~(LTDC_LxCACR_CONSTA);
    Layer0->CACR = (255);

    // Specifies the blending factors
    Layer0->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
    Layer0->BFCR = (LTDC_BLENDING_FACTOR1_PAxCA | LTDC_BLENDING_FACTOR2_PAxCA);

    // Configure the color frame buffer start address
    Layer0->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
    Layer0->CFBAR = (LCD_LAYER_0_ADDRESS);

    // Configure the color frame buffer pitch in byte
    Layer0->CFBLR &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
    Layer0->CFBLR = (((Width * 2) << 16U) | (((Width - 0) * 2) + 7U));
    // Configure the frame buffer line number
    Layer0->CFBLNR &= ~(LTDC_LxCFBLNR_CFBLNBR);
    Layer0->CFBLNR = Height;

    // Enable LTDC_Layer by setting LEN bit
    Layer0->CR |= (uint32_t)LTDC_LxCR_LEN;

    // Set the Immediate Reload type
    LTDC->SRCR = LTDC_SRCR_IMR;

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
    LL_DMA2D_SetLineOffset(DMA2D, 0);
    LL_DMA2D_ConfigSize(DMA2D, Height, Width);
    DMA2D->OPFCCR = PIXEL_FORMAT_RGB565;

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

    SCB_CleanInvalidateDCache();
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

    //    TIM_SET_COMPARE(&hlcd_tim, LCD_TIMx_CHANNEL, 2U*Brightness);
    return 1;
}

//
// brief Set the brightness value *@param Instance LCD Instance *@param Brightness [00:Min(black), 100 Max] *
//    @retval BSP status

int32_t BSP_LCD_GetBrightness(uint32_t Instance, uint32_t *Brightness)
{
    (void)Instance;
    (void)Brightness;
    return 1;
}
