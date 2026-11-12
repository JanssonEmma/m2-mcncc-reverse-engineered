#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "../../libthecore/include/typedef.h"
#include "tea_oracle.inc"

int TEA_Encrypt(DWORD *, const DWORD *, const DWORD *, int);
int TEA_Decrypt(DWORD *, const DWORD *, const DWORD *, int);
void tea_code(const DWORD, const DWORD, const DWORD *, DWORD *);
void tea_decode(const DWORD, const DWORD, const DWORD *, DWORD *);
extern char tea_nilbuf[8];

static DWORD random_word(void)
{
    static DWORD state = 0x12345678UL;
    state ^= state << 13; state ^= state >> 17; state ^= state << 5;
    return state;
}

static int oracle(DWORD *dst, const DWORD *src, const DWORD *key, int size, int decrypt)
{
    int n = size, blocks;
    if ((size & 7) != 0) {
        n = (int)(((DWORD)(size < 0 ? size + 7 : size) & 0xfffffff8UL) + 8UL);
        if (!decrypt) memset((char *)src + size, 0, (unsigned int)n - (unsigned int)size);
    }
    blocks = n >> 3;
    while (blocks-- > 0) {
        if (decrypt) tea_oracle_tea_decode(src[1], src[0], key, dst);
        else tea_oracle_tea_code(src[1], src[0], key, dst);
        dst += 2; src += 2;
    }
    return n;
}

int main(void)
{
    DWORD k[4] = {0,0,0,0}, zero[2] = {0,0}, out[2];
    DWORD a[64], b[64];
    unsigned int trial, j;
    int decrypt, length, layout;
    assert(sizeof(DWORD) == 4);
    for (j = 0; j < 8; ++j) assert(tea_nilbuf[j] == 0);
    /* Fixed XTEA zero vector: independent of the decompiler oracle. */
    assert(TEA_Encrypt(out, zero, k, 8) == 8);
    assert(out[0] == 0xdee9d4d8UL && out[1] == 0xf7131ed9UL);
    assert(TEA_Decrypt(out, out, k, 8) == 8);
    assert(out[0] == 0 && out[1] == 0);

    for (trial = 0; trial < 1000; ++trial) {
        DWORD expected[2], actual[2];
        DWORD y = random_word(), z = random_word();
        for (j = 0; j < 4; ++j) k[j] = random_word();
        tea_code(z, y, k, actual); tea_oracle_tea_code(z, y, k, expected);
        assert(memcmp(actual, expected, sizeof actual) == 0);
        tea_decode(z, y, k, actual); tea_oracle_tea_decode(z, y, k, expected);
        assert(memcmp(actual, expected, sizeof actual) == 0);
    }
    /* Whole-arena comparisons catch input mutation, output extent and overlap.
       Source starts centrally so negative lengths have valid padding storage. */
    for (decrypt = 0; decrypt < 2; ++decrypt)
    for (length = -17; length <= 40; ++length)
    for (layout = 0; layout < 7; ++layout) {
        int dest = layout == 0 ? 40 : layout == 1 ? 16 : layout == 2 ? 17 : layout == 3 ? 15 : 24;
        int key = layout == 4 ? dest : layout == 5 ? 16 : layout == 6 ? dest + 1 : 4;
        int got, want;
        for (j = 0; j < 64; ++j) a[j] = random_word();
        memcpy(b, a, sizeof a);
        got = decrypt ? TEA_Decrypt(a+dest, a+16, a+key, length) : TEA_Encrypt(a+dest, a+16, a+key, length);
        want = oracle(b+dest, b+16, b+key, length, decrypt);
        assert(got == want);
        assert(memcmp(a, b, sizeof a) == 0);
    }
    assert(TEA_Encrypt(NULL, NULL, NULL, 0) == 0);
    assert(TEA_Decrypt(NULL, NULL, NULL, 0) == 0);
    assert(TEA_Decrypt(NULL, NULL, NULL, -8) == -8);
    assert(TEA_Decrypt(NULL, NULL, NULL, INT_MIN) == INT_MIN);
    assert(TEA_Decrypt(NULL, NULL, NULL, INT_MAX) == INT_MIN);
    puts("TEA regression passed: fixed vector, 2000 helper oracle comparisons, 812 wrapper/alias cases");
    return 0;
}
