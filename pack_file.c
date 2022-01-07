#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
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
	fread(&pack_file, PACK_FILE_HEADER_SIZE, 1, fp);

	PF_DBG("Items: %d\n", pack_file.item_count);

	/* Allocate memory for array of items */
	pack_file.items = (pf_item_t*) malloc(sizeof(pf_item_t) * pack_file.item_count);

	/* Location of first item's data */
	long data_address_current = pack_file.first_data_address_minus_0x14 + 0x14 & (~3);

	/* For backtracking when loading item data */
	long old_pos;

	for (int j = 0; j < pack_file.item_count; ++j) {
		/* Read item's header */
		fread(&pack_file.items[j], PF_ITEM_HEADER_SIZE, 1, fp);
		PF_DBG("\nFilename: %s\nData length: %li\nCombined size since last item: %li\n",
			pack_file.items[j].name,
			pack_file.items[j].data_length,
			pack_file.items[j].combined_size_since_last_file
		);

		/* Load item's data */

		/* Set old position */
		old_pos = ftell(fp);

		/* Go to data address */
		fseek(fp, data_address_current, SEEK_SET);

		/* Allocate data buffer */
		pack_file.items[j].data = (char*) malloc(pack_file.items[j].data_length);

		/* Read content into buffer */
		fread(pack_file.items[j].data, pack_file.items[j].data_length, 1, fp);

		/* Advance data address and pad on 4 because the game does that */
		data_address_current = data_address_current + (pack_file.items[j].data_length & (~3));

		/* Go back to old position */
		fseek(fp, old_pos, SEEK_SET);
	}
	/* Close file */
	fclose(fp);

	PF_DBG("Loaded pack file %s\n", filename);

	/* Return the pack file struct */
	return pack_file;
}

void free_pack_file(pack_file_t* pack_file) {
	for (int i = 0; i < pack_file->item_count; ++i) {
		free(pack_file->items[i].data);
	}
	free(pack_file->items);
}