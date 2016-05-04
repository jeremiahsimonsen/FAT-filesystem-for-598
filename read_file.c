#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "root_dir.h"

// #define IMAGE_FILE "fat.img"
#define IMAGE_FILE "fat_tutorial1/test.img"

#define PARTITION_TABLE_OFFSET 0x1BE

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

void fat_read_file(FILE * in, FILE * out,
                   unsigned long fat_start, 
                   unsigned long data_start, 
                   unsigned long cluster_size, 
                   unsigned short cluster, 
                   unsigned long file_size) {
    unsigned char buffer[4096];
    size_t bytes_read, bytes_to_read,
           file_left = file_size, cluster_left = cluster_size;

    // Go to first data cluster
    fseek(in, data_start + cluster_size * (cluster-2), SEEK_SET);
    
    // Read until we run out of file or clusters
    while(file_left > 0 && cluster != 0xFFFF) {
        bytes_to_read = sizeof(buffer);
        
        // don't read past the file or cluster end
        if(bytes_to_read > file_left)
            bytes_to_read = file_left;
        if(bytes_to_read > cluster_left)
            bytes_to_read = cluster_left;
        
        // read data from cluster, write to file
        bytes_read = fread(buffer, 1, bytes_to_read, in);
        fwrite(buffer, 1, bytes_read, out);
        printf("Copied %d bytes\n", bytes_read);
        
        // decrease byte counters for current cluster and whole file
        cluster_left -= bytes_read;
        file_left -= bytes_read;
        
        // if we have read the whole cluster, read next cluster # from FAT
        if(cluster_left == 0) {
            fseek(in, fat_start + cluster*2, SEEK_SET);
            fread(&cluster, 2, 1, in);
            
            printf("End of cluster reached, next cluster %d\n", cluster);
            
            fseek(in, data_start + cluster_size * (cluster-2), SEEK_SET);
            cluster_left = cluster_size; // reset cluster byte counter
        }
    }
}

int main(int argc, char *argv[]) {
	int i,j, retVal;
	Partition partitionTable[4];
	FAT16BootSector bootSector;
	DirEntry entry;
	int boot_offset;

	uint32_t fat_start, root_start, data_start;

	/* START copy from fat_tutorial2/read_file.c */
	FILE *fff, *out;
	char filename[9] = "        ", file_ext[4] = "   "; // initially pad with spaces

	if (argc < 3) {
		printf("Usage: read_file <fs_image> <FILE.EXT>\n");
		return 0;
	}

	if((fff = fopen(argv[1],"rb")) == NULL) {
		printf("Filesystem image file %s not found!\n", argv[1]);
		return -1;
	}

	// Copy filename and extension to space-padded search strings
	for(i=0; i<8 && argv[2][i] != '.' && argv[2][i] != 0; i++)
		filename[i] = argv[2][i];
	for(j=1; j<=3 && argv[2][i+j] != 0; j++)
		file_ext[j-1] = argv[2][i+j];
		
	printf("Opened %s, looking for [%s.%s]\n", argv[1], filename, file_ext);
	/* END copy from fat_tutorial2/read_file.c */

	/* Open the FAT disk image file */
	// FILE *fff = fopen(IMAGE_FILE, "rb");
	// if (fff == NULL) {
	// 	printf("Error opening file\n");
	// 	fclose(fff);
	// 	return -1;
	// }

	/* Seek to the start of the partition table in the MBR */
	retVal = fseek(fff, PARTITION_TABLE_OFFSET, SEEK_SET);
	if (retVal < 0) {
		printf("Error seeking to partition table\n");
		fclose(fff);
		return -1;
	}

	/* Read the partition table */
	fread(partitionTable, sizeof(Partition), 4, fff);
	if (ferror(fff) != 0) {
		printf("Error reading partitions\n");
		fclose(fff);
		return -1;
	}

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
		return -1;
	}

	/* Read the boot sector */
	fread(&bootSector, sizeof(FAT16BootSector), 1, fff);
	if (ferror(fff) != 0) {
		printf("Error reading boot sector\n");
		fclose(fff);
		return -1;
	}

	/* Calculate start locations of FAT, root dir, and data */
	fat_start = ftell(fff) + (bootSector.reserved_sectors-1) * bootSector.sector_size;
	root_start = fat_start + bootSector.sectors_per_FAT * bootSector.num_FATs * bootSector.sector_size;
	data_start = root_start + bootSector.num_root_entries * sizeof(DirEntry);

	/* Seek to the beginning of the root directory */
    retVal = fseek(fff, root_start, SEEK_SET);
    if (retVal < 0) {
		printf("Error seeking to rootdir\n");
		fclose(fff);
		return -1;
	}

	/* Dump info for each root dir entry */
    for (i = 0; i < bootSector.num_root_entries; i++) {
    	retVal = fread(&entry, sizeof(DirEntry), 1, fff);
    	if (ferror(fff) != 0) {
    		printf("Error reading rootdir entry #%d\n",i);
			fclose(fff);
    		return -1;
    	}
    	print_file_info(&entry);
    }

	/* START copy from fat_tutorial2/read_file.c */
    out = fopen(argv[2], "wb");
    fat_read_file(fff, out, fat_start, data_start, 
    	bootSector.sectors_per_cluster*bootSector.sector_size, 
    	entry.start_cluster, entry.size);
    fclose(out);
    fclose(fff);

	/* Close the FAT disk image file */
	// fclose(fff);

	return 0;
}