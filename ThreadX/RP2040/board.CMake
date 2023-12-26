#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

    list(APPEND NF_INTERPRETER_TARGET_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H735G_DK/BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H735G_DK/board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H735G_DK/bs2_default_padded_checksummed.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H735G_DK/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H735G_DK/FlashDriver.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H735G_DK/Power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H735G_DK/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H735G_DK/tx_initialize_low_level.s
   )

