/* Recovered from game SHA256 61fa6e73...; see reconstruction/tea-recovery.md. */
#include "../include/crypt.h"
#include <string.h>
#include <limits.h>

typedef char tea_requires_32_bit_DWORD[(sizeof(DWORD) == 4 && ULONG_MAX == 0xffffffffUL) ? 1 : -1];

char tea_nilbuf[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

/* The target emits these helpers as external symbols, not solely inlined code. */
void tea_code(const DWORD sz, const DWORD sy, const DWORD *key, DWORD *dest)
{
    DWORD y = sy, z = sz, sum = 0;
    unsigned int round;
    for (round = 0; round < 32; ++round) {
        y += (((z << 4) ^ (z >> 5)) + z) ^ (sum + key[sum & 3]);
        sum += 0x9e3779b9UL;
        z += (((y << 4) ^ (y >> 5)) + y) ^ (sum + key[(sum >> 11) & 3]);
    }
    dest[0] = y;
    dest[1] = z;
}

void tea_decode(const DWORD sz, const DWORD sy, const DWORD *key, DWORD *dest)
{
    DWORD y = sy, z = sz, sum = 0xc6ef3720UL;
    unsigned int round;
    for (round = 0; round < 32; ++round) {
        z -= (((y << 4) ^ (y >> 5)) + y) ^ (sum + key[(sum >> 11) & 3]);
        sum -= 0x9e3779b9UL;
        y -= (((z << 4) ^ (z >> 5)) + z) ^ (sum + key[sum & 3]);
    }
    dest[0] = y;
    dest[1] = z;
}

int TEA_Encrypt(DWORD *dest, const DWORD *src, const DWORD *key, int size)
{
    int i, resize = size;
    if (size % 8 != 0) {
        resize = (int)((((DWORD)(size < 0 ? size + 7 : size)) & 0xfffffff8UL) + 8UL);
        /* Historical API writes through its const source parameter. */
        memset((char *)src + size, 0, (size_t)((unsigned int)resize - (unsigned int)size));
    }
    for (i = 0; i < (resize >> 3); ++i, dest += 2, src += 2)
        tea_code(src[1], src[0], key, dest);
    return resize;
}

int TEA_Decrypt(DWORD *dest, const DWORD *src, const DWORD *key, int size)
{
    int i, resize = size;
    if (size % 8 != 0)
        resize = (int)((((DWORD)(size < 0 ? size + 7 : size)) & 0xfffffff8UL) + 8UL);
    for (i = 0; i < (resize >> 3); ++i, dest += 2, src += 2)
        tea_decode(src[1], src[0], key, dest);
    return resize;
}
