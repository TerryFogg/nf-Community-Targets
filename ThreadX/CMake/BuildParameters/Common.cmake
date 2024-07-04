#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

 # TODO: Set build type here as buildpresets, inherits in CMakePresets.json does not appear to be working
    set(CMAKE_BUILD_TYPE "Debug")

# This build does not support a nanbooter, code starts in the CLR
    target_compile_definitions(nanoCLR.elf PUBLIC -DTARGET_HAS_NANOBOOTER=FALSE)

# Reference the linker script
    target_link_options(nanoCLR.elf PUBLIC "-T${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}/Linker_script.ld")

# Informational strings
     SET(OEM "nanoCLR running ${TARGET_BOARD}")
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
     target_compile_definitions(nanoCLR.elf PUBLIC -DTARGETNAMESTRING=\"${TARGET_BOARD}\")
     target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORMNAMESTRING=\"${TARGET_FAMILY}\")
     target_compile_definitions(nanoCLR.elf PUBLIC -DOEMSYSTEMINFOSTRING=\"${OEM}\")

# Setup the type of build to customise elements of the CLR
    if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
        target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(nanoCLR.elf PUBLIC -DBUILD_RTM)
    endif()
    
    target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_NO_CLR_TRACE=1)
    target_compile_definitions(nanoCLR.elf PUBLIC -DDEBUG)
    target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_ARM)
    target_compile_definitions(nanoCLR.elf PUBLIC -D${TARGET_SERIES})

    if(NANOCLR_REFLECTION)
        target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_REFLECTION=1)
    else()
        target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_REFLECTION=0)
    endif()
    if(NF_FEATURE_LIGHT_MATH)
        target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_LIGHT_MATH=1)
    else()
        target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_LIGHT_MATH=0)
    endif()
        if(DP_FLOATINGPOINT)
        target_compile_definitions(nanoCLR.elf PUBLIC -DP_FLOATINGPOINT=1)
    else()
        target_compile_definitions(nanoCLR.elf PUBLIC -DP_FLOATINGPOINT=0)
    endif()
   if(HAS_CONFIG_BLOCK)
        target_compile_definitions(nanoCLR.elf PUBLIC -DHAS_CONFIG_BLOCK=1)
    else()
        target_compile_definitions(nanoCLR.elf PUBLIC -DHAS_CONFIG_BLOCK=0)
    endif()
    if(NANOCLR_SYSTEM_COLLECTIONS)
        target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_SYSTEM_COLLECTIONS=1)
    else()
        target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_SYSTEM_COLLECTIONS=0)
    endif()
    if(REALTIME_CLOCK)
        target_compile_definitions(nanoCLR.elf PUBLIC -DHAL_USE_RTC=1)
    else()
        target_compile_definitions(nanoCLR.elf PUBLIC -DHAL_USE_RTC=0)
    endif()
    if(WATCHDOG)
        target_compile_definitions(nanoCLR.elf PUBLIC -DHAL_USE_WDG=1)
    else()
        target_compile_definitions(nanoCLR.elf PUBLIC -DHAL_USE_WDG=0)
    endif()
    if(RANDOM_NUMBER_GENERATOR)
        target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_HAS_RNG=1)
    else()
        target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_HAS_RNG=0)
    endif()

# Detect when linker script changes so a re-link is executed
    set_target_properties(nanoCLR.elf PROPERTIES LINK_DEPENDS "${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}/Linker_script.ld")

# Common FileX definitions
    if(FILEX_SYSTEM_SD OR FILEX_SYSTEM_NAND OR FILEX_SYSTEM_NOR OR FILEX_SYSTEM_RAM)
        target_compile_definitions(nanoCLR.elf PUBLIC -DFILEX=TRUE)
        if(FILEX_SYSTEM_SD)
              target_compile_definitions(nanoCLR.elf PUBLIC -DFILEX_SYSTEM_SD=TRUE)
        endif()
        if(FILEX_SYSTEM_NAND)
              target_compile_definitions(nanoCLR.elf PUBLIC -DFILEX_SYSTEM_NAND=TRUE)
        endif()
        if(FILEX_SYSTEM_NOR)
              target_compile_definitions(nanoCLR.elf PUBLIC -DFILEX_SYSTEM_NOR=TRUE)
        endif()
        if(FILEX_SYSTEM_RAM)
              target_compile_definitions(nanoCLR.elf PUBLIC -DFILEX_SYSTEM_RAM=TRUE)
        endif()
    endif()

# Common use button defintion
    if(HAS_A_USER_BUTTON)
          target_compile_definitions(nanoCLR.elf PUBLIC -DUSER_BUTTON=TRUE)
    else()
          target_compile_definitions(nanoCLR.elf PUBLIC -DUSER_BUTTON=FALSE)
    endif()

# Common Graphics definitions
    if(GRAPHICS_SUPPORT)
          target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_GRAPHICS=${GRAPHICS_SUPPORT})
    else()
          target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_GRAPHICS=FALSE)
    endif()

# Defines used in the c/c++ code in rare situations to debug and profile
    if(ADVANCED_PROFILING)
           target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_APPDOMAINS=1)
           target_compile_definitions(nanoCLR.elf PUBLIC -DSUPPORT_ANY_BASE_CONVERSION=1)
           target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_NO_IL_INLINE=1)
           target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_NO_CLR_TRACE=1)
           target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_PROFILE_NEW_CALLS=1)
           target_compile_definitions(nanoCLR.elf PUBLIC -DPROFILE_NEW_ALLOCATIONS=1)
           target_compile_definitions(nanoCLR.elf PUBLIC -DTRACE_MEMORY_STATS=1)
    endif()

# Post build commands to create logs and additional files
    add_custom_command(TARGET nanoCLR.elf POST_BUILD
      COMMAND ${CMAKE_OBJCOPY}           $<TARGET_FILE:nanoCLR.elf>   ${CMAKE_BINARY_DIR}/nanoCLR.elf
      COMMAND ${CMAKE_OBJCOPY} -Oihex    $<TARGET_FILE:nanoCLR.elf>   nanoCLR.hex
      COMMAND ${CMAKE_OBJCOPY} -Obinary  $<TARGET_FILE:nanoCLR.elf>   nanoCLR.bin
      COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:nanoCLR.elf> > nanoCLR.lst
    )
