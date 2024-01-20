//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "board.h"

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follows :
 *            System Clock source            = PLL (MSI)
 *            SYSCLK(Hz)                     = 160000000
 *            HCLK(Hz)                       = 160000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            MSI Frequency(Hz)              = 4000000
 *            PLL_MBOOST                     = 1
 *            PLL_M                          = 1
 *            PLL_N                          = 80
 *            PLL_Q                          = 2
 *            PLL_R                          = 2
 *            PLL_P                          = 2
 *            Flash Latency(WS)              = 4
 */
static void SystemClock_Config(void)
{
    /* Enable PWR clock */
    LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);

    /* Set the regulator supply output voltage */
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    while (LL_PWR_IsActiveFlag_VOS() == 0)
    {
    }

    /* Enable MSI oscillator */
    LL_RCC_MSIS_SetRange(LL_RCC_MSISRANGE_4);
    LL_RCC_MSI_SetCalibTrimming(10, LL_RCC_MSI_OSCILLATOR_0);
    LL_RCC_MSIS_Enable();
    while (LL_RCC_MSIS_IsReady() != 1)
    {
    }

    /* Set FLASH latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

    /* Configure PLL clock source */
    LL_RCC_PLL1_SetMainSource(LL_RCC_PLL1SOURCE_MSIS);

    /* Enable the EPOD to reach max frequency */
    LL_PWR_EnableEPODBooster();
    while (LL_PWR_IsActiveFlag_BOOST() == 0)
    {
    }

    /* Main PLL configuration and activation */
    LL_RCC_PLL1_EnableDomain_SYS();
    LL_RCC_PLL1FRACN_Disable();
    LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
    LL_RCC_SetPll1EPodPrescaler(LL_RCC_PLL1MBOOST_DIV_1);
    LL_RCC_PLL1_SetDivider(1);
    LL_RCC_PLL1_SetN(80);
    LL_RCC_PLL1_SetP(2);
    LL_RCC_PLL1_SetQ(2);
    LL_RCC_PLL1_SetR(2);

    LL_RCC_PLL1_Enable();
    while (LL_RCC_PLL1_IsReady() != 1)
    {
    }

    /* Set AHB, APB1, APB2 and APB3 prescalers */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

    /* Set PLL1 as System Clock Source */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
    {
    }

    /* Set systick to 1ms with frequency set to 160MHz */
    LL_Init1msTick(160000000);

    /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
    LL_SetSystemCoreClock(160000000);
}
