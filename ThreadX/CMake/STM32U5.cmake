#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
  # Setup STM32U5 Cube Sources
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

 list(APPEND STM32U5_LL_INCLUDES 
             ${STM32U5_CUBE_SOURCE}/src/boards/include
             ${STM32U5_CUBE_SOURCE}/Drivers/CMSIS/Include
             ${STM32U5_CUBE_SOURCE}/Drivers/CMSIS/Device/ST/STM32U5xx/Include
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Inc
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Inc/Legacy
 )
 list(APPEND STM32U5_LL_INCLUDES

             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_adc.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_comp.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_cordic.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_crc.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_crs.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_dac.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_dlyb.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_dma.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_dma2d.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_exti.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_fmac.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_fmc.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_gpio.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_icache.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_i2c.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_lpgpio.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_lptim.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_lpuart.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_opamp.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_pka.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_pwr.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_rcc.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_rng.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_rtc.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_sdmmc.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_spi.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_tim.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_ucpd.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_usart.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_usb.c
             ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_Hal_Driver/src/STM32U5xx_ll_utils.c
 )



