#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Check and find source repository
 if(IS_DIRECTORY ${STM32U5_CUBE_SOURCE})
     message(STATUS "Source code for STM32U5 support is local folder ${STM32U5_CUBE_SOURCE}")
     FetchContent_Declare(
         stm32u5_ll
         SOURCE_DIR ${STM32U5_CUBE_SOURCE}
     )
 else()
       message(STATUS "Source code for STM32U5 support from GitHub repo version ${CUBE_VERSION}")
     FetchContent_Declare(
         stm32u5_ll
         GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeU5.git
         GIT_TAG ${CUBE_VERSION}
     )
 endif()

 FetchContent_Populate(stm32u5_ll)

# LL Sources
     list(APPEND STM32U5_INCLUDES 
             ${STM32U5_CUBE_SOURCE}/Drivers/BSP/Components/Common
             ${STM32U5_CUBE_SOURCE}/Drivers/CMSIS/Device/ST/STM32U5xx/Include
             ${STM32U5_CUBE_SOURCE}/Drivers/CMSIS/Include
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Inc
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Inc/Legacy
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/STM32
      )
      list(APPEND STM32U5_SOURCES
            ${STM32U5_CUBE_SOURCE}/Drivers/CMSIS/Device/ST/STM32U5xx/Source/Templates/system_stm32u5xx.c

            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_adc.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_comp.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_cordic.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_crc.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_crs.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dac.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dlyb.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dma.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dma2d.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_exti.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_fmac.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_fmc.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_gpio.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_i2c.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_icache.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_lpgpio.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_lptim.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_lpuart.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_opamp.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_pka.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_pwr.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_rcc.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_rng.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_rtc.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_sdmmc.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_spi.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_tim.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_ucpd.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_usart.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_usb.c
            ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_utils.c
      )
                     
# Overide compile flags on selected files
 set_source_files_properties(${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/stm32u5xx_ll_dac.c  COMPILE_FLAGS -Wno-unused-parameter)
 set_source_files_properties(${STM32U5_CUBE_SOURCE}/Drivers/CMIS/src/Device/ST/STM32U5XX/Include/stm32u5xx.h COMPILE_FLAGS -Wno-unused-value)


 