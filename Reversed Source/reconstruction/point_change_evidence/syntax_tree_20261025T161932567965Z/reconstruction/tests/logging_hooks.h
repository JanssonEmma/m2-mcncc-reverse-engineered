// Test-only replacements for every logger file, console, clock and shell call.
#ifndef MCNCC_TEST_LOGGING_HOOKS_H
#define MCNCC_TEST_LOGGING_HOOKS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
extern FILE* lg_stdout;
extern FILE* lg_stderr;
FILE* lg_fopen(const char* path, const char* mode);
int lg_fclose(FILE* file);
int lg_fprintf(FILE* file, const char* format, ...);
int lg_vfprintf(FILE* file, const char* format, va_list args);
int lg_fputs(const char* text, FILE* file);
int lg_fputc(int byte, FILE* file);
int lg_fflush(FILE* file);
time_t lg_time(time_t* result);
struct tm* lg_localtime(const time_t* value);
char* lg_asctime(const struct tm* value);
char* lg_strdup(const char* value);
void lg_perror(const char* message);
int lg_system(const char* command);
#define fopen lg_fopen
#define fclose lg_fclose
#define fprintf lg_fprintf
#define vfprintf lg_vfprintf
#define fputs lg_fputs
#define fputc lg_fputc
#define fflush lg_fflush
#define time lg_time
#define localtime lg_localtime
#define asctime lg_asctime
#define strdup lg_strdup
#define perror lg_perror
#define system lg_system
#undef stdout
#undef stderr
#define stdout lg_stdout
#define stderr lg_stderr
#endif
