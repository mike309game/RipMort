#ifndef _PACK_FILE_H
#define _PACK_FILE_H
#include <errno.h>
#include <string.h>

/* Debug macros */
#ifdef _PF_DEBUG
#define PF_DBG(string, ...) printf(string, __VA_ARGS__)
#else
#define PF_DBG(string, ...)
#endif
#define PF_ERR(string, ...) fprintf(stderr, string, __VA_ARGS__, strerror( errno ));

enum pack_file_kind {
	PF_TEX = 0x31,
	PF_PMD = 0x32,
};

typedef struct pf_file_t {
	char name[0x10];
	long data_length;
	long combined_size_since_last_file;
	char* data;
} pf_file_t;

typedef struct pointer_file_t {
	/* Look at file kind enum */
	short type;
	short file_count;
	/* Despite the file having the address to the first data block,
		the game chose to use this and adds 0x14 to it and ANDs it by (~3)*/
	long first_data_address_minus_0x14;
	/* Identification? */
	long unknown_always_14;
	/* File size - this = address of first file's data */
	long filesize_without_header;
	/* Not used by game, for whatever reason */
	long first_data_address;
	pf_file_t* files;
} pack_file_t;

pack_file_t load_pack_file(const char filename[]);
void free_pack_file(pack_file_t* pack_file);

#endif