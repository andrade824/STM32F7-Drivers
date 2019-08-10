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

#include <ctype.h>
#include <stdint.h>
#include <string.h>

/* The sector size in bytes. */
#define FAT_SECTOR_SIZE 512U

/* Macros used to access multi-byte fields out of a byte-aligned buffer. */
#define EXTRACT_BYTE(p, offset) ((uint8_t)(p[offset]))
#define EXTRACT_HALF(p, offset) ((uint16_t)(p[offset] | (p[offset + 1] << 8)))
#define EXTRACT_WORD(p, offset) ((uint32_t)(EXTRACT_HALF(p, offset) | (EXTRACT_HALF(p, offset + 2) << 16)))

/* Byte offset into the MBR where the first partition entry is located. */
#define MBR_PART1_OFFSET 446U

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
#define FAT_BPB_VOL_LABEL       0x47 /* 11 Bytes */
#define BBP_VOL_LABEL_SIZE      11U

/* The number of File Allocation Tables (FAT) should always be 2. */
#define NUM_FATS 2

/* Size of a single directory record */
#define DIR_RECORD_SIZE 32U
#define RECORDS_PER_SEC (FAT_SECTOR_SIZE / DIR_RECORD_SIZE)

/**
 * 32-byte directory record field offsets.
 */
#define DIR_NAME             0x00 /* 11 Bytes */
#define DIR_NAME_SIZE        11U
#define DIR_FILE_NAME_SIZE   8U
#define DIR_FILE_EXT_SIZE    3U

#define DIR_ATTR             0x0B /* BYTE */
#define DIR_FIRST_CLUSTER_HI 0x14 /* HALF */
#define DIR_FIRST_CLUSTER_LO 0x1A /* HALF */
#define DIR_FILE_SIZE        0x1C /* WORD */

/* Bits within the ATTR field */
#define ATTR_LFN       0xF /* Long Filename records have first nibble set high */
#define ATTR_READ_ONLY 0x1
#define ATTR_HIDDEN    0x2
#define ATTR_SYSTEM    0x4
#define ATTR_VOLUME_ID 0x8
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20

/**
 * If the first byte of a directory record is 0xE5 that record is unused. If it's
 * zero, then that's the end of the directory.
 */
#define DIR_UNUSED 0xE5
#define END_OF_DIR 0x00

#define CLUSTER_MASK    0x0FFFFFFFU /* Only the bottom 28-bits of a cluster are valid. */
#define INVALID_CLUSTER 0xFFFFFFFFU /* Invalid cluster marker in the FAT. */
/**
 * Bits 7-31 of the cluster number is the sector offset into the FAT starting
 * from the fat_begin_lba.
 */
#define CLUSTER_FAT_LBA(x) ((x & CLUSTER_MASK) >> 7)

/**
 * Bits 0-6 of the cluster number is the index into the 128 uint32_t's in that
 * sector that represent the next cluster in the chain.
 */
#define CLUSTER_FAT_INDEX(x) (x & 0x7F)

/* Representation of a directory record. */
typedef struct {
	char name[DIR_NAME_SIZE + 1];
	uint32_t size; /* Size in bytes of the record. */
	uint32_t first_cluster;
	bool is_dir;
} fat_dir_entry_t;

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
 * Return the logical block address for a given cluster and a byte offset
 * into that cluster.
 *
 * @param cluster The cluster to derive the LBA from.
 * @param offset  The byte offset into the cluster to derive the LBA from.
 */
static inline uint32_t cluster_to_lba(uint32_t cluster, uint32_t offset)
{
	const uint32_t first_lba =
	    part.cluster_begin_lba + ((cluster - 2) * part.sectors_per_cluster);

	return first_lba + (offset / FAT_SECTOR_SIZE);
}

/**
 * Return back the next cluster in the cluster chain by parsing the FAT table.
 *
 * Bits 7-31 of the cluster number is the sector offset from the fat_begin_lba.
 * Bits 0-6 of the cluster number is the index into the 128 uint32_t's in that
 * sector that represent the next cluster in the chain.
 *
 * @param cluster The starting cluster, of which the next in the chain will be found.
 */
static uint32_t get_next_cluster(uint32_t cluster)
{
	ASSERT(cluster != INVALID_CLUSTER);

	const uint32_t fat_lba = part.fat_begin_lba + CLUSTER_FAT_LBA(cluster);

	if(part.ops.read_sectors(temp_sector, fat_lba, 1) != SD_SUCCESS) {
		ABORT("[FAT ERROR] Failed to read the FAT. LBA: %lu", fat_lba);
	}

	const uint32_t *clusters = (uint32_t*)&temp_sector;
	const uint32_t next_cluster = clusters[CLUSTER_FAT_INDEX(cluster)] & CLUSTER_MASK;

	return next_cluster;
}

