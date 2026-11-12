#ifndef MCNCC_TEST_SOCKET_FCNTL_H
#define MCNCC_TEST_SOCKET_FCNTL_H
#define F_GETFL 3
#define F_SETFL 4
#define O_NONBLOCK 4
int fcntl(int, int, ...);
#endif
