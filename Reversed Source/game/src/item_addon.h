#ifndef MCNCC_GAME_ITEM_ADDON_H
#define MCNCC_GAME_ITEM_ADDON_H
#include "../../common/singleton.h"
class CItem;
class CItemAddonManager : public singleton<CItemAddonManager> {
public:
    CItemAddonManager();
    virtual ~CItemAddonManager();
    void ApplyAddonTo(int iAddonType,CItem* pItem);
};
#endif
