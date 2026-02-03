//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "target_board.h"

#if defined(DEBUG)
bool g_waitForDebuggerRequested = true;
#else
bool g_waitForDebuggerRequested = false;
#endif

int main(void)
{
    Initialize_Board();
    tx_kernel_enter();
}
/*
*
* Flash Size	    16,777,216
* PSRAM	            8,388,608
* ----------------------------
HeapBegin	        0x11000000		
HeapEnd	            0x11400000		

__nanoConfig_end__	        0x106c0000
__nanoConfig_start__	    0x106b0000
native_block_start	        0x0	        0
native_flash_blocks	        0xb	        11		720896

config_block_start	        0x6b	    107
config_block_end	        0x6b	    107
deployment_block_end	    0x6a	    106
deployment_block_start	    0xb	11
erase_block_size	        0x10000	    65536
flash_disk_size	            0x940000	9699328
flash_disk_start_address    0x106c0000
flash_start	                0x10000000
native_block_end	        0xa	10
native_flash_size	        0xb0000
native_flash_start_address	0x10000000
ram_disk_size	            0x40000	    262144
ram_disk_start_address	    0x11400000
total_flash_blocks	        0x100	    256
total_flash_size	        0x1000000	16777216

*/
