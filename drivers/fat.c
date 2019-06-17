/**
 * @author Devon Andrade
 * @created 5/11/2019
 *
 * FAT32 Filesystem Driver.
 */
#if INCLUDE_FAT_DRIVER

#include "config.h"
#include "debug.h"
#include "fat.h"

#include <stdint.h>

/* The sector size in bytes. */
#define FAT_SECTOR_SIZE 512

/* Macros used to access multi-byte fields out of a byte-aligned buffer. */
#define EXTRACT_BYTE(p, offset) ((uint8_t)(p[offset]))
#define EXTRACT_HALF(p, offset) ((uint16_t)(p[offset] | (p[offset + 1] << 8)))
#define EXTRACT_WORD(p, offset) ((uint32_t)(EXTRACT_HALF(p, offset) | (EXTRACT_HALF(p, offset + 2) << 16)))

/* Byte offset into the MBR where the first partition entry is located. */
#define MBR_PART1_OFFSET 446

/**
 * MBR partition entry field offsets
 */
#define MBR_PART_TYPE      0x4
#define MBR_PART_FIRST_LBA 0x8

/* The two valid MBR partition type codes for FAT32 */
#define MBR_FAT32_TYPE1 0xB
#define MBR_FAT32_TYPE2 0xC

/* The signature field for both the MBR partition and FAT32 Volume ID */
#define MBR_FAT_SIG_OFFSET 0x1FE
#define MBR_FAT_SIG        0xAA55

/**
 * FAT32 Volume ID (BIOS Partition Block) field offsets.
 */
#define FAT_BPB_BYTES_PER_SEC    0xB /* HALF */
#define FAT_BPB_SEC_PER_CLUSTER  0xD /* BYTE */
#define FAT_BPB_NUM_RESERVED     0xE /* HALF */
#define FAT_BPB_NUM_FATS        0x10 /* BYTE */
#define FAT_BPB_SMALL_TOTAL_SEC 0x13 /* HALF */
#define FAT_BPB_LARGE_TOTAL_SEC 0x20 /* WORD */
#define FAT_BPB_SEC_PER_FAT     0x24 /* WORD */
#define FAT_BPB_ROOT_CLUSTER    0x2C /* WORD */
#define FAT_BPB_VOL_LABEL       0x47 /* 11 bytes */

/* The number of File Allocation Tables (FAT) should always be 2. */
#define NUM_FATS 2

/* Data structure representing a FAT32 partition. */
typedef struct {
	fat_ops_t ops;
	uint32_t total_sectors;          /* Total logical sectors */
	uint32_t fat_begin_lba;          /* Partition_LBA_Begin + Number_Of_Reserved_Sectors */
	uint32_t cluster_begin_lba;      /* fat_begin_lba + (Number_of_FATs * Sectors_Per_FAT) */
	uint8_t sectors_per_cluster;
	uint32_t cluster_size;           /* In bytes */
	uint32_t root_dir_first_cluster;
} fat_partition_t;

/* The currently loaded FAT partition. */
static fat_partition_t part;

/* Temporary sector-sized buffer used when reading/writing data across sectors. */
static uint8_t temp_sector[FAT_SECTOR_SIZE];

/**
 * Initialize the FAT32 filesystem.
 *
 * This involves parsing the MBR partition to find the first FAT32 partition,
 * and then parsing the FAT32 Volume ID (BIOS partition block).
 */
