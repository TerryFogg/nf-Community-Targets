#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

    list(APPEND NF_INTERPRETER_TARGET_SOURCES
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/tx_initialize_low_level.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/startup_stm32u5a9xx.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/FlashDriver.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/Clock_configuration.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/target_common.c
    )
