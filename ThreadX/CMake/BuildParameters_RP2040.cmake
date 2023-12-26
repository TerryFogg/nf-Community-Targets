#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

    
# The following pico_sdk_init.cmake is included in the top level CMakeLists.txt.
# If it is not included before the project declaration, an error occurs in adding the ELF2UF2.
#include(${PICO_SDK_SOURCE}/pico_sdk_init.cmake)
#pico_sdk_init()

 target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:  -mthumb -mcpu=cortex-m0plus>)
 target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>: -mthumb -mcpu=cortex-m0plus>)
 target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:ASM>: -mcpu=cortex-m0plus -x assembler-with-cpp>)


target_compile_definitions(nanoCLR.elf PUBLIC 
                           DEBUG                           
                           DPLATFORM_ARM
                           __CM0_CMSIS_VERSION
                           LIB_PICO_BIT_OPS=1
                           LIB_PICO_BIT_OPS_PICO=1
                           LIB_PICO_DIVIDER=1
                           LIB_PICO_DIVIDER_HARDWARE=1
                           #
                           #  Apparently float/double in RAM is faster
                           #
                           PICO_FLOAT_IN_RAM=1
                           PICO_DOUBLE_IN_RAM=1

                           LIB_PICO_DOUBLE=1
                           LIB_PICO_DOUBLE_PICO=1
                           
                           LIB_PICO_FLOAT=1
                           LIB_PICO_FLOAT_PICO=1
                           
                           LIB_PICO_INT64_OPS=1
                           LIB_PICO_INT64_OPS_PICO=1

                           LIB_PICO_MALLOC=1
                           LIB_PICO_MEM_OPS=1
                           LIB_PICO_MEM_OPS_PICO=1

                           LIB_PICO_PLATFORM=1
                           LIB_PICO_PRINTF=1
                           LIB_PICO_PRINTF_PICO=1
                           LIB_PICO_RUNTIME=1
                           LIB_PICO_STANDARD_LINK=1

                           LIB_PICO_STDIO=1
                           LIB_PICO_STDIO_UART=1

                           LIB_PICO_STDLIB=1

                           LIB_PICO_SYNC=1
                           LIB_PICO_SYNC_CORE=1
                           LIB_PICO_SYNC_CRITICAL_SECTION=1
                           LIB_PICO_SYNC_MUTEX=1
                           LIB_PICO_SYNC_SEM=1

                           LIB_PICO_TIME=1
                           LIB_PICO_UTIL=1
                           PICO_BOARD=\"pico\"
                           PICO_BUILD=1
                           PICO_COPY_TO_RAM=0
                           PICO_CXX_ENABLE_EXCEPTIONS=0
                           PICO_NO_FLASH=0
                           PICO_NO_HARDWARE=0
                           PICO_ON_DEVICE=1
                           PICO_USE_BLOCKED_RAM=0
                           PendSV_Handler=isr_pendsv
                           SysTick_Handler=isr_systick
                           CFG_TUSB_MCU=OPT_MCU_RP2040
 )


    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__ctzsi2")               # gcc
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__ctzdi2")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__popcountdi2")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__popcountsi2")

    # armclang
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__clz")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__clzl")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__clzsi2")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__clzll")

    # Double
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_cdcmpeq")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_cdcmple")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_cdrcmple")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_dadd")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_dcmpeq")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_dcmpge")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_dcmpgt")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_dcmple")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_dcmplt")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_dcmpun")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_ddiv")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_dmul")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_drsub")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_dsub")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_i2d")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_l2d")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_ui2d")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_ul2f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=acos")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=acosh")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=asin")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=asinh")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=atan")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=atan2")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=atanh")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=cbrt")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=ceil")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=copysign")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=cos")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=cosh")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=drem")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=exp")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=exp10")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=exp2")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=floor")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=fmod")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=hypot")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=ldexpf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=log")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=log10")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=log2")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=pow")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=powint")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=remainder")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=remquo")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=round")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=sin")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=sincos") # gnu
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=sqrt")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=tan")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=tanh")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=trunc")

    # Divider
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_idiv")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_idivmod")
     target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_ldivmod")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_uidiv")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_uidivmod")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=expm1")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=log1p")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=fma")

    #float
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_cfcmpeq")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_cfcmple")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_cfrcmple")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_d2f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_d2iz")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_d2lz")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_d2uiz")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_d2ulz")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_f2d")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_f2iz")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_f2lz")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_f2uiz")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_f2ulz")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fadd")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fcmpeq")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fcmpge")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fcmpgt")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fcmple")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fcmplt")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fcmpun")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fdiv")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fmul")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_frsub")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_fsub")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_i2f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_ui2f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_ul2d")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_uldivmod")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=acosf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=acoshf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=asinf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=asinhf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=atan2f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=atanf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=atanhf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=cbrtf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=ceilf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=copysignf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=cosf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=coshf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=dremf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=exp10f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=exp2f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=expf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=expm1f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=floorf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=fmaf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=fmodf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=hypotf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=ldexp")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=log10f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=log1pf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=log2f")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=logf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=powf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=powintf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=remainderf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=remquof")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=roundf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=sincosf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=sinf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=sinhf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=sqrtf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=tanf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=tanhf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=truncf")

    #int 64
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_lmul")

    #malloc
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=calloc")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=free")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=malloc")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=realloc")

    #memory operations
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_memcpy")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_memcpy4")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_memcpy8")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_memset")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_memset4")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=__aeabi_memset8")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=memcpy")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=memset")

    #printf
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=snprintf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=vsnprintf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=sprintf")


    #stdio
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=getchar")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=printf")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=putchar")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=puts")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--wrap=vprintf")


    target_link_options(nanoCLR.elf PUBLIC "LINKER:-gc-sections")
    target_link_options(nanoCLR.elf PUBLIC "LINKER:--no-warn-rwx-segments")

    target_link_options(nanoCLR.elf PUBLIC "-T${CMAKE_SOURCE_DIR}/targets/AzureRTOS/RaspberryPi/RP2040/nanoCLR/Memory/RaspberryPiPico.ld")


# Adds .hex, .bin, .dis, .elf outputs
# If  PICO_NO_UF2 not set, builds ELF2UF2
#pico_add_extra_outputs(nanoCLR.elf)



