#ifndef MCNCC_GAME_BLEND_ITEM_H
#define MCNCC_GAME_BLEND_ITEM_H
#include "../../common/tables.h"
class CItem;
#define MAX_BLEND_ITEM_VALUE 5
bool Blend_Item_init();
bool Blend_Item_load(char* file);
bool Blend_Item_set_value(CItem* item);
bool Blend_Item_find(DWORD item_vnum);
#endif
