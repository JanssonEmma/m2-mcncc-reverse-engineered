// Partial recovery: add other typedefs as their dependent modules are recovered.
#ifndef MCNCC_LIBTHECORE_TYPEDEF_H
#define MCNCC_LIBTHECORE_TYPEDEF_H

// DWARF event.c DIE 0x37cd -> unsigned long, 4 bytes on the target ABI.
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;
// text_file_loader.cpp DWARF: BOOL is signed int,4 bytes.
typedef int BOOL;
// log.cpp DWARF INT typedef used by CAsyncSQL::CopyQuery.
typedef int INT;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
// socket.c DWARF: signed int, four bytes on the original ABI.
typedef int socket_t;

#endif

