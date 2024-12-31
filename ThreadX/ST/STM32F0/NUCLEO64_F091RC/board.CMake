#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

    list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO64_F091RC/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO64_F091RC/board.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO64_F091RC/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO64_F091RC/Power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO64_F091RC/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/NUCLEO64_F091RC/tx_initialize_low_level.s
  )
