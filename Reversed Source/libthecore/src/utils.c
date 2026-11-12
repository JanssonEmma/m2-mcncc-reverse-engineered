// Reconstructed utilities; see reconstruction/utils-recovery.md for evidence.
#include <errno.h>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include "../include/utils.h"
#include "../include/log.h"
#include "../include/memcpy.h"

// BSD random(), also declared here for strict-C99 feature visibility.
extern long random(void);

static struct timeval null_time = {0, 0};

static int32_t signed_time_word(uint32_t bits)
{
    int32_t value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

// 0x081b8c10: returns borrowed static storage; negative differences clamp to zero.
struct timeval* timediff(struct timeval* a, struct timeval* b)
{
    static struct timeval result;
    const int32_t a_seconds = signed_time_word((uint32_t)a->tv_sec);
    const int32_t b_seconds = signed_time_word((uint32_t)b->tv_sec);
    const int32_t a_microseconds = signed_time_word((uint32_t)a->tv_usec);
    const int32_t b_microseconds = signed_time_word((uint32_t)b->tv_usec);
    const int borrow = a_microseconds < b_microseconds;
    if (a_seconds < b_seconds || (a_seconds == b_seconds && borrow))
        return &null_time;
    result.tv_sec = signed_time_word((uint32_t)a_seconds - (uint32_t)b_seconds - borrow);
    result.tv_usec = signed_time_word((uint32_t)a_microseconds - (uint32_t)b_microseconds
                                    + (borrow ? UINT32_C(1000000) : 0));
    return &result;
}

// 0x081b8ca0: positive microsecond overflow is normalized; negatives are not.
struct timeval* timeadd(struct timeval* a, struct timeval* b)
{
    static struct timeval result;
    uint32_t seconds = (uint32_t)a->tv_sec + (uint32_t)b->tv_sec;
    int32_t microseconds = signed_time_word((uint32_t)a->tv_usec + (uint32_t)b->tv_usec);
    while (microseconds >= 1000000)
    {
        microseconds -= 1000000;
        ++seconds;
    }
    result.tv_sec = signed_time_word(seconds);
    result.tv_usec = microseconds;
    return &result;
}

// 0x081b9350: interrupted sleeps are not retried.
void thecore_sleep(struct timeval* timeout)
{
    if (select(0, NULL, NULL, NULL, timeout) < 0 && errno != EINTR)
        _sys_err("thecore_sleep", 355, "select sleep %s", strerror(errno));
}

// 0x081b94b0: fixed epoch offset, NOT the newer get_boot_sec() behavior.
// Unsigned arithmetic preserves the emitted 32-bit wrap without signed overflow.
DWORD get_dword_time(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    const uint32_t milliseconds = (uint32_t)now.tv_sec * UINT32_C(1000)
        + (uint32_t)(now.tv_usec / 1000) - UINT32_C(0x43d4db10);
    return (DWORD)milliseconds;
}

// 0x081b90e0. Preserve the legacy leap-day adjustment and single-month arithmetic.
struct tm* tm_calc(const struct tm* current, int days)
{
    static struct tm result;
    int monthdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int year;
    if (current)
        thecore_memcpy(&result, current, sizeof(struct tm));
    else
    {
        time_t now = time(NULL);
        result = *localtime(&now);
    }
    year = result.tm_year + 1900;
    if (result.tm_mon == 1 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
    {
        ++result.tm_mday;
        monthdays[1] = 29;
    }
    result.tm_mday += days;
    if (result.tm_mday <= 0)
    {
        if (--result.tm_mon < 0)
        {
            --result.tm_year;
            result.tm_mon = 11;
        }
        result.tm_mday += monthdays[result.tm_mon];
    }
    else if (result.tm_mday > monthdays[result.tm_mon])
    {
        if (++result.tm_mon > 11)
        {
            ++result.tm_year;
            result.tm_mon = 0;
        }
        result.tm_mday = monthdays[result.tm_mon] - result.tm_mday;
    }
    return &result;
}

// 0x081b93c0: the original uses the entire millisecond value for tv_usec.
void thecore_msleep(DWORD milliseconds)
{
    static struct timeval timeout;
    const uint32_t duration = (uint32_t)milliseconds;
    const uint32_t microsecond_bits = duration * UINT32_C(1000);
    int32_t microseconds;
    memcpy(&microseconds, &microsecond_bits, sizeof(microseconds));
    timeout.tv_sec = duration / UINT32_C(1000);
    timeout.tv_usec = microseconds;
    thecore_sleep(&timeout);
}

// 0x081b9400: fork failure returns, just like a successful parent branch.
void core_dump_unix(const char* who, WORD line)
{
    _sys_err("core_dump_unix", 371, "*** Dumping Core %s:%d ***", who, (int)line);
    fflush(stdout);
    fflush(stderr);
    if (fork() == 0)
        abort();
}

// 0x081b9460: bare, unserialized RDTSC; return is EDX:EAX on the target.
unsigned long long rdtsc(void)
{
    unsigned int low, high;
    __asm__ volatile ("rdtsc" : "=a"(low), "=d"(high));
    return ((unsigned long long)high << 32) | low;
}

// 0x081b9470: retain the fixed epoch and signed 32-bit subtraction.
long double get_float_time_extended(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    const uint32_t seconds_bits = (uint32_t)now.tv_sec - UINT32_C(1057699978);
    int32_t seconds;
    memcpy(&seconds, &seconds_bits, sizeof(seconds));
    return (long double)seconds + (long double)now.tv_usec / 1000000.0L;
}

float get_float_time(void)
{
    return (float)get_float_time_extended();
}
/* Numeric functions: see reconstruction/utils-numeric-recovery.md. */
int MIN(int a, int b)
{
    return a < b ? a : b;
}

int MAX(int a, int b)
{
    return a > b ? a : b;
}

int MINMAX(int min, int value, int max)
{
    int tv = min > value ? min : value;
    return max < tv ? max : tv;
}

DWORD thecore_random(void)
{
    return (DWORD)random();
}

int number_ex(int from, int to, char *file, int line)
{
    DWORD span;
    if (from > to)
    {
        int tmp = from;
        _sys_err("number_ex", 339,
                 "number(): first argument is bigger than second argument %d -> %d, %s %d",
                 from, to, file, line);
        from = to;
        to = tmp;
    }
    /* Original 32-bit SUB/LEA wrap before unsigned DIV; no zero-span guard. */
    span = (DWORD)to - (DWORD)from + 1UL;
    return (int)((DWORD)from + thecore_random() % span);
}
/* Recovered utils.c string/file portion.
 * Preserve the old API and edge behavior; see utils-strings-recovery.md.
 * Requires stdio.h, stdlib.h, string.h, time.h and ctype.h.
 */
static int utils_ascii_lower(int c)
{
    return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}

void printdata(const unsigned char *data, int bytes)
{
    int i, j = bytes, k;
    const unsigned char *p;
    fprintf(stderr, "------------------------------------------------------------------\n");
    do {
        k = j >= 16 ? 16 : j;
        p = data;
        for (i = 0; i < 16; ++i, ++p) {
            if (i >= k) fprintf(stderr, "   ");
            else fprintf(stderr, "%02x ", *p);
        }
        fprintf(stderr, "| ");
        p = data;
        for (i = 0; i < k; ++i, ++p) {
            int a = ((*p & 0xe0) > 0x90) || isprint(*p);
            fprintf(stderr, "%c", a && (((p[1] & 0xe0) > 0x90) || isprint(p[1])) ? *p : '.');
        }
        fprintf(stderr, "\n");
        j -= 16;
        data += 16;
    } while (j > 0);
    fprintf(stderr, "------------------------------------------------------------------\n");
}

char *time_str(time_t ct)
{
    static char *time_s;
    ct = (time_t)signed_time_word((uint32_t)ct);
    time_s = asctime(localtime(&ct));
    time_s[strlen(time_s) - 6] = '\0';
    time_s += 4;
    return time_s;
}

void trim_and_lower(char *dest, const char *src, int max_len)
{
    int len = 0;
    if (!src) return;
    while (*src && (((*src & 0xe0) <= 0x90) && isspace(*src))) ++src;
    while (*src) {
        *dest++ = (char)utils_ascii_lower(*src++);
        if (++len >= max_len) break;
    }
    /* The old machine code decrements before checking, even with zero copied. */
    do {
        *dest = '\0';
        --dest;
    } while (*dest && isspace(*dest));
}

void lower_string(const char *src, char *dest, int len)
{
    int i = 0;
    while (*src && i < len) {
        *dest++ = (char)utils_ascii_lower(*src++);
        ++i;
    }
    *dest = '\0';
    while (i < len) {
        *dest++ = '\0';
        ++i;
    }
}

char *str_dup(const char *source)
{
    char *new_line = (char *)calloc(strlen(source) + 1, 1);
    if (!new_line) {
        perror("malloc failure");
        abort();
    }
    return strcpy(new_line, source);
}

int is_abbrev(char *arg1, char *arg2)
{
    if (!*arg1) return 0;
    for (; *arg1 && *arg2; ++arg1, ++arg2)
        if (utils_ascii_lower(*arg1) != utils_ascii_lower(*arg2)) return 0;
    return !*arg1;
}

int filesize(FILE *fp)
{
    int pos = (int)ftell(fp);
    int size;
    fseek(fp, 0, SEEK_END);
    size = (int)ftell(fp);
    fseek(fp, pos, SEEK_SET);
    return size;
}

void parse_token(char *src, char *token, char *value)
{
    char *tmp;
    for (tmp = src; *tmp && *tmp != ':'; ++tmp) {
        if (((*tmp & 0xe0) <= 0x90) && isspace(*tmp)) continue;
        *token++ = (char)utils_ascii_lower(*tmp);
    }
    *token = '\0';
    for (tmp += 2; *tmp; ++tmp) {
        if (*tmp == '\n' || *tmp == '\r') continue;
        *value++ = *tmp;
    }
    *value = '\0';
}

