#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

extern const int HeapBegin;
extern const int HeapEnd;

extern const uint32_t erase_block_size;

extern const uint32_t flash_start;
extern const uint32_t total_flash_size;

extern const uint32_t native_flash_start_address;
extern const uint32_t native_flash_size;

extern const uint32_t total_flash_blocks;
extern const uint32_t native_flash_blocks;
extern const uint32_t native_block_start;
extern const uint32_t native_block_end;
extern const uint32_t deployment_block_start;
extern const uint32_t deployment_block_end;
extern const uint32_t config_block_start;
extern const uint32_t config_block_end;

//extern uint32_t __nanoImage_start__;
//extern uint32_t __nanoImage_end__;
extern uint32_t __nanoConfig_start__;
extern uint32_t __nanoConfig_end__;
extern uint32_t __nanoConfig_size__;

//extern uint32_t __deployment_start__;
//extern uint32_t __deployment_end__;

extern const uint32_t ram_disk_start_address;
extern const uint32_t ram_disk_size;
extern const uint32_t flash_disk_start_address;
extern const uint32_t flash_disk_size;
