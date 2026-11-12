// Link support for isolated numeric utility tests. These calls must stay unused.
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <sys/select.h>
void* (*thecore_memcpy)(void*, const void*, size_t) = memcpy;
int fork(void) { assert(!"unexpected fork"); return -1; }
int gettimeofday(struct timeval* value, struct timezone* zone)
{ (void)value; (void)zone; assert(!"unexpected clock access"); return -1; }
int select(int n, fd_set* r, fd_set* w, fd_set* e, struct timeval* timeout)
{ (void)n; (void)r; (void)w; (void)e; (void)timeout; assert(!"unexpected sleep"); return -1; }
