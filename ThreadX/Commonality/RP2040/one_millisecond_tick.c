#include "board.h"

int one_millisecond_tick = 0;

// tick interrupt priority (lowest by default)
#define TICK_INT_PRIORITY_LOW 15

// This uses the STM32 TIM7 to generate a 1 millisecond timer
// for small delays used in configuring components
void InitOneMillisecondTick()
{
}
void HAL_SuspendTick()
{
}
void HAL_ResumeTick()
{
}
void OneMillisecond_Tick_IRQHandler()
{
    one_millisecond_tick += 1;
    // Clear interrupt
}
int GetCurrentMilliseconds()
{
    return one_millisecond_tick;
}
void DelayMilliseconds(int milliseconds)
{
    int tick_start = one_millisecond_tick;
    while ((one_millisecond_tick - tick_start) < milliseconds)
        ;
}