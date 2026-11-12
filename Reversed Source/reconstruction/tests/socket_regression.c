// Full socket.c linked against deterministic syscall/DNS fixtures only.
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "../../libthecore/include/socket.h"
#include "../../libthecore/include/memcpy.h"
#include "socket_hooks.h"

int socket_test_errno;
typedef struct io_step { int result, error; } io_step;
typedef struct option_call { char op; int fd, name; socklen_t length; long first, second; } option_call;
static io_step tx[16];
static int tx_count, tx_index, rx_result, rx_error, udp_result;
static const char* tx_base;
static size_t tx_offsets[16], tx_lengths[16];
static char trace[256];
static int trace_length, error_calls, error_line, log_calls, log_level, perror_calls;
static char error_function[64], error_format[160], error_message[256], log_format[160], perror_message[64];
static int socket_result, socket_kind, bind_result, listen_result, accept_result, connect_result;
static int close_count, closed[16], close_error;
static int dns_fail, dns_calls, inet_calls, copied;
static const char* resolved_host;
static struct sockaddr_in bound_address, connected_address;
static struct sockaddr* udp_peer;
static socklen_t* udp_peer_length;
static size_t udp_size, recv_size;
static option_call options[32];
static int option_count, fail_option_at, fcntl_get, fcntl_set_result, fcntl_set_flags;
static unsigned int numeric_ip = 0x04030201U, dns_ip = 0x08070605U;
static char* dns_addresses[2];
static struct hostent dns_entry;
static void record(char op) { assert(trace_length < 255); trace[trace_length++] = op; trace[trace_length] = 0; }
static void* fixture_copy(void* dest, const void* source, size_t length) {
    assert(length == 4); ++copied; return memcpy(dest, source, length);
}
void* (*thecore_memcpy)(void*, const void*, size_t) = fixture_copy;
char* sk_strerror(int error) { assert(error == socket_test_errno); return "fixture error"; }
void _sys_err(const char* function, int line, const char* format, ...) {
    va_list ap; record('E'); ++error_calls; error_line = line;
    strcpy(error_function, function); strcpy(error_format, format);
    va_start(ap, format); vsnprintf(error_message, sizeof(error_message), format, ap); va_end(ap);
}
void sys_log(int level, const char* format, ...) {
    record('L'); ++log_calls; log_level = level; strcpy(log_format, format);
}
void sk_perror(const char* text) { record('P'); ++perror_calls; strcpy(perror_message, text); }
int sk_socket(int family, int kind, int protocol) {
    assert(family == AF_INET && protocol == 0); record('S'); socket_kind = kind;
    if (socket_result < 0) socket_test_errno = EINVAL;
    return socket_result;
}
unsigned int sk_inet_addr(const char* host) {
    record('I'); ++inet_calls; resolved_host = host; return numeric_ip;
}
unsigned short sk_htons(unsigned short value) {
    record('H'); return (unsigned short)((value << 8) | (value >> 8));
}
int sk_bind(int fd, const struct sockaddr* addr, socklen_t size) {
    assert(fd == socket_result && size == 16); record('B');
    memcpy(&bound_address, addr, size);
    if (bind_result < 0) socket_test_errno = EINVAL;
    return bind_result;
}
int sk_listen(int fd, int backlog) {
    assert(fd == socket_result && backlog == 128); record('N'); return listen_result;
}
int sk_accept(int fd, struct sockaddr* peer, socklen_t* size) {
    assert(fd == 7 && peer && *size == 16); record('A'); *size = 12;
    if (accept_result < 0) socket_test_errno = EAGAIN;
    return accept_result;
}
int sk_connect(int fd, const struct sockaddr* addr, socklen_t size) {
    assert(fd == socket_result && size == 16); record('C'); memcpy(&connected_address, addr, size);
    if (connect_result < 0) socket_test_errno = ECONNREFUSED;
    return connect_result;
}
int sk_close(int fd) {
    assert(close_count < 16); record('X'); closed[close_count++] = fd;
    if (close_error) socket_test_errno = close_error;
    return -1; // The wrapper ignores close errors.
}
struct hostent* sk_gethostbyname(const char* host) {
    record('D'); ++dns_calls; resolved_host = host; return dns_fail ? NULL : &dns_entry;
}
ssize_t sk_recv(int fd, void* dest, size_t length, int flags) {
    assert(fd == 7 && dest && !flags); record('R'); recv_size = length;
    socket_test_errno = rx_error;
    if (rx_result > 0) memset(dest, 'r', (size_t)rx_result);
    return rx_result;
}
ssize_t sk_recvfrom(int fd, void* dest, size_t length, int flags, struct sockaddr* peer, socklen_t* size) {
    assert(fd == 7 && dest && !flags); record('U');
    udp_peer = peer; udp_peer_length = size; udp_size = length; *size = 9;
    return udp_result;
}
ssize_t sk_send(int fd, const void* data, size_t length, int flags) {
    assert(fd == 7 && !flags && tx_index < tx_count); record('W');
    tx_offsets[tx_index] = (size_t)((const char*)data - tx_base);
    tx_lengths[tx_index] = length;
    socket_test_errno = tx[tx_index].error;
    return tx[tx_index++].result;
}
int sk_fcntl(int fd, int op, ...) {
    va_list ap; int value;
    va_start(ap, op); value = va_arg(ap, int); va_end(ap);
    assert(fd == socket_result || fd == accept_result || fd == 7);
    if (op == F_GETFL) { assert(value == 0); record('F'); return fcntl_get; }
    assert(op == F_SETFL); record('f'); fcntl_set_flags = value;
    if (fcntl_set_result < 0) socket_test_errno = EINVAL;
    return fcntl_set_result;
}
int sk_setsockopt(int fd, int level, int name, const void* value, socklen_t size) {
    option_call* out;
    assert(level == SOL_SOCKET && option_count < 32 && value); record('T');
    out = &options[option_count++]; out->op = 'T'; out->fd = fd; out->name = name; out->length = size;
    if (name == SO_RCVTIMEO || name == SO_SNDTIMEO) {
        struct timeval copy; assert(size == 8); memcpy(&copy, value, sizeof(copy));
        out->first = copy.tv_sec; out->second = copy.tv_usec;
    } else if (name == SO_LINGER) {
        struct linger copy; assert(size == 8); memcpy(&copy, value, sizeof(copy));
        out->first = copy.l_onoff; out->second = copy.l_linger;
    } else {
        assert(size == 4);
        // DONTROUTE deliberately passes indeterminate stack bytes in the original.
        if (name != SO_DONTROUTE) { int copy; memcpy(&copy, value, 4); out->first = copy; }
    }
    if (option_count == fail_option_at) { socket_test_errno = EINVAL; return -1; }
    return 0;
}
int sk_getsockopt(int fd, int level, int name, void* value, socklen_t* size) {
    option_call* out;
    assert(level == SOL_SOCKET && option_count < 32 && value); record('G');
    out = &options[option_count++]; out->op = 'G'; out->fd = fd; out->name = name; out->length = *size;
    if (option_count == fail_option_at) { socket_test_errno = EINVAL; return -1; }
    if (name == SO_RCVTIMEO || name == SO_SNDTIMEO) {
        struct timeval* tv = (struct timeval*)value; assert(*size == 8);
        tv->tv_sec += 1; tv->tv_usec += 2; *size = 4; // Next send option must reset length.
    } else {
        unsigned int* v = (unsigned int*)value; assert(*size == 4 && *v == 0);
        *v = 65432; *size = 4;
    }
    return 0;
}
static void reset(void) {
    memset(trace, 0, sizeof(trace)); memset(options, 0, sizeof(options));
    trace_length = error_calls = error_line = log_calls = log_level = perror_calls = 0;
    close_count = close_error = dns_fail = dns_calls = inet_calls = copied = 0;
    option_count = fail_option_at = fcntl_set_result = bind_result = listen_result = connect_result = 0;
    tx_index = tx_count = rx_result = rx_error = udp_result = 0;
    socket_result = 20; accept_result = 30; fcntl_get = 0x40; socket_test_errno = 0;
    dns_addresses[0] = (char*)&dns_ip; dns_addresses[1] = NULL;
    memset(&dns_entry, 0, sizeof(dns_entry)); dns_entry.h_addr_list = dns_addresses;
    error_function[0] = error_format[0] = log_format[0] = perror_message[0] = 0;
}
static void step(int result, int error) { assert(tx_count < 16); tx[tx_count].result = result; tx[tx_count++].error = error; }
static void check_io(void) {
    char bytes[32] = "abcdef"; struct sockaddr peer; socklen_t peer_length = sizeof(peer);
    int transient[4] = {EINTR, EAGAIN, EWOULDBLOCK, EDEADLK}, i;
    reset(); rx_result = 3; assert(socket_read(7, bytes, 32) == 3 && recv_size == 32 && !memcmp(bytes, "rrr", 3));
    reset(); assert(socket_read(7, bytes, 0) == -1 && !error_calls && recv_size == 0);
    for (i = 0; i < 4; ++i) {
        reset(); rx_result = -1; rx_error = transient[i];
        assert(socket_read(7, bytes, 32) == 0 && !error_calls && !strcmp(trace, "R"));
    }
    reset(); rx_result = -1; rx_error = ECONNRESET;
    assert(socket_read(7, bytes, 32) == -1 && error_line == 58);
    assert(!strcmp(error_format, "about to lose connection"));
    reset(); udp_result = -1; assert(socket_udp_read(7, bytes, 13, &peer, &peer_length) == -1);
    assert(udp_peer == &peer && udp_peer_length == &peer_length && peer_length == 9 && udp_size == 13 && !error_calls);
    reset(); tx_base = bytes; step(2, 0); assert(socket_write_tcp(7, bytes, 6) == 2);
    reset(); tx_base = bytes; step(0, 0); assert(socket_write_tcp(7, bytes, 6) == -1 && !error_calls);
    for (i = 1; i < 4; ++i) {
        reset(); tx_base = bytes; step(-1, transient[i]); assert(socket_write_tcp(7, bytes, 6) == 0);
    }
    reset(); tx_base = bytes; step(-1, EINTR); assert(socket_write_tcp(7, bytes, 6) == -1);
    reset(); tx_base = bytes; step(-1, EAGAIN); step(2, 0); step(-1, EDEADLK); step(4, 0);
    assert(socket_write(7, bytes, 6) == 0 && tx_index == 4);
    assert(tx_offsets[0] == 0 && tx_offsets[1] == 0 && tx_offsets[2] == 2 && tx_offsets[3] == 2);
    assert(tx_lengths[0] == 6 && tx_lengths[1] == 6 && tx_lengths[2] == 4 && tx_lengths[3] == 4);
    reset(); tx_base = bytes; step(0, EAGAIN);
    assert(socket_write(7, bytes, 0) == -1 && tx_index == 1 && error_line == 109);
    reset(); tx_base = bytes; step(-1, EINTR);
    assert(socket_write(7, bytes, 6) == -1 && error_line == 111 && !close_count);
}
static void check_bind_accept(void) {
    struct sockaddr_in peer; int i;
    reset(); assert(socket_tcp_bind("1.2.3.4", 65537) == 20);
    assert(socket_kind == SOCK_STREAM && !strcmp(trace, "STTIHBFfLN"));
    assert(bound_address.sin_len == 0 && bound_address.sin_family == 2 && bound_address.sin_port == 0x0100);
    assert(bound_address.sin_addr.s_addr == numeric_ip && fcntl_set_flags == 0x44);
    for (i = 0; i < 8; ++i) assert(bound_address.sin_zero[i] == 0);
    assert(!strcmp(log_format, "SYSTEM: BINDING TCP PORT ON [%d]"));
    reset(); listen_result = -1; assert(socket_tcp_bind("bad", 5) == 20 && !close_count);
    reset(); assert(socket_udp_bind("1.2.3.4", 4) == 20 && socket_kind == SOCK_DGRAM);
    assert(!strcmp(trace, "STTIHBFfL"));
    reset(); socket_result = -1; assert(socket_tcp_bind("unused", 2) == -1 && error_line == 137 && !option_count && !close_count);
    reset(); bind_result = -1; assert(socket_tcp_bind("ip", 2) == -1 && error_line == 151 && !close_count);
    reset(); fail_option_at = 1; assert(socket_tcp_bind("ip", 2) == 20 && close_count == 1 && closed[0] == 20);
    assert(strchr(trace, 'B')); // bind continues after reuse closed its descriptor.
    reset(); assert(socket_accept(7, &peer) == 30 && !strcmp(trace, "AFfT"));
    reset(); accept_result = -1; assert(socket_accept(7, &peer) == -1 && error_line == 196 && !close_count);
    reset(); accept_result = 65499; assert(socket_accept(7, &peer) == 65499);
    reset(); accept_result = 65500; assert(socket_accept(7, &peer) == -1 && error_line == 202);
    assert(close_count == 1 && closed[0] == 7 && !option_count);
    reset(); socket_close(7); assert(close_count == 1 && closed[0] == 7);
}
static void check_connect(void) {
    int i;
    reset(); assert(socket_connect("1.2.3.4", 1234) == 20);
    assert(inet_calls == 1 && !dns_calls && !copied && !strcmp(trace, "IHSTTGLTGLTGTGLTC"));
    assert(connected_address.sin_len == 0 && connected_address.sin_family == AF_INET);
    assert(connected_address.sin_port == 0xd204 && connected_address.sin_addr.s_addr == numeric_ip);
    assert(option_count == 10 && options[0].name == SO_KEEPALIVE);
    assert(options[1].name == SO_SNDBUF && options[1].first == 233016);
    assert(options[3].name == SO_RCVBUF && options[3].first == 233016);
    assert(options[5].name == SO_RCVTIMEO && options[5].first == 10 && !options[5].second);
    assert(options[7].name == SO_SNDTIMEO && options[7].length == 8 && options[7].first == 10);
    assert(options[9].name == SO_LINGER && options[9].first == 1 && !options[9].second);
    reset(); assert(socket_connect("server.example", 1) == 20 && dns_calls == 1 && !inet_calls && copied == 1);
    assert(connected_address.sin_addr.s_addr == dns_ip);
    reset(); assert(socket_connect("1hostname", 1) == 20 && inet_calls == 1 && !dns_calls);
    reset(); { char host[] = {(char)0x80, 'A', 0}; assert(socket_connect(host, 1) == 20 && dns_calls == 1); }
    reset(); dns_fail = 1; assert(socket_connect("missing", 9) == -1 && error_line == 229);
    assert(!strcmp(trace, "DE") && !perror_calls);
    reset(); socket_result = -1; assert(socket_connect("1", 9) == -1 && !strcmp(perror_message, "socket") && !error_calls);
    reset(); connect_result = -1; close_error = EINTR;
    assert(socket_connect("1", 9) == -1 && close_count == 1 && closed[0] == 20);
    assert(error_line == 285 && !strcmp(error_format, "HOST %s:%d, could not connect."));
    assert(!strcmp(perror_message, "connect"));
    for (i = 1; i <= 10; ++i) {
        reset(); fail_option_at = i;
        assert(socket_connect("1", 9) == 20 && strchr(trace, 'C'));
        // Void option helpers do not stop the connect sequence after failure.
    }
}
static void check_options(void) {
    int i;
    reset(); socket_nonblock(7); assert(fcntl_set_flags == 0x44 && !error_calls);
    reset(); fcntl_get = 0x45; socket_block(7); assert(fcntl_set_flags == 0x41);
    reset(); fcntl_get = -1; socket_nonblock(7); assert(fcntl_set_flags == -1 && !error_calls);
    reset(); fcntl_set_result = -1; socket_block(7); assert(error_calls == 1 && !close_count);
    reset(); socket_lingeroff(7); assert(options[0].first == 0 && options[0].second == 0);
    reset(); socket_lingeron(7); assert(options[0].first == 1 && options[0].second == 0);
    reset(); socket_reuse(7); assert(options[0].first == 1 && options[0].name == SO_REUSEADDR);
    reset(); socket_keepalive(7); assert(options[0].first == 1 && options[0].name == SO_KEEPALIVE);
    reset(); socket_dontroute(7); assert(options[0].name == SO_DONTROUTE && options[0].length == 4);
    reset(); fail_option_at = 1; socket_dontroute(7); assert(close_count == 1);
    reset(); fail_option_at = 1; socket_lingeroff(7); assert(close_count == 1);
    reset(); fail_option_at = 1; socket_lingeron(7); assert(close_count == 1);
    reset(); fail_option_at = 1; socket_reuse(7); assert(close_count == 1);
    reset(); fail_option_at = 1; socket_keepalive(7); assert(close_count == 1 && perror_calls == 1 && !error_calls);
    reset(); socket_rcvbuf(7, 123); assert(option_count == 2 && options[0].first == 123 && log_level == 1);
    reset(); socket_sndbuf(7, 456); assert(option_count == 2 && options[0].first == 456 && log_level == 1);
    for (i = 1; i <= 2; ++i) {
        reset(); fail_option_at = i; socket_rcvbuf(7, 123); assert(close_count == 1 && option_count == i);
        reset(); fail_option_at = i; socket_sndbuf(7, 123); assert(!close_count && option_count == i);
    }
    reset(); socket_timeout(7, -2, 1500000);
    assert(option_count == 4 && options[0].first == -2 && options[0].second == 1500000);
    assert(options[2].first == -2 && options[2].second == 1500000 && log_level == 1);
    for (i = 1; i <= 4; ++i) {
        reset(); fail_option_at = i; socket_timeout(7, 2, 3);
        assert(close_count == 1 && option_count == i && error_calls == 1);
    }
}
int main(void) {
    check_io(); check_bind_accept(); check_connect(); check_options();
    puts("socket regression passed: I/O, retries, addresses, bind/accept/connect and option failures");
    return 0;
}
