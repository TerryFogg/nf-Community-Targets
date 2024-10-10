# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# -------------------------------------------
# NOTE: See CMakePresets.json for all options
# -------------------------------------------
include(CMakePrintHelpers)
include(BuildParameters/Repositories.cmake)

if( "${TARGET_SERIES}" STREQUAL "RP2040" OR "${TARGET_SERIES}" STREQUAL "RP2350")
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 17)
    SET(PICO_TOOLCHAIN_PATH "C:/nftools/GNU_Tools_ARM_Embedded/13.2.rel1")
    Set(PICO_LINKER_SCRIPT_PATH  "${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}")
 include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
endif()

project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)
 
if( "${TARGET_SERIES}" STREQUAL "RP2040" OR "${TARGET_SERIES}" STREQUAL "RP2350")
    pico_sdk_init()
endif()

# Sources
    include(../${PARENT_DIRECTORY}/${TARGET_BOARD}/board.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/ThreadX.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/${TARGET_FAMILY}.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.options.cmake)

    add_executable(nanoCLR ${NF_INTERPRETER_SOURCES}
                               ${THREADX_SOURCES}
                               ${OPTIONAL_SOURCES}
                               ${${TARGET_FAMILY}_SOURCES}
    )
    target_include_directories(nanoCLR PUBLIC ${NF_INTERPRETER_INCLUDES}
                                              ${THREADX_INCLUDES}
                                              ${OPTIONAL_INCLUDES}
                                              ${${TARGET_FAMILY}_INCLUDES}
    )

if( "${TARGET_SERIES}" STREQUAL "RP2040" OR "${TARGET_SERIES}" STREQUAL "RP2350")
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/${TARGET_FAMILY}_Libraries.cmake)
endif()

# Build parameters
    include( BuildParameters/Common.cmake)
    include( BuildParameters/${TARGET_FAMILY}.cmake)
