#ifndef PCRE_CAPTURE_H
#define PCRE_CAPTURE_H

struct captured_data *getdata(pcre2_code *, const char *);

pcre2_code *getregex(const char *);

void free_pcre2(pcre2_code *, pcre2_match_data *);

#endif