/**
 * Search a given directory for an entry (file or sub-directory) within that
 * directory.
 *
 * @param name        The 8.3 filename (without the ".") to look for.
 * @param dir_cluster The starting cluster of the directory to search.
 * @param entry       A directory entry to fill if found.
 *
 * @return If the entry is found, then populate `entry` and return FAT_SUCCESS.
 *         Otherwise, return FAT_FILE_NOT_FOUND.
 */
static fat_status_t find_dir_entry(char *name, uint32_t dir_cluster, fat_dir_entry_t *entry)
{
	ASSERT(name != NULL);
	ASSERT(dir_cluster != INVALID_CLUSTER);
	ASSERT(entry != NULL);

	bool done_parsing = false;
	uint32_t current_cluster = dir_cluster;

	/**
	 * Loop through every cluster this directory spans and parse the directory
	 * entries in each sector.
	 */
	while(!done_parsing) {
		uint32_t current_sec = cluster_to_lba(current_cluster, 0);

		/* Loop through every sector in the current cluster and dump the entries. */
		for(uint8_t sec_index = 0; (sec_index < part.sectors_per_cluster) && !done_parsing; ++sec_index) {
			if(part.ops.read_sectors(temp_sector, current_sec + sec_index, 1) != SD_SUCCESS) {
				ABORT("[FAT ERROR] Failed to read a directory sector.");
			}

			/* Loop through every directory record in this sector (there are 16). */
			for(uint8_t record = 0; (record < RECORDS_PER_SEC) && !done_parsing; ++record) {
				const uint16_t offset = record * DIR_RECORD_SIZE;

				/**
				 * First byte determines whether a record is unusued or if this is
				 * the end of the directory listing.
				 */
				if(EXTRACT_BYTE(temp_sector, offset) == END_OF_DIR) {
					done_parsing = true;
					break;
				} else if(EXTRACT_BYTE(temp_sector, offset) == DIR_UNUSED) {
					continue;
				}

				const uint8_t record_attr = EXTRACT_BYTE(temp_sector, offset + DIR_ATTR);

				/* Skip any Long Filename records. */
				if((record_attr & ATTR_LFN) == ATTR_LFN) {
					continue;
				}

				memcpy((void*)entry->name, (void*)&temp_sector[offset + DIR_NAME], DIR_NAME_SIZE);
				entry->name[DIR_NAME_SIZE] = '\0';

				if(strcmp(name, entry->name) == 0) {
					/* Found the entry! */
					const uint16_t cluster_lo = EXTRACT_HALF(temp_sector, offset + DIR_FIRST_CLUSTER_LO);
					const uint16_t cluster_hi = EXTRACT_HALF(temp_sector, offset + DIR_FIRST_CLUSTER_HI);
					entry->first_cluster = cluster_lo | (cluster_hi << 16);
					entry->size = EXTRACT_WORD(temp_sector, offset + DIR_FILE_SIZE);
					entry->is_dir = (record_attr & ATTR_DIRECTORY) ? true : false;

					return FAT_SUCCESS;
				}
			}
		}

		/**
		 * If the file hasn't been found and there are more entries, get the
		 * next cluster the directory spans into.
		 */
		if(!done_parsing) {
			current_cluster = get_next_cluster(current_cluster);

			if(current_cluster == INVALID_CLUSTER) {
				/* Looks like the directory didn't contain an END_OF_DIR record. */
				done_parsing = false;
			}
		}
	}

	return FAT_FILE_NOT_FOUND;
}

/**
 * Parses an absolute path starting at the root directory and searches for a
 * wanted file in the 8.3 filename format.
 *
 * @note Any files with a name larger than 8 characters will be converted into
 *       "short" filename format (8.3) when created. That short filename will
 *       need to be passed into this method. Filenames larger than 8 characters
 *       are not supported. Filenames less than 8 characters are supported and
 *       will be padded with spaces as according to the 8.3 spec.
 *
 * @note Only one "period" is allowed in this file to denote the extension. If
 *       you create a file with multiple periods in the name, they will be
 *       stripped when converted to the 8.3 format anyway.
 *
 * @param path  The absolute path to the file starting at the root.
 * @param entry A directory entry to fill if file is found.
 *
 * @return If the entry is found, then populate `entry` and return FAT_SUCCESS.
 *         Otherwise, return FAT_FILE_NOT_FOUND, FAT_NOT_DIRECTORY, or
 *         FAT_IS_DIRECTORY depending on the error. The passed in `entry` will
 *         be clobbered in this case and should not be used.
 */
