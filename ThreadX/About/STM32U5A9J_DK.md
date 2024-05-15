# Welcome to .NET **nanoFramework** community targets port of STM32U5A9J Discovery Kit

## Overview

Ultra-low-power STM32U5x9NJH6Q microcontroller based on the 
Arm® Cortex®‑M33 core with Arm® TrustZone®, featuring 4 Mbytes of flash 
memory, 3 Mbytes of SRAM for STM32U5G9NJH6Q or 2.5 Mbytes for 
STM32U5A9NJH6Q, and SMPS in a TFBGA216 package
• 2.47" RGB 480 × 480 pixels TFT round LCD module with 16.7M color depth, 
with MIPI DSI® 2‑data lane interface and capacitive touch panel
• USB Type-C® with USB 2.0 HS interface, sink only
• Low‑power system designed for VDD at 1.8 V only
• MEMS sensors from STMicroelectronics
– Time‑of‑Flight and gesture-detection sensor
– Temperature sensor
• 512‑Mbit Octo‑SPI NOR flash memory
• 512‑Mbit Hexadeca‑SPI PSRAM
• 4‑Gbyte eMMC flash memory
• Two user LEDs
• User and reset push-buttons
• Board connectors:
– USB ST-LINK Micro-B
– USB Type-C®
– Two double-row 2.54 mm pitch expansion connectors for additional 
peripherals prototyping
– Audio MEMS daughterboard expansion (for STM32U5G9J-DK1)
– MIPI10
– Tag‑Connect™ 10‑pin footprint
• Flexible power-supply options: ST-LINK USB VBUS, USB connector, or external 
sources
• On-board STLINK-V3E debugger/programmer with USB re-enumeration 
capability: mass storage, Virtual COM port, and debug port
• Comprehensive free software libraries and examples available with the 
STM32CubeU5 MCU Package
• Support of a wide choice of Integrated Development Environments (IDEs) 
including IAR Embedded Workbench®, MDK-ARM, and STM32CubeIDE

**Display**

Shenzhen Jinghua Displays Electronics
www.china-lcd.com
2.47" TFT round display 480x480 resolution, 16.7 million color depth with MIPI 2-lane interface and capacitive multi-touch display

Part number J025F1CN0201W or J025F1CN0201N.




More information about the board can be found at the [STM32U5A9J-DK website](https://www.st.com/en/evaluation-tools/stm32u5a9j-dk.html). More information about STM32U5A9NJH6Q can be found here:

- [STM32U5A9NJ on www.st.com](https://www.st.com/en/microcontrollers-microprocessors/stm32u5a9nj.html)
- [STM32U5 Series reference manual](https://www.st.com/resource/en/reference_manual/rm0456-stm32u5-series-armbased-32bit-mcus-stmicroelectronics.pdf)
- [STM32U5Axxx datasheet](https://www.st.com/resource/en/datasheet/stm32u5a9nj.pdf)

### Supported Features

The current Zephyr stm32u5a9j_dk board configuration supports the following hardware features:

| Interface | Controller | Driver/Component |
| --------- | ---------- | ---------------- |
| NVIC      | on-chip    | In Development   |
| UART      | on-chip    | In Development   |
| LPUART    | on-chip    | In Development   |
| PINMUX    | on-chip    | In Development   |
| GPIO      | on-chip    | In Development   |
| RNG       | on-chip    | In Development   |
| I2C       | on-chip    | In Development   |
| SPI       | on-chip    | In Development   |
| FLASH     | on-chip    | In Development   |
| ADC       | on-chip    | In Development   |
| SDMMC     | on-chip    | In Development   |
| WATCHDOG  | on-chip    | In Development   |
| PWM       | on-chip    | In Development   |

Other hardware features have not been enabled yet for this board.

#### Peripheral Mapping:

- USART_1 TX/RX : PA9/PA10 (ST-Link Virtual Port Com)
- LD3 : PE0 - LED Green
- LD4 : PE1 - LED Red
- User Button: PC13
- USART_3 TX/RX : PB10/PB11
- LPUART_1 TX/RX : PG7/PG8
- I2C1 SCL/SDA : PG14/PG13
- I2C2 SCL/SDA : PF1/PF0
- I2C6 SCL/SDA : PD1/PD0
- SPI2 SCK/MISO/MOSI/CS : PB13/PD3/PD4/PB12
- SPI3 SCK/MISO/MOSI/CS : PG9/PG10/PG11/PG15
- ADC1 : channel5 PA0, channel14 PC5
- ADC2 : channel9 PA4
- ADC4 : channel5 PF14

### System Clock

The STM32U5A9J-DK Discovery kit relies on an HSE oscillator (16 MHz crystal) and an LSE oscillator (32.768 kHz crystal) as clock references. Using the HSE (instead of HSI) is mandatory to manage the DSI interface for the LCD module and the USB high?speed interface.

### Serial Port

The STM32U5A9J Discovery kit has up to 4 USARTs, 2 UARTs, and 1 LPUART. The Zephyr console output is assigned to USART1 which connected to the onboard ST-LINK/V3.0. Virtual COM port interface. Default communication settings are 115200 8N1.

## Programming and Debugging

STM32U5A9J Discovery kit includes an ST-LINK/V3 embedded debug tool interface. This probe allows to flash and debug the board using various tools.



