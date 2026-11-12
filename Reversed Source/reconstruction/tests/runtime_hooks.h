// Force-included only by the runtime test build. No real PID file is written.
#ifndef MCNCC_TEST_RUNTIME_HOOKS_H
#define MCNCC_TEST_RUNTIME_HOOKS_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
FILE* mc_test_fopen(const char* path, const char* mode);
int mc_test_fprintf(FILE* file, const char* format, ...);
int mc_test_fclose(FILE* file);
int mc_test_printf(const char* format, ...);
int mc_test_remove(const char* path);
time_t mc_test_time(time_t* value);
int mc_test_getpid(void);
unsigned int mc_test_getuid(void);
void mc_test_srandom(unsigned int seed);
void mc_test_srandomdev(void);
#define fopen mc_test_fopen
#define fprintf mc_test_fprintf
#define fclose mc_test_fclose
#define printf mc_test_printf
#define remove mc_test_remove
#define time mc_test_time
#define getpid mc_test_getpid
#define getuid mc_test_getuid
#define srandom mc_test_srandom
#define srandomdev mc_test_srandomdev
#endif
