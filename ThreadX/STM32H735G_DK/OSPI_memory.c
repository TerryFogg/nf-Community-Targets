
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_rcc.h"
#include "board.h"

#define OSPI_TIMEOUT_DEFAULT_VALUE         ((uint32_t)5000U)
#define OSPI_FUNCTIONAL_MODE_MEMORY_MAPPED ((uint32_t)OCTOSPI_CR_FMODE)
#define OSPI_WRAP_NOT_SUPPORTED            ((uint32_t)0x00000000U)
#define OSPI_DELAY_BLOCK_USED              ((uint32_t)0x00000000U)
#define OSPI_MEMTYPE_HYPERBUS              ((uint32_t)OCTOSPI_DCR1_MTYP_2)
#define OSPI_CLOCK_MODE_0                  ((uint32_t)0x00000000U)
#define OSPI_DUALQUAD_DISABLE              ((uint32_t)0x00000000U)
#define OSPI_SAMPLE_SHIFTING_NONE          ((uint32_t)0x00000000U)
#define OSPI_DHQC_ENABLE                   ((uint32_t)OCTOSPI_TCR_DHQC)
#define OSPI_FLAG_BUSY                     OCTOSPI_SR_BUSY
#define OSPI_LATENCY_ON_WRITE              ((uint32_t)0x00000000U)
#define OSPI_FIXED_LATENCY                 ((uint32_t)OCTOSPI_HLCR_LM)
#define OSPI_MEMORY_ADDRESS_SPACE          ((uint32_t)0x00000000U)
#define OSPI_ADDRESS_32_BITS               ((uint32_t)OCTOSPI_CCR_ADSIZE)
#define OSPI_DQS_ENABLE                    ((uint32_t)OCTOSPI_CCR_DQSE)
#define OSPI_TIMEOUT_COUNTER_DISABLE       (uint32_t)0x00000000U
// Port G
#define OSPI_RAM_CS_PIN LL_GPIO_PIN_12
// Port F
#define OSPI_RAM_DQS_PIN LL_GPIO_PIN_12
#define OSPI_RAM_CLK_PIN LL_GPIO_PIN_4

#define OSPI_RAM_D0_PIN LL_GPIO_PIN_0
#define OSPI_RAM_D1_PIN LL_GPIO_PIN_1
#define OSPI_RAM_D2_PIN LL_GPIO_PIN_2
#define OSPI_RAM_D3_PIN LL_GPIO_PIN_3
#define OSPI_RAM_D4_PIN LL_GPIO_PIN_0
#define OSPI_RAM_D5_PIN LL_GPIO_PIN_1
#define OSPI_RAM_D6_PIN LL_GPIO_PIN_10
#define OSPI_RAM_D7_PIN LL_GPIO_PIN_11

// Hyperam definitions based on the Integrated Circuit S....
#define OSPI_HYPERRAM_SIZE        24
#define OSPI_HYPERRAM_RW_REC_TIME 3
#define OSPI_HYPERRAM_LATENCY     6

#define OCTOSPI2_DISABLE CLEAR_BIT(OCTOSPI2->CR, OCTOSPI_CR_EN)
#define OCTOSPI2_ENABLE  SET_BIT(OCTOSPI2->CR, OCTOSPI_CR_EN)

// OCTOSPI2 - octal configuration (8 data lines),
// using HyperBus protocol,
// memory mapped,
// double transfer rate

