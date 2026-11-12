#ifndef MCNCC_GAME_FISHING_H
#define MCNCC_GAME_FISHING_H

class CHARACTER;
class CItem;

#include "event.h"

namespace fishing {
void Initialize();
void UseFish(CHARACTER* character, CItem* item);
void Grill(CHARACTER* character, CItem* item);
struct fishing_event_info;
LPEVENT CreateFishingEvent(CHARACTER* character);
void Take(fishing_event_info* info, CHARACTER* character);
int DetermineFishByProbIndex(int probability);
void Simulation(int level, int count, int probability, CHARACTER* character);
int GetProbIndexByMapIndex(int map);
int DetermineFish(CHARACTER* character);
bool RefinableRod(CItem* rod);
int RealRefineRod(CHARACTER* character, CItem* rod);
}

#endif
