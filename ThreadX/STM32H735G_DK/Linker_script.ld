/*
   Copyright (c) .NET Foundation and Contributors
   See LICENSE file in the project root for full license information.
*/

/* NOTE */
/* Division of internal flash is limited by the smallest erase size of 128K */

/* Entry point */
ENTRY(Reset_Handler)

/* _estack is used in ThreadX to define the end of the stack memory for the main thread */
_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);

/* _sidata is used in ThreadX to copy the data segment from ROM to RAM during initialization */
_sidata = LOADADDR(.data);

 /* Stack and heap size */
_Min_Heap_Size = 0x200; 
_Min_Stack_Size = 0x400; 

/* Define the availability of FLASH */
/* Minimum flash erase size is define as a sector for STM32H7 series*/
flash_block_1 = 0x08000000;
flash_block_1_size = 1024K;
sector_size = 128k; 

block_size = sector_size;
number_flash_banks = 1;
blocks_per_bank = 8;
total_blocks = number_flash_banks * blocks_per_bank;

clr_block_start = 0;
clr_block_end = 4;
blocks_reserved_for_clr = (clr_block_end - clr_block_start) + 1;

deployment_block_start = 5;
deployment_block_end = 7;
blocks_reserved_for_deployment = (deployment_block_end - deployment_block_start) + 1;

config_block_start = 0;
config_block_end = 0;
blocks_reserved_for_config = 0;

MEMORY
{
  /* ===============*/
  /* PHYSICAL MEMORY*/
  /* ===============*/

    /* ============== */
    /* STM32H735IGK6U */
    /* ============== */

     ITCM_SRAM  (rxw):ORIGIN = 0x00000000,LENGTH = 64K 
     FLASH      (rx) :ORIGIN = 0x08000000,LENGTH = 1024K
     OTP        (rx) :ORIGIN = 0x08FFF000,LENGTH = 1024
     DTCM_SRAM  (rxw):ORIGIN = 0x20000000,LENGTH = 128K 
     RAM_D1     (rxw):ORIGIN = 0x24000000,LENGTH = 320K
     RAM_D2     (rxw):ORIGIN = 0x30000000,LENGTH = 32K
     RAM_D4     (rxw):ORIGIN = 0x38000000,LENGTH = 16K
     BACKUP_RAM (rxw):ORIGIN = 0x38800000,LENGTH = 4k

     /* External RAM on development board (STM32H735G-DK) */
     OCTOSPI2_RAM   (rxw):ORIGIN = 0x70000000,LENGTH = 16M
     OCTOSPI1_FLASH (rx ):ORIGIN = 0x09000000,LENGTH = 64M

  /* ============================= */
  /* Memory usage by nanoFramework */
  /* ============================= */

     nanoCLR    (rx) : ORIGIN = ORIGIN(FLASH), LENGTH = (blocks_reserved_for_clr*block_size)
     deployment (rx) : ORIGIN = ORIGIN(nanoCLR) + LENGTH(nanoCLR), LENGTH = (blocks_reserved_for_deployment*block_size)
     config     (rx) : ORIGIN = ORIGIN(OCTOSPI1_FLASH), LENGTH = (blocks_reserved_for_config*block_size)

     /* Native code ram (Threadx, nativeCLR) */
     NATIVE_RAM   (rxw) : ORIGIN = ORIGIN(RAM_D1), LENGTH = LENGTH(RAM_D1)

     /* RAM accessible by the DMA controller */
     RAM_DMA      (rxw) : ORIGIN = ORIGIN(RAM_D2), LENGTH = LENGTH(RAM_D2)

     graphics_vfb (rxw) : ORIGIN = ORIGIN(OCTOSPI2_RAM), LENGTH = 255K
     graphics_ram (rxw) : ORIGIN = ORIGIN(graphics_vfb) + LENGTH(graphics_vfb), LENGTH = 4M
     CLR_RAM  (rxw) : ORIGIN = ORIGIN(graphics_ram)+ LENGTH(graphics_ram), LENGTH =  LENGTH(OCTOSPI2_RAM) - LENGTH(graphics_ram) - LENGTH(graphics_vfb)
}

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
Graphics_frame_buffer = ORIGIN(graphics_vfb); 
  
/*  Bitmap objects */
Graphics_managed_heap = ORIGIN(graphics_ram); 
Graphics_managed_end  = ORIGIN(graphics_ram) + LENGTH(graphics_ram);   

/* Define output sections */
SECTIONS
{
  /*----------------------------------------*/
  /* The startup code goes first into FLASH */
  /*----------------------------------------*/
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /*-----------------------------------------------------*/
  /* The program code and other data goes into FLASH     */
  /* executable instructions, of a program               */
  /* glue arm to thumb code                              */
  /* glue thumb to arm code                              */
  /* the system arranges to execute the code in this     */
  /* section before calling the main program entry point */
  /*-----------------------------------------------------*/
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbol at end of code */
  } >FLASH

  /*-----------------------------------------*/
  /* Read only data, constants, strings etc. */
  /*-----------------------------------------*/
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  /* --------------------------------------------------------------------------------------*/
  /* ARM special sections                                                                  */
  /* Each function that can throw an exception will have entries in the following sections */
  /* used for C++ and for C programs that try to examine backtraces                        */
  /* --------------------------------------------------------------------------------------*/
  .ARM.extab :                 /* extab names sections containing exception unwinding information */
  {
   *(.ARM.extab* .gnu.linkonce.armextab.*) 
   } >FLASH
  
  .ARM : 
  {
    __exidx_start = .;         /* exidx is used to contain index entries for stack unwinding */
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH


  /* ----------------------------------*/
  /*   Initialization of C/C++ objects
  /* ----------------------------------*/
  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  
  /* ----------------------------------------------------------------------*/
  /* Tear down of C/C++ objects                                            */
  /* C++ runtime destructors for static variables, C designated finalizers */
  /* ----------------------------------------------------------------------*/
  .fini_array : /*  */
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH
  

  /* -----------------------------------------------------------------------*/
  /* Holds initialized data that contributes to the program's memory image  */
  /* -----------------------------------------------------------------------*/
  .data :   
  {
    . = ALIGN(4);
    _sdata = .;        /* Start address for the .data section */
                       /* Used in LoopCopyDataInit:, "startup_st,32h735igks.s"
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >NATIVE_RAM AT> FLASH
  
  /* -------------------------------------------------------------------*/
  /* This section holds uninitialized data                             */
  /* By definition, and through the code in startup_st32h735igks.s, the */
  /* system initializes the data with zeros at startup                  */
  /* -------------------------------------------------------------------*/
  . = ALIGN(4);
  .bss :       
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >NATIVE_RAM

/* -----------------------------------------------------------------*/
/* Not using "DYNAMIC_MEMORY" so no need for a ._threadx_heap here.
/* -----------------------------------------------------------------*/

/* ------------------------------------------------------------------*/
/* Application defined for use as memory accessible by DMA1 and DMA2 */
/* ------------------------------------------------------------------*/
  .dma_buffer :
{
  *(.dma_buffer)
} >RAM_DMA

/* ---------------------------------------------------------------------*/
/* User_heap_stack section, used to check that there is enough RAM left */
/* ---------------------------------------------------------------------*/
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
        __RAM_segment_used_end__ = .;
  } >NATIVE_RAM


  /* -----------------------------------------------*/
  /* Remove information from the standard libraries */
  /* -----------------------------------------------*/
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }
  .ARM.attributes 0 : { *(.ARM.attributes) }
}
