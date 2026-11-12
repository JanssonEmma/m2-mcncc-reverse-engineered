// Test-only FreeBSD 32-bit socket ABI declarations. No real networking.
#ifndef MCNCC_TEST_SOCKET_ABI_H
#define MCNCC_TEST_SOCKET_ABI_H
#include <stddef.h>
typedef unsigned int socklen_t;
// Keep the fixture return width at 32 bits despite MinGW64's CRT ssize_t.
typedef int mc_socket_ssize_t;
#define ssize_t mc_socket_ssize_t
struct sockaddr { unsigned char sa_len, sa_family; char sa_data[14]; };
struct linger { int l_onoff, l_linger; };
#define AF_INET 2
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOL_SOCKET 65535
#define SOMAXCONN 128
#define SO_REUSEADDR 4
#define SO_KEEPALIVE 8
#define SO_DONTROUTE 16
#define SO_LINGER 128
#define SO_SNDBUF 4097
#define SO_RCVBUF 4098
#define SO_SNDTIMEO 4101
#define SO_RCVTIMEO 4102
int socket(int, int, int);
ssize_t recv(int, void*, size_t, int);
ssize_t recvfrom(int, void*, size_t, int, struct sockaddr*, socklen_t*);
ssize_t send(int, const void*, size_t, int);
int bind(int, const struct sockaddr*, socklen_t);
int listen(int, int);
int accept(int, struct sockaddr*, socklen_t*);
int connect(int, const struct sockaddr*, socklen_t);
int setsockopt(int, int, int, const void*, socklen_t);
int getsockopt(int, int, int, void*, socklen_t*);
#endif
