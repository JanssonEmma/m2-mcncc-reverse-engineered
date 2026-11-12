#ifndef MCNCC_CRC32_H
#define MCNCC_CRC32_H

#include <cstddef>
typedef unsigned long crc_t;

// Original names, as recorded in the 2009 symbol table.
crc_t get_crc32(const char* buffer, std::size_t count);
crc_t get_crc32_case(const char* buffer, std::size_t count);
crc_t get_fast_hash(const char* buffer, std::size_t count);

#endif
