#ifndef DATATYPES_H
#define DATATYPES_H

/* Captured strings (substrings),
* no. of captures (size)
* and start and end index of
* each captured string in
* subject string (ovector) */
struct captured_data
{
	size_t size, *ovector;
	char **substrings;
};

/* Insertion location for all
 * substrings (locs), padding
 * values for integer values
 * and -1 for strings (padding)
 * and all substrings that must
 * be inserted to form final
 * string (substrings) */
struct rename_data
{
    size_t size;
    int *padding;
    char **substrings, *refstr;
};

#endif
