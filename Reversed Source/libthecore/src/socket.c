// Reconstructed from the original target; see reconstruction/socket-recovery.md.
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include "../include/socket.h"
#include "../include/log.h"
#include "../include/memcpy.h"

int socket_udp_read(socket_t descriptor, char* destination, size_t size,
                    struct sockaddr* peer, socklen_t* peer_length)
{
    return (int)recvfrom(descriptor, destination, size, 0, peer, peer_length);
}

int socket_read(socket_t descriptor, char* destination, size_t size)
{
    int result = (int)recv(descriptor, destination, size, 0);
    if (result > 0)
        return result;
    if (result == 0)
        return -1;
    if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK || errno == EDEADLK)
        return 0;
    _sys_err("socket_read", 58, "about to lose connection");
    return -1;
}

int socket_write_tcp(socket_t descriptor, const char* data, int length)
{
    int result = (int)send(descriptor, data, (size_t)length, 0);
    if (result > 0)
        return result;
    if (result == 0)
        return -1;
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EDEADLK)
        return 0;
    return -1;
}

int socket_write(socket_t descriptor, const char* data, size_t length)
{
    do
    {
        int written = socket_write_tcp(descriptor, data, (int)length);
        if (written < 0)
        {
            if (errno == EWOULDBLOCK)
                errno = EAGAIN;
            if (errno == EAGAIN)
                _sys_err("socket_write", 109, "socket write would block, about to close!");
            else
                _sys_err("socket_write", 111, "write to desc error");
            return -1;
        }
        data += written;
        length -= (size_t)written;
    } while (length != 0);
    return 0;
}

int socket_bind(const char* ip, int port, int protocol)
{
    int descriptor = socket(AF_INET, protocol, 0);
    struct sockaddr_in address;
    if (descriptor < 0)
    {
        _sys_err("socket_bind", 137, "socket: %s", strerror(errno));
        return -1;
    }
    socket_reuse(descriptor);
    socket_lingeroff(descriptor);
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons((unsigned short)port);
    if (bind(descriptor, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        _sys_err("socket_bind", 151, "bind: %s", strerror(errno));
        return -1;
    }
    socket_nonblock(descriptor);
    if (protocol == SOCK_STREAM)
    {
        sys_log(0, "SYSTEM: BINDING TCP PORT ON [%d]", port);
        listen(descriptor, SOMAXCONN);
    }
    else
        sys_log(0, "SYSTEM: BINDING UDP PORT ON [%d]", port);
    return descriptor;
}

int socket_tcp_bind(const char* ip, int port) { return socket_bind(ip, port, SOCK_STREAM); }
int socket_udp_bind(const char* ip, int port) { return socket_bind(ip, port, SOCK_DGRAM); }
void socket_close(socket_t descriptor) { close(descriptor); }

socket_t socket_accept(socket_t descriptor, struct sockaddr_in* peer)
{
    socklen_t peer_length = sizeof(*peer);
    socket_t accepted = accept(descriptor, (struct sockaddr*)peer, &peer_length);
    if (accepted == -1)
    {
        _sys_err("socket_accept", 196, "accept: %s (fd %d)", strerror(errno), descriptor);
        return -1;
    }
    if (accepted >= 65500)
    {
        _sys_err("socket_accept", 202, "SOCKET FD 65500 LIMIT! %d", accepted);
#ifdef MCNCC_DB_MYSQL8
        socket_close(accepted);
#else
        socket_close(descriptor); // Historical game behavior.
#endif
        return -1;
    }
#ifdef MCNCC_DB_MYSQL8
    const int flags = fcntl(accepted, F_GETFL, 0);
    const struct linger linger = {0, 0};
    if (flags < 0 || fcntl(accepted, F_SETFL, flags | O_NONBLOCK) < 0 ||
        setsockopt(accepted, SOL_SOCKET, SO_LINGER, (const char*)&linger, sizeof(linger)) < 0)
    {
        _sys_err("socket_accept", 0, "configure accepted socket: %s", strerror(errno));
        socket_close(accepted);
        return -1;
    }
#else
    socket_nonblock(accepted);
    socket_lingeroff(accepted);
#endif
    return accepted;
}

socket_t socket_connect(const char* host, WORD port)
{
    struct sockaddr_in address;
    socket_t descriptor;
    int result;
    memset(&address, 0, sizeof(address));
    // FreeBSD's observed rune test excludes negative/high signed-char inputs.
    if ((unsigned int)(int)*host < 256U && isdigit((unsigned char)*host))
        address.sin_addr.s_addr = inet_addr(host);
    else
    {
        struct hostent* entry = gethostbyname(host);
        if (!entry)
        {
            _sys_err("socket_connect", 229, "socket_connect(): can not connect to %s:%d",
                     host, (int)port);
            return -1;
        }
        thecore_memcpy(&address.sin_addr, entry->h_addr_list[0], sizeof(address.sin_addr));
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor < 0)
    {
        perror("socket");
        return -1;
    }
    socket_keepalive(descriptor);
    socket_sndbuf(descriptor, 233016);
    socket_rcvbuf(descriptor, 233016);
    socket_timeout(descriptor, 10, 0);
    socket_lingeron(descriptor);
    result = connect(descriptor, (struct sockaddr*)&address, sizeof(address));
    if (result < 0)
    {
        socket_close(descriptor);
        // The original switches on the return value, not errno.
        switch (result)
        {
        case EINTR:
            _sys_err("socket_connect", 267, "HOST %s:%d connection timeout.", host, (int)port);
            break;
        case ECONNREFUSED:
            _sys_err("socket_connect", 274,
                     "HOST %s:%d port is not opened. connection refused.", host, (int)port);
            break;
        case ENETUNREACH:
            _sys_err("socket_connect", 281,
                     "HOST %s:%d is not reachable from this host.", host, (int)port);
            break;
        default:
            _sys_err("socket_connect", 285, "HOST %s:%d, could not connect.", host, (int)port);
            break;
        }
        perror("connect");
        return -1;
    }
    return descriptor;
}
void socket_nonblock(socket_t s)
{
    int flags;

    flags = fcntl(s, F_GETFL, 0);
    flags |= O_NONBLOCK;

    if (fcntl(s, F_SETFL, flags) < 0) 
    {
	_sys_err(__func__, 311, "fcntl: nonblock: %s", strerror(errno));
	return;
    }
}

void socket_block(socket_t s)
{
    int flags;

    flags = fcntl(s, F_GETFL, 0);
    flags &= ~O_NONBLOCK;

    if (fcntl(s, F_SETFL, flags) < 0)
    {
	_sys_err(__func__, 325, "fcntl: nonblock: %s", strerror(errno));
	return;
    }
}
void socket_dontroute(socket_t s)
{
    int set;

    if (setsockopt(s, SOL_SOCKET, SO_DONTROUTE, (const char *) &set, sizeof(int)) < 0)
    {
	_sys_err(__func__, 349, "setsockopt: dontroute: %s", strerror(errno));
	socket_close(s);
	return;
    }
}

void socket_lingeroff(socket_t s)
{
    struct linger linger;

    linger.l_onoff	= 0;
    linger.l_linger	= 0;
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, (const char*) &linger, sizeof(linger)) < 0)
    {
	_sys_err(__func__, 368, "setsockopt: linger: %s", strerror(errno));
	socket_close(s);
	return;
    }
}

