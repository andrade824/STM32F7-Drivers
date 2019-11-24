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
	SdStatus (*read_sectors)(void *data, uint32_t sec_addr, uint16_t num_sectors);
	SdStatus (*write_sectors)(void *data, uint32_t sec_addr, uint16_t num_sectors);
} FatOperations;

/* FAT file status flags */
typedef enum {
	FAT_FAIL           = 0,
	FAT_SUCCESS        = 1,
	FAT_FILE_NOT_FOUND = 2, /* The wanted file or directory was not found. */
	FAT_IS_DIRECTORY   = 3, /* Wanted a file but a directory was found instead. */
	FAT_NOT_DIRECTORY  = 4  /* Expected a directory but found a file instead. */
} FatStatus;

/* The mode to open a file in. Only FAT_READ_MODE is supported right now. */
typedef enum {
	FAT_READ_MODE,  /* Read-only mode. */
	FAT_WRITE_MODE, /* Write-only mode. */
	FAT_APPEND_MODE /* Similar to write-only mode except the position is EOF by default. */
} FatOpenMode;

/* Structure representing a file in a FAT32 filesystem. */
typedef struct {
	/* What mode the file was opened in. */
	FatOpenMode mode;

	/* Current seek position (in bytes), the next byte to read. */
	uint32_t position;

	/* The current cluster we’re seeked inside of. */
	uint32_t cluster;

	/* The offset (in bytes) in the cluster that corresponds to the current seek position. */
	uint32_t cluster_offset;

	/* Size of file in bytes. */
	uint32_t size;
} FatFile;

FatStatus fat_init(FatOperations ops);
FatStatus fat_open(FatFile *file, const char *path, FatOpenMode mode);
uint32_t fat_read(FatFile *file, void *buf, uint32_t size);

/**
 * I'm disabling this code but leaving it in the codebase since it can be useful
 * for debugging purposes.
 */
#if 0
void fat_dump_root_dir(void);
#endif

#endif /* INCLUDE_FAT_DRIVER */