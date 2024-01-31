#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

    list(APPEND NF_INTERPRETER_TARGET_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/tx_initialize_low_level.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/startup_stm32h743xx.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/Clock_configuration.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO_H743ZI2/target_common.c
    )
