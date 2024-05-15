#include "stm32u5xx_ll_tim.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_rcc.h"

int one_millisecond_tick = 0;

// tick interrupt priority (lowest by default)
#define TICK_INT_PRIORITY_LOW 15

// This uses the STM32 TIM6 to generate a 1 millisecond timer
// for small delays used in configuring components
void InitOneMillisecondTick()
{
    LL_RCC_ClocksTypeDef RCC_Clocks;

    // 1/1000 second time base
    uint32_t Period = (1000000U / 1000U) - 1U;

    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);
    uint32_t timer_clock_frequency = RCC_Clocks.PCLK1_Frequency;
    uint32_t uwPrescalerValue = (uint32_t)((timer_clock_frequency / 1000000U) - 1U);

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
    LL_TIM_SetUpdateSource(TIM6, LL_TIM_UPDATESOURCE_COUNTER);

    // Configure the TIM6 time base
    LL_TIM_InitTypeDef TIM_InitStruct;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    TIM_InitStruct.Prescaler = uwPrescalerValue;
    TIM_InitStruct.Autoreload = Period;
    TIM_InitStruct.RepetitionCounter = 0;
    LL_TIM_Init(TIM6, &TIM_InitStruct);
    LL_TIM_EnableIT_UPDATE(TIM6);
    LL_TIM_EnableCounter(TIM6);

    NVIC_SetPriority(TIM6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), TICK_INT_PRIORITY_LOW, 0));
    NVIC_EnableIRQ(TIM6_IRQn);
}
void HAL_SuspendTick()
{
    LL_TIM_DisableIT_UPDATE(TIM6);
}
void HAL_ResumeTick()
{
    LL_TIM_EnableCounter(TIM6);
}
void TIM6_IRQHandler()
{
    one_millisecond_tick += 1;
    // Clear interrupt
    TIM6->SR = ~TIM_DIER_UIE;
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