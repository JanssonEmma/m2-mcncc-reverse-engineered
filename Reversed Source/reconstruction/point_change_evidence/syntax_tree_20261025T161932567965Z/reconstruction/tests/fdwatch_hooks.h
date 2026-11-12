#ifndef MCNCC_TEST_FDWATCH_HOOKS_H
#define MCNCC_TEST_FDWATCH_HOOKS_H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
void* fw_calloc(size_t, size_t);
void fw_free(void*);
void fw_perror(const char*);
void fw_abort(void);
char* fw_strerror(int);


#define calloc fw_calloc
#define free fw_free
#define perror fw_perror
#define abort fw_abort
#define strerror fw_strerror
#endif
