// Recovered old logging API and file rotation. Production target is FreeBSD/POSIX.
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "../include/log.h"
#include "../include/utils.h"

typedef struct log_file_s
{
    char* filename;
    FILE* fp;
    int last_hour;
    int last_day;
} LOGFILE, *LPLOGFILE;

LPLOGFILE log_file_sys = NULL;
LPLOGFILE log_file_err = NULL;
LPLOGFILE log_file_pt = NULL;
static char log_dir[32];
static int log_level;
static char sys_log_header_string[33];
int log_keep_days = 3;

LPLOGFILE log_file_init(const char* filename, const char* mode);
void log_file_destroy(LPLOGFILE file);
void log_file_check(LPLOGFILE file);
void log_file_delete_old(const char* directory);
void log_file_rotate(LPLOGFILE file);
void log_file_set_dir(const char* directory);

// 0x081b8010 / 0x081b8020
void log_set_level(int level) { log_level = level; }
int log_get_level(void) { return log_level; }

// 0x081b8030
int log_init(void)
{
    log_file_set_dir("./log");
    log_file_sys = log_file_init("syslog", "a+");
    if (!log_file_sys)
        return 0;
    log_file_err = log_file_init("syserr", "a+");
    if (!log_file_err)
    {
        log_file_destroy(log_file_sys);
        return 0;
    }
    log_file_pt = log_file_init("PTS", "w");
    if (!log_file_pt)
    {
        log_file_destroy(log_file_sys);
        log_file_destroy(log_file_err);
        return 0;
    }
    return 1;
}

// 0x081b80d0: globals are intentionally not reset; destruction is single-use.
void log_destroy(void)
{
    log_file_destroy(log_file_sys);
    log_file_destroy(log_file_err);
    log_file_destroy(log_file_pt);
}

// 0x081b8110
void log_rotate(void)
{
    log_file_check(log_file_sys);
    log_file_check(log_file_err);
    log_file_check(log_file_pt);
    log_file_rotate(log_file_sys);
}

// 0x081b8150: writes to syserr and syslog, without console echo.
void _sys_err(const char* function, int line, const char* format, ...)
{
    va_list args;
    time_t now = time(NULL);
    char* stamp = asctime(localtime(&now));
    char buffer[1026];
    int length;
    (void)line;
    if (!log_file_err)
        return;
    stamp[strlen(stamp) - 1] = '\0';
    length = snprintf(buffer, 1024, "SYSERR: %-15.15s :: %s: ", stamp + 4, function);
    buffer[1025] = '\0';
    if (length < 1024)
    {
        va_start(args, format);
        vsnprintf(buffer + length, 1024 - length, format, args);
        va_end(args);
    }
    strcat(buffer, "\n");
    fputs(buffer, log_file_err->fp);
    fflush(log_file_err->fp);
    fputs(buffer, log_file_sys->fp);
    fflush(log_file_sys->fp);
}

// 0x081b8270: the final byte remains zero from static initialization.
void sys_log_header(const char* header)
{
    strncpy(sys_log_header_string, header, 32);
}

// 0x081b82a0: numeric level threshold, not the newer bitmask.
void sys_log(int level, const char* format, ...)
{
    va_list args;
    time_t now;
    char* stamp;
    if (level > log_level || !log_file_sys)
        return;
    now = time(NULL);
    stamp = asctime(localtime(&now));
    fputs(sys_log_header_string, log_file_sys->fp);
    stamp[strlen(stamp) - 1] = '\0';
    fprintf(log_file_sys->fp, "%-15.15s :: ", stamp + 4);
    va_start(args, format);
    vfprintf(log_file_sys->fp, format, args);
    va_end(args);
    fputc('\n', log_file_sys->fp);
    fflush(log_file_sys->fp);
    if (log_level > 0)
    {
        fputs(sys_log_header_string, stdout);
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fputc('\n', stdout);
        fflush(stdout);
    }
}

// 0x081b83e0
void pt_log(const char* format, ...)
{
    va_list args;
    if (!log_file_pt)
        return;
    va_start(args, format);
    vfprintf(log_file_pt->fp, format, args);
    va_end(args);
    fputc('\n', log_file_pt->fp);
    fflush(log_file_pt->fp);
}

