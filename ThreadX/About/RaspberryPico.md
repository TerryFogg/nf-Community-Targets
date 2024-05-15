# Welcome to .NET **nanoFramework** community targets port of STM32H7 boards



# Raspberry Pi Pico

## Overview

The Raspberry Pi Pico and Pico W are small, low-cost, versatile boards from Raspberry Pi. They are equipped with an RP2040 SoC, an on-board LED, a USB connector, and an SWD interface. The Pico W additionally contains an Infineon CYW43439 2.4 GHz Wi-Fi/Bluetoth module. The USB bootloader allows the ability to flash without any adapter, in a drag-and-drop manner. It is also possible to flash and debug the boards with their SWD interface, using an external adapter.

### RP2040 Device

- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Hardware design with RP2040](https://datasheets.raspberrypi.com/rp2040/hardware-design-with-rp2040.pdf)
- [Raspberry Pi Pico Datasheet](https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf)
- [Getting started with Raspberry Pi Pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
- [Raspberry Pi Pico W Datasheet](https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf)
- [Connecting to the Internet with Raspberry Pi Pico W](https://datasheets.raspberrypi.com/picow/connecting-to-the-internet-with-pico-w.pdf)
- [Connecting to the Internet with Raspberry Pi Pico W](https://datasheets.raspberrypi.com/picow/connecting-to-the-internet-with-pico-w.pdf) book.
- [Raspberry Pi Pico C/C++ SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
- [Pico tool Github repository](https://github.com/raspberrypi/picotool).

## Hardware

- Dual core Arm Cortex-M0+ processor running up to 133MHz
- 264KB on-chip SRAM
- 2MB on-board QSPI flash with XIP capabilities
- 26 GPIO pins
- 3 Analog inputs
- 2 UART peripherals
- 2 SPI controllers
- 2 I2C controllers
- 16 PWM channels
- USB 1.1 controller (host/device)
- 8 Programmable I/O (PIO) for custom peripherals
- On-board LED
- 1 Watchdog timer peripheral
- Infineon CYW43439 2.4 GHz Wi-Fi chip (Pico W only)

![Raspberry Pi Pico](https://docs.zephyrproject.org/latest/_images/rpi_pico.jpg)

![Raspberry Pi Pico W](https://docs.zephyrproject.org/latest/_images/rpi_pico_w.jpg)

*Raspberry Pi Pico (above) and Pico W (below) (Images courtesy of Raspberry Pi)*

### Supported Features

The rpi_pico board configuration supports the following hardware features:

| Peripheral           | STATUS         |
| -------------------- | -------------- |
| NVIC                 | N/A            |
| UART                 | In Development |
| GPIO                 | In Development |
| ADC                  | In Development |
| I2C                  | In Development |
| SPI                  | In Development |
| USB Device           | In Development |
| HWINFO               | In Development |
| Watchdog Timer (WDT) | In Development |
| PWM                  | In Development |
| Flash                | In Development |
| UART (PIO)           | In Development |
| SPI (PIO)            | In Development |



### Pin Mapping

The peripherals of the RP2040 SoC can be routed to various pins on the board. The configuration of these routes can be modified through DTS. Please refer to the datasheet to see the possible routings for each peripheral.

External pin mapping on the Pico W is identical to the Pico, but note that internal RP2040 GPIO lines 23, 24, 25, and 29 are routed to the Infineon module on the W. Since GPIO 25 is routed to the on-board LED on the Pico, but to the Infineon module on the Pico W, the “blinky” sample program does not work on the W (use hello_world for a simple test program instead).

#### Default Peripheral Mapping:

- UART0_TX : P0
- UART0_RX : P1
- I2C0_SDA : P4
- I2C0_SCL : P5
- I2C1_SDA : P14
- I2C1_SCL : P15
- SPI0_RX : P16
- SPI0_CSN : P17
- SPI0_SCK : P18
- SPI0_TX : P19
- ADC_CH0 : P26
- ADC_CH1 : P27
- ADC_CH2 : P28
- ADC_CH3 : P29

## Programmable I/O (PIO)

The RP2040 SoC comes with two PIO periherals. These are two simple co-processors that are designed for I/O operations. The PIOs run a custom instruction set, generated from a custom assembly language. PIO programs are assembled using pioasm, a tool provided by Raspberry Pi.

Future use

## Programming

#### UF2

You can flash the Raspberry Pi Pico with a UF2 file.  If the Pico is powered on with the BOOTSEL button pressed, it will appear on the host as a mass storage device. The nanoCLR.uf2  file should be drag-and-dropped to the device, which will flash the nanoCLR onto the device.

 
