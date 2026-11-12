#ifndef MCNCC_TEST_SOCKET_ERRNO_H
#define MCNCC_TEST_SOCKET_ERRNO_H
#include <stddef.h>
extern int socket_test_errno;
#undef errno
#define errno socket_test_errno
#define EINTR 4
#define EDEADLK 11
#define EINVAL 22
#define EAGAIN 35
#define EWOULDBLOCK 35
#define ENETUNREACH 51
#define ECONNRESET 54
#define ECONNREFUSED 61
#endif