static fat_status_t parse_path(const char *path, fat_dir_entry_t *entry)
{
	ASSERT(path != NULL);
	ASSERT(entry != NULL);

	char name[DIR_NAME_SIZE + 1]; /* Used to hold the intermediate 8.3 filenames. */
	size_t name_index = 0;
	uint32_t temp_cluster = part.root_dir_first_cluster;

	bool done_parsing = false;

	/* If the name starts with a "/" to represent the root directory, skip that. */
	if(*path == '/') {
		path++;
	}

	while(!done_parsing) {
		name_index = 0;

		/* Parse the first eight characters before the extension. */
		for(unsigned int i = 0; i < DIR_FILE_NAME_SIZE; ++i, ++name_index) {
			if(*path == '/' || *path == '.' || *path == '\0') {
				/* Pad with spaces until 8 characters is reached. */
				name[name_index] = ' ';
				continue;
			}

			name[name_index] = toupper(*path);
			path++;
		}

		/* Check to see if there's an extension */
		if(*path == '.') {
			/* Grab up to the next three characters as the extension. */
			path++;

			for (unsigned int i = 0; i < DIR_FILE_EXT_SIZE; ++i, ++name_index) {
				if(*path == '/' || *path == '\0') {
					/* Pad with spaces until three characters is reached. */
					name[name_index] = ' ';
					continue;
				}

				name[name_index] = toupper(*path);
				path++;
			}
		} else {
			/* There's no extension so pad the last three characters with spaces. */
			name[DIR_FILE_NAME_SIZE]     = ' ';
			name[DIR_FILE_NAME_SIZE + 1] = ' ';
			name[DIR_FILE_NAME_SIZE + 2] = ' ';
		}

		name[DIR_NAME_SIZE] = '\0';

		fat_status_t ret = find_dir_entry(name, temp_cluster, entry);

		if(ret != FAT_SUCCESS) {
			return ret;
		}

		/* Skip any other characters until the next '/' or end of string. */
		while((*path != '/') && (*path != '\0')) {
			path++;
		}

		if(*path != '\0') {
			if(!entry->is_dir) {
				/* Expected a directory but found a file instead. */
				return FAT_NOT_DIRECTORY;
			}

			/* Skip the last '/' character to get to the start of the next name. */
			path++;
			temp_cluster = entry->first_cluster;
		} else {
			if(entry->is_dir) {
				/* We reached the end of the path and found a directory. */
				return FAT_IS_DIRECTORY;
			}

			/* We found the file! */
			done_parsing = true;
		}
	}

	return FAT_SUCCESS;
}

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
		ABORT("[FAT ERROR] Failed to read the MBR sector.");
	}

	uint8_t *mbr_part = &temp_sector[MBR_PART1_OFFSET];

	/* Validate the MBR sector. */
	if(EXTRACT_HALF(temp_sector, MBR_FAT_SIG_OFFSET) != MBR_FAT_SIG) {
		ABORT("[FAT ERROR] MBR Partition signature doesn't match 0xAA55");
	}

	const uint8_t part_type = EXTRACT_BYTE(mbr_part, MBR_PART_TYPE);
	if(!((part_type == MBR_FAT32_TYPE1) || (part_type == MBR_FAT32_TYPE2))) {
		ABORT("[FAT ERROR] First partition isn't FAT32.");
	}

	/* Sector address of the first sector in the FAT32 partition. */
	const uint32_t fat_bpb_lba = EXTRACT_WORD(mbr_part, MBR_PART_FIRST_LBA);

	/* Read the first partition on the MBR. */
	if(part.ops.read_sectors(temp_sector, fat_bpb_lba, 1) != SD_SUCCESS) {
		ABORT("[FAT ERROR] Failed to read the first FAT32 Volume ID.");
	}

	/**
	 * Validate the FAT32 Volume ID.
	 *
	 * Check the sector size, number of FATs, and the signature.
	 */
	if(EXTRACT_HALF(temp_sector, FAT_BPB_BYTES_PER_SEC) != FAT_SECTOR_SIZE) {
		ABORT("[FAT ERROR] Sector Size != 512 bytes");
	}

	if(EXTRACT_BYTE(temp_sector, FAT_BPB_NUM_FATS) != NUM_FATS) {
		ABORT("[FAT ERROR] The number of FATs != 2");
	}

	if(EXTRACT_HALF(temp_sector, MBR_FAT_SIG_OFFSET) != MBR_FAT_SIG) {
		ABORT("[FAT ERROR] FAT Volume ID signature doesn't match 0xAA55");
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
		ABORT("[FAT ERROR] Both the large and small total sector values are zero.");
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

	char vol_label[BBP_VOL_LABEL_SIZE + 1];
	for(size_t i = 0; i < BBP_VOL_LABEL_SIZE; ++i) {
		vol_label[i] = temp_sector[FAT_BPB_VOL_LABEL + i];
	}
	vol_label[BBP_VOL_LABEL_SIZE] = '\0';

	dbprintf("[FAT] Volume Label: %s\n", vol_label);

	return FAT_SUCCESS;
}

