#ifndef MCNCC_GAME_SHOP_H
#define MCNCC_GAME_SHOP_H

#include "../../common/tables.h"
#include <stdint.h>
#include <map>
#include <vector>

class CHARACTER;
class CGrid;
class CItem;

class CShop {
public:
    typedef struct shop_item {
        DWORD vnum;
        int32_t price;
        BYTE count;
        CItem* pkItem;
    } SHOP_ITEM;

    CShop();
    ~CShop();

    bool Create(DWORD vnum, DWORD npcVnum, TShopItemTable* items);
    void SetShopItems(TShopItemTable* items, BYTE count);
    void SetPCShop(CHARACTER* character);
    bool IsPCShop() { return m_pkPC != 0; }
    bool AddGuest(CHARACTER* character, DWORD ownerVid, bool otherEmpire);
    void RemoveGuest(CHARACTER* character);
    int Buy(CHARACTER* character, BYTE position);
    void BroadcastUpdateItem(BYTE position);

protected:
    void Broadcast(const void* data, int bytes);

    typedef std::map<CHARACTER*, bool> GuestMapType;
    DWORD m_dwVnum;
    DWORD m_dwNPCVnum;
    CGrid* m_pGrid;
    GuestMapType m_map_guest;
    std::vector<SHOP_ITEM> m_itemVector;
    CHARACTER* m_pkPC;
};

#endif
