//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// -------------------------------------------------------
// For the STM32

#include <stdarg.h>
#include <stdio.h>
#include "DisplayInterface.h"
#include <nanoCLR_Interop.h>
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

void DisplayInterface::Initialize(DisplayInterfaceConfig &config)
{
    Width = config.Screen.width;
    Height = config.Screen.height;
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