/**
 * Attempt to open a file and return a handle to the file if found.
 *
 * @param file Pointer to the file handle to populate if the file is found.
 * @param path The absolute path to the file starting at the root.
 * @param mode The mode to open the file in.
 *
 * @note Any files with a name larger than 8 characters will be converted into
 *       "short" filename format (8.3) when created. That short filename will
 *       need to be passed into this method. Filenames larger than 8 characters
 *       are not supported. Filenames less than 8 characters are supported and
 *       will be padded with spaces as according to the 8.3 spec.
 *
 * @note Only one "period" is allowed in this file to denote the extension. If
 *       you create a file with multiple periods in the name, they will be
 *       stripped when converted to the 8.3 format anyway.
 *
 * @note There are only read-only and write-only modes. There is no mode that
 *       allows for both reading and writing concurrently with the same file
 *       handle.
 *
 * @return If the file is found, then populate `file` and return FAT_SUCCESS.
 *         Otherwise, return FAT_FILE_NOT_FOUND, FAT_NOT_DIRECTORY, or
 *         FAT_IS_DIRECTORY depending on the error.
 */
fat_status_t fat_open(fat_file_t *file, const char *path, fat_open_mode_t mode)
{
	ASSERT(file != NULL);

	fat_dir_entry_t temp_entry;
	fat_status_t ret = parse_path(path, &temp_entry);

	if(ret != FAT_SUCCESS) {
		dbprintf("[FAT] Couldn't open \"%s\": %d\n", path, ret);
		return ret;
	}

	file->mode = mode;
	file->cluster = temp_entry.first_cluster;
	file->cluster_offset = 0;
	file->size = temp_entry.size;
	file->position = (mode == FAT_APPEND_MODE) ? file->size : 0;

	return FAT_SUCCESS;
}

/**
 * Read arbitrary data out of a file.
 *
 * @param file The file to read from.
 * @param buf  Buffer large enough to contain the read data.
 * @param size The number of bytes to read from the file.
 *
 * @return The number of bytes that was read (which maybe less than what you
 *         requested if the end of the file was hit).
 */
uint32_t fat_read(fat_file_t *file, void *buf, uint32_t size)
{
	ASSERT(file != NULL);
	ASSERT(buf != NULL);

	uint32_t bytes_read = 0;
	while(bytes_read < size) {
		/* The amount of bytes left to read. */
		uint32_t bytes_left = size - bytes_read;

		/* Make sure we haven't reached the end of the file yet. */
		if((file->position + bytes_left) > file->size) {
			bytes_left = file->size - file->position;
		}

		/* If we've read past the end of the file, then return early. */
		if(bytes_left == 0) {
			break;
		}

		/* The sector to read from. */
		const uint32_t file_lba = cluster_to_lba(file->cluster, file->cluster_offset);

		/* The offset within that sector to start reading data from. */
		const uint32_t sector_offset = file->cluster_offset % FAT_SECTOR_SIZE;

		/* How many bytes to read from that sector. */
		uint32_t sector_bytes = 0;
		if((sector_offset + bytes_left) > FAT_SECTOR_SIZE) {
			sector_bytes = FAT_SECTOR_SIZE - sector_offset;
		} else {
			sector_bytes = bytes_left;
		}

		if(part.ops.read_sectors(temp_sector, file_lba, 1) != SD_SUCCESS) {
			ABORT("[FAT ERROR] Failed to read sector from file. %lu", file_lba);
		}

		/* Copy data into the user's buffer. */
		uint8_t *offset_buf = ((uint8_t*)buf) + bytes_read;
		uint8_t *offset_sector = ((uint8_t*)&temp_sector) + sector_offset;
		memcpy((void*)offset_buf, (void*)offset_sector, sector_bytes);

		bytes_read += sector_bytes;

		/* Check to see if all bytes have been read from the current cluster. */
		if((file->cluster_offset + sector_bytes) >= part.cluster_size) {
			file->cluster_offset = 0;
			file->cluster = get_next_cluster(file->cluster);

			/* The file should still have more data to be read... */
			if(file->cluster == INVALID_CLUSTER) {
				ABORT("[FAT ERROR] Reached unexpected end of file while reading.");
			}
		} else {
			file->cluster_offset += sector_bytes;
		}

		file->position += sector_bytes;
	}

	return bytes_read;
}

