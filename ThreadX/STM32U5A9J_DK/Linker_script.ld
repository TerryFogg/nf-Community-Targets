/*
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
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


ENTRY(Reset_Handler)                           /* Entry Point */
_sidata = LOADADDR(.data);                     /* Copy from flash to RAM at startup */
_estack = ORIGIN(RAM_NS) + LENGTH(RAM_NS);     /* Stack pointer, top of RAM)


_Min_Heap_Size    = 0x200; 
_Min_Stack_Size   = 0x400; 
flash_sector_size = 0x2000;      /* The flash page size is 8K bytes 8?
total_flash_size  =  512*flash_sector_size;
clr_reserved_size =  40*flash_sector_size;
config_size       =  3*flash_sector_size;

MEMORY    /*   Usage by nanoFramework */
{

/* Memory can be secure or non secure and when the secure bit is set */ 
/* different addresses are used to access the memory */ 

/*  -------------------- */
/*  NON SECURE ADDRESSES */ 
/*  -------------------- */
    FLASH_NS (rx)   : ORIGIN = 0x08000000,LENGTH = 4096K
    RAM_NS   (xrw)  : ORIGIN = 0x20000000,LENGTH = 2496K 
        SRAM1_NS   (xrw) : ORIGIN = 0x20000000,LENGTH = 768K   
        SRAM2_NS   (xrw) : ORIGIN = 0x200C0000,LENGTH = 64K 
        SRAM3_NS   (xrw) : ORIGIN = 0x200D0000,LENGTH = 832K 
        SRAM4_NS   (xrw) : ORIGIN = 0x28000000,LENGTH = 16K  /* Low Power in Stop Mode 2  */
    SRAM5_NS   (xrw) : ORIGIN = 0x201A0000,LENGTH = 832K 
    BKPSRAM_NS (xrw) : ORIGIN = 0x40036400,LENGTH = 2K

/*  -------------------- */
/*  SECURE ADDRESSES     */ 
/*  -------------------- */
    FLASH_S   (rx)  : ORIGIN = 0x0C000000,LENGTH = 4096K
    RAM_S     (xrw) : ORIGIN = 0x30000000,LENGTH = 2496K 
        SRAM1_S   (xrw) : ORIGIN = 0x30000000,LENGTH = 768K   
        SRAM2_S   (xrw) : ORIGIN = 0x300C0000,LENGTH = 64K 
        SRAM3_S   (xrw) : ORIGIN = 0x300D0000,LENGTH = 832K 
        SRAM4_S   (xrw) : ORIGIN = 0x38000000,LENGTH = 16K  /* Low Power in Stop Mode 2 */
        SRAM5_S   (xrw) : ORIGIN = 0x301A0000,LENGTH = 832K 
    BKPSRAM_S (xrw) : ORIGIN = 0x50036400,LENGTH = 2K

/* future */  
   OTP (r)         : ORIGIN = 0x0BFA0000,LENGTH = 512

}



/*       | nanoFramework memory usage         |     */   
/*       |------------------------------------|     */

