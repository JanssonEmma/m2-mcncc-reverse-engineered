#include "block_country.h"
#include "dev_log.h"

#include <arpa/inet.h>
#include <set>
#include <string>
#include <vector>

namespace {
std::vector<TPacketBlockCountryIp> s_blocked_ip;
std::set<std::string> s_block_exception;

void __add_block_exception(const char* login)
{
    dev_log("block_country.cpp", 0x2a, "__add_block_exception", L_DEB0,
            const_cast<char*>("BLOCK_EXCEPTION_ADD : %s"), login);
    s_block_exception.insert(login);
}

void __del_block_exception(const char* login)
{
    dev_log("block_country.cpp", 0x3a, "__del_block_exception", L_DEB0,
            const_cast<char*>("BLOCK_EXCEPTION_DEL : %s"), login);
    s_block_exception.erase(login);
}
}

void add_blocked_country_ip(TPacketBlockCountryIp* data)
{
    s_blocked_ip.push_back(*data);
    dev_log("block_country.cpp", 0x56, "add_blocked_country_ip", L_DEB0,
            const_cast<char*>("BLOCKED_IP = %u - %u"), data->ip_from, data->ip_to);
}

void block_exception(TPacketBlockException* data)
{
    if (!data)
        return;
    switch (data->cmd) {
        case 1:
            __add_block_exception(data->login);
            break;
        case 2:
            __del_block_exception(data->login);
            break;
    }
}

bool is_blocked_country_ip(const char* user_ip)
{
    in_addr address;
    if (!inet_aton(user_ip, &address)) {
        dev_log("block_country.cpp", 0x76, "is_blocked_country_ip", L_INFO,
                const_cast<char*>("BLOCKED_COUNTRY_IP (%s) : YES"), user_ip);
        return true;
    }

    const std::uint32_t ip = ntohl(address.s_addr);
    for (const TPacketBlockCountryIp& range : s_blocked_ip) {
        if (range.ip_from <= ip && ip <= range.ip_to) {
            dev_log("block_country.cpp", 0x80, "is_blocked_country_ip", L_INFO,
                    const_cast<char*>("BLOCKED_COUNTRY_IP (%s) : YES"), user_ip);
            return true;
        }
    }
    dev_log("block_country.cpp", 0x85, "is_blocked_country_ip", L_INFO,
            const_cast<char*>("BLOCKED_COUNTRY_IP (%s) : NO"), user_ip);
    return false;
}

bool is_block_exception(const char* login)
{
    return s_block_exception.find(login) != s_block_exception.end();
}
