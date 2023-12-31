#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

     list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/Clock_configuration.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/FlashDriver.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/Initialize_Graphics.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/low_level_i2c.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/MicroSD.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/OSPI_memory.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/startup_stm32h7b3xx.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/tx_initialize_low_level.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/STM32/${DISPLAY_INTERFACE}
    )
