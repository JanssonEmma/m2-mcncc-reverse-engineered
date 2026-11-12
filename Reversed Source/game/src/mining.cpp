#include "char_manager.h"
#include "db.h"
#include "locale.hpp"
#include <algorithm>
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
        if (roll <= fraction[0])
            return number_ex(fraction[1], fraction[2], const_cast<char*>("mining.cpp"), 0x65);
        roll -= fraction[0];
    }
    return 0;
}

DWORD GetRefineFromRawOre(DWORD raw)
{
    for (const OreInfo& ore : info)
        if (ore.raw == raw)
            return ore.refined;
    return 0;
}

int GetOrePct(CHARACTER* character)
{
    CItem* pick = character->GetWear(4);
    if (!pick || !Pick_Check(*pick))
        return 0;

    const int grade = std::clamp(pick->GetRefineLevel(), 0, 9);
    // The 2009 executable passes zero as the mining skill level.
    return 30 + SkillLevelAddPct[0] + PickGradeAddPct[grade];
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

    const auto x = static_cast<std::uint32_t>(ch->GetX()) +
        number_ex(-200, 200, const_cast<char*>("mining.cpp"), 0x80);
    const auto y = static_cast<std::uint32_t>(ch->GetY()) +
        number_ex(-200, 200, const_cast<char*>("mining.cpp"), 0x81);
    const PIXEL_POSITION position = {static_cast<std::int32_t>(x), static_cast<std::int32_t>(y), 0};
    item->AddToGround(ch->GetMapIndex(), position);
    item->StartDestroyEvent();
    item->SetOwnership(ch, 15);
    DBManager::instance().SendMoneyLog(8, item->GetVnum(), item->GetCount());
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
        ch.ChatPacket(1, locale_find("\260\356\261\252\300\314\300\307\040\274\366\267\303\265\265\260\241\040\301\365\260\241\307\317\277\264\275\300\264\317\264\331\041\040\050\045\144\057\045\144\051", g_iUseLocale),
            Pick_GetCurExp(pick), Pick_GetMaxExp(pick));
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
    CHARACTER_MANAGER& chars = CHARACTER_MANAGER::instance();
    CHARACTER* ch = chars.FindByPID(info->pid);
    CHARACTER* vein = chars.Find(info->vein);
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
    for (const OreInfo& ore : info)
        if (ore.vein == vein)
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
    return Pick_GetCurExp(pick) >= Pick_GetMaxExp(pick);
}

bool Pick_IsRefineSuccess(CItem& pick)
{
    const int roll = number_ex(1, 100, const_cast<char*>("mining.cpp"), 0xc4);
    return roll <= pick.GetValue(3);
}

void CHEAT_MAX_PICK(CHARACTER* character, CItem* pick)
{
    if (!pick || !Pick_Check(*pick))
        return;
    Pick_MaxCurExp(*pick);
    character->ChatPacket(1, LC_TEXT("\260\356\261\252\300\314 \274\366\267\303\265\265\260\241 \303\326\264\353(%d)\260\241 \265\307\276\372\275\300\264\317\264\331."), Pick_GetCurExp(*pick));
}

int RealRefinePick(LPCHARACTER character, LPITEM pick)
{
    if (!character || !pick || pick->GetOwner() != character)
        return 2;

    auto& logs = LogManager::instance();
    const char* name = pick->GetName();
    if (!Pick_Check(*pick)) {
        _sys_err("RealRefinePick", 0xd0, "REFINE_PICK_HACK pid(%lu) item(%s:%d) type(%d)",
                 character->GetPlayerID(), name ? name : "", static_cast<int>(pick->GetID()), pick->GetType());
        logs.Query("INSERT INTO refinelog%s VALUES(%lu, '%s', %lu, %d, NOW(), 1, 'PICK_HACK')",
                   get_table_postfix(), character->GetPlayerID(), name ? name : "", pick->GetID(), -1);
        return 2;
    }
    if (!Pick_Refinable(*pick))
        return 2;

    const int stage = static_cast<int>(pick->GetValue(0) / 10);
    const bool success = Pick_IsRefineSuccess(*pick);
    logs.Query("INSERT INTO refinelog%s VALUES(%lu, '%s', %lu, %d, NOW(), %d, 'PICK')",
               get_table_postfix(), character->GetPlayerID(), name, pick->GetID(), stage, success);
    const DWORD replacementVnum = success ? pick->GetProto()->dwRefinedVnum : pick->GetValue(4);
    auto& items = ITEM_MANAGER::instance();
    LPITEM replacement = items.CreateItem(replacementVnum, 1, 0, false, -1);
    if (!replacement)
        return 2;

    const BYTE cell = pick->GetCell();
    items.RemoveItem(pick, "REMOVE (REFINE PICK)");
    replacement->AddToCharacter(character, cell);
    logs.ItemLog(character->GetPlayerID(), character->GetX(), character->GetY(), replacement->GetID(),
                 success ? "REFINE PICK SUCCESS" : "REFINE PICK FAIL", replacement->GetName());
    return success ? 1 : 0;
}
}
