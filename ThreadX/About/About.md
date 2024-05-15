# Community contribution

These folders contain code and notes for a number of boards 

>Note: This is not an official port, you can ask questions about it on Discord .

### CMakeLists at the top level
The CMake build for this community edition is fully contained in the ThreadX/CMake directory

For the community target ThreadX all build options are accessed through CMakePresets.json in the ThreadX CMake directory

```C:\nf-interpreter\targets-community\ThreadX\CMake\CMakePresets.json```

The following code fragment can be found in the top level CMakeLists.txt to bypass all of the standard CMake build system

```json
########################################################
if ("${COMMUNITY_TARGET}" STREQUAL "ThreadX")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/targets-community/ThreadX/CMake)
    return()
endif()
########################################################
```


### Directory layout for targets-community/ThreadX

 
    ├── About                        # Documentation relating to maintaining and building
    ├── CMake                        # All common CMake related files other than the specific board CMake file.
    │   ├── BuildParameters          # Common build parameters for GCC compiler
    │   ├── Sources                  # Sources required by function of MCU family
    │   │   BuildPresets.json        # All of the different optional build configurations
    │   │   CMakeLists.txt           # Main CMakeLists.txt
    │   │   CmakePresets.json        # Build options and parameter setup
    ├── Common                       # Any code common to ThreadX that runs on all MCU's
    ├── Commonality                  # Any code common to ThreadX that runs on an MCU family
    ├── NUCLEO_H743ZI2               # NUCLEO board from STMicroelectronics
    └── RP2040                       # Rapsberry Pi Pico
    └── STM32H7B3I_DK                # Development board from STMicroelectronics based on the STM32H7b3 MCU
    └── STM32H735G_DK                # Development board from STMicroelectronics based on the STM32H735 MCU
    └── STM32U5A9J_DK                # Development board from STMicroelectronics based on the STM32U5A9 MCU
    └── main.c                       # main entry point
    

---

## Setup and Installation

### Build ToolChain
Download version 13.2.rel1 of GNU_Tools_ARM_Embedded from ```https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads/13-2-rel1```


### Get Repositories
 | Repository             |Version  | Command                                                                                    
 | -----------            |         |:-------------------------------------------------------------------------------------------
 | Eclipse ThreadX        |         |```git clone https://github.com/eclipse-threadx/threadx.git```                                                                                           |
 | Eclipse FileX          |         |```git clone https://github.com/eclipse-threadx/filex.git```                                                                                           |
 | Eclipse NetXDuo        |         |```git clone https://github.com/eclipse-threadx/netxduo.git```                                                                                           |
 | Eclipse UsbX           |         |```git clone https://github.com/eclipse-threadx/usbx.git```                                                                                           |
 | Eclipse LevelX         |         |```git clone https://github.com/eclipse-threadx/levelx.git```                                                                                           |
 | STM32CubeF7            |         |```git clone --recurse-submodules https://github.com/STMicroelectronics/STM32CubeF7.git```  
 | STM32CubeH7            |         |```git clone --recurse-submodules https://github.com/STMicroelectronics/STM32CubeH7.git```  
 | STM32CubeU5            |         |```git clone --recurse-submodules https://github.com/STMicroelectronics/STM32CubeU5.git```  



 # Reference Material

 # RTOS
 [ThreadX and components](https://github.com/eclipse-threadx/rtos-docs/tree/main)

 


 ## STM32
  [Using the SDMMC interface on the STM32H7](https://www.st.com/resource/en/application_note/an5200-getting-started-with-stm32h7-mcus-sdmmc-host-controller-stmicroelectronics.pdf)

 [WIKI on ThreadX,FileX,NetXDuo,LevelX,USBx](https://wiki.st.com/stm32mcu/wiki/Category:STM32_middleware)

  ## Raspberry Pi Pico RP2040