fat_status_t fat_init(fat_ops_t ops)
{
	part.ops = ops;

	/* Read the first partition on the MBR. */
	if(part.ops.read_sectors(temp_sector, 0, 1) != SD_SUCCESS) {
		ABORT("FAT ERROR: Failed to read the MBR sector.");
	}

	uint8_t *mbr_part = &temp_sector[MBR_PART1_OFFSET];

	/* Validate the MBR sector. */
	if(EXTRACT_HALF(temp_sector, MBR_FAT_SIG_OFFSET) != MBR_FAT_SIG) {
		ABORT("FAT ERROR: MBR Partition signature doesn't match 0xAA55");
	}

	const uint8_t part_type = EXTRACT_BYTE(mbr_part, MBR_PART_TYPE);
	if(!((part_type == MBR_FAT32_TYPE1) || (part_type == MBR_FAT32_TYPE2))) {
		ABORT("FAT ERROR: First partition isn't FAT32.");
	}

	/* Sector address of the first sector in the FAT32 partition. */
	const uint32_t fat_bpb_lba = EXTRACT_WORD(mbr_part, MBR_PART_FIRST_LBA);

	/* Read the first partition on the MBR. */
	if(part.ops.read_sectors(temp_sector, fat_bpb_lba, 1) != SD_SUCCESS) {
		ABORT("FAT ERROR: Failed to read the first FAT32 Volume ID.");
	}

	/**
	 * Validate the FAT32 Volume ID.
	 *
	 * Check the sector size, number of FATs, and the signature.
	 */
	if(EXTRACT_HALF(temp_sector, FAT_BPB_BYTES_PER_SEC) != FAT_SECTOR_SIZE) {
		ABORT("FAT ERROR: Sector Size != 512 bytes");
	}

	if(EXTRACT_BYTE(temp_sector, FAT_BPB_NUM_FATS) != NUM_FATS) {
		ABORT("FAT ERROR: The number of FATs != 2");
	}

	if(EXTRACT_HALF(temp_sector, MBR_FAT_SIG_OFFSET) != MBR_FAT_SIG) {
		ABORT("FAT ERROR: FAT Volume ID signature doesn't match 0xAA55");
	}

	/**
	 * The small_total_secs value will be zero if the total sectors is greater
	 * than 65535. In that case, check the large_total_secs value.
	 */
	const uint16_t small_total_secs = EXTRACT_HALF(temp_sector, FAT_BPB_SMALL_TOTAL_SEC);
	const uint32_t large_total_secs = EXTRACT_WORD(temp_sector, FAT_BPB_LARGE_TOTAL_SEC);
	if(small_total_secs != 0) {
		part.total_sectors = small_total_secs;
	} else if(large_total_secs != 0) {
		part.total_sectors = large_total_secs;
	} else {
		ABORT("FAT ERROR: Both the large and small total sector values are zero.");
	}

	/* Extract needed values from the FAT Volume ID. */
	part.fat_begin_lba = fat_bpb_lba + EXTRACT_HALF(temp_sector, FAT_BPB_NUM_RESERVED);
	const uint32_t fat_sectors = EXTRACT_BYTE(temp_sector, FAT_BPB_NUM_FATS) * EXTRACT_WORD(temp_sector, FAT_BPB_SEC_PER_FAT);
	part.cluster_begin_lba = part.fat_begin_lba + fat_sectors;
	part.sectors_per_cluster = EXTRACT_BYTE(temp_sector, FAT_BPB_SEC_PER_CLUSTER);
	part.cluster_size = part.sectors_per_cluster * FAT_SECTOR_SIZE;
	part.root_dir_first_cluster = EXTRACT_WORD(temp_sector, FAT_BPB_ROOT_CLUSTER);

	dbprintf("[FAT] SD total_sectors: 0x%lx | fat_bpb_lba: 0x%lx | FAT total_sectors: 0x%lx | fat_begin_lba: 0x%lx | cluster_begin_lba: 0x%lx | sectors_per_cluster: 0x%x | cluster_size: 0x%lx | root_dir_first_cluster: 0x%lx\n",
	    part.ops.total_sectors, fat_bpb_lba, part.total_sectors, part.fat_begin_lba, part.cluster_begin_lba, part.sectors_per_cluster, part.cluster_size, part.root_dir_first_cluster);

	char vol_label[12];
	for(size_t i = 0; i < 11; ++i) {
		vol_label[i] = temp_sector[FAT_BPB_VOL_LABEL + i];
	}
	vol_label[11] = '\0';

	dbprintf("[FAT] Volume Label: %s\n", vol_label);

	return FAT_SUCCESS;
}

#endif /* INCLUDE_FAT_DRIVER */