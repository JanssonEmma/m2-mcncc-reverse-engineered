// Target-backed old buffer interface; see reconstruction/buffer-recovery.md.
#ifndef MCNCC_LIBTHECORE_BUFFER_H
#define MCNCC_LIBTHECORE_BUFFER_H
#include "typedef.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct buffer BUFFER;
typedef struct buffer* LPBUFFER;
struct buffer {
    struct buffer* next;
    char* write_point;
    int write_point_pos;
    const char* read_point;
    int length;
    char* mem_data;
    int mem_size;
    long flag;
};
LPBUFFER buffer_new(void* destination, int size);
void buffer_delete(LPBUFFER buffer);
DWORD buffer_size(LPBUFFER buffer);
void buffer_reset(LPBUFFER buffer);
void buffer_write(LPBUFFER buffer, const void* source, int length);
void buffer_read(LPBUFFER buffer, void* destination, int bytes);
BYTE buffer_byte(LPBUFFER buffer);
WORD buffer_word(LPBUFFER buffer);
DWORD buffer_dword(LPBUFFER buffer);
const void* buffer_read_peek(LPBUFFER buffer);
void buffer_read_proceed(LPBUFFER buffer, int length);
void* buffer_write_peek(LPBUFFER buffer);
void buffer_write_proceed(LPBUFFER buffer, int length);
int buffer_has_space(LPBUFFER buffer);
void buffer_adjust_size(LPBUFFER buffer, int add_size);
void buffer_realloc(LPBUFFER buffer, int length);
#ifdef __cplusplus
}
#endif
#endif
