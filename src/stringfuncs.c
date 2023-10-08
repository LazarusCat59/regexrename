#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include <datatypes.h>
#include <stringfuncs.h>


/******************************************************************************
 * Function:         char *join_path
 * Description:      Concatenates 2 paths, adding the seperator '/' if
 *                   necessary
 * Where:
 *                   char path1[] - First half of path
 *                   char path2[] - Second half of path
 * Return:           Pointer to concatenated path, has to be freed
 * Error:            Not expectin this to fail rn
 *****************************************************************************/
char *join_path(char path1[], char path2[])
{
    char *newpath = calloc(PATH_MAX, sizeof(char));
    strcpy(newpath, path1);

    if(path1[strlen(path1)-1] != '/') {
        strcat(newpath, "/");
    } strcat(newpath, path2);

    return newpath;
}

/******************************************************************************
 * Function:         int set_rename_data
 * Description:      Gets value after the $ and checks if there is padding
 *                   value after it. If there is, padding value is stored 
 *                   to appropriate location in struct rename_data and
 *                   value after $ will be useed to store substring
 * Where:
 *                   struct rename_data *rat - for storing padding data,
 *                       should already be allocated, except for substrings
 *                   char *str - pointer to character after "$"
 *                   int len - length until end of str from pointer char
 * Return:           Integer value after $ if the pattern is valid
 * Error:            Negative values on error
 *****************************************************************************/
int set_rename_data(struct captured_data *cat, struct rename_data *rat, char **str)
{
    int i = 0, val = -1;
    char *temp;

    // printf("str = %s\n", *str);
    val = (int) strtol(*str, &temp, 10);

    if(temp[0] == '{' && val > 0) {
        *str = temp+1;
        i = (int) strtol(*str, &temp, 10);
        if(temp[0] != '}') {
            return -1;
        } else {
            rat->padding[rat->size] = i;
            temp++;
        }
    }

    if(val > (int) cat->size) {
        return -2;
    }

    if(val == 0) {
        if(*(temp-1) == '0')
            return -4;
        else
            return -3;
    }

    // Since the loop in set_refstr increments str at start, we need this
    temp--;

    *str = temp;
    rat->substrings[rat->size] = cat->substrings[val];

    return 0;
}

/******************************************************************************
 * Function:         int set_refstr
 * Description:      Parses the substitute string and calls set_rename_data
 *                   to store values to be substituted in struct
 *                   rename_data, with padding if it's an integer value.
 *                   If it's not an integer value, padding will be -1
 * Where:
 *                   struct captured_data *cat - To get substrings to store
 *                       in struct rename_data
 *                   struct rename_data *rat - Stores substrings in this
 *                       struct, initializes padding values
 *                   char *str - Substitution pattern (eg. $2 $1{3})
 * Return:           0 if there were no errors 
 * Error:            -1 if there was an error
 *****************************************************************************/
int set_refstr(struct captured_data *cat, struct rename_data *rat, char *str)
{
    int j = 0, rc;
    char *temp, refstr[256], *ptr;

    if(strlen(str) > 255) {
        printf("String should not be more than 255 chars\n");
        return 1;
    }

    for(ptr = str; *ptr != '\0'; ptr++) {
        refstr[j++] = *ptr;
        if(*ptr == '\\') {
            if(*(ptr+1) == '$') {
                refstr[j++] = '\\';
                refstr[j++] = '$';
            } else if(*(ptr+1) == '\\') {
                refstr[j++] = '\\';
                refstr[j++] = '\\';
            } else {
                return 1;
            }
        }

        if(*ptr == '$') {
            temp = NULL;

            // To start doin shiz at value after $ instead of $
            ptr++;

            rc = set_rename_data(cat, rat, &ptr);
            // printf("rc = %d\n", rc);

            if(rc == -1) {
                printf("No closing braces.\n");
                return 1;
            } else if(rc == -2) {
                printf("Variable num higher than no. of captures\n");
                return 1;
            } else if (rc == -3) {
                printf("Unescaped '$' in string\n");
                return 1;
            } else if(rc == -4) {
                printf("Zero given as padding\n");
            } else {
                (void) strtol(rat->substrings[rat->size], &temp, 10);

                if(temp[0] != '\0' && rat->padding[rat->size] > 0) {
                    printf("Padding not allowed for strings.\n");
                    return 1;
                }

                // printf("$%d = %s\n", (int) rat->size+1, rat->substrings[rat->size]);
                rat->size++;
            }
        }
    } refstr[j] = '\0';

    rat->refstr = strndup(refstr, 256);

    return 0;
}

/******************************************************************************
 * Function:         char *construct_string
 * Description:      Constructs a string to replace the current name of files
 *                   files or folders, according to string provided
 * Where:
 *                   struct rename_data *rat - has data needed to construct
 *                   string
 * Return:           String to use for renaming
 * Error:            NULL
 *****************************************************************************/
char *construct_string(struct rename_data *rat)
{
    int i, j = 0, k = 0, l = 0, len = strlen(rat->refstr), templen;
    char *new_str = calloc(256, sizeof(char));
    char *temp = NULL;

    for (i = 0; i < len; ++i) {
        if(k >= 255) {
            free(new_str);
            return NULL;
        }

        temp = NULL;

        // handle \$ and escaped backslash
        if(rat->refstr[i] == '\\') {
            if(rat->refstr[i+1] == '\\') {
                new_str[k++] = '\\';
            } else if(rat->refstr[i+1] == '$') {
                new_str[k++] = '$';
            } i++; continue;
        }

        // Inputting word or padded digits
        else if(rat->refstr[i] == '$') {
            if(rat->padding[j] < 0) {
                templen = strlen(rat->substrings[j]);
                if((k + templen) < 256) {
                    for(l = 0; l < templen; l++) {
                        new_str[k++] = rat->substrings[j][l];
                    } j++;
                } else {
                    free(new_str);
                    return NULL;
                }
            } else {
                asprintf(&temp, "%%0%dd", rat->padding[j]);
                asprintf(&temp, temp, strtol(rat->substrings[j++], NULL, 10));

                templen = strlen(temp);
                if((k + templen) < 256) {
                    for(l = 0; l < templen; l++) {
                        new_str[k++] = temp[l];
                    }
                } else {
                    free(new_str);
                    free(temp);
                    return NULL;
                }
            }
        } else {
            new_str[k++] = rat->refstr[i];
        }
    }

    return new_str;
}
