#ifndef MCNCC_BLOCK_COUNTRY_H
#define MCNCC_BLOCK_COUNTRY_H

#include "../../common/length.h"
#include <cstdint>

struct TPacketBlockCountryIp {
    std::uint32_t ip_from;
    std::uint32_t ip_to;
};

struct TPacketBlockException {
    std::uint8_t cmd;
    char login[LOGIN_MAX_LEN + 1];
};

static_assert(sizeof(TPacketBlockCountryIp) == 8, "DB country IP block packet");
static_assert(sizeof(TPacketBlockException) == 32, "DB country block exception packet");

void add_blocked_country_ip(TPacketBlockCountryIp* data);
void block_exception(TPacketBlockException* data);
bool is_block_exception(const char* login);
bool is_blocked_country_ip(const char* user_ip);

#endif
