#include <assert.h>
#include <string.h>
#include "../../libthecore/src/memcpy.c"

static int messages;
void sys_log(int level, const char* format, ...)
{
    assert(level == 1 && format);
    ++messages;
}

int main(void)
{
    unsigned char source[4160], destination[4160];
    const size_t sizes[] = {0, 1, 2, 3, 4, 5, 7, 8, 15, 16, 31, 32, 63, 64, 65,
                            127, 128, 2047, 2048, 2049, 4095, 4096, 4097};
    void* (*methods[3])(void*, const void*, size_t) = {mmx_memcpy, mmx2_memcpy, linux_kernel_memcpy};
    unsigned int method, source_offset, destination_offset;
    size_t i, test;
    int best = 1, index;
    for (i = 0; i < sizeof(source); ++i)
        source[i] = (unsigned char)(i * 73 + 19);
    assert(thecore_memcpy == memcpy);
    for (method = 0; method < 3; ++method)
        for (source_offset = 0; source_offset < 8; ++source_offset)
            for (destination_offset = 0; destination_offset < 8; ++destination_offset)
                for (test = 0; test < sizeof(sizes) / sizeof(sizes[0]); ++test)
                {
                    size_t size = sizes[test];
                    unsigned char* to = destination + 16 + destination_offset;
                    void* result;
                    memset(destination, 0xa5, sizeof(destination));
                    result = methods[method](to, source + source_offset, size);
                    assert(!memcmp(to, source + source_offset, size));
                    assert(to[-1] == 0xa5 && to[size] == 0xa5);
                    assert(result == (method == 2 && size < 4 ? to + size : to));
                }
    thecore_find_best_memcpy();
    assert(messages == 6); // Header, four timings and selected method.
    for (index = 2; index < 5; ++index)
        if (memcpy_method[index].time < memcpy_method[best].time)
            best = index;
    assert(thecore_memcpy == memcpy_method[best].function);
    memset(destination, 0, sizeof(destination));
    thecore_memcpy(destination, source, sizeof(destination));
    assert(!memcmp(destination, source, sizeof(destination)));
    return 0;
}
