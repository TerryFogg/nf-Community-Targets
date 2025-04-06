/* Copyright (c) .NET Foundation and Contributors                     */
/*   See LICENSE file in the project root for full license information. */

MEMORY
{
   /* Physical memory - STM32H7B3LIH6QU */
   /* --------------------------------- */
     FLASH      (rx) :ORIGIN = 0x08000000,LENGTH = 2048k
     OTP        (rx) :ORIGIN = 0x08FFF000,LENGTH = 1024

     ITCM_SRAM  (rxw):ORIGIN = 0x00000000,LENGTH = 64K 
     DTCM_SRAM  (rxw):ORIGIN = 0x20000000,LENGTH = 128K

     AXI_RAM1   (rxw):ORIGIN = 0x24000000,LENGTH = 256K
     AXI_RAM2   (rxw):ORIGIN = 0x24040000,LENGTH = 384K
     AXI_RAM3   (rxw):ORIGIN = 0x240A0000,LENGTH = 384K
     /* Contiguous ram base on AXI group */
     RAM        (rxw):ORIGIN = ORIGIN(AXI_RAM1)  ,LENGTH = LENGTH(AXI_RAM1) + LENGTH(AXI_RAM2) + LENGTH(AXI_RAM3)

     AHB_RAM    (rxw):ORIGIN = 0x30000000,LENGTH = 128K   /* (64K + 64K) */
     SRD_RAM    (rxw):ORIGIN = 0x38000000,LENGTH = 32K
     BACKUP_RAM (rxw):ORIGIN = 0x38800000,LENGTH = 4k

     /* External RAM on development board (STM32H7B3I_DK) */
     OCTOSPI1_FLASH (xrw) : ORIGIN = 0x09000000, LENGTH = 64M
     FMC_SDRAM      (xrw) : ORIGIN = 0xD0000000, LENGTH = 16M

   /* Allocated memory */
   /* ---------------- */
     NATIVE_FLASH     (rx)  : ORIGIN = ORIGIN(FLASH), LENGTH = (320 * 1024)
     CONFIG_FLASH     (rx)  : ORIGIN = ORIGIN(NATIVE_FLASH) + LENGTH(NATIVE_FLASH), LENGTH = (64 * 1024)
     DEPLOYMENT_FLASH (rx)  : ORIGIN = ORIGIN(CONFIG_FLASH) + LENGTH(CONFIG_FLASH), LENGTH = LENGTH(FLASH) - LENGTH(CONFIG_FLASH) - LENGTH(NATIVE_FLASH)

     FLASH_DISK       (rx)  : ORIGIN = ORIGIN(OCTOSPI1_FLASH),LENGTH = LENGTH(OCTOSPI1_FLASH)

     NATIVE_RAM       (rxw) : ORIGIN = ORIGIN(RAM),     LENGTH = (120 * 1024)
     CLR_MANAGED_RAM  (rxw) : ORIGIN = ORIGIN(NATIVE_RAM), LENGTH =  LENGTH(RAM) - LENGTH(NATIVE_RAM) 
     RAM_DMA          (rxw) : ORIGIN = ORIGIN(AHB_RAM), LENGTH = LENGTH(AHB_RAM)

     GRAPHICS_FRAME_BUFFER (rxw) : ORIGIN = ORIGIN(FMC_SDRAM), LENGTH = 255K
     GRAPHICS_HEAP         (rxw) : ORIGIN = ORIGIN(GRAPHICS_FRAME_BUFFER) + LENGTH(GRAPHICS_FRAME_BUFFER), LENGTH = 4M
     /* Not used */
     GRAPHICS_HEAP_FAST    (rxw) : ORIGIN = 0, LENGTH = 0
}

/* NOTE */
/* Division of internal flash is limited by the smallest erase size of 8K */
/* ------------------------------------ */
/* Allocation by number of erase blocks */
/* ------------------------------------ */
FLASH_ERASE_SIZE = 8;
FLASH_DISK_ERASE_SIZE = 8;

erase_block_size = (FLASH_ERASE_SIZE * 1024);
flash_start = ORIGIN(FLASH);
total_flash_size = LENGTH(FLASH); 


ASSERT( LENGTH(NATIVE_FLASH) % (FLASH_ERASE_SIZE * 1024) == 0, "Native flash allocation must be an integral number block erase size");
ASSERT( LENGTH(DEPLOYMENT_FLASH) % (FLASH_ERASE_SIZE * 1024) == 0, "Deployment flash allocation must be an integral number block erase size");
ASSERT( LENGTH(CONFIG_FLASH) % (FLASH_ERASE_SIZE * 1024) == 0, "Config flash allocation must be an integral number block erase size");

ASSERT( LENGTH(FLASH_DISK) % (FLASH_DISK_ERASE_SIZE * 1024) == 0, "Flash disk allocation must be an integral number block erase size");

total_flash_blocks = LENGTH(FLASH)/(FLASH_ERASE_SIZE * 1024); 
native_flash_blocks = LENGTH(NATIVE_FLASH)/(FLASH_ERASE_SIZE * 1024); 
deployment_flash_blocks = LENGTH(DEPLOYMENT_FLASH)/(FLASH_ERASE_SIZE * 1024);
config_flash_blocks = LENGTH(CONFIG_FLASH)/(FLASH_ERASE_SIZE * 1024);


/* ----------------------------- */
/* Variables used by 'memory.cpp' */
/* ----------------------------- */
HeapBegin             = ORIGIN(CLR_MANAGED_RAM); 
HeapEnd               = HeapBegin + LENGTH(CLR_MANAGED_RAM); 
/*----------------------------------------------------------*/

/* -------------------------------------------- */
/* Variables used by 'Device_BlockStorage.c'
/* -------------------------------------------- */
native_flash_start_address = ORIGIN(NATIVE_FLASH);
native_flash_size = LENGTH(NATIVE_FLASH);
native_block_start = 0;
native_block_end   = native_block_start + native_flash_blocks - 1;
deployment_block_start = native_block_end + 1;
deployment_block_end   = deployment_block_start + deployment_flash_blocks - 1;
config_block_start = deployment_block_end + 1;
config_block_end   = config_block_start + config_flash_blocks - 1;
/*----------------------------------------------------------------------------*/

/* -------------------------------------- */
/* Variables used by 'Graphics_Memory.cpp'
/* -------------------------------------- */
Graphics_managed_heap          = ORIGIN(GRAPHICS_HEAP); 
Graphics_managed_end           = Graphics_managed_heap + LENGTH(GRAPHICS_HEAP);   
Graphics_heap_fast_address     = ORIGIN(GRAPHICS_HEAP_FAST); 
Graphics_heap_end_fast_address = Graphics_heap_fast_address + LENGTH(GRAPHICS_HEAP_FAST);

/* For Video controller this is a bit pixel copy of the screen */
GRAPHICS_FRAME_BUFFER = ORIGIN(GRAPHICS_FRAME_BUFFER); 

/*--------------------------------------------------------------------------------------*/

ENTRY(Reset_Handler)

/* _estack is used in ThreadX to define the end of the stack memory for the main thread */
_estack = ORIGIN(DTCM_SRAM) + LENGTH(DTCM_SRAM);

/* _sidata is used in ThreadX to copy the data segment from ROM to RAM during initialization */
_sidata = LOADADDR(.data);

 /* Stack and heap size */
_Min_Heap_Size = 0x200; 
_Min_Stack_Size = 0x400; 


SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

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

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

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

  .fini_array : /*  */
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

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

  .dma_buffer :
{
  *(.dma_buffer)
} >RAM_DMA

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
