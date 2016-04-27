#ifndef _ROOT_DIR_H_
#define _ROOT_DIR_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
	uint8_t		filename[8];	///< Name of the file; 0th byte meaning:
								///< 	0x00: Unused
								///< 	0xe5: Deleted
								///< 	0x05: Actually starts with 0xe5
								///< 	0x2e: Directory
	uint8_t 	ext[3];	///< File extension; no special characters
	uint8_t 	attributes;		///< File attributes
	uint8_t 	reserved[10];	///< Special meaning - implementation-dependent
	uint16_t 	modify_time;	///< Time created/last modified; format:
								///<   	hhhhhmmmmmmsssss (seconds/2)
	uint16_t 	modify_date;	///< Data created/last modified; format:
								///<   	yyyyyyymmmmddddd (year 0 = 1980)
	uint16_t 	start;			///< Start of file in clusters
	uint32_t 	size;			///< File size in bytes

	uint16_t    parent_dir;		///< points to starting cluster of parent dir ".."
	uint16_t* 	clusters;		///< buffer of all the clusters in the file
								///< cluster val:
								///<	0x0000: 		free
								///<	0x0002-ffef: 	cluster in use; 
								///<					the value given is the number of the next cluster in the file
								///<	0xfff0-fff6: 	reserved
								///<	0xfff7: 		bad cluster
								///<	0xfff8-ffff: 	cluster in use; 
								///<					EOC end of chain which is last cluster in file
} __attribute__((packed)) DirEntry;

/**
 * @brief Print file metadata
 * 
 * @param[in] entry The directory entry (file) whose info is to be printed
 */
void print_file_info(DirEntry *entry);

/**
 * @brief Read the root directory
 * 
 * @param[in] Pointer to FAT file system image
 * @param[in,out] entries Pointer to array of directory entries
 * 
 * @returns On return all root directory entries are in entries
 */
void parse_root_dir(int fd, DirEntry *entries);

#endif