/*
   Copyright (c) .NET Foundation and Contributors
   See LICENSE file in the project root for full license information.
*/

/* NOTE */
/* Division of internal flash is limited by the smallest erase size of 8K */

/* Entry point */
ENTRY(Reset_Handler)

/* _estack is used in ThreadX to define the end of the stack memory for the main thread */
_estack = ORIGIN(DTCM_SRAM) + LENGTH(DTCM_SRAM);

/* _sidata is used in ThreadX to copy the data segment from ROM to RAM during initialization */
_sidata = LOADADDR(.data);

/* Stack and heap size */
_Min_Heap_Size  = 0x200; 
_Min_Stack_Size = 0x400; 

/* Define the availability of FLASH                                                                  */
/* The flash memory is organized as 137-bit flash words memory that can be used for                  */
/* storing both code and data constants.                                                             */
/* Each word consists of:                                                                            */
/*    One flash word (4 words, 16 bytes or 128 bits)                                                 */
/*    9 ECC bits                                                                                     */
/* The flash memory is organized as follows:                                                         */
/*  1.  Two independent 1 Mbyte banks containing 128 user sectors of 8 Kbytes each.                  */
/*  2.  128 Kbytes of System flash memory from which the device can boot.                            */
/*  3.  1 Kbyte of OTP (one-time programmable) memory containing option bytes for user configuration */

/* Minimum flash erase size is defined as a sector (8K) for STM32H7B series */

flash_block_1 = 0x08000000;
sector_size = 8K;
block_size = sector_size;
number_of_banks = 2;
total_blocks = 128*number_of_banks;
clr_block_start = 0;
clr_block_end = 79;
clr_total_blocks = clr_block_end - clr_block_start + 1;

deployment_block_start = 80;
deployment_block_end = 253;
deployment_total_blocks = deployment_block_end - deployment_block_start + 1;

config_block_start = 254;
config_block_end = 255;
config_total_blocks = config_block_end - config_block_start + 1;

MEMORY
{
  /* ===============*/
  /* PHYSICAL MEMORY*/
  /* ===============*/

    /* ===============*/
    /* STM32H7B3LIH6QU*/
    /* ===============*/

     FLASH      (rx) :ORIGIN = 0x08000000,LENGTH = 2048k
     OTP        (rx) :ORIGIN = 0x08FFF000,LENGTH = 1024

     ITCM_SRAM  (rxw):ORIGIN = 0x00000000,LENGTH = 64K 
     DTCM_SRAM  (rxw):ORIGIN = 0x20000000,LENGTH = 128K 
     AXI_RAM1   (rxw):ORIGIN = 0x24000000,LENGTH = 256K
     AXI_RAM2   (rxw):ORIGIN = 0x24040000,LENGTH = 384K
     AXI_RAM3   (rxw):ORIGIN = 0x240A0000,LENGTH = 384K
     AHB_RAM    (rxw):ORIGIN = 0x30000000,LENGTH = 128K   /* (64K + 64K) */
     SRD_RAM    (rxw):ORIGIN = 0x38000000,LENGTH = 32K
     BACKUP_RAM (rxw):ORIGIN = 0x38800000,LENGTH = 4k

     /* External RAM on development board (STM32H7B3I_DK) */
     OCTOSPI1_FLASH (xrw) : ORIGIN = 0x09000000, LENGTH = 64M
     FMC_SDRAM      (xrw) : ORIGIN = 0xD0000000, LENGTH = 16M

  /* ============================= */
  /* Memory usage by nanoFramework */
  /* ============================= */

     nanoCLR    (rx) : ORIGIN = ORIGIN(FLASH), LENGTH = clr_total_blocks*sector_size
     deployment (rx) : ORIGIN = ORIGIN(nanoCLR) + LENGTH(nanoCLR), LENGTH = deployment_total_blocks*sector_size
     config     (rx) : ORIGIN = ORIGIN(deployment) + LENGTH(deployment), LENGTH = config_total_blocks*sector_size

     /* Native code ram (ThreadX, nativeCLR) */
     NATIVE_RAM   (rxw) : ORIGIN = ORIGIN(AXI_RAM1), LENGTH = LENGTH(AXI_RAM1)
     CLR_RAM  (rxw) : ORIGIN = ORIGIN(AXI_RAM2), LENGTH =  LENGTH(AXI_RAM2) + LENGTH(AXI_RAM3) 

     /* RAM accessible by the DMA controller */
     RAM_DMA      (rxw) : ORIGIN = ORIGIN(AHB_RAM), LENGTH = LENGTH(AHB_RAM)

     graphics_vfb (rxw) : ORIGIN = ORIGIN(FMC_SDRAM), LENGTH = 255K
     graphics_ram (rxw) : ORIGIN = ORIGIN(graphics_vfb) + LENGTH(graphics_vfb), LENGTH = 4M

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