void Initialize_OCTOSPI2_Hyperam()
{
    uint32_t FifoThreshold = 4;
    uint32_t ChipSelectHighTime = 8;
    uint32_t ClkChipSelectHighTime = 0;
    uint32_t ClockPrescaler = 4;
    uint32_t MaxTran = 0;
    uint32_t ChipSelectBoundary = 23;
    uint32_t Refresh = 250; // The chip select should be released every 4µs

    // Initialize OctoSPI
    LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_OSPI2);

    // Reset the OctoSPI memory interfaceLL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_OSPI2);
    LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_OSPI2);
    //  Ports F and G have
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOF);
    //  OCTOSPI2 signals on port pins
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG);

    LL_GPIO_InitTypeDef gpio_InitStruct = {0};
    // Common pin settings
    gpio_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_InitStruct.Pull = LL_GPIO_PULL_UP;
    gpio_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

    // Chip Select
    gpio_InitStruct.Pin = OSPI_RAM_CS_PIN;
    gpio_InitStruct.Alternate = LL_GPIO_AF_3;
    LL_GPIO_Init(GPIOG, &gpio_InitStruct);

    // DQS
    gpio_InitStruct.Pin = OSPI_RAM_DQS_PIN;

    gpio_InitStruct.Alternate = LL_GPIO_AF_9;
    LL_GPIO_Init(GPIOF, &gpio_InitStruct);

    // Clock
    gpio_InitStruct.Pin = OSPI_RAM_CLK_PIN;
    gpio_InitStruct.Alternate = LL_GPIO_AF_9;
    LL_GPIO_Init(GPIOF, &gpio_InitStruct);

    gpio_InitStruct.Pin = OSPI_RAM_D0_PIN | OSPI_RAM_D1_PIN | OSPI_RAM_D2_PIN | OSPI_RAM_D3_PIN;
    gpio_InitStruct.Alternate = LL_GPIO_AF_9;
    LL_GPIO_Init(GPIOF, &gpio_InitStruct);

    gpio_InitStruct.Pin = OSPI_RAM_D4_PIN | OSPI_RAM_D5_PIN | OSPI_RAM_D7_PIN;
    gpio_InitStruct.Alternate = LL_GPIO_AF_9;
    LL_GPIO_Init(GPIOG, &gpio_InitStruct);

    gpio_InitStruct.Pin = OSPI_RAM_D6_PIN;
    gpio_InitStruct.Alternate = LL_GPIO_AF_3;
    LL_GPIO_Init(GPIOG, &gpio_InitStruct);
    // Enable and set OctoSPI interrupt to the lowest priority
    NVIC_SetPriority(OCTOSPI2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x0F, 0));
    NVIC_EnableIRQ((OCTOSPI2_IRQn));
    
    OCTOSPI2_DISABLE;
    {
        CLEAR_BIT(OCTOSPI2->DCR1, OCTOSPI_DCR1_FRCK);
        MODIFY_REG(
            OCTOSPI2->DCR1,
            (OCTOSPI_DCR1_MTYP | OCTOSPI_DCR1_DEVSIZE | OCTOSPI_DCR1_CSHT | OCTOSPI_DCR1_CKCSHT | OCTOSPI_DCR1_DLYBYP |
             OCTOSPI_DCR1_FRCK | OCTOSPI_DCR1_CKMODE),

            OSPI_MEMTYPE_HYPERBUS | ((OSPI_HYPERRAM_SIZE - 1U) << OCTOSPI_DCR1_DEVSIZE_Pos) |
                ((ChipSelectHighTime - 1U) << OCTOSPI_DCR1_CSHT_Pos) |
                (ClkChipSelectHighTime << OCTOSPI_DCR1_CKCSHT_Pos) | OSPI_DELAY_BLOCK_USED | OSPI_CLOCK_MODE_0);
        MODIFY_REG(OCTOSPI2->DCR2, OCTOSPI_DCR2_WRAPSIZE, OSPI_WRAP_NOT_SUPPORTED);
        MODIFY_REG(OCTOSPI2->DCR2, OCTOSPI_DCR2_PRESCALER, ((ClockPrescaler - 1U) << OCTOSPI_DCR2_PRESCALER_Pos));
        OCTOSPI2->DCR3 = ((ChipSelectBoundary << OCTOSPI_DCR3_CSBOUND_Pos) | (MaxTran << OCTOSPI_DCR3_MAXTRAN_Pos));
        OCTOSPI2->DCR4 = Refresh;
        MODIFY_REG(OCTOSPI2->CR, OCTOSPI_CR_FTHRES, ((FifoThreshold - 1U) << OCTOSPI_CR_FTHRES_Pos));
        while (READ_BIT(OCTOSPI2->SR, OSPI_FLAG_BUSY) == SET)
        {
        };
        // Timing Configuration register settings
        // Configure sample shifting and delay hold quarter cycle
        MODIFY_REG(
            OCTOSPI2->TCR,
            (OCTOSPI_TCR_SSHIFT | OCTOSPI_TCR_DHQC),
            (OSPI_SAMPLE_SHIFTING_NONE | OSPI_DHQC_ENABLE));
        MODIFY_REG(OCTOSPI2->CR, OCTOSPI_CR_DQM, OSPI_DUALQUAD_DISABLE);
    }
    OCTOSPI2_ENABLE;
    // Wait till busy flag is reset
    while (READ_BIT(OCTOSPI2->SR, OSPI_FLAG_BUSY) == SET)
    {
    };

    // Configure the Hyperbus to access memory space
    // Hyperbus configuration Latency register
    {
        WRITE_REG(
            OCTOSPI2->HLCR,
            ((OSPI_HYPERRAM_RW_REC_TIME << OCTOSPI_HLCR_TRWR_Pos) | (OSPI_HYPERRAM_LATENCY << OCTOSPI_HLCR_TACC_Pos) |
             OSPI_LATENCY_ON_WRITE | OSPI_FIXED_LATENCY));
        // Wait till busy flag is reset
        while (READ_BIT(OCTOSPI2->SR, OSPI_FLAG_BUSY) == SET)
        {
        };
        MODIFY_REG(OCTOSPI2->CR, OCTOSPI_CR_FMODE, 0U);
        MODIFY_REG(OCTOSPI2->DCR1, OCTOSPI_DCR1_MTYP_0, OSPI_MEMORY_ADDRESS_SPACE);
    }
    // Configure the CCR and WCCR registers with the address size and the
    //   following configuration :
    //   - DQS signal enabled (used as RWDS)
    //   - DTR mode enabled on address and data
    //   - address and data on 8 lines */
    //  Data Double Transfer Rate
    WRITE_REG(
        OCTOSPI2->CCR,
        (OSPI_DQS_ENABLE | OCTOSPI_CCR_DDTR | OCTOSPI_CCR_DMODE_2 | OSPI_ADDRESS_32_BITS | OCTOSPI_CCR_ADDTR |
         OCTOSPI_CCR_ADMODE_2));
    WRITE_REG(
        OCTOSPI2->WCCR,
        (OSPI_DQS_ENABLE | OCTOSPI_WCCR_DDTR | OCTOSPI_WCCR_DMODE_2 | OSPI_ADDRESS_32_BITS | OCTOSPI_WCCR_ADDTR |
         OCTOSPI_WCCR_ADMODE_2));
    // Configure the Data Length register register with the number of data
    uint32_t NbData = 1;
    WRITE_REG(OCTOSPI2->DLR, (NbData - 1U));
    // Configure the Address register register with the address value
    uint32_t Address = 0;
    WRITE_REG(OCTOSPI2->AR, Address);

    //------------------------------
    // Set Hyperam to memory mapped
    //------------------------------
    while (READ_BIT(OCTOSPI2->SR, OSPI_FLAG_BUSY) == SET)
    {
    };
    // Configure CR register with functional mode as memory-mapped
    //  Setting bits, Timeout Counter Enable and Functional Mode
    //  Disable Timer Counter
    //  Set Functional mode to memory mapped
    MODIFY_REG(
        OCTOSPI2->CR,
        (OCTOSPI_CR_TCEN | OCTOSPI_CR_FMODE),
        (OSPI_TIMEOUT_COUNTER_DISABLE | OSPI_FUNCTIONAL_MODE_MEMORY_MAPPED));
}
