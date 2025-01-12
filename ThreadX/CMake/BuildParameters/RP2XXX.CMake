#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
# Raspberry Pi Pico uses the Pico-Sdk for build parameters

# nanoCLR requirement
 target_compile_definitions(nanoCLR PUBLIC -DPLATFORM_ARM)

 # Cortex-m33 can run non secure or secure, at boot the RP2350 is in secure mode.
 if( "${TARGET_SERIES}" STREQUAL "RP2350")
       target_compile_definitions(nanoCLR PUBLIC -DTX_SINGLE_MODE_SECURE -DPICO_PLATFORM=rp2350)

        # The RP2350 can have up 2MB internal RP2350B, or up to 16MB flash external
        target_compile_definitions(nanoCLR PUBLIC -DFLASH_SIZE=${FLASH_SIZE}) 
        target_compile_definitions(nanoCLR PUBLIC -DFLASH_CHIP=${FLASH_CHIP}) 

        # The RP2350 can have up tp 16MB external PSRAM (It could be additional FLASH, but not supported here)
        if(${PSRAM})
            target_compile_definitions(nanoCLR PUBLIC -DPSRAM) 
            target_compile_definitions(nanoCLR PUBLIC -DPSRAM_SIZE=${PSRAM_SIZE}) 
            target_compile_definitions(nanoCLR PUBLIC -DPSRAM_CHIP =${PSRAM_CHIP}) 
        endif()
else()
        target_compile_definitions(nanoCLR PUBLIC -DPICO_PLATFORM=rp2040)
 endif()

# Redirect the pico-sdk handlers to the ThreadX handlers
target_compile_definitions(nanoCLR PUBLIC -DPendSV_Handler=isr_pendsv) 
target_compile_definitions(nanoCLR PUBLIC -DSysTick_Handler=isr_systick) 
target_compile_definitions(nanoCLR PUBLIC -DSVC_Handler=isr_svcall) 
target_compile_definitions(nanoCLR PUBLIC -DHardFault_Handler=isr_hardfault) 
target_compile_definitions(nanoCLR PUBLIC -D__tx_NMIHandler=isr_nmi) 
target_compile_definitions(nanoCLR PUBLIC -D__tx_BadHandler=isr_invalid) 

# Use RP2040 version of the tinyusb file for RP2350 as well for now.
# The code works on both the same RP2040 and RP2350, but the code modules have the original RP2040 names
target_compile_definitions(nanoCLR PUBLIC -DCFG_TUSB_MCU=OPT_MCU_RP2040 -DDEBUG) 
if(CFG_USB_USE_DTR)
 target_compile_definitions(nanoCLR PUBLIC -DCFG_USB_USE_DTR) 
endif() 


#  Some options
target_link_options(nanoCLR PUBLIC "-Wl,--print-memory-usage")

# Add hex,bin,map and uf2 file outputs
pico_add_extra_outputs(nanoCLR)

# Add a listing file to the standard Pico SDK build
add_custom_command(TARGET nanoCLR POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:nanoCLR> > nanoCLR.lst
)
