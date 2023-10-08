#ifndef STRINGFUNCS_H
#define STRINGFUNCS_H

char *join_path(char [], char []);

int set_refstr(struct captured_data *, struct rename_data *, char *);

char *construct_string(struct rename_data *);

#endif
