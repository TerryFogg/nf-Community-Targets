#include "board.h"

#define MSICALIBRATION_DEFAULT 0x10U

void SystemClock_Config(void)
{
    uint32_t PLLM = 1;
    uint32_t PLLN = 80;
    uint32_t PLLP = 2;
    uint32_t PLLQ = 2;
    uint32_t PLLR = 2;
    uint32_t PLLRGE = LL_RCC_PLLINPUTRANGE_4_8;
    uint32_t PLLFRACN = 0;

    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_RCC_MSIS_SetRange(LL_RCC_MSISRANGE_4);
    {
        MODIFY_REG(RCC->ICSCR2, RCC_ICSCR2_MSITRIM1, MSICALIBRATION_DEFAULT << RCC_ICSCR2_MSITRIM1_Pos);
    }
    LL_RCC_MSI_SetCalibTrimming(0, LL_RCC_MSI_OSCILLATOR_1);
    LL_RCC_HSE_Enable();
    while (LL_RCC_HSE_IsReady() != 1U)
    {
    }

    LL_RCC_PLL1_Disable();
    {
        // Wait till PLL is disabled
        while (LL_RCC_PLL1_IsReady() != 0U)
        {
        };
        LL_PWR_DisableEPODBooster();
        {
            LL_RCC_PLL1_ConfigDomain_SYS(LL_RCC_PLL1SOURCE_MSIS, PLLM, PLLN, PLLR);
            LL_RCC_PLL1_SetQ(PLLQ);
            LL_RCC_PLL1_SetP(PLLP);
            LL_RCC_SetPll1EPodPrescaler(LL_RCC_PLL1MBOOST_DIV_1);

            LL_RCC_PLL1FRACN_Disable();
            {
                LL_RCC_PLL1_SetFRACN(PLLFRACN);
            }
            LL_RCC_PLL1FRACN_Enable();

            LL_RCC_PLL1_SetVCOInputRange(PLLRGE);
        }
        LL_PWR_EnableEPODBooster();
        if (LL_PWR_IsEnabledEPODBooster())
        {
            while (!LL_PWR_IsActiveFlag_BOOST())
            {
            };
        }

        LL_RCC_PLL1_EnableDomain_SYS();
    }
    LL_RCC_PLL1_Enable();
    while (LL_RCC_PLL1_IsReady() != 1U)
    {
    }

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
    {
    };
    return;
}

void PeriphCommonClock_Config(void)
{
    uint32_t PLL3M = 4;
    uint32_t PLL3N = 125;
    uint32_t PLL3P = 8;
    uint32_t PLL3Q = 2;
    uint32_t PLL3R = 24;
    uint32_t PLL3FRACN = 0;

    LL_RCC_PLL3_Disable();
    while (LL_RCC_PLL3_IsReady() != 0U)
    {
    };
    {
        LL_RCC_PLL3_SetSource(LL_RCC_PLL3SOURCE_HSE);
        LL_RCC_PLL3_SetDivider(PLL3M);
        LL_RCC_PLL3_SetN(PLL3N);
        LL_RCC_PLL3_SetP(PLL3P);
        LL_RCC_PLL3_SetQ(PLL3Q);
        LL_RCC_PLL3_SetR(PLL3R);
        LL_RCC_PLL3_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
        LL_RCC_HSE_Enable();

        LL_RCC_PLL3_EnableDomain_SAI();
        LL_RCC_PLL3_EnableDomain_HSPI_LTDC();
        LL_RCC_PLL3FRACN_Disable();
        {
            LL_RCC_PLL3_SetFRACN(PLL3FRACN);
        }
        LL_RCC_PLL3FRACN_Enable();
    }
    LL_RCC_PLL3_Enable();
    while (LL_RCC_PLL3_IsReady() != 1U)
    {
    };

    LL_RCC_SetLTDCClockSource(LL_RCC_LTDC_CLKSOURCE_PLL3);
    LL_RCC_SetDSIClockSource(LL_RCC_DSI_CLKSOURCE_PLL3);
}
