#include <assert.h>
#include <string.h>
#include "../../libthecore/include/crypt.h"

int main(void)
{
    const DWORD key[8] = {0x01234567UL, 0x89abcdefUL, 0xfedcba98UL, 0x76543210UL,
        0x13579bdfUL, 0x2468ace0UL, 0x0f1e2d3cUL, 0x4b5a6978UL};
    DWORD iv[2] = {0x12345678UL, 0x9abcdef0UL};
    const DWORD original_iv[2] = {0x12345678UL, 0x9abcdef0UL};
    const DWORD plain[6] = {0, 0xffffffffUL, 0x11223344UL, 0x55667788UL,
        0x13579bdfUL, 0x2468ace0UL};
    // Independent nibble-oriented calculation using the exact target S-box bytes.
    const DWORD expected[6] = {0x7edbe4a6UL, 0xdf50f588UL, 0x10151cb2UL,
        0xb3e6942cUL, 0xcd1c5815UL, 0x536edee2UL};
    DWORD encrypted[8], decoded[6], inplace[6];
    DWORD input[32], cipher[32], output[32];
    DWORD state = 1;
    int i, blocks;
    assert(sizeof(DWORD) == 4); // Compile for the recovered 32-bit word ABI.
    GOST_Init();
    for (i = 0; i < 8; ++i) encrypted[i] = 0xdeadbeefUL;
    assert(GOST_Encrypt(encrypted, plain, key, 24, iv) == 24);
    assert(!memcmp(encrypted, expected, sizeof(expected)));
    assert(encrypted[6] == 0xdeadbeefUL && encrypted[7] == 0xdeadbeefUL);
    assert(!memcmp(iv, original_iv, sizeof(iv))); // API does not write back the IV.
    assert(GOST_Decrypt(decoded, encrypted, key, 24, iv) == 24);
    assert(!memcmp(decoded, plain, sizeof(plain)));
    memcpy(inplace, plain, sizeof(plain));
    assert(GOST_Encrypt(inplace, inplace, key, 24, iv) == 24);
    assert(!memcmp(inplace, expected, sizeof(expected)));
    assert(GOST_Decrypt(inplace, inplace, key, 24, iv) == 24);
    assert(!memcmp(inplace, plain, sizeof(plain)));

    // Preserve the target's (Length >> 2) loop condition, including odd word counts.
    for (i = 0; i < 8; ++i) encrypted[i] = 0xdeadbeefUL;
    assert(GOST_Encrypt(encrypted, plain, key, 0, iv) == 0);
    assert(GOST_Encrypt(encrypted, plain, key, 3, iv) == 3);
    assert(encrypted[0] == 0xdeadbeefUL);
    assert(GOST_Encrypt(encrypted, plain, key, 4, iv) == 4);
    assert(encrypted[0] == expected[0] && encrypted[1] == expected[1]);
    assert(encrypted[2] == 0xdeadbeefUL);
    assert(GOST_Encrypt(encrypted, plain, key, 9, iv) == 9);
    assert(encrypted[2] == 0xdeadbeefUL);
    assert(GOST_Encrypt(encrypted, plain, key, 12, iv) == 12);
    assert(!memcmp(encrypted, expected, 4 * sizeof(DWORD)));
    assert(encrypted[4] == 0xdeadbeefUL);

    for (i = 0; i < 32; ++i)
    {
        state = state * 1664525UL + 1013904223UL;
        input[i] = state;
    }
    for (blocks = 1; blocks <= 16; ++blocks)
    {
        DWORD size = (DWORD)blocks * 8;
        assert(GOST_Encrypt(cipher, input, key, size, iv) == (int)size);
        assert(GOST_Decrypt(output, cipher, key, size, iv) == (int)size);
        assert(!memcmp(input, output, size));
        memcpy(output, cipher, size);
        assert(GOST_Decrypt(output, output, key, size, iv) == (int)size);
        assert(!memcmp(input, output, size));
    }
    GOST_Init();
    assert(GOST_Encrypt(encrypted, plain, key, 24, iv) == 24);
    assert(!memcmp(encrypted, expected, sizeof(expected)));
    return 0;
}
