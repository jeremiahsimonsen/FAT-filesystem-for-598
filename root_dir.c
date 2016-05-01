#include "root_dir.h"

void print_file_info(DirEntry *entry) {
	switch(entry->filename[0]) {
	case 0x00: 
		// printf("Unused entry\n");
		return;	// unused entry
	case 0xE5:
		printf("Deleted file: ?%.7s.%.3s \n", entry->filename+1, entry->ext);
		return;
	case 0x05:
		printf("File: %c%.7s.%.3s \n", 0xE5, entry->filename+1, entry->ext);
		break;
	case 0x2E:
		printf("Directory: %.8s.%.3s \n", entry->filename, entry->ext);
		break;
	default:
		printf("File: %.8s.%.3s \n", entry->filename, entry->ext);
	}

	printf("  Modified: %04d-%02d-%02d %02d:%02d.%02d  Start: %04X Size: %d\n",
		1980 + (entry->modify_date >> 9),	// year
		(entry->modify_date >> 5) & 0xF,	// month
		(entry->modify_date & 0x1F),		// day
		(entry->modify_time >> 11),			// hours
		(entry->modify_time >> 5) & 0x3F,	// minutes
		(entry->modify_time & 0x1F),		// seconds
		entry->start_cluster, entry->size);
}

// void parse_root_dir(int fd, DirEntry *entries) {
// 	lseek(fd, )
// }