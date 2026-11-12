// Recovered socket interface.
#ifndef MCNCC_LIBTHECORE_SOCKET_H
#define MCNCC_LIBTHECORE_SOCKET_H
#include <stddef.h>
#include "typedef.h"
#ifdef __WIN32__
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
int socket_udp_read(socket_t descriptor, char* destination, size_t size,
                    struct sockaddr* peer, socklen_t* peer_length);
int socket_read(socket_t descriptor, char* destination, size_t size);
int socket_write_tcp(socket_t descriptor, const char* data, int length);
int socket_write(socket_t descriptor, const char* data, size_t length);
int socket_bind(const char* ip, int port, int protocol);
int socket_tcp_bind(const char* ip, int port);
int socket_udp_bind(const char* ip, int port);
void socket_close(socket_t descriptor);
socket_t socket_accept(socket_t descriptor, struct sockaddr_in* peer);
socket_t socket_connect(const char* host, WORD port);
void socket_nonblock(socket_t descriptor);
void socket_block(socket_t descriptor);
void socket_dontroute(socket_t descriptor);
void socket_lingeroff(socket_t descriptor);
void socket_lingeron(socket_t descriptor);
void socket_rcvbuf(socket_t descriptor, unsigned int size);
void socket_sndbuf(socket_t descriptor, unsigned int size);
void socket_timeout(socket_t descriptor, long seconds, long microseconds);
void socket_reuse(socket_t descriptor);
void socket_keepalive(socket_t descriptor);
#ifdef __cplusplus
}
#endif
#endif
