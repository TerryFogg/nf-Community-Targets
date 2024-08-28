

# Welcome to .NET **nanoFramework** community targets port of STM32H7B3I_DK

# STM32H7B3I Discovery Kit

## Overview

The STM32H7B3I-DK Discovery kit is a complete demonstration and development platform for STMicroelectronics Arm® Cortex®-M7 core-based STM32H7B3LIH6QU microcontroller.

The STM32H7B3I-DK Discovery kit is used as a reference design for user application development before porting to the final product, thus simplifying the application development.

The full range of hardware features available on the board helps users enhance their application development by an evaluation of almost all peripherals (such as USB OTG_HS, microSD, USART, FDCAN, audio DAC stereo with audio jack input and output, camera, SDRAM, Octo-SPI Flash memory and RGB interface LCD with capacitive touch panel). ARDUINO® Uno V3 connectors provide easy connection to extension shields or daughterboards for specific applications.

STLINK-V3E is integrated into the board, as an embedded in-circuit debugger and programmer for the STM32 MCU and the USB Virtual COM port bridge. The STM32H7B3I-DK board comes with the STM32CubeH7 MCU Package, which provides an STM32 comprehensive software HAL library as well as various software examples.

![STM32H7B3I-DK](assets/stm32h7b3i_dk.jpg)

More information about the board can be found at the [STM32H7B3I-DK website](https://www.st.com/en/evaluation-tools/stm32h7b3i-dk.html). More information about STM32H7B3 can be found here:

- [STM32H7A3/7B3 on www.st.com](https://www.st.com/en/microcontrollers-microprocessors/stm32h7a3-7b3.html)
- [STM32H7A3/7B3/7B0 reference manual](https://www.st.com/resource/en/reference_manual/rm0455-stm32h7a37b3-and-stm32h7b0-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [STM32H7B3xI datasheet](https://www.st.com/resource/en/datasheet/stm32h7b3ai.pdf)
- [STM32H7B3I_DK data brief](https://www.st.com/resource/en/data_brief/stm32h7b3i-dk.pdf)

- [STM32H7B3I_DK user manual](https://www.st.com/resource/en/user_manual/um2569-discovery-kit-with-stm32h7b3li-mcu-stmicroelectronics.pdf)

# Arduino UNO V3  compatible connector



| Arduino Pin Number | STM32 Pin Reference | Analogue   |
| ------------------ | ------------------- | ---------- |
| A0                 | PA4                 | ADC1_INP18 |
| A1                 | PC4                 | ADC12_INP4 |
| A2                 | PA0_C               | ADC1_INP0  |
| A3                 | PA1_C               | ADC1_INP1  |
| A4                 | PC2_C               | ADC2_INP0  |
| A5                 | PC3_C               | ADC2_INP1  |



|      | Arduino Pin Number | STM32 Pin Reference | Digital  | Alternate                          |
| ---- | ------------------ | ------------------- | -------- | ---------------------------------- |
|      | D0                 | PH14                | GPIO     | USART4_RX                          |
|      | D1                 | PH13                | GPIO     | USART4_TX                          |
|      | D2                 | PI9                 | GPIO     |                                    |
|      | D3                 | PH9                 | GPIO     | TIM12_CH2                          |
|      | D4                 | PE2                 | GPIO     |                                    |
|      | D5                 | PH11                | GPIO     | TIM5_CH2                           |
|      | D6                 | PH10                | GPIO     | TIM5_CH1                           |
|      | D7                 | PI10                | GPIO     |                                    |
|      | D8                 | PF10                | GPIO     |                                    |
|      | D9                 | PI7                 | GPIO     | TIM8_CH3                           |
|      | D10                | PI0                 | GPIO     | SPI2_NSS                           |
|      | D11                | PB15                | GPIO     | SPI2 _MOSO                         |
|      | D12                | PB14                | GPIO     | SPI2 _MISO                         |
|      | D13                | PA12                | GPIO     | SPI2_SCK                           |
|      | D14                | PD13                | I2C4_SDA | Wired to touch and audio circuitry |
|      | D15                | PD12                | I2C4_SCL | Wired to touch and audio circuitry |
