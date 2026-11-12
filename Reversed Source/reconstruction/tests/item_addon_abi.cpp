#include "../../game/src/item.h"
#include "../../game/src/item_addon.h"
#include "../../game/src/utils.h"
typedef char addon_scalar_gate[sizeof(long)==4?1:-1];
void CheckItemAddonAPI() {
    void (CItem::*apply)(int)=&CItem::ApplyAddon;
    bool (CItem::*remove)(BYTE)=&CItem::RemoveAttributeType;
    void (CItemAddonManager::*manager)(int,CItem*)=&CItemAddonManager::ApplyAddonTo;
    float (*gaussian)(float,float)=gauss_random;
    (void)apply;(void)remove;(void)manager;(void)gaussian;
}
