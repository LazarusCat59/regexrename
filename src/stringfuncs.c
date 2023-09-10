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
 *                   char path1[] - Beginning of path
 *                   char path2[] - Ending of path
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
 * Function:         int check_nums
 * Description:      Gets value after the $ and checks if there is padding
 *                   value after it. If there is, padding value is stored 
 *                   to appropriate location in struct rename_data
 * Where:
 *                   struct rename_data *rat - for storing padding data,
 *                       should already be allocated, except for substrings
 *                   char *str - pointer to character after "$"
 *                   int len - length until end of str from pointer char
 * Return:           Integer value after $ if the pattern is valid
 * Error:            Negative values on error
 *****************************************************************************/
int check_nums(struct rename_data *rat, char *str, int len)
{
    int i, retval = -1;
    char temp[2];

    for (i = 0; i < len; i++) {
        if(i == 2 && isdigit((unsigned char) str[i])) {
            retval = -2;
            break;
        }

        if(isdigit((unsigned char) str[i])) {
            temp[i] = str[i];
        } else if (str[i] == '{') {
            if(str[i+2] == '}') {
                temp[i] = '\0';
                retval = atoi(temp);
                if(isdigit((unsigned char) str[i+1]))
                    rat->padding[retval-1] = str[i+1] - '0';
                printf("padding = %d\n", rat->padding[retval]);
            } else {
                retval = -1;
            } break;
        } else {
            temp[i] = '\0';
            retval = atoi(temp);
            break;
        }
    } 

    return retval;
}

/******************************************************************************
 * Function:         int get_rename_data
 * Description:      Parses the substitute string and stores values to be 
 *                   substituted in struct rename_data, with padding if it's
 *                   an integer value. If it's not an integer value, padding
 *                   will be -1
 * Where:
 *                   struct captured_data *cat - To get substrings to store
 *                       in struct rename_data
 *                   struct rename_data *rat - Stores substrings in this
 *                       struct, initializes padding values
 *                   char *str - Substitution pattern (eg. $2 $1{3})
 * Return:           0 if there were no errors 
 * Error:            -1 if there was an error
 *****************************************************************************/
int get_rename_data(struct captured_data *cat, struct rename_data *rat, char *str)
{
    int i, rc, len = strlen(str);
    char *temp;

    for(i = 0; i < len; i++) {
        if(str[i] == '$') {
            temp = NULL;
            rc = check_nums(rat, str+i+1, len-i);
            if(rc == -1) {
                printf("No closing braces or padding value > 9.\n");
                return 1;
            } else if(rc == -2) {
                printf("Cannot have more than 99 capture groups.\n");
                return 1;
            } else if(rc >= (int) cat->size) {
                printf("Variable num higher than no. of captures\n");
                return 1;
            } else {
                rc--;
                rat->substrings[rc] = cat->substrings[rc+1];
                rat->locs[rc] = i;
                (void)strtol(rat->substrings[rc], &temp, 10);
                if(temp[0] != '\0' && rat->padding[rc] > 0) {
                    printf("Padding not allowed for strings, no padding will be applied.\n");
                    rat->padding[rc] = -1;
                } else if(temp[0] == '\0' && rat->padding[rc] == -1) {
                    rat->padding[rc] = 0;
                }
                printf("%d: %s, padding: %d\n", rc+1, rat->substrings[rc], rat->padding[rc]);
            }
        }
    }

    return 0;
}
