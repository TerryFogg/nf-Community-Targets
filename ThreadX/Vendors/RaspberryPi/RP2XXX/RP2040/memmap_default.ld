/*                                                                              */
/* Copyright (c) .NET Foundation and Contributors                               */
/* See LICENSE file in the project root for full license information.           */


/* Division of internal flash is limited by the smallest erase size of 4K       */
/* The first sector contains a prepended bs2_default_padded_checksummed.S boot  */

/* Minimum flash erase size is define as a page for RP2040 series*/
flash_block_1 = 0x10000000;
flash_block_1_size = 2048K;
flash_page_size = 4096; 

block_size = flash_page_size;
total_blocks = 512;

stage2_boot_block_start = 0;
stage2_boot_block_end = 0;
blocks_reserved_for_stage2_boot = 1;

clr_block_start = 1;
clr_block_end = 159;
blocks_reserved_for_clr = (clr_block_end - clr_block_start) + 1;

deployment_block_start = 161;
deployment_block_end = 511;
blocks_reserved_for_deployment = (deployment_block_end - deployment_block_start) + 1;

config_block_start = 160;
config_block_end = 160;
blocks_reserved_for_config = 1;

flash_sector_size  = 4096 ; 
flash_page_size    = 256;
Total_flash_page_count   = 512;
config_size =  1;
graphics_Frame_Buffer_not_used = 0; 

/* Define the availability of FLASH */
sector_size = 128k;
number_sectors_total = 8;
total_flash_size = number_sectors_total * sector_size;
ram_total = 192K + 128K;
native_ram_size = 20k;

/* PHYSICAL MEMORY RP2040 (RaspberryPi Pico) */

MEMORY
{
     FACTORY_SYSTEM_MEMORY (rx) :ORIGIN = 0x00000000, LENGTH = 16K
     FLASH                 (rx) :ORIGIN = 0x10000000,LENGTH = 2048K
     RAM_STRIPED           (rwx):ORIGIN = 0x20000000,LENGTH = 256K  

 /*  RAM_NONSTRIPED_0  (rxw):ORIGIN = 0x21000000,LENGTH = 64K   */
 /*  RAM_NONSTRIPED_1  (rxw):ORIGIN = 0x21010000,LENGTH = 64K   */ 
 /*  RAM_NONSTRIPED_2  (rxw):ORIGIN = 0x21020000,LENGTH = 64K   */
 /*  RAM_NONSTRIPED_3  (rxw):ORIGIN = 0x21030000,LENGTH = 64K   */

     SCRATCH_X         (rxw):ORIGIN = 0x20040000,LENGTH = 4K  
     SCRATCH_Y         (rxw):ORIGIN = 0x20041000,LENGTH = 4K  

     XIP_CACHING       (rxw):ORIGIN = 0x15000000,LENGTH = 16K
     DPRAM             (rxw):ORIGIN = 0x50100000,LENGTH = 4K

  /* ============================= */
  /* Memory usage by nanoFramework */
  /* ============================= */
     
     /* Flash */
     STAGE2_BOOT  (rx):ORIGIN = ORIGIN(FLASH), LENGTH = 252 + 4
     nanoCLR      (rx):ORIGIN = ORIGIN(STAGE2_BOOT) + LENGTH(STAGE2_BOOT), LENGTH = (blocks_reserved_for_clr*block_size)
     config       (rx):ORIGIN = ORIGIN(nanoCLR)     + LENGTH(nanoCLR)    , LENGTH = (blocks_reserved_for_config*block_size)
     deployment   (rx):ORIGIN = ORIGIN(config)      + LENGTH(config)     , LENGTH = (blocks_reserved_for_deployment*block_size)

     /* Ram */
     NATIVE_RAM   (rxw):ORIGIN = ORIGIN(RAM_STRIPED), LENGTH = 75K
     CLR_RAM      (rxw):ORIGIN = ORIGIN(NATIVE_RAM) + LENGTH(NATIVE_RAM), LENGTH = 75K
     graphics_ram (rxw):ORIGIN = ORIGIN(CLR_RAM) + LENGTH(CLR_RAM), LENGTH = LENGTH(RAM_STRIPED) - LENGTH(NATIVE_RAM) - LENGTH(CLR_RAM)
     
     /* NO FRAME BUFFER - use SPI local buffer to transfer graphics */
}
  
