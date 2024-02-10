
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.

#include "board.h"

//  System Clock Configuration
//        The system Clock is configured as follow :
//           System Clock source            = PLL (HSE BYPASS)
//           SYSCLK(Hz)                     = 400000000 (CPU Clock)
//           HCLK(Hz)                       = 200000000 (AXI and AHBs Clock)
//           AHB Prescaler                  = 2
//           D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
//           D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
//           D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
//           D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
//           HSE Frequency(Hz)              = 8000000
//           PLL_M                          = 4
//           PLL_N                          = 400
//           PLL_P                          = 2
//           PLL_Q                          = 4
//           PLL_R                          = 2
//           VDD(V)                         = 3.3
//           Flash Latency(WS)              = 4

void SystemClock_Config(void)
{
    // Supply configuration update enable
    LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    while (LL_PWR_IsActiveFlag_VOS() == 0)
    {
    }

    LL_RCC_HSE_EnableBypass();
    LL_RCC_HSE_Enable();
    while (LL_RCC_HSE_IsReady() != 1)
    {
    }

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

    // Main PLL configuration and activation
    LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSE);
    LL_RCC_PLL1P_Enable();
    LL_RCC_PLL1Q_Enable();
    LL_RCC_PLL1R_Enable();
    LL_RCC_PLL1FRACN_Disable();
    LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_2_4);
    LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
    LL_RCC_PLL1_SetM(4);
    LL_RCC_PLL1_SetN(400);
    LL_RCC_PLL1_SetP(2);
    LL_RCC_PLL1_SetQ(4);
    LL_RCC_PLL1_SetR(2);
    LL_RCC_PLL1_Enable();
    while (LL_RCC_PLL1_IsReady() != 1)
    {
    }

    // Setup peripheral prescalers
    LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
    LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);

    // Set PLL1 as System Clock Source
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
    {
    }

    //
    // Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
    //         (GPIO, SPI, FMC, OSPI ...)  when  operating at  high frequencies (please refer to product datasheet)
    //         The I/O Compensation Cell activation  procedure requires :
    // - The activation of the CSI clock
    // - The activation of the SYSCFG clock
    // - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
    //
    //   To do this please uncomment the following code
    //

    LL_RCC_CSI_Enable();
    LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SYSCFG); // System clock enable
    LL_SYSCFG_EnableCompensationCell();
}
