#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)

include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources_nf-Interpreter.cmake)
include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources_ThreadX.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Sources_${TARGET_FAMILY}.cmake)
include(../${PARENT_DIRECTORY}/${TARGET_BOARD}/board.cmake)

add_executable(nanoCLR.elf ${NF_INTERPRETER_SOURCES}
                           ${THREADX_SOURCES}
                           ${${TARGET_FAMILY}_SOURCES}
)

target_include_directories(nanoCLR.elf PUBLIC ${NF_INTERPRETER_INCLUDES}
                                              ${THREADX_INCLUDES}
                                              ${${TARGET_FAMILY}_INCLUDES}
)

include( BuildParameters_Common.cmake)
include( BuildParameters_${TARGET_FAMILY}.cmake)

