#include "shop.h"
#include "db.h"
#include "locale.hpp"
#include "locale_service.h"
#include "log.h"
#include "char.h"
#include "desc.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include <cstdint>
#include <cstdio>
#include "packet.h"
#include "questmanager.h"
#include "../../libgame/include/grid.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/memcpy.h"
#include <cstring>

namespace {
void CopyShopSockets(void* destination, const long* sockets)
{
    if (sizeof(long) == sizeof(std::int32_t)) {
        thecore_memcpy(destination, sockets, 3 * sizeof(std::int32_t));
    } else {
        const std::int32_t packedSockets[] = {
            static_cast<std::int32_t>(sockets[0]),
            static_cast<std::int32_t>(sockets[1]),
            static_cast<std::int32_t>(sockets[2]),
        };
        thecore_memcpy(destination, packedSockets, sizeof(packedSockets));
    }
}
}

CShop::CShop() : m_dwVnum(0), m_pkPC(0) {
    m_pGrid = new CGrid(5, 9);
}

CShop::~CShop() {
    const TPacketGCShop packet = {38, sizeof(TPacketGCShop), 1};
    Broadcast(&packet, sizeof(packet));
    for (GuestMapType::iterator it = m_map_guest.begin(); it != m_map_guest.end(); ++it)
        it->first->SetShop(0);
    delete m_pGrid;
}

void CShop::Broadcast(const void* data, int bytes) {
    sys_log(0, "Shop::Broadcast %p %d", data, bytes);
    for (GuestMapType::iterator it = m_map_guest.begin(); it != m_map_guest.end(); ++it) {
        if (DESC* desc = it->first->GetDesc())
            desc->Packet(data, bytes);
    }
}

bool CShop::AddGuest(CHARACTER* character, DWORD ownerVid, bool otherEmpire) {
    if (!character || character->GetExchange() || character->GetShop())
        return false;

    character->SetShop(this);
    m_map_guest.insert(GuestMapType::value_type(character, otherEmpire));

    const TPacketGCShop header = {38, sizeof(TPacketGCShop) + sizeof(TPacketGCShopStart), 0};
    TPacketGCShopStart start;
    std::memset(&start, 0, sizeof(start));
    start.owner_vid = ownerVid;
    for (DWORD index = 0; index < m_itemVector.size() && index < 40; ++index) {
        const SHOP_ITEM& item = m_itemVector[index];
        if (!quest::CQuestManager::instance().GetEventFlag("hivalue_item_sell") &&
            (item.vnum == 70024 || item.vnum == 70035))
            continue;
        if (m_pkPC && !item.pkItem)
            continue;

        TPacketGCShopItem& entry = start.items[index];
        entry.vnum = item.vnum;
        entry.price = static_cast<std::uint32_t>(item.price);
        if (otherEmpire)
            entry.price *= 3U;
        entry.count = item.count;
        if (item.pkItem) {
            const long* sockets = item.pkItem->GetSockets();
            CopyShopSockets(entry.alSockets, sockets);
            thecore_memcpy(entry.aAttr, item.pkItem->GetAttributes(), sizeof(entry.aAttr));
        }
    }

    character->GetDesc()->BufferedPacket(&header, sizeof(header));
    character->GetDesc()->Packet(&start, sizeof(start));
    return true;
}

// Original 080d3340 and 080d3350.
void CShop::SetPCShop(CHARACTER* character) {
    m_pkPC = character;
}

bool CShop::Create(DWORD vnum, DWORD npcVnum, TShopItemTable* table) {
    sys_log(0, "SHOP #%d (Shopkeeper %d)", static_cast<int>(vnum), static_cast<int>(npcVnum));
    m_dwVnum = vnum;
    m_dwNPCVnum = npcVnum;
    BYTE count = 0;
    while (count < 40 && table[count].vnum)
        ++count;
    SetShopItems(table, count);
    return true;
}

