#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
    list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/${TARGET_SERIES}/board.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/${TARGET_SERIES}/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/${TARGET_SERIES}/Power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/${TARGET_SERIES}/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/${TARGET_SERIES}/tx_initialize_low_level.S
    )

#    set_source_files_properties(${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/RP2040/tx_initialize_low_level.s COMPILE_FLAGS -std=gnu17)
