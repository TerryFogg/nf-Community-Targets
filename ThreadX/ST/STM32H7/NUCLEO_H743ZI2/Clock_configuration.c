
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

    LL_RCC_PLL1_Disable();
    {
        LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

        // Main PLL configuration and activation
        LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSI);
        LL_RCC_PLL1P_Enable();
        LL_RCC_PLL1Q_Enable();
        LL_RCC_PLL1R_Enable();
        LL_RCC_PLL1FRACN_Disable();
        LL_RCC_PLL1_SetFRACN(0);
        LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_8_16);
        LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
        LL_RCC_PLL1_SetM(4);
        LL_RCC_PLL1_SetN(60);
        LL_RCC_PLL1_SetP(2);
        LL_RCC_PLL1_SetQ(2);
        LL_RCC_PLL1_SetR(2);
    }
    LL_RCC_PLL1_Enable();
    while (LL_RCC_PLL1_IsReady() != 1)
    {
    }

    // Setup peripheral prescalers
    LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
    LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_2);
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

    //    LL_RCC_CSI_Enable();
    //    LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SYSCFG); // System clock enable
    //    LL_SYSCFG_EnableCompensationCell();
    //
}

//#include "stm32h7xx_hal_rcc.h"
//void SystemClock_Config(void)
//{
//    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//    /** Supply configuration update enable
//     */
//    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
//
//    /** Configure the main internal regulator output voltage
//     */
//    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
//
//    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
//    {
//    }
//
//    __HAL_RCC_SYSCFG_CLK_ENABLE();
//    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
//
//    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
//    {
//    }
//
//    /** Initializes the RCC Oscillators according to the specified parameters
//     * in the RCC_OscInitTypeDef structure.
//     */
//    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
//    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
//    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
//    RCC_OscInitStruct.PLL.PLLM = 4;
//    RCC_OscInitStruct.PLL.PLLN = 60;
//    RCC_OscInitStruct.PLL.PLLP = 2;
//    RCC_OscInitStruct.PLL.PLLQ = 2;
//    RCC_OscInitStruct.PLL.PLLR = 2;
//    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
//    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
//    RCC_OscInitStruct.PLL.PLLFRACN = 0;
//    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//    {
//       // Error_Handler();
//    }
//
//    /** Initializes the CPU, AHB and APB buses clocks
//     */
//    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
//                                  RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
//    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
//    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
//    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
//    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
//    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
//    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
//
//    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
//    {
//        //Error_Handler();
//    }
//}
