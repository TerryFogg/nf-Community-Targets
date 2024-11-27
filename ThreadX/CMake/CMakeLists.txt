# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# -------------------------------------------
# NOTE: See CMakePresets.json for all options
# -------------------------------------------
include(CMakePrintHelpers)
include(BuildParameters/Repositories.cmake)

if( "${TARGET_SERIES}" STREQUAL "RP2040" OR "${TARGET_SERIES}" STREQUAL "RP2350")
    set(PICO_NO_CMSE true)
    Set(PICO_LINKER_SCRIPT_PATH  "${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}")
    include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
    project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)
    pico_sdk_init()
else()
    project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)
endif()

# Sources
    include(../${PARENT_DIRECTORY}/${TARGET_BOARD}/board.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/ThreadX.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/${TARGET_FAMILY}.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.options.cmake)

    add_executable(nanoCLR ${NF_INTERPRETER_SOURCES}
                               ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/main.c
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


    # Function to print all variables
function(print_all_variables)
    message(STATUS "CMake variables at configuration time:")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

# Call the function to print variables
print_all_variables()

