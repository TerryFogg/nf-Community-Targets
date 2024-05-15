# Welcome to .NET **nanoFramework** community targets port of STM32H735G_DK

## Overview

The STM32H735G-DK Discovery kit is a complete demonstration and development platform for Arm® Cortex®-M7 core-based STM32H735IGK6U microcontroller, with 1 Mbyte of Flash memory and 564 Kbytes of SRAM.

The STM32H735G-DK Discovery kit is used as a reference design for user application development before porting to the final product, thus simplifying the application development.

The full range of hardware features available on the board helps users to enhance their application development by an evaluation of all the peripherals (such as USB OTG FS, Ethernet, microSD™ card, USART, CAN FD, SAI audio DAC stereo with audio jack input and output, MEMS digital microphone, HyperRAM™, Octo-SPI Flash memory, RGB interface LCD with capacitive touch panel, and others). ARDUINO® Uno V3, Pmod™ and STMod+ connectors provide easy connection to extension shields or daughterboards for specific applications.

STLINK-V3E is integrated into the board, as the embedded in-circuit debugger and programmer for the STM32 MCU and USB Virtual COM port bridge. STM32H735G-DK board comes with the STM32CubeH7 MCU Package, which provides an STM32 comprehensive software HAL library as well as various software examples.

![STM32H735G-DISCO](../1.nanoFramework/assets/stm32h735g_disco.jpg)

More information about the board can be found at the [STM32H735G-DISCO website](https://www.st.com/en/evaluation-tools/stm32h735g-dk.html). More information about STM32H735 can be found here:

- [STM32H725/735 on www.st.com](https://www.st.com/en/microcontrollers-microprocessors/stm32h725-735.html)
- [STM32H735xx reference manual](https://www.st.com/resource/en/reference_manual/dm00603761-stm32h723733-stm32h725735-and-stm32h730-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [STM32H735xx datasheet](https://www.st.com/resource/en/datasheet/stm32h735ag.pdf)

### Supported Features

The current STM32H735G_DK board configuration supports the following hardware features:

| Interface | Controller | Driver/Component |
| --------- | ---------- | ---------------- |
| NVIC      | on-chip    | In Development   |
| UART      | on-chip    | In Development   |
| GPIO      | on-chip    | In Development   |
| FLASH     | on-chip    | In Development   |
| ETHERNET  | on-chip    | In Development   |
| RNG       | on-chip    | In Development   |
| FMC       | on-chip    | In Development   |
| ADC       | on-chip    | In Development   |

#### Peripheral Mapping:

##### Serial Ports

##### UART_3 - (ST-Link Virtual Port Com)

TX - PD8

RX - PD9

##### UART_7 - (Arduino Serial)

TX - PF7

RX - PF6

##### LEDS

LED1 : PC3 (__HAL_RCC_GPIOC_CLK_ENABLE())

LED2 : PC2

##### Buttons

User Pin : PC13

### System Clock

The STM32H735G System Clock can be driven by an internal or external oscillator, as well as by the main PLL clock. By default, the System clock is driven by the PLL clock at 550MHz. PLL clock is feed by a 25MHz high speed external clock.

### Serial Port

The STM32H735G Discovery kit has up to 6 UARTs, UART3 which connected to the onboard ST-LINK/V3.0. Virtual COM port interface.

## Programming and Debugging

### Flashing

Flashing operation will depend on the target to be flashed and the SoC option bytes configuration. It is advised to use [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) to check and update option bytes configuration and flash the `stm32h735g_disco` target.

# STM32H736NI

The **STM32H736NI** is a high-performance microcontroller from STMicroelectronics. It is based on the **ARM Cortex-M7** core and is part of the **STM32H7** series of microcontrollers. Here are some of its features:

- **CPU**: ARM Cortex-M7 core running at up to 400 MHz

- **Memory**: 1 MB of Flash memory, 564 KB of RAM

- Peripherals

  :

  - 2 x USB OTG FS/HS
  - 2 x CAN FD
  - 3 x I2C
  - 4 x USART
  - 4 x SPI
  - 2 x SAI
  - 2 x I2S
  - 2 x SDMMC
  - 2 x DFSDM
  - 2 x ADC (16-bit)
  - 2 x DAC (12-bit)
  - 2 x RNG
  - 2 x HASH
  - 2 x CRYP
  - 2 x FMC/SDRAM
  - 1 x QSPI
  - 1 x Ethernet MAC
  - 1 x LCD-TFT controller
  - 1 x JPEG codec
  - 1 x Chrom-ART graphic accelerator

- Operating conditions

  :

  - Voltage range: 1.71 V to 3.6 V
  - Temperature range: -40°C to +125°C

- Security

  :

  - AES-256 hardware encryption
  - Secure boot and secure firmware update
  - TrustZone and STSAFE secure elements