/*       | FLASH USAGE - 512x8K = 4096K bytes |     */
/*       | 8K minimum erase page size         |     */   
/*       |------------------------------------|     */
/*       | FLASH USAGE - 512x8K = 4096K bytes |CLR>>*/  nanoCLR    (rx) : ORIGIN = ORIGIN(FLASH_NS), LENGTH = clr_reserved_size
/*       | 8K minimum erase page size         |CLR>>*/  deployment (rx) : ORIGIN = ORIGIN(nanoCLR) + LENGTH(nanoCLR), LENGTH = total_flash_size - clr_reserved_size - config_size
/*       |                                    |CLR>>*/  config     (rx) : ORIGIN = ORIGIN(nanoCLR) + LENGTH(nanoCLR)+ LENGTH(deployment), LENGTH = config_size
/*       |------------------------------------|     */
/*       | RAM USAGE  - 2496K                 |     */
/*       |------------------------------------|     */
/*       | native C code RAM                  |CLR>>*/  Native_RAM      (xrw) : ORIGIN = ORIGIN(RAM_NS), LENGTH = 100k
/*       | nanoCLR managed heap               |CLR>>*/  CLR_managed_RAM (xrw) : ORIGIN = ORIGIN(Native_RAM)+ LENGTH(Native_RAM), LENGTH = 1M
/*       | Video frame buffer                 |CLR>>*/  graphics_vfb    (xrw) : ORIGIN = ORIGIN(CLR_managed_RAM) + LENGTH(CLR_managed_RAM), LENGTH = 460800
/*       | nanoCLR graphics bitmaps,etc       |CLR>>*/  graphics_ram    (xrw) : ORIGIN = ORIGIN(graphics_vfb) + LENGTH(graphics_vfb), LENGTH = LENGTH(RAM_NS) -LENGTH(Native_RAM) - LENGTH(CLR_managed_RAM) - LENGTH(graphics_vfb)
/*       |------------------------------------|     */
/*       | External RAM                       |     */ 
/*       |------------------------------------|     */
/*       | 512?Mbit Octo?SPI NOR flash        |SYS>>*/  OCTOSPI2_FLASH (rx) : ORIGIN = 0x70000000, LENGTH = 64M
/*       | 512?Mbit  Hexadeca?SPI PSRAM       |SYS>>*/  OCTOSPI1_RAM (xrw)  : ORIGIN = 0x90000000, LENGTH = 64M
/*       \------------------------------------/     */
}
        
/*       /------------------------------------\     */
/*       |  VARIABLES REFERENCED BY C/C++     |     */   
/*       | ================================== |     */
/*       | The CLR managed heap for CLR       |CLR>>*/  HeapBegin             = ORIGIN(CLR_managed_RAM); 
/*       | objects                            |CLR>>*/  HeapEnd               = ORIGIN(CLR_managed_RAM) + LENGTH(CLR_managed_RAM); 
/*       |------------------------------------|     */   
/*       | CLR Image                          |CLR>>*/  __nanoImage_start__   = ORIGIN(nanoCLR); 
/*       |                                    |CLR>>*/  __nanoImage_end__     = LENGTH(nanoCLR); 
/*       |------------------------------------|     */   
/*       | Flash region where the C#          |CLR>>*/  __deployment_start__  = ORIGIN(deployment); 
/*       | Portable Executable (PE) file      |CLR>>*/  __deployment_size__   = LENGTH(deployment); 
/*       | is stored                          |CLR>>*/  __deployment_end__    = __deployment_start__ + __deployment_size__; 
/*       |------------------------------------|     */   
/*       | Small area of persistent storage   |     */  __nanoConfig_start__  = ORIGIN(config);
/*       |                                    |     */  __nanoConfig_end__    = ORIGIN(config) + config_size;
/*       |------------------------------------|     */   
/*       | Bit pixel copy of the screen       |CLR>>*/  Graphics_frame_buffer = ORIGIN(graphics_vfb); 
/*       | Updated by DMA                     |     */    
/*       |------------------------------------|     */   
/*       | Bitmap objects created by C#       |CLR>>*/  Graphics_managed_heap = ORIGIN(graphics_ram); 
/*       | and drawn into                     |CLR>>*/  Graphics_managed_end  = ORIGIN(graphics_ram) + LENGTH(graphics_ram);   
/*       |                                    |     */  
/*       \------------------------------------/     */



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
  } >Native_RAM AT> FLASH_NS
  
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
  } >Native_RAM

/* -----------------------------------------------------------------*/
/* Not using "DYNAMIC_MEMORY" so no need for a ._threadx_heap here.
/* -----------------------------------------------------------------*/

/* ------------------------------------------------------------------*/
/* Application defined for use as memory accessible by DMA1 and DMA2 */
/* ------------------------------------------------------------------*/
  .dma_buffer :
{
  *(.dma_buffer)
} >Native_RAM

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
  } >Native_RAM


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

