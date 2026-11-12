// Test-only stat/mkdir ABI; production uses the platform headers.
#ifndef MCNCC_TEST_SYS_STAT_H
#define MCNCC_TEST_SYS_STAT_H
struct stat { unsigned int st_mode; };
#define S_IFDIR 0040000
#define S_IFREG 0100000
#define S_ISDIR(mode) (((mode) & 0170000) == S_IFDIR)
int stat(const char* path, struct stat* status);
int mkdir(const char* path, unsigned int mode);
#endif
