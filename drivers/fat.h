/**
 * @author Devon Andrade
 * @created 5/11/2019
 *
 * FAT32 Filesystem Driver.
 */
#if INCLUDE_FAT_DRIVER
#pragma once

#include "sdmmc.h"

#include <stdint.h>

/* Data and operations needed to interact with a storage medium. */
typedef struct {
	uint64_t total_size;         /* Total size in bytes */
	uint32_t total_sectors;      /* Total sectors availables on device */
	sd_status_t (*read_sectors)(void *data, uint32_t sec_addr, uint16_t num_sectors);
	sd_status_t (*write_sectors)(void *data, uint32_t sec_addr, uint16_t num_sectors);
} fat_ops_t;

/* FAT file status flags */
typedef enum {
	FAT_FAIL           = 0,
	FAT_SUCCESS        = 1,
	FAT_FILE_NOT_FOUND = 2, /* The wanted file or directory was not found. */
	FAT_IS_DIRECTORY   = 3, /* Wanted a file but a directory was found instead. */
	FAT_NOT_DIRECTORY  = 4  /* Expected a directory but found a file instead. */
} fat_status_t;

/* Structure representing a file in a FAT32 filesystem. */
typedef struct {
	uint32_t position;       /* Current seek position (in bytes), the next byte to read */
	uint32_t cluster;        /* The current cluster we’re seeked inside of. */
	uint32_t cluster_offset; /* The offset (in bytes) in the cluster that corresponds to the current seek position */
	uint32_t size;           /* Size of file in bytes */
} fat_file_t;

fat_status_t fat_init(fat_ops_t ops);
fat_status_t fat_open(fat_file_t *file, const char *path);
void dump_root_dir(void);

#endif /* INCLUDE_FAT_DRIVER */