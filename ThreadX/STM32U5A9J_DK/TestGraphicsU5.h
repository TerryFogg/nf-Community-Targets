
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "life_augmented_argb8888.h"

#include "stm32u5xx_hal.h"
#include "stm32u5x9j_discovery.h"
#include "stm32_lcd.h"
#include "image_320x240_argb8888.h"
#include <string.h>
#include <stdio.h>
#include "stm32u5xx_hal.h"

    int TestGraphicsU5(void);

// Private defines
#define VFP              50
#define LCD_WIDTH        480
#define VBP              12
#define LCD_FRAME_BUFFER 0x200D0000
#define HACT             480
#define VSYNC            1
#define HFP              1
#define VACT             481
#define IMAGE_HEIGHT     240
#define HBP              1
#define IMAGE_WIDTH      320
#define LCD_HEIGHT       481
#define HSYNC            2

#ifdef __cplusplus
}
#endif

#endif
