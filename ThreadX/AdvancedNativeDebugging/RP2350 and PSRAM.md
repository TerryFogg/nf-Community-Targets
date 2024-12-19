



### [RP2350: How to enable XIP PSRAM?](https://forums.raspberrypi.com/viewtopic.php?t=375109#p2244432)

- [Quote](https://forums.raspberrypi.com/posting.php?mode=quote&p=2244432&sid=589ce4093daf2b99a50b82862f89d2d4)

[Wed Aug 14, 2024 7:36 am](https://forums.raspberrypi.com/viewtopic.php?p=2244432&sid=589ce4093daf2b99a50b82862f89d2d4#p2244432)

I want to enable XIP PSRAM on the RP2350. The CS line is connected to RP47.

As far as I can see I need to set alternate function F9 on GPIO47
I also need to set the memory as writable which I do by setting WRITABLE_M1 in the XIP CTRL register.
Is there anything else I need to do and are there any SDK helper functions to do this?

Thanks

- [matherp](https://forums.raspberrypi.com/memberlist.php?mode=viewprofile&u=231982&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Posts:** [396](https://forums.raspberrypi.com/search.php?author_id=231982&sr=posts&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Joined:** Tue May 02, 2017 10:54 am

### [Re: RP2350: How to enable XIP PSRAM?](https://forums.raspberrypi.com/viewtopic.php?t=375109#p2244934)

- [Quote](https://forums.raspberrypi.com/posting.php?mode=quote&p=2244934&sid=589ce4093daf2b99a50b82862f89d2d4)

[Fri Aug 16, 2024 7:19 am](https://forums.raspberrypi.com/viewtopic.php?p=2244934&sid=589ce4093daf2b99a50b82862f89d2d4#p2244934)

I'm using the Pimoroni PGA2350 module to test and is seems that all that is needed to enable the RAM is

Code: [Select all](https://forums.raspberrypi.com/viewtopic.php?t=375109#)

```
    gpio_set_function(47, GPIO_FUNC_XIP_CS1); // CS for PSRAM
    xip_ctrl_hw->ctrl|=XIP_CTRL_WRITABLE_M1_BITS;
```

Another question though. Having enabled the RAM is appears at address 0x11000000.
How do I create a memory section to map this?
In the STM32 world I would just define the section in the .ld file but cmake seems to hide all that from me.

- [arg001](https://forums.raspberrypi.com/memberlist.php?mode=viewprofile&u=256228&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Posts:** [982](https://forums.raspberrypi.com/search.php?author_id=256228&sr=posts&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Joined:** Tue Jan 23, 2018 10:06 am

  **Location:** Cambridge, UK

### [Re: RP2350: How to enable XIP PSRAM?](https://forums.raspberrypi.com/viewtopic.php?t=375109#p2244948)

- [Quote](https://forums.raspberrypi.com/posting.php?mode=quote&p=2244948&sid=589ce4093daf2b99a50b82862f89d2d4)

[Fri Aug 16, 2024 8:19 am](https://forums.raspberrypi.com/viewtopic.php?p=2244948&sid=589ce4093daf2b99a50b82862f89d2d4#p2244948)

> matherp wrote:In the STM32 world I would just define the section in the .ld file but cmake seems to hide all that from me.

It's not very deeply hidden - you can use:

Code: [Select all](https://forums.raspberrypi.com/viewtopic.php?t=375109#)

```
pico_set_linker_script(my_program ${CMAKE_SOURCE_DIR}/my_custom_script.ld)
```

And you can then go and copy the standard scripts out of **pico-sdk/src/rp2_common/pico_crt0/rp2350** to use as the basis of your own one.

However, the 2.0.0 SDK does seem to have introduced a certain amount of magic to the script; I notice that the FLASH output region is no longer defined in the .ld file (to allow dynamic specification of the hardware flash size). I haven't found how it sets this yet, maybe via command-line options to the linker. Possibly a similar approach could be used to create a region for the PSRAM.

- [![User avatar](https://forums.raspberrypi.com/download/file.php?avatar=387220_1700092725.jpg)](https://forums.raspberrypi.com/memberlist.php?mode=viewprofile&u=387220&sid=589ce4093daf2b99a50b82862f89d2d4)[adam_green](https://forums.raspberrypi.com/memberlist.php?mode=viewprofile&u=387220&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Posts:** [132](https://forums.raspberrypi.com/search.php?author_id=387220&sr=posts&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Joined:** Tue Dec 14, 2021 12:43 am

### [Re: RP2350: How to enable XIP PSRAM?](https://forums.raspberrypi.com/viewtopic.php?t=375109#p2244962)

- [Quote](https://forums.raspberrypi.com/posting.php?mode=quote&p=2244962&sid=589ce4093daf2b99a50b82862f89d2d4)

[Fri Aug 16, 2024 9:49 am](https://forums.raspberrypi.com/viewtopic.php?p=2244962&sid=589ce4093daf2b99a50b82862f89d2d4#p2244962)

> [arg001](https://forums.raspberrypi.com/memberlist.php?mode=viewprofile&u=256228&sid=589ce4093daf2b99a50b82862f89d2d4) wrote: [↑](https://forums.raspberrypi.com/viewtopic.php?p=2244948&sid=589ce4093daf2b99a50b82862f89d2d4#p2244948)
>
> Fri Aug 16, 2024 8:19 am
>
> However, the 2.0.0 SDK does seem to have introduced a certain amount of magic to the script; I notice that the FLASH output region is no longer defined in the .ld file (to allow dynamic specification of the hardware flash size). I haven't found how it sets this yet, maybe via command-line options to the linker. Possibly a similar approach could be used to create a region for the PSRAM.

It looks like it gets the FLASH size from a board header file through comments of the following format:

Code: [Select all](https://forums.raspberrypi.com/viewtopic.php?t=375109#)

```
// pico_cmake_set_default PICO_FLASH_SIZE_BYTES = (16 * 1024 * 1024)
```

The magic in the CMake magic which appears to accomplishes this is comprised of:
*** cmake/generic_board.cmake -** Reads the board header file, looking for the **pico_cmake_set_default** comments and turns them into CMake variable settings.
*** src/rp2_common/pico_standard_link/CMakeLists.txt -** Uses the following line to replace the FLASH size in a template with the CMake variable created above:

Code: [Select all](https://forums.raspberrypi.com/viewtopic.php?t=375109#)

```
configure_file(${CMAKE_CURRENT_LIST_DIR}/pico_flash_region.template.ld ${CMAKE_BINARY_DIR}/pico_flash_region.ld)
```

\* **src/rp2_common/pico_standard_link/pico_flash_region.template.ld -** The template used as the source file in the above **configure_file** command:

Code: [Select all](https://forums.raspberrypi.com/viewtopic.php?t=375109#)

```
FLASH(rx) : ORIGIN = 0x10000000, LENGTH = ${PICO_FLASH_SIZE_BYTES_STRING}
```

*** src/rp2_common/pico_crt0/rp2350/\*.ld -** The linker scripts in this folder **INCLUDE** the **pico_flash_region.ld** created above as part of the build process.

- [arg001](https://forums.raspberrypi.com/memberlist.php?mode=viewprofile&u=256228&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Posts:** [982](https://forums.raspberrypi.com/search.php?author_id=256228&sr=posts&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Joined:** Tue Jan 23, 2018 10:06 am

  **Location:** Cambridge, UK

### [Re: RP2350: How to enable XIP PSRAM?](https://forums.raspberrypi.com/viewtopic.php?t=375109#p2244966)

- [Quote](https://forums.raspberrypi.com/posting.php?mode=quote&p=2244966&sid=589ce4093daf2b99a50b82862f89d2d4)

[Fri Aug 16, 2024 10:13 am](https://forums.raspberrypi.com/viewtopic.php?p=2244966&sid=589ce4093daf2b99a50b82862f89d2d4#p2244966)

Thanks for digging out that detail.

So it looks like what we really want is for pico_flash_region.template.ld to also define a region for PSRAM that could be enabled from the board file. Not easy to see how to do that as an add-on to the existing SDK (while lobbying for the SDK change to make it easier in future). Probably easiest to just have a project-specific linker script in the meanwhile and plan to change it.

- [matherp](https://forums.raspberrypi.com/memberlist.php?mode=viewprofile&u=231982&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Posts:** [396](https://forums.raspberrypi.com/search.php?author_id=231982&sr=posts&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Joined:** Tue May 02, 2017 10:54 am

### [Re: RP2350: How to enable XIP PSRAM?](https://forums.raspberrypi.com/viewtopic.php?t=375109#p2245101)

- [Quote](https://forums.raspberrypi.com/posting.php?mode=quote&p=2245101&sid=589ce4093daf2b99a50b82862f89d2d4)

[Fri Aug 16, 2024 5:59 pm](https://forums.raspberrypi.com/viewtopic.php?p=2245101&sid=589ce4093daf2b99a50b82862f89d2d4#p2245101)

I've hacked the template in the sdk and that seems to propagate into any binaries. I can't see any reason why that shouldn't be benign. Likewise I've set the flash set permanently at 16Mb

Code: [Select all](https://forums.raspberrypi.com/viewtopic.php?t=375109#)

```
MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = (16 * 1024 * 1024)
    RAM(rwx) : ORIGIN =  0x20000000, LENGTH = 512k
    SCRATCH_X(rwx) : ORIGIN = 0x20080000, LENGTH = 4k
    SCRATCH_Y(rwx) : ORIGIN = 0x20081000, LENGTH = 4k
	PSRAM(rwx) : ORIGIN = 0x11000000, LENGTH = (8 * 1024 * 1024)
}
```

Code: [Select all](https://forums.raspberrypi.com/viewtopic.php?t=375109#)

```
    .psram (NOLOAD): {
        . = ALIGN(4);
        *(.psram*)
    } > PSRAM
```

Next idea is to set the heap into the PSRAM and see what happens

- [kilograham](https://forums.raspberrypi.com/memberlist.php?mode=viewprofile&u=294330&sid=589ce4093daf2b99a50b82862f89d2d4)

  Raspberry Pi Engineer & Forum Moderator ![Raspberry Pi Engineer & Forum Moderator](https://forums.raspberrypi.com/images/ranks/Forum-Banners_Engineer.png)

  **Posts:** [1699](https://forums.raspberrypi.com/search.php?author_id=294330&sr=posts&sid=589ce4093daf2b99a50b82862f89d2d4)

  **Joined:** Fri Apr 12, 2019 11:00 am

  **Location:** austin tx

### [Re: RP2350: How to enable XIP PSRAM?](https://forums.raspberrypi.com/viewtopic.php?t=375109#p2245178)

- [Quote](https://forums.raspberrypi.com/posting.php?mode=quote&p=2245178&sid=589ce4093daf2b99a50b82862f89d2d4)

[Fri Aug 16, 2024 11:10 pm](https://forums.raspberrypi.com/viewtopic.php?p=2245178&sid=589ce4093daf2b99a50b82862f89d2d4#p2245178)

probably open a github issue so we can add more regions to the linker script by default... we wanted to get generic linker script templates in for this release, but it turned out to be hard for various reasons!







### [RP2350: Combining PSRAM with flash storage - don't forget the cache!](https://forums.raspberrypi.com/viewtopic.php?t=375845#p2248383)

- [Quote](https://forums.raspberrypi.com/posting.php?mode=quote&p=2248383&sid=9408ba98468fcdd7d4f56564addf7c1a)

[Thu Aug 29, 2024 5:40 am](https://forums.raspberrypi.com/viewtopic.php?p=2248383&sid=9408ba98468fcdd7d4f56564addf7c1a#p2248383)

Leaving this as a note for anyone who may run into this issue in the future.

I'm working on an app that needs to store some file data in the internal flash, and also uses PSRAM for extra memory (tested using the Pimoroni Pico Plus 2). I'm using the SparkFun PSRAM allocator from https://github.com/sparkfun/sparkfun-pico to configure PSRAM, and it was all going swimmingly for a while. However, once I started implementing the flash storage code, I was running into some really weird issues where the allocator would start to throw assertion failures at random places after writing to flash.

I first thought I was running out of stack space in my FreeRTOS task, so I bumped it up, but this didn't solve the problem. I soon figured out that the flash writes were resetting some of the PSRAM configs, so I had my flash code reconfigure the PSRAM after finishing writing, which seemed to help a little, but not completely. I then tried a different allocator (umm_alloc), but this one would instead give addresses that were already allocated in previous calls. Running an integrity check on the allocator directly after reconfiguring PSRAM revealed something was being corrupted.

Eventually, I realized the issue: the XIP memory is cached, and initiating a write disables XIP (understandably) and *invalidates* the cache. Some memory changes to the allocator state weren't being written to the PSRAM chip! To fix this, I made my code clean the cache before initiating any flash commands, using the cache maintenance block at 0x18000000:

Code: [Select all](https://forums.raspberrypi.com/viewtopic.php?t=375845#)

```
    if (psramActive) {
        // flush XIP cache to PSRAM
        for (volatile uint8_t* cache = (volatile uint8_t*)0x18000001; cache < (volatile uint8_t*)(0x18000001 + 2048 * 8); cache += 8)
            *cache = 0;
        psramActive = false;
    }
    // flash action...
```

I have no idea if this is the "correct" way to do it, but so far, I've had zero memory corruption issues, and zero crashes as a result of bad allocator state. Hope this helps anyone else who's having issues with using PSRAM and flash write commands together.