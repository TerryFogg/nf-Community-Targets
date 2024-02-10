
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# -------------------------------------------
# NOTE: See CMakePresets.json for all options
# -------------------------------------------
project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)

# Sources
include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.cmake)
include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.options.cmake)
include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/ThreadX.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/${TARGET_FAMILY}.cmake)
include(../${PARENT_DIRECTORY}/${TARGET_BOARD}/board.cmake)

add_executable(nanoCLR.elf ${NF_INTERPRETER_SOURCES}
                           ${THREADX_SOURCES}
                           ${${TARGET_FAMILY}_SOURCES}
)
target_include_directories(nanoCLR.elf PUBLIC ${NF_INTERPRETER_INCLUDES}
                                              ${THREADX_INCLUDES}
                                              ${${TARGET_FAMILY}_INCLUDES}
)

# Build parameters
include( BuildParameters/Common.cmake)
include( BuildParameters/${TARGET_FAMILY}.cmake)
