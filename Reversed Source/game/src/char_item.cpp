#include "../../libthecore/include/main.h"
#include "../../libthecore/include/hangul.h"
#include <algorithm>
#include "item_manager.h"
#include "refine.h"
#include "mob_manager.h"
#include "polymorph.h"
#include "skill.h"
#include "arena.h"
#include "marriage.h"
#include "dungeon.h"
#include "war_map.h"
#include <algorithm>
#include "fishing.h"
#include "blend_item.h"
#include "constants.h"
#include "vector.h"
#include <cstdint>
#include "char_manager.h"
#include "../../libthecore/include/utils.h"
#include "db.h"
#include "locale.hpp"
#include "locale_service.h"
#include "sectree_manager.h"
#include "questmanager.h"
#include "questpc.h"
#include "questpc.h"
#include "questlua_forked.h"
#include <cmath>
#include <cstdio>
#include "log.h"
// Old inventory storage and packet encoding, recovered from char_item.cpp.
#include "char.h"
#include "config.h"
#include "item.h"
#include "desc.h"
#include "desc_client.h"
#include "packet.h"
#include "party.h"
#include "utils.h"
#include "../../libthecore/include/log.h"
#include <cassert>
#include <utility>
#include <cstring>

bool IsPCBangIP(const char* ip);

void TransformRefineItem(CItem* source, CItem* result)
{
    const long* sockets = source->GetSockets();
    if (source->IsAccessoryForSocket())
    {
        for (int index = 0; index < 3; ++index)
            result->SetSocket(index, static_cast<std::int32_t>(sockets[index]));
        result->StartAccessorySocketExpireEvent();
    }
    else
    {
        for (int index = 0; index < 3 && static_cast<std::int32_t>(sockets[index]) != 0; ++index)
            result->SetSocket(index, 1);

        int destination = 0;
        for (int index = 0; index < 3; ++index)
        {
            const auto socket = static_cast<std::int32_t>(sockets[index]);
            if (socket > 2 && socket != 28960)
                result->SetSocket(destination++, socket);
        }
    }
    source->CopyAttributeTo(result);
}

namespace
{
void LogRefineResult(CHARACTER& character, CItem& item, const char* method, int success)
{
    const int step = item.GetRefineLevel();
    const auto pid = static_cast<unsigned long>(static_cast<std::uint32_t>(character.GetPlayerID()));
    const auto itemId = static_cast<unsigned long>(static_cast<std::uint32_t>(item.GetID()));
    const char* name = item.GetName();
    const char* postfix = get_table_postfix();
    LogManager::instance().Query(
        "INSERT INTO refinelog%s (pid, item_name, item_id, step, time, is_success, setType) VALUES(%lu, '%s', %lu, %d, NOW(), %d, '%s')",
        postfix, pid, name, itemId, step, success, method);
}
}

void NotifyRefineSuccess(CHARACTER* character, CItem* item, const char* method)
{
    character->ChatPacket(5, "RefineSuceeded");
    LogRefineResult(*character, *item, method, 1);
}

void NotifyRefineFail(CHARACTER* character, CItem* item, const char* method, int success)
{
    character->ChatPacket(5, "RefineFailed");
    LogRefineResult(*character, *item, method, success);
}

