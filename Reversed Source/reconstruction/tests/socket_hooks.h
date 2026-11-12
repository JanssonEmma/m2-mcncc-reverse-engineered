// Every socket/DNS/descriptor operation is intercepted in this test build.
#ifndef MCNCC_TEST_SOCKET_HOOKS_H
#define MCNCC_TEST_SOCKET_HOOKS_H
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
ssize_t sk_recv(int, void*, size_t, int);
ssize_t sk_recvfrom(int, void*, size_t, int, struct sockaddr*, socklen_t*);
ssize_t sk_send(int, const void*, size_t, int);
int sk_socket(int, int, int);
int sk_bind(int, const struct sockaddr*, socklen_t);
int sk_listen(int, int);
int sk_accept(int, struct sockaddr*, socklen_t*);
int sk_connect(int, const struct sockaddr*, socklen_t);
int sk_close(int);
int sk_setsockopt(int, int, int, const void*, socklen_t);
int sk_getsockopt(int, int, int, void*, socklen_t*);
int sk_fcntl(int, int, ...);
unsigned int sk_inet_addr(const char*);
unsigned short sk_htons(unsigned short);
struct hostent* sk_gethostbyname(const char*);
void sk_perror(const char*);
char* sk_strerror(int);
#define recv sk_recv
#define recvfrom sk_recvfrom
#define send sk_send
#define socket sk_socket
#define bind sk_bind
#define listen sk_listen
#define accept sk_accept
#define connect sk_connect
#define close sk_close
#define setsockopt sk_setsockopt
#define getsockopt sk_getsockopt
#define fcntl sk_fcntl
#define inet_addr sk_inet_addr
#define htons sk_htons
#define gethostbyname sk_gethostbyname
#define perror sk_perror
#define strerror sk_strerror
#endif
