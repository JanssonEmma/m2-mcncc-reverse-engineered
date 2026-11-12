#ifndef MCNCC_GAME_QUESTLUA_FORKED_H
#define MCNCC_GAME_QUESTLUA_FORKED_H

extern int g_iForkedVecIndex[2];
extern int g_bRegenFlag;

int LoadForked();
void InitForked();
int SetForkedMap(int pass, int sungzi);
int IsForkedMapIndex(int mapIndex);
int IsSungziMapIndex(int mapIndex);
#include "../../libthecore/include/typedef.h"

int GetSungziMapIndex();
int GetSungziStartX(BYTE empire);
int GetSungziStartY(BYTE empire);
int GetPassMapIndex(BYTE empire);
int GetPassStartX(BYTE empire);
int GetPassStartY(BYTE empire);
const char* GetSungziMapPath();
int get_mob_kill_count_per_empire(int empire);
int get_boss_kill_count();
int inc_boss_kill_count();
int inc_mob_kill_count_per_empire(int empire);
void init_dead_count(int pid);
void init_all_dead_count();
int get_dead_count(int pid);
int dec_dead_count(int pid);
int GetSungziBossVnum();
int get_kill_count_per_empire(int empire);
int set_kill_count_per_empire(int empire, int kills);
int inc_kill_count_per_empire(int empire, int kills);

#endif
