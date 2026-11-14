#include "char_manager.h"
#include "db.h"
#include "locale.hpp"
#include <cstdint>
#include "mining.h"
#include "char.h"
#include "config.h"
#include "item.h"
#include "item_manager.h"
#include "log.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"

namespace mining {
struct OreInfo { std::uint32_t vein, raw, refined; };
OreInfo info[13] = {
    {20047, 50601, 50621},
    {20048, 50602, 50622},
    {20049, 50603, 50623},
    {20050, 50604, 50624},
    {20051, 50605, 50625},
    {20052, 50606, 50626},
    {20053, 50607, 50627},
    {20054, 50608, 50628},
    {20055, 50609, 50629},
    {20056, 50610, 50630},
    {20057, 50611, 50631},
    {20058, 50612, 50632},
    {20059, 50613, 50633},
};
int fraction_info[9][3] = {
    {20, 1, 10},
    {30, 11, 20},
    {20, 21, 30},
    {15, 31, 40},
    {5, 41, 50},
    {4, 51, 60},
    {3, 61, 70},
    {2, 71, 80},
    {1, 81, 90},
};
int PickGradeAddPct[10] = {3, 5, 8, 11, 15, 20, 26, 32, 40, 50};
int SkillLevelAddPct[41] = {
    0, 10, 10, 10, 10, 12, 12, 12, 12, 12, 14, 14, 14, 14, 14,
    16, 16, 16, 16, 16, 20, 20, 20, 20, 20, 25, 25, 25, 25, 25,
    30, 30, 30, 30, 30, 35, 35, 35, 35, 35, 45,
};

int GetFractionCount()
{
    int roll = number_ex(1, 100, const_cast<char*>("mining.cpp"), 0x61);
    for (const auto& fraction : fraction_info)
    {
        const int weight = fraction[0];
        if (roll <= weight) {
            const int upper = fraction[2];
            const int lower = fraction[1];
            return number_ex(lower, upper, const_cast<char*>("mining.cpp"), 0x65);
        }
        roll -= weight;
    }
    return 0;
}

DWORD GetRefineFromRawOre(DWORD raw)
{
    const auto vnum = static_cast<std::uint32_t>(raw);
    for (const OreInfo& ore : info)
        if (ore.raw == vnum)
            return ore.refined;
    return 0;
}

int GetOrePct(CHARACTER* character)
{
    CItem* pick = character->GetWear(4);
    if (!pick || !Pick_Check(*pick))
        return 0;

    // The 2009 executable passes zero as the mining skill level.
    const int skillBonus = SkillLevelAddPct[MINMAX(0, 0, 40)];
    const int refineLevel = pick->GetRefineLevel();
    const auto baseChance = 30u + static_cast<std::uint32_t>(skillBonus);
    const int grade = MINMAX(0, refineLevel, 9);
    return static_cast<std::int32_t>(baseChance + static_cast<std::uint32_t>(PickGradeAddPct[grade]));
}

void Pick_IncCurExp(CItem& pick)
{
    const auto current = static_cast<std::uint32_t>(Pick_GetCurExp(pick));
    pick.SetSocket(0, static_cast<std::int32_t>(current + 1u));
}

void Pick_MaxCurExp(CItem& pick)
{
    pick.SetSocket(0, Pick_GetMaxExp(pick));
}

bool Pick_IsPracticeSuccess(CItem& pick)
{
    return number_ex(1, pick.GetValue(1), const_cast<char*>("mining.cpp"), 0xc0) == 1;
}

void OreDrop(CHARACTER* ch, DWORD vein)
{
    const DWORD raw = GetRawOreFromLoad(vein);
    if (!GetFractionCount())
    {
        _sys_err("OreDrop", 0x74, "Wrong ore fraction count");
        return;
    }
    CItem* item = ITEM_MANAGER::instance().CreateItem(raw, GetFractionCount(), 0, false, -1);
    if (!item)
    {
        _sys_err("OreDrop", 0x7b, "cannot create item vnum %d", static_cast<int>(raw));
        return;
    }

    const auto baseX = static_cast<std::uint32_t>(ch->GetX());
    const int offsetX = number_ex(-200, 200, const_cast<char*>("mining.cpp"), 0x80);
    const auto x = baseX + offsetX;
    const auto baseY = static_cast<std::uint32_t>(ch->GetY());
    const int offsetY = number_ex(-200, 200, const_cast<char*>("mining.cpp"), 0x81);
    const auto y = baseY + offsetY;
    const PIXEL_POSITION position = {static_cast<std::int32_t>(x), static_cast<std::int32_t>(y), 0};
    item->AddToGround(static_cast<std::int32_t>(ch->GetMapIndex()), position);
    item->StartDestroyEvent();
    item->SetOwnership(ch, 15);
    const auto* proto = item->GetProto();
    const int count = static_cast<std::int32_t>(item->GetCount());
    const DWORD vnum = proto ? proto->dwVnum : 0;
    DBManager::instance().SendMoneyLog(8, vnum, count);
}

void PracticePick(CHARACTER* character, CItem* item)
{
    if (!item || !Pick_Check(*item))
        return;
    CHARACTER& ch = *character;
    CItem& pick = *item;
    const TItemTable* proto = pick.GetProto();
    if (!proto || !proto->dwRefinedVnum || !Pick_IsPracticeSuccess(pick))
        return;

    if (!Pick_Refinable(pick))
    {
        Pick_IncCurExp(pick);
        const int maxExp = Pick_GetMaxExp(pick);
        const int currentExp = Pick_GetCurExp(pick);
        const char* message = locale_find("\260\356\261\252\300\314\300\307\040\274\366\267\303\265\265\260\241\040\301\365\260\241\307\317\277\264\275\300\264\317\264\331\041\040\050\045\144\057\045\144\051", g_iUseLocale);
        ch.ChatPacket(1, message, currentExp, maxExp);
        if (!Pick_Refinable(pick))
            return;
    }
    ch.ChatPacket(1, locale_find("\260\356\261\252\300\314\260\241\040\303\326\264\353\040\274\366\267\303\265\265\277\241\040\265\265\264\336\307\317\277\264\275\300\264\317\264\331\056", g_iUseLocale));
    ch.ChatPacket(1, locale_find("\263\252\271\253\262\333\270\246\040\305\353\307\330\040\264\331\300\275\040\267\271\272\247\300\307\040\260\356\261\252\300\314\267\316\040\276\367\261\327\267\271\300\314\265\345\040\307\322\040\274\366\040\300\326\275\300\264\317\264\331\056", g_iUseLocale));
}

namespace {
struct MiningInfo { std::uint32_t pid, vein; };

}

long mining_event(LPEVENT event)
{
    const auto* info = static_cast<const MiningInfo*>(event->info);
    CHARACTER* ch = CHARACTER_MANAGER::instance().FindByPID(info->pid);
    CHARACTER* vein = CHARACTER_MANAGER::instance().Find(info->vein);
    if (!ch)
        return 0;

    ch->mining_take();
    CItem* pick = ch->GetWear(4);
    if (!pick || !Pick_Check(*pick))
        ch->ChatPacket(1, locale_find("\260\356\261\252\300\314\270\246\040\265\351\260\355\040\300\326\301\366\040\276\312\276\306\274\255\040\304\266\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
    else if (!vein)
        ch->ChatPacket(1, locale_find("\264\365\300\314\273\363\040\304\263\263\276\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
    else
    {
        const int chance = GetOrePct(ch);
        if (number_ex(1, 100, const_cast<char*>("mining.cpp"), 0x153) <= chance)
        {
            OreDrop(ch, vein->GetRaceNum());
            ch->ChatPacket(1, locale_find("\303\244\261\244\277\241\040\274\272\260\370\307\317\277\264\275\300\264\317\264\331\056", g_iUseLocale));
        }
        else
            ch->ChatPacket(1, locale_find("\303\244\261\244\277\241\040\275\307\306\320\307\317\277\264\275\300\264\317\264\331\056", g_iUseLocale));
        PracticePick(ch, pick);
    }
    return 0;
}

DWORD GetRawOreFromLoad(DWORD vein)
{
    const auto vnum = static_cast<std::uint32_t>(vein);
    for (const OreInfo& ore : info)
        if (ore.vein == vnum)
            return ore.raw;
    return 0;
}

LPEVENT CreateMiningEvent(CHARACTER* ch, CHARACTER* vein, int count)
{
    void* raw = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(MiningInfo), &raw);
    auto* info = static_cast<MiningInfo*>(raw);
    info->pid = ch->GetPlayerID();
    info->vein = static_cast<DWORD>(vein->GetVID());
    const auto delay = static_cast<std::int32_t>(static_cast<std::uint32_t>(count) * passes_per_sec * 2u);
    return event_create_ex(mining_event, id, delay,
        const_cast<char*>("mining.cpp"), 0x169);
}

bool OreRefine(CHARACTER* ch, CHARACTER* npc, CItem* ore, int cost, int chance, CItem* stone)
{
    if (!ch || !npc || !ore)
        return false;
    if (ore->GetOwner() != ch)
    {
        _sys_err("OreRefine", 0x173, "wrong owner");
        return false;
    }
    if (ore->GetCount() < 100)
    {
        _sys_err("OreRefine", 0x179, "not enough count");
        return false;
    }

    const DWORD refined = GetRefineFromRawOre(ore->GetVnum());
    if (!refined)
        return false;
    ch->SetRefineNPC(npc);
    ore->SetCount(ore->GetCount() - 100);
    const int fee = ch->ComputeRefineFee(cost, 1);
    if (ch->GetGold() < fee)
        return false;

    ch->PayRefineFee(fee);
    if (stone)
        ITEM_MANAGER::instance().RemoveItem(stone, "REMOVE (MELT)");
    if (number_ex(1, 100, const_cast<char*>("mining.cpp"), 0x18d) > chance)
        return false;
    ch->AutoGiveItem(refined, 1, -1, true);
    return true;
}

bool Pick_Check(CItem& pick)
{
    return pick.GetType() == 24;
}

int Pick_GetCurExp(CItem& pick)
{
    return static_cast<std::int32_t>(pick.GetSockets()[0]);
}

int Pick_GetMaxExp(CItem& pick)
{
    return static_cast<std::int32_t>(pick.GetValue(2));
}

bool Pick_Refinable(CItem& pick)
{
    const int currentExp = Pick_GetCurExp(pick);
    const int maxExp = Pick_GetMaxExp(pick);
    return currentExp >= maxExp;
}

bool Pick_IsRefineSuccess(CItem& pick)
{
    const int roll = number_ex(1, 100, const_cast<char*>("mining.cpp"), 0xc4);
    return roll <= static_cast<std::int32_t>(pick.GetValue(3));
}

void CHEAT_MAX_PICK(CHARACTER* character, CItem* pick)
{
    if (!pick || !Pick_Check(*pick))
        return;
    Pick_MaxCurExp(*pick);
    const int currentExp = Pick_GetCurExp(*pick);
    const char* message = LC_TEXT("\260\356\261\252\300\314 \274\366\267\303\265\265\260\241 \303\326\264\353(%d)\260\241 \265\307\276\372\275\300\264\317\264\331.");
    character->ChatPacket(1, message, currentExp);
}

int RealRefinePick(LPCHARACTER character, LPITEM pick)
{
    if (!character || !pick || pick->GetOwner() != character)
        return 2;

    auto& logs = LogManager::instance();
    auto& items = ITEM_MANAGER::instance();
    if (!Pick_Check(*pick)) {
        const char* name = pick->GetName();
        _sys_err("RealRefinePick", 0xd0, "REFINE_PICK_HACK pid(%lu) item(%s:%d) type(%d)",
                 character->GetPlayerID(), name ? name : "", static_cast<int>(pick->GetID()), pick->GetType());
        logs.Query("INSERT INTO refinelog%s VALUES(%lu, '%s', %lu, %d, NOW(), 1, 'PICK_HACK')",
                   get_table_postfix(), character->GetPlayerID(), name ? name : "", pick->GetID(), -1);
        return 2;
    }
    if (!Pick_Refinable(*pick))
        return 2;

    const auto stage = static_cast<std::int32_t>(pick->GetValue(0)) / 10;
    const bool success = Pick_IsRefineSuccess(*pick);
    const auto itemId = pick->GetID();
    const char* name = pick->GetName();
    const auto playerId = character->GetPlayerID();
    const char* tablePostfix = get_table_postfix();
    logs.Query("INSERT INTO refinelog%s VALUES(%lu, '%s', %lu, %d, NOW(), %d, 'PICK')",
               tablePostfix, playerId, name, itemId, stage, success);
    const auto replacementVnum = success
        ? static_cast<std::uint32_t>(pick->GetRefinedVnum())
        : static_cast<std::uint32_t>(pick->GetValue(4));
    LPITEM replacement = ITEM_MANAGER::instance().CreateItem(replacementVnum, 1, 0, false, -1);
    if (!replacement)
        return 2;

    const BYTE cell = pick->GetCell();
    items.RemoveItem(pick, "REMOVE (REFINE PICK)");
    replacement->AddToCharacter(character, cell);
    const char* replacementName = replacement->GetName();
    if (success) {
        const auto replacementId = replacement->GetID();
        const auto x = character->GetX();
        const auto y = character->GetY();
        const auto pid = character->GetPlayerID();
        logs.ItemLog(pid, x, y, replacementId, "REFINE PICK SUCCESS", replacementName);
        return 1;
    }
    const auto y = character->GetY();
    const auto replacementId = replacement->GetID();
    const auto pid = character->GetPlayerID();
    const auto x = character->GetX();
    logs.ItemLog(pid, x, y, replacementId, "REFINE PICK FAIL", replacementName);
    return 0;
}
}
