#define PCRE2_CODE_UNIT_WIDTH 8
#include <stdio.h>
#include <pcre2.h>

#include <datatypes.h>
#include <pcre_capture.h>

/******************************************************************************
* Function:         void print_error
* Description:      Error handling function for pcre2, prints error message
*                   Error handling function for pcre2, prints error message *
*                   along with string str for context
* Where:
*                   int errcode - Error code returned by pcre2 functions
*                   int erroffset - Offset given by pcre2_compile (optional,
*                       can give -1 if not present)
*                   const char *str - String to print along with error
*                       message, a regex if pcre2_compile fails or the
*                       subject string if pcre2_match or other functions
*                       fail
* Return:           None
* Error:            None
*****************************************************************************/
void print_error(int errcode, int erroffset, const char *str)
{
	int erc;
	PCRE2_UCHAR buffer[256];

	if(erroffset != -1)
		printf("Compilation failed at offset: %d\n", (int)erroffset);
	erc = pcre2_get_error_message(errcode, buffer, sizeof(buffer));
	if(erc < 0) {
		switch(erc) {
			case PCRE2_ERROR_NOMEMORY:
				printf("Buffer too small, error message truncated.\n");
				printf("ERROR: %s\n", buffer);
				break;
			case PCRE2_ERROR_BADDATA:
				printf("Not a recognized error code.\n");
				break;
		}
	} else {
		printf("ERROR: %s for '%s'\n", buffer, str);
	}
}

/******************************************************************************
* Function:         pcre2_code *getregex
*                   Takes regex string and compiles it using pcre2_compile
* Where:
*                   const char *rstr - Regex to compile
* Return:           Compiled pcre2_code object
* Error:            NULL on failure
*****************************************************************************/
pcre2_code *getregex(const char *rstr)
{
	int errcode;
	PCRE2_SIZE erroffset;

	printf("Regex: %s\n", rstr);

	// Casting to PCRE2_SPTR to avoid warnings
	PCRE2_SPTR regstr = (PCRE2_SPTR) rstr;

	// Compiling regex and storing in reg
	pcre2_code *regex = pcre2_compile(regstr,
			PCRE2_ZERO_TERMINATED, 0, &errcode, &erroffset, NULL);
	// Handling errors
	if(regex == NULL) {
		print_error(errcode, erroffset, rstr);
		return NULL;
	}

	return regex;
}

/******************************************************************************
* Function:         struct captured_data *getdata
* Description:      Uses compiled regex and subject string to check
*                   for matches and store in struct captured_data
* Where:
*                   pcre2_code *regex - Compiled regex
*                   const char *rstr - String to check for matches
* Return:           struct captured_data with ovector and substrings
* Error:            NULL on failure
*****************************************************************************/
struct captured_data *getdata(pcre2_code *regex, const char *str)
{
	int rc, i;

	// String to run regex capture on
	PCRE2_SPTR srcstr = (PCRE2_SPTR) str;

	// For pcre2_match
	pcre2_match_data *matchdata;
	PCRE2_SIZE *ovector;

	// To print no. of capture groups for debugging
	uint32_t captures;

    // For captured substrings
    struct captured_data *cat;

	rc = pcre2_pattern_info(regex, PCRE2_INFO_CAPTURECOUNT, &captures);
	if(rc < 0) {
		print_error(rc, -1, str);
	}
	// printf("No. of capture groups: %d\n", captures);

	captures++; // Need capture groups + 1 since whole string is matched

	matchdata = pcre2_match_data_create_from_pattern(regex, NULL);

	rc = pcre2_match(regex, srcstr, PCRE2_ZERO_TERMINATED, 0, 0, matchdata, NULL);

	if (rc == 0) printf("ovector not big enough for all captured substrings\n");

	if (rc < 0) {
		print_error(rc, -1, str);
		pcre2_match_data_free(matchdata);
		return NULL;
	}

	cat = malloc(sizeof(struct captured_data));
	cat->size = (size_t)captures;
	cat->substrings = calloc(captures, sizeof(char *));
	cat->ovector = calloc(captures*2, sizeof(size_t));

	// ovector has locations to matched substrings
	// index 2*i and 2*i+1 for start and end of substring
	// where i < rc (no. of matches + 1)
	ovector = pcre2_get_ovector_pointer(matchdata);
	for(i = 0; i < rc; i++) {
		const char *strptr = str + ovector[2*i];
		size_t len = ovector[2*i+1] - ovector[2*i];

		cat->ovector[2*i] = ovector[2*i];
		cat->ovector[2*i+1] = ovector[2*i+1];

		asprintf(((cat->substrings)+i), "%.*s", (int)len, strptr);

		// printf("%2d: %s\n", i, cat->substrings[i]);
		// Didn't work for some reason
		// strncpy(substrings[i], str, (size_t) len);
		// pcre2_substring_copy_bynumber(matchdata, i, substrings[i], &strsize);
	}

	pcre2_match_data_free(matchdata);
	return cat;
}
