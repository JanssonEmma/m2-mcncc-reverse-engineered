#include "shop_manager.h"
#include "config.h"
#include "db.h"
#include "desc.h"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"
#include "packet.h"
#include "char.h"
#include "locale.hpp"
#include "utils.h"
#include "../../libthecore/include/log.h"
#include <cstdint>

CShopManager::CShopManager() {}

CShopManager::~CShopManager() {
    Destroy();
}
// Original 080d2590. Only the VNUM map owns these shop pointers.
void CShopManager::Destroy() {
    for (std::map<DWORD, CShop*>::iterator it = m_map_pkShop.begin();
         it != m_map_pkShop.end(); ++it)
        delete it->second;
    m_map_pkShop.clear();
}

// Original 080d2630.
LPSHOP CShopManager::Get(DWORD vnum) {
    const std::map<DWORD, CShop*>::iterator it = m_map_pkShop.find(vnum);
    return it == m_map_pkShop.end() ? 0 : it->second;
}

// Original 080d2680.
LPSHOP CShopManager::GetByNPCVnum(DWORD vnum) {
    const std::map<DWORD, CShop*>::iterator it = m_map_pkShopByNPCVnum.find(vnum);
    return it == m_map_pkShopByNPCVnum.end() ? 0 : it->second;
}

// Original 080d26d0. The locale lookup key retains the original ELF bytes.
bool CShopManager::StartShopping(CHARACTER* character, CHARACTER* keeper, int shopVnum) {
    if (keeper->GetDesc())
        return false;

    if (character->IsOpenSafebox() || character->GetExchange() ||
        character->GetMyShop() || character->IsCubeOpen()) {
        character->ChatPacket(1, LC_TEXT(
            "\264\331\270\245 \260\305\267\241\303\242\300\314 \277\255\270\260"
            "\273\363\305\302\277\241\274\255\264\302 \273\363\301\241\260\305\267"
            "\241\270\246 \307\322\274\366 \260\241 \276\370\275\300\264\317"
            "\264\331."));
        return false;
    }

    const int dx = static_cast<int>(static_cast<unsigned int>(character->GetX()) -
                                    static_cast<unsigned int>(keeper->GetX()));
    const int dy = static_cast<int>(static_cast<unsigned int>(character->GetY()) -
                                    static_cast<unsigned int>(keeper->GetY()));
    const int distance = DISTANCE_APPROX(dx, dy);
    if (distance >= 1000) {
        sys_log(1, "SHOP: TOO_FAR: %s distance %d", character->GetName(), distance);
        return false;
    }

    CShop* shop = shopVnum ? Get(static_cast<std::uint32_t>(shopVnum)) : GetByNPCVnum(keeper->GetRaceNum());
    if (!shop) {
        sys_log(1, "SHOP: NO SHOP");
        return false;
    }

    shop->AddGuest(character, keeper->GetVID(), character->GetEmpire() != keeper->GetEmpire());
    character->SetShopOwner(keeper);
    sys_log(0, "SHOP: START: %s", character->GetName());
    return true;
}

// Original 080d24a0.
bool CShopManager::Initialize(TShopTable* table, int size) {
    if (!m_map_pkShop.empty())
        return false;
    for (int index = 0; index < size; ++index, ++table) {
        CShop* shop = new CShop;
        if (shop->Create(table->dwVnum, table->dwNPCVnum, table->items)) {
            m_map_pkShop.insert(TShopMap::value_type(table->dwVnum, shop));
            m_map_pkShopByNPCVnum.insert(TShopMap::value_type(table->dwNPCVnum, shop));
        } else {
            delete shop;
        }
    }
    return true;
}

LPSHOP CShopManager::FindPCShop(DWORD vid) {
    const TShopMap::iterator it = m_map_pkShopByPC.find(vid);
    return it == m_map_pkShopByPC.end() ? 0 : it->second;
}

LPSHOP CShopManager::CreatePCShop(CHARACTER* character, TShopItemTable* table, BYTE count) {
    if (FindPCShop(character->GetVID()))
        return 0;
    CShop* shop = new CShop;
    shop->SetPCShop(character);
    shop->SetShopItems(table, count);
    m_map_pkShopByPC.insert(TShopMap::value_type(character->GetVID(), shop));
    return shop;
}

void CShopManager::DestroyPCShop(CHARACTER* character) {
    CShop* shop = FindPCShop(character->GetVID());
    if (!shop)
        return;
    character->SetMyShopTime();
    m_map_pkShopByPC.erase(character->GetVID());
    delete shop;
}

void CShopManager::StopShopping(CHARACTER* character) {
    CShop* shop = character->GetShop();
    if (!shop)
        return;
    character->SetMyShopTime();
    shop->RemoveGuest(character);
    sys_log(0, "SHOP: END: %s", character->GetName());
}


