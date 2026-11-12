#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <assert.h>
#include <setjmp.h>

#include "../../libthecore/include/utils.h"

static char output[8192], ascii_time[64];
static size_t output_len;
static long fake_position, fake_size;
static int seek_count, fail_alloc, perror_count, abort_count;
static long seek_offsets[4];
static int seek_origins[4];
static size_t allocation_count, allocation_size;
static time_t received_time;
static struct tm fixture_tm;
static jmp_buf allocation_failure;
int strings_test_fprintf(FILE *fp, const char *format, ...)
{
    int n;
    va_list ap;
    assert(fp == stderr);
    va_start(ap, format);
    n = vsnprintf(output + output_len, sizeof(output) - output_len, format, ap);
    va_end(ap);
    assert(n >= 0 && (size_t)n < sizeof(output) - output_len);
    output_len += (size_t)n;
    return n;
}
long strings_test_ftell(FILE *fp)
{
    assert(fp == (FILE *)1);
    return fake_position;
}
int strings_test_fseek(FILE *fp, long offset, int origin)
{
    assert(fp == (FILE *)1);
    assert(seek_count < 4);
    seek_offsets[seek_count] = offset;
    seek_origins[seek_count++] = origin;
    fake_position = origin == SEEK_END ? fake_size + offset : offset;
    return 0;
}
struct tm *strings_test_localtime(const time_t *ct)
{
    received_time = *ct;
    return &fixture_tm;
}
char *strings_test_asctime(const struct tm *tm)
{
    assert(tm == &fixture_tm);
    strcpy(ascii_time, "Wed Jun 30 21:49:08 1993\n");
    return ascii_time;
}
void *strings_test_calloc(size_t count, size_t size)
{
    allocation_count = count;
    allocation_size = size;
    return fail_alloc ? NULL : calloc(count, size);
}
void strings_test_perror(const char *msg)
{
    assert(strcmp(msg, "malloc failure") == 0);
    ++perror_count;
}
void strings_test_abort(void)
{
    ++abort_count;
    longjmp(allocation_failure, 1);
}

int main(void)
{
    char b[64], token[64], value[64], padded[96];
    char *s;
    unsigned char data[48];
    int i;
    char *dest = padded + 16;
    memset(padded, 0, sizeof(padded));
    trim_and_lower(dest, " \t\nMiXeD\t Name \r\n", 40);
    assert(strcmp(dest, "mixed\t name") == 0);
    trim_and_lower(dest, NULL, 40);
    assert(strcmp(dest, "mixed\t name") == 0);
    trim_and_lower(dest, "ABCD", 2);
    assert(strcmp(dest, "ab") == 0);
    trim_and_lower(dest, "XYZ", 0);
    assert(strcmp(dest, "x") == 0);
    trim_and_lower(dest, "XYZ", -1);
    assert(strcmp(dest, "x") == 0);
    trim_and_lower(dest, "", 40);
    assert(*dest == 0);
    padded[15] = ' '; padded[14] = '\t'; padded[13] = 'Q';
    trim_and_lower(dest, "   \t", 40);
    assert(padded[15] == 0 && padded[14] == 0 && padded[13] == 'Q');
    trim_and_lower(dest, "\xa0" "AB ", 40);
    assert((unsigned char)dest[0] == 0xa0 && strcmp(dest + 1, "ab") == 0);
    strcpy(b, "IN PLACE  ");
    trim_and_lower(b + 1, "AB ", 3); /* backed previous byte is non-space */
    assert(strcmp(b + 1, "ab") == 0);
    memset(b, '!', sizeof(b));
    lower_string("Ab", b, 6);
    assert(b[0] == 'a' && b[1] == 'b');
    for (i = 2; i < 6; ++i) assert(b[i] == 0);
    assert(b[6] == '!');
    memset(b, '!', sizeof(b)); lower_string("ABCDEFG", b, 3);
    assert(memcmp(b, "abc\0!", 5) == 0);
    memset(b, '!', sizeof(b)); lower_string("A", b, 0);
    assert(b[0] == 0 && b[1] == '!');
    memset(b, '!', sizeof(b)); lower_string("", b, -1);
    assert(b[0] == 0 && b[1] == '!');
    strcpy(b, "MIXED"); lower_string(b, b, 5);
    assert(strcmp(b, "mixed") == 0);
    assert(is_abbrev("", "hello") == 0);
    assert(is_abbrev("He", "HELLO") == 1);
    assert(is_abbrev("hello", "he") == 0);
    assert(is_abbrev("hey", "hello") == 0);
    assert(is_abbrev("\xa0", "\xa0" "rest") == 1);
    s = str_dup("Mixed bytes\xa0");
    assert(strcmp(s, "Mixed bytes\xa0") == 0);
    assert(allocation_count == strlen(s) + 1 && allocation_size == 1);
    free(s);
    s = str_dup(""); assert(*s == 0 && allocation_count == 1); free(s);
    fail_alloc = 1;
    if (setjmp(allocation_failure) == 0) { str_dup("failure"); assert(0); }
    assert(perror_count == 1 && abort_count == 1);
    fail_alloc = 0;
    parse_token(" \tNa Me : VALUE\r\n X\t ", token, value);
    assert(strcmp(token, "name") == 0 && strcmp(value, "VALUE X\t ") == 0);
    parse_token("NAME:Value", token, value);
    assert(strcmp(value, "alue") == 0); /* skips colon and exactly one byte */
    parse_token("NAME:  Value", token, value);
    assert(strcmp(value, " Value") == 0);
    { char malformed[] = {'A', 0, 'X', 'V', 0};
      parse_token(malformed, token, value);
      assert(strcmp(token, "a") == 0 && strcmp(value, "V") == 0); }
    { char high[] = {(char)0xa0, 'A', ':', ' ', 'V', 0};
      parse_token(high, token, value);
      assert((unsigned char)token[0] == 0xa0 && token[1] == 'a' && token[2] == 0); }
    s = time_str(12345);
    assert(received_time == 12345 && s == ascii_time + 4);
    assert(strcmp(s, "Jun 30 21:49:08") == 0 && ascii_time[19] == 0);
    fake_position = 7; fake_size = 123; seek_count = 0;
    assert(filesize((FILE *)1) == 123 && fake_position == 7);
    assert(seek_count == 2 && seek_offsets[0] == 0 && seek_origins[0] == SEEK_END);
    assert(seek_offsets[1] == 7 && seek_origins[1] == SEEK_SET);
    memset(data, 'A', sizeof(data)); data[0] = 'A'; data[1] = '\n'; data[2] = 0xa0; data[3] = 0xa1;
    output_len = 0; printdata(data, 4);
    assert(strstr(output, "41 0a a0 a1 ") != NULL);
    assert(strstr(output, "| ..\xa0\xa1\n") != NULL);
    output_len = 0; printdata(data, 0);
    assert(strlen(output) == 67 + 48 + 2 + 1 + 67);
    output_len = 0; printdata(data, 17);
    { int newlines = 0; for (i = 0; output[i]; ++i) newlines += output[i] == '\n'; assert(newlines == 4); }
    puts("utils string/file regression passed");
    return 0;
}

