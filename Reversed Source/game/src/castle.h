#ifndef MCNCC_GAME_CASTLE_H
#define MCNCC_GAME_CASTLE_H
#include "../../libthecore/include/typedef.h"

class CHARACTER;
bool castle_can_attack(CHARACTER* attacker, CHARACTER* victim);
bool castle_is_tower_vnum(DWORD vnum);
bool castle_is_my_castle(int empire, int mapIndex);
bool castle_is_guard_vnum(DWORD vnum);
int castle_cost_of_hiring_guard(DWORD groupVnum);
bool castle_boot();
int castle_guard_count(int empire, int region);
int castle_frog_count(int empire);
void castle_save();
CHARACTER* castle_spawn_frog(int empire);
CHARACTER* castle_spawn_frog_force(int empire, int slot);
CHARACTER* castle_spawn_guard(int empire, DWORD group, int region);
void castle_frog_die(CHARACTER* character, CHARACTER* killer);
void castle_guard_die(CHARACTER* character, CHARACTER* killer);
void castle_tower_die(CHARACTER* character, CHARACTER* killer);
bool castle_spawn_tower(int empire);
void castle_start_siege(int empire);
void castle_end_siege();
void castle_siege(int empire);

#endif
