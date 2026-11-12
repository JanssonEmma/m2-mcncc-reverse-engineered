// Includes the actual module to inspect its three private pools.
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../libthecore/include/buffer.h"
static void* bf_malloc(size_t);
static void* bf_calloc(size_t, size_t);
static void bf_free(void*);
static void bf_perror(const char*);
static void bf_abort(void);
#define malloc bf_malloc
#define calloc bf_calloc
#define free bf_free
#define perror bf_perror
#define abort bf_abort
#include "../../libthecore/src/buffer.c"
#undef malloc
#undef calloc
#undef free
#undef perror
#undef abort

typedef struct allocation { void* pointer; size_t size; int live; } allocation;
static allocation allocations[128];
static int allocated, freed, calloc_calls, malloc_calls, copy_calls;
static int fail_calloc_at, fail_malloc, expected_abort, expected_null_copy;
static int perror_calls, abort_calls, error_calls, error_line, log_calls;
static int copy_length;
static void* copy_destination;
static const void* copy_source;
static char error_format[160], log_format[160];
static jmp_buf escape;
static void* remember(void* value, size_t size) {
    assert(value && allocated < 128);
    allocations[allocated].pointer = value;
    allocations[allocated].size = size;
    allocations[allocated++].live = 1;
    return value;
}
static void* bf_calloc(size_t count, size_t size) {
    ++calloc_calls;
    if (calloc_calls == fail_calloc_at) return NULL;
    // A zero-size allocation is permitted to return nonnull.
    return remember(calloc(count ? count : 1, size ? size : 1), count * size);
}
static void* bf_malloc(size_t size) {
    void* value;
    ++malloc_calls;
    if (fail_malloc) return NULL;
    value = malloc(size ? size : 1);
    memset(value, 0xa5, size);
    return remember(value, size);
}
static void bf_free(void* value) {
    int i;
    for (i = allocated - 1; i >= 0; --i)
        if (allocations[i].pointer == value && allocations[i].live) break;
    assert(i >= 0);
    allocations[i].live = 0; ++freed; free(value);
}
static void bf_perror(const char* text) {
    assert(!strcmp(text, "malloc failure")); ++perror_calls;
}
static void bf_abort(void) {
    assert(expected_abort); ++abort_calls; longjmp(escape, 1);
}
static void* bf_copy(void* dest, const void* src, size_t bytes) {
    ++copy_calls; copy_destination = dest; copy_source = src; copy_length = (int)bytes;
    if (!dest) {
        assert(expected_null_copy); longjmp(escape, 2);
    }
    return memcpy(dest, src, bytes);
}
void* (*thecore_memcpy)(void*, const void*, size_t) = bf_copy;
int MIN(int a, int b) { return a < b ? a : b; }
void _sys_err(const char* function, int line, const char* format, ...) {
    assert(!strcmp(function, "buffer_read_proceed") || !strcmp(function, "buffer_realloc"));
    ++error_calls; error_line = line;
    strcpy(error_format, format);
}
void sys_log(int level, const char* format, ...) {
    assert(level == 0); ++log_calls; strcpy(log_format, format);
}
static void reset_fixture(void) {
    int i;
    // Test-only teardown: the original module has no pool destruction API.
    for (i = 0; i < allocated; ++i)
        if (allocations[i].live) free(allocations[i].pointer);
    buffer_poll = buffer_poll_big = buffer_nodelete_poll = NULL;
    allocated = freed = calloc_calls = malloc_calls = copy_calls = 0;
    fail_calloc_at = fail_malloc = expected_abort = expected_null_copy = 0;
    perror_calls = abort_calls = error_calls = error_line = log_calls = 0;
}
static void check_pools(void) {
    LPBUFFER first, second, reused, big, huge;
    char external[32], other[40];
    assert(buffer_new(NULL, -1) == NULL && buffer_new(external, -1) == NULL);
    assert(calloc_calls == 0);
    first = buffer_new(NULL, 64);
    assert(first->mem_size == 64 && calloc_calls == 2);
    assert(first->mem_data[0] == 0 && first->mem_data[63] == 0);
    first->mem_data[0] = 'Q'; first->flag = 8;
    second = buffer_new(NULL, 128);
    buffer_delete(first); buffer_delete(second);
    assert(buffer_poll == second && second->next == first);
    reused = buffer_new(NULL, 32);
    assert(reused == second && reused->mem_size == 128 && reused->next == first);
    assert(buffer_poll == first && !reused->length && reused->read_point == reused->mem_data);
    reused = buffer_new(NULL, 16);
    assert(reused == first && reused->mem_size == 64 && reused->flag == 8);
    assert(reused->mem_data[0] == 'Q');
    reset_fixture();
    first = buffer_new(NULL, 8192); big = buffer_new(NULL, 8193);
    buffer_delete(first); buffer_delete(big);
    assert(buffer_poll == first && buffer_poll_big == big);
    assert(buffer_new(NULL, 8192) == first && buffer_new(NULL, 8193) == big);
    huge = buffer_new(NULL, 131073);
    { int old = freed; buffer_delete(huge); assert(freed == old + 2); }
    huge = buffer_new(NULL, 131072);
    buffer_delete(huge); assert(buffer_poll_big == huge);
    reset_fixture();
    first = buffer_new(external, 32);
    assert(calloc_calls == 1 && first->flag == 1 && first->mem_data == external);
    first->flag |= 16;
    buffer_write(first, "abcd", 4);
    buffer_delete(first);
    assert(!freed && !first->mem_data && first->mem_size == 32);
    assert(first->read_point == external && first->write_point == external);
    assert(buffer_nodelete_poll == first);
    second = buffer_new(other, 40);
    assert(second == first && calloc_calls == 1 && second->flag == 17);
    assert(second->mem_data == other && second->read_point == other);
    buffer_delete(second);
    reset_fixture();
}
static void check_growth(void) {
    LPBUFFER b = buffer_new(NULL, 16), identity = b;
    char* old;
    int old_copies, old_logs;
    memset(b->mem_data, 'P', 16);
    buffer_write(b, "ABCDEFGH", 8);
    buffer_read_proceed(b, 3);
    b->flag = 8; b->next = b;
    old = b->mem_data;
    buffer_realloc(b, 48);
    assert(b == identity && b->mem_size == 48 && b->mem_data != old);
    assert(copy_source == old && copy_length == 16 && freed == 1);
    assert(b->write_point_pos == 8 && b->length == 5);
    assert(b->read_point == b->mem_data + 3 && b->write_point == b->mem_data + 8);
    assert(b->flag == 8 && b->next == b && !memcmp(b->mem_data, "ABCDEFGHPPPPPPPP", 16));
    assert((unsigned char)b->mem_data[16] == 0xa5);
    old_copies = copy_calls; old_logs = log_calls;
    buffer_realloc(b, 48); buffer_realloc(b, 1);
    assert(copy_calls == old_copies && log_calls == old_logs);
    buffer_reset(b); assert(b->flag == 8 && b->next == b);
    reset_fixture();
    b = buffer_new(NULL, 4); buffer_write(b, "ABCD", 4);
    assert(b->mem_size == 16 && b->length == 4 && b->write_point_pos == 4);
    assert(malloc_calls == 1); // Equality already triggers growth.
    old_logs = log_calls;
    buffer_adjust_size(b, 12);
    assert(log_calls == old_logs && b->mem_size == 16);
    buffer_adjust_size(b, 13);
    assert(log_calls == old_logs + 2 && b->mem_size == 29 && b->length == 4);
    reset_fixture();
    { char external[64];
      b = buffer_new(external, 4);
      buffer_realloc(b, 8);
      assert(error_line == 240 && b->mem_size == 4 && !malloc_calls);
      assert(!strcmp(error_format, "this buffer cannot be reallocated."));
      buffer_write(b, "ABCD", 4);
      assert(error_calls == 2 && b->length == 4 && !memcmp(external, "ABCD", 4));
      buffer_write(b, "EF", 2); // Backed external memory permits checking unchecked continuation.
      assert(error_calls == 3 && b->length == 6 && buffer_has_space(b) == -2);
      buffer_adjust_size(b, 10);
      assert(error_calls == 4 && b->mem_size == 4);
    }
    reset_fixture();
    b = buffer_new(NULL, 600000);
    { char* payload = (char*)calloc(600000, 1);
      buffer_write(b, payload, 600000);
      assert(b->mem_size == 600000 + 600000 + 1048576);
      free(payload);
    }
    reset_fixture();
}
static void check_io(void) {
    LPBUFFER b = buffer_new(NULL, 32);
    unsigned char packet[8] = {0x81, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12, 'Z'};
    char out[8];
    assert(buffer_size(b) == 0 && buffer_has_space(b) == 32);
    buffer_write(b, packet, 8);
    assert(buffer_size(b) == 8 && buffer_read_peek(b) == b->mem_data);
    assert(buffer_write_peek(b) == b->mem_data + 8 && buffer_has_space(b) == 24);
    assert(buffer_byte(b) == 0x81);
    assert(buffer_word(b) == 0x1234);
    assert(buffer_dword(b) == 0x12345678UL);
    buffer_read(b, out, 1); assert(out[0] == 'Z');
    assert(!b->length && !b->write_point_pos && b->read_point == b->mem_data);
    memcpy(buffer_write_peek(b), "hello", 5); buffer_write_proceed(b, 5);
    buffer_read_proceed(b, 0); assert(b->length == 5 && !error_calls);
    buffer_read(b, out, 2); assert(!memcmp(out, "he", 2) && b->length == 3);
    buffer_read_proceed(b, -1);
    assert(error_line == 176 && b->length == 4 && b->read_point == b->mem_data + 1);
    buffer_read_proceed(b, 100);
    assert(error_line == 179 && !b->length && b->read_point == b->mem_data);
    // buffer_read copies before its proceed step clamps an oversized count.
    memcpy(b->mem_data, "WXYZ", 4); b->length = 2;
    buffer_read(b, out, 4);
    assert(copy_length == 4 && !memcmp(out, "WXYZ", 4) && !b->length);
    reset_fixture();
}
static void check_failures(void) {
    LPBUFFER b;
    fail_calloc_at = 1; expected_abort = 1;
    if (!setjmp(escape)) { buffer_new(NULL, 16); assert(!"header allocation returned"); }
    assert(perror_calls == 1 && abort_calls == 1);
    reset_fixture();
    fail_calloc_at = 2; expected_abort = 1;
    if (!setjmp(escape)) { buffer_new(NULL, 16); assert(!"data allocation returned"); }
    assert(perror_calls == 1 && abort_calls == 1 && allocated == 1);
    reset_fixture();
    b = buffer_new(NULL, 16); fail_malloc = expected_null_copy = 1;
    if (!setjmp(escape)) { buffer_realloc(b, 32); assert(!"null copy returned"); }
    assert(copy_destination == NULL && copy_length == 16 && !freed && !abort_calls);
    reset_fixture();
    b = buffer_new(NULL, 32); b->mem_size = 4; b->length = 10;
    b->read_point = b->mem_data + 3; expected_abort = 1;
    if (!setjmp(escape)) { buffer_read_proceed(b, 2); assert(!"overflow returned"); }
    assert(error_line == 189 && abort_calls == 1);
    assert(b->read_point == b->mem_data + 3 && b->length == 10);
    reset_fixture();
}
int main(void) {
    assert(sizeof(DWORD) == 4);
    check_pools(); check_growth(); check_io(); check_failures();
    puts("buffer regression passed: pools, external ownership, growth, I/O and failures");
    return 0;
}
