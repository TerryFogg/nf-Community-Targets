#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Fetch repository
    include(FetchContent)
    if(IS_DIRECTORY ${LEVELX_SOURCE})
	    message(STATUS "Source code for Eclipse Levelx from local folder ${LEVELX_SOURCE}")
	    FetchContent_Declare( levelX SOURCE_DIR ${LEVELX_SOURCE} )
    else()
	    message(STATUS "Source code for Eclipse Levelx from GitHub repo version ${LEVELX_VERSION}")
	    FetchContent_Declare( levelX GIT_REPOSITORY https://github.com/eclipse-threadx/levelX.git GIT_TAG ${LEVELX_VERSION} )
    endif()
    FetchContent_Populate(levelX)

# Common
        list(APPEND OPTIONAL_INCLUDES 
            ${LEVELX_SOURCE}/common/inc
            )
        list(APPEND OPTIONAL_SOURCES
#	        ${LEVELX_SOURCE}/common/src/fx_nand_flash_simulated_driver.c
#	        ${LEVELX_SOURCE}/common/src/fx_nor_flash_simulator_driver.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_256byte_ecc_check.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_256byte_ecc_compute.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_block_allocate.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_block_data_move.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_block_find.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_block_mapping_set.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_block_status_set.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_close.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_data_page_copy.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_defragment.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_driver_block_erase.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_driver_block_erased_verify.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_driver_block_status_get.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_driver_block_status_set.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_driver_page_erased_verify.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_erase_count_set.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_extended_cache_enable.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_format.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_free_block_list_add.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_initialize.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_mapped_block_list_add.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_mapped_block_list_get.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_mapped_block_list_remove.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_memory_initialize.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_metadata_allocate.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_metadata_build.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_metadata_write.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_open.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_page_ecc_check.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_page_ecc_compute.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_partial_defragment.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_sectors_read.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_sectors_release.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_sectors_write.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_sector_read.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_sector_release.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_sector_write.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_simulator.c
            ${LEVELX_SOURCE}/common/src/lx_nand_flash_system_error.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_block_reclaim.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_close.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_defragment.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_driver_block_erase.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_driver_read.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_driver_write.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_extended_cache_enable.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_initialize.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_logical_sector_find.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_next_block_to_erase_find.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_open.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_partial_defragment.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_physical_sector_allocate.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_sector_mapping_cache_invalidate.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_sector_read.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_sector_release.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_sector_write.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_simulator.c
            ${LEVELX_SOURCE}/common/src/lx_nor_flash_system_error.c
        )