// Original 080d33d0. The table advances only after a successful placement.
void CShop::SetShopItems(TShopItemTable* table, BYTE count) {
    if (count > 40)
        return;

    m_pGrid->Clear();
    m_itemVector.resize(40);
    std::memset(&m_itemVector[0], 0, 40 * sizeof(SHOP_ITEM));
    for (int index = 0; index < count; ++index) {
        CItem* item = 0;
        const TItemTable* proto;
        if (m_pkPC) {
            item = m_pkPC->GetItem(table->pos);
            if (!item) {
                _sys_err("SetShopItems", 408, "cannot find item on %u (name: %s)",
                         static_cast<unsigned int>(table->pos), m_pkPC->GetName());
                continue;
            }
            proto = item->GetProto();
        } else {
            if (!table->vnum)
                continue;
            proto = ITEM_MANAGER::instance().GetTable(table->vnum);
        }
        if (!proto) {
            _sys_err("SetShopItems", 424, "Shop: no item table by item vnum #%d",
                     static_cast<int>(table->vnum));
            continue;
        }

        int position;
        if (m_pkPC) {
            position = table->display_pos;
            sys_log(0, "MyShop: use position %d", position);
        } else {
            position = m_pGrid->FindBlank(1, proto->bSize);
        }
        if (position < 0) {
            _sys_err("SetShopItems", 440, "not enough shop window");
            continue;
        }
        if (!m_pGrid->IsEmpty(position, 1, proto->bSize)) {
            if (m_pkPC)
                _sys_err("SetShopItems", 448, "not empty position for pc shop %s[%d]",
                         m_pkPC->GetName(), static_cast<int>(m_pkPC->GetPlayerID()));
            else
                _sys_err("SetShopItems", 452, "not empty position for npc shop");
            continue;
        }
        m_pGrid->Put(position, 1, proto->bSize);
        SHOP_ITEM& slot = m_itemVector[position];
        slot.pkItem = item;
        if (item) {
            slot.vnum = item->GetVnum();
            slot.count = static_cast<BYTE>(item->GetCount());
            slot.price = static_cast<std::int32_t>(table->price);
        } else {
            slot.vnum = table->vnum;
            slot.count = table->count;
            const bool countPerGold = (proto->dwFlags & 8) != 0;
            const auto gold = static_cast<std::uint32_t>(proto->dwGold);
            if (!countPerGold)
                slot.price = static_cast<std::int32_t>(slot.count * gold);
            else
                slot.price = gold ? slot.count / gold : slot.count;
        }
        char name[37];
        std::snprintf(name, 36, "%-20s(#%-5d) (x %d)", proto->szName,
                      static_cast<int>(slot.vnum), static_cast<int>(slot.count));
        sys_log(0, "SHOP_ITEM: %-36s PRICE %-5d", name, static_cast<int>(slot.price));
        ++table;
    }
}

// Original 080d4890.
void CShop::RemoveGuest(CHARACTER* character) {
    if (character->GetShop() != this)
        return;
    m_map_guest.erase(character);
    character->SetShop(0);
    const TPacketGCShop packet = {38, sizeof(TPacketGCShop), 1};
    character->GetDesc()->Packet(&packet, sizeof(packet));
}

// Original 080d49a0.
void CShop::BroadcastUpdateItem(BYTE position) {
    TEMP_BUFFER buffer;
    const TPacketGCShop header = {38, sizeof(TPacketGCShop) + sizeof(TPacketGCShopUpdateItem), 2};
    TPacketGCShopUpdateItem update{};
    update.pos = position;
    if (position >= m_itemVector.size())
        return;

    CItem* item = m_itemVector[position].pkItem;
    update.item.vnum = m_pkPC && !item ? 0 : m_itemVector[position].vnum;
    if (item) {
        CopyShopSockets(update.item.alSockets, item->GetSockets());
        if (position >= m_itemVector.size())
            return;
        item = m_itemVector[position].pkItem;
        if (!item)
            return;
        thecore_memcpy(update.item.aAttr, item->GetAttributes(), sizeof(update.item.aAttr));
        if (position >= m_itemVector.size())
            return;
    }
    update.item.price = static_cast<std::uint32_t>(m_itemVector[position].price);
    update.item.count = m_itemVector[position].count;
    buffer.write(&header, sizeof(header));
    buffer.write(&update, sizeof(update));
    const int bytes = buffer.size();
    Broadcast(buffer.read_peek(), bytes);
}


extern void VCardUse(CHARACTER* owner, CHARACTER* taker, CItem* item);

static const char* ShopItemName(CItem* item) {
    const TItemTable* proto = item->GetProto();
    return proto ? proto->szLocaleName : 0;
}

