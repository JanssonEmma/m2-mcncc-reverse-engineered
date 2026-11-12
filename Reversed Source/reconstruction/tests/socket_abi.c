// Checks the test declarations against target DWARF, not the installed OS ABI.
#include <stddef.h>
#include <sys/time.h>
#include <netdb.h>
#include "../../libthecore/include/socket.h"
#define CHECK(name, expr) typedef char name[(expr) ? 1 : -1]
CHECK(socket_t_width, sizeof(socket_t) == 4);
CHECK(socket_t_signed, (socket_t)-1 < 0);
CHECK(socket_size_t_width, sizeof(size_t) == 4);
CHECK(socket_ssize_width, sizeof(ssize_t) == 4);
CHECK(socket_socklen_width, sizeof(socklen_t) == 4);
CHECK(socket_pointer_width, sizeof(void*) == 4);
CHECK(socket_address_size, sizeof(struct sockaddr) == 16);
CHECK(socket_address_len, offsetof(struct sockaddr, sa_len) == 0);
CHECK(socket_address_family, offsetof(struct sockaddr, sa_family) == 1);
CHECK(socket_address_data, offsetof(struct sockaddr, sa_data) == 2);
CHECK(socket_ipv4_size, sizeof(struct sockaddr_in) == 16);
CHECK(socket_ipv4_len, offsetof(struct sockaddr_in, sin_len) == 0);
CHECK(socket_ipv4_family, offsetof(struct sockaddr_in, sin_family) == 1);
CHECK(socket_ipv4_port, offsetof(struct sockaddr_in, sin_port) == 2);
CHECK(socket_ipv4_addr, offsetof(struct sockaddr_in, sin_addr) == 4);
CHECK(socket_ipv4_zero, offsetof(struct sockaddr_in, sin_zero) == 8);
CHECK(socket_in_addr_size, sizeof(struct in_addr) == 4);
CHECK(socket_linger_size, sizeof(struct linger) == 8);
CHECK(socket_linger_on, offsetof(struct linger, l_onoff) == 0);
CHECK(socket_linger_wait, offsetof(struct linger, l_linger) == 4);
CHECK(socket_timeval_size, sizeof(struct timeval) == 8);
CHECK(socket_timeval_usec, offsetof(struct timeval, tv_usec) == 4);
CHECK(socket_hostent_size, sizeof(struct hostent) == 20);
CHECK(socket_hostent_aliases, offsetof(struct hostent, h_aliases) == 4);
CHECK(socket_hostent_type, offsetof(struct hostent, h_addrtype) == 8);
CHECK(socket_hostent_length, offsetof(struct hostent, h_length) == 12);
CHECK(socket_hostent_addresses, offsetof(struct hostent, h_addr_list) == 16);
#undef CHECK
