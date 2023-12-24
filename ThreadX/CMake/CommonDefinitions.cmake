#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Common compiler definitions
# --------------------------------------------------------------
set(CMAKE_C_STANDARD 17 CACHE INTERNAL "C standard for all targets")
set(CMAKE_C_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")
set(CMAKE_CXX_STANDARD 17 CACHE INTERNAL "C++ standard for all targets")
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")

target_compile_definitions(nanoCLR.elf PUBLIC -D${TARGET_SERIES})
target_compile_definitions(nanoCLR.elf PUBLIC -DDEBUG)
target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_ARM)
target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_LIGHT_MATH=FALSE)
target_compile_definitions(nanoCLR.elf PUBLIC -DDP_FLOATINGPOINT=TRUE)
target_compile_definitions(nanoCLR.elf PUBLIC -DHAS_CONFIG_BLOCK=TRUE)
target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_SYSTEM_COLLECTIONS=TRUE)
target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_REFLECTION=TRUE)
target_compile_definitions(nanoCLR.elf PUBLIC -DHAL_USE_RTC=FALSE)
target_compile_definitions(nanoCLR.elf PUBLIC -DHAL_USE_WDG=FALSE)
target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_NO_CLR_TRACE)

target_compile_definitions(nanoCLR.elf PUBLIC -DTARGET_HAS_NANOBOOTER=FALSE)

target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_GRAPHICS=${GRAPHICS_SUPPORT})



# Split BUILD_VERSION into VERSION_MAJOR;VERSION_MINOR;VERSION_BUILD;VERSION_REVISION 
string(REPLACE "." " " BUILD_VERSION_SPACED ${BUILD_VERSION})
separate_arguments(VERSION_LIST UNIX_COMMAND "${BUILD_VERSION_SPACED}")
list (GET VERSION_LIST 0 VERSION_MAJOR)
list (GET VERSION_LIST 1 VERSION_MINOR)
list (GET VERSION_LIST 2 VERSION_BUILD)
list (GET VERSION_LIST 3 VERSION_REVISION)

target_compile_definitions(nanoCLR.elf PUBLIC -DVERSION_MAJOR=${VERSION_MAJOR})
target_compile_definitions(nanoCLR.elf PUBLIC -DVERSION_MINOR=${VERSION_MINOR})
target_compile_definitions(nanoCLR.elf PUBLIC -DVERSION_BUILD=${VERSION_BUILD})
target_compile_definitions(nanoCLR.elf PUBLIC -DVERSION_REVISION=${VERSION_REVISION})
target_compile_definitions(nanoCLR.elf PUBLIC -DTARGETINFOSTRING=\"${INFOSTRING}\")


# !!!!!!!!!! NOTE
# Set build type here as buildpresets, inherits in CMakePresets.json does not appear to be working
set(CMAKE_BUILD_TYPE "Debug")

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    target_compile_definitions(nanoCLR.elf PUBLIC -DBUILD_RTM)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") 
    target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_ENABLE_SOURCELEVELDEBUGGING)
endif()


# ADVANCED
# --------
# Defines used in the c/c++ code in rare situations to debug and profile
# Unsure if APPDOMAINS works, never seen it implemented
# ---------------------------------------------------------------------------------
#target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_APPDOMAINS)
#target_compile_definitions(nanoCLR.elf PUBLIC -DSUPPORT_ANY_BASE_CONVERSION=TRUE)
#target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_NO_IL_INLINE=1)
#target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_NO_CLR_TRACE=1)
#target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_PROFILE_NEW_CALLS=1)
#target_compile_definitions(nanoCLR.elf PUBLIC -DPROFILE_NEW_ALLOCATIONS=1)
#target_compile_definitions(nanoCLR.elf PUBLIC -DTRACE_MEMORY_STATS=1)