// Original 080d37d0. Treasury transfers are outside the reconstruction scope.
int CShop::Buy(CHARACTER* character, BYTE position) {
    sys_log(0, "CShop::Buy pc ? or no : name %s pos %d  ", character->GetName(), static_cast<int>(position));
    if (position >= m_itemVector.size()) {
        sys_log(0, "CShop::Buy : Invalid position %d : %s", static_cast<int>(position), character->GetName());
        return 8;
    }

    const GuestMapType::iterator guest = m_map_guest.find(character);
    if (guest == m_map_guest.end())
        return 1;
    SHOP_ITEM& slot = m_itemVector[position];
    std::uint32_t price = static_cast<std::uint32_t>(slot.price);
    if (guest->second)
        price *= 3U;

    const int gold = character->GetGold();
    if (gold < static_cast<int>(price)) {
        sys_log(1, "CShop::Buy : Not enough money : %s has %d, price %d",
                character->GetName(), gold, static_cast<int>(price));
        return 5;
    }

    CItem* item = m_pkPC ? slot.pkItem :
        ITEM_MANAGER::instance().CreateItem(slot.vnum, slot.count, 0, false, -1);
    if (!item)
        return 9;

    if (!m_pkPC && !quest::CQuestManager::instance().GetEventFlag("hivalue_item_sell") &&
        (item->GetVnum() == 70024 || item->GetVnum() == 70035))
        return 1;

    const int empty = character->GetEmptyInventory(item->GetSize());
    if (empty < 0) {
        if (m_pkPC) {
            sys_log(1, "CShop::Buy at PC Shop : Inventory full : %s size %d",
                    character->GetName(), static_cast<int>(item->GetSize()));
        } else {
            sys_log(1, "CShop::Buy : Inventory full : %s size %d",
                    character->GetName(), static_cast<int>(item->GetSize()));
            ITEM_MANAGER::instance().DestroyItem(item);
        }
        return 7;
    }

    character->PointChange(11, static_cast<int>(0UL - price), false, false);
    const char* taxFlag = (LC_IsYMIR() || LC_IsKorea() || LC_IsCIBN()) ? "trade_tax" : "personal_shop";
    const int taxRate = quest::CQuestManager::instance().GetEventFlag(taxFlag) ? 3 : 0;
    if (taxRate)
        price -= (price * 3U) / 100U;

    if (!m_pkPC) {
        item->AddToCharacter(character, static_cast<BYTE>(empty));
        ITEM_MANAGER::instance().FlushDelayedSave(item);
        LogManager::instance().ItemLog(character->GetPlayerID(), character->GetX(), character->GetY(),
                                       item->GetID(), "BUY", ShopItemName(item));
        if (item->GetVnum() > 80002 && item->GetVnum() < 80008)
            LogManager::instance().GoldBarLog(character->GetPlayerID(), item->GetID(), PERSONAL_SHOP_BUY, "");
        DBManager::instance().SendMoneyLog(2, item->GetVnum(), static_cast<int>(0UL - price));
    } else {
        m_pkPC->SyncQuickslot(1, item->GetCell(), 255);
        const DWORD vnum = item->GetVnum();
        if (vnum == 90008 || vnum == 90009) {
            VCardUse(m_pkPC, character, item);
            item = 0;
        } else {
            char log[52];
            if (vnum > 80002 && vnum < 80008) {
                std::snprintf(log, 51, "%s FROM: %lu TO: %lu PRICE: %lu", ShopItemName(item),
                              character->GetPlayerID(), m_pkPC->GetPlayerID(), static_cast<unsigned long>(price));
                LogManager::instance().GoldBarLog(character->GetPlayerID(), item->GetID(), SHOP_BUY, log);
                LogManager::instance().GoldBarLog(m_pkPC->GetPlayerID(), item->GetID(), SHOP_SELL, log);
            }
            item->RemoveFromCharacter();
            item->AddToCharacter(character, static_cast<BYTE>(empty));
            ITEM_MANAGER::instance().FlushDelayedSave(item);
            std::snprintf(log, 51, "%s %lu(%s) %lu %lu", ShopItemName(item),
                          m_pkPC->GetPlayerID(), m_pkPC->GetName(), static_cast<unsigned long>(price), item->GetCount());
            LogManager::instance().ItemLog(character->GetPlayerID(), character->GetX(), character->GetY(),
                                           item->GetID(), "SHOP_BUY", log);
            std::snprintf(log, 51, "%s %lu(%s) %lu %lu", ShopItemName(item),
                          character->GetPlayerID(), character->GetName(), static_cast<unsigned long>(price), item->GetCount());
            LogManager::instance().ItemLog(m_pkPC->GetPlayerID(), m_pkPC->GetX(), m_pkPC->GetY(),
                                           item->GetID(), "SHOP_SELL", log);
        }
        slot.pkItem = 0;
        BroadcastUpdateItem(position);
        m_pkPC->PointChange(11, static_cast<int>(price), false, false);
        m_pkPC->ChatPacket(1, LC_TEXT("\306\307\270\305\261\335\276\327\300\307\040\045\144\040\045\045\040\260\241\040\274\274\261\335\300\270\267\316\040\263\252\260\241\260\324\265\313\264\317\264\331"), taxRate);
    }

    if (item)
        sys_log(0, "SHOP: BUY: name %s %s(x %d):%lu price %lu", character->GetName(),
                ShopItemName(item), static_cast<int>(item->GetCount()), item->GetID(), static_cast<unsigned long>(price));
    character->Save();
    return 4;
}
