#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

 # Setup STM32H7 Cube Sources
 if(IS_DIRECTORY ${STM32H7_CUBE_SOURCE})
     message(STATUS "Source code for STM32H7 support is local folder ${STM32H7_CUBE_SOURCE}")
     FetchContent_Declare(
         stm32h7
         SOURCE_DIR ${STM32H7_CUBE_SOURCE}
     )
 else()
       message(STATUS "Source code for STM32H7 support from GitHub repo version ${CUBE_VERSION}")
     FetchContent_Declare(
         stm32h7
         GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeH7.git
         GIT_TAG ${CUBE_VERSION}
     )
 endif()
 FetchContent_Populate(stm32h7)

 list(APPEND STM32H7_INCLUDES 
             ${STM32H7_CUBE_SOURCE}/src/boards/include
             ${STM32H7_CUBE_SOURCE}/Drivers/CMSIS/Include
             ${STM32H7_CUBE_SOURCE}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_HAL_Driver/Inc
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
 )
 list(APPEND STM32H7_SOURCES
             ${STM32H7_CUBE_SOURCE}/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/system_stm32h7xx.c

             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_adc.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_bdma.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_comp.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_cordic.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_crc.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_crs.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_dac.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_delayblock.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_dma.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_dma2d.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_exti.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_fmac.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_fmc.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_gpio.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_hrtim.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_i2c.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_lptim.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_lpuart.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_mdma.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_opamp.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_pwr.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_rcc.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_rng.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_rtc.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_sdmmc.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_spi.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_swpmi.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_tim.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_usart.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_usb.c
             ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_utils.c
)

 # Set flags on code that is supplied by others but produces warning/errors with current build parameters
 set_source_files_properties(${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_hrtim.c COMPILE_FLAGS -Wno-unused-parameter)
 set_source_files_properties(${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_rng.c COMPILE_FLAGS -Wno-unused-variable)
 set_source_files_properties(${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_rng.c COMPILE_FLAGS -Wno-unused-parameter)

