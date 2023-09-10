#define PCRE2_CODE_UNIT_WIDTH 8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <pcre2.h>

#include <datatypes.h>
#include <pcre_capture.h>
#include <stringfuncs.h>

/******************************************************************************
* Function:         void captured_data_free
* Description:      Frees struct captured_data
* Where:
*                   struct captured_data *cat - Pointer to struct to free
* Return:           None
* Error:            None
*****************************************************************************/
void captured_data_free(struct captured_data *cat)
{
	int i;

	for(i = 0; i < (int) cat->size; i++) {
		free(cat->substrings[i]);
	} free(cat->substrings);
    free(cat->ovector);

	free(cat);
}

/******************************************************************************
* Function:         void rename_data_free
* Description:      Free struct rename_data
* Where:
*                   struct rename_data *rat - Pointer to struct to be freed
*                   size_t size - Substrings size, usually captured_data->size-1
* Return:           None
* Error:            None
*****************************************************************************/
void rename_data_free(struct rename_data *rat, size_t size)
{
    free(rat->substrings);
    free(rat->padding);
    free(rat->locs);

    free(rat);
}

/******************************************************************************
* Function:         struct rename_data *rename_data_alloc
* Description:      Allocates memory for struct rename_data, giving NULL
*                   values to individual substrings since they will be 
*                   allocated later.
* Where:
*                   size_t size - No. of substrings, usually cat->size-1
* Return:           struct rename_data
* Error:            Not expectin malloc to fail yet
*****************************************************************************/
struct rename_data *rename_data_alloc(size_t size)
{
    int i;
    struct rename_data *rat = malloc(sizeof(struct rename_data));

    rat->padding = calloc(size, sizeof(int));
    rat->locs = calloc(size, sizeof(size_t));

    rat->substrings = calloc(size, sizeof(char *));
    for(i = 0; i < (int) size; i++) {
        rat->substrings[i] = NULL;
    }

    return rat;
}

int main(int argc, char *argv[])
{
	DIR *workdir;
	struct dirent *direntry;

	if(argc == 4) {
		workdir = opendir(argv[3]);
	} else {
		printf("USAGE: ./renameutil <regex> <substitution> <path>\n");
		return 1;
	}

	if(workdir == NULL) {
		printf("Failed to open directory.\n");
		return 1;
	}

	// We get compiled regex here
	pcre2_code *regex = getregex(argv[1]);
	if(regex == NULL) {
		return 1;
	}

	while((direntry = readdir(workdir)) != NULL) {
		// We use compiled regex to get values from dirnames
		struct captured_data *cat = NULL;
		cat = getdata(regex, direntry->d_name);
        if(cat == NULL) {
            continue;
        }

        struct rename_data *rat = rename_data_alloc(cat->size - 1);

        get_rename_data(cat, rat, argv[2]);

        // '(\d+)(.*)' '$2 $1' tests/
        rename_data_free(rat, cat->size - 1);
        captured_data_free(cat);
/*
		char *str = NULL;

		if(str == NULL) {
			captured_data_free(cat);
			continue;
		}

		char *oldpath = join_path(argv[1], direntry->d_name);
		char *newpath = join_path(argv[1], str);

		// printf("Old path: %s\n", oldpath);
		// printf("New path: %s\n\n", newpath);
		
		rename(oldpath, newpath);

		captured_data_free(cat); 
		free(oldpath);
		free(newpath);
		free(str);
*/
	}

    closedir(workdir);
	pcre2_code_free(regex);
	return 0;
}
