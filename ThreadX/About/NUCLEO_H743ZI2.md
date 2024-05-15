# NUCLEO-H743ZI2

## Overview

The **NUCLEO-H743ZI2** is a **development board** designed by **STMicroelectronics**. 



The board features the **STM32H743ZI** microcontroller and has the following.

## Key features 

- Support for Arduino shields.
- ST Zio connector
- ST morpho headers - expansion with specialized shields.
- ST-LINK debugger/programmer.
- Power supply via ST-LINK USB V BUS, USB connector, or external sources.
- Ethernet compliant with IEEE-802.3-2002.
- USB Device only, USB OTG full speed, or SNK/UFP (full-speed or high-speed mode).
- 3 user LEDs
- 2 user and reset push-buttons
- 32.768 kHz crystal oscillator
- Board connectors:

> - USB with Micro-AB
> - SWD
> - Ethernet RJ45 (depending on STM32 support)
> - ST Zio connector including Arduino* Uno V3
> - ST morpho

![image-20240311130019113](assets/image-20240311130019113.png)

## Physical memory

| Type       | Size  | Description                                                  |
| ---------- | ----- | ------------------------------------------------------------ |
| FLASH A    | 1024K | On chip flash (Bank 1)                                       |
| FLASH B    | 1024K | On chip flash (Bank 2)                                       |
| ITCM_SRAM  | 64K   | Tightly coupled instruction RAM (Can be used for critical real-times routines) |
| DTCM_SRAM  | 128K  | Tightly coupled data RAM (Can be used for critical real-time data, such as interrupt service routines or stack/heap memory) |
| AXI_RAM    | 512K  | RAM on 64-bit AXI bus (Power Domain 1)                       |
| SRAM1      | 128K  | RAM on 32-bit AHB bus (Power Domain 2)                       |
| SRAM2      | 128K  | RAM on 32-bit AHB bus (Power Domain 2)                       |
| SRAM3      | 32K   | RAM on 32-bit AHB bus (Power Domain 3)                       |
| SRAM4      | 64K   | RAM on 32-bit AHB bus (Power Domain 3)                       |
| BACKUP_RAM | 4k    | Retained in standby or backup mode                           |

## nanoFramework memory usage

| Type       | Size  | nanoFramework usage                   |
| ---------- | ----- | ------------------------------------- |
| FLASH      | 640K  | Reserved for Native Image             |
| FLASH      | 1408K | Reserved for nanoFramework deployment |
| ITCM_SRAM  | 64K   | *Future use*                          |
| DTCM_SRAM  | 128K  | Native image stack and future use     |
| AXI_RAM    | 512K  | Managed code available RAM            |
| SRAM1      | 128K  | DMA Accessible RAM                    |
| SRAM2      | 128K  | *Future use*                          |
| SRAM3      | 32K   | *Future use*                          |
| SRAM4      | 64K   | *Future use*                          |
| BACKUP_RAM | 4k    | *Future use*                          |

## nanoFramework supported features

| Interface        |                                                              |
| ---------------- | ------------------------------------------------------------ |
|                  |                                                              |
| UART             | In development                                               |
|                  |                                                              |
| GPIO             | In development                                               |
| RTC              | In development                                               |
| I2C              | In development ( PB8,PB9 )                                   |
| PWM              | In development                                               |
| ADC              | In development ( INP15 - PA3)                                |
| DAC              | In development (PA4)                                         |
| RNG              | In development                                               |
| ETHERNET         | In development ( PA1, PA2, PA7, PB13, PC1, PC4, PC5, PG11, PG13) |
| SPI              | In development (NSS/SCK/MISO/MOSI : PD14/PA5/PA6/PB5 (Arduino SPI)) |
| Backup SRAM      | In development                                               |
| WATCHDOG         | In development                                               |
| USB              | In development                                               |
| CAN/CANFD        | In development (PD0, PD1)                                    |
| LD1              | PB0                                                          |
| LD2              | PB7                                                          |
| LD3              | PB14                                                         |
| User Push Button | PC13                                                         |
| USART3- VCP      | PD8,PD9                                                      |
|                  |                                                              |
| die-temp         | In development                                               |



## STM32H743ZI bus connections

------

![image-STM32H743ZI-Internals](assets/image-STM32H743ZI-Internals.png)