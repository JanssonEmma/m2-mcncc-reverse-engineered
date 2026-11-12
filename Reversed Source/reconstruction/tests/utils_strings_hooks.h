#ifndef UTILS_STRINGS_HOOKS_H
#define UTILS_STRINGS_HOOKS_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int strings_test_fprintf(FILE *, const char *, ...);
long strings_test_ftell(FILE *);
int strings_test_fseek(FILE *, long, int);
struct tm *strings_test_localtime(const time_t *);
char *strings_test_asctime(const struct tm *);
void *strings_test_calloc(size_t, size_t);
void strings_test_perror(const char *);
void strings_test_abort(void);
#define fprintf strings_test_fprintf
#define ftell strings_test_ftell
#define fseek strings_test_fseek
#define localtime strings_test_localtime
#define asctime strings_test_asctime
#define calloc strings_test_calloc
#define perror strings_test_perror
#define abort strings_test_abort
#endif
