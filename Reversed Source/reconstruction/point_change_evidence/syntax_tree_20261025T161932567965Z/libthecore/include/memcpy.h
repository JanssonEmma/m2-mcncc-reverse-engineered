// Dependency interface; the CPU-specific selector implementation is still pending.
#ifndef MCNCC_LIBTHECORE_MEMCPY_H
#define MCNCC_LIBTHECORE_MEMCPY_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
extern void* (*thecore_memcpy)(void* destination, const void* source, size_t count);
void thecore_find_best_memcpy(void);
#ifdef __cplusplus
}
#endif
#endif