bool CHARACTER::DoRefine(CItem* item, bool moneyOnly)
{
    if (!CanHandleItem(true, false))
    {
        ClearRefineMode();
        return false;
    }

    const int lastUpdate = quest::CQuestManager::instance().GetEventFlag("update_refine_time");
    if (lastUpdate && get_global_time() < static_cast<std::int32_t>(static_cast<std::uint32_t>(lastUpdate) + 300U))
    {
        sys_log(0, "can't refine %d %s", static_cast<int>(GetPlayerID()), GetName());
        return false;
    }

    ClearRefineMode();
    if (!item || item->GetOwner() != this || item->GetCell() >= 90)
        return false;
    const TItemTable* source = item->GetProto();
    if (!source)
        return false;
    const TRefineTable* recipe = CRefineManager::instance().GetRefineRecipe(source->wRefineSet);
    if (!recipe)
        return false;

    int cost = ComputeRefineFee(recipe->cost, 5);
    const char* const freeRefineFlag = "main_quest_lv7.refine_chance";
    const int freeRefines = GetQuestFlag(freeRefineFlag);
    if (freeRefines > 0)
    {
        if (!LC_IsJapan() && !item->CheckItemUseLevel(20))
        {
            ChatPacket(1, "%s", locale_find("\271\253\267\341 \260\263\267\256 \261\342\310\270\264\302 20 \300\314\307\317\300\307 \271\253\261\342\270\270 \260\241\264\311\307\325\264\317\264\331", g_iUseLocale));
            return false;
        }
        cost = 0;
        SetQuestFlag(freeRefineFlag, freeRefines - 1);
    }

    const DWORD resultVnum = source->dwRefinedVnum;
    if (!resultVnum)
    {
        ChatPacket(1, "%s", locale_find("\264\365 \300\314\273\363 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }
    if (source->bType == 3 && source->bSubType == 2)
        return false;

    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    const TItemTable* resultProto = items.GetTable(resultVnum);
    if (!resultProto)
    {
        _sys_err("DoRefine", 0x264, "DoRefine NOT GET ITEM PROTO %d", static_cast<int>(resultVnum));
        ChatPacket(1, "%s", locale_find("\300\314 \276\306\300\314\305\333\300\272 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }
    if (g_iUseLocale == 0)
    {
        for (const auto& limit : resultProto->aLimits)
        {
            if (limit.bType == 1 && GetLevel() < limit.lValue)
            {
                ChatPacket(1, "%s", locale_find("\260\263\267\256\265\310 \310\304 \276\306\300\314\305\333\300\307 \267\271\272\247 \301\246\307\321\272\270\264\331 \267\271\272\247\300\314 \263\267\275\300\264\317\264\331.", g_iUseLocale));
                return false;
            }
        }
    }
    if (GetGold() < cost)
    {
        ChatPacket(1, "%s", locale_find("\260\263\267\256\300\273 \307\317\261\342 \300\247\307\321 \265\267\300\314 \272\316\301\267\307\325\264\317\264\331.", g_iUseLocale));
        return false;
    }

    if (!moneyOnly && freeRefines == 0)
    {
        if (recipe->material_count > sizeof(recipe->materials) / sizeof(recipe->materials[0]))
            return false;
        for (unsigned index = 0; index < recipe->material_count; ++index)
        {
            const TRefineMaterial& material = recipe->materials[index];
            if (material.count < 0 || (material.count && material.vnum == item->GetVnum()))
                return false;

            std::int64_t required = material.count;
            for (unsigned previous = 0; previous < index; ++previous)
            {
                if (recipe->materials[previous].vnum == material.vnum)
                    required += recipe->materials[previous].count;
            }
            const int available = CountSpecifyItem(material.vnum);
            if (available < required)
            {
                if (test_server)
                    ChatPacket(1, "Find %d, count %d, require %d",
                               static_cast<int>(material.vnum), available, material.count);
                ChatPacket(1, "%s", locale_find("\260\263\267\256\300\273 \307\317\261\342 \300\247\307\321 \300\347\267\341\260\241 \272\316\301\267\307\325\264\317\264\331.", g_iUseLocale));
                return false;
            }
        }
        for (unsigned index = 0; index < recipe->material_count; ++index)
        {
            const TRefineMaterial& material = recipe->materials[index];
            if (material.count)
                RemoveSpecifyItem(material.vnum, material.count);
        }
    }

    int roll = number_ex(1, 100, const_cast<char*>("char_item.cpp"), 0x297);
    const bool guild = IsRefineThroughGuild();
    if (guild || moneyOnly)
        roll -= 10;
    const char* const method = guild ? "GUILD" : "POWER";

    if (recipe->prob < roll)
    {
        DBManager::instance().SendMoneyLog(3, source->dwVnum, static_cast<std::int32_t>(0U - static_cast<std::uint32_t>(cost)));
        NotifyRefineFail(this, item, method, 0);
        items.RemoveItem(item, "REMOVE (REFINE FAIL)");
        PayRefineFee(cost);
        return true;
    }

    CItem* replacement = items.CreateItem(resultVnum, 1, 0, false, -1);
    if (!replacement)
    {
        _sys_err("DoRefine", 0x2be, "cannot create item %lu", static_cast<unsigned long>(resultVnum));
        NotifyRefineFail(this, item, method, 0);
        return true;
    }
    const BYTE cell = item->GetCell();
    if (!IsEmptyItemGrid(cell, replacement->GetSize(), cell))
    {
        items.DestroyItem(replacement);
        NotifyRefineFail(this, item, method, 0);
        return true;
    }
    TransformRefineItem(item, replacement);
    NotifyRefineSuccess(this, item, method);
    DBManager::instance().SendMoneyLog(3, source->dwVnum, static_cast<std::int32_t>(0U - static_cast<std::uint32_t>(cost)));
    items.RemoveItem(item, "REMOVE (REFINE SUCCESS)");
    replacement->AddToCharacter(this, cell);
    items.FlushDelayedSave(replacement);
    sys_log(0, "Refine Success %d", cost);
    LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), replacement->GetID(),
                                   "REFINE SUCCESS", replacement->GetName());
    sys_log(0, "PayPee %d", cost);
    PayRefineFee(cost);
    sys_log(0, "PayPee End %d", cost);
    return true;
}

// 08172480
bool CHARACTER::DoRefineWithScroll(CItem* item)
{
    const bool canRefine = CanHandleItem(true, false);
    ClearRefineMode();
    if (!canRefine || !item)
        return false;

    const int updateTime = quest::CQuestManager::instance().GetEventFlag("update_refine_time");
    if (updateTime && get_global_time() < static_cast<std::int32_t>(static_cast<std::uint32_t>(updateTime) + 300U)) {
        sys_log(0, "can't refine %d %s", static_cast<int>(GetPlayerID()), GetName());
        return false;
    }

    const TItemTable* source = item->GetProto();
    if (!source)
        return false;
    const TRefineTable* recipe = CRefineManager::instance().GetRefineRecipe(source->wRefineSet);
    if (!recipe || m_iRefineAdditionalCell < 0)
        return false;
    CItem* scroll = GetItem(static_cast<BYTE>(m_iRefineAdditionalCell));
    if (!scroll || scroll->GetType() != 3 || scroll->GetSubType() != 2 ||
        scroll->GetVnum() == item->GetVnum())
        return false;

    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    const DWORD resultVnum = source->dwRefinedVnum;
    const DWORD downgradeVnum = item->GetRefineFromVnum();
    if (!resultVnum) {
        ChatPacket(1, "%s", locale_find("\264\365 \300\314\273\363 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }

    const long scrollType = scroll->GetValue(0);
    const int refineLevel = item->GetRefineLevel();
    const char* restriction = nullptr;
    if (scrollType == 3 && refineLevel >= 4)
        restriction = "\300\314 \260\263\267\256\274\255\267\316 \264\365 \300\314\273\363 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    else if (scrollType == 5 && refineLevel != scroll->GetValue(1))
        restriction = "\300\314 \260\263\267\256\274\255\267\316 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    else if (scrollType == 6 && (item->GetType() != 10 || refineLevel != 4))
        restriction = "\300\314 \276\306\300\314\305\333\300\270\267\316 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    if (restriction) {
        ChatPacket(1, "%s", locale_find(restriction, g_iUseLocale));
        return false;
    }

    const TItemTable* resultProto = items.GetTable(resultVnum);
    if (!resultProto) {
        _sys_err("DoRefineWithScroll", 0x334, "DoRefineWithScroll NOT GET ITEM PROTO %d",
                 static_cast<int>(resultVnum));
        ChatPacket(1, "%s", locale_find("\300\314 \276\306\300\314\305\333\300\272 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }
    if (!g_iUseLocale) {
        for (const auto& limit : resultProto->aLimits) {
            if (limit.bType == 1 && GetLevel() < limit.lValue) {
                ChatPacket(1, "%s", locale_find("\260\263\267\256\265\310 \310\304 \276\306\300\314\305\333\300\307 \267\271\272\247 \301\246\307\321\272\270\264\331 \267\271\272\247\300\314 \263\267\275\300\264\317\264\331.", g_iUseLocale));
                return false;
            }
        }
    }
    const int cost = recipe->cost;
    if (GetGold() < cost) {
        ChatPacket(1, "%s", locale_find("\260\263\267\256\300\273 \307\317\261\342 \300\247\307\321 \265\267\300\314 \272\316\301\267\307\325\264\317\264\331.", g_iUseLocale));
        return false;
    }

    if (recipe->material_count > sizeof(recipe->materials) / sizeof(recipe->materials[0]))
        return false;
    for (unsigned index = 0; index < recipe->material_count; ++index) {
        const auto& material = recipe->materials[index];
        if (material.count < 0 || (material.count &&
            (material.vnum == item->GetVnum() || material.vnum == scroll->GetVnum())))
            return false;
        std::int64_t required = material.count;
        for (unsigned previous = 0; previous < index; ++previous) {
            if (recipe->materials[previous].vnum == material.vnum)
                required += recipe->materials[previous].count;
        }
        const int available = CountSpecifyItem(material.vnum);
        if (available < required) {
            if (test_server)
                ChatPacket(1, "Find %d, count %d, require %d", static_cast<int>(material.vnum),
                           available, material.count);
            ChatPacket(1, "%s", locale_find("\260\263\267\256\300\273 \307\317\261\342 \300\247\307\321 \300\347\267\341\260\241 \272\316\301\267\307\325\264\317\264\331.", g_iUseLocale));
            return false;
        }
    }
    for (unsigned index = 0; index < recipe->material_count; ++index) {
        const auto& material = recipe->materials[index];
        if (material.count)
            RemoveSpecifyItem(material.vnum, material.count);
    }

    const int roll = number_ex(1, 100, const_cast<char*>("char_item.cpp"), 0x363);
    int successChance = recipe->prob;
    const char* method = "SCROLL";
    const bool cibn = LC_IsCIBN();
    if (scrollType == 1 || scrollType == 2 || scrollType == 4) {
        static constexpr int hyunironCibn[] = {100, 90, 80, 70, 35, 20, 7, 5, 0};
        static constexpr int yongsinCibn[] = {100, 90, 80, 70, 40, 25, 8, 2, 1};
        static constexpr int hyunironEucKr[] = {85, 75, 65, 55, 45, 40, 35, 25, 20};
        static constexpr int yagong[] = {100, 100, 90, 80, 70, 60, 50, 30, 20};
        const int level = std::min(8, std::max(0, refineLevel));
        if (cibn && scrollType == 1)
            successChance = hyunironCibn[level];
        else if (cibn && scrollType == 2)
            successChance = yongsinCibn[level];
        else if (!cibn && scrollType == 2)
            successChance = hyunironEucKr[level];
        else if (!cibn && scrollType == 4)
            successChance = yagong[level];
        else
            _sys_err("DoRefineWithScroll", cibn ? 0x37d : 0x38b,
                     "REFINE : Unknown refine scroll item. Value0: %d", static_cast<int>(scrollType));
        if (test_server)
            ChatPacket(1, "[Only Test] Success_Prob %d , RefineLevel %d ", successChance, refineLevel);
        method = scrollType == 1 ? "HYUNIRON" : scrollType == 2 ? "GOD_SCROLL" : "YAGONG_SCROLL";
    } else if (scrollType == 3) {
        successChance = 100;
        method = "MUSIN_SCROLL";
    } else if (scrollType == 5) {
        successChance = 100;
        method = "MEMO_SCROLL";
    } else if (scrollType == 6) {
        successChance = 60;
        method = "BDRAGON_SCROLL";
    }

    scroll->SetCount(scroll->GetCount() - 1);
    const bool success = roll <= successChance;
    if (!success && (scrollType == 1 || !downgradeVnum)) {
        if (cibn)
            DBManager::instance().SendMoneyLog(3, source->dwVnum, static_cast<std::int32_t>(0U - static_cast<std::uint32_t>(cost)));
        NotifyRefineFail(this, item, method, 0);
        if (cibn)
            items.RemoveItem(item, "REMOVE (REFINE FAIL)");
        PayRefineFee(cost);
        return true;
    }

    const DWORD replacementVnum = success ? resultVnum : downgradeVnum;
    CItem* replacement = items.CreateItem(replacementVnum, 1, 0, false, -1);
    if (!replacement) {
        _sys_err("DoRefineWithScroll", success ? 0x3d5 : 0x3f3, "cannot create item %lu",
                 static_cast<unsigned long>(replacementVnum));
        NotifyRefineFail(this, item, method, 0);
        return true;
    }

    const BYTE cell = item->GetCell();
    if (!IsEmptyItemGrid(cell, replacement->GetSize(), cell)) {
        items.DestroyItem(replacement);
        NotifyRefineFail(this, item, method, 0);
        return true;
    }
    TransformRefineItem(item, replacement);
    if (success)
        NotifyRefineSuccess(this, item, method);
    DBManager::instance().SendMoneyLog(3, source->dwVnum, static_cast<std::int32_t>(0U - static_cast<std::uint32_t>(cost)));
    if (!success)
        NotifyRefineFail(this, item, method, -1);
    items.RemoveItem(item, success ? "REMOVE (REFINE SUCCESS)" : "REMOVE (REFINE FAIL)");
    replacement->AddToCharacter(this, cell);
    items.FlushDelayedSave(replacement);
    LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), replacement->GetID(),
                                  success ? "REFINE SUCCESS" : "REFINE FAIL", replacement->GetName());
    PayRefineFee(cost);
    return true;
}

int CHARACTER::CountSpecifyItem(DWORD vnum)
{
    DWORD total = 0;
    for (BYTE cell = 0; cell < INVENTORY_MAX_NUM; ++cell)
    {
        CItem* item = GetItem(cell);
        if (item && item->GetVnum() == vnum)
            total += item->GetCount();
    }
    return static_cast<std::int32_t>(total);
}

void CHARACTER::RemoveSpecifyItem(DWORD vnum, DWORD count)
{
    for (BYTE cell = 0; cell < INVENTORY_MAX_NUM; ++cell)
    {
        CItem* item = GetItem(cell);
        if (!item || item->GetVnum() != vnum)
            continue;
        if (vnum >= 80003 && vnum <= 80007)
            LogManager::instance().GoldBarLog(GetPlayerID(), item->GetID(), QUEST, "RemoveSpecifyItem");

        const DWORD available = item->GetCount();
        if (count < available)
        {
            item->SetCount(available - count);
            return;
        }
        count -= available;
        item->SetCount(0);
        if (!count)
            return;
    }
    if (count)
        sys_log(0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %lu, still remain %d",
            vnum, static_cast<int>(count));
}

void CHARACTER::SetRefineNPC(CHARACTER* npc)
{
    m_dwRefineNPCVID = static_cast<DWORD>(npc->GetVID());
}

namespace
{
std::int32_t SkillBookReadDeadline(int delay)
{
    const auto now = static_cast<std::uint32_t>(get_global_time());
    return static_cast<std::int32_t>(now + static_cast<std::uint32_t>(delay));
}

enum class SpecialBookResult { Unhandled, Rejected, Used };
enum class SpecialBookKind { Leadership, Combo, Language, Mining, ValueSkill, FixedSkill, TestSkill };

struct SpecialBookRule
{
    DWORD firstVnum;
    DWORD lastVnum;
    SpecialBookKind kind;
    DWORD fixedSkill;
    int delayLine;
};

constexpr SpecialBookRule specialBookRules[] = {
    {50061, 50061, SpecialBookKind::ValueSkill, 0, 0xba2},
    {50301, 50303, SpecialBookKind::Leadership, 121, 0xb3e},
    {50304, 50306, SpecialBookKind::Combo, 122, 0xb68},
    {50311, 50313, SpecialBookKind::Language, 0, 0xb85},
    {50314, 50316, SpecialBookKind::Mining, 129, 0xbd6},
    {50600, 50600, SpecialBookKind::FixedSkill, 124, 0xc1f},
    {50902, 50904, SpecialBookKind::TestSkill, 123, 0xbf5},
};

SpecialBookResult RejectSpecialBook(CHARACTER& character, const char* localeKey)
{
    character.ChatPacket(1, locale_find(localeKey, g_iUseLocale));
    return SpecialBookResult::Rejected;
}

SpecialBookResult UseSpecialSkillBook(CHARACTER& character, CItem& item)
{
    const DWORD vnum = item.GetVnum();
    const SpecialBookRule* rule = nullptr;
    for (const SpecialBookRule& candidate : specialBookRules)
    {
        if (vnum >= candidate.firstVnum && vnum <= candidate.lastVnum)
        {
            rule = &candidate;
            break;
        }
    }
    if (!rule)
        return SpecialBookResult::Unhandled;

    const auto kind = rule->kind;
    if (character.IsPolymorphed())
        return RejectSpecialBook(character, kind == SpecialBookKind::Leadership || kind == SpecialBookKind::Mining
            ? "\265\320\260\251 \301\337\277\241\264\302 \264\311\267\302\300\273 \277\303\270\261 \274\366 \276\370\275\300\264\317\264\331."
            : "\272\257\275\305\301\337\277\241\264\302 \303\245\300\273 \300\320\300\273\274\366 \276\370\275\300\264\317\264\331.");

    const DWORD skill = rule->fixedSkill ? rule->fixedSkill : static_cast<DWORD>(item.GetValue(0));
    const int skillLevel = character.GetSkillLevel(skill);
    const char* const tooAdvanced = "\264\365 \300\314\273\363 \274\366\267\303\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    const char* const tooDifficult = "\300\314 \303\245\300\272 \263\312\271\253 \276\356\267\301\277\366 \300\314\307\330\307\317\261\342\260\241 \310\373\265\354\264\317\264\331.";
    BYTE chance = 0;
    switch (kind)
    {
        case SpecialBookKind::Leadership:
            if (skillLevel < item.GetValue(0))
                return RejectSpecialBook(character, tooDifficult);
            if (skillLevel >= item.GetValue(1))
                return RejectSpecialBook(character, "\300\314 \303\245\300\272 \276\306\271\253\270\256 \272\301\265\265 \265\265\277\362\300\314 \265\311 \260\315 \260\260\301\366 \276\312\275\300\264\317\264\331.");
            break;
        case SpecialBookKind::Combo:
            if (skillLevel == 0 && character.GetLevel() < 30)
                return RejectSpecialBook(character, "\267\271\272\247 30\300\314 \265\307\261\342 \300\374\277\241\264\302 \275\300\265\346\307\322 \274\366 \300\326\300\273 \260\315 \260\260\301\366 \276\312\275\300\264\317\264\331.");
            if (skillLevel == 1 && character.GetLevel() < 50)
                return RejectSpecialBook(character, "\267\271\272\247 50\300\314 \265\307\261\342 \300\374\277\241\264\302 \275\300\265\346\307\322 \274\366 \300\326\300\273 \260\315 \260\260\301\366 \276\312\275\300\264\317\264\331.");
            if (skillLevel > 1)
                return RejectSpecialBook(character, "\277\254\260\350\261\342\264\302 \264\365\300\314\273\363 \274\366\267\303\307\322 \274\366 \276\370\275\300\264\317\264\331.");
            chance = static_cast<BYTE>(item.GetValue(0));
            break;
        case SpecialBookKind::Language:
            if (skillLevel >= 20 || static_cast<std::uint32_t>(skill) - 125u == character.GetEmpire())
                return RejectSpecialBook(character, "\300\314\271\314 \277\317\272\256\307\317\260\324 \276\313\276\306\265\351\300\273 \274\366 \300\326\264\302 \276\360\276\356\300\314\264\331.");
            chance = static_cast<BYTE>(MINMAX(0, static_cast<int>(item.GetValue(1)), 100));
            break;
        case SpecialBookKind::Mining:
            if (character.GetLevel() < item.GetValue(3))
                return RejectSpecialBook(character, "\300\314 \303\245\300\273 \300\320\300\270\267\301\270\351 \267\271\272\247\300\273 \264\365 \277\303\267\301\276\337 \307\325\264\317\264\331.");
            if (skillLevel > 39)
                return RejectSpecialBook(character, tooAdvanced);
            if (skillLevel < item.GetValue(0))
                return RejectSpecialBook(character, tooDifficult);
            if (skillLevel >= item.GetValue(1))
                return RejectSpecialBook(character, "\300\314 \303\245\300\270\267\316\264\302 \264\365 \300\314\273\363 \274\366\267\303\307\322 \274\366 \276\370\275\300\264\317\264\331.");
            chance = static_cast<BYTE>(MINMAX(0, static_cast<int>(item.GetValue(2)), 100));
            break;
        case SpecialBookKind::ValueSkill:
        case SpecialBookKind::FixedSkill:
        case SpecialBookKind::TestSkill:
            if (skillLevel >= (kind == SpecialBookKind::ValueSkill ? 10 : 40))
                return RejectSpecialBook(character, tooAdvanced);
            chance = static_cast<BYTE>(MINMAX(0, static_cast<int>(item.GetValue(1)), 100));
            break;
    }

    const bool learned = character.LearnSkillByBook(skill, chance);
    if (learned)
    {
        ITEM_MANAGER::instance().RemoveItem(&item, nullptr);
        int delay = number_ex(64800, 108000, const_cast<char*>("char_item.cpp"), rule->delayLine);
        if (distribution_test_server)
            delay /= 3;
        character.SetSkillNextReadTime(skill, SkillBookReadDeadline(delay));
    }
    if (kind == SpecialBookKind::TestSkill && test_server)
        character.ChatPacket(1, learned ? "[TEST_SERVER] Success to learn skill " : "[TEST_SERVER] Failed to learn skill ");

    return SpecialBookResult::Used;
}
}

void CHARACTER::__OpenPrivateShop()
{
    if (GetPart(0) < 3)
        ChatPacket(5, "OpenPrivateShop");
    else
        ChatPacket(1, "%s", locale_find("\260\251\277\312\300\273 \271\376\276\356\276\337 \260\263\300\316 \273\363\301\241\300\273 \277\255 \274\366 \300\326\275\300\264\317\264\331.", g_iUseLocale));
}

void CHARACTER::UseSilkBotary()
{
    if (!m_bNoOpenedShop)
    {
        __OpenPrivateShop();
        return;
    }

    const std::uint32_t playerId = GetPlayerID();
    db_clientdesc->DBPacket(109, GetDesc()->GetHandle(), &playerId, sizeof(playerId));
    m_bNoOpenedShop = false;
}

void CHARACTER::UseSilkBotaryReal(const char* entries, BYTE count)
{
    if (count == 0)
        SendMyShopPriceListCmd(1, 0);
    else
    {
        for (unsigned index = 0; index < count; ++index)
        {
            std::uint32_t vnum, price;
            std::memcpy(&vnum, entries, sizeof(vnum));
            std::memcpy(&price, entries + sizeof(vnum), sizeof(price));
            SendMyShopPriceListCmd(vnum, price);
            entries += sizeof(vnum) + sizeof(price);
        }
    }
    __OpenPrivateShop();
}

void CHARACTER::SendMyShopPriceListCmd(DWORD vnum, DWORD price)
{
    char line[256];
    std::snprintf(line, sizeof(line) - 1, "MyShopPriceList %lu %lu",
                  static_cast<unsigned long>(vnum), static_cast<unsigned long>(price));
    ChatPacket(5, "%s", line);
    sys_log(0, "%s", line);
}

bool CHARACTER::UseSkillBookItem(CItem* item)
{
    const BYTE type = item->GetType();
    if (type == 17 && IsPolymorphed())
    {
        ChatPacket(1, locale_find("\272\257\275\305\301\337\277\241\264\302 \303\245\300\273 \300\320\300\273\274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }
    const bool forget = type == 22;
    const bool standard = item->GetVnum() == 50300;
    const DWORD vnum = forget || standard ? item->GetSockets()[0] : item->GetValue(0);
    ITEM_MANAGER& manager = ITEM_MANAGER::instance();
    if (!vnum)
    {
        manager.RemoveItem(item, nullptr);
        return false;
    }

    if (forget)
    {
        const bool removed = SkillLevelDown(vnum);
        if (removed)
            manager.RemoveItem(item, nullptr);
        ChatPacket(1, locale_find(removed ? "\275\272\305\263 \267\271\272\247\300\273 \263\273\270\256\264\302\265\245 \274\272\260\370\307\317\277\264\275\300\264\317\264\331." : "\275\272\305\263 \267\271\272\247\300\273 \263\273\270\261 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
    }
    else if (LearnSkillByBook(vnum))
    {
        manager.RemoveItem(item, nullptr);
        int delay = number_ex(64800, 108000, const_cast<char*>("char_item.cpp"), standard ? 0x7cd : 0x7e7);
        if (distribution_test_server)
            delay /= 3;
        if (LC_IsKorea())
            delay = 86400;
        SetSkillNextReadTime(vnum, SkillBookReadDeadline(delay));
    }
    return true;
}

bool CHARACTER::IsEquipUniqueItem(DWORD vnum)
{
    for (BYTE slot = 7; slot <= 8; ++slot) {
        CItem* item = GetWear(slot);
        if (item && item->GetVnum() == vnum)
            return true;
    }
    return vnum == 0x11176 && IsEquipUniqueItem(0x1119f);
}

// 081713d0
void CHARACTER::ClearItem()
{
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    for (BYTE cell = 0; cell < 109; ++cell) {
        CItem* item = GetItem(cell);
        if (!item)
            continue;
        item->SetSkipSave(true);
        items.FlushDelayedSave(item);
        item->RemoveFromCharacter();
        items.DestroyItem(item);
        SyncQuickslot(1, cell, 255);
    }
}

LPITEM CHARACTER::GetItem(BYTE cell) {
    if(cell<109) return m_pointsInstant.pItems[cell];
    _sys_err("GetItem",212,"CHARACTER::GetItem: invalid item cell %d",static_cast<unsigned>(cell));
    return 0;
}
LPITEM CHARACTER::GetWear(BYTE cell) {
    if(cell<19) return m_pointsInstant.pItems[cell+90];
    _sys_err("GetWear",350,"CHARACTER::GetWear: invalid wear cell %d",static_cast<unsigned>(cell));
    return 0;
}
void CHARACTER::SetItem(BYTE cell,LPITEM item) {
    assert(!item || !item->GetOwner());
    if(cell>=109) {
        _sys_err("SetItem",229,"CHARACTER::SetItem: invalid item cell %d",static_cast<unsigned>(cell));
        return;
    }
    LPITEM old=m_pointsInstant.pItems[cell];
    if (old && old != item) {
        for (CItem*& selected : m_pointsInstant.pCubeItems) {
            if (selected == old)
                selected = nullptr;
        }
    }
    if(old) {
        if(cell<90) {
            for(int i=0;i<old->GetSize();++i) {
                int position=cell+i*5;
                if(position<90 && (!m_pointsInstant.pItems[position] || m_pointsInstant.pItems[position]==old))
                    m_pointsInstant.bItemGrid[position]=0;
            }
        } else m_pointsInstant.bItemGrid[cell]=0;
    }
    if(item) {
        if(cell<90) {
            for(int i=0;i<item->GetSize();++i) {
                int position=cell+i*5;
                if(position<90) m_pointsInstant.bItemGrid[position]=cell+1;
            }
        } else m_pointsInstant.bItemGrid[cell]=cell+1;
    }
    m_pointsInstant.pItems[cell]=item;
    if(GetDesc()) {
        // The target compares signed flag magnitude; it does not test a flag bit here.
        if(!item || item->GetFlag()<0x4000) {
            TPacketGCItemSet packet;
            packet.header=20; packet.pos=cell;
            if(!item) {
                packet.count=0; packet.vnum=0;
                std::memset(packet.alSockets,0,sizeof(packet.alSockets));
                std::memset(packet.aAttr,0,sizeof(packet.aAttr));
            } else {
                packet.vnum=item->GetVnum(); packet.count=static_cast<BYTE>(item->GetCount());
                for (int i = 0; i < 3; ++i)
                    packet.alSockets[i] = static_cast<std::int32_t>(item->GetSockets()[i]);
                std::memcpy(packet.aAttr,item->GetAttributes(),sizeof(packet.aAttr));
            }
            GetDesc()->Packet(&packet,sizeof(packet));
        } else {
            TPacketGCItemSet2 packet;
            packet.header=21; packet.pos=cell; packet.vnum=item->GetVnum();
            packet.count=static_cast<BYTE>(item->GetCount()); packet.flags=item->GetFlag();
            for (int i = 0; i < 3; ++i)
                packet.alSockets[i] = static_cast<std::int32_t>(item->GetSockets()[i]);
            std::memcpy(packet.aAttr,item->GetAttributes(),sizeof(packet.aAttr));
            GetDesc()->Packet(&packet,sizeof(packet));
        }
    }
    if(item) {
        item->SetCell(this,cell);
        item->SetWindow(cell>89 ? 2 : 1);
    }
}
LPITEM CHARACTER::FindSpecifyItem(DWORD vnum) {
    for(int cell=0;cell<90;++cell) {
        LPITEM item=GetItem(static_cast<BYTE>(cell));
        if(item && item->GetVnum()==vnum) return item;
    }
    return 0;
}

// Complete old inventory occupancy helpers081714d0/08171680.
bool CHARACTER::IsEmptyItemGrid(BYTE cell,BYTE size,int exception)
{
    if (cell>=90) return false;
    const unsigned int occupiedBy=(unsigned int)exception+1U;
    BYTE grid=m_pointsInstant.bItemGrid[cell];
    if (grid!=0 && grid!=occupiedBy) return false;
    if (size==1) return true;
    int j=1;
    do {
        const BYTE p=(BYTE)(cell+5*j);
        if (p>=90 || p/45!=cell/45) return false;
        grid=m_pointsInstant.bItemGrid[p];
        if (grid!=0 && grid!=occupiedBy) return false;
        ++j;
    } while (j<(int)size);
    return true;
}
// 081716e0
int CHARACTER::CountEmptyInventory()
{
    int occupied = 0;
    for (BYTE cell = 0; cell < INVENTORY_MAX_NUM; ++cell) {
        if (CItem* item = GetItem(cell)) {
            if (const TItemTable* proto = item->GetProto())
                occupied += proto->bSize;
        }
    }
    return INVENTORY_MAX_NUM - occupied;
}

int CHARACTER::GetEmptyInventory(BYTE size)
{
    for(int i=0;i<90;++i)
        if(IsEmptyItemGrid((BYTE)i,size,-1)) return i;
    return -1;
}


// Original 08170ee0.
bool CHARACTER::CanHandleItem(bool skipRefine, bool skipObserver) {
    if ((!skipObserver && IsObserverMode()) || m_pkMyShop ||
        (!skipRefine && m_bUnderRefine) || m_pointsInstant.pCubeNpc)
        return false;
    if (!m_pkWarpEvent)
        return true;
    if (test_server)
        sys_log(0, "[WARP] Warping.");
    return false;
}

void CHARACTER::SetWear(BYTE cell,LPITEM item)
{
    if(cell>=19) {
        _sys_err("SetWear",361,"CHARACTER::SetItem: invalid item cell %d",(unsigned int)cell);
        return;
    }
    SetItem((BYTE)(cell+90),item);
    if(!item && cell==4) {
        if(IsAffectFlag(20)) RemoveAffect(63);
        if(IsAffectFlag(16)) RemoveAffect(4);
    }
}

template <typename Socket>
static bool HaveSameItemSockets(const long* left, const Socket* right)
{
    for (int socket = 0; socket < 3; ++socket) {
        if (static_cast<std::uint32_t>(left[socket]) !=
            static_cast<std::uint32_t>(right[socket]))
            return false;
    }
    return true;
}

bool FN_check_item_socket(LPITEM item);
namespace auto_give_detail {
static int signed32(unsigned int v) { return v <= 0x7fffffffU ? (int)v : -1-(int)(0xffffffffU-v); }
}
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, BYTE bCount, int iRarePct, bool bMsg)
{
    (void)iRarePct; // Original explicitly passes -1 to CreateItem.
    TItemTable* p = ITEM_MANAGER::instance().GetTable(dwItemVnum);
    if (!p) return 0;
    DBManager::instance().SendMoneyLog(8, dwItemVnum, (int)bCount);
    if ((p->dwFlags & 4) && p->bType != 27) {
        for (int i=0; i<90; ++i) {
            LPITEM item=GetItem((BYTE)i);
            if (item && item->GetVnum()==dwItemVnum && FN_check_item_socket(item)) {
                const int room=auto_give_detail::signed32(200U-item->GetCount());
                const BYTE contribution=(BYTE)(room<(int)bCount ? room : (int)bCount);
                bCount=(BYTE)(bCount-contribution);
                item->SetCount(item->GetCount()+contribution);
                if (!bCount) {
                    if (bMsg) {
                        const char* name=item->GetName();
                        ChatPacket(1,locale_find("\276\306\300\314\305\333\040\310\271\265\346\072\040\045\163",g_iUseLocale),name);
                    }
                    return item;
                }
            }
        }
    }
    LPITEM item=ITEM_MANAGER::instance().CreateItem(dwItemVnum,(DWORD)bCount,0,true,-1);
    if (!item) {
        _sys_err("AutoGiveItem",5536,"cannot create item by vnum %lu (name: %s)",dwItemVnum,GetName());
        return 0;
    }
    if (item->GetType()==27) {
        for (int i=0;i<90;++i) {
            LPITEM inv=GetItem((BYTE)i);
            if (inv && inv->GetType()==27 && inv->GetVnum()==item->GetVnum()
                && HaveSameItemSockets(inv->GetSockets(), item->GetSockets())
                && inv->GetCount()<200U) {
                inv->SetCount(inv->GetCount()+item->GetCount());
                return inv; // Original leaves created item registered.
            }
        }
    }
    const int cell=GetEmptyInventory(item->GetSize());
    if (cell==-1) {
        item->AddToGround(GetMapIndex(),GetXYZ());
        item->StartDestroyEvent();
        item->SetOwnership(this,60);
        const char* name=item->GetName();
        LogManager::instance().ItemLog(GetPlayerID(),GetX(),GetY(),item->GetID(),"SYSTEM_DROP",name);
    } else {
        if (bMsg) {
            const char* name=item->GetName();
            ChatPacket(1,locale_find("\276\306\300\314\305\333\040\310\271\265\346\072\040\045\163",g_iUseLocale),name);
        }
        item->AddToCharacter(this,(BYTE)cell);
        const char* name=item->GetName();
        LogManager::instance().ItemLog(GetPlayerID(),GetX(),GetY(),item->GetID(),"SYSTEM",name);
        TQuickslot* old;
        if (item->GetType()==3 && item->GetSubType()==0 && GetQuickslot(0,&old) && old->type==0) {
            TQuickslot slot; slot.type=1; slot.pos=(BYTE)cell;
            SetQuickslot(0,slot);
        }
    }
    sys_log(0,"7: %d %d",auto_give_detail::signed32(dwItemVnum),static_cast<int>(bCount));
    return item;
}

bool FN_check_item_socket(LPITEM item)
{
    return HaveSameItemSockets(item->GetSockets(), item->GetProto()->alSockets);
}

int g_nPortalLimitTime=10;

namespace {
int pulse_delta(int a, int b)
{
    unsigned int bits=static_cast<unsigned int>(a)-static_cast<unsigned int>(b);
    int result;
    std::memcpy(&result,&bits,sizeof(result));
    return result;
}
}

// 08180970
bool CHARACTER::IsEquipUniqueGroup(DWORD group)
{
    LPITEM item = GetWear(7);
    if (item && static_cast<DWORD>(item->GetSpecialGroup()) == group)
        return true;
    item = GetWear(8);
    return item && static_cast<DWORD>(item->GetSpecialGroup()) == group;
}

bool CHARACTER::GiveItemFromSpecialItemGroup(DWORD groupVnum, DWORD& itemVnum, DWORD& itemCount, LPITEM& item)
{
    CSpecialItemGroup* group = ITEM_MANAGER::instance().GetSpecialItemGroup(groupVnum);
    if (!group) {
        _sys_err("GiveItemFromSpecialItemGroup", 0x172b, "cannot find special item group %d",
                 static_cast<int>(groupVnum));
        return false;
    }
    const CSpecialItemGroup::CSpecialItemInfo* reward = group->SelectItem();
    if (!reward)
        return false;

    const DWORD vnum = reward->vnum;
    const DWORD count = static_cast<unsigned int>(reward->count);
    switch (vnum) {
        case CSpecialItemGroup::GOLD:
            PointChange(11, count, false, false);
            LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), count, "TREASURE_GOLD", "");
            break;
        case CSpecialItemGroup::EXP:
            PointChange(3, count, false, false);
            LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), count, "TREASURE_EXP", "");
            break;
        case CSpecialItemGroup::MOB: {
            sys_log(0, "CSpecialItemGroup::MOB %ld", static_cast<long>(count));
            const long x = GetX() + number_ex(-500, 500, const_cast<char*>("char_item.cpp"), 0x1749);
            const long y = GetY() + number_ex(-500, 500, const_cast<char*>("char_item.cpp"), 0x174a);
            CHARACTER* mob = CHARACTER_MANAGER::instance().SpawnMob(count, GetMapIndex(), x, y, 0, true, -1, true);
            if (mob)
                mob->SetAggressive();
            break;
        }
        case CSpecialItemGroup::SLOW:
            sys_log(0, "CSpecialItemGroup::SLOW %d", -reward->count);
            AddAffect(0xd3, 19, -reward->count, 5, 300, 0, true, false);
            break;
        case CSpecialItemGroup::DRAIN_HP: {
            const unsigned amount = static_cast<unsigned>(m_pointsInstant.iMaxHP) *
                                    static_cast<unsigned>(count) / 100;
            sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -static_cast<int>(amount));
            const int drain = MIN(static_cast<int>(amount), m_points.hp - 1);
            sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -drain);
            PointChange(5, -drain, false, false);
            break;
        }
        case CSpecialItemGroup::POISON:
            AttackedByPoison(0);
            break;
        case CSpecialItemGroup::MOB_GROUP: {
            char source[] = "char_item.cpp";
            const long sx = GetX() - number_ex(300, 500, source, 0x176c);
            const long sy = GetY() - number_ex(300, 500, source, 0x176d);
            const long ex = GetX() + number_ex(300, 500, source, 0x176e);
            const long ey = GetY() + number_ex(300, 500, source, 5999);
            CHARACTER_MANAGER::instance().SpawnGroup(count, GetMapIndex(), sx, sy, ex, ey, 0, true, 0);
            break;
        }
        default:
            item = AutoGiveItem(vnum, static_cast<BYTE>(count), reward->rare, true);
            if (!item)
                return false;
            break;
    }
    itemVnum = vnum;
    itemCount = count;
    return true;
}

// 0817e9d0: share gold by proximity to the receiving character.
namespace {
constexpr const char* dropForbidden =
    "\271\366\270\261\040\274\366\040\276\370\264\302\040\276\306\300\314\305\333\300\324\264\317\264\331\056";
constexpr const char* dropTimeout =
    "\266\263\276\356\301\370\040\276\306\300\314\305\333\300\272\040\063\272\320\040\310\304\040\273\347\266\363\301\375\264\317\264\331\056";
constexpr const char* pickupNotice =
    "\276\306\300\314\305\333\040\310\271\265\346\072\040\045\163";
constexpr const char* inventoryFull =
    "\274\322\301\366\307\317\260\355\040\300\326\264\302\040\276\306\300\314\305\333\300\314\040\263\312\271\253\040\270\271\275\300\264\317\264\331\056";
constexpr const char* pickupForOwner =
    "\276\306\300\314\305\333\040\310\271\265\346\072\040\045\163\040\264\324\300\270\267\316\272\316\305\315\040\045\163";
constexpr const char* pickupForParty =
    "\276\306\300\314\305\333\040\300\374\264\336\072\040\045\163\040\264\324\277\241\260\324\040\045\163";

bool CanSplitStack(DWORD vnum)
{
    return !LC_IsNewCIBN() || (vnum != 0x115b7 && vnum != 0x1158a && vnum != 0x11196);
}

void LogGroundItem(CHARACTER& character, CItem& item, const char* operation)
{
    char hint[32];
    std::snprintf(hint, sizeof(hint), "%s %lu %lu", item.GetName(), item.GetCount(), item.GetVnum());
    LogManager::instance().ItemLog(character.GetPlayerID(), character.GetX(), character.GetY(),
                                  item.GetID(), operation, hint);
}
}

bool CHARACTER::SwapItem(BYTE cell, BYTE destination)
{
    if (!CanHandleItem(false, false) || cell >= 109 || destination >= 109 || cell == destination)
        return false;
    if (cell >= 90) {
        if (destination >= 90)
            return false;
        std::swap(cell, destination);
    }

    CItem* incoming = GetItem(cell);
    CItem* outgoing = GetItem(destination);
    if (!incoming || !outgoing || incoming == outgoing || incoming->IsExchanging() || outgoing->IsExchanging())
        return false;
    if (!IsEmptyItemGrid(cell, outgoing->GetSize(), cell))
        return false;

    if (destination >= 90) {
        const BYTE slot = destination - 90;
        if ((outgoing->GetFlag() & 0x100) || incoming->FindEquipCell(this) != slot)
            return false;
        outgoing->RemoveFromCharacter();
        if (!incoming->EquipTo(this, slot)) {
            _sys_err("SwapItem", 0x14d0, "SwapItem cannot equip %s! item1 %s", outgoing->GetName(), incoming->GetName());
            outgoing->EquipTo(this, slot);
            return false;
        }
        outgoing->AddToCharacter(this, cell);
        return true;
    }

    incoming->RemoveFromCharacter();
    outgoing->RemoveFromCharacter();
    if (IsEmptyItemGrid(destination, incoming->GetSize(), -1) && incoming->AddToCharacter(this, destination)) {
        if (IsEmptyItemGrid(cell, outgoing->GetSize(), -1) && outgoing->AddToCharacter(this, cell))
            return true;
        incoming->RemoveFromCharacter();
    }
    incoming->AddToCharacter(this, cell);
    outgoing->AddToCharacter(this, destination);
    return false;
}

bool CHARACTER::EquipItem(CItem* item)
{
    if (!item || item->IsExchanging())
        return false;
    switch (item->GetType()) {
        case 1: case 2: case 13: case 16: case 24: break;
        default: return false;
    }
    const int wear = item->FindEquipCell(this);
    sys_log(0, "FindEquipCell %s wear %d", item->GetName(), wear);
    if (wear < 0)
        return false;

    if (wear == 0 && IsHorseRiding()) {
        const DWORD vnum = item->GetVnum();
        if (vnum >= 11901 && vnum <= 11904) {
            ChatPacket(1, LC_TEXT(
                "\270\273\300\273\040\305\272\040\273\363\305\302\277\241\274\255\040\277\271\272\271\300\273\040"
                "\300\324\300\273\040\274\366\040\276\370\275\300\264\317\264\331\056"));
            return false;
        }
    }
    if (wear != 9 && IsPolymorphed()) {
        ChatPacket(1, LC_TEXT(
            "\265\320\260\251\040\301\337\277\241\264\302\040\302\370\277\353\301\337\300\316\040"
            "\300\345\272\361\270\246\040\272\257\260\346\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056"));
        return false;
    }
    const DWORD now = get_dword_time();
    if (wear != 9 && (static_cast<unsigned>(now - m_dwLastAttackTime) <= 1500 ||
                      static_cast<unsigned>(now - m_dwLastSkillTime) <= 1500)) {
        ChatPacket(1, LC_TEXT(
            "\260\241\270\270\310\367\040\300\326\300\273\040\266\247\270\270\040\302\370\277\353\307\322\040"
            "\274\366\040\300\326\275\300\264\317\264\331\056"));
        return false;
    }

    const BYTE slot = static_cast<BYTE>(wear);
    CItem* existing = GetWear(slot);
    if (!existing || (existing->GetFlag() & 0x100)) {
        const BYTE oldCell = item->GetCell();
        if (item->EquipTo(this, slot)) {
            SyncQuickslot(1, oldCell, slot);
            if (item->GetVnum() == 70048)
                ShowAlignment(false);
        }
        return true;
    }
    if (item->GetProto() && item->GetProto()->dwWearFlags != 0x800)
        SwapItem(item->GetCell(), slot + 90);
    return false;
}

bool CHARACTER::UnequipItem(CItem* item)
{
    if (!item)
        return false;
    const int cell = GetEmptyInventory(item->GetSize());
    if (cell < 0 || (item->GetFlag() & 0x100))
        return false;
    if (item->GetVnum() == 70048)
        ShowAlignment(true);
    item->RemoveFromCharacter();
    item->AddToCharacter(this, static_cast<BYTE>(cell));
    CheckMaximumPoints();
    return true;
}

// 08170e20
static void FN_copy_item_socket(CItem* destination, CItem* source)
{
    for (int slot = 0; slot < 3; ++slot)
        destination->SetSocket(slot, source->GetSockets()[slot]);
}

bool CHARACTER::MoveItem(BYTE cell, BYTE destination, BYTE count)
{
    if (!CanHandleItem(false, false) || cell >= 109 || destination >= 109)
        return false;
    CItem* item = GetItem(cell);
    if (!item || item->IsExchanging() || item->GetCount() < count ||
        (cell >= 90 && (item->GetFlag() & 0x100)))
        return false;

    CItem* target = GetItem(destination);
    if (destination >= 90) {
        if (target)
            return false;
        EquipItem(item);
        return true;
    }
    const DWORD available = item->GetCount();
    if (target && target != item && (target->GetFlag() & 4) &&
        target->GetProto() && !(target->GetProto()->dwAntiFlags & 0x8000) &&
        target->GetVnum() == item->GetVnum()) {
        if (!HaveSameItemSockets(target->GetSockets(), item->GetSockets()))
            return false;
        const DWORD requested = count ? count : available;
        sys_log(0, "%s: ITEM_STACK %s %d -> %d count %d", GetName(), item->GetName(),
                static_cast<int>(cell), static_cast<int>(destination), static_cast<int>(requested));
        if (target->GetCount() > 200)
            return true;
        const DWORD transferred = MIN(200 - target->GetCount(), requested);
        item->SetCount(available - transferred);
        target->SetCount(target->GetCount() + transferred);
        return true;
    }

    if (!IsEmptyItemGrid(destination, item->GetSize(), cell))
        return false;
    if (count && count < available && (item->GetFlag() & 4) &&
        item->GetProto() && !(item->GetProto()->dwAntiFlags & 0x8000)) {
        if (!CanSplitStack(item->GetVnum()) || !IsEmptyItemGrid(destination, item->GetSize(), -1))
            return false;
        sys_log(0, "%s: ITEM_SPLIT %s %d -> %d count %d", GetName(), item->GetName(),
                static_cast<int>(cell), static_cast<int>(destination), static_cast<int>(count));
        ITEM_MANAGER& items = ITEM_MANAGER::instance();
        CItem* split = items.CreateItem(item->GetVnum(), count, 0, false, -1);
        if (!split)
            return false;
        item->SetCount(available - count);
        FN_copy_item_socket(split, item);
        if (!split->AddToCharacter(this, destination)) {
            item->SetCount(available);
            items.DestroyItem(split);
            return false;
        }

        char hint[52];
        std::snprintf(hint, sizeof(hint), "%lu %lu %lu %lu ", split->GetID(), split->GetCount(),
                      item->GetCount(), split->GetCount() + item->GetCount());
        LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), item->GetID(), "ITEM_SPLIT", hint);
        return true;
    }

    sys_log(0, "%s: ITEM_MOVE %s %d -> %d count %d", GetName(), item->GetName(),
            static_cast<int>(cell), static_cast<int>(destination), static_cast<int>(count));
    item->RemoveFromCharacter();
    SetItem(destination, item);
    SyncQuickslot(1, cell, destination);
    return true;
}

bool CHARACTER::DropItem(BYTE cell, BYTE count)
{
    if (!CanHandleItem(false, false) || IsDead() || cell >= 90)
        return false;
    CItem* item = GetItem(cell);
    if (!item || item->IsExchanging())
        return false;
    if (item->GetProto() && (item->GetProto()->dwAntiFlags & 0x2080)) {
        ChatPacket(1, LC_TEXT(dropForbidden));
        return false;
    }

    const DWORD available = item->GetCount();
    const DWORD quantity = !count || count > available ? available : count;
    if (!quantity || (quantity < available && !CanSplitStack(item->GetVnum())))
        return false;

    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    const PIXEL_POSITION position = GetXYZ();
    const long map = GetMapIndex();
    if (!SECTREE_MANAGER::instance().Get(map, position.x, position.y))
        return false;

    CItem* dropped = item;
    if (quantity == available) {
        SyncQuickslot(1, cell, 255);
        item->RemoveFromCharacter();
        if (!item->AddToGround(map, position)) {
            item->AddToCharacter(this, cell);
            return false;
        }
    } else {
        dropped = items.CreateItem(item->GetVnum(), quantity, 0, false, -1);
        if (!dropped)
            return false;
        SyncQuickslot(1, cell, 255);
        item->SetCount(available - quantity);
        items.FlushDelayedSave(item);
        FN_copy_item_socket(dropped, item);
        if (!dropped->AddToGround(map, position)) {
            item->SetCount(available);
            items.FlushDelayedSave(item);
            items.DestroyItem(dropped);
            return false;
        }

        char hint[52];
        std::snprintf(hint, sizeof(hint), "%lu %lu", dropped->GetID(), dropped->GetCount());
        LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), item->GetID(), "ITEM_SPLIT", hint);
    }
    ChatPacket(1, LC_TEXT(dropTimeout));
    dropped->StartDestroyEvent();
    items.FlushDelayedSave(dropped);
    LogGroundItem(*this, *dropped, "DROP");
    return true;
}

bool CHARACTER::DropGold(long gold)
{
    if (gold < 1 || gold > GetGold() || !CanHandleItem(false, false))
        return false;
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    CItem* item = items.CreateItem(1, gold, 0, false, -1);
    if (!item)
        return false;
    if (item->AddToGround(GetMapIndex(), GetXYZ())) {
        PointChange(POINT_GOLD, -gold, true, false);
        if (gold > 1000)
            LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), gold, "DROP_GOLD", "");
    } else {
        items.DestroyItem(item);
    }
    // 0817e2d0 queues a save and succeeds after creation even if placement fails.
    Save();
    return true;
}

