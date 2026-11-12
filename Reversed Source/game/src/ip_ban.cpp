#include "ip_ban.h"
#include "../../libthecore/include/log.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

struct IP {
    bool Read(const char* text, unsigned char* destination);

    in_addr_t start;
    in_addr_t end;
    in_addr_t mask;
};

// 0815b9f0
bool IP::Read(const char* text, unsigned char* destination)
{
    in_addr address{};
    if (inet_pton(AF_INET, text, &address) != 1)
        return false;
    std::memcpy(destination, &address.s_addr, sizeof(address.s_addr));
    return true;
}

namespace {
std::map<int, std::vector<IP>> mapBanIP;

int FirstOctet(in_addr_t address)
{
    return static_cast<int>(ntohl(address) >> 24);
}
}

bool LoadBanIP(const char* filename)
{
    FILE* input = std::fopen(filename, "r");
    if (!input)
        return false;

    std::fprintf(stderr, "LOADING BANNED IP LIST\n");
    char line[256];
    while (std::fgets(line, sizeof(line), input)) {
        char* newline = std::strchr(line, '\n');
        if (!newline && !std::feof(input)) {
            int next;
            do {
                next = std::fgetc(input);
            } while (next != '\n' && next != EOF);
            continue;
        }
        line[std::strcspn(line, "\r\n")] = '\0';
        char* endText = std::strchr(line, '\t');
        if (endText) {
            *endText++ = '\0';
            if (char* comment = std::strchr(endText, '\t'))
                *comment = '\0';
        }

        IP rule{};
        if (!rule.Read(line, reinterpret_cast<unsigned char*>(&rule.start)))
            continue;
        rule.end = rule.start;
        if (endText && *endText && !rule.Read(endText, reinterpret_cast<unsigned char*>(&rule.end)))
            continue;

        rule.mask = static_cast<in_addr_t>(~(rule.start ^ rule.end));
        const in_addr start = {rule.start};
        const in_addr end = {rule.end};
        const in_addr mask = {rule.mask};
        const int first = FirstOctet(rule.start);
        std::fprintf(stderr, "\t%s", inet_ntoa(start));
        std::fprintf(stderr, "\t%s", inet_ntoa(end));
        std::fprintf(stderr, "\t%s\tfirst %d\n", inet_ntoa(mask), first);
        mapBanIP[first].push_back(rule);
    }
    std::fclose(input);
    return true;
}

bool IsBanIP(in_addr address)
{
    const int first = FirstOctet(address.s_addr);
    const auto found = mapBanIP.find(first);
    sys_log(0, "IsBanIP %d", first);
    if (found == mapBanIP.end())
        return false;

    for (const IP& rule : found->second) {
        if ((rule.start & rule.mask) == (address.s_addr & rule.mask))
            return true;
    }
    return false;
}
