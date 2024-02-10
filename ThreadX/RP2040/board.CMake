#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

    list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/bs2_default_padded_checksummed.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/Initialize_Graphics.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/Power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/tx_initialize_low_level.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/usb_descriptors.c
  )
