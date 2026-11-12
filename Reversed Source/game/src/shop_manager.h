#ifndef MCNCC_GAME_SHOP_MANAGER_H
#define MCNCC_GAME_SHOP_MANAGER_H

#include "../../common/singleton.h"
#include "shop.h"

typedef CShop* LPSHOP;

class CShopManager : public singleton<CShopManager> {
public:
    typedef std::map<DWORD, CShop*> TShopMap;

    CShopManager();
    virtual ~CShopManager();

    bool Initialize(TShopTable* table, int size);
    void Destroy();
    LPSHOP Get(DWORD vnum);
    LPSHOP GetByNPCVnum(DWORD vnum);
    bool StartShopping(CHARACTER* character, CHARACTER* keeper, int shopVnum = 0);
    void StopShopping(CHARACTER* character);
    void Buy(CHARACTER* character, BYTE position);
    void Sell(CHARACTER* character, BYTE cell, BYTE count = 0);
    LPSHOP CreatePCShop(CHARACTER* character, TShopItemTable* table, BYTE count);
    LPSHOP FindPCShop(DWORD vid);
    void DestroyPCShop(CHARACTER* character);

private:
    TShopMap m_map_pkShop;
    TShopMap m_map_pkShopByNPCVnum;
    TShopMap m_map_pkShopByPC;
};

#endif
