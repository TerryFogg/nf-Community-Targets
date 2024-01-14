#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
# See CMakePresets.json for all options

project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)

# nanoFramework Sources
include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources_nf-Interpreter.cmake)

# ThreadX RTOS sources
include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources_ThreadX.cmake)

# Sources related to the MCU processor family
include(${CMAKE_CURRENT_SOURCE_DIR}/Sources_${TARGET_FAMILY}.cmake)

# Sources specific to the target board
include(../${PARENT_DIRECTORY}/${TARGET_BOARD}/board.cmake)

add_executable(nanoCLR.elf ${NF_INTERPRETER_SOURCES}
                           ${THREADX_SOURCES}
                           ${${TARGET_FAMILY}_SOURCES}
)

target_include_directories(nanoCLR.elf PUBLIC ${NF_INTERPRETER_INCLUDES}
                                              ${THREADX_INCLUDES}
                                              ${${TARGET_FAMILY}_INCLUDES}
)

# Common Defines across all MCU builds
include( BuildParameters_Common.cmake)

# Build parameters related to the MCU processor family
include( BuildParameters_${TARGET_FAMILY}.cmake)

