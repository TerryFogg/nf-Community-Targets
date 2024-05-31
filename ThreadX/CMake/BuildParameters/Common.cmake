#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

target_link_options(nanoCLR.elf PUBLIC "-T${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}/Linker_script.ld")

# detect when linker changes so a re-link is executed
set_target_properties(nanoCLR.elf PROPERTIES LINK_DEPENDS "${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}/Linker_script.ld")


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

if(HAS_A_USER_BUTTON)
      target_compile_definitions(nanoCLR.elf PUBLIC -DUSER_BUTTON=TRUE)
else()
      target_compile_definitions(nanoCLR.elf PUBLIC -DUSER_BUTTON=FALSE)
endif()


if(GRAPHICS_SUPPORT)
      target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_GRAPHICS=TRUE)
else()
      target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_GRAPHICS=FALSE)
endif()

target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_REFLECTION=TRUE)

target_compile_definitions(nanoCLR.elf PUBLIC -DTARGET_HAS_NANOBOOTER=FALSE)

target_compile_definitions(nanoCLR.elf PUBLIC -D${TARGET_SERIES})
target_compile_definitions(nanoCLR.elf PUBLIC -DDEBUG)
target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_ARM)
target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_LIGHT_MATH=FALSE)
target_compile_definitions(nanoCLR.elf PUBLIC -DDP_FLOATINGPOINT=TRUE)
target_compile_definitions(nanoCLR.elf PUBLIC -DHAS_CONFIG_BLOCK=TRUE)
target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_SYSTEM_COLLECTIONS=TRUE)
target_compile_definitions(nanoCLR.elf PUBLIC -DHAL_USE_RTC=FALSE)
target_compile_definitions(nanoCLR.elf PUBLIC -DHAL_USE_WDG=FALSE)
target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_HAS_RNG=${RANDOM_NUMBER_GENERATOR})
target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORM_NO_CLR_TRACE)

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

target_compile_definitions(nanoCLR.elf PUBLIC -DTARGETNAMESTRING=\"${TARGET_BOARD}\")
target_compile_definitions(nanoCLR.elf PUBLIC -DPLATFORMNAMESTRING=\"${TARGET_FAMILY}\")
SET(OEM "nanoCLR running ${TARGET_BOARD}")
target_compile_definitions(nanoCLR.elf PUBLIC -DOEMSYSTEMINFOSTRING=\"${OEM}\")

# !!!!!!!!!! NOTE
# Set build type here as buildpresets, inherits in CMakePresets.json does not appear to be working
set(CMAKE_BUILD_TYPE "Debug")

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    target_compile_definitions(nanoCLR.elf PUBLIC -DBUILD_RTM)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") 
    target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_ENABLE_SOURCELEVELDEBUGGING)
endif()

add_custom_command(TARGET nanoCLR.elf POST_BUILD
      COMMAND ${CMAKE_OBJCOPY}           $<TARGET_FILE:nanoCLR.elf>   ${CMAKE_BINARY_DIR}/nanoCLR.elf
      COMMAND ${CMAKE_OBJCOPY} -Oihex    $<TARGET_FILE:nanoCLR.elf>   nanoCLR.hex
      COMMAND ${CMAKE_OBJCOPY} -Obinary  $<TARGET_FILE:nanoCLR.elf>   nanoCLR.bin
      COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:nanoCLR.elf> > nanoCLR.lst
)

# --------
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