bool CHARACTER::PickupItem(DWORD vid)
{
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    CItem* item = items.FindByVID(vid);
    if (IsObserverMode() || !item || !item->DistanceValid(this) ||
        SECTREE_MANAGER::instance().Get(GetMapIndex(), item->GetX(), item->GetY()) != item->GetSectree())
        return false;

    if (item->IsOwnership(this)) {
        if (item->GetType() == 9) {
            GiveGold(item->GetCount());
            item->RemoveFromGround();
            items.DestroyItem(item);
            Save();
            return true;
        }

        if ((item->GetFlag() & 4) && item->GetProto() && !(item->GetProto()->dwAntiFlags & 0x8000)) {
            DWORD remaining = item->GetCount();
            for (BYTE slot = 0; slot < 90; ++slot) {
                CItem* stack = GetItem(slot);
                if (!stack || stack->GetVnum() != item->GetVnum() || stack->GetCount() > 200 ||
                    !HaveSameItemSockets(item->GetSockets(), stack->GetSockets()))
                    continue;
                const DWORD transferred = MIN(200 - stack->GetCount(), remaining);
                stack->SetCount(stack->GetCount() + transferred);
                remaining -= transferred;
                if (!remaining) {
                    ChatPacket(1, LC_TEXT(pickupNotice), stack->GetName());
                    items.DestroyItem(item);
                    return true;
                }
            }
            item->SetCount(remaining);
        }

        const int slot = GetEmptyInventory(item->GetSize());
        if (slot < 0) {
            sys_log(0, "No empty inventory pid %lu size %lud itemid %lu", GetPlayerID(),
                    static_cast<unsigned long>(item->GetSize()), item->GetID());
            ChatPacket(1, LC_TEXT(inventoryFull));
            return false;
        }
        item->RemoveFromGround();
        item->AddToCharacter(this, static_cast<BYTE>(slot));
        LogGroundItem(*this, *item, "GET");
        ChatPacket(1, LC_TEXT(pickupNotice), item->GetName());
        return true;
    }

    CParty* party = GetParty();
    if (!party)
        return false;
    CHARACTER* owner = nullptr;
    party->ForEachOnlineMember([&](CHARACTER* member) {
        if (item->IsOwnership(member))
            owner = member;
    });
    // 0817f27e: use the picker if the ownership search finds no linked member
    // or the owner cannot hold the item.
    int slot = owner ? owner->GetEmptyInventory(item->GetSize()) : -1;
    if (slot < 0) {
        owner = this;
        slot = GetEmptyInventory(item->GetSize());
    }
    if (slot < 0) {
        ChatPacket(1, LC_TEXT(inventoryFull));
        return false;
    }

    item->RemoveFromGround();
    item->AddToCharacter(owner, static_cast<BYTE>(slot));
    LogGroundItem(*owner, *item, "GET");
    if (owner == this) {
        ChatPacket(1, LC_TEXT(pickupNotice), item->GetName());
    } else {
        owner->ChatPacket(1, LC_TEXT(pickupForOwner), GetName(), item->GetName());
        ChatPacket(1, LC_TEXT(pickupForParty), owner->GetName(), item->GetName());
    }
    return false;
}

