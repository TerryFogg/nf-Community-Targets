#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
    list(APPEND NF_INTERPRETER_SOURCES
                ${STM32U5_CUBE_SOURCE}/Drivers/BSP/STM32U5x9J-DK
    )

    list(APPEND NF_INTERPRETER_SOURCES
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/Initialize_Graphics.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/stm32_lcd.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/startup_stm32u5a9xx.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/target_common.c

                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/font8.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/font12.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/font16.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/font20.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/font24.c

                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/TestGraphicsU5.c

                ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma2d.c
                ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dsi.c
                ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c
                ${STM32U5_CUBE_SOURCE}/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ltdc.c

    )

  set_source_files_properties(${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/TestGraphicsU5.c COMPILE_FLAGS -Wno-unused-parameter)

                    