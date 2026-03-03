/*
   Copyright (c) .NET Foundation and Contributors
   See LICENSE file in the project root for full license information.
*/
/*       | The flash page size is 8K bytes    |CLR>>*/  
/*       | Flash word is (4x32) bits (Normal) |     */
/*       | The flash controller implements two|     */
/*       | Programming modes:                 |     */
/*       |  -Single quadword (normal mode)    |     */
/*       |  (4x32) bits                       |     */
/*       |  -Eight quadwords (burst mode)     |     */
/*       |   128 bytes                        |     */
/*       |------------------------------------|     */


/* Entry point */
ENTRY(Reset_Handler)

/* _estack is used in ThreadX to define the end of the stack memory for the main thread */
_estack = ORIGIN(RAM_NS) + LENGTH(RAM_NS);

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
number_flash_banks = 2;
blocks_per_bank = 128;
total_blocks = number_flash_banks * blocks_per_bank;

clr_block_start = 0;
clr_block_end = 79;
blocks_reserved_for_clr = (clr_block_end - clr_block_start) + 1;

deployment_block_start = 80;
deployment_block_end = 251;
blocks_reserved_for_deployment = (deployment_block_end - deployment_block_start) + 1;

config_block_start = 0;
config_block_end = 0;
blocks_reserved_for_config = 0; 

MEMORY
{
  /* ===============*/
  /* PHYSICAL MEMORY*/
  /* ===============*/

    /* ============ */
    /* STM32U5A9NIJ */
    /* ============ */


/* Memory can be secure or non secure and when the secure bit is set */ 
/* different addresses are used to access the memory */ 
/*  -------------------- */
/*  NON SECURE ADDRESSES */ 
/*  -------------------- */
    FLASH_NS (rx)   : ORIGIN = 0x08000000,LENGTH = 4096K
    RAM_NS   (rxw)  : ORIGIN = 0x20000000,LENGTH = 2496K 
        SRAM1_NS   (rxw) : ORIGIN = 0x20000000,LENGTH = 768K   
        SRAM2_NS   (rxw) : ORIGIN = 0x200C0000,LENGTH = 64K 
        SRAM3_NS   (rxw) : ORIGIN = 0x200D0000,LENGTH = 832K 
        SRAM5_NS   (rxw) : ORIGIN = 0x201A0000,LENGTH = 832K 
    SRAM4_NS   (rxw) : ORIGIN = 0x28000000,LENGTH = 16K  /* Low Power in Stop Mode 2  */
    BKPSRAM_NS (rxw) : ORIGIN = 0x40036400,LENGTH = 2K

/*  -------------------- */
/*  SECURE ADDRESSES     */ 
/*  -------------------- */
    FLASH_S   (rx)  : ORIGIN = 0x0C000000,LENGTH = 4096K
    RAM_S     (rxw) : ORIGIN = 0x30000000,LENGTH = 2496K 
        SRAM1_S   (rxw) : ORIGIN = 0x30000000,LENGTH = 768K   
        SRAM2_S   (rxw) : ORIGIN = 0x300C0000,LENGTH = 64K 
        SRAM3_S   (rxw) : ORIGIN = 0x300D0000,LENGTH = 832K 
        SRAM5_S   (rxw) : ORIGIN = 0x301A0000,LENGTH = 832K 
    SRAM4_S   (rxw) : ORIGIN = 0x38000000,LENGTH = 16K  /* Low Power in Stop Mode 2 */
    BKPSRAM_S (rxw) : ORIGIN = 0x50036400,LENGTH = 2K

     /* External RAM on development board (STM32U5A9J_DK) */
     OCTOSPI1_FLASH (xrw) : ORIGIN = 0x09000000, LENGTH = 64M
     HSPI_RAM      (xrw) : ORIGIN = 0xA0000000, LENGTH = 16M

   /* future */  
   OTP (r)         : ORIGIN = 0x0BFA0000,LENGTH = 512

  /* ============================= */
  /* Memory usage by nanoFramework */
  /* ============================= */

     nanoCLR    (rx) : ORIGIN = ORIGIN(FLASH_NS), LENGTH = (blocks_reserved_for_clr*block_size)
     deployment (rx) : ORIGIN = ORIGIN(nanoCLR) + LENGTH(nanoCLR), LENGTH = (blocks_reserved_for_deployment*block_size)
     config     (rx) : ORIGIN = ORIGIN(FLASH_NS), LENGTH = (blocks_reserved_for_config*block_size)

     /* Native code ram (ThreadX, nativeCLR) */
     NATIVE_RAM (rxw) : ORIGIN = ORIGIN(RAM_NS), LENGTH = 100K
     CLR_RAM    (rxw) : ORIGIN = ORIGIN(NATIVE_RAM), LENGTH =  LENGTH(RAM_NS) - LENGTH(NATIVE_RAM) 

     /* RAM accessible by the DMA controller */
     RAM_DMA      (rxw) : ORIGIN = ORIGIN(SRAM4_S), LENGTH = LENGTH(SRAM4_S)

     graphics_vfb (rxw) : ORIGIN = ORIGIN(HSPI_RAM),     LENGTH = 255K
     graphics_ram (rxw) : ORIGIN = ORIGIN(graphics_vfb) ,LENGTH = LENGTH(HSPI_RAM) - LENGTH(graphics_vfb)
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
  } >FLASH_NS

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
  } >FLASH_NS

  /*-----------------------------------------*/
  /* Read only data, constants, strings etc. */
  /*-----------------------------------------*/
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH_NS

  /* --------------------------------------------------------------------------------------*/
  /* ARM special sections                                                                  */
  /* Each function that can throw an exception will have entries in the following sections */
  /* used for C++ and for C programs that try to examine backtraces                        */
  /* --------------------------------------------------------------------------------------*/
  .ARM.extab :                 /* extab names sections containing exception unwinding information */
  {
   *(.ARM.extab* .gnu.linkonce.armextab.*) 
   } >FLASH_NS
  
  .ARM : 
  {
    __exidx_start = .;         /*exidx is used to contain index entries for stack unwinding */
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH_NS


  /* ----------------------------------*/
  /*   Initialization of C/C++ objects
  /* ----------------------------------*/
  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH_NS

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH_NS
  
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
  } >FLASH_NS
  

  /* -----------------------------------------------------------------------*/
  /* Holds initialized data that contributes to the program's memory image  */
  /* -----------------------------------------------------------------------*/

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  .data :   
  {
    . = ALIGN(4);
    _sdata = .;        /* Start address for the .data section */
                       /* Used in LoopCopyDataInit:, "startup_st,32h735igks.s"
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamFunc)        /* .RamFunc sections */
    *(.RamFunc*)       /* .RamFunc* sections */
    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >NATIVE_RAM AT> FLASH_NS
  
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

