#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
    list(APPEND NF_INTERPRETER_SOURCES
                ${STM32U5_CUBE_SOURCE}/Drivers/BSP/STM32U5x9J-DK
    )
    list(APPEND NF_INTERPRETER_SOURCES
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/clock_configuration.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/startup_stm32u5a9njhxq.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/tx_initialize_low_level.S
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/one_millisecond_tick.c

                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/Initialize_Graphics.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/LTDC_DSI_DMA2D_Controller.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/Stm32U5a9_Video_Controller.cpp
                                       
    )


   set_source_files_properties(${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/stm32_lcd.c COMPILE_FLAGS -Wno-sign-compare)
   set_source_files_properties(${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/test.c COMPILE_FLAGS -Wno-unused-parameter)