extern bool IsSelledItem(CHARACTER* character, int id);

static int ShopDistance(CHARACTER* character, CHARACTER* keeper) {
    return DISTANCE_APPROX(
        static_cast<int>(static_cast<unsigned int>(character->GetX()) - static_cast<unsigned int>(keeper->GetX())),
        static_cast<int>(static_cast<unsigned int>(character->GetY()) - static_cast<unsigned int>(keeper->GetY())));
}

// Original 080d2a20.
void CShopManager::Buy(CHARACTER* character, BYTE position) {
    if (!character->GetShop() || !character->GetShopOwner())
        return;
    if (ShopDistance(character, character->GetShopOwner()) > 2000) {
        character->ChatPacket(1, LC_TEXT("\273\363\301\241\260\372\300\307\040\260\305\270\256\260\241\040\263\312\271\253\040\270\326\276\356\040\271\260\260\307\300\273\040\273\354\040\274\366\040\276\370\275\300\264\317\264\331\056"));
        return;
    }
    character->SetMyShopTime();
    const int result = character->GetShop()->Buy(character, position);
    if (result != 4) {
        const TPacketGCShop packet = {38, sizeof(TPacketGCShop), static_cast<BYTE>(result)};
        character->GetDesc()->Packet(&packet, sizeof(packet));
    }
}

// Original 080d2b10. Treasury transfers are outside the reconstruction scope.
void CShopManager::Sell(CHARACTER* character, BYTE cell, BYTE count) {
    if (!character->GetShop() || !character->GetShopOwner() ||
        !character->CanHandleItem(false, false) || character->GetShop()->IsPCShop())
        return;
    if (ShopDistance(character, character->GetShopOwner()) > 2000) {
        character->ChatPacket(1, LC_TEXT("\273\363\301\241\260\372\300\307\040\260\305\270\256\260\241\040\263\312\271\253\040\270\326\276\356\040\271\260\260\307\300\273\040\306\310\040\274\366\040\276\370\275\300\264\317\264\331\056"));
        return;
    }

    CItem* item = character->GetItem(cell);
    if (!item)
        return;
    const TItemTable* proto = item->GetProto();
    if (proto && (proto->dwAntiFlags & 0x100))
        return;
    if (!count || item->GetCount() < count)
        count = static_cast<BYTE>(item->GetCount());

    const auto unit = static_cast<std::uint32_t>(item->GetShopBuyPrice());
    std::uint32_t price = !(item->GetFlag() & 8) ? count * unit : (unit ? count / unit : count);
    if (!LC_IsYMIR() && !LC_IsKorea())
        LC_IsCIBN();
    price /= 5;
    price -= (price * 3U) / 100U;

    if (test_server) {
        const DWORD id = item->GetID();
        const char* name = character->GetName();
        sys_log(0, "Sell Item price id %d %s itemid %d", static_cast<int>(character->GetPlayerID()),
                name, static_cast<int>(id));
    }
    if (price >= 50001 && !IsSelledItem(character, static_cast<int>(item->GetID()))) {
        _sys_err("Sell", 297, "[HACK_ITEM] id %lu name %s itemid %lu", character->GetPlayerID(),
                 character->GetName(), item->GetID());
        return;
    }
    const int gold = character->GetGold();
    if (price + static_cast<std::uint32_t>(gold) > 1999999999U) {
        _sys_err("Sell", 305, "[OVERFLOW_GOLD] id %lu name %s gold %lu", character->GetPlayerID(),
                 character->GetName(), static_cast<unsigned long>(static_cast<std::uint32_t>(gold)));
        character->ChatPacket(1, LC_TEXT("\062\060\276\357\263\311\300\314\040\303\312\260\372\307\317\277\251\040\271\260\307\260\300\273\040\306\310\274\366\040\276\370\275\300\264\317\264\331\056"));
        return;
    }

    proto = item->GetProto();
    sys_log(0, "SHOP: SELL: %s item name: %s(x%d):%lu price: %lu", character->GetName(),
            proto ? proto->szLocaleName : 0, static_cast<int>(count), item->GetID(), static_cast<unsigned long>(price));
    character->ChatPacket(1, LC_TEXT("\306\307\270\305\261\335\276\327\300\307\040\045\144\040\045\045\040\260\241\040\274\274\261\335\300\270\267\316\040\263\252\260\241\260\324\265\313\264\317\264\331"), 3);
    DBManager::instance().SendMoneyLog(2, item->GetVnum(), static_cast<int>(price));
    if (count == item->GetCount())
        ITEM_MANAGER::instance().RemoveItem(item, "SELL");
    else
        item->SetCount(item->GetCount() - count);
    character->PointChange(11, static_cast<int>(price), false, false);
}
