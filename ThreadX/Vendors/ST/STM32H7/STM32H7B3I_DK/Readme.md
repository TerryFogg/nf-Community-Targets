# STM32H7B3I Discovery Kit

## Overview

The STM32H7B3I-DK Discovery kit is a complete demonstration and development platform for STMicroelectronics Arm® Cortex®-M7 core-based STM32H7B3LIH6QU microcontroller.

The STM32H7B3I-DK Discovery kit is used as a reference design for user application development before porting to the final product, thus simplifying the application development.

The full range of hardware features available on the board helps users enhance their application development by an evaluation of almost all peripherals (such as USB OTG_HS, microSD, USART, FDCAN, audio DAC stereo with audio jack input and output, camera, SDRAM, Octo-SPI Flash memory and RGB interface LCD with capacitive touch panel). ARDUINO® Uno V3 connectors provide easy connection to extension shields or daughterboards for specific applications.

STLINK-V3E is integrated into the board, as an embedded in-circuit debugger and programmer for the STM32 MCU and the USB Virtual COM port bridge. The STM32H7B3I-DK board comes with the STM32CubeH7 MCU Package, which provides an STM32 comprehensive software HAL library as well as various software examples.

![STM32H7B3I-DK](https://docs.zephyrproject.org/latest/_images/stm32h7b3i_dk.jpg)

More information about the board can be found at the [STM32H7B3I-DK website](https://www.st.com/en/evaluation-tools/stm32h7b3i-dk.html). More information about STM32H7B3 can be found here:

- [STM32H7A3/7B3 on www.st.com](https://www.st.com/en/microcontrollers-microprocessors/stm32h7a3-7b3.html)
- [STM32H7A3/7B3/7B0 reference manual](https://www.st.com/resource/en/reference_manual/rm0455-stm32h7a37b3-and-stm32h7b0-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [STM32H7B3xI datasheet](https://www.st.com/resource/en/datasheet/stm32h7b3ai.pdf)
- [STM32H7B3I_DK data brief](https://www.st.com/resource/en/data_brief/stm32h7b3i-dk.pdf)

- [STM32H7B3I_DK user manual](https://www.st.com/resource/en/user_manual/um2569-discovery-kit-with-stm32h7b3li-mcu-stmicroelectronics.pdf)



### Supported Features

The current configuration supports the following hardware features:

| Interface | Controller | Driver/Component                           |
| --------- | ---------- | ------------------------------------------ |
| NVIC      | on-chip    | nested vector interrupt controller         |
| UART      | on-chip    | serial port-polling; serial port-interrupt |
| PINMUX    | on-chip    | pinmux                                     |
| GPIO      | on-chip    | gpio                                       |
| I2C       | on-chip    | i2c                                        |
| FLASH     | on-chip    | flash memory                               |
| FMC       | on-chip    | memc (SDRAM)                               |
| LTDC      | on-chip    | display                                    |
| CANFD     | on-chip    | can                                        |

Other hardware features have not been enabled yet for this board.

### Standard Peripheral Mapping:

#### Serial Ports

##### UART_1 -  (ST-Link Virtual Port Com)

- TX - PA9
- RX - PA10

##### UART_4  -  (Arduino Serial)

- TX - PH13
- RX - PH14

#### **Touch Screen - FT5336**

##### I2C4 

- SCL - PD12
- SDA - PD13
- Touch Screen Interrupt - PH2

#### **Arduino SPI**

##### SPI2

- SCK - PA12
- MISO - PB14
- MOSI - PB15
- NSS - PI0

#### LEDS

- LD1 : PG11
- LD2 : PG2

#### USER Push Button

- PC13

#### SDMMC

- D0 - PC8
- D1 - PC9
- D2 - PC10
- D3 - PC11
- CK - PC12
- CMD - PD2
- CD - PI8

#### CANFD

- RX  - PA11
- TX - PA12
- WAKE 1 - PH8

#### FMC SDRAM

#### LTDC

#### System Clock

The STM32H7B3I System Clock can be driven by an internal or external oscillator, as well as by the main PLL clock. By default, the System clock is driven by the PLL clock at 280MHz. PLL clock is fed by a 24MHz high speed external clock.

#### Serial Port

The STM32H7B3I Discovery kit has up to 8 UARTs. The Zephyr console output is assigned to UART1 which connected to the onboard ST-LINK/V3.0. Virtual COM port interface. Default communication settings are 115200 8N1.

### Programming and Debugging

See [Building an Application](https://docs.zephyrproject.org/latest/develop/application/index.html#build-an-application) for more information about application builds.

### Flashing

Flashing operation will depend on the target to be flashed and the SoC option bytes configuration. It is advised to use [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) to check and update option bytes configuration and flash the `stm32h7b3i_dk` target.

### Debugging

You can debug an application in the usual way. Here is an example for the [Hello World](https://docs.zephyrproject.org/latest/samples/hello_world/README.html#hello-world) application.

```
# From the root of the zephyr repository
west build -b stm32h7b3i_dk samples/hello_world
west debug
```

To use CAN, solder bridges SB3, SB4 and SB5 need to be connected. Take note that CANFD pins are shared with STMOD+ connector (P1), so please check [STM32H7B3I_DK board schematics](https://www.st.com/resource/en/schematic_pack/mb1332-h7b3i-c02_schematic.pdf) for possible collisions if using that connector.



