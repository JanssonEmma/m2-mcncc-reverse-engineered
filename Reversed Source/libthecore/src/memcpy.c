// Reconstructed from the target's x86 assembly and method-table data.
// The supplied reference retains attribution to the DirectFB/xine memcpy code;
// see reconstruction/memcpy-reference-notice.txt for its original notice.
#include <stdlib.h>
#include <string.h>
#include "../include/memcpy.h"
#include "../include/log.h"

void* (*thecore_memcpy)(void*, const void*, size_t) = memcpy;

// 0x081ba910. The short path returns the advanced pointer in the original code.
static void* linux_kernel_memcpy(void* to, const void* from, size_t length)
{
    void* original = to;
    if (length < 4)
    {
        __asm__ volatile ("rep movsb" : "+D"(to), "+S"(from), "+c"(length) : : "memory");
        return to;
    }
    else
    {
        size_t words = length >> 2;
        __asm__ volatile ("rep movsl" : "+D"(to), "+S"(from), "+c"(words) : : "memory");
        if (length & 2)
            __asm__ volatile ("movsw" : "+D"(to), "+S"(from) : : "memory");
        if (length & 1)
            __asm__ volatile ("movsb" : "+D"(to), "+S"(from) : : "memory");
    }
    return original;
}

#define MMX_LOAD \
    "movq 0(%0), %%mm0\n\t" "movq 8(%0), %%mm1\n\t" \
    "movq 16(%0), %%mm2\n\t" "movq 24(%0), %%mm3\n\t" \
    "movq 32(%0), %%mm4\n\t" "movq 40(%0), %%mm5\n\t" \
    "movq 48(%0), %%mm6\n\t" "movq 56(%0), %%mm7\n\t"
#define MMX_STORE \
    "movq %%mm0, 0(%1)\n\t" "movq %%mm1, 8(%1)\n\t" \
    "movq %%mm2, 16(%1)\n\t" "movq %%mm3, 24(%1)\n\t" \
    "movq %%mm4, 32(%1)\n\t" "movq %%mm5, 40(%1)\n\t" \
    "movq %%mm6, 48(%1)\n\t" "movq %%mm7, 56(%1)\n\t"
#define MMX_STREAM_STORE \
    "movntq %%mm0, 0(%1)\n\t" "movntq %%mm1, 8(%1)\n\t" \
    "movntq %%mm2, 16(%1)\n\t" "movntq %%mm3, 24(%1)\n\t" \
    "movntq %%mm4, 32(%1)\n\t" "movntq %%mm5, 40(%1)\n\t" \
    "movntq %%mm6, 48(%1)\n\t" "movntq %%mm7, 56(%1)\n\t"
#define MMX_CLOBBERS "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7", "memory"

// 0x081ba770: 2 KiB threshold, eight-byte destination alignment, 64-byte blocks.
static void* mmx_memcpy(void* to, const void* from, size_t length)
{
    void* original = to;
    unsigned char* destination = (unsigned char*)to;
    const unsigned char* source = (const unsigned char*)from;
    if (length >= 2048)
    {
        size_t alignment = (8 - ((size_t)destination & 7)) & 7;
        size_t bytes = alignment;
        size_t blocks;
        __asm__ volatile ("rep movsb" : "+D"(destination), "+S"(source), "+c"(bytes) : : "memory");
        length -= alignment;
        blocks = length >> 6;
        length &= 63;
        while (blocks--)
        {
            __asm__ volatile (MMX_LOAD MMX_STORE : : "r"(source), "r"(destination) : MMX_CLOBBERS);
            source += 64;
            destination += 64;
        }
        __asm__ volatile ("emms" : : : "memory");
    }
    if (length)
        linux_kernel_memcpy(destination, source, length);
    return original;
}

// 0x081ba830. Ghidra labels the 0f 18 /0 prefetch hints as NOPs; bytes retained.
static void* mmx2_memcpy(void* to, const void* from, size_t length)
{
    void* original = to;
    unsigned char* destination = (unsigned char*)to;
    const unsigned char* source = (const unsigned char*)from;
    __asm__ volatile ("prefetchnta 0(%0)\n\t" "prefetchnta 64(%0)\n\t"
                      "prefetchnta 128(%0)\n\t" "prefetchnta 192(%0)\n\t"
                      "prefetchnta 256(%0)" : : "r"(source));
    if (length >= 64)
    {
        size_t alignment = (8 - ((size_t)destination & 7)) & 7;
        size_t bytes = alignment;
        size_t blocks;
        __asm__ volatile ("rep movsb" : "+D"(destination), "+S"(source), "+c"(bytes) : : "memory");
        length -= alignment;
        blocks = length >> 6;
        length &= 63;
        while (blocks--)
        {
            __asm__ volatile ("prefetchnta 320(%0)\n\t" MMX_LOAD MMX_STREAM_STORE
                              : : "r"(source), "r"(destination) : MMX_CLOBBERS);
            source += 64;
            destination += 64;
        }
        __asm__ volatile ("sfence\n\temms" : : : "memory");
    }
    if (length)
        linux_kernel_memcpy(destination, source, length);
    return original;
}

typedef struct
{
    const char* name;
    void* (*function)(void*, const void*, size_t);
    unsigned long long time;
    unsigned int cpu;
} MEMCPY_METHOD;

static MEMCPY_METHOD memcpy_method[] = {
    {NULL, NULL, 0, 0},
    {"glibc memcpy()", memcpy, 0, 0},
    {"MMXEXT optimized memcpy()", mmx2_memcpy, 0, 0},
    {"MMX optimized memcpy()", mmx_memcpy, 0, 0},
    {"linux kernel memcpy()", linux_kernel_memcpy, 0, 0},
    {NULL, NULL, 0, 0}
};

static inline unsigned long long read_cycles(void)
{
    unsigned int low, high;
    __asm__ volatile ("rdtsc" : "=a"(low), "=d"(high));
    return ((unsigned long long)high << 32) | low;
}

// 0x081ba970. No feature gating is present in the original table/selection loop.
void thecore_find_best_memcpy(void)
{
    const size_t size = 8192 * 2000;
    char* first = (char*)calloc(1, size);
    char* second;
    int best = 0, i, j;
    if (!first)
        return;
    second = (char*)calloc(1, size);
    if (!second)
    {
        free(first);
        return;
    }
    sys_log(1, "memcpy: Benchmarking memcpy methods (smaller is better):");
    memcpy(first, second, size);
    memcpy(second, first, size);
    for (i = 1; memcpy_method[i].name; ++i)
    {
        unsigned long long start = read_cycles();
        for (j = 0; j < 2000; ++j)
            memcpy_method[i].function(first + j * 8192, second + j * 8192, 8192);
        memcpy_method[i].time = read_cycles() - start;
        sys_log(1, "memcpy: \t%s : %lld", memcpy_method[i].name, (long long)memcpy_method[i].time);
        if (!best || memcpy_method[i].time < memcpy_method[best].time)
            best = i;
    }
    if (best)
    {
        sys_log(1, "memcpy: using %s", memcpy_method[best].name);
        thecore_memcpy = memcpy_method[best].function;
    }
    free(first);
    free(second);
}
