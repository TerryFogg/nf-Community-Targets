# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# The project command must come later for the RP2XXX series
if( NOT ${TARGET_FAMILY} STREQUAL "RP2XXX")
  project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)
endif()

add_executable(nanoCLR)

# Sources
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/${TARGET_FAMILY}.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.options.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/ThreadX/ThreadX.cmake)
    include(../${PARENT_DIRECTORY}/${TARGET_VENDOR}/${TARGET_FAMILY}/${TARGET_BOARD}/board.cmake)

    target_sources(nanoCLR PUBLIC ${NF_INTERPRETER_SOURCES}
                                  ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/main.c
                                  ${THREADX_SOURCES}
                                  ${OPTIONAL_SOURCES}
                                  ${${TARGET_FAMILY}_SOURCES}
    )

# Includes
    target_include_directories(nanoCLR PUBLIC ${NF_INTERPRETER_INCLUDES}
                                              ${THREADX_INCLUDES}
                                              ${OPTIONAL_INCLUDES}
                                              ${${TARGET_FAMILY}_INCLUDES}
    )

# Build parameters
    include( BuildParameters/Common.cmake)
    include( BuildParameters/${TARGET_FAMILY}.cmake)
