// Test-only process interception; no fork, abort or stream flush reaches the OS.
#ifndef MCNCC_TEST_UTILS_PROCESS_H
#define MCNCC_TEST_UTILS_PROCESS_H
#include <stdio.h>
#include <stdlib.h>
extern FILE* ut_stdout;
extern FILE* ut_stderr;
int ut_fork(void);
int ut_fflush(FILE* stream);
void ut_abort(void);
#define fork ut_fork
#define fflush ut_fflush
#define abort ut_abort
#undef stdout
#undef stderr
#define stdout ut_stdout
#define stderr ut_stderr
#endif
