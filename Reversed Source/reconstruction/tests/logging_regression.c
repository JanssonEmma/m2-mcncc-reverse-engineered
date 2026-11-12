// Includes the real recovered logger to inspect its internal lifecycle state.
// All file operations, process launches and clock calls are test fixtures.
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include "logging_hooks.h"
#include "../../libthecore/src/log.c"

static unsigned char stream_tokens[5];
FILE* lg_stdout = (FILE*)&stream_tokens[3];
FILE* lg_stderr = (FILE*)&stream_tokens[4];
static char output[5][32768];
static int opens[3], closes[3], flushes[5], open_failure = -1;
static char last_mode[3][8];
static struct tm current;
static char commands[32][256];
static int command_count, mkdir_count, perror_count;
static const char* missing_file;
static int missing_error = ENOENT, folder_is_regular;
void* (*thecore_memcpy)(void*, const void*, size_t) = memcpy;

static int Stream(FILE* stream)
{
    int i;
    for (i = 0; i < 5; ++i)
        if (stream == (FILE*)&stream_tokens[i])
            return i;
    assert(0);
    return -1;
}
static void Append(FILE* file, const char* text)
{
    int index = Stream(file);
    assert(strlen(output[index]) + strlen(text) < sizeof(output[index]));
    strcat(output[index], text);
}
static void ClearOutput(void) { memset(output, 0, sizeof(output)); }
FILE* lg_fopen(const char* name, const char* mode)
{
    int index = !strcmp(name, "syslog") ? 0 : !strcmp(name, "syserr") ? 1 : 2;
    assert(index != 2 || !strcmp(name, "PTS"));
    ++opens[index];
    strcpy(last_mode[index], mode);
    return index == open_failure ? NULL : (FILE*)&stream_tokens[index];
}
int lg_fclose(FILE* file) { ++closes[Stream(file)]; return 0; }
int lg_vfprintf(FILE* file, const char* format, va_list args)
{
    char buffer[4096];
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    assert(length >= 0 && (size_t)length < sizeof(buffer));
    Append(file, buffer);
    return length;
}
int lg_fprintf(FILE* file, const char* format, ...)
{
    int result;
    va_list args;
    va_start(args, format);
    result = lg_vfprintf(file, format, args);
    va_end(args);
    return result;
}
int lg_fputs(const char* text, FILE* file) { Append(file, text); return 0; }
int lg_fputc(int byte, FILE* file)
{
    char text[2];
    text[0] = (char)byte;
    text[1] = 0;
    Append(file, text);
    return byte;
}
int lg_fflush(FILE* file) { ++flushes[Stream(file)]; return 0; }
time_t lg_time(time_t* result) { if (result) *result = 0; return 0; }
struct tm* lg_localtime(const time_t* value) { (void)value; return &current; }
char* lg_asctime(const struct tm* value)
{
    static char stamp[32];
    (void)value;
    strcpy(stamp, "Tue Oct 20 12:34:56 2009\n");
    return stamp;
}
char* lg_strdup(const char* value)
{
    char* copy = (char*)malloc(strlen(value) + 1);
    assert(copy);
    return strcpy(copy, value);
}
void lg_perror(const char* message) { (void)message; ++perror_count; }
int lg_system(const char* command)
{
    assert(command_count < 32 && strlen(command) < sizeof(commands[0]));
    strcpy(commands[command_count++], command);
    return 0;
}
int stat(const char* name, struct stat* status)
{
    if (missing_file && !strcmp(missing_file, name))
    {
        errno = missing_error;
        return -1;
    }
    status->st_mode = !strncmp(name, "./log", 5) && !folder_is_regular ? S_IFDIR : S_IFREG;
    return 0;
}
int mkdir(const char* name, unsigned int mode)
{
    assert(!strncmp(name, "./log/", 6) && mode == 0700);
    ++mkdir_count;
    errno = EEXIST;
    return -1; // Existing-directory failure is ignored by the original.
}
int alphasort(const struct dirent** left, const struct dirent** right)
{
    return strcmp((*left)->d_name, (*right)->d_name);
}
int scandir(const char* path, struct dirent*** result,
            int (*filter)(const struct dirent*),
            int (*compare)(const struct dirent**, const struct dirent**))
{
    static const char* names[] = {".", "..", "20091016", "20091017", "20091018", "foo"};
    int i;
    assert(!strcmp(path, "./log") && !filter && compare == alphasort);
    *result = (struct dirent**)malloc(6 * sizeof(**result));
    for (i = 0; i < 6; ++i)
    {
        (*result)[i] = (struct dirent*)malloc(sizeof(struct dirent));
        strcpy((*result)[i]->d_name, names[i]);
    }
    return 6;
}
int gettimeofday(struct timeval* value, struct timezone* zone)
{
    (void)value; (void)zone;
    assert(0); // No wall-clock millisecond helper is used by logger tests.
    return -1;
}
int select(int count, fd_set* a, fd_set* b, fd_set* c, struct timeval* timeout)
{
    (void)count; (void)a; (void)b; (void)c; (void)timeout;
    assert(0);
    return -1;
}

