#ifndef STRING_UTILS_H
#define STRING_UTILS_H
char *string_utils_zfill (const int n, const int digits);
char *string_utils_filename (const char *prefixe, const int n,
			     const int digits, const char *sufixe);
#endif /* STRING_UTILS_H  */
