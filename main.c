#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "root_dir.h"

// #define IMAGE_FILE "fat.img"
#define IMAGE_FILE "fat_tutorial1/test.img"

#define PARTITION_TABLE_OFFSET 0x1BE

// typedef struct {

// } __attribute((packed)) BIOSParameterBlock;

/**
 * @brief An entry in the Master Boot Record (MBR) partition table
 */
typedef struct {
	uint8_t bootable;		///< 0x80 if bootable, 0 else
	uint8_t start_chs[3];	///< start of partion in CHS-addressing
	uint8_t type;			///< type of the partition
	uint8_t end_chs[3];		///< end of partition in CHS-addressing
	uint32_t start_sector;	///< relative offset to partition in sectors (LBA)
	uint32_t size_sectors;	///< size of partition in sectors
} __attribute__((packed)) Partition;

typedef struct {
	uint8_t 	jump[3];					///< Intel 80x86 jump instruction
	uint8_t		oem_name[8];				///< OEM name
	uint16_t 	sector_size;				///< Sector size in Bytes
	uint8_t 	sectors_per_cluster;		///< Number of sectors per cluster
	uint16_t	reserved_sectors;			///< Reserved sectors
	uint8_t 	num_FATs;					///< Number of FATs
	uint16_t	num_root_entries;			///< Number entries in root dir
	uint16_t 	total_sectors_short;		///< Total # of sectors; if 0 see  
											///<   later field
	uint8_t 	media_descriptor;			///< Media descriptor
	uint16_t 	sectors_per_FAT;			///< Number of sectors per FAT
	uint16_t 	sectors_per_cylinder;		///< Number of sectors per track 
											///<   (cylinder), CHS addressing
	uint16_t 	num_heads;					///< Number of heads, CHS addressing
	uint32_t 	num_hidden_sectors;			///< Number of hidden sectors (before 
											///<   boot sector)
	uint32_t 	total_sectors_long;			///< Total # of sectors; only valid if 
											///<   total_sectors_short is 0

	uint8_t 	drive_number;				///< Drive number
	uint8_t 	current_head;				///< Current head
	uint8_t 	boot_signature;				///< Boot signature; 0x29 means
											///<   next three fields valid
	uint32_t 	volume_id;					///< Volume ID (serial number)
	uint8_t		volume_label[11];			///< Volume label
	uint8_t		fs_type[8];					///< File system type
	uint8_t		boot_code[448];				///< Boot code
	uint8_t		boot_sector_signature[2];	///< Must be 0x55AA
} __attribute__((packed)) FAT16BootSector;

