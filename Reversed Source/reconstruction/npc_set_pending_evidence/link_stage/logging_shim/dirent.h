// Test-only directory enumeration; the logger never opens a real directory here.
#ifndef MCNCC_TEST_DIRENT_H
#define MCNCC_TEST_DIRENT_H
struct dirent { char d_name[256]; };
int scandir(const char* path, struct dirent*** entries,
            int (*filter)(const struct dirent*),
            int (*compare)(const struct dirent**, const struct dirent**));
int alphasort(const struct dirent** left, const struct dirent** right);
#endif
