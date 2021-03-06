/**
 * 
 */


/**
 * @brief [This structure represents the entire File Allocation Table]
 */
typedef struct {
	uint8_t FAT_ID;			// 8 bits in the first cluster (0) and is also copied into the BPB of the boot sector
	uint16_t mount_flag;	// bit 15 if not set then was not properly unmounted before shutdown and could be dirty. should scan disk
	uint16_t disk_flag;		// bit 14 if not set then disk I/O error on startup, possibly bad sectors. should run scan	
	uint16_t * files;		// buffer containing start cluster of every file in the FAT
} FAT_T;


/**
 * @brief []
 * @details [long description]
 * 
 * @param address [description]
 * @return [description]
 */
int init_FAT(FAT_T * FATable);
