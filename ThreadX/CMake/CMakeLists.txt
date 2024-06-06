
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

# -------------------------------------------
# NOTE: See CMakePresets.json for all options
# -------------------------------------------
include(CMakePrintHelpers)

project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)

# Sources
    include(../${PARENT_DIRECTORY}/${TARGET_BOARD}/board.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/ThreadX.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/${TARGET_FAMILY}.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.options.cmake)

    add_executable(nanoCLR.elf ${NF_INTERPRETER_SOURCES}
                               ${THREADX_SOURCES}
                               ${OPTIONAL_SOURCES}
                               ${${TARGET_FAMILY}_SOURCES}
    )
    target_include_directories(nanoCLR.elf PUBLIC ${NF_INTERPRETER_INCLUDES}
                                                  ${THREADX_INCLUDES}
                                                  ${OPTIONAL_INCLUDES}
                                                  ${${TARGET_FAMILY}_INCLUDES}
    )

# Build parameters
    include( BuildParameters/Common.cmake)
    include( BuildParameters/${TARGET_FAMILY}.cmake)

# Display active settings
    MESSAGE(STATUS "")
    MESSAGE(STATUS "")
    MESSAGE(STATUS "-----VARIABLES-----")
    cmake_print_variables(SEMIHOSTING)
    cmake_print_variables(CMAKE_BUILD_TYPE)
    cmake_print_variables(CMAKE_C_COMPILER )
    cmake_print_variables(CMAKE_C_FLAGS_DEBUG)
    cmake_print_variables(CMAKE_CXX_COMPILER)
    cmake_print_variables(CMAKE_CXX_COMPILER_ID)
    cmake_print_variables(CMAKE_CXX_COMPILER_VERSION)    
    cmake_print_variables(CMAKE_CXX_FLAGS_DEBUG)
    cmake_print_variables(CMAKE_ASM_COMPILER)
    cmake_print_variables(CMAKE_LINKER)
    cmake_print_variables(CMAKE_C_COMPILE_FEATURES)
    cmake_print_variables(CMAKE_CXX_COMPILE_FEATURES)
   
    MESSAGE(STATUS "----------------------")
    MESSAGE(STATUS "")
    MESSAGE(STATUS "")
   