// 0x081b8440
LPLOGFILE log_file_init(const char* filename, const char* mode)
{
    time_t now = time(NULL);
    struct tm current = *localtime(&now);
    FILE* stream = fopen(filename, mode);
    LPLOGFILE file;
    if (!stream)
        return NULL;
    file = (LPLOGFILE)malloc(sizeof(LOGFILE));
    file->filename = strdup(filename);
    file->fp = stream;
    file->last_hour = current.tm_hour;
    file->last_day = current.tm_mday;
    return file;
}

// 0x081b84c0: no null-file guard exists in the target.
void log_file_destroy(LPLOGFILE file)
{
    if (file->filename)
    {
        free(file->filename);
        file->filename = NULL;
    }
    if (file->fp)
    {
        fclose(file->fp);
        file->fp = NULL;
    }
    free(file);
}

// 0x081b8510
void log_file_check(LPLOGFILE file)
{
    struct stat status;
    if (stat(file->filename, &status) != 0 && errno == ENOENT)
    {
        fclose(file->fp);
        file->fp = fopen(file->filename, "a+");
    }
}

// 0x081b8570. Shell-command behavior is preserved; tests intercept system().
void log_file_delete_old(const char* directory)
{
    struct stat status;
    struct tm cutoff;
    struct dirent** names;
    char name[256], command[64], date[32];
    int count, cutoff_number, number;
    if (stat(directory, &status) == -1)
    {
        perror("log_file_delete_old: stat");
        return;
    }
    if (!S_ISDIR(status.st_mode))
        return;
    cutoff = *tm_calc(NULL, -log_keep_days);
    sprintf(date, "%04d%02d%02d", cutoff.tm_year + 1900, cutoff.tm_mon + 1, cutoff.tm_mday);
    cutoff_number = atoi(date);
    count = scandir(directory, &names, NULL, alphasort);
    if (count < 0)
        perror("scandir");
    else
    {
        while (count-- > 0)
        {
            strncpy(name, names[count]->d_name, 255);
            name[255] = '\0';
            free(names[count]);
            if (name[0] == '.' || name[0] < '0' || name[0] > '9')
                continue;
            number = atoi(name);
            if (number <= cutoff_number)
            {
                sprintf(command, "rm -rf %s/%s", directory, name);
                system(command);
                sys_log(0, "%s: SYSTEM_CMD: %s", "log_file_delete_old", command);
                fprintf(stderr, "%s: SYSTEM_CMD: %s %s:%d %s:%d\n",
                        "log_file_delete_old", command, date, cutoff_number, name, number);
            }
        }
    }
    free(names);
}

// 0x081b87d0
void log_file_rotate(LPLOGFILE file)
{
    time_t now = time(NULL);
    struct tm current = *localtime(&now);
    struct tm cutoff;
    struct stat status;
    char command[128], directory[128];
    if (current.tm_mday != file->last_day)
    {
        cutoff = *tm_calc(&current, -3);
        sprintf(command, "rm -rf %s/%04d%02d%02d", log_dir,
                cutoff.tm_year + 1900, cutoff.tm_mon + 1, cutoff.tm_mday);
        system(command);
        sys_log(0, "SYSTEM_CMD: %s", command);
        file->last_day = current.tm_mday;
    }
    if (current.tm_hour != file->last_hour)
    {
        snprintf(directory, 128, "%s/%04d%02d%02d", log_dir,
                 current.tm_year + 1900, current.tm_mon + 1, current.tm_mday);
        if (stat(directory, &status) != 0 || S_ISDIR(status.st_mode))
            mkdir(directory, 0700);
        sys_log(0, "SYSTEM: LOG ROTATE (%04d-%02d-%02d %d)",
                current.tm_year + 1900, current.tm_mon + 1, current.tm_mday, file->last_hour);
        fclose(file->fp);
        snprintf(command, 128, "mv %s %s/%s.%02d", file->filename,
                 directory, file->filename, file->last_hour);
        system(command);
        file->last_hour = current.tm_hour;
        file->fp = fopen(file->filename, "a+");
    }
}

// 0x081b8a00
void log_file_set_dir(const char* directory)
{
    strcpy(log_dir, directory);
    log_file_delete_old(log_dir);
}