/**
 * I'm leaving these methods in the code because they can be useful for debugging
 * purposes since they serve as a poor-man's on-device "ls". The algorithm is
 * mostly copy/pasted from find_dir_entry() above.
 */
#if 0
/**
 * Loop through every record in a directory and dump it to the console.
 */
static void dump_dir(uint32_t dir_first_cluster, uint16_t level)
{
	bool done_parsing = false;
	uint32_t current_cluster = dir_first_cluster;

	/**
	 * Loop through every cluster this directory spans and parse the directory
	 * entries in each sector.
	 */
	while(!done_parsing) {
		uint32_t current_sec = cluster_to_lba(current_cluster, 0);

		/* Loop through every sector in the current cluster and dump the entries. */
		for(uint8_t sec_index = 0; (sec_index < part.sectors_per_cluster) && !done_parsing; ++sec_index) {
			if(part.ops.read_sectors(temp_sector, current_sec + sec_index, 1) != SD_SUCCESS) {
				ABORT("[FAT ERROR] Failed to read a directory sector.");
			}

			/* Loop through every directory record in this sector (there are 16). */
			for(uint8_t record = 0; (record < RECORDS_PER_SEC) && !done_parsing; ++record) {
				const uint16_t offset = record * DIR_RECORD_SIZE;

				/**
				 * First byte determines whether a record is unusued or if this is
				 * the end of the directory listing.
				 */
				if(EXTRACT_BYTE(temp_sector, offset) == END_OF_DIR) {
					done_parsing = true;
					break;
				} else if(EXTRACT_BYTE(temp_sector, offset) == DIR_UNUSED) {
					continue;
				}

				const uint8_t record_attr = EXTRACT_BYTE(temp_sector, offset + DIR_ATTR);

				/* Skip any Long Filename records. */
				if((record_attr & ATTR_LFN) == ATTR_LFN) {
					continue;
				}

				char filename[DIR_NAME_SIZE + 1];
				memcpy((void*)filename, (void*)&temp_sector[offset + DIR_NAME], DIR_NAME_SIZE);
				filename[DIR_NAME_SIZE] = '\0';

				/* Print the filename with a series of hyphens preceding it based on directory depth. */
				for(uint16_t i = 0; i < level; ++i) {
					dbprintf("--");
				}

				if(level > 0) {
					dbprintf(" ");
				}

				if(record_attr & ATTR_DIRECTORY) {
					dbprintf("DIR: %s\n", filename);

					const uint16_t dir_cluster_lo = EXTRACT_HALF(temp_sector, offset + DIR_FIRST_CLUSTER_LO);
					const uint16_t dir_cluster_hi = EXTRACT_HALF(temp_sector, offset + DIR_FIRST_CLUSTER_HI);
					const uint32_t dir_cluster = dir_cluster_lo | (dir_cluster_hi << 16);

					if(level == 0 || (level > 0 && record >= 2)) {
						dump_dir(dir_cluster, level + 1);
					}

					/**
					 * The current sector needs to be reloaded because the recursive call
					 * probably overwrote the sector.
					 */
					if(part.ops.read_sectors(temp_sector, current_sec + sec_index, 1) != SD_SUCCESS) {
						ABORT("[FAT ERROR] Failed to read a directory sector.");
					}
				} else {
					const uint32_t filesize = EXTRACT_WORD(temp_sector, offset + DIR_FILE_SIZE);
					dbprintf("FILE: %s | SIZE: %lu bytes\n", filename, filesize);
				}
			}
		}

		/* If there are more entries, get the next cluster the directory spans into. */
		if(!done_parsing) {
			current_cluster = get_next_cluster(current_cluster);

			if(current_cluster == INVALID_CLUSTER) {
				/* Looks like the directory didn't contain an END_OF_DIR record. */
				done_parsing = false;
			}
		}
	}
}

/**
 * Loop through every record in the root directory and dump it to the console.
 */
void dump_root_dir(void)
{
	dump_dir(part.root_dir_first_cluster, 0);
}
#endif

#endif /* INCLUDE_FAT_DRIVER */