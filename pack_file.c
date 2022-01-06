#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "pack_file.h"

pack_file_t load_pack_file(const char *filename) {
	pack_file_t pack_file;

	FILE* fp = fopen(filename, "rb");

	if(!fp) {
		PF_ERR("Failed opening %s: %s\n", filename);
		return pack_file;
	}

	PF_DBG("Processing %s\n", filename);

	/* Read pack file's header */
	fread(&pack_file, 0x14, 1, fp);

	PF_DBG("Files: %d\n", pack_file.file_count);

	/* Allocate pf struct of files */
	pack_file.files = (pf_file_t*) malloc(sizeof(pf_file_t) * pack_file.file_count);

	size_t data_address_current = pack_file.first_data_address;

	/* For backtracking when loading file data */
	size_t old_pos;

	for (int j = 0; j < pack_file.file_count; ++j) {
		/* Read file's header */
		fread(&pack_file.files[j], 0x18, 1, fp);
		PF_DBG("\nFilename: %s\nData length: %li\nCombined size since last file: %li\n",
			pack_file.files[j].name,
			pack_file.files[j].data_length,
			pack_file.files[j].combined_size_since_last_file
		);

		/* Load file's data */

		/* Set old position */
		old_pos = ftell(fp);

		/* Go to data address */
		fseek(fp, data_address_current, SEEK_SET);

		/* Allocate data buffer */
		pack_file.files[j].data = (char*) malloc(pack_file.files[j].data_length);

		/* Read content into buffer */
		fread(pack_file.files[j].data, pack_file.files[j].data_length, 1, fp);

		/* Advance data address */
		data_address_current += pack_file.files[j].data_length;

		/* Go back to old position */
		fseek(fp, old_pos, SEEK_SET);
	}
	/* Close file */
	fclose(fp);

	PF_DBG("Loaded pack file %s\n", filename);

	/* Return the pack file struct pointer */
	return pack_file;
}

void free_pack_file(pack_file_t pack_file) {
	for (int i = 0; i < pack_file.file_count; ++i) {
		free(pack_file.files[i].data);
	}
	free(pack_file.files);
}