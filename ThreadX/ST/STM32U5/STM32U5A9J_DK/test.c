//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "board.h"

int GetCurrentMilliseconds();

void SetPanelConfig(void);
void LCD_FillRect2(uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);


uint32_t HAL_GetTick(void)
{
    return GetCurrentMilliseconds();
}

void test(void)
{
    SetPanelConfig();
    // Clear LCD
    LCD_FillRect2(0, 0, LCD_WIDTH, LCD_HEIGHT, UTIL_LCD_COLOR_BLACK);
    LCD_FillRect2(0, 0, LCD_WIDTH, LCD_HEIGHT, UTIL_LCD_COLOR_BROWN);
    LCD_FillRect2(0, 0, LCD_WIDTH, LCD_HEIGHT, UTIL_LCD_COLOR_ORANGE);
    LCD_FillRect2(0, 0, LCD_WIDTH, LCD_HEIGHT, UTIL_LCD_COLOR_YELLOW);
    return;
}

void LCD_FillRect2(uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
    while (LL_DMA2D_IsTransferOngoing(DMA2D))
        ; // DMA2D is asynchronous, we may return here from a previous call before it was finished

    // Setup DMA to clear the graphic frame buffer
    LL_DMA2D_SetMode(DMA2D, LL_DMA2D_MODE_R2M);
    LL_DMA2D_SetOutputColor(DMA2D, CONVERTARGB88882RGB565(Color));
    LL_DMA2D_SetOutputMemAddr(DMA2D, LCD_FRAME_BUFFER);
    LL_DMA2D_SetLineOffset(DMA2D, 0);
    LL_DMA2D_ConfigSize(DMA2D, Height, Width);

    // Start the transfer
    LL_DMA2D_Start(DMA2D);

    return;
}
