/**
 * 
 */



typedef struct {

	uint8_t FAT_ID;			// 8 bits in the first cluster (0) and is also copied into the BPB of the boot sector
	short mount_flag;		// bit 15 if not set then was not properly unmounted before shutdown and could be dirty. should scan disk
	short disk_flag;		// bit 14 if not set then disk I/O error on startup, possibly bad sectors. should run scan	
	FILE_T * files;			// buffer of all the files in the FAT

} FAT_T;



typedef struct {

	short * filename;		// name of the file
	uint16_t parent_dir;	// points to starting cluster of parent dir ".."
	CLUSTER_T * clusters;	// buffer of all the clusters in the file
	uint16_t last_cluster;	// EOC end of chain which is last cluster in file

} FILE_T;



typedef struct {

	uint16_t next_cluster;	// points to next cluster

} CLUSTER_T;


/**
 * @brief []
 * @details [long description]
 * 
 * @param address [description]
 * @return [description]
 */
FAT_T * FAT(uint16_t address);


/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param address [description]
 * @return [description]
 */
FILE_T * FILE(uint16_t address);


/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param address [description]
 * @return [description]
 */
CLUSTER_T * CLUSTER(uint16_t address);


