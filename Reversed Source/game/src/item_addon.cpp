#include "item_addon.h"
#include "item.h"
#include "utils.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
// Original complete empty derived lifecycle; actual singleton base still runs.
// Constructor variants0816e6c0/0816e720, dtor0816e780/0816e7d0/0816e820.
CItemAddonManager::CItemAddonManager() {}
CItemAddonManager::~CItemAddonManager() {}
// 0816e880. iAddonType is unused in this target, including unknown values.
void CItemAddonManager::ApplyAddonTo(int iAddonType,CItem* pItem) {
    (void)iAddonType;
    if(!pItem) {
        _sys_err("ApplyAddonTo",17,"ITEM pointer null");
        return;
    }
    // The 2009 x87 caller adds 0.5 before truncating, with no float store.
    const double roll = gauss_random_extended(0.0f, 5.0f) + 0.5;
    int skill = MINMAX(-30, static_cast<int>(roll), 30);
    int normal=-2*skill;
    if(skill>=-20 && skill<=20) {
        int a=number_ex(-8,8,const_cast<char*>("item_addon.cpp"),26);
        int b=number_ex(-8,8,const_cast<char*>("item_addon.cpp"),26);
        int sum=a+b;
        if(sum<0) sum=-sum;
        normal+=sum;
        normal+=number_ex(1,4,const_cast<char*>("item_addon.cpp"),26);
    } else {
        normal+=number_ex(1,5,const_cast<char*>("item_addon.cpp"),28);
    }
    pItem->RemoveAttributeType(71);
    pItem->RemoveAttributeType(72);
    pItem->AddAttribute(72,static_cast<short>(normal));
    pItem->AddAttribute(71,static_cast<short>(skill));
}
