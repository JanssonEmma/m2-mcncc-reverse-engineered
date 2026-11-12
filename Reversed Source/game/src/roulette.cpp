#include "roulette.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "locale.hpp"
#include "config.h"
#include "log.h"
#include "../../libthecore/include/log.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>

// 08085a60
int RandomRangeModulo(int low, int high)
{
    return low + std::rand() % (high - low + 1);
}

CRoulette::CRoulette() = default;
CRoulette::~CRoulette() = default;

// 08085d50. The original uses the loaded item table, not the filename.
bool CRoulette::LoadRouletteTableData(const char*)
{
    constexpr int levels = 14;
    const auto& items = ITEM_MANAGER::instance().m_map_vid;
    sys_log(0, "[CRoulette] AllItemSize [%d] ", static_cast<int>(items.size()));

    struct RouletteItem {
        int vnum;
        int price;
    };
    std::vector<RouletteItem> sorted;
    sorted.reserve(items.size());
    for (const auto& item : items)
        sorted.push_back({static_cast<int>(item.first), static_cast<int>(item.second.dwShopBuyPrice)});
    std::sort(sorted.begin(), sorted.end(), [](const RouletteItem& first, const RouletteItem& second) {
        return first.price < second.price;
    });

    for (const auto& row : sorted) {
        if (!GetItem(row.vnum)) {
            _sys_err("LoadRouletteTableData", 0x5e, "[CRoulette] Short of Roulette Item Data");
            return false;
        }
    }

    m_Info = tRouletteInfo{};
    const std::size_t width = (sorted.size() + levels - 1) / levels;
    std::size_t index = 0;
    for (int level = 0; level < levels && index < sorted.size(); ++level) {
        std::int64_t total = 0;
        for (std::size_t i = 0; i < width && index < sorted.size(); ++i, ++index) {
            const int vnum = sorted[index].vnum;
            total += GetItem(vnum)->dwShopBuyPrice;
            SetItem(level, vnum);
        }
        m_Info.m_nAvgPrice[level] = static_cast<int>(total / width);
        sys_log(0, "[CRoulette] Level %d AvgPrice %d count %d", level, m_Info.m_nAvgPrice[level],
                static_cast<int>(index));
    }
    sys_log(0, "[CRoulette] AllItemSize [%d] %d", static_cast<int>(sorted.size()), static_cast<int>(index));
    for (const auto& pool : m_Info.m_vec_vnum) {
        for (int vnum : pool) {
            const auto* item = GetItem(vnum);
            sys_log(0, "[CRoulette] Roulette Data VNum[%d] , Price[%d] Gold[%d]", vnum,
                    static_cast<int>(item->dwShopBuyPrice), static_cast<int>(item->dwGold));
        }
    }
    return true;
}

// 08086250
bool CRoulette::Roulette(CHARACTER* character, int money)
{
    if (character->IsObserverMode() || character->GetMyShop() || !character->CanHandleItem())
        return false;

    const int roll = RandomRangeModulo(0, 100);
    sys_log(0, "[%s]\267\352\267\277\277\241 \305\365\300\332\307\321 \265\267 %d", character->GetName(), money);
    character->ChatPacket(1, locale_find("%d \263\311\300\273 \305\365\300\332\307\317\274\314\275\300\264\317\264\331.", g_iUseLocale), money);
    if (roll < 20) {
        sys_log(0, "\265\267\300\273 \271\336\300\332");
        character->PointChange(POINT_GOLD, 10000);
        character->ChatPacket(1, locale_find("%d \263\311\300\273 \310\271\265\346\307\317\274\314\275\300\264\317\264\331.", g_iUseLocale), 10000);
        return true;
    }

    const int level = GetNewSourceLevel(GetSourceLevel(money));
    const TItemTable* row = GetRandomItem(level);
    if (!row) {
        sys_log(0, "[CRoulette] GetRandomItem() return NULL Money %d", money);
        return false;
    }
    CItem* prize = row->dwVnum == 1 ? nullptr : CreateItem(row->dwVnum, 1);
    if (!prize) {
        if (test_server)
            character->ChatPacket(1, "%s", locale_find("\301\270\300\347\307\317\301\366 \276\312\264\302 \276\306\300\314\305\333 \300\324\264\317\264\331", g_iUseLocale));
        return false;
    }

    const int cell = CheckEmptyInventory(character, prize);
    if (cell < 0 || !prize->AddToCharacter(character, static_cast<BYTE>(cell))) {
        ITEM_MANAGER::instance().DestroyItem(prize);
        character->ChatPacket(1, "%s", locale_find("\273\363\307\260\300\273 \271\336\300\273 \300\316\272\245\305\344\270\256\260\241 \272\316\301\267\307\325\264\317\264\331. \300\316\272\245\305\344\270\256\270\246 \272\361\277\366\301\326\274\274\277\344!", g_iUseLocale));
        return false;
    }
    LogManager::instance().ItemLog(character->GetPlayerID(), character->GetX(), character->GetY(),
                                  prize->GetID(), "Roulette", prize->GetName());
    character->ChatPacket(1, locale_find("%s \276\306\300\314\305\333 \300\273 \310\271\265\346\307\317\274\314\275\300\264\317\264\331", g_iUseLocale), prize->GetName());
    return true;
}

// 080865a0
bool CRoulette::Roulette(CHARACTER* character, CItem* item)
{
    Roulette(character, item->GetShopBuyPrice());
    return true;
}

// 080865d0
int CRoulette::GetSourceLevel(int price)
{
    for (int level = 0; level < 14; ++level) {
        if (m_Info.m_nAvgPrice[level] < price)
            return level;
    }
    return 0;
}

// 08086600
int CRoulette::GetNewSourceLevel(int sourceLevel)
{
    for (int i = 0; i < 5 && sourceLevel != 4; ++i) {
        if (RandomRangeModulo(0, 10000) < 2000)
            ++sourceLevel;
    }
    for (int i = 0; i < 5 && sourceLevel != 0; ++i) {
        if (RandomRangeModulo(0, 10000) < 3000)
            --sourceLevel;
    }
    return sourceLevel;
}

// 08086670
TItemTable* CRoulette::GetRandomItem(int sourceLevel)
{
    if (sourceLevel < 0 || sourceLevel >= 14) {
        sys_log(0, "[CRoulette] Get out of MAX_SOURCE_LEVEL Range");
        return nullptr;
    }
    const auto& pool = m_Info.m_vec_vnum[sourceLevel];
    const auto index = static_cast<std::size_t>(RandomRangeModulo(0, static_cast<int>(pool.size())));
    return index < pool.size() ? GetItem(pool[index]) : nullptr;
}

TItemTable* CRoulette::GetItem(int vnum)
{
    return ITEM_MANAGER::instance().GetTable(vnum);
}

void CRoulette::SetItem(int sourceLevel, int vnum)
{
    m_Info.m_vec_vnum[sourceLevel].push_back(vnum);
}

CItem* CRoulette::CreateItem(int vnum, int count)
{
    return ITEM_MANAGER::instance().CreateItem(vnum, count, 0, false, -1);
}

int CRoulette::CheckEmptyInventory(CHARACTER* character, CItem* item)
{
    const BYTE size = item->GetSize();
    const int cell = character->GetEmptyInventory(size);
    if (cell < 0) {
        sys_log(1, "[CRoulette] Inventory full : %s size %d", character->GetName(), size);
        return -1;
    }
    return cell;
}
