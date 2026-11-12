// Recovered GOST and TEA interfaces; DES awaits module recovery.
#ifndef MCNCC_LIBTHECORE_CRYPT_H
#define MCNCC_LIBTHECORE_CRYPT_H
#include "typedef.h"
#ifdef __cplusplus
extern "C" {
#endif
void GOST_Init(void);
int TEA_Encrypt(DWORD* destination, const DWORD* source, const DWORD* key, int size);
int TEA_Decrypt(DWORD* destination, const DWORD* source, const DWORD* key, int size);
void tea_code(const DWORD sz, const DWORD sy, const DWORD* key, DWORD* destination);
void tea_decode(const DWORD sz, const DWORD sy, const DWORD* key, DWORD* destination);
int GOST_Encrypt(DWORD* destination, const DWORD* source, const DWORD* key,
                 DWORD length, DWORD* initial_vector);
int GOST_Decrypt(DWORD* destination, const DWORD* source, const DWORD* key,
                 DWORD length, DWORD* initial_vector);
#ifdef __cplusplus
}
#endif
#endif
