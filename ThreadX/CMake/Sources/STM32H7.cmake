#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
# Check and find source repository
include(FetchContent)
 if(IS_DIRECTORY ${TARGET_GIT_LOCAL_PATH})
     message(STATUS "Source code support is local folder ${TARGET_GIT_LOCAL_PATH}")
     FetchContent_Declare(
         Repository
         SOURCE_DIR ${TARGET_GIT_LOCAL_PATH}
     )
 else()
       message(STATUS "Source code from GitHub repo version ${TARGET_GIT_REPOSITORY}")
     FetchContent_Declare(
         Repository
         GIT_REPOSITORY  ${TARGET_GIT_REPOSITORY}
         GIT_TAG ${TARGET_GIT_TAG}
     )
 endif()
 FetchContent_MakeAvailable(Repository)

#
# Common
     list(APPEND STM32H7_INCLUDES 
             ${TARGET_GIT_LOCAL_PATH}/src/boards/include
             ${TARGET_GIT_LOCAL_PATH}/Drivers/CMSIS/Include
             ${TARGET_GIT_LOCAL_PATH}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Inc
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
             ${TARGET_GIT_LOCAL_PATH}/Drivers/BSP/Components/Common
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/Common
     )
      list(APPEND STM32H7_SOURCES
             ${TARGET_GIT_LOCAL_PATH}/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/system_stm32h7xx.c

             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_adc.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_bdma.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_comp.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_cordic.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_crc.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_crs.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_dac.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_delayblock.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_dma.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_dma2d.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_exti.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_fmac.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_fmc.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_gpio.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_hrtim.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_i2c.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_lptim.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_lpuart.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_mdma.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_opamp.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_pwr.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_rcc.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_rng.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_rtc.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_sdmmc.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_spi.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_swpmi.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_tim.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_usart.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_usb.c
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_utils.c
      )

# Overide compile flags on selected files
 set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_hrtim.c COMPILE_FLAGS -Wno-unused-parameter)
 set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_rng.c COMPILE_FLAGS -Wno-unused-variable)
 set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_ll_rng.c COMPILE_FLAGS -Wno-unused-parameter)
