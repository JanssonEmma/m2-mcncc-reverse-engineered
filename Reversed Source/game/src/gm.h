#ifndef MCNCC_GAME_GM_H
#define MCNCC_GAME_GM_H
#include "../../common/tables.h"

void gm_new_clear();
void gm_new_insert(const tAdminInfo& info);
void gm_new_host_inert(const char* host);
BYTE gm_new_get_level(const char* name, const char* host);
BYTE gm_get_level(const char* name, const char* host);
#endif
