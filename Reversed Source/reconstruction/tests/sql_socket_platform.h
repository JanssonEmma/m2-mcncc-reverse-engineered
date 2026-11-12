// Minimal test-only MySQL socket type; no global OS-call redirection.
#ifndef MCNCC_TEST_SQL_SOCKET_PLATFORM_H
#define MCNCC_TEST_SQL_SOCKET_PLATFORM_H
#define my_socket_defined
typedef int my_socket;
#endif
