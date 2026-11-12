// Recovered utility interfaces; parameter types follow target DWARF.
#ifndef MCNCC_LIBTHECORE_UTILS_H
#define MCNCC_LIBTHECORE_UTILS_H

#include <sys/time.h>
#include "typedef.h"
#include <time.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
struct timeval* timediff(struct timeval* a, struct timeval* b);
struct timeval* timeadd(struct timeval* a, struct timeval* b);
void thecore_sleep(struct timeval* timeout);
DWORD get_dword_time(void);
struct tm* tm_calc(const struct tm* current, int days);
void core_dump_unix(const char* who, WORD line);
void thecore_msleep(DWORD milliseconds);
unsigned long long rdtsc(void);
float get_float_time(void);
int MIN(int a, int b);
int MAX(int a, int b);
int MINMAX(int min, int value, int max);
DWORD thecore_random(void);
int number_ex(int from, int to, char* file, int line);
void printdata(const unsigned char* data, int bytes);
char* time_str(time_t current);
void trim_and_lower(char* destination, const char* source, int max_len);
void lower_string(const char* source, char* destination, int len);
char* str_dup(const char* source);
int is_abbrev(char* arg1, char* arg2);
int filesize(FILE* stream);
void parse_token(char* source, char* token, char* value);
#ifdef __cplusplus
}
#endif

#endif
