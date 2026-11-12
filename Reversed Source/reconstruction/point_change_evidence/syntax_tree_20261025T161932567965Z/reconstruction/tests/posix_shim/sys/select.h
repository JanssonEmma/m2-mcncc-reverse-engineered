// Test-only select interface. All fd_set arguments are NULL in recovered code.
#ifndef MCNCC_TEST_SYS_SELECT_H
#define MCNCC_TEST_SYS_SELECT_H
#include <sys/time.h>
typedef struct { unsigned int unused; } fd_set;
#ifdef __cplusplus
extern "C" {
#endif
int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
           struct timeval* timeout);
#ifdef __cplusplus
}
#endif
#endif