/* =============== */
/* VARIABLES USED  */
/* =============== */

/* Stack and heap size */
_Min_Heap_Size = 0x200; 
_Min_Stack_Size = 0x400; 

/* The CLR managed heap for CLR */
HeapBegin             = ORIGIN(CLR_RAM); 
HeapEnd               = ORIGIN(CLR_RAM) + LENGTH(CLR_RAM); 

/* The CLR native Image */
__nanoImage_start__   = ORIGIN(nanoCLR); 
__nanoImage_end__     = ORIGIN(nanoCLR) + LENGTH(nanoCLR); 

/*  C# Portable Executable (PE) file is stored */
__deployment_start__  = ORIGIN(deployment); 
__deployment_end__    = ORIGIN(deployment) + LENGTH(deployment); 

/* Small area of persistent storage */
__nanoConfig_start__  = ORIGIN(config);
__nanoConfig_end__    = ORIGIN(config) + LENGTH(config);

/* Bit pixel copy of the screen */
Graphics_frame_buffer = 0;

/*  Bitmap objects */
Graphics_managed_heap = ORIGIN(graphics_ram); 
Graphics_managed_end  = ORIGIN(graphics_ram) + LENGTH(graphics_ram);   


/* Entry point */
ENTRY(_entry_point)

/* Define output sections */
SECTIONS
{
  /* --------------------------------------------------------------------------*/
  /* The startup code goes first into FLASH----------------------------------  */
  /* --------------------------------------------------------------------------*/
  /* Second stage bootloader is prepended to the image. It must be 256 bytes   */
  /* and have a checksum. It is usually built by the boot_stage2 target        */
  /* in the Raspberry Pi Pico SDK                                              */

    .flash_begin : {
        __flash_binary_start = .;
    } > FLASH

    .boot2 : {
        __boot2_start__ = .;
        KEEP (*(.boot2))
        __boot2_end__ = .;
    } > FLASH

     ASSERT(__boot2_end__ - __boot2_start__ == 256, "ERROR: Pico second stage bootloader must be 256 bytes in size")

    /* The second stage will always enter the image at the start of .text.   */
    /* The debugger will use the ELF entry point, which is the _entry_point  */
    /* symbol if present, otherwise defaults to start of .text               */
    /* This can be used to transfer control back to the boot rom on debugger */
    /* launches only, to perform proper flash setup                          */
    /*                                                                       */

    .text : {
        __logical_binary_start = .;
        KEEP (*(.vectors))
        KEEP (*(.binary_info_header))
        __binary_info_header_end = .;
        KEEP (*(.reset))
        /* TODO revisit this now memset/memcpy/float in ROM */
        /* bit of a hack right now to exclude all floating point and time critical (e.g. memset, memcpy) code from */
        /* FLASH ... we will include any thing excluded here in .data below by default */
        *(.init)
        *(EXCLUDE_FILE(*libgcc.a: *libc.a:*lib_a-mem*.o *libm.a:) .text*)
        *(.fini)
        /* Pull all c'tors into .text */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        /* Followed by destructors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.eh_frame*)
        . = ALIGN(4);
    } > FLASH

  /*-----------------------------------------*/
  /* Read only data, constants, strings etc. */
  /*-----------------------------------------*/
    .rodata : {
        *(EXCLUDE_FILE(*libgcc.a: *libc.a:*lib_a-mem*.o *libm.a:) .rodata*)
        . = ALIGN(4);
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.flashdata*)))
        . = ALIGN(4);
    } > FLASH

  /* --------------------------------------------------------------------------------------*/
  /* ARM special sections                                                                  */
  /* Each function that can throw an exception will have entries in the following sections */
  /* used for C++ and for C programs that try to examine backtraces                        */
  /* --------------------------------------------------------------------------------------*/

  /* extab names sections containing exception unwinding information */
 
 .ARM.extab :                                   
  {
   *(.ARM.extab* .gnu.linkonce.armextab.*) 
   } > FLASH
  
/* exidx is used to contain index entries for stack unwinding */

__exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;


  /* --------------------------------------- */
  /* Machine inspectable binary information  */
  /* --------------------------------------- */
    . = ALIGN(4);
    __binary_info_start = .;
    .binary_info :
    {
        KEEP(*(.binary_info.keep.*))
        *(.binary_info.*)
    } > FLASH
    __binary_info_end = .;
    . = ALIGN(4);

    /* End of .text-like segments */
    __etext = .;

   .ram_vector_table (COPY): {
        *(.ram_vector_table)
    } > NATIVE_RAM

  /* -----------------------------------------------------------------------*/
  /* Holds initialized data that contributes to the program's memory image  */
  /* -----------------------------------------------------------------------*/
    .data : {
        __data_start__ = .;
        *(vtable)

        *(.time_critical*)

        /* remaining .text and .rodata; i.e. stuff we exclude above because we want it in RAM */
        *(.text*)
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);

        *(.data*)

        . = ALIGN(4);
        *(.after_data.*)
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__mutex_array_start = .);
        KEEP(*(SORT(.mutex_array.*)))
        KEEP(*(.mutex_array))
        PROVIDE_HIDDEN (__mutex_array_end = .);

  /* ----------------------------------*/
  /*   Initialization of C/C++ objects */
  /* ----------------------------------*/

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(SORT(.preinit_array.*)))
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

  /* ----------------------------------------------------------------------*/
  /* Tear down of C/C++ objects                                            */
  /* C++ runtime destructors for static variables, C designated finalizers */
  /* ----------------------------------------------------------------------*/
        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        *(SORT(.fini_array.*))
        *(.fini_array)
        PROVIDE_HIDDEN (__fini_array_end = .);

        *(.jcr)
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
    } > NATIVE_RAM AT> FLASH

     .uninitialized_data (COPY): {
        . = ALIGN(4);
        *(.uninitialized_data*)
    } > NATIVE_RAM

    /* Start and end symbols must be word-aligned */
    .scratch_x : {
        __scratch_x_start__ = .;
        *(.scratch_x.*)
        . = ALIGN(4);
        __scratch_x_end__ = .;
    } > SCRATCH_X AT > FLASH
    __scratch_x_source__ = LOADADDR(.scratch_x);

    .scratch_y : {
        __scratch_y_start__ = .;
        *(.scratch_y.*)
        . = ALIGN(4);
        __scratch_y_end__ = .;
    } > SCRATCH_Y AT > FLASH
    __scratch_y_source__ = LOADADDR(.scratch_y);
 
  /* -------------------------------------------------------------------*/
  /* This section holds uninitialized data                              */
  /* -------------------------------------------------------------------*/
  .bss :       
  {
    /* This is used by the startup in order to initialize the .bss section */
        . = ALIGN(4);
        __bss_start__ = .;
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.bss*)))
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > NATIVE_RAM

    .heap (COPY):
    {
        __end__ = .;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    } > NATIVE_RAM

/* -----------------------------------------------------------------*/
/* Not using "DYNAMIC_MEMORY" so no need for a ._threadx_heap here. */
/* -----------------------------------------------------------------*/

    /* .stack*_dummy section doesn't contains any symbols. It is only           */
    /* used for linker to calculate size of stack sections, and assign          */
    /* values to stack symbols later                                            */ 
    /*                                                                          */
    /* stack1 section may be empty/missing if platform_launch_core1 is not used */

    /* By default we put                                 */
    /* core 0 stack at the end of scratch Y              */
    /* so that if core 1 stack is not used then all      */
    /* of SCRATCH_X is free.                             */

    .stack1_dummy (COPY):
    {
        *(.stack1*)
    } > SCRATCH_X
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > SCRATCH_Y

    .flash_end : {
        __flash_binary_end = .;
    } > FLASH

    /* stack limit is poorly named, but historically is maximum heap ptr */
    __StackLimit = ORIGIN(NATIVE_RAM) + LENGTH(NATIVE_RAM);
    __StackOneTop = ORIGIN(SCRATCH_X) + LENGTH(SCRATCH_X);
    __StackTop = ORIGIN(SCRATCH_Y) + LENGTH(SCRATCH_Y);
    __StackOneBottom = __StackOneTop - SIZEOF(.stack1_dummy);
    __StackBottom = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds NATIVE_RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region NATIVE_RAM overflowed")

    ASSERT( __binary_info_header_end - __logical_binary_start <= 256, "Binary info must be in first 256 bytes of the binary")
}
