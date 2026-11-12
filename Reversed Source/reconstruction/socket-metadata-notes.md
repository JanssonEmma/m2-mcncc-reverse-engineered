# Socket DWARF metadata

Observed by read-only pyelftools extraction from `PROJECT_ROOT\Original Binaries\game`, SHA256 `61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c`. No original program execution. Full DIE attributes and qualifier chains retained in socket-metadata.json, with reproducible extract_socket_metadata.py. Scope: the socket.c compilation unit only. DWARF is source/type evidence; operational behavior still requires instruction/decompilation evidence.

Twenty emitted module functions, all DW_AT_external=true:

```c
int socket_udp_read(socket_t desc, char *read_point, size_t space_left,
                    struct sockaddr *from, socklen_t *fromlen); // 081b6250
int socket_read(socket_t desc, char *read_point, size_t space_left); // 081b6290
int socket_write_tcp(socket_t desc, const char *txt, int length); // 081b6330
int socket_write(socket_t desc, const char *data, size_t length); // 081b63a0
int socket_bind(const char *ip, int port, int protocol); // 081b6440
int socket_tcp_bind(const char *ip, int port); // 081b6580
int socket_udp_bind(const char *ip, int port); // 081b65b0
void socket_close(socket_t s); // 081b65e0
socket_t socket_accept(socket_t s, struct sockaddr_in *peer); // 081b65f0
socket_t socket_connect(const char *host, WORD port); // 081b66b0
void socket_nonblock(socket_t s); // 081b68d0
void socket_block(socket_t s); // 081b6950
void socket_dontroute(socket_t s); // 081b69d0
void socket_lingeroff(socket_t s); // 081b6a50
void socket_lingeron(socket_t s); // 081b6ae0
void socket_rcvbuf(socket_t s, unsigned int opt); // 081b6b70
void socket_sndbuf(socket_t s, unsigned int opt); // 081b6c90
void socket_timeout(socket_t s, long sec, long usec); // 081b6da0
void socket_reuse(socket_t s); // 081b6f80
void socket_keepalive(socket_t s); // 081b7010
```

Absent return-type DIE denotes void. Helper inline definitions __bswap16 and __isctype are retained in metadata but excluded from the twenty module functions. Supplied reference header declares only fifteen functions; the old target emits the other five as external functions too (socket_write_tcp, socket_bind, socket_timeout, socket_reuse, socket_keepalive).

Target scalar ABI: socket_t=int (signed,4), size_t=unsigned int (4), socklen_t=unsigned int (4), ssize_t=int (signed,4), WORD=unsigned short (2), pointer=4, long=int-width signed4. DWARF encoding5=signed;6=signed_char;7=unsigned;8=unsigned_char.

| Type | Size | Members (offset:type) |
|---|---:|---|
| sockaddr |16|sa_len 0:unsigned char; sa_family 1:unsigned char (sa_family_t); sa_data 2:char[14]|
| sockaddr_in |16|sin_len 0:uint8_t; sin_family 1:sa_family_t; sin_port 2:unsigned short (in_port_t); sin_addr 4:in_addr; sin_zero 8:char[8]|
| in_addr |4|s_addr 0:unsigned int (in_addr_t)|
| hostent |20|h_name 0:char*; h_aliases 4:char**; h_addrtype 8:int; h_length 12:int; h_addr_list 16:char**|
| linger |8|l_onoff 0:int; l_linger 4:int|
| timeval |8|tv_sec 0:long; tv_usec 4:long (suseconds_t)|

This is the BSD sockaddr layout with separate 8-bit length and family, not the Linux 16-bit family layout. Production source should use the target system headers; these values support only test shims and ABI checks.

The CU contains enum types pthread_mutextype, EFdwatch and ENUM_PROFILER. There are no socket-option, address-family, errno, fcntl or IP-protocol named constants in its DWARF. Their exact numeric operands must be recovered separately from assembly or binary-backed decompilation; do not infer macro values from the contemporary host headers. Full unrelated enum enumerator values are nevertheless retained under structures[].members[].attributes.const_value.
