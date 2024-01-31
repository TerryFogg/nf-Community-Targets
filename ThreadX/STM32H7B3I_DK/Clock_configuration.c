
 // Copyright (c) .NET Foundation and Contributors
 // Portions Copyright (c) Microsoft Corporation.  All rights reserved.
 // See LICENSE file in the project root for full license information.

#include "board.h"

// System Clock Configuration
//          The system Clock is configured as follow :
//             System Clock source            = PLL (HSE)
//             SYSCLK(Hz)                     = 280000000 (CPU Clock)
//             HCLK(Hz)                       = 280000000 (Bus matrix and AHBs Clock)
//             AHB Prescaler                  = 1
//             CD APB3 Prescaler              = 2 (APB3 Clock  140MHz)
//             CD APB1 Prescaler              = 2 (APB1 Clock  140MHz)
//             CD APB2 Prescaler              = 2 (APB2 Clock  140MHz)
//             SRD APB4 Prescaler             = 2 (APB4 Clock  140MHz)
//             HSE Frequency(Hz)              = 24000000
//             PLL_M                          = 12
//             PLL_N                          = 280
//             PLL_P                          = 2
//             PLL_Q                          = 2
//             PLL_R                          = 2
//             VDD(V)                         = 3.3
//             Flash Latency(WS)              = 6
// /

  void SystemClock_Config(void)
 {
     // Power Configuration
     LL_PWR_ConfigSupply(LL_PWR_DIRECT_SMPS_SUPPLY);
     LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
     while (LL_PWR_IsActiveFlag_VOS() == 0)
     {
     }

     // Enable HSE oscillator
     LL_RCC_HSE_Enable();
     while (LL_RCC_HSE_IsReady() != 1)
     {
     }

     /* Set FLASH latency */
     LL_FLASH_SetLatency(LL_FLASH_LATENCY_6);

     /* Main PLL configuration and activation */
     LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSE);
     LL_RCC_PLL1P_Enable();
     LL_RCC_PLL1Q_Enable();
     LL_RCC_PLL1R_Enable();
     LL_RCC_PLL1FRACN_Disable();
     LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_1_2);
     LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
     LL_RCC_PLL1_SetM(12);
     LL_RCC_PLL1_SetN(280);
     LL_RCC_PLL1_SetP(2);
     LL_RCC_PLL1_SetQ(2);
     LL_RCC_PLL1_SetR(2);
     LL_RCC_PLL1_Enable();

     while (LL_RCC_PLL1_IsReady() != 1)
     {
     }

     /* Set Sys & AHB & APB1 & APB2 & APB4  prescaler */
     LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
     LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_1);
     LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
     LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
     LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);

     /* Set PLL1 as System Clock Source */
     LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
     while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
     {
     }

 }

 void LTDCClock_Config(void)
 {
     // RK043FN48H LCD clock configuration
     // LCD clock configuration
     // PLL3_VCO Input = HSE_VALUE/PLL3M = 4 Mhz
     // PLL3_VCO Output = PLL3_VCO Input * PLL3N = 800 Mhz
     // PLLLCDCLK = PLL3_VCO Output/PLL3R = 800/83 = 9.63 Mhz
     // LTDC clock frequency = PLLLCDCLK = 9.63 Mhz
     LL_RCC_PLL3_Disable();
     LL_RCC_PLL3_SetM(6);
     LL_RCC_PLL3_SetN(200);
     LL_RCC_PLL3_SetP(10);
     LL_RCC_PLL3_SetQ(10);
     LL_RCC_PLL3_SetR(83);
     LL_RCC_PLL3_SetFRACN(0);
     LL_RCC_PLL3_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
     LL_RCC_PLL3_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_1_2);
     LL_RCC_PLL3_Enable();
 }
