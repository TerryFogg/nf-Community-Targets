#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Check and find source repository
 if(IS_DIRECTORY ${STM32F0_CUBE_SOURCE})
     message(STATUS "Source code for STM32F0 support is local folder ${STM32F0_CUBE_SOURCE}")
     FetchContent_Declare(
         stm32F0
         SOURCE_DIR ${STM32F0_CUBE_SOURCE}
     )
 else()
       message(STATUS "Source code for STM32F0 support from GitHub repo version ${CUBE_VERSION}")
     FetchContent_Declare(
         stm32F0
         GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF0.git
         GIT_TAG ${CUBE_VERSION}
     )
 endif()
 FetchContent_Populate(stm32F0)

# Common
     list(APPEND STM32F0_INCLUDES 
             ${STM32F0_CUBE_SOURCE}/src/boards/include
             ${STM32F0_CUBE_SOURCE}/Drivers/CMSIS/Include
             ${STM32F0_CUBE_SOURCE}/Drivers/CMSIS/Device/ST/STM32F0xx/Include
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Inc
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy
             ${STM32F0_CUBE_SOURCE}/Drivers/BSP/Components/Common
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/STM32
     )
      list(APPEND STM32F0_SOURCES
             ${STM32F0_CUBE_SOURCE}/Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/system_stm32F0xx.c

             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_adc.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_comp.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_crc.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_crs.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_dac.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_dma.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_exti.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_gpio.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_i2c.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_pwr.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_rcc.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_rtc.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_spi.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_tim.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_usart.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_usb.c
             ${STM32F0_CUBE_SOURCE}/Drivers/STM32F0xx_HAL_Driver/Src/stm32F0xx_ll_utils.c
      )

# Overide compile flags on selected files
                                           