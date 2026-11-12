// Test-only process identity interface. The runtime tests remap these calls.
#ifndef MCNCC_TEST_UNISTD_H
#define MCNCC_TEST_UNISTD_H
#ifdef __cplusplus
extern "C" {
#endif
int getpid(void);
unsigned int getuid(void);
int fork(void);
int usleep(unsigned int);
#ifdef __cplusplus
}
#endif
#endif