void CHARACTER::GiveGold(int amount)
{
    if (amount <= 0)
        return;

    sys_log(0, "GIVE_GOLD: %s %d", GetName(), amount);
    int ownAmount = amount;
    if (CParty* party = GetParty()) {
        const auto x = static_cast<std::uint32_t>(GetX());
        const auto y = static_cast<std::uint32_t>(GetY());
        const auto isNear = [x, y](CHARACTER* member) {
            const auto dx = static_cast<std::int32_t>(static_cast<std::uint32_t>(member->GetX()) - x);
            const auto dy = static_cast<std::int32_t>(static_cast<std::uint32_t>(member->GetY()) - y);
            return DISTANCE_APPROX(dx, dy) <= 5000;
        };
        int nearCount = 0;
        party->ForEachOnlineMember([&](CHARACTER* member) {
            if (isNear(member))
                ++nearCount;
        });
        if (nearCount > 1) {
            const int share = amount / nearCount;
            ownAmount -= share * (nearCount - 1);
            party->ForEachOnlineMember([&](CHARACTER* member) {
                if (member != this && isNear(member)) {
                    member->PointChange(POINT_GOLD, share, true, false);
                    if (share > 1000)
                        LogManager::instance().CharLog(member->GetPlayerID(), member->GetX(), member->GetY(),
                                                       share, "GET_GOLD", "");
                }
            });
        }
    }

    PointChange(POINT_GOLD, ownAmount, true, false);
    if (ownAmount > 1000)
        LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), ownAmount, "GET_GOLD", "");
}

namespace {
bool FN_check_item_sex(CHARACTER* character, CItem* item)
{
    const DWORD flags = item->GetProto() ? item->GetProto()->dwAntiFlags : 0;
    const bool female = GET_SEX(character) == SEX_FEMALE;
    return !(flags & (female ? 1U : 2U));
}
}

EVENTFUNC(kill_campfire_event)
{
    const auto vid = *static_cast<std::uint32_t*>(event->info);
    auto& manager = CHARACTER_MANAGER::instance();
    if (LPCHARACTER campfire = manager.Find(vid))
        manager.DestroyCharacter(campfire);
    return 0;
}

static bool IS_SUMMON_ITEM(int vnum)
{
    return vnum == 22000 || vnum == 22010 || vnum == 22011 || vnum == 22020 || vnum == 70302;
}

static bool IS_MONKEY_DUNGEON(int map)
{
    return map == 5 || map == 25 || map == 45 || map == 108 || map == 109;
}

bool IS_SUMMONABLE_ZONE(int map)
{
    return !IS_MONKEY_DUNGEON(map) && (map < 181 || map > 183) && (map < 71 || map > 73);
}

