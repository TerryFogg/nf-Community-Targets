# Welcome to .NET **nanoFramework** community targets port of STM32H7 boards

## Note: This is not an official port, you can ask questions about it on Discord .


##### Development environment : Visual Studio 2022 version 17.7.6
> This code was developed and debugged with CMake under Visual Studio 2022, and *__should__* also be able to be used with VsCode *(not tested)*  .
> 

The development environment was setup as follows. The nf-interpreter repository is cloned to __C:\nf-intepreter__
The additional support code is cloned to __C:\nfTools__

The following table outlines the additional sources required for the STM32H7 series builds using __ThreadX__ *(formerly AzureRTOS)* as the real-time operating system (RTOS).



| C:\nfTools\ |                            |                |                      |
| ----------- | :------------------------- | :------------- | :------------------- |
|             | **GNU_Tools_ARM_Embedded** | 12.3.rel       | arm-none-eabi        |
|             |                            |                | bin                  |
|             |                            |                | include              |
|             |                            |                | libexec              |
|             | **ThreadX**                | cmake          |                      |
|             |                            | common         |                      |
|             |                            | common_modules |                      |
|             |                            | common_smp     |                      |
|             |                            | docs           |                      |
|             |                            | out            |                      |
|             |                            | ports          |                      |
|             |                            | ports_module   |                      |
|             |                            | ports_smp      |                      |
|             |                            | sample         |                      |
|             |                            | utility        |                      |
|             | **STM32CubeH7**            | Driver         | BSP                  |
|             |                            |                | CMSIS                |
|             |                            |                | STM32H7xx_HAL_Driver |



The following Visual Studio file are required 

**tasks.json**

```json
{
  "version": "0.2.1",
  "tasks": [
    {
      "taskLabel": "Flash nanoCLR on STM32H7",
      "default": null,
      "appliesTo": "*.*",
      "command": "${env.COMSPEC}",
      "args": [
        "openocd -f interface/stlink.cfg  -f target/stm32h7x.cfg -c \"program build/nanoCLR.elf verify\" -c \"reset halt\" -c shutdown"
      ],
      "windows": {
        "options": {
          "shell": {
            "executable": "cmd.exe",
            "args": [ "/c" ]
          }
        }
      }
    }
  ]
}
```



launch.json

```json
{
  "version": "0.2.1",
  "configurations": [
    {
      "type": "cppdbg",
      "name": "STM32H735G_DISCOVERY",
      "program": "build/nanoCLR.elf",
      "projectTarget": "nanoCLR.elf",
      "project": "CMakeLists.txt",
      "request": "launch",
      "cwd": "${workspaceRoot}",
      "processName": "STM32H735G_DISCOVERY",
      "MIMode": "gdb",
      "miDebuggerPath": "c:/nfTools/GNU_Tools_ARM_Embedded/12.3.rel1/bin/arm-none-eabi-gdb.exe",
      "miDebuggerServerAddress": "localhost:3333",
      "debugServerPath": "C:/nftools/OpenOCD-20230621/bin/openocd.exe",
      "debugServerArgs": "-s \"c:/nfTools/openocd/scripts/\" -f interface/stlink.cfg -f board/stm32h735g-disco.cfg",
      "serverStarted": "Listening on port 3333 for gdb connections",
      "stopOnEntry": false,
      "postRemoteConnectCommands": [
        { "text": "monitor reset init" },
        { "text": "load" }
      ],
      "launchCompleteCommand": "exec-run",
      "filterStderr": true,
      "filterStdout": true,
      "logging": {
        "engineLogging": true,
        "trace": true,
        "traceResponse": true
      }
    }
  ]
}
```






# STM32H735G-DK Discovery

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

| Interface | Controller | Driver/Component                           |
| --------- | ---------- | ------------------------------------------ |
| NVIC      | on-chip    | nested vector interrupt controller         |
| UART      | on-chip    | serial port-polling; serial port-interrupt |
| PINMUX    | on-chip    | pinmux                                     |
| GPIO      | on-chip    | gpio                                       |
| FLASH     | on-chip    | flash memory                               |
| ETHERNET  | on-chip    | ethernet                                   |
| RNG       | on-chip    | True Random number generator               |
| FMC       | on-chip    | memc (SDRAM)                               |
| ADC       | on-chip    | ADC Controller                             |

### Pin Mapping

For mode details please refer to [STM32H735G-DISCO website](https://www.st.com/en/evaluation-tools/stm32h735g-dk.html).

#### Peripheral Mapping:

- UART_3 TX/RX : PD8/PD9 (ST-Link Virtual Port Com)
- UART_7 TX/RX : PF7/PF6 (Arduino Serial)
- LD1 : PC2
- LD2 : PC3

### System Clock

The STM32H735G System Clock can be driven by an internal or external oscillator, as well as by the main PLL clock. By default, the System clock is driven by the PLL clock at 550MHz. PLL clock is feed by a 25MHz high speed external clock.

### Serial Port

The STM32H735G Discovery kit has up to 6 UARTs, UART3 which connected to the onboard ST-LINK/V3.0. Virtual COM port interface.

## Programming and Debugging

### Flashing

Flashing operation will depend on the target to be flashed and the SoC option bytes configuration. It is advised to use [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) to check and update option bytes configuration and flash the `stm32h735g_disco` target.

