#include <io.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "pack_file.h"
#include "main.h"
#ifdef _WIN32
	#include <windows.h>
	#include <direct.h>
#endif

pack_file_t* pack_files;

//stackoverflow.com/a/2736841
char *ext_remove(const char* myStr) {
    char *retStr;
    char *lastExt;
    if (myStr == NULL) return NULL;
    if ((retStr = malloc (strlen (myStr) + 1)) == NULL) return NULL;
    strcpy (retStr, myStr);
    lastExt = strrchr (retStr, '.');
    if (lastExt != NULL)
        *lastExt = '\0';
    return retStr;
}

int main(int argc, char const *argv[])
{
	printf("RipMort %s\n",VERSION);

	if(argc < 2) {
		perror("Give me files");
		exit(EXIT_FAILURE);
	}

	/* Allocate pointer file array */
	//pack_files = (pack_file_t*)malloc(sizeof(pack_file_t)*argc-1);

	for (int i = 1; i < argc; ++i) {
		char* argv_no_ext = ext_remove(argv[i]);

		pack_file_t pack_file = load_pack_file(argv[i]);

		mkdir(argv_no_ext);

		/* Random size */
		char path[0xFF];

		for (int j = 0; j < pack_file.file_count; ++j) {
			/* Make file name */
			sprintf(path, "%s/%s", argv_no_ext, pack_file.files[j].name);

			FILE* fp = fopen(path, "wb");
			if(!fp) {
				perror("Error creating extracted file");
				exit(EXIT_FAILURE);
			}
			fwrite(pack_file.files[j].data, pack_file.files[j].data_length, 1, fp);
			fclose(fp);
		}
		free_pack_file(pack_file);
		free(argv_no_ext);
	}

	return 0;
}