bool CHARACTER::UseItem(BYTE cell, int destination)
{
    if (!CanHandleItem(false, false) || cell > 108)
        return false;
    CItem* item = GetItem(cell);
    if (!item)
        return false;
    sys_log(0, "%s: USE_ITEM %s (cell: %d)", GetName(), item->GetName(), cell);
    if (item->IsExchanging())
        return false;
    if (!item->CanUsedBy(this)) {
        ChatPacket(1, "%s", locale_find("\261\272\301\367\300\314\040\270\302\301\366\276\312\276\306\040\300\314\040\276\306\300\314\305\333\300\273\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return false;
    }
    if (IsStun())
        return false;
    if (!FN_check_item_sex(this, item)) {
        ChatPacket(1, "%s", locale_find("\274\272\272\260\300\314\040\270\302\301\366\276\312\276\306\040\300\314\040\276\306\300\314\305\333\300\273\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return false;
    }

    const DWORD vnum = item->GetVnum();
    const bool busy = m_pkExchange || m_pkMyShop || m_pkChrShopOwner ||
                      m_isOpenSafebox || IsCubeOpen();
    if (IS_SUMMON_ITEM(static_cast<int>(vnum))) {
        const int map = GetMapIndex();
        if (!IS_SUMMONABLE_ZONE(map)) {
            ChatPacket(1, "%s", locale_find("\273\347\277\353\307\322\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
            return false;
        }
        if (IsForkedMapIndex(map)) {
            ChatPacket(1, "%s", locale_find("\273\357\260\305\270\256\040\300\374\305\365\040\302\374\260\241\301\337\277\241\264\302\040\261\315\310\257\272\316\054\261\315\310\257\261\342\276\357\272\316\270\246\040\273\347\277\353\307\322\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
            return false;
        }

        const int pulse = thecore_pulse();
        const int limit = passes_per_sec * g_nPortalLimitTime;
        if (pulse_delta(pulse, m_iSafeboxLoadTime) < limit) {
            ChatPacket(1, locale_find("\303\242\260\355\270\246\040\277\254\310\304\040\045\144\303\312\040\300\314\263\273\277\241\264\302\040\261\315\310\257\272\316\054\261\315\310\257\261\342\276\357\272\316\270\246\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale), g_nPortalLimitTime);
            if (test_server)
                ChatPacket(1, "[TestOnly]Pulse %d LoadTime %d PASS %d", pulse, m_iSafeboxLoadTime, limit);
            return false;
        }
        if (busy) {
            ChatPacket(1, "%s", locale_find("\260\305\267\241\303\242\054\303\242\260\355\040\265\356\300\273\040\277\254\040\273\363\305\302\277\241\274\255\264\302\040\261\315\310\257\272\316\054\261\315\310\257\261\342\276\357\272\316\040\270\246\040\273\347\277\353\307\322\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
            return false;
        }
        if (pulse_delta(pulse, m_iRefineTime) < limit) {
            ChatPacket(1, locale_find("\276\306\300\314\305\333\040\260\263\267\256\310\304\040\045\144\303\312\040\300\314\263\273\277\241\264\302\040\261\315\310\257\272\316\054\261\315\310\257\261\342\276\357\272\316\270\246\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale), g_nPortalLimitTime);
            return false;
        }
        if (pulse_delta(pulse, m_iMyShopTime) < limit) {
            ChatPacket(1, locale_find("\260\263\300\316\273\363\301\241\040\273\347\277\353\310\304\040\045\144\303\312\040\300\314\263\273\277\241\264\302\040\261\315\310\257\272\316\054\261\315\310\257\261\342\276\357\272\316\270\246\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale), g_nPortalLimitTime);
            return false;
        }
        if (vnum != 70302) {
            PIXEL_POSITION recall{};
            if (vnum == 22000)
                SECTREE_MANAGER::instance().GetRecallPositionByEmpire(map, GetEmpire(), recall);
            int dx = 0;
            int dy = 0;
            if (vnum == 22010 || vnum == 22000) {
                const long* sockets = item->GetSockets();
                const bool stored = vnum == 22010 || sockets[0] != 0;
                const auto x = static_cast<unsigned>(stored ? sockets[0] : recall.x);
                const auto y = static_cast<unsigned>(stored ? sockets[1] : recall.y);
                dx = auto_give_detail::signed32(x - static_cast<unsigned>(GetX()));
                dy = auto_give_detail::signed32(y - static_cast<unsigned>(GetY()));
            }
            const double distance = std::sqrt(std::pow(static_cast<double>(dx), 2.0) +
                                              std::pow(static_cast<double>(dy), 2.0));
            if (distance < 5000.0) {
                ChatPacket(1, "%s", locale_find("\300\314\265\277\040\265\307\276\356\301\372\040\300\247\304\241\277\315\040\263\312\271\253\040\260\241\261\356\277\366\040\261\315\310\257\272\316\270\246\040\273\347\277\353\307\322\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
                if (test_server)
                    ChatPacket(1, "PossibleDistant %f nNowDist %f", 5000.0, distance);
                return false;
            }
        }
        if (pulse_delta(pulse, m_iExchangeTime) < limit) {
            ChatPacket(1, locale_find("\260\305\267\241\040\310\304\040\045\144\303\312\040\300\314\263\273\277\241\264\302\040\261\315\310\257\272\316\054\261\315\310\257\261\342\276\357\272\316\265\356\300\273\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale), g_nPortalLimitTime);
            return false;
        }
    }
    if ((vnum == 71049 || vnum == 50200) && busy) {
        ChatPacket(1, "%s", locale_find("\260\305\267\241\303\242\054\303\242\260\355\040\265\356\300\273\040\277\254\040\273\363\305\302\277\241\274\255\264\302\040\272\270\265\373\270\256\054\272\361\264\334\272\270\265\373\270\256\270\246\040\273\347\277\353\307\322\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return false;
    }

    const DWORD vid = item->GetVID();
    if (item->GetFlag() & (0x400 | 0x800)) {
        const bool receiveAll = (item->GetFlag() & 0x800) != 0;
        sys_log(0, "%s", receiveAll ? "ITEM_FLAG_QUEST_USE_MULTIPLE" : "ITEM_FLAG_QUEST_USE");
        const bool handled = quest::CQuestManager::instance().UseItem(GetPlayerID(), item, receiveAll);
        item = ITEM_MANAGER::instance().FindByVID(vid);
        if (!item || item->GetOwner() != this)
            return handled;
    }

    if (!(item->GetFlag() & 0x2000))
        return UseItemEx(item, destination);

    const DWORD id = item->GetID();
    const DWORD count = item->GetCount();
    char hint[57];
    std::snprintf(hint, 24, "%s", item->GetName());
    const bool used = UseItemEx(item, destination);
    item = ITEM_MANAGER::instance().FindByVID(vid);
    const char* operation = "REMOVE";
    if (item) {
        if (count == item->GetCount())
            return used;
        const size_t length = std::strlen(hint);
        std::snprintf(hint + length, 56 - length, " %lu", static_cast<unsigned long>(item->GetCount()));
        operation = "USE_ITEM";
    }
    LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), id, operation, hint);
    return used;
}

namespace {
int RecallMapEmpire(int map)
{
    if (map < 20)
        return 1;
    if (map < 40)
        return 2;
    if (map < 60)
        return 3;
    return map == 66 || map >= 10000 ? -1 : 0;
}
}

bool CHARACTER::GiveRecallItem(CItem* item)
{
    const int empire = RecallMapEmpire(GetMapIndex());
    if (empire && empire != GetEmpire()) {
        ChatPacket(1, "%s", locale_find("\261\342\276\357\307\330\040\265\321\040\274\366\040\276\370\264\302\040\300\247\304\241\040\300\324\264\317\264\331\056", g_iUseLocale));
        return false;
    }
    if (item->GetCount() == 1) {
        item->SetSocket(0, GetX());
        item->SetSocket(1, GetY());
        return true;
    }

    const int position = GetEmptyInventory(item->GetSize());
    if (position < 0) {
        ChatPacket(1, "%s", locale_find("\274\322\301\366\307\260\277\241\040\272\363\040\260\370\260\243\300\314\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return false;
    }
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    CItem* recall = items.CreateItem(item->GetVnum(), 1, 0, false, -1);
    if (!recall)
        return false;
    recall->SetSocket(0, GetX());
    recall->SetSocket(1, GetY());
    if (!recall->AddToCharacter(this, static_cast<BYTE>(position))) {
        items.RemoveItem(recall, "RECALL");
        return false;
    }
    item->SetCount(item->GetCount() - 1);
    return true;
}

int g_nWarpTime = 5;

namespace {
struct DelayedItemWarpInfo {
    CHARACTER* character;
    int hpChange;
    int x;
    int y;
    int elapsed;
};

bool WaitForItemWarp(DelayedItemWarpInfo& info)
{
    const int flag = quest::CQuestManager::instance().GetEventFlag("warp_time");
    const int duration = flag ? flag : g_nWarpTime;
    info.elapsed = auto_give_detail::signed32(static_cast<unsigned>(info.elapsed) + 1u);
    if (info.elapsed >= duration)
        return false;
    info.character->ChatPacket(1, locale_find("%d \xc3\xca\xc8\xc4 \xb8\xf1\xc0\xfb\xc1\xf6\xb7\xce \xc0\xcc\xb5\xbf\xb5\xcb\xb4\xcf\xb4\xd9.", g_iUseLocale),
                               auto_give_detail::signed32(static_cast<unsigned>(g_nWarpTime) -
                                                          static_cast<unsigned>(info.elapsed)));
    return true;
}
}

// 08174350
EVENTFUNC(delayed_warp_event)
{
    auto& info = *static_cast<DelayedItemWarpInfo*>(event->info);
    if (!info.character) {
        if (test_server)
            sys_log(0, "delayed_warp_event failed : ch is NULL");
        return 0;
    }
    if (WaitForItemWarp(info))
        return passes_per_sec;
    info.character->m_pkWarpEvent = nullptr;
    info.character->PointChange(POINT_HP, info.hpChange, false, false);
    info.character->WarpSet(info.x, info.y, 0);
    return 0;
}

// 081744d0
EVENTFUNC(delayed_timewarp_event)
{
    auto& info = *static_cast<DelayedItemWarpInfo*>(event->info);
    CHARACTER* character = info.character;
    if (!character) {
        if (test_server)
            sys_log(0, "delayed_timewarp_event failed : ch is NULL");
        return 0;
    }
    if (WaitForItemWarp(info))
        return passes_per_sec;
    character->m_pkWarpEvent = nullptr;
    sys_log(1, "Recall: %s %d %d -> %d %d", character->GetName(),
            static_cast<int>(character->GetX()), static_cast<int>(character->GetY()), info.x, info.y);
    character->PointChange(POINT_HP, info.hpChange, false, false);
    character->WarpSet(info.x, info.y, 0);
    return 0;
}

void CHARACTER::ProcessRecallItem(CItem* item)
{
    const long x = item->GetSockets()[0];
    const long y = item->GetSockets()[1];
    const int map = SECTREE_MANAGER::instance().GetMapIndex(x, y);
    if (!map)
        return;
    const int empire = RecallMapEmpire(map);
    if (empire && empire != GetEmpire()) {
        ChatPacket(1, "%s", locale_find("\261\342\276\357\265\310\040\300\247\304\241\260\241\040\305\270\301\246\261\271\277\241\040\274\323\307\330\040\300\326\276\356\274\255\040\261\315\310\257\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        item->SetSocket(0, 0);
        item->SetSocket(1, 0);
        return;
    }
    sys_log(1, "Recall: %s %d %d -> %d %d", GetName(),
            static_cast<std::int32_t>(GetX()), static_cast<std::int32_t>(GetY()),
            static_cast<std::int32_t>(x), static_cast<std::int32_t>(y));
    WarpSet(item->GetSockets()[0], item->GetSockets()[1], 0);
    item->SetCount(item->GetCount() - 1);
}

// 08174230
static int CalculateConsume(CHARACTER* character)
{
    const int maximum = character->GetMaxHP();
    const int current = character->GetHP();
    const int need = auto_give_detail::signed32(static_cast<unsigned>(maximum) * 30U) / 100;
    if (current < need) {
        character->ChatPacket(1, "%s", locale_find("\263\262\300\272 \273\375\270\355\267\302 \276\347\300\314 \270\360\300\332\266\363 \273\347\277\353\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return -1;
    }
    const int minimum = maximum / 10;
    const unsigned currentBits = static_cast<unsigned>(current);
    const int afterUse = auto_give_detail::signed32(currentBits - static_cast<unsigned>(need));
    const int consume = afterUse < minimum
        ? auto_give_detail::signed32(currentBits - static_cast<unsigned>(minimum)) : need;
    return std::max(0, consume);
}

// 081742d0
static int CalculateConsumeSP(CHARACTER* character)
{
    const int need = auto_give_detail::signed32(static_cast<unsigned>(character->GetMaxSP()) * 30U) / 100;
    if (character->GetSP() < need) {
        character->ChatPacket(1, "%s", locale_find("\263\262\300\272 \301\244\275\305\267\302 \276\347\300\314 \270\360\300\332\266\363 \273\347\277\353\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return -1;
    }
    return need;
}

bool CHARACTER::UseRecallItem(CItem* item)
{
    const int map = GetMapIndex();
    if (map == 200 || map == 113) {
        ChatPacket(1, "%s", locale_find("\307\366\300\347\040\300\247\304\241\277\241\274\255\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return false;
    }
    if (CArenaManager::instance().IsArenaMap(map)) {
        ChatPacket(1, "%s", locale_find("\264\353\267\303\040\301\337\277\241\264\302\040\300\314\277\353\307\322\040\274\366\040\276\370\264\302\040\271\260\307\260\300\324\264\317\264\331\056", g_iUseLocale));
        return false;
    }
    if (m_pkWarpEvent) {
        ChatPacket(1, "%s", locale_find("\300\314\265\277\307\322\040\301\330\272\361\260\241\040\265\307\276\356\300\326\300\275\300\270\267\316\040\261\315\310\257\272\316\270\246\040\273\347\277\353\307\322\274\366\040\276\370\275\300\264\317\264\331", g_iUseLocale));
        return false;
    }

    const int cost = CalculateConsume(this);
    if (cost < 0)
        return false;
    const long portal = item->GetValue(0);
    if (portal != 1 && portal != 2)
        return true;

    if (item->GetSockets()[0]) {
        if (portal == 2 || LC_IsCIBN())
            PointChange(5, -cost, false, false);
        if (portal == 1 && test_server)
            ChatPacket(1, "%s", locale_find("\277\370\267\241\040\300\247\304\241\267\316\040\272\271\261\315", g_iUseLocale));
        ProcessRecallItem(item);
        return true;
    }

    if (portal == 2) {
        if (m_pkDungeon) {
            const char* suffix = "";
            if (g_iUseLocale == 0)
                suffix = locale_find(under_han(item->GetName()) ? "\300\273" : "\270\246", g_iUseLocale);
            ChatPacket(1, locale_find("\264\370\300\374\040\276\310\277\241\274\255\264\302\040\045\163\045\163\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale), item->GetName(), suffix);
            return false;
        }
        return GiveRecallItem(item);
    }

    if (!m_pkDungeon && !GiveRecallItem(item))
        return false;
    PIXEL_POSITION recall;
    if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(map, GetEmpire(), recall)) {
        PointChange(5, -cost, false, false);
        WarpSet(recall.x, recall.y, 0);
    } else {
        _sys_err("UseItemEx", 0xf9b, "CHARACTER::UseItem : cannot find spawn position (name %s, %ld x %ld)",
                 GetName(), GetX(), GetY());
    }
    return true;
}

bool CHARACTER::UseMetinStone(CItem* item, int destination)
{
    if (destination < 0 || destination >= 90)
        return false;
    CItem* target = GetItem(static_cast<BYTE>(destination));
    if (!target || target->IsExchanging())
        return false;

    const BYTE targetType = target->GetType();
    if (targetType == 24 || targetType == 13)
        return false;

    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    const long* sockets = target->GetSockets();
    for (int index = 0; index < 3; ++index) {
        const auto vnum = static_cast<std::uint32_t>(sockets[index]);
        if (vnum <= 2)
            continue;
        const TItemTable* stone = items.GetTable(vnum);
        if (stone && item->GetValue(5) == stone->alValues[5]) {
            ChatPacket(1, "%s", locale_find("\260\260\300\272\040\301\276\267\371\300\307\040\270\336\306\276\274\256\300\272\040\277\251\267\257\260\263\040\272\316\302\370\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
            return false;
        }
    }

    const DWORD wear = item->GetProto()->dwWearFlags;
    if (targetType == 2 && !(wear & 1)) {
        ChatPacket(1, "%s", locale_find("\300\314\040\270\336\306\276\274\256\300\272\040\300\345\272\361\277\241\040\272\316\302\370\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return false;
    }
    if (targetType == 1 && !(wear & 0x10)) {
        ChatPacket(1, "%s", locale_find("\300\314\040\270\336\306\276\274\256\300\272\040\271\253\261\342\277\241\040\272\316\302\370\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return false;
    }

    for (int index = 0; index < 3; ++index) {
        const long socket = sockets[index];
        if (socket < 1 || socket > 2 || item->GetValue(2) > socket)
            continue;
        const bool success = number_ex(1, 100, const_cast<char*>("char_item.cpp"), 0x11d3) <= 30;
        ChatPacket(1, "%s", locale_find(success ? "\270\336\306\276\274\256\040\272\316\302\370\277\241\040\274\272\260\370\307\317\277\264\275\300\264\317\264\331\056" : "\270\336\306\276\274\256\040\272\316\302\370\277\241\040\275\307\306\320\307\317\277\264\275\300\264\317\264\331\056", g_iUseLocale));
        target->SetSocket(index, success ? item->GetVnum() : 0x7120);
        LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), target->GetID(),
                                       "SOCKET", item->GetName());
        items.RemoveItem(item, "REMOVE (METIN)");
        return true;
    }
    ChatPacket(1, "%s", locale_find("\272\316\302\370\307\322\040\274\366\040\300\326\264\302\040\275\275\267\324\300\314\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
    return true;
}

bool CHARACTER::ItemProcess_Hair(CItem* item, int /*destination*/)
{
    if (!item->CheckItemUseLevel(GetLevel())) {
        ChatPacket(1, "%s", locale_find("\276\306\301\367\040\300\314\040\270\323\270\256\270\246\040\273\347\277\353\307\322\040\274\366\040\276\370\264\302\040\267\271\272\247\300\324\264\317\264\331\056", g_iUseLocale));
        return false;
    }

    static const DWORD jobBase[] = {72000, 71250, 70500, 69750};
    const BYTE job = GetJob();
    if (job >= sizeof(jobBase) / sizeof(jobBase[0]))
        return false;
    const WORD hair = static_cast<WORD>(item->GetVnum() - jobBase[job]);
    if (GetPart(3) == hair) {
        ChatPacket(1, "%s", locale_find("\265\277\300\317\307\321\040\270\323\270\256\040\275\272\305\270\300\317\267\316\264\302\040\261\263\303\274\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return true;
    }

    item->SetCount(item->GetCount() - 1);
    SetPart(3, hair);
    UpdatePacket();
    return true;
}

// 08181120
bool CHARACTER::ItemProcess_Polymorph(CItem* item)
{
    if (IsPolymorphed()) {
        ChatPacket(1, "%s", LC_TEXT("\300\314\271\314\040\265\320\260\251\301\337\300\316\040\273\363\305\302\300\324\264\317\264\331\056"));
        return false;
    }

    const DWORD race = item->GetSockets()[0];
    const CMob* mob = race ? CMobManager::instance().Get(race) : nullptr;
    if (!mob) {
        ChatPacket(1, "%s", LC_TEXT("\300\337\270\370\265\310\040\265\320\260\251\040\276\306\300\314\305\333\300\324\264\317\264\331\056"));
        item->SetCount(item->GetCount() - 1);
        return false;
    }

    const DWORD vnum = item->GetVnum();
    if (vnum == 50322) {
        sys_log(0, "USE_POLYMORPH_BOOK PID(%d) vnum(%d)", static_cast<int>(GetPlayerID()), static_cast<int>(race));
        CPolymorphUtils& polymorph = CPolymorphUtils::instance();
        if (polymorph.PolymorphCharacter(this, item, mob))
            polymorph.UpdateBookPracticeGrade(this, item);
        return true;
    }
    if ((vnum < 70104 || vnum > 70107) && vnum != 71093) {
        _sys_err("ItemProcess_Polymorph", 0x1806, "POLYMORPH invalid item passed PID(%d) vnum(%d)",
                 static_cast<int>(GetPlayerID()), static_cast<int>(vnum));
        return false;
    }

    sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", static_cast<int>(GetPlayerID()), static_cast<int>(race));
    const int level = GetLevel();
    const int allowance = level * 3 / 10 < 20 ? 20 - level * 3 / 10 : 0;
    if (mob->m_table.bLevel >= level + allowance) {
        ChatPacket(1, "%s", LC_TEXT("\263\252\272\270\264\331\040\263\312\271\253\040\263\364\300\272\040\267\271\272\247\300\307\040\270\363\275\272\305\315\267\316\264\302\040\272\257\275\305\040\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056"));
        return false;
    }

    const int skill = GetSkillLevel(SKILL_POLYMORPH);
    const int duration = (skill ? 10 + skill / 40 * 25 : 5) * 60;
    const int bonus = GetSkillLevel(SKILL_POLYMORPH) / 40 * 100 + 200;
    AddAffect(220, POINT_POLYMORPH, race, 35, duration, 0, true, false);
    AddAffect(220, POINT_ATT_BONUS, bonus, 35, duration, 0, false, false);
    item->SetCount(item->GetCount() - 1);
    return true;
}

namespace {
void SendTreasureRewardNotice(CHARACTER& character, DWORD rewardVnum, DWORD rewardCount, LPITEM reward)
{
    switch (rewardVnum) {
    case CSpecialItemGroup::GOLD:
        character.ChatPacket(1, LC_TEXT("\265\267 %d \263\311\300\273 \310\271\265\346\307\337\275\300\264\317\264\331."), static_cast<int>(rewardCount));
        break;
    case CSpecialItemGroup::EXP:
        character.ChatPacket(1, "%s", LC_TEXT("\273\363\300\332\277\241\274\255 \272\316\305\315 \275\305\272\361\307\321 \272\373\300\314 \263\252\277\311\264\317\264\331."));
        character.ChatPacket(1, LC_TEXT("%d\300\307 \260\346\307\350\304\241\270\246 \310\271\265\346\307\337\275\300\264\317\264\331."), static_cast<int>(rewardCount));
        break;
    case CSpecialItemGroup::MOB:
    case CSpecialItemGroup::MOB_GROUP:
        character.ChatPacket(1, "%s", LC_TEXT("\273\363\300\332\277\241\274\255 \270\363\275\272\305\315\260\241 \263\252\305\270\263\265\275\300\264\317\264\331!"));
        break;
    case CSpecialItemGroup::SLOW:
        character.ChatPacket(1, "%s", LC_TEXT("\273\363\300\332\277\241\274\255 \263\252\277\302 \273\241\260\243 \277\254\261\342\270\246 \265\351\300\314\270\266\275\303\300\332 \277\362\301\367\300\314\264\302 \274\323\265\265\260\241 \264\300\267\301\301\263\275\300\264\317\264\331!"));
        break;
    case CSpecialItemGroup::DRAIN_HP:
        character.ChatPacket(1, "%s", LC_TEXT("\273\363\300\332\260\241 \260\251\300\332\261\342 \306\370\271\337\307\317\277\264\275\300\264\317\264\331! \273\375\270\355\267\302\300\314 \260\250\274\322\307\337\275\300\264\317\264\331."));
        break;
    case CSpecialItemGroup::POISON:
        character.ChatPacket(1, "%s", LC_TEXT("\273\363\300\332\277\241\274\255 \263\252\277\302 \263\354\273\366 \277\254\261\342\270\246 \265\351\300\314\270\266\275\303\300\332 \265\266\300\314 \277\302\270\366\300\270\267\316 \306\333\301\375\264\317\264\331!"));
        break;
    default:
        if (reward && rewardCount < 2)
            character.ChatPacket(1, LC_TEXT("\273\363\300\332\277\241\274\255 %s \260\241 \263\252\277\324\275\300\264\317\264\331."), reward->GetName());
        else if (reward)
            character.ChatPacket(1, LC_TEXT("\273\363\300\332\277\241\274\255 %s \260\241 %d \260\263 \263\252\277\324\275\300\264\317\264\331."), reward->GetName(), static_cast<int>(rewardCount));
        break;
    }
}
} // namespace

bool CHARACTER::UseItemEx(CItem* item, int destination)
{
    const TItemTable* proto = item->GetProto();
    if (!proto)
        return false;

    for (const auto& limit : proto->aLimits) {
        if (limit.bType == 1 && GetLevel() < limit.lValue) {
            ChatPacket(1, "%s", locale_find("\276\306\300\314\305\333\300\307\040\267\271\272\247\040\301\246\307\321\272\270\264\331\040\267\271\272\247\300\314\040\263\267\275\300\264\317\264\331\056", g_iUseLocale));
            return false;
        }
    }
    if (LC_IsCIBN() && (proto->dwVnum == 50085 || proto->dwVnum == 50086)) {
        const int limit = passes_per_sec * 5;
        if (pulse_delta(thecore_pulse(), m_iSeedTime) < limit) {
            ChatPacket(1, "%s", locale_find("\277\371\272\264\054\301\276\300\332\040\273\347\277\353\310\304\040\065\303\312\265\277\276\310\040\273\347\277\353\307\322\274\366\040\276\370\275\300\264\317\264\331\056\040", g_iUseLocale));
            if (test_server)
                ChatPacket(1, "Now %d Limit%d", pulse_delta(thecore_pulse(), m_iSeedTime), limit);
            return false;
        }
    }

    const BYTE type = item->GetType();
    if (test_server)
        sys_log(0, "USE_ITEM %s , Cell %d , ItemType %d , SubType %d",
                item->GetName(), destination, type, item->GetSubType());
    if (type == 3) {
        const BYTE subtype = item->GetSubType();
        const DWORD vnum = item->GetVnum();
        if (vnum >= 50801 && vnum <= 50820) {
            if (test_server)
                sys_log(0, "ADD addtional effect : vnum(%d) subtype(%d)", static_cast<int>(vnum), subtype);
            if (subtype != 7 && subtype != 8 && subtype != 11)
                return true;
        } else {
            if (vnum >= 27863 && vnum <= 27883 && CArenaManager::instance().IsArenaMap(GetMapIndex())) {
                ChatPacket(1, "%s", LC_TEXT("\264\353\267\303 \301\337\277\241\264\302 \300\314\277\353\307\322 \274\366 \276\370\264\302 \271\260\307\260\300\324\264\317\264\331."));
                return false;
            }
            if (test_server)
                sys_log(0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), type,
                        subtype, static_cast<int>(vnum));
        }
        if ((subtype >= 3 && subtype <= 5) || subtype == 15 || subtype > 24)
            return true;
        if (subtype == 1)
            return UseRecallItem(item);
        if (subtype == 0 || (subtype >= 7 && subtype <= 9) || subtype == 11 || subtype == 16)
            return UseConsumableItem(item, subtype);
        if (subtype == 2 || subtype == 14)
            return UseRefineItem(item, destination);
        if ((subtype >= 17 && subtype <= 22) || subtype == 24)
            return UseItemModification(item, destination, subtype);
    }
    if (type == 17 || type == 22)
        return UseSkillBookItem(item);
    if (type == 3 && item->GetSubType() == 23)
    {
        struct Material
        {
            LPITEM stack;
            std::uint64_t count;
        };
        Material materials[] = {
            {FindSpecifyItem(static_cast<DWORD>(item->GetValue(1))),
             static_cast<std::uint32_t>(item->GetValue(2))},
            {FindSpecifyItem(static_cast<DWORD>(item->GetValue(3))),
             static_cast<std::uint32_t>(item->GetValue(4))},
            {nullptr, 1},
        };
        for (unsigned index = 0; index < 2; ++index)
        {
            if (!materials[index].stack && materials[index].count)
            {
                ChatPacket(1, "%s", locale_find("\271\260\276\340 \301\266\307\325\300\273 \300\247\307\321 \300\347\267\341\260\241 \272\316\301\267\307\325\264\317\264\331.", g_iUseLocale));
                return false;
            }
        }

        materials[2].stack = FindSpecifyItem(50901);
        for (unsigned index = 0; index < 3; ++index)
        {
            const Material& current = materials[index];
            if (!current.count)
                continue;
            std::uint64_t required = current.count;
            if (current.stack)
            {
                // Keep the recipe item itself, including when a material shares its stack.
                if (current.stack == item)
                    ++required;
                for (unsigned previous = 0; previous < index; ++previous)
                {
                    if (materials[previous].stack == current.stack)
                        required += materials[previous].count;
                }
            }
            if (current.stack && current.stack->GetCount() >= required)
                continue;
            if (index == 2)
                ChatPacket(1, "%s", locale_find("\272\363 \272\264\300\314 \270\360\300\332\270\250\264\317\264\331.", g_iUseLocale));
            else
                ChatPacket(1, locale_find("\300\347\267\341(%s)\260\241 \272\316\301\267\307\325\264\317\264\331.", g_iUseLocale), current.stack->GetName());
            return false;
        }

        const DWORD reward = static_cast<DWORD>(item->GetValue(0));
        const long chance = item->GetValue(5);
        for (unsigned index = 0; index < 3; ++index)
        {
            const Material& material = materials[index];
            if (!material.stack)
                continue;
            const DWORD remaining = material.stack->GetCount() - static_cast<DWORD>(material.count);
            if (!remaining)
            {
                for (unsigned next = index + 1; next < 3; ++next)
                {
                    if (materials[next].stack == material.stack)
                        materials[next].stack = nullptr;
                }
            }
            material.stack->SetCount(remaining);
        }
        if (number_ex(1, 100, const_cast<char*>("char_item.cpp"), 0x118f) <= chance)
        {
            AutoGiveItem(reward, 1, -1, true);
            return true;
        }
        ChatPacket(1, "%s", locale_find("\271\260\276\340 \301\246\301\266\277\241 \275\307\306\320\307\337\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }
    if (type == 3 && item->GetSubType() == 12)
    {
        RemoveBadAffect();
        item->SetCount(item->GetCount() - 1);
        return true;
    }
    if (type == 3 && item->GetSubType() == 13)
    {
        if (item->GetVnum() == 70026)
        {
            if (quest::PC* pc = quest::CQuestManager::instance().GetPC(GetPlayerID()))
            {
                const char* const flag = "mirror_of_disapper.last_use_time";
                if (get_global_time() - pc->GetFlag(flag) < 600)
                {
                    ChatPacket(1, "%s", locale_find("\276\306\301\367 \273\347\277\353\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
                    return false;
                }
                pc->SetFlag(flag, get_global_time(), false);
            }
        }
        AddAffect(203, 0, 0, 2, 300, 0, true, false);
        item->SetCount(item->GetCount() - 1);
        return true;
    }
    if (type == 3 && item->GetSubType() == 10)
    {
        const SpecialBookResult result = UseSpecialSkillBook(*this, *item);
        if (result != SpecialBookResult::Unhandled)
            return result == SpecialBookResult::Used;
        const DWORD vnum = item->GetVnum();
        switch (vnum)
        {
            case 50023:
            case 50024:
            case 50025:
            case 50031:
            case 50032: {
                if (vnum != 50023)
                {
                    const bool female = GET_SEX(this) == SEX_FEMALE;
                    const bool femaleGift = vnum == 50024 || vnum == 50032;
                    if (female != femaleGift)
                    {
                        ChatPacket(1, "%s", locale_find("\274\272\272\260\300\314 \270\302\301\366\276\312\276\306 \300\314 \276\306\300\314\305\333\300\273 \277\255 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
                        return false;
                    }
                }
                DWORD rewardVnum = 0;
                DWORD rewardCount = 0;
                LPITEM reward = nullptr;
                if (GiveItemFromSpecialItemGroup(vnum, rewardVnum, rewardCount, reward))
                {
                    if (vnum == 50023 && rewardVnum == 1)
                        ChatPacket(1, locale_find("\265\267 %d \263\311\300\273 \310\271\265\346\307\337\275\300\264\317\264\331.", g_iUseLocale), static_cast<int>(rewardCount));
                    item->SetCount(item->GetCount() - 1);
                }
                return true;
            }
            case 50026: {
                const int gold = static_cast<int>(item->GetSockets()[0]);
                ITEM_MANAGER::instance().RemoveItem(item, nullptr);
                ChatPacket(1, locale_find("\265\267 %d \263\311\300\273 \310\271\265\346\307\337\275\300\264\317\264\331.", g_iUseLocale), gold);
                PointChange(POINT_GOLD, gold, false, false);
                return true;
            }
            case 70008:
                ForgetMyAttacker();
                item->SetCount(item->GetCount() - 1);
                return true;
            case 70014:
                PointChange(POINT_STAT_RESET_COUNT, 1, false, false);
                item->SetCount(item->GetCount() - 1);
                return true;
            case 27610:
                AddAffect(208, 0, 0, 14, 1200, 0, true, false);
                item->SetCount(item->GetCount() - 1);
                return true;
            case 27996:
                item->SetCount(item->GetCount() - 1);
                return true;
            case 50020:
                PointChange(POINT_HP, GetMaxHP() - GetHP(), false, false);
                item->SetCount(item->GetCount() - 1);
                return true;
            case 50021:
                PointChange(POINT_SP, GetMaxSP() - GetSP(), false, false);
                item->SetCount(item->GetCount() - 1);
                return true;
            case 50022:
                PointChange(POINT_STAMINA, m_pointsInstant.iMaxStamina - m_points.stamina, false, false);
                item->SetCount(item->GetCount() - 1);
                return true;
            case 50100: case 50101: case 50102: case 50103:
            case 50104: case 50105: case 50106:
                CreateFly(static_cast<BYTE>(vnum - 50100 + 7), this);
                item->SetCount(item->GetCount() - 1);
                return true;
            case 50107:
            case 50108:
                if (vnum == 50108 && CArenaManager::instance().IsArenaMap(GetMapIndex()))
                {
                    ChatPacket(1, "%s", locale_find("\264\353\267\303 \301\337\277\241\264\302 \300\314\277\353\307\322 \274\366 \276\370\264\302 \271\260\307\260\300\324\264\317\264\331.", g_iUseLocale));
                    return false;
                }
                EffectPacket(vnum == 50107 ? 9 : 10);
                AddAffect(224, 38, 30, 39, 300, 0, true, false);
                item->SetCount(item->GetCount() - 1);
                return true;
            case 50200:
                __OpenPrivateShop();
                return true;
            case 70038:
                AggregateMonster();
                item->SetCount(item->GetCount() - 1);
                return true;
            case 70102:
            case 70103: {
                const int alignment = GetAlignment();
                if (alignment >= (LC_IsJapan() ? 200000 : 0))
                    return false;
                const int delta = MIN(-alignment, static_cast<int>(item->GetValue(0)));
                sys_log(0, "%s ALIGNMENT ITEM %d", GetName(), delta);
                UpdateAlignment(delta);
                item->SetCount(item->GetCount() - 1);
                if (delta / 10 > 0)
                {
                    ChatPacket(0, "%s", locale_find("\270\266\300\275\300\314 \270\274\276\306\301\366\264\302\261\272. \260\241\275\277\300\273 \301\376\264\251\270\243\264\370 \271\253\276\360\260\241\260\241 \301\273 \260\241\272\255\277\366\301\370 \264\300\263\246\300\314\276\337.", g_iUseLocale));
                    ChatPacket(1, locale_find("\274\261\276\307\304\241\260\241 %d \301\365\260\241\307\317\277\264\275\300\264\317\264\331.", g_iUseLocale), delta / 10);
                }
                return true;
            }
            case 70201: case 70202: case 70203: case 70204: case 70205: case 70206: {
                if (GetPart(3) > 1000)
                {
                    ChatPacket(1, "%s", locale_find("\307\366\300\347 \307\354\276\356\275\272\305\270\300\317\277\241\274\255\264\302 \277\260\273\366\260\372 \305\273\273\366\300\314 \272\322\260\241\264\311\307\325\264\317\264\331.", g_iUseLocale));
                    return true;
                }
                quest::PC* pc = quest::CQuestManager::instance().GetPC(GetPlayerID());
                if (!pc)
                    return true;
                const char* const flag = "dyeing_hair.last_dye_level";
                const int lastDye = pc->GetFlag(flag);
                if (vnum != 70201 && lastDye && GetLevel() < lastDye + 3)
                {
                    ChatPacket(1, locale_find("%d \267\271\272\247\300\314 \265\307\276\356\276\337 \264\331\275\303 \277\260\273\366\307\317\275\307 \274\366 \300\326\275\300\264\317\264\331.", g_iUseLocale), lastDye + 3);
                    return true;
                }
                SetPart(3, static_cast<WORD>(vnum - 70201));
                item->SetCount(item->GetCount() - 1);
                UpdatePacket();
                pc->SetFlag(flag, vnum == 70201 ? 0 : GetLevel(), false);
                return true;
            }
            case 71013:
                CreateFly(static_cast<BYTE>(number_ex(7, 12, const_cast<char*>("char_item.cpp"), 0xb0a)), this);
                item->SetCount(item->GetCount() - 1);
                return true;
            case 71051:
            case 71052: {
                CItem* target = destination >= 0 && destination < 90
                    ? GetItem(static_cast<BYTE>(destination)) : nullptr;
                if (!target || target == item || target->IsExchanging())
                    return false;
                if (target->GetAttributeSetIndex() == -1)
                {
                    ChatPacket(1, "%s", locale_find("\274\323\274\272\300\273 \272\257\260\346\307\322 \274\366 \276\370\264\302 \276\306\300\314\305\333\300\324\264\317\264\331.", g_iUseLocale));
                    return false;
                }
                const bool add = vnum == 71051;
                if (!(add ? target->AddRareAttribute() : target->ChangeRareAttribute()))
                {
                    ChatPacket(1, "%s", locale_find(add ? "\264\365 \300\314\273\363 \300\314 \276\306\300\314\305\333\300\270\267\316 \274\323\274\272\300\273 \303\337\260\241\307\322 \274\366 \276\370\275\300\264\317\264\331" : "\272\257\260\346 \275\303\305\263 \274\323\274\272\300\314 \276\370\275\300\264\317\264\331", g_iUseLocale));
                    return true;
                }
                const DWORD sourceID = item->GetID();
                item->SetCount(item->GetCount() - 1);
                char hint[20];
                std::snprintf(hint, sizeof(hint), "%lu", static_cast<unsigned long>(target->GetID()));
                if (add)
                {
                    ChatPacket(1, "%s", locale_find("\274\272\260\370\300\373\300\270\267\316 \274\323\274\272\300\314 \303\337\260\241 \265\307\276\372\275\300\264\317\264\331", g_iUseLocale));
                    const auto& attribute = target->GetAttributes()[target->GetRareAttrCount() + 4];
                    LogManager::instance().ItemLog(GetPlayerID(), attribute.bType, attribute.sValue,
                                                  sourceID, "ADD_RARE_ATTR", hint);
                }
                else
                {
                    LogManager::instance().ItemLog(GetPlayerID(), 0, 0, sourceID, "CHANGE_RARE_ATTR", hint);
                }
                return true;
            }
            case 27987: {
                item->SetCount(item->GetCount() - 1);
                const int roll = number_ex(1, 100, const_cast<char*>("char_item.cpp"), 0xadd);
                const bool korean = g_iUseLocale == 0;
                const int limits[] = {50, korean ? 80 : 95, korean ? 90 : 97, korean ? 97 : 99, 100};
                const DWORD rewards[] = {27990, 0, 27992, 27993, 27994};
                const char* const notices[] = {"\301\266\260\263\277\241\274\255 \265\271\301\266\260\242\300\314 \263\252\277\324\275\300\264\317\264\331.", "\301\266\260\263\260\241 \310\347\300\373\265\265 \276\370\300\314 \273\347\266\363\301\375\264\317\264\331.", "\301\266\260\263\277\241\274\255 \271\351\301\370\301\326\260\241 \263\252\277\324\275\300\264\317\264\331.", "\301\266\260\263\277\241\274\255 \303\273\301\370\301\326\260\241 \263\252\277\324\275\300\264\317\264\331.", "\301\266\260\263\277\241\274\255 \307\307\301\370\301\326\260\241 \263\252\277\324\275\300\264\317\264\331."};
                unsigned outcome = 0;
                while (roll > limits[outcome])
                    ++outcome;
                ChatPacket(1, "%s", locale_find(notices[outcome], g_iUseLocale));
                if (rewards[outcome])
                    AutoGiveItem(rewards[outcome], 1, -1, true);
                return true;
            }
            case 50011: {
                DWORD rewardVnum = 0;
                DWORD rewardCount = 0;
                LPITEM reward = nullptr;
                if (!GiveItemFromSpecialItemGroup(vnum, rewardVnum, rewardCount, reward))
                {
                    ChatPacket(0, "%s", locale_find("\276\306\271\253\260\315\265\265 \276\362\300\273 \274\366 \276\370\276\372\275\300\264\317\264\331.", g_iUseLocale));
                    return false;
                }
                char hint[50];
                std::snprintf(hint, sizeof(hint), "%lu %lu", static_cast<unsigned long>(rewardVnum),
                              static_cast<unsigned long>(rewardCount));
                LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), item->GetID(), "MOONLIGHT_GET", hint);
                item->SetCount(item->GetCount() - 1);
                SendTreasureRewardNotice(*this, rewardVnum, rewardCount, reward);
                return true;
            }
            case 50060: {
                if (IsPolymorphed())
                {
                    ChatPacket(1, "%s", locale_find("\272\257\275\305\301\337\277\241\264\302 \303\245\300\273 \300\320\300\273\274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
                    return false;
                }
                const int chance = MINMAX(0, static_cast<int>(item->GetValue(1)), 100);
                if (GetLevel() < 50)
                {
                    ChatPacket(1, "%s", locale_find("\276\306\301\367 \275\302\270\266 \275\272\305\263\300\273 \274\366\267\303\307\322 \274\366 \300\326\264\302 \267\271\272\247\300\314 \276\306\264\325\264\317\264\331.", g_iUseLocale));
                    return false;
                }
                if (!test_server)
                {
                    const auto now = get_global_time();
                    const auto next = GetSkillNextReadTime(SKILL_HORSE);
                    if (now < next)
                    {
                        if (!FindAffect(513))
                        {
                            SkillLearnWaitMoreTimeMessage(next - now);
                            return false;
                        }
                        RemoveAffect(513);
                        ChatPacket(1, "%s", locale_find("\301\326\276\310\274\372\274\255\270\246 \305\353\307\330 \301\326\310\255\300\324\270\266\277\241\274\255 \272\374\301\256\263\252\277\324\275\300\264\317\264\331.", g_iUseLocale));
                    }
                }
                const int shared = GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE);
                if (GetPoint(POINT_HORSE_SKILL) >= 20 || shared + GetSkillLevel(SKILL_HORSE_WILDATTACK) >= 60 ||
                    shared + GetSkillLevel(SKILL_HORSE_WILDATTACK_RANGE) >= 60)
                {
                    ChatPacket(1, "%s", locale_find("\264\365 \300\314\273\363 \275\302\270\266 \274\366\267\303\274\255\270\246 \300\320\300\273 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
                    return false;
                }
                if (number_ex(1, 100, const_cast<char*>("char_item.cpp"), 0xc50) > chance)
                    ChatPacket(1, "%s", locale_find("\275\302\270\266 \274\366\267\303\274\255 \300\314\307\330\277\241 \275\307\306\320\307\317\277\264\275\300\264\317\264\331.", g_iUseLocale));
                else
                {
                    ChatPacket(1, "%s", locale_find("\275\302\270\266 \274\366\267\303\274\255\270\246 \300\320\276\356 \275\302\270\266 \275\272\305\263 \306\367\300\316\306\256\270\246 \276\362\276\372\275\300\264\317\264\331.", g_iUseLocale));
                    ChatPacket(1, "%s", locale_find("\276\362\300\272 \306\367\300\316\306\256\267\316\264\302 \275\302\270\266 \275\272\305\263\300\307 \267\271\272\247\300\273 \277\303\270\261 \274\366 \300\326\275\300\264\317\264\331.", g_iUseLocale));
                    PointChange(POINT_HORSE_SKILL, 1, false, false);
                    int delay = number_ex(64800, 108000, const_cast<char*>("char_item.cpp"), 0xc56);
                    if (distribution_test_server)
                        delay /= 3;
                    if (!test_server)
                        SetSkillNextReadTime(SKILL_HORSE, SkillBookReadDeadline(delay));
                }
                ITEM_MANAGER::instance().RemoveItem(item, nullptr);
                return true;
            }
            case 30093: case 30094: case 30095: case 30096: {
                struct Reward
                {
                    DWORD vnum;
                    DWORD hongKongVnum;
                    int weight;
                    unsigned count;
                };
                static constexpr Reward rewards[] = {
                    {1, 1, 302, 1000},
                    {27002, 27002, 150, 10},
                    {27003, 27002, 75, 10},
                    {27005, 27005, 100, 10},
                    {27006, 27005, 50, 10},
                    {27001, 27001, 80, 10},
                    {27002, 27002, 50, 10},
                    {27004, 27004, 80, 10},
                    {27005, 27005, 50, 10},
                    {50300, 50300, 10, 1},
                    {92, 92, 6, 1},
                    {132, 132, 2, 1},
                    {1052, 1052, 6, 1},
                    {1092, 1092, 2, 1},
                    {2082, 2082, 6, 1},
                    {2122, 2122, 2, 1},
                    {3082, 3082, 6, 1},
                    {3122, 3122, 2, 1},
                    {5052, 5052, 6, 1},
                    {5082, 5082, 2, 1},
                    {7082, 7082, 6, 1},
                    {7122, 7122, 2, 1},
                    {11282, 11282, 1, 1},
                    {11482, 11482, 1, 1},
                    {11682, 11682, 1, 1},
                    {11882, 11882, 1, 1},
                };
                const bool hongKong = LC_IsHongKong();
                int roll = number_ex(1, 1000, const_cast<char*>("char_item.cpp"), 0x977);
                for (const Reward& reward : rewards)
                {
                    if (roll > reward.weight)
                    {
                        roll -= reward.weight;
                        continue;
                    }

                    const DWORD rewardVnum = hongKong ? reward.hongKongVnum : reward.vnum;
                    if (rewardVnum == 50300)
                        GiveRandomSkillBook();
                    else if (rewardVnum == 1)
                        PointChange(POINT_GOLD, static_cast<int>(reward.count), true, false);
                    else
                        AutoGiveItem(rewardVnum, static_cast<BYTE>(reward.count), -1, true);
                    ITEM_MANAGER::instance().RemoveItem(item, nullptr);
                    return true;
                }
                return false;
            }
            case 27989:
            case 50004: {
                const bool eventMap = vnum == 50004;
                long attempt = item->GetSockets()[0];
                long targetX = item->GetSockets()[1];
                long targetY = item->GetSockets()[2];
                if (!attempt)
                {
                    PIXEL_POSITION location;
                    if (!SECTREE_MANAGER::instance().GetRandomLocation(GetMapIndex(), location, eventMap ? 700 : 0, 0, 0))
                    {
                        ChatPacket(1, "%s", locale_find(eventMap ? "\300\314 \260\367\277\241\274\261 \300\314\272\245\306\256\277\353 \260\250\301\366\261\342\260\241 \265\277\300\333\307\317\301\366 \276\312\264\302\260\315 \260\260\275\300\264\317\264\331." : "\300\314 \260\367\277\241\274\255 \277\265\274\256 \260\250\301\366\261\342\260\241 \265\277\300\333\307\317\301\366 \276\312\264\302\260\315 \260\260\275\300\264\317\264\331.", g_iUseLocale));
                        return false;
                    }
                    attempt = 1;
                    targetX = location.x;
                    targetY = location.y;
                    item->SetSocket(0, attempt);
                    item->SetSocket(1, targetX);
                    item->SetSocket(2, targetY);
                }
                else
                    item->SetSocket(0, ++attempt);

                const auto sendDirection = [this](int distanceCode, int direction) {
                    constexpr int headerSize = 9;
                    BYTE packet[headerSize + 85] = {};
                    const int length = std::snprintf(reinterpret_cast<char*>(packet + headerSize), 85,
                        "StoneDetect %lu %d %d", static_cast<unsigned long>(static_cast<DWORD>(GetVID())),
                        distanceCode, direction);
                    if (length < 0 || length >= 85)
                        return;
                    const int size = headerSize + length + 1;
                    packet[0] = 4;
                    packet[1] = static_cast<BYTE>(size);
                    packet[2] = static_cast<BYTE>(size >> 8);
                    packet[3] = 5;
                    packet[8] = GetDesc() ? GetDesc()->GetEmpire() : GetEmpire();
                    PacketAround(packet, size);
                };

                const auto dx = static_cast<std::uint32_t>(GetX()) - static_cast<std::uint32_t>(targetX);
                const auto dy = static_cast<std::uint32_t>(GetY()) - static_cast<std::uint32_t>(targetY);
                const int squaredDistance = auto_give_detail::signed32(dx * dx + dy * dy);
                const float distance = static_cast<float>(std::sqrt(static_cast<double>(squaredDistance)));
                // The original treats an unordered distance as the farthest bucket.
                if (!(distance < 1000))
                {
                    if (attempt < (eventMap ? 10 : 6))
                    {
                        const int distanceCode = distance < 20000 ? 1 : distance < 70000 ? 2 : 3;
                        const long double heading = GetDegreeFromPositionXY(GetX(), targetY, targetX, GetY());
                        const int angle = std::isfinite(heading) ? static_cast<int>(heading) : INT32_MIN;
                        sendDirection(distanceCode, angle);
                    }
                    else if (eventMap)
                    {
                        ChatPacket(1, "%s", locale_find("\300\314\272\245\306\256\277\353 \260\250\301\366\261\342\260\241 \310\347\300\373\265\265 \276\370\300\314 \273\347\266\363\301\375\264\317\264\331.", g_iUseLocale));
                        ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 0");
                        AutoGiveItem(27002, 1, -1, true);
                    }
                    else
                    {
                        ChatPacket(1, "%s", locale_find("\277\265\274\256 \260\250\301\366\261\342\260\241 \310\347\300\373\265\265 \276\370\300\314 \273\347\266\363\301\375\264\317\264\331.", g_iUseLocale));
                        item->SetCount(0);
                    }
                    return true;
                }

                if (!eventMap)
                {
                    ChatPacket(1, "%s", locale_find("\277\265\274\256 \260\250\301\366\261\342\260\241 \275\305\272\361\267\316\277\356 \272\373\300\273 \263\273\270\347 \273\347\266\363\301\375\264\317\264\331.", g_iUseLocale));
                    ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_STONE) 1");
                    AutoGiveItem(27999, 1, -1, true);
                }
                else
                {
                    ChatPacket(1, "%s", locale_find("\300\314\272\245\306\256\277\353 \260\250\301\366\261\342\260\241 \275\305\272\361\267\316\277\356 \272\373\300\273 \263\273\270\347 \273\347\266\363\301\375\264\317\264\331.", g_iUseLocale));
                    struct Reward
                    {
                        DWORD vnum;
                        DWORD count;
                        int weight;
                    };
                    static constexpr Reward tables[3][15] = {
                        {
                            {27001, 10, 8},
                            {27004, 10, 6},
                            {27002, 10, 12},
                            {27005, 10, 12},
                            {27100, 1, 9},
                            {27103, 1, 9},
                            {27101, 1, 10},
                            {27104, 1, 10},
                            {27999, 1, 12},
                            {25040, 1, 4},
                            {27410, 1, 0},
                            {27600, 1, 0},
                            {25100, 1, 0},
                            {50001, 1, 0},
                            {50003, 1, 1},
                        },
                        {
                            {27001, 10, 1},
                            {27004, 10, 1},
                            {27004, 10, 9},
                            {27005, 10, 9},
                            {27100, 1, 5},
                            {27103, 1, 5},
                            {27101, 1, 10},
                            {27104, 1, 10},
                            {27999, 1, 14},
                            {25040, 1, 5},
                            {27410, 1, 5},
                            {27600, 1, 5},
                            {25100, 1, 5},
                            {50001, 1, 0},
                            {50003, 1, 5},
                        },
                        {
                            {27001, 10, 0},
                            {27004, 10, 0},
                            {27002, 10, 0},
                            {27005, 10, 0},
                            {27100, 1, 0},
                            {27103, 1, 0},
                            {27101, 1, 0},
                            {27104, 1, 0},
                            {27999, 1, 25},
                            {25040, 1, 0},
                            {27410, 1, 0},
                            {27600, 1, 0},
                            {25100, 1, 15},
                            {50001, 1, 10},
                            {50003, 1, 50},
                        },
                    };
                    const auto& rewards = tables[attempt <= 4 ? 2 : attempt <= 7 ? 1 : 0];
                    int roll = number_ex(1, 100, const_cast<char*>("char_item.cpp"), 0xa09);
                    for (const Reward& reward : rewards)
                    {
                        if (roll > reward.weight)
                        {
                            roll -= reward.weight;
                            continue;
                        }
                        if (reward.vnum == 50001)
                        {
                            DWORD* data = new DWORD[3]{reward.vnum, reward.count, 0};
                            DBManager::instance().ReturnQuery(3, GetPlayerID(), data,
                                "INSERT INTO lotto_list VALUES(0, 'server%s', %lu, NOW())",
                                get_table_postfix(), static_cast<unsigned long>(GetPlayerID()));
                        }
                        else
                            AutoGiveItem(reward.vnum, static_cast<BYTE>(reward.count), -1, true);
                        break;
                    }
                }
                sendDirection(0, 0);
                if (eventMap)
                    ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 1");
                return true;
            }
            case 70302: {
                marriage::TMarriage* marriage = marriage::CManager::instance().Get(GetPlayerID());
                if (!marriage)
                {
                    ChatPacket(1, "%s", locale_find("\260\341\310\245 \273\363\305\302\260\241 \276\306\264\317\270\351 \260\341\310\245\271\335\301\366\270\246 \273\347\277\353\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
                    return true;
                }
                CArenaManager& arenas = CArenaManager::instance();
                if ((marriage->ch1 && arenas.IsArenaMap(marriage->ch1->GetMapIndex())) ||
                    (marriage->ch2 && arenas.IsArenaMap(marriage->ch2->GetMapIndex())))
                {
                    ChatPacket(1, "%s", locale_find("\264\353\267\303 \301\337\277\241\264\302 \300\314\277\353\307\322 \274\366 \276\370\264\302 \271\260\307\260\300\324\264\317\264\331.", g_iUseLocale));
                    return true;
                }

                const bool costsHP = LC_IsCIBN();
                const int cost = costsHP ? CalculateConsume(this) : CalculateConsumeSP(this);
                if (cost < 0)
                    return false;
                PointChange(costsHP ? POINT_HP : POINT_SP, -cost, false, false);
                WarpToPID(marriage->GetOther(GetPlayerID()));
                return true;
            }
            case 71092: {
                CHARACTER* target = m_pkChrTarget ? m_pkChrTarget : this;
                if (target->IsPolymorphed())
                {
                    target->SetPolymorph(0, false);
                    target->RemoveAffect(220);
                }
                return true;
            }
            case 90008:
            case 90009:
                VCardUse(this, this, item);
                return true;
        }
        return true;
    }
    switch (type) {
    case 0:
        _sys_err("UseItemEx", 0x1212, "Item type NONE %s", item->GetName());
        return true;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return true;
    case 10:
        return UseMetinStone(item, destination);
    case 18: {
        const DWORD vnum = item->GetVnum();
        if ((m_pArena || IsObserverMode()) && vnum >= 50051 && vnum <= 50053) {
            ChatPacket(1, "%s", locale_find("\264\353\267\303\040\301\337\277\241\264\302\040\300\314\277\353\307\322\040\274\366\040\276\370\264\302\040\271\260\307\260\300\324\264\317\264\331\056", g_iUseLocale));
            return false;
        }
        if (!(item->GetFlag() & (0x400 | 0x800)))
            quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
        return true;
    }
    case 19:
        return ItemProcess_Polymorph(item);
    case 16:
        if (item->GetSubType() == 10)
        {
            const DWORD vnum = item->GetVnum();
            sys_log(0, "ITEM_UNIQUE: USE_SPECIAL %lu", static_cast<unsigned long>(vnum));
            if (vnum == 71049)
                UseSilkBotary();
            return true;
        }
        [[fallthrough]];
    case 1:
    case 2:
    case 13:
    case 24:
        if (item->IsEquipped())
            UnequipItem(item);
        else
            EquipItem(item);
        return true;
    case 25:
        return ItemProcess_Hair(item, destination);
    case 27: {
        if (!Blend_Item_find(item->GetVnum()))
            return true;
        const long* sockets = item->GetSockets();
        const long apply = sockets[0];
        if (apply < 0 || apply >= MAX_APPLY_NUM)
            return false;
        const BYTE point = aApplyInfo[apply].bPointType;
        if (FindAffect(0x213, point)) {
            ChatPacket(1, "%s", locale_find("\300\314\271\314\040\310\277\260\372\260\241\040\260\311\267\301\040\300\326\275\300\264\317\264\331\056", g_iUseLocale));
            return true;
        }
        AddAffect(0x213, point, sockets[1], 0, sockets[2], 0, false, false);
        item->SetCount(item->GetCount() - 1);
        return true;
    }
    case 11:
        sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), type);
        return false;
    case 20:
        return false;
    case 21: {
        if (destination < 0 || destination >= 90)
            return false;
        LPITEM box = GetItem(static_cast<BYTE>(destination));
        if (!box || box->IsExchanging())
            return false;
        if (box->GetType() != 20) {
            ChatPacket(0, "%s", LC_TEXT("\277\255\274\350\267\316 \277\251\264\302 \271\260\260\307\300\314 \276\306\264\321\260\315 \260\260\264\331."));
            return false;
        }

        const char* wrongKey = "\277\255\274\350\260\241 \270\302\301\366 \276\312\264\302 \260\315 \260\260\264\331.";
        if (item->GetValue(0) != box->GetValue(0)) {
            ChatPacket(0, "%s", LC_TEXT(wrongKey));
            return false;
        }
        DWORD rewardVnum = 0;
        DWORD rewardCount = 0;
        LPITEM reward = nullptr;
        if (!GiveItemFromSpecialItemGroup(box->GetVnum(), rewardVnum, rewardCount, reward)) {
            ChatPacket(0, "%s", LC_TEXT(wrongKey));
            return false;
        }
        ITEM_MANAGER& items = ITEM_MANAGER::instance();
        items.RemoveItem(item, nullptr);
        items.RemoveItem(box, nullptr);
        SendTreasureRewardNotice(*this, rewardVnum, rewardCount, reward);
        return true;
    }
    case 23: {
        const DWORD boxVnum = item->GetVnum();
        if (boxVnum == 50033 && LC_IsYMIR() && GetLevel() < 15) {
            ChatPacket(1, "%s", "15\267\271\272\247 \300\314\307\317\277\241\274\255\264\302 \273\347\277\353\307\322 \274\366 \276\370\275\300\264\317\264\331.");
            return false;
        }
        DWORD rewardVnum = 0;
        DWORD rewardCount = 0;
        LPITEM reward = nullptr;
        if (!GiveItemFromSpecialItemGroup(boxVnum, rewardVnum, rewardCount, reward)) {
            ChatPacket(0, "%s", LC_TEXT("\276\306\271\253\260\315\265\265 \276\362\300\273 \274\366 \276\370\276\372\275\300\264\317\264\331."));
            return false;
        }
        item->SetCount(item->GetCount() - 1);
        SendTreasureRewardNotice(*this, rewardVnum, rewardCount, reward);
        return true;
    }
    case 26:
        if (!item->IsEquipped())
            EquipItem(item);
        return true;
    default:
        break;
    }

    if (type != 12 && type != 15 && !(type == 3 && item->GetSubType() == 6)) {
        if (type != 3)
            sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), type);
        return false;
    }

    if (type == 3) {
        if (m_pkFishingEvent) {
            ChatPacket(1, locale_find("\263\254\275\303 \301\337\277\241 \271\314\263\242\270\246 \260\245\276\306\263\242\277\357 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
            return false;
        }
        LPITEM rod = GetWear(4);
        if (!rod || rod->GetType() != 13)
            return false;
        const char* notice = rod->GetSockets()[2] ? "\300\314\271\314 \262\310\307\364\300\326\264\370 \271\314\263\242\270\246 \273\251\260\355 %s\270\246 \263\242\277\363\264\317\264\331." : "\263\254\275\303\264\353\277\241 %s\270\246 \271\314\263\242\267\316 \263\242\277\363\264\317\264\331.";
        ChatPacket(1, locale_find(notice, g_iUseLocale), item->GetName());
        rod->SetSocket(2, item->GetValue(0));
        item->SetCount(item->GetCount() - 1);
        return true;
    }

    if (type == 12) {
        if (CArenaManager::instance().IsArenaMap(GetMapIndex())) {
            ChatPacket(1, locale_find("\264\353\267\303 \301\337\277\241\264\302 \300\314\277\353\307\322 \274\366 \276\370\264\302 \271\260\307\260\300\324\264\317\264\331.", g_iUseLocale));
            return false;
        }
        if (item->GetSubType() == 0)
            ::fishing::UseFish(this, item);
        return true;
    }

    float dx;
    float dy;
    GetDeltaByDegree(m_pointsInstant.fRot, 100.0f, &dx, &dy);
    const auto campfireCoordinate = [](long origin, float offset) {
        const long double value = static_cast<std::int32_t>(origin) + static_cast<long double>(offset);
        if (!(value > -2147483649.0L && value < 2147483648.0L))
            return INT32_MIN;
        return static_cast<std::int32_t>(value);
    };
    const auto x = campfireCoordinate(GetX(), dx);
    const auto y = campfireCoordinate(GetY(), dy);
    const long map = GetMapIndex();
    LPSECTREE sector = SECTREE_MANAGER::instance().Get(map, x, y);
    if (!sector) {
        ChatPacket(1, locale_find("\270\360\264\332\272\322\300\273 \307\307\277\357 \274\366 \276\370\264\302 \301\366\301\241\300\324\264\317\264\331.", g_iUseLocale));
        return false;
    }
    if (sector->IsAttr(x, y, ATTR_WATER)) {
        ChatPacket(1, locale_find("\271\260 \274\323\277\241 \270\360\264\332\272\322\300\273 \307\307\277\357 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }

    const int rotation = number_ex(0, 359, const_cast<char*>("char_item.cpp"), 0x68e);
    LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(12000, map, x, y, 0, false, rotation, true);
    if (!campfire)
        return false;
    void* raw = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(std::uint32_t), &raw);
    *static_cast<std::uint32_t*>(raw) = static_cast<std::uint32_t>(static_cast<DWORD>(campfire->GetVID()));
    event_create_ex(kill_campfire_event, id, passes_per_sec * 40, const_cast<char*>("char_item.cpp"), 0x694);
    item->SetCount(item->GetCount() - 1);
    return true;
}

bool CHARACTER::GiveItem(LPCHARACTER target, BYTE cell)
{
    if (!target || !CanHandleItem(false, false))
        return false;
    LPITEM item = GetItem(cell);
    if (!item || item->IsExchanging() || !target->CanReceiveItem(this, item))
        return false;
    target->ReceiveItem(this, item);
    return true;
}

bool CHARACTER::CanReceiveItem(LPCHARACTER from, LPITEM item)
{
    if (GetDesc())
        return false;
    const int dx = auto_give_detail::signed32(static_cast<unsigned>(GetX()) -
                                              static_cast<unsigned>(from->GetX()));
    const int dy = auto_give_detail::signed32(static_cast<unsigned>(GetY()) -
                                              static_cast<unsigned>(from->GetY()));
    if (DISTANCE_APPROX(dx, dy) > 2000)
        return false;

    const WORD race = GetRaceNum();
    const TItemTable* proto = item->GetProto();
    const bool refinable = proto && proto->dwRefinedVnum;
    switch (race) {
        case 20016:
            return refinable && proto->wRefineSet < 500;
        case 20091:
            return proto && proto->wRefineSet >= 500;
        case 20044:
        case 20074:
            return refinable && item->GetType() == 1;
        case 20045:
        case 20075:
            return refinable && item->GetType() == 2 &&
                   item->GetSubType() <= 2;
        case 20046:
        case 20076:
            return refinable && item->GetType() == 2 && item->GetSubType() > 2;
        default:
            break;
    }

    if (race < 20101 || race > 20109)
        return true;
    const DWORD vnum = proto ? proto->dwVnum : 0;
    const unsigned tier = (race - 20101) / 3;
    if (vnum == 50057 + tier) {
        if (IsDead())
            return true;
        from->ChatPacket(1, "%s", locale_find("\xc1\xd7\xc1\xf6 \xbe\xca\xc0\xba \xb8\xbb\xbf\xa1\xb0\xd4 \xbc\xb1\xc3\xca\xb8\xa6 \xb8\xd4\xc0\xcf \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", g_iUseLocale));
        return false;
    }
    if (vnum < 50054 || vnum > 50056)
        return true;
    if (vnum != 50054 + tier)
        return false;
    if (!IsDead())
        return true;
    from->ChatPacket(1, "%s", locale_find("\xc1\xd7\xc0\xba \xb8\xbb\xbf\xa1\xb0\xd4 \xbb\xe7\xb7\xe1\xb8\xa6 \xb8\xd4\xc0\xcf \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", g_iUseLocale));
    return false;
}

void CHARACTER::ReceiveItem(LPCHARACTER from, LPITEM item)
{
    if (GetDesc())
        return;

    const WORD race = GetRaceNum();
    const TItemTable* proto = item->GetProto();
    if (race == 20016 || race == 20091 ||
        (race >= 20044 && race <= 20046) || (race >= 20074 && race <= 20076)) {
        if (proto && proto->dwRefinedVnum) {
            from->SetRefineNPC(this);
            from->RefineInformation(item->GetCell(), race >= 20074 && race <= 20076 ? 4 : 0, -1);
        }
        return;
    }

    if (race >= 20101 && race <= 20109) {
        const DWORD vnum = proto ? proto->dwVnum : 0;
        if (vnum >= 50054 && vnum <= 50056) {
            from->FeedHorse();
            from->ChatPacket(1, "%s", locale_find("\xb8\xbb\xbf\xa1\xb0\xd4 \xbb\xe7\xb7\xe1\xb8\xa6 \xc1\xd6\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", g_iUseLocale));
            item->SetCount(item->GetCount() - 1);
            EffectPacket(1);
        } else if (vnum >= 50057 && vnum <= 50059) {
            from->ReviveHorse();
            item->SetCount(item->GetCount() - 1);
            from->ChatPacket(1, "%s", locale_find("\xb8\xbb\xbf\xa1\xb0\xd4 \xbc\xb1\xc3\xca\xb8\xa6 \xc1\xd6\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.", g_iUseLocale));
        }
        return;
    }

    if (race == 12000 && item->GetType() == 12 && item->GetSubType() <= 1) {
        ::fishing::Grill(from, item);
        return;
    }
    if (race != 12000)
        sys_log(0, "TakeItem %s %d %s", from->GetName(), static_cast<int>(race), item->GetName());
    from->SetQuestNPCID(GetVID());
    quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), race, item);
}

// Potion branches of UseItemEx (081746b0).
bool CHARACTER::UsePotionItem(CItem* item, BYTE subtype)
{
    constexpr BYTE recovery = 0;
    constexpr BYTE immediate = 11;
    constexpr BYTE continuous = 16;
    const DWORD vnum = item->GetVnum();
    const auto scale = [](long value, long factor) {
        return auto_give_detail::signed32(static_cast<unsigned>(value) *
                                             static_cast<unsigned>(factor)) / 100;
    };
    const auto potionRate = [this] {
        return auto_give_detail::signed32(100U + static_cast<unsigned>(GetPoint(POINT_POTION_BONUS)));
    };

    if (subtype != continuous && CArenaManager::instance().IsArenaMap(GetMapIndex())) {
        auto& quests = quest::CQuestManager::instance();
        const bool allowed = subtype == recovery ? vnum >= 27001 && vnum <= 27006 :
                             vnum == 70020 || (vnum >= 71018 && vnum <= 71020);
        if (quests.GetEventFlag("arena_potion_limit") > 0 || !allowed) {
            ChatPacket(1, "%s", locale_find("\264\353\267\303\300\345\277\241\274\255 \273\347\277\353\307\317\275\307 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
            return false;
        }
        if (quests.GetEventFlag("arena_potion_limit_count") < 10000 && m_nPotionLimit <= 0) {
            ChatPacket(1, "%s", locale_find("\273\347\277\353 \301\246\307\321\267\256\300\273 \303\312\260\372\307\317\277\264\275\300\264\317\264\331.", g_iUseLocale));
            return false;
        }
    }

    const long hp = item->GetValue(0);
    const long sp = item->GetValue(1);
    if (subtype == recovery) {
        if (sp) {
            if (GetPoint(POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
                return false;
            PointChange(POINT_SP_RECOVERY, scale(sp, std::min(200, potionRate())));
            StartAffectEvent();
            EffectPacket(2);
        }
        if (hp) {
            if (GetPoint(POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
                return false;
            PointChange(POINT_HP_RECOVERY, scale(hp, std::min(200, potionRate())));
            StartAffectEvent();
            EffectPacket(1);
        }
    } else if (subtype == continuous) {
        if (hp)
            AddAffect(218, POINT_HP_RECOVER_CONTINUE, hp, 0, item->GetValue(2), 0, true, false);
        else if (sp)
            AddAffect(219, POINT_SP_RECOVER_CONTINUE, sp, 0, item->GetValue(2), 0, true, false);
        else
            return false;
    } else {
        bool used = false;
        if (hp && GetHP() < GetMaxHP()) {
            PointChange(POINT_HP, scale(hp, potionRate()));
            EffectPacket(1);
            used = true;
        }
        if (sp && GetSP() < GetMaxSP()) {
            PointChange(POINT_SP, scale(sp, potionRate()));
            EffectPacket(2);
            used = true;
        }
        const long hpPercent = item->GetValue(3);
        if (hpPercent && GetHP() < GetMaxHP()) {
            PointChange(POINT_HP, scale(hpPercent, GetMaxHP()));
            EffectPacket(1);
            used = true;
        }
        const long spPercent = item->GetValue(4);
        if (spPercent && GetSP() < GetMaxSP()) {
            PointChange(POINT_SP, scale(spPercent, GetMaxSP()));
            EffectPacket(2);
            used = true;
        }
        if (!used)
            return true;
        if (vnum == 50085 || vnum == 50086) {
            if (test_server)
                ChatPacket(1, "%s", locale_find("\277\371\272\264 \266\307\264\302 \301\276\300\332 \270\246 \273\347\277\353\307\317\277\264\275\300\264\317\264\331", g_iUseLocale));
            m_iSeedTime = thecore_pulse();
        }
    }

    if (m_pkDungeon)
        m_pkDungeon->UsePotion(this);
    if (m_pWarMap)
        m_pWarMap->UsePotion(this, item);

    if (subtype == immediate)
        --m_nPotionLimit;
    item->SetCount(item->GetCount() - 1);
    if (subtype == recovery)
        --m_nPotionLimit;
    return true;
}

// ITEM_USE subtypes 0, 7, 8, 9, 11 and 16 in UseItemEx (081746b0).
bool CHARACTER::UseConsumableItem(CItem* item, BYTE subtype)
{
    const DWORD vnum = item->GetVnum();
    const bool additionalEffect = vnum >= 50801 && vnum <= 50820;

    switch (subtype) {
    case 7:
        return UseAbilityItem(item, additionalEffect ? 0x204 : 0);
    case 8: {
        const long apply = item->GetValue(1);
        if (apply < 0 || apply >= MAX_APPLY_NUM)
            return false;
        const BYTE point = aApplyInfo[apply].bPointType;
        const DWORD affect = additionalEffect ? 0x204 : item->GetValue(0);
        if (FindAffect(affect, point)) {
            ChatPacket(1, "%s", locale_find("\300\314\271\314 \310\277\260\372\260\241 \260\311\267\301 \300\326\275\300\264\317\264\331.", g_iUseLocale));
            return true;
        }
        if (item->IsPCBangItem() && (!GetDesc() || !IsPCBangIP(GetDesc()->GetHostName()))) {
            ChatPacket(1, "%s", locale_find("\300\314 \276\306\300\314\305\333\300\272 PC\271\346\277\241\274\255\270\270 \273\347\277\353\307\322 \274\366 \300\326\275\300\264\317\264\331.", g_iUseLocale));
            return false;
        }
        AddAffect(affect, point, item->GetValue(2), 0, item->GetValue(3), 0, false, additionalEffect);
        item->SetCount(item->GetCount() - 1);
        return true;
    }
    case 9: {
        const DWORD reward = number_ex(28000, 28013, const_cast<char*>("char_item.cpp"), 0x114b);
        AutoGiveItem(reward, 1, -1, true);
        item->SetCount(item->GetCount() - 1);
        return true;
    }
    default:
        return UsePotionItem(item, subtype);
    }
}

bool CHARACTER::UseAbilityItem(CItem* item, DWORD sharedAffect)
{
    static constexpr struct {
        long apply;
        BYTE point;
        DWORD affect;
        DWORD flag;
    } abilities[] = {
        {3, POINT_HT, 0xce, 0},
        {4, POINT_IQ, 0xcf, 0},
        {5, POINT_ST, 0xcc, 0},
        {6, POINT_DX, 0xcd, 0},
        {7, POINT_ATT_SPEED, 0xc9, 13},
        {8, POINT_MOV_SPEED, 0xc8, 12},
        {9, POINT_CASTING_SPEED, 0xd9, 0},
        {37, POINT_RESIST_MAGIC, 0, 0},
        {53, POINT_ATT_GRADE_BONUS, 0xca, 0},
        {54, POINT_DEF_GRADE_BONUS, 0xe2, 0},
    };

    const long apply = item->GetValue(0);
    for (const auto& ability : abilities) {
        if (ability.apply == apply && (ability.affect || sharedAffect)) {
            AddAffect(sharedAffect ? sharedAffect : ability.affect, ability.point, item->GetValue(2),
                      ability.flag, item->GetValue(1), 0, true, sharedAffect != 0);
            break;
        }
    }

    if (m_pkDungeon)
        m_pkDungeon->UsePotion(this);
    if (m_pWarMap)
        m_pWarMap->UsePotion(this, item);
    item->SetCount(item->GetCount() - 1);
    return true;
}

// Item-use target validation from 081746b0.
bool CHARACTER::UseItemModification(CItem* item, int destination, BYTE subtype)
{
    if (destination < 0 || destination >= 90)
        return false;
    LPITEM target = GetItem(static_cast<BYTE>(destination));
    if (!target || target->IsExchanging())
        return false;

    // The original second-level jump table stops at subtype 22.
    if (subtype == 24)
        return true;
    char source[] = "char_item.cpp";
    char targetId[20];
    const DWORD sourceId = item->GetID();

    if (subtype == 17) {
        constexpr long brokenStone = 28960;
        const long* sockets = target->GetSockets();
        if (sockets[0] != brokenStone && sockets[1] != brokenStone && sockets[2] != brokenStone) {
            ChatPacket(1, "%s", LC_TEXT("\303\273\274\322\307\322 \274\256\300\314 \271\332\307\364\300\326\301\366 \276\312\275\300\264\317\264\331."));
            return false;
        }
        int destinationSocket = 0;
        for (int index = 0; index < 3; ++index) {
            const long socket = sockets[index];
            if (socket != brokenStone && socket != 0)
                target->SetSocket(destinationSocket++, socket);
        }
        for (; destinationSocket < 3; ++destinationSocket) {
            if (sockets[destinationSocket] > 0)
                target->SetSocket(destinationSocket, 1);
        }
        item->SetCount(item->GetCount() - 1);
        std::snprintf(targetId, sizeof(targetId), "%lu", target->GetID());
        LogManager::instance().ItemLog(GetPlayerID(), 0, 0, sourceId, "CLEAN_SOCKET", targetId);
        return true;
    }

    if (subtype == 20) {
        std::snprintf(targetId, sizeof(targetId), "%lu", target->GetID());
        if (!target->GetProto() || !target->IsAccessoryForSocket()) {
            ChatPacket(1, "%s", LC_TEXT("\300\314 \276\306\300\314\305\333\300\270\267\316 \274\322\304\317\300\273 \303\337\260\241\307\322 \274\366 \276\370\264\302 \276\306\300\314\305\333\300\324\264\317\264\331."));
            return true;
        }
        const int capacity = target->GetAccessorySocketMaxGrade();
        if (capacity >= 3) {
            ChatPacket(1, "%s", LC_TEXT("\300\314 \276\327\274\274\274\255\270\256\277\241\264\302 \264\365\300\314\273\363 \274\322\304\317\300\273 \303\337\260\241\307\322 \260\370\260\243\300\314 \276\370\275\300\264\317\264\331."));
            return true;
        }
        const bool success = number_ex(1, 100, source, 0x10bd) <= 50;
        if (success)
            target->SetAccessorySocketMaxGrade(capacity + 1);
        ChatPacket(1, "%s", LC_TEXT(success ? "\274\322\304\317\300\314 \274\272\260\370\300\373\300\270\267\316 \303\337\260\241\265\307\276\372\275\300\264\317\264\331." : "\274\322\304\317 \303\337\260\241\277\241 \275\307\306\320\307\317\277\264\275\300\264\317\264\331."));
        LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), sourceId,
                                       success ? "ADD_SOCKET_SUCCESS" : "ADD_SOCKET_FAIL", targetId);
        item->SetCount(item->GetCount() - 1);
        return true;
    }

    if (subtype == 21) {
        if (!target->GetProto() || !target->IsAccessoryForSocket() || !item->CanPutInto(target)) {
            ChatPacket(1, "%s", LC_TEXT("\300\314 \276\306\300\314\305\333\300\273 \300\345\302\370\307\322 \274\366 \276\370\275\300\264\317\264\331."));
            return true;
        }
        std::snprintf(targetId, sizeof(targetId), "%lu", target->GetID());
        const int capacity = target->GetAccessorySocketMaxGrade();
        const int grade = target->GetAccessorySocketGrade();
        if (grade < capacity) {
            const int roll = number_ex(1, 100, source, 0x10e4);
            const bool success = grade >= 0 && grade < 4 && roll <= aiAccessorySocketPutPct[grade];
            if (success)
                target->SetAccessorySocketGrade(grade + 1);
            ChatPacket(1, "%s", LC_TEXT(success ? "\300\345\302\370\277\241 \274\272\260\370\307\317\277\264\275\300\264\317\264\331." : "\300\345\302\370\277\241 \275\307\306\320\307\317\277\264\275\300\264\317\264\331."));
            LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), sourceId,
                                           success ? "PUT_SOCKET_SUCCESS" : "PUT_SOCKET_FAIL", targetId);
            item->SetCount(item->GetCount() - 1);
        } else if (!capacity) {
            ChatPacket(1, "%s", LC_TEXT("\270\325\300\372 \264\331\300\314\276\306\270\363\265\345\267\316 \276\307\274\274\274\255\270\256\277\241 \274\322\304\317\300\273 \303\337\260\241\307\330\276\337\307\325\264\317\264\331."));
        } else if (capacity < 3) {
            ChatPacket(1, "%s", LC_TEXT("\300\314 \276\327\274\274\274\255\270\256\277\241\264\302 \264\365\300\314\273\363 \300\345\302\370\307\322 \274\322\304\317\300\314 \276\370\275\300\264\317\264\331."));
            ChatPacket(1, "%s", LC_TEXT("\264\331\300\314\276\306\270\363\265\345\267\316 \274\322\304\317\300\273 \303\337\260\241\307\330\276\337\307\325\264\317\264\331."));
        } else {
            ChatPacket(1, "%s", LC_TEXT("\300\314 \276\327\274\274\274\255\270\256\277\241\264\302 \264\365\300\314\273\363 \272\270\274\256\300\273 \300\345\302\370\307\322 \274\366 \276\370\275\300\264\317\264\331."));
        }
        return true;
    }

    if (target->GetAttributeSetIndex() == -1) {
        ChatPacket(1, "%s", LC_TEXT("\274\323\274\272\300\273 \272\257\260\346\307\322 \274\366 \276\370\264\302 \276\306\300\314\305\333\300\324\264\317\264\331."));
        return false;
    }
    const int count = target->GetAttributeCount();
    if (subtype == 18) {
        if (count == 0) {
            ChatPacket(1, "%s", LC_TEXT("\272\257\260\346\307\322 \274\323\274\272\300\314 \276\370\275\300\264\317\264\331."));
            return false;
        }
        if (GetGMLevel() == 0) {
            quest::CQuestManager& quests = quest::CQuestManager::instance();
            std::uint32_t cycle = quests.GetEventFlag("change_itemattr_cycle");
            if (cycle < 10)
                cycle = 0;
            if (quest::PC* player = quests.GetPC(GetPlayerID())) {
                const auto now = static_cast<std::uint32_t>(get_global_time() / 60);
                const auto last = static_cast<std::uint32_t>(player->GetFlag("Item.LastChangeItemAttr"));
                sys_log(0, "UseChangeAttribute cycle=%ld, now=%ld, last=%ld",
                        static_cast<long>(auto_give_detail::signed32(cycle)),
                        static_cast<long>(auto_give_detail::signed32(now)),
                        static_cast<long>(auto_give_detail::signed32(last)));
                if (now < cycle + last) {
                    ChatPacket(1, LC_TEXT("\274\323\274\272\300\273 \271\331\262\333\301\366 %d\272\320 \300\314\263\273\277\241\264\302 \264\331\275\303 \272\257\260\346\307\322 \274\366 \276\370\275\300\264\317\264\331.(%d \272\320 \263\262\300\275)"),
                               auto_give_detail::signed32(cycle), auto_give_detail::signed32(cycle - (now - last)));
                    return false;
                }
                player->SetFlag("Item.LastChangeItemAttr", auto_give_detail::signed32(now), false);
            }
        }
        target->ChangeAttribute();
        item->SetCount(item->GetCount() - 1);
        ChatPacket(1, "%s", LC_TEXT("\274\323\274\272\300\273 \272\257\260\346\307\317\277\264\275\300\264\317\264\331."));
        std::snprintf(targetId, sizeof(targetId), "%lu", target->GetID());
        LogManager::instance().ItemLog(GetPlayerID(), 0, 0, sourceId, "CHANGE_ATTRIBUTE", targetId);
        return true;
    }

    const bool fifth = subtype == 22;
    if (!fifth && count >= 4) {
        ChatPacket(1, "%s", LC_TEXT("\264\365\300\314\273\363 \300\314 \276\306\300\314\305\333\300\273 \300\314\277\353\307\317\277\251 \274\323\274\272\300\273 \303\337\260\241\307\322 \274\366 \276\370\275\300\264\317\264\331."));
        return true;
    }
    if (fifth && count != 4) {
        if (count == 5)
            ChatPacket(1, "%s", LC_TEXT("\264\365 \300\314\273\363 \300\314 \276\306\300\314\305\333\300\273 \300\314\277\353\307\317\277\251 \274\323\274\272\300\273 \303\337\260\241\307\322 \274\366 \276\370\275\300\264\317\264\331."));
        else if (count < 4)
            ChatPacket(1, "%s", LC_TEXT("\270\325\300\372 \300\347\260\241\272\361\274\255\270\246 \300\314\277\353\307\317\277\251 \274\323\274\272\300\273 \303\337\260\241\275\303\304\321 \301\326\274\274\277\344."));
        else
            _sys_err("UseItemEx", 0x10b0, "ADD_ATTRIBUTE2 : Item has wrong AttributeCount(%d)", count);
        return true;
    }
    std::snprintf(targetId, sizeof(targetId), "%lu", target->GetID());
    const bool success = number_ex(1, 100, source, fifth ? 0x1094 : 0x106e) <= aiItemAttributeAddPercent[count];
    DWORD attrType = 0;
    DWORD attrValue = 0;
    const char* action;
    if (success) {
        target->AddAttribute();
        ChatPacket(1, "%s", LC_TEXT("\274\323\274\272 \303\337\260\241\277\241 \274\272\260\370\307\317\277\264\275\300\264\317\264\331."));
        const int addedIndex = target->GetAttributeCount() - 1;
        if (addedIndex >= 0) {
            const auto& attribute = target->GetAttributes()[addedIndex];
            attrType = attribute.bType;
            attrValue = attribute.sValue;
        }
        action = fifth ? "ADD_ATTRIBUTE2_SUCCESS" : "ADD_ATTRIBUTE_SUCCESS";
    } else {
        ChatPacket(1, "%s", LC_TEXT("\274\323\274\272 \303\337\260\241\277\241 \275\307\306\320\307\317\277\264\275\300\264\317\264\331."));
        action = fifth ? "ADD_ATTRIBUTE2_FAIL" : "ADD_ATTRIBUTE_FAIL";
    }
    LogManager::instance().ItemLog(GetPlayerID(), attrType, attrValue, sourceId, action, targetId);
    item->SetCount(item->GetCount() - 1);
    return true;
}

bool CHARACTER::UseRefineItem(CItem* item, int destination)
{
    const DWORD vnum = item->GetVnum();
    if (destination < 0 || destination >= 90)
        return false;
    CItem* target = GetItem(static_cast<BYTE>(destination));
    if (!target || target->IsExchanging())
        return false;

    const DWORD targetVnum = target->GetVnum();
    if (targetVnum >= 28330 && targetVnum <= 28343) {
        ChatPacket(1, "%s", locale_find("+3 \277\265\274\256\300\272 \276\306\300\314\305\333\300\270\267\316 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331", g_iUseLocale));
        return false;
    }
    if (targetVnum >= 28430 && targetVnum <= 28443 && vnum != 71056) {
        ChatPacket(1, "%s", locale_find("\277\265\274\256\300\272 \300\314 \276\306\300\314\305\333\300\270\267\316 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331", g_iUseLocale));
        return true;
    }

    RefineItem(item, target);
    return true;
}

// Detachment path of RefineItem (08173a00).
bool CHARACTER::RefineItem(CItem* item, CItem* target)
{
    constexpr long brokenStone = 28960;
    if (!CanHandleItem(false, false))
        return false;
    if (item->GetSubType() == 2) {
        BYTE type = 2;
        switch (item->GetValue(0)) {
        case 1: type = 3; break;
        case 3: type = 5; break;
        case 6: type = 6; break;
        }
        RefineInformation(target->GetCell(), type, item->GetCell());
        return false;
    }
    if (item->GetSubType() != 14 || !(target->GetFlag() & 1))
        return false;

    LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), target->GetID(),
                                  "USE_DETACHMENT", target->GetName());
    bool detached = false;
    for (int i = 0; i < 3; ++i) {
        const long stone = target->GetSockets()[i];
        if (stone > 2 && stone != brokenStone) {
            AutoGiveItem(stone, 1, -1, true);
            target->SetSocket(i, brokenStone);
            detached = true;
        }
    }
    if (!detached) {
        ChatPacket(1, "%s", locale_find("\273\251\263\276 \274\366 \300\326\264\302 \270\336\306\276\274\256\300\314 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }
    item->SetCount(item->GetCount() - 1);
    return true;
}

// 081809e0
void CHARACTER::SetRefineMode(int additionalCell)
{
    m_bUnderRefine = true;
    m_iRefineAdditionalCell = additionalCell;
}

// 08180a00
void CHARACTER::ClearRefineMode()
{
    m_bUnderRefine = false;
}

// 081735e0
bool CHARACTER::RefineInformation(BYTE cell, BYTE type, int additionalCell)
{
    if (cell > 90 || !GetDesc())
        return false;
    CItem* item = GetItem(cell);
    if (!item)
        return false;

    if (type == 4 && !GetQuestFlag("deviltower_zone.can_refine")) {
        ChatPacket(1, "%s", locale_find("\273\347\261\315 \305\270\277\366 \277\317\267\341 \272\270\273\363\300\272 \307\321\271\370\261\356\301\366 \273\347\277\353\260\241\264\311\307\325\264\317\264\331.", g_iUseLocale));
        return false;
    }

    TPacketGCRefineInformation packet{};
    packet.header = 0x77;
    packet.type = type;
    packet.pos = cell;
    packet.src_vnum = item->GetVnum();
    const TItemTable* proto = item->GetProto();
    packet.result_vnum = proto ? proto->dwRefinedVnum : 0;
    if (!packet.result_vnum) {
        _sys_err("RefineInformation", 0x42a, "RefineInformation p.result_vnum == 0");
        ChatPacket(1, "%s", locale_find("\300\314 \276\306\300\314\305\333\300\272 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }

    if (item->GetType() == 3 && item->GetSubType() == 2) {
        if (!type) {
            ChatPacket(1, "%s", locale_find("\300\314 \276\306\300\314\305\333\300\272 \300\314 \271\346\275\304\300\270\267\316\264\302 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
            return false;
        }
        CItem* scroll = GetItem(static_cast<BYTE>(additionalCell));
        if (!scroll || scroll->GetVnum() == item->GetVnum()) {
            ChatPacket(1, "%s", locale_find("\260\260\300\272 \260\263\267\256\274\255\270\246 \307\325\304\245 \274\366\264\302 \276\370\275\300\264\317\264\331.", g_iUseLocale));
            ChatPacket(1, "%s", locale_find("\303\340\272\271\300\307 \274\255\277\315 \307\366\303\266\300\273 \307\325\304\245 \274\366 \300\326\275\300\264\317\264\331.", g_iUseLocale));
            return false;
        }
    }

    const auto* recipe = CRefineManager::instance().GetRefineRecipe(proto->wRefineSet);
    if (!recipe) {
        _sys_err("RefineInformation", 0x448, "RefineInformation NOT GET REFINE SET %d", proto->wRefineSet);
        ChatPacket(1, "%s", locale_find("\300\314 \276\306\300\314\305\333\300\272 \260\263\267\256\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }

    if (GetQuestFlag("main_quest_lv7.refine_chance") > 0) {
        if (!LC_IsJapan() && !item->CheckItemUseLevel(20)) {
            ChatPacket(1, "%s", locale_find("\271\253\267\341 \260\263\267\256 \261\342\310\270\264\302 20 \300\314\307\317\300\307 \271\253\261\342\270\270 \260\241\264\311\307\325\264\317\264\331", g_iUseLocale));
            return false;
        }
    } else {
        packet.cost = ComputeRefineFee(recipe->cost, 5);
    }
    packet.prob = recipe->prob;
    if (type != 4) {
        packet.material_count = recipe->material_count;
        static_assert(sizeof(packet.materials) == sizeof(recipe->materials));
        std::memcpy(packet.materials, recipe->materials, sizeof(packet.materials));
    }

    GetDesc()->Packet(&packet, sizeof(packet));
    SetRefineMode(additionalCell);
    return true;
}

bool CHARACTER::CanDoCube()
{
    return !IsObserverMode() && !m_pkShop && !m_pkMyShop && !m_bUnderRefine && !m_pkWarpEvent;
}
