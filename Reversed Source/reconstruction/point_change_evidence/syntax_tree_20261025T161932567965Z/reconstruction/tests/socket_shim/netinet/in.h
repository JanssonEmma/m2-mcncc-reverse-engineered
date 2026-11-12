#ifndef MCNCC_TEST_INET_ABI_H
#define MCNCC_TEST_INET_ABI_H
#include <sys/socket.h>
struct in_addr { unsigned int s_addr; };
struct sockaddr_in {
    unsigned char sin_len, sin_family;
    unsigned short sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
};
#endif