void socket_lingeron(socket_t s)
{
    struct linger linger;

    linger.l_onoff	= 1;
    linger.l_linger	= 0;
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, (const char*) &linger, sizeof(linger)) < 0)
    {
	_sys_err(__func__, 387, "setsockopt: linger: %s", strerror(errno));
	socket_close(s);
	return;
    }
}

void socket_rcvbuf(socket_t s, unsigned int opt)
{
    socklen_t optlen;

    optlen = sizeof(opt);

    if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, (const char*) &opt, optlen) < 0)
    {
	_sys_err(__func__, 401, "setsockopt: rcvbuf: %s", strerror(errno));
	socket_close(s);
	return;
    }

    opt         = 0;
    optlen      = sizeof(opt);

    if (getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*) &opt, &optlen) < 0)
    {
	_sys_err(__func__, 411, "getsockopt: rcvbuf: %s", strerror(errno));
	socket_close(s);
	return;
    }

    sys_log(1, "SYSTEM: %d: receive buffer changed to %d", s, opt);
}

void socket_sndbuf(socket_t s, unsigned int opt)
{
    socklen_t optlen;

    optlen = sizeof(opt);

    if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (const char*) &opt, optlen) < 0)
    {
	_sys_err(__func__, 427, "setsockopt: sndbuf: %s", strerror(errno));
	return;
    }

    opt         = 0;
    optlen      = sizeof(opt);

    if (getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*) &opt, &optlen) < 0)
    {
	_sys_err(__func__, 436, "getsockopt: sndbuf: %s", strerror(errno));
	return;
    }

    sys_log(1, "SYSTEM: %d: send buffer changed to %d", s, opt);
}

// sec : seconds, usec : microseconds
void socket_timeout(socket_t s, long sec, long usec)
{
    struct timeval      rcvopt;
    struct timeval      sndopt;
    socklen_t		optlen = sizeof(rcvopt);

    rcvopt.tv_sec = sndopt.tv_sec = sec;
    rcvopt.tv_usec = sndopt.tv_usec = usec;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*) &rcvopt, optlen) < 0)
    {
	_sys_err(__func__, 460, "setsockopt: timeout: %s", strerror(errno));
	socket_close(s);
	return;
    }

    if (getsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*) &rcvopt, &optlen) < 0)
    {
	_sys_err(__func__, 467, "getsockopt: timeout: %s", strerror(errno));
	socket_close(s);
	return;
    }

    optlen = sizeof(sndopt);

    if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*) &sndopt, optlen) < 0)
    {
	_sys_err(__func__, 476, "setsockopt: timeout: %s", strerror(errno));
	socket_close(s);
	return;
    }

    if (getsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*) &sndopt, &optlen) < 0)
    {
	_sys_err(__func__, 483, "getsockopt: timeout: %s", strerror(errno));
	socket_close(s);
	return;
    }

    sys_log(1, "SYSTEM: %d: TIMEOUT RCV: %d.%d, SND: %d.%d", s, rcvopt.tv_sec, rcvopt.tv_usec, sndopt.tv_sec, sndopt.tv_usec);
}

void socket_reuse(socket_t s)
{
    int opt = 1;

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*) &opt, sizeof(opt)) < 0)
    {
	_sys_err(__func__, 497, "setsockopt: reuse: %s", strerror(errno));
	socket_close(s);
	return;
    }
}

void socket_keepalive(socket_t s)
{
    int opt = 1;

    if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char*) &opt, sizeof(opt)) < 0)
    {
	perror("setsockopt: keepalive");
	socket_close(s);
	return;
    }
}



