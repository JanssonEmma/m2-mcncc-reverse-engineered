#ifndef MCNCC_GAME_IP_BAN_H
#define MCNCC_GAME_IP_BAN_H

#include <netinet/in.h>

bool LoadBanIP(const char* filename);
bool IsBanIP(in_addr address);

#endif
