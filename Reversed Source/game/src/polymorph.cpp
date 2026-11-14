#include "polymorph.h"
#include "char.h"
#include "item.h"
#include "mob_manager.h"
#include "locale.hpp"
#include "skill.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
#include <cstdint>

// 0816eb70
bool CPolymorphUtils::Initialize()
{
    m_mapSPDType.insert(std::make_pair(101UL, 101UL));
    m_mapSPDType.insert(std::make_pair(1901UL, 1901UL));
    return true;
}

POLYMORPH_BONUS_TYPE CPolymorphUtils::GetBonusType(DWORD race)
{
    const auto raceId = static_cast<std::uint32_t>(race);
    if (m_mapSPDType.find(raceId) != m_mapSPDType.end())
        return POLYMORPH_SPD_BONUS;
    if (m_mapATKType.find(raceId) != m_mapATKType.end())
        return POLYMORPH_ATK_BONUS;
    if (m_mapDEFType.find(raceId) != m_mapDEFType.end())
        return POLYMORPH_DEF_BONUS;
    return POLYMORPH_NO_BONUS;
}

// 0816ed70
bool CPolymorphUtils::PolymorphCharacter(CHARACTER* ch, CItem* item, const CMob* mob)
{
    const BYTE skill = static_cast<BYTE>(ch->GetSkillLevel(SKILL_POLYMORPH));
    const int mastery = ch->GetSkillMasterType(SKILL_POLYMORPH);
    if (mastery < 0 || mastery > 3)
        return false;

    const int duration = 10 + mastery * 5;
    const auto characterLevel = static_cast<std::uint32_t>(ch->GetLevel());
    const auto mobLevel = static_cast<std::uint32_t>(mob->m_table.bLevel);
    const auto levelDifference = characterLevel - mobLevel;
    const auto bookLevel = static_cast<std::uint32_t>(item->GetSockets()[2]);
    const auto chance = static_cast<std::int32_t>(levelDifference + bookLevel + skill + 29u);
    char source[] = "polymorph.cpp";
    if (chance <= 0 || number_ex(1, 100, source, 0x47) > chance) {
        ch->ChatPacket(1, "%s", LC_TEXT("\265\320\260\251\277\241\040\275\307\306\320\040\307\317\277\264\275\300\264\317\264\331"));
        return false;
    }

    const DWORD race = mob->m_table.dwVnum;
    ch->AddAffect(220, POINT_POLYMORPH, race, 35, duration, 0, true, false);
    const auto bonus = static_cast<std::int32_t>(static_cast<std::uint32_t>(item->GetSockets()[2]) + skill);
    BYTE point;
    switch (GetBonusType(mob->m_table.dwVnum)) {
    case POLYMORPH_ATK_BONUS: point = POINT_ATT_BONUS; break;
    case POLYMORPH_DEF_BONUS: point = POINT_DEF_BONUS; break;
    case POLYMORPH_SPD_BONUS: point = POINT_MOV_SPEED; break;
    default: return true;
    }
    ch->AddAffect(220, point, bonus, 35, duration - 1, 0, false, false);
    return true;
}

// 0816efc0
bool CPolymorphUtils::UpdateBookPracticeGrade(CHARACTER* ch, CItem* item)
{
    if (!ch || !item)
        return false;
    const auto practice = static_cast<std::int32_t>(item->GetSockets()[1]);
    if (practice < 1)
        ch->ChatPacket(1, "%s", LC_TEXT("\265\320\260\251\274\255\040\274\366\267\303\300\273\040\270\266\303\272\275\300\264\317\264\331\056\040\275\305\274\261\277\241\260\324\040\303\243\276\306\260\241\274\274\277\344\056"));
    else
        item->SetSocket(1, practice - 1);
    return true;
}

bool CPolymorphUtils::GiveBook(CHARACTER* ch, DWORD mobVnum, DWORD practice, BYTE bookLevel, BYTE levelLimit)
{
    if (!ch)
        return false;

    CItem* item = ch->AutoGiveItem(50322, 1, -1, true);
    if (!item)
        return false;
    const auto originalMobVnum = static_cast<std::uint32_t>(mobVnum);
    const auto originalPractice = static_cast<std::int32_t>(static_cast<std::uint32_t>(practice));
    if (!CMobManager::instance().Get(originalMobVnum))
    {
        _sys_err("GiveBook", 0x8f,
            "Wrong Polymorph vnum passed: CPolymorphUtils::GiveBook( PID(%d), %d %d %d %d)",
            static_cast<int>(ch->GetPlayerID()), static_cast<std::int32_t>(originalMobVnum), originalPractice,
            static_cast<int>(bookLevel), static_cast<int>(levelLimit));
        return false;
    }

    item->SetSocket(0, static_cast<std::int32_t>(originalMobVnum));
    item->SetSocket(1, originalPractice);
    item->SetSocket(2, bookLevel);
    return true;
}

bool CPolymorphUtils::BookUpgrade(CHARACTER* ch, CItem* item)
{
    if (!ch || !item)
        return false;
    const auto level = static_cast<std::uint32_t>(item->GetSockets()[2]);
    item->SetSocket(1, static_cast<std::int32_t>(level * 50u));
    const auto updatedLevel = static_cast<std::uint32_t>(item->GetSockets()[2]);
    item->SetSocket(2, static_cast<std::int32_t>(updatedLevel + 1u));
    return true;
}
