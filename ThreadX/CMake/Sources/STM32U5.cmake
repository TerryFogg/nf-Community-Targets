#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Common
     list(APPEND STM32U5_INCLUDES 
             ${TARGET_GIT_LOCAL_PATH}/Drivers/BSP/Components/Common
             ${TARGET_GIT_LOCAL_PATH}/Drivers/CMSIS/Device/ST/STM32U5xx/Include
             ${TARGET_GIT_LOCAL_PATH}/Drivers/CMSIS/Include
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Inc
             ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Inc/Legacy
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/STM32
      )
      list(APPEND STM32U5_SOURCES
            ${TARGET_GIT_LOCAL_PATH}/Drivers/CMSIS/Device/ST/STM32U5xx/Source/Templates/system_stm32u5xx.c

            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_adc.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_comp.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_cordic.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_crc.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_crs.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dac.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dlyb.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dma.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dma2d.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_exti.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_fmac.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_fmc.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_gpio.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_i2c.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_icache.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_lpgpio.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_lptim.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_lpuart.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_opamp.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_pka.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_pwr.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_rcc.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_rng.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_rtc.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_sdmmc.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_spi.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_tim.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_ucpd.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_usart.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_usb.c
            ${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_utils.c
      )
                     
# Overide compile flags on selected files
 set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/Drivers/STM32U5xx_Hal_Driver/src/stm32u5xx_ll_dac.c  COMPILE_FLAGS -Wno-unused-parameter)
 set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/Drivers/CMIS/src/Device/ST/STM32U5XX/Include/stm32u5xx.h COMPILE_FLAGS -Wno-unused-value)
