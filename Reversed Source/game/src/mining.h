#ifndef MCNCC_GAME_MINING_H
#define MCNCC_GAME_MINING_H

#include "event.h"

class CHARACTER;
class CItem;

namespace mining {
DWORD GetRawOreFromLoad(DWORD vein);
int GetFractionCount();
LPEVENT CreateMiningEvent(CHARACTER* ch, CHARACTER* vein, int count);
bool OreRefine(CHARACTER* ch, CHARACTER* npc, CItem* ore, int cost, int chance, CItem* stone);
bool Pick_Check(CItem& pick);
int Pick_GetCurExp(CItem& pick);
int Pick_GetMaxExp(CItem& pick);
bool Pick_Refinable(CItem& pick);
bool Pick_IsRefineSuccess(CItem& pick);
void CHEAT_MAX_PICK(CHARACTER* character, CItem* pick);
int RealRefinePick(CHARACTER* character, CItem* pick);
}

#endif
