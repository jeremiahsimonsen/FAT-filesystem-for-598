#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define IMAGE_FILE "fat.img"

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
} __attribute((packed)) Partition;

int main(int argc, char *argv[]) {
	int i, retVal;
	Partition partitionTable[4];

	/* Open the FAT disk image file */
	int fd = open(IMAGE_FILE, O_RDONLY);
	if (fd < 0) {
		perror("Error opening file");
		return errno;
	}

	retVal = lseek(fd, PARTITION_TABLE_OFFSET, SEEK_SET);
	if (retVal < 0) {
		perror("Error seeking to partition table");
		return errno;
	}

	retVal = read(fd, partitionTable, 4*sizeof(Partition));
	if (retVal < 0) {
		perror("Error reading partitions");
		return errno;
	}

	for (i = 0; i < 4; i++) {
		printf("Partition %d, type %02X\n", i, partitionTable[i].type);
		printf("  Start sector %08X, %d sectors long\n", 
			partitionTable[i].start_sector, partitionTable[i].size_sectors);
	}

	/* Close the FAT disk image file */
	retVal = close(fd);
	if (retVal < 0) {
		perror("Error closing file");
		return errno;
	}

	return 0;
}