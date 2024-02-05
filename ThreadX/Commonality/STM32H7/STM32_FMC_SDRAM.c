
// Copyright(c).NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "board.h"
#include "stm32h7xx_hal_mdma.h"
#include "stm32h7xx_hal_sdram.h"

#define SDRAM_MEMORY_WIDTH                       FMC_SDRAM_MEM_BUS_WIDTH_16
#define SDCLOCK_PERIOD                           FMC_SDRAM_CLOCK_PERIOD_3
#define SDRAM_TIMEOUT                            ((uint32_t)0xFFFF)
#define REFRESH_COUNT                            ((uint32_t)0x0603)
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

void Initialize_SDRAM(uint32_t sdram_base_address, uint32_t sdram_region_size)
{
    (void)sdram_base_address;
    (void)sdram_region_size;
#pragma region Setup the FMC pin definitions
    LL_GPIO_InitTypeDef gpio_init_structure;

    // Enable FMC clock
    SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

    // Enable GPIOs clock
    ENABLE_CLOCK_ON_PORT_GPIOD;
    ENABLE_CLOCK_ON_PORT_GPIOE;
    ENABLE_CLOCK_ON_PORT_GPIOF;
    ENABLE_CLOCK_ON_PORT_GPIOG;
    ENABLE_CLOCK_ON_PORT_GPIOH;

    /* Common GPIO configuration */
    gpio_init_structure.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init_structure.Pull = LL_GPIO_PULL_NO;
    gpio_init_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = LL_GPIO_AF_12;

    // GPIOD configuration
    gpio_init_structure.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 |
                              LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOD, &gpio_init_structure);

    // GPIOE configuration
    gpio_init_structure.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_7 | LL_GPIO_PIN_8 | LL_GPIO_PIN_9 |
                              LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 |
                              LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOE, &gpio_init_structure);

    // GPIOF configuration
    gpio_init_structure.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4 |
                              LL_GPIO_PIN_5 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 |
                              LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOF, &gpio_init_structure);

    // GPIOG configuration
    gpio_init_structure.Pin =
        LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_8 | LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOG, &gpio_init_structure);

    // GPIOH configuration
    gpio_init_structure.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    LL_GPIO_Init(GPIOH, &gpio_init_structure);

#pragma endregion

#pragma region MPU config

    // Disable MPU
    LL_MPU_Disable();
    {
            // Configure the MPU as Strongly ordered for undefined regions
        LL_MPU_ConfigRegion(
            LL_MPU_REGION_NUMBER0,
            0x87,
            0x00,
            LL_MPU_REGION_SIZE_4GB | LL_MPU_REGION_NO_ACCESS | LL_MPU_ACCESS_NOT_BUFFERABLE |
                LL_MPU_ACCESS_NOT_CACHEABLE | LL_MPU_ACCESS_SHAREABLE | LL_MPU_TEX_LEVEL0 |
                LL_MPU_INSTRUCTION_ACCESS_DISABLE);

        // Configure SDRAM as region 16MB
        LL_MPU_ConfigRegion(
            LL_MPU_REGION_NUMBER1,
            0x00,
            sdram_base_address,
            sdram_region_size | LL_MPU_REGION_FULL_ACCESS | LL_MPU_ACCESS_NOT_BUFFERABLE | LL_MPU_ACCESS_CACHEABLE |
                LL_MPU_ACCESS_NOT_SHAREABLE | LL_MPU_TEX_LEVEL0 | LL_MPU_INSTRUCTION_ACCESS_ENABLE);
    }
    // Enable MPU (any access not covered by any enabled region will cause a fault)
    LL_MPU_Enable(LL_MPU_CTRL_PRIVILEGED_DEFAULT);
#pragma endregion

#pragma region SDRAM Timing

    FMC_SDRAM_TimingTypeDef SDRAM_Timing;
    SDRAM_Timing.LoadToActiveDelay = 2;
    SDRAM_Timing.ExitSelfRefreshDelay = 7;
    SDRAM_Timing.SelfRefreshTime = 4;
    SDRAM_Timing.RowCycleDelay = 7;
    SDRAM_Timing.WriteRecoveryTime = 2;
    SDRAM_Timing.RPDelay = 2;
    SDRAM_Timing.RCDDelay = 2;

    SDRAM_HandleTypeDef hsdram;
    hsdram.Instance = FMC_SDRAM_DEVICE;
    hsdram.Init.SDBank = FMC_SDRAM_BANK2;
    hsdram.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
    hsdram.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
    hsdram.Init.MemoryDataWidth = SDRAM_MEMORY_WIDTH;
    hsdram.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hsdram.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
    hsdram.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hsdram.Init.SDClockPeriod = SDCLOCK_PERIOD;
    hsdram.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    hsdram.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_2;

    FMC_SDRAM_Init(hsdram.Instance, &(hsdram.Init));
    FMC_SDRAM_Timing_Init(hsdram.Instance, &SDRAM_Timing, hsdram.Init.SDBank);
    __FMC_ENABLE();

#pragma endregion

#pragma region Setup attached SDRAM device
    FMC_SDRAM_CommandTypeDef Command;
    Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    ;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    FMC_SDRAM_SendCommand(FMC_SDRAM_DEVICE, &Command, SDRAM_TIMEOUT);

    // TODO: Replace with proper delay tick code later
    // Insert a 100us delay, assuming a nop on 280Mhz executes in 4.57 nanoSeconds
    for (volatile uint32_t i = 0; i < 21000; i++)
    {
        __ASM("nop");
    }

    Command.CommandMode = FMC_SDRAM_CMD_PALL;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    FMC_SDRAM_SendCommand(FMC_SDRAM_DEVICE, &Command, SDRAM_TIMEOUT);
    Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;
    FMC_SDRAM_SendCommand(FMC_SDRAM_DEVICE, &Command, SDRAM_TIMEOUT);
    Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1 | SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
                                     SDRAM_MODEREG_CAS_LATENCY_2 | SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
    FMC_SDRAM_SendCommand(FMC_SDRAM_DEVICE, &Command, SDRAM_TIMEOUT);
    FMC_SDRAM_ProgramRefreshRate(FMC_SDRAM_DEVICE, REFRESH_COUNT);

#pragma endregion
}
