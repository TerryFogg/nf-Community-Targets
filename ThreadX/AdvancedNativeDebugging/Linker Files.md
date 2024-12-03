# An Introduction to Linker Files: Crafting Your Own for Embedded Projects

# 1. What is a linker file?

A linker file, commonly known as a linker script, is more than just a map for memory allocation. It controls every aspect of the linking process, describing how sections in input files should map into the output file and manage the memory layout. Beyond mapping, a linker script can direct the linker to perform a range of operations using specific linker commands.

![img](https://miro.medium.com/v2/resize:fit:680/1*FH4VTikgEoTx_MqytBfUrQ.jpeg)

# 2. Anatomy of a linker file

A typical linker file consists of five key components:

- **MEMORY**: This block defines the available memory regions in your embedded system, specifying their start addresses, sizes, and access permissions. Each memory region is given a name, such as FLASH or RAM, which will be used later when assigning sections to memory regions. The access permissions are represented by a combination of ‘r’ (read), ‘w’ (write), and ‘x’ (execute).
- **SECTIONS**: This block specifies the placement of different sections within the memory regions defined in the MEMORY block. Sections like .text (for code), .data (for initialized data), and .bss (for uninitialized data) are common in embedded projects. You can also create custom sections for specific purposes. The ‘>’ symbol is used to assign a section to a memory region.
- **ENTRY**: This directive sets the entry point for the program execution. It is usually the address of the reset handler or the main function. The linker will generate an error if the specified entry point is not found in the code.
- **SYMBOLS**: This optional component allows you to define custom symbols in your linker file. These symbols can be used for various purposes, such as defining the start and end addresses of a particular section or memory region, or as constants in your code. You can also create aliases for existing symbols using the ALIAS directive.

The script can also include advanced directives like `NOLOAD` for non-loadable sections, `AT` for defining load addresses differing from runtime addresses, and `ALIAS` for creating symbol aliases.

# 3. Creating a basic linker file: Step-by-step guide

Let’s create a simple linker file for an example microcontroller with 64KB of flash memory and 16KB of RAM:

Step 1: Define memory regions

```
MEMORY { 
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}
```

Step 2: Specify sections placement SECTIONS

```
SECTIONS
{
  .text :
  {
    *(.text)
    *(.text.*)
    . = ALIGN(4);
  } >FLASH

  .data :
  {
    *(.data)
    . = ALIGN(4);
  } >RAM

  .bss :
  {
    *(.bss)
    . = ALIGN(4);
  } >RAM
}
```

Step 3: Set the entry point

```
ENTRY(Reset_Handler)
```

Step 4: Define some symbols

```
_stack_top = 0x20004000;
PROVIDE(_heap_start = 0x20002000);
ALIAS(_heap_end, _stack_top);
```

Step 5: Save your linker file with a .ld extension

# 4. Advanced linker file features

For complex projects, advanced features are essential for fine-tuned memory allocation and organization. Features like `PHDRS` command help define program headers for ELF files, and the `VERSION` command manages symbol versioning. The `MEMORY` command is crucial for specifying the size and location of memory blocks, and the `SECTIONS` command details how to map input sections into output sections.

- **NOLOAD**: The NOLOAD keyword indicates that a particular section should not be loaded into memory during the program’s startup. This is useful for sections that are intended to be used only during debugging or for sections that are loaded dynamically at runtime. For example:

```
SECTIONS
{
  .debug_info (NOLOAD) :
  {
    *(.debug_info)
  } >FLASH
}
```

- **AT**: The AT directive is used to define the load address of a section, which can be different from the runtime address. This is helpful when you want to store a section in one memory region (e.g., flash) but have it loaded into another memory region (e.g., RAM) at runtime. For example:

```
SECTIONS
{
  .data :
  {
    *(.data)
    . = ALIGN(4);
  } >RAM AT>FLASH
}
```

- **ALIAS:** The ALIAS directive allows you to create an alias for an existing symbol. This can be useful for providing backward compatibility with older code or for creating more descriptive names for commonly used symbols. For example:

```
ALIAS(_legacy_heap_start, _heap_start);
```

- **ASSERT**: The ASSERT directive is used to generate an error during the linking process if a specified condition is not met. This can be helpful for ensuring that certain constraints are satisfied, such as making sure that a section does not overflow its assigned memory region. For example:

```
ASSERT(.text <= 64K, "Text section overflow - FLASH memory region is too small!");
```

# 5. Examples of linker files for common microcontrollers

In this section, we provide example linker files tailored for popular microcontrollers like STM32F103, STM32F407, and STM32G071.

## STM32F103 Linker File Example:

The STM32F103 microcontroller has 64KB of flash memory and 20KB of RAM. Let’s create a linker file for this microcontroller:

```
MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
  CCMRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0K
}

ENTRY(Reset_Handler)

SECTIONS
{
  .text :
  {
    *(.text)
    *(.text.*)
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
  } >FLASH

  .data :
  {
    *(.data)
    . = ALIGN(4);
  } >RAM AT>FLASH

  .bss :
  {
    *(.bss)
    . = ALIGN(4);
  } >RAM

}
```

# 6. Important memory concepts

## Heap Memory

Heap memory is a region of RAM used for dynamic memory allocation. In embedded systems, the heap allows you to allocate and deallocate memory during runtime using functions like malloc(), calloc(), realloc(), and free(). The heap memory is managed by the memory allocation routines provided by the C library.

![img](https://miro.medium.com/v2/resize:fit:995/1*gmaUHAAyThlX8NkMA5N7rA.png)

To define the heap in your linker file, you can create custom symbols that represent the start and end addresses of the heap region. For example:

```
_heap_start = ORIGIN(RAM) + LENGTH(RAM) - 4K;
_heap_end = ORIGIN(RAM) + LENGTH(RAM);
```

In this example, we allocate 4KB of RAM for the heap, starting 4KB below the end of the RAM region. Ensure that the heap and stack do not overlap, as this can cause memory corruption.

## Stack Memory

The stack is another important memory region in embedded systems. It is used for storing local variables, function call parameters, and return addresses. The stack grows downward, meaning that as new data is added, the stack pointer moves towards lower memory addresses.

To define the stack size in your linker file, you can create a custom symbol representing the stack top address. Typically, the stack is placed at the end of the RAM region. For example:

```
_stack_top = ORIGIN(RAM) + LENGTH(RAM);
```

In your startup code, you’ll need to initialize the stack pointer register with the value of the _stack_top symbol. The stack size depends on your specific application requirements and can be adjusted according to the available memory.

# 7. Tips for optimizing memory allocation

Creating and debugging linker files can be a challenging task, but these tips will help you streamline the process and address common issues:

- Consult the datasheet or reference manual: Always refer to your microcontroller’s datasheet or reference manual for accurate information on memory regions, addresses, and sizes. This ensures that your linker file is tailored to your specific microcontroller.
- Start with a template: If possible, start with a linker script template provided by your microcontroller vendor, as it will include the necessary configuration for your specific microcontroller family. You can then customize the template to suit your project requirements.
- Verify your memory layout: Use a memory map provided by your development tool or IDE to visualize your memory layout. This helps ensure that sections are placed correctly and that there are no overlaps or memory corruption issues.
- Check for alignment: Make sure that your sections are correctly aligned, as misaligned sections can lead to unpredictable behavior. Use the ALIGN() function in your linker file to enforce proper alignment.
- Use ASSERT() for constraints: Use the ASSERT() directive to ensure that certain constraints are satisfied, such as making sure that a section does not overflow its assigned memory region.
- Debug with linker map file: Most toolchains generate a linker map file that provides detailed information about section addresses, sizes, and symbols. Analyze the map file to troubleshoot issues related to memory allocation and placement.