int main(int argc, char *argv[]) {
	int debug = 1;

	int i, retVal;
	Partition partitionTable[4];
	FAT16BootSector bootSector;
	DirEntry entry;
	int boot_offset;

	/* Open the FAT disk image file */
	FILE *fff = fopen(IMAGE_FILE, "rb");
	if (fff == NULL) {
		printf("Error opening file\n");
		fclose(fff);
		return errno;
	}
	if (debug) printf("Opened %s\n",IMAGE_FILE);

	/* Seek to the start of the partition table in the MBR */
	retVal = fseek(fff, PARTITION_TABLE_OFFSET, SEEK_SET);
	if (retVal < 0) {
		printf("Error seeking to partition table\n");
		fclose(fff);
		return errno;
	}

	if (debug) printf("Seeked to partition table\n");

	/* Read the partition table */
	fread(partitionTable, sizeof(Partition), 4, fff);
	if (ferror(fff) != 0) {
		printf("Error reading partitions\n");
		fclose(fff);
		return errno;
	}

	if (debug) printf("Read partition tables\n");

	/* Print partition info and check if there is a valid FAT16 partition */
	for (i = 0; i < 4; i++) {
		printf("Partition %d, type %02X\n", i, partitionTable[i].type);
		printf("  Start sector %08X, %d sectors long\n", 
			partitionTable[i].start_sector, partitionTable[i].size_sectors);

		/* Check for valid FAT16 partition type */
		if (partitionTable[i].type == 4 || partitionTable[i].type == 6 
			|| partitionTable[i].type == 14) {
			printf("FAT16 filesystem found on partition %d\n", i);
			break;
		}
	}

	/* If no FAT16 partition listed in partition table, assume we should start 
	   at sector 0. Otherwise, start at first FAT16 partition */
	if (i == 4) {
		printf("No FAT16 partition found. Assuming start at sector 0\n");
		boot_offset = 0;
	} else {
		boot_offset = 512*partitionTable[i].start_sector;
	}

	/* Seek to MBR */
	retVal = fseek(fff, boot_offset, SEEK_SET);
	if (retVal < 0) {
		printf("Error seeking to beginning\n");
		fclose(fff);
		return errno;
	}

	if (debug) printf("Successfully seeked to beginning of MBR\n");

	/* Read the boot sector */
	fread(&bootSector, sizeof(FAT16BootSector), 1, fff);
	if (ferror(fff) != 0) {
		printf("Error reading boot sector\n");
		fclose(fff);
		return errno;
	}

	if (debug) printf("Successfully read boot sector\n");

	// seek to FAT
    // not quite sure how this offset works...I think it's magic
    // fat_start = ftell(fff) + (bootSector.reserved_sectors-1) * bootSector.sector_size;
    // root_start = fat_start + bootSector.sectors_per_FAT * bootSector.num_FATs * bootSector.sector_size;
    // data_start = root_start + bootSector.num_root_entries * sizeof(DirEntry);
    // for (i = 0; i < bootSector.num_FATs; i++) {


    // }

	printf("  Jump code: %02X:%02X:%02X\n", bootSector.jump[0], bootSector.jump[1], bootSector.jump[2]);
    printf("  OEM code: [%.8s]\n", bootSector.oem_name);
    printf("  sector_size: %d\n", bootSector.sector_size);
    printf("  sectors_per_cluster: %d\n", bootSector.sectors_per_cluster);
    printf("  reserved_sectors: %d\n", bootSector.reserved_sectors);
    printf("  num_FATs: %d\n", bootSector.num_FATs);
    printf("  num_root_entries: %d\n", bootSector.num_root_entries);
    printf("  total_sectors_short: %d\n", bootSector.total_sectors_short);
    printf("  media_descriptor: 0x%02X\n", bootSector.media_descriptor);
    printf("  sectors_per_FAT: %d\n", bootSector.sectors_per_FAT);
    printf("  sectors_per_cylinder: %d\n", bootSector.sectors_per_cylinder);
    printf("  num_heads: %d\n", bootSector.num_heads);
    printf("  num_hidden_sectors: %d\n", bootSector.num_hidden_sectors);
    printf("  total_sectors_long: %d\n", bootSector.total_sectors_long);
    printf("  drive_number: 0x%02X\n", bootSector.drive_number);
    printf("  current_head: 0x%02X\n", bootSector.current_head);
    printf("  boot_signature: 0x%02X\n", bootSector.boot_signature);
    printf("  volume_id: 0x%08X\n", bootSector.volume_id);
    printf("  Volume label: [%.11s]\n", bootSector.volume_label);
    printf("  Filesystem type: [%.8s]\n", bootSector.fs_type);
    printf("  Boot sector signature: 0x%.2X%.2X\n", bootSector.boot_sector_signature[0], bootSector.boot_sector_signature[1]);

    if (debug) printf("Now at 0x%X, sector size %d, FAT size %d sectors, %d FATs\n\n", ftell(fff), bootSector.sector_size, bootSector.sectors_per_FAT, bootSector.num_FATs);

    retVal = fseek(fff, (bootSector.reserved_sectors-1 + 
    	bootSector.sectors_per_FAT * bootSector.num_FATs) * 
    	bootSector.sector_size, SEEK_CUR);
    if (retVal < 0) {
		printf("Error seeking to rootdir\n");
		fclose(fff);
		return errno;
	}

	if (debug) printf("Successfully seeked to rootdir\n");

    for (i = 0; i < bootSector.num_root_entries; i++) {
    	retVal = fread(&entry, sizeof(DirEntry), 1, fff);
    	if (ferror(fff) != 0) {
    		printf("Error reading rootdir entry #%d\n",i);
			fclose(fff);
    		return errno;
    	}
    	print_file_info(&entry);
    }


	/* Close the FAT disk image file */
	fclose(fff);

	return 0;
}