int main(void)
{
    int stage;
    current.tm_year = 109;
    current.tm_mon = 9;
    current.tm_mday = 20;
    current.tm_hour = 12;
    assert(log_get_level() == 0 && log_keep_days == 3);
    sys_log(0, "before init");
    pt_log("before init");
    _sys_err("before", 1, "before init");
    assert(output[0][0] == 0 && output[3][0] == 0);
    assert(log_init() == 1);
    assert(command_count == 2);
    assert(!strcmp(commands[0], "rm -rf ./log/20091017"));
    assert(!strcmp(commands[1], "rm -rf ./log/20091016"));
    assert(!strcmp(last_mode[0], "a+") && !strcmp(last_mode[2], "w"));
    ClearOutput();
    sys_log(1, "hidden %d", 7);
    assert(output[0][0] == 0);
    sys_log(0, "v=%d", 7);
    assert(!strcmp(output[0], "Oct 20 12:34:56 :: v=7\n") && output[3][0] == 0);
    log_set_level(2);
    sys_log_header("N ");
    ClearOutput();
    sys_log(1, "shown %d", 9);
    assert(!strcmp(output[0], "N Oct 20 12:34:56 :: shown 9\n"));
    assert(!strcmp(output[3], "N shown 9\n"));
    ClearOutput();
    _sys_err("fn", 777, "err %d", 4);
    assert(!strcmp(output[1], "SYSERR: Oct 20 12:34:56 :: fn: err 4\n"));
    assert(!strcmp(output[0], output[1]) && output[3][0] == 0);
    pt_log("point %d", 5);
    assert(!strcmp(output[2], "point 5\n"));
    {
        char header[41];
        memset(header, 'H', 40); header[40] = 0;
        sys_log_header(header);
        ClearOutput();
        sys_log(0, "x");
        assert(strlen(output[3]) == 34 && output[3][31] == 'H' && output[3][32] == 'x');
    }
    log_destroy();
    // The original globals dangle after destruction. Reset only in this fixture.
    log_file_sys = log_file_err = log_file_pt = NULL;
    for (stage = 0; stage < 3; ++stage)
    {
        int old_closes = closes[0] + closes[1] + closes[2];
        open_failure = stage;
        assert(log_init() == 0);
        assert(closes[0] + closes[1] + closes[2] - old_closes == stage);
        log_file_sys = log_file_err = log_file_pt = NULL;
    }
    open_failure = -1;
    assert(log_init() == 1);
    log_set_level(0);
    sys_log_header("");
    command_count = 0;
    log_rotate();
    assert(command_count == 0 && mkdir_count == 0);
    missing_file = "PTS";
    log_rotate();
    assert(!strcmp(last_mode[2], "a+"));
    missing_file = NULL;
    current.tm_mday = 21;
    log_keep_days = 9;
    log_rotate();
    assert(command_count == 1 && !strcmp(commands[0], "rm -rf ./log/20091018"));
    current.tm_hour = 13;
    log_rotate();
    assert(command_count == 2 && mkdir_count == 1);
    assert(!strcmp(commands[1], "mv syslog ./log/20091021/syslog.12"));
    folder_is_regular = 1;
    current.tm_hour = 14;
    log_rotate();
    assert(mkdir_count == 1 && log_file_sys->last_hour == 14);
    assert(log_file_sys->last_day == 21);
    log_destroy();
    assert(perror_count == 0);
    return 0;
}
