#include <cmath>
#include <cstdint>
#include <climits>
#include "vector.h"
#include "party.h"
#include "char_manager.h"
#include "skill.h"
#include "mob_manager.h"
#include "item.h"
#include "guild.h"
#include "constants.h"
#include "char.h"
#include "desc.h"
#include "packet.h"
#include "../../libthecore/include/memcpy.h"
#include "log.h"
#include "locale.hpp"
#include <algorithm>
#include <cstdio>
#include "locale_service.h"
#include "questmanager.h"
#include "questpc.h"
#include "battle.h"
#include "config.h"
#include "sectree.h"
#include "sectree_manager.h"
#include "utils.h"
#include <cstring>
#include <set>
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
// Original placement char_skill.cpp2611, STB_GLOBAL and NOT inline.
// New partial game/src/char_skill.cpp needs char.h, coreutils.h/log.h.
//08073560: target skill record is packed6 bytes; array has255 entries.
bool CHARACTER::IsUsableSkillMotion(DWORD motion)
{
    static const BYTE skillGroup[] = {
        0, 11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        12, 12, 12, 12, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21,
        21, 21, 21, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 22,
        22, 22, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 31, 31,
        31, 31, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 32, 32,
        32, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 41, 41, 41, 41,
        41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 42, 42, 42, 42, 42,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    static const BYTE motionSkills[][5] = {
        {0, 0, 0, 0, 0},
        {4, 1, 31, 61, 91},
        {4, 2, 32, 62, 92},
        {4, 3, 33, 63, 93},
        {4, 4, 34, 64, 94},
        {4, 5, 35, 65, 95},
        {4, 0, 0, 66, 96},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {4, 16, 46, 76, 106},
        {4, 17, 47, 77, 107},
        {4, 18, 48, 78, 108},
        {4, 19, 49, 79, 109},
        {4, 20, 50, 80, 110},
        {4, 0, 0, 81, 111},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {4, 1, 31, 61, 91},
        {4, 2, 32, 62, 92},
        {4, 3, 33, 63, 93},
        {4, 4, 34, 64, 94},
        {4, 5, 35, 65, 95},
        {4, 0, 0, 66, 96},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {4, 16, 46, 76, 106},
        {4, 17, 47, 77, 107},
        {4, 18, 48, 78, 108},
        {4, 19, 49, 79, 109},
        {4, 20, 50, 80, 110},
        {4, 0, 0, 81, 111},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {4, 1, 31, 61, 91},
        {4, 2, 32, 62, 92},
        {4, 3, 33, 63, 93},
        {4, 4, 34, 64, 94},
        {4, 5, 35, 65, 95},
        {4, 0, 0, 66, 96},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {4, 16, 46, 76, 106},
        {4, 17, 47, 77, 107},
        {4, 18, 48, 78, 108},
        {4, 19, 49, 79, 109},
        {4, 20, 50, 80, 110},
        {4, 0, 0, 81, 111},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {4, 1, 31, 61, 91},
        {4, 2, 32, 62, 92},
        {4, 3, 33, 63, 93},
        {4, 4, 34, 64, 94},
        {4, 5, 35, 65, 95},
        {4, 0, 0, 66, 96},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {4, 16, 46, 76, 106},
        {4, 17, 47, 77, 107},
        {4, 18, 48, 78, 108},
        {4, 19, 49, 79, 109},
        {4, 20, 50, 80, 110},
        {4, 0, 0, 81, 111},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 152, 0, 0, 0},
        {1, 153, 0, 0, 0},
        {1, 154, 0, 0, 0},
        {1, 155, 0, 0, 0},
        {1, 156, 0, 0, 0},
        {1, 157, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {2, 137, 140, 0, 0},
        {1, 138, 0, 0, 0},
        {1, 139, 0, 0, 0},
    };

    if (motion >= sizeof(motionSkills) / sizeof(motionSkills[0])) {
        _sys_err("IsUsableSkillMotion", 0xcb1, "OUT_OF_MOTION_VNUM: name=%s, motion=%lu/%d", GetName(), motion, 124);
        return false;
    }
    const auto& skills = motionSkills[motion];
    const unsigned group = GetSkillGroup() + 10 + GetJob() * 10;
    for (unsigned index = 1; index <= skills[0]; ++index) {
        const BYTE skill = skills[index];
        if (!skill || (skillGroup[skill] && skillGroup[skill] != group))
            continue;
        if (skill >= 151 && skill < 163) {
            if (m_pGuild && m_pGuild->GetSkillLevel(skill))
                return true;
        } else if (GetSkillLevel(skill)) {
            return true;
        }
    }
    return false;
}

void CHARACTER::SetSkillGroup(BYTE group)
{
    if (group < 3 && GetLevel() > 4)
    {
        m_points.skill_group = group;
        if (DESC* desc = GetDesc())
        {
            const BYTE packet[] = {0x70, group};
            desc->Packet(packet, sizeof(packet));
        }
    }
}

namespace {
const DWORD subSkillVnums[] = {121, 122, 124, 126, 127, 128, 129, 130, 131, 137, 138, 139, 140};
}

namespace {
int TruncateSkill(long double value);

struct MobSkillHit {
    CHARACTER* character;
    PIXEL_POSITION position;
    DWORD vnum;
    int key;
    BYTE level;
};
}

bool CHARACTER::HasMobSkill()
{
    return CountMobSkill() > 0;
}

int CHARACTER::CountMobSkill()
{
    if (!m_pkMobData)
        return 0;
    const TMobTable& table = m_pkMobData->m_table;
    if (table.dwVnum == 0x9bd)
        return 5;
    return table.Skills[0].dwVnum ? 1 + (table.Skills[1].dwVnum != 0) : 0;
}

const SMobSkillInfo* CHARACTER::GetMobSkill(int index)
{
    const CMob* mob = m_pkMobData;
    if (!mob)
        return nullptr;
    const int count = CountMobSkill();
    return index >= 0 && index < count ? &mob->m_mobSkillInfo[index] : nullptr;
}

bool CHARACTER::CanUseMobSkill(int index)
{
    if (!m_pkMobData) {
        _sys_err("CanUseMobSkill", 0xace, "NO-MOB use mob skill");
        return false;
    }
    if (GetRaceNum() != 0x9bd && index > 1)
        return false;
    if (!GetMobSkill(index)) {
        _sys_err("CanUseMobSkill", 0xadf, "no mob skill info");
        return false;
    }
    const auto now = get_dword_time();
    if (now < m_adwMobSkillCooltime[index])
        return false;
    const auto logTime = get_dword_time();
    sys_log(0, "MOB_SKILL now %lu cooltime %lu", static_cast<unsigned long>(logTime),
            static_cast<unsigned long>(m_adwMobSkillCooltime[index]));
    return number_ex(0, 1, const_cast<char*>("char_skill.cpp"), 0xae8) == 0;
}

namespace {
struct SkillGwihwanInfo {
    std::uint32_t pid;
    std::uint32_t skillLevel;
};
}

// 0806d900
EVENTFUNC(skill_gwihwan_event)
{
    const auto& info = *static_cast<SkillGwihwanInfo*>(event->info);
    CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(info.pid);
    if (!character)
        return 0;

    char source[] = "char_skill.cpp";
    if (number_ex(1, 100, source, 0x5d8) > static_cast<BYTE>(info.skillLevel) * 20 - 1) {
        character->ChatPacket(1, "%s", locale_find("\xb1\xcd\xc8\xaf\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", g_iUseLocale));
        return 0;
    }

    const BYTE empire = character->GetEmpire();
    PIXEL_POSITION position{};
    if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(character->GetMapIndex(), empire, position)) {
        sys_log(1, "Recall: %s %d %d -> %d %d", character->GetName(),
                static_cast<int>(character->GetX()), static_cast<int>(character->GetY()),
                static_cast<int>(position.x), static_cast<int>(position.y));
    } else {
        _sys_err("skill_gwihwan_event", 0x5e4,
                 "CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)",
                 character->GetName(), static_cast<int>(character->GetX()), static_cast<int>(character->GetY()));
        if (empire >= 1 && empire <= 3) {
            position.x = g_start_position[empire][0];
            position.y = g_start_position[empire][1];
        }
    }
    character->WarpSet(position.x, position.y, 0);
    return 0;
}

// 08073cf0
EVENTFUNC(mob_skill_hit_event)
{
    const auto& data = *static_cast<MobSkillHit*>(event->info);
    CHARACTER* character = data.character;
    const VID vid = character->GetVID();
    const int key = data.key;
    character->ComputeSkillAtPosition(data.vnum, data.position, data.level);
    // The hit can run callbacks that remove its caster or replace the pending hit.
    if (CHARACTER* caster = CHARACTER_MANAGER::instance().Find(vid)) {
        const auto pending = caster->m_mapMobSkillEvent.find(key);
        if (pending != caster->m_mapMobSkillEvent.end() && pending->second == event)
            caster->m_mapMobSkillEvent.erase(pending);
    }
    return 0;
}

bool CHARACTER::UseMobSkill(int index)
{
    if (GetDesc())
        return false;
    const SMobSkillInfo* info = GetMobSkill(index);
    if (!info)
        return false;
    const DWORD vnum = info->dwSkillVnum;
    const BYTE level = info->bSkillLevel;
    CSkillProto* skill = CSkillManager::instance().Get(vnum);
    sys_log(0, "%s: USE_MOB_SKILL: %d %d", GetName(), static_cast<int>(vnum), index);
    if (!skill)
        return false;

    const int skillPower = GetSkillPower(skill->dwVnum, level);
    const float power = static_cast<float>(
        static_cast<long double>(skill->bMaxLevel) * skillPower / 100.0L);
    skill->kCooldownPoly.SetVar("k", power);
    const int cooldown = TruncateSkill(skill->kCooldownPoly.Eval() * 1000.0L);
    m_adwMobSkillCooltime[index] = static_cast<std::uint32_t>(get_dword_time() + cooldown);
    if (info->vecSplashAttack.empty()) {
        _sys_err("UseMobSkill", 0xb18, "No skill hit data for mob %s index %d", GetName(), index);
        return false;
    }

    int nextKey = 0;
    for (const auto& splash : info->vecSplashAttack) {
        PIXEL_POSITION position = GetXYZ();
        if (splash.dwHitDistance) {
            float x, y;
            GetDeltaByDegree(GetRotation(), static_cast<float>(splash.dwHitDistance), &x, &y);
            position.x = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(position.x) + static_cast<std::uint32_t>(TruncateSkill(x)));
            position.y = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(position.y) + static_cast<std::uint32_t>(TruncateSkill(y)));
        }
        if (!splash.dwTiming) {
            if (test_server)
                sys_log(0, "Use Normal Skill from Mob [%s]", GetName());
            ComputeSkillAtPosition(vnum, position, info->bSkillLevel);
            continue;
        }

        if (test_server)
            sys_log(0, "Use Timeinng Skill from Mob mob[%s]", GetName());
        while (m_mapMobSkillEvent.count(nextKey))
            ++nextKey;
        void* data = nullptr;
        const DWORD id = __AllocEventInfo(sizeof(MobSkillHit), &data);
        if (!data)
            continue;
        *static_cast<MobSkillHit*>(data) = {this, position, vnum, nextKey, level};
        const auto scaledTiming = static_cast<std::uint32_t>(passes_per_sec) *
                                  static_cast<std::uint32_t>(splash.dwTiming);
        const long delay = scaledTiming / 1000u;
        if (LPEVENT event = event_create_ex(mob_skill_hit_event, id, delay, const_cast<char*>("char_skill.cpp"), 0xb37))
            m_mapMobSkillEvent.emplace(nextKey++, event);
    }
    return true;
}

void CHARACTER::ResetMobSkillCooltime()
{
    for (DWORD& cooldown : m_adwMobSkillCooltime)
        cooldown = 0;
}

void CHARACTER::DisableCooltime()
{
    m_bDisableCooltime = true;
}

void CHARACTER::SetSkillLevel(DWORD vnum, BYTE level)
{
    if (!m_pSkillLevels || vnum >= 255)
        return;
    TPlayerSkill& skill = m_pSkillLevels[vnum];
    skill.bLevel = level < 40 ? level : 40;
    if (level >= 20)
        skill.bMasterType = level >= 40 ? 3 : (level >= 30 ? 2 : 1);
}

void CHARACTER::ResetOneSkill(DWORD vnum)
{
    if (!m_pSkillLevels || vnum >= 255)
        return;
    TPlayerSkill& skill = m_pSkillLevels[vnum];
    const int refund = skill.bLevel < 17 ? skill.bLevel : 17;
    skill = {};
    PointChange(POINT_SKILL, refund, false, false);
    const auto x = GetX();
    const DWORD pid = GetPlayerID();
    const auto y = GetY();
    LogManager::instance().CharLog(pid, x, y, vnum, "ONE_SKILL_RESET_BY_SCROLL", "");
    ComputePoints();
    SkillLevelPacket();
}

// 0806cd80
void CHARACTER::ResetSkill()
{
    if (!m_pSkillLevels)
        return;

    TPlayerSkill saved[sizeof(subSkillVnums) / sizeof(subSkillVnums[0])];
    for (std::size_t i = 0; i < sizeof(saved) / sizeof(saved[0]); ++i)
        saved[i] = m_pSkillLevels[subSkillVnums[i]];
    std::memset(m_pSkillLevels, 0, sizeof(TPlayerSkill) * 255);
    for (std::size_t i = 0; i < sizeof(saved) / sizeof(saved[0]); ++i)
        m_pSkillLevels[subSkillVnums[i]] = saved[i];
    ComputePoints();
    SkillLevelPacket();
}

namespace {
int TruncateSkill(long double value);
}

// 0806d070
void CHARACTER::ComputePassiveSkill(DWORD vnum)
{
    if (g_bSkillDisable)
        return;
    if (!GetSkillLevel(vnum))
        return;
    CSkillProto* skill = CSkillManager::instance().Get(vnum);
    if (!skill)
        return;

    skill->SetPointVar("k", GetSkillLevel(vnum));
    const int amount = TruncateSkill(skill->kPointPoly.Eval());
    const char* name = GetName();
    sys_log(2, "%s passive #%d on %d amount %d", name, static_cast<int>(vnum),
            skill->bPointOn, amount);
    PointChange(skill->bPointOn, amount, false, false);
}

void CHARACTER::ClearSkill()
{
    const auto level = static_cast<std::uint32_t>(GetLevel());
    const auto points = static_cast<std::uint32_t>(GetPoint(POINT_SKILL));
    const auto delta = static_cast<std::int32_t>(level - points - 1U);
    PointChange(POINT_SKILL, delta, false, false);
    if (!m_pSkillLevels)
    {
        _sys_err("ClearSkill", 0xcee, "CLEAR_SKILL: Character have not m_pSkillLevels. CharName: %s", GetName());
        return;
    }

    TPlayerSkill saved[sizeof(subSkillVnums) / sizeof(subSkillVnums[0])];
    for (std::size_t i = 0; i < sizeof(saved) / sizeof(saved[0]); ++i)
        saved[i] = m_pSkillLevels[subSkillVnums[i]];
    std::memset(m_pSkillLevels, 0, sizeof(TPlayerSkill) * 255);
    for (std::size_t i = 0; i < sizeof(saved) / sizeof(saved[0]); ++i)
        m_pSkillLevels[subSkillVnums[i]] = saved[i];
    ComputePoints();
    SkillLevelPacket();
}

void CHARACTER::ClearSubSkill()
{
    const int level = GetLevel();
    int delta = 0;
    if (level > 9) {
        const auto points = static_cast<std::uint32_t>(GetPoint(POINT_SUB_SKILL));
        delta = static_cast<std::int32_t>(static_cast<std::uint32_t>(level) - points - 9U);
    }
    PointChange(POINT_SUB_SKILL, delta, false, false);
    if (!m_pSkillLevels)
    {
        _sys_err("ClearSubSkill", 0xd1b, "CLEAR_SKILL: Character have not m_pSkillLevels. CharName: %s", GetName());
        return;
    }

    for (DWORD vnum : subSkillVnums)
        m_pSkillLevels[vnum] = {};
    ComputePoints();
    SkillLevelPacket();
}

namespace {
const char grandSubSkill[] = "\261\327\267\243\265\345\040\270\266\275\272\305\315\040\274\366\267\303\300\273\040\307\322\040\274\366\040\276\370\264\302\040\275\272\305\263\300\324\264\317\264\331\056";
const char grandAlreadyPerfect[] = "\306\333\306\345\306\256\040\270\266\275\272\305\315\265\310\040\275\272\305\263\300\324\264\317\264\331\056\040\264\365\040\300\314\273\363\040\274\366\267\303\040\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056";
const char grandRequiresMaster[] = "\300\314\040\275\272\305\263\300\272\040\276\306\301\367\040\261\327\267\243\265\345\040\270\266\275\272\305\315\040\274\366\267\303\300\273\040\307\322\040\260\346\301\366\277\241\040\300\314\270\243\301\366\040\276\312\276\322\275\300\264\317\264\331\056";
const char skillUnlearnable[] = "\274\366\267\303\307\322\040\274\366\040\276\370\264\302\040\275\272\305\263\300\324\264\317\264\331\056";
const char grandFailTalk[] = "\305\251\300\271\054\040\261\342\260\241\040\277\252\267\371\307\317\260\355\040\300\326\276\356\041\040\300\314\260\305\040\274\263\270\266\040\301\326\310\255\300\324\270\266\300\316\260\241\041\077\040\301\250\300\345\041";
const char grandFailInfo[] = "\274\366\267\303\300\314\040\275\307\306\320\267\316\040\263\241\263\265\275\300\264\317\264\331\056\040\264\331\275\303\040\265\265\300\374\307\330\301\326\275\303\261\342\040\271\331\266\370\264\317\264\331\056";
const char grandSuccessTalk1[] = "\270\366\277\241\274\255\040\271\272\260\241\040\310\373\300\314\040\305\315\301\256\040\263\252\277\300\264\302\040\261\342\272\320\300\314\276\337\041";
const char grandSuccessTalk2[] = "\266\337\260\305\277\356\040\271\253\276\371\300\314\040\260\350\274\323\040\277\353\274\332\300\275\304\241\260\355\040\300\326\276\356\041\040\300\314\260\307\054\040\300\314\260\315\300\272\041";
const char grandSuccessInfo[] = "\264\365\040\263\364\300\272\040\260\346\301\366\300\307\040\274\366\267\303\300\273\040\274\272\260\370\300\373\300\270\267\316\040\263\241\263\273\274\314\275\300\264\317\264\331\056";
const char skillPolymorph[] = "\265\320\260\251\040\301\337\277\241\264\302\040\264\311\267\302\300\273\040\277\303\270\261\040\274\366\040\276\370\275\300\264\317\264\331\056";
}

namespace muyeong_detail {
inline int bits(unsigned int value);
inline int add(int a, int b);
}

time_t CHARACTER::GetSkillNextReadTime(DWORD vnum)
{
    return m_pSkillLevels && vnum < 255 ? m_pSkillLevels[vnum].tNextRead : 0;
}

void CHARACTER::SetSkillNextReadTime(DWORD vnum, time_t time)
{
    if (m_pSkillLevels && vnum < 255)
        m_pSkillLevels[vnum].tNextRead = static_cast<std::int32_t>(time);
}

bool CHARACTER::IsLearnableSkill(DWORD vnum)
{
    const CSkillProto* skill = CSkillManager::instance().Get(vnum);
    if (!skill || vnum >= 255)
        return false;
    const int level = GetSkillLevel(vnum);
    if (level >= 40)
        return false;
    if (skill->bType == 0)
        return GetSkillLevel(vnum) < skill->bMaxLevel;
    if (skill->bType == 5)
        return vnum != 140 || GetJob() == 1;
    return GetSkillGroup() != 0 && skill->bType - 1 == GetJob();
}

void CHARACTER::SkillLevelUp(DWORD vnum, BYTE method)
{
    if (!m_pSkillLevels || g_bSkillDisable)
        return;
    if (IsPolymorphed())
    {
        ChatPacket(1, locale_find(skillPolymorph, g_iUseLocale));
        return;
    }

    const CSkillProto* skill = CSkillManager::instance().Get(vnum);
    if (!skill)
    {
        _sys_err("SkillLevelUp", 0x24f, "There is no such skill by number %u", static_cast<unsigned>(vnum));
        return;
    }
    if (!IsLearnableSkill(vnum) || skill->dwVnum >= 255)
        return;
    const DWORD id = skill->dwVnum;
    if (skill->bType) {
        const int mastery = GetSkillMasterType(skill->dwVnum);
        if ((mastery == 2 && method != 3) || mastery == 3)
            return;
    }
    if (method == 0 && (GetSkillMasterType(skill->dwVnum) != 0 || (skill->dwFlag & 0x400000)))
        return;
    if (method == 1 && skill->bType && GetSkillMasterType(skill->dwVnum) != 1)
        return;
    if (GetLevel() < skill->bLevelLimit)
        return;
    if (skill->preSkillVnum && GetSkillMasterType(skill->preSkillVnum) == 0 &&
        GetSkillLevel(skill->preSkillVnum) < skill->preSkillLevel)
        return;
    if (!GetSkillGroup())
        return;

    if (method == 0)
    {
        BYTE point;
        if (skill->bType < 5)
            point = skill->bType == 0 ? POINT_SUB_SKILL : POINT_SKILL;
        else if (skill->bType == 5)
            point = POINT_HORSE_SKILL;
        else
        {
            _sys_err("SkillLevelUp", 0x296, "Wrong skill type %d skill vnum %d",
                static_cast<int>(skill->bType), static_cast<int>(id));
            return;
        }
        if (GetPoint(point) < 1)
            return;
        PointChange(point, -1, false, false);
    }

    const int before = GetSkillLevel(id);
    SetSkillLevel(id, m_pSkillLevels[id].bLevel + 1);
    if (skill->bType)
    {
        const int newMastery = GetSkillMasterType(id);
        if (newMastery == 0)
        {
            const char* flag = "reset_scroll.force_to_master_skill";
            if (GetSkillLevel(id) >= 17)
            {
                if (GetQuestFlag(flag) > 0)
                {
                    SetSkillLevel(id, 20);
                    SetQuestFlag(flag, 0);
                }
                else if (number_ex(1, 21 - std::min(20, GetSkillLevel(id)), const_cast<char*>("char_skill.cpp"), 0x2b5) == 1)
                    SetSkillLevel(id, 20);
            }
            else if (GetSkillLevel(id) >= 20 &&
                     number_ex(1, 21 - std::min(20, GetSkillLevel(id)), const_cast<char*>("char_skill.cpp"), 0x2d0) == 1)
                SetSkillLevel(id, 20);
        }
        else if (newMastery == 1 && GetSkillLevel(id) >= 30)
        {
            if (number_ex(1, 31 - std::min(30, GetSkillLevel(id)), const_cast<char*>("char_skill.cpp"), 0x2d9) == 1)
                SetSkillLevel(id, 30);
        }
        else if (newMastery == 2 && GetSkillLevel(id) >= 40)
            SetSkillLevel(id, 40);
    }

    char detail[1024];
    std::snprintf(detail, sizeof(detail), "SkillUp: %s %d %d %d[Before:%d] type %d",
        GetName(), static_cast<int>(id), static_cast<int>(m_pSkillLevels[id].bMasterType),
        static_cast<int>(m_pSkillLevels[id].bLevel), before, static_cast<int>(skill->bType));
    sys_log(0, "%s", detail);
    LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), 0, "SKILLUP", detail);
    Save();
    ComputePoints();
    SkillLevelPacket();
}

namespace {
const char bookUnlearnable[] = "\274\366\267\303\307\322 \274\366 \276\370\264\302 \275\272\305\263\300\324\264\317\264\331.";
const char bookNeedsExp[] = "\260\346\307\350\304\241\260\241 \272\316\301\267\307\317\277\251 \303\245\300\273 \300\320\300\273 \274\366 \276\370\275\300\264\317\264\331.";
const char bookDelayRemoved[] = "\301\326\276\310\274\372\274\255\270\246 \305\353\307\330 \301\326\310\255\300\324\270\266\277\241\274\255 \272\374\301\256\263\252\277\324\275\300\264\317\264\331.";
const char bookNeedsMaster[] = "\300\314 \275\272\305\263\300\272 \276\306\301\367 \303\245\300\270\267\316 \274\366\267\303\307\322 \260\346\301\366\277\241 \300\314\270\243\301\366 \276\312\276\322\275\300\264\317\264\331.";
const char bookAlreadyGrand[] = "\300\314 \275\272\305\263\300\272 \303\245\300\270\267\316 \264\365\300\314\273\363 \274\366\267\303\307\322 \274\366 \276\370\275\300\264\317\264\331.";
const char bookProgressTalk[] = "\265\345\265\360\276\356 \263\241\300\314 \272\270\300\314\264\302 \260\307\260\241...  \300\314 \261\342\274\372\300\272 \300\314\307\330\307\317\261\342\260\241 \263\312\271\253 \310\373\265\351\276\356..";
const char bookProgressTalkAlt[] = "\276\356\264\300\301\244\265\265 \300\314 \261\342\274\372\277\241 \264\353\307\330 \300\314\307\330\260\241 \265\307\276\372\301\366\270\270 \301\266\261\335 \272\316\301\267\307\321\265\355 \307\321\265\245..";
const char bookProgressTalkThird[] = "\277\255\275\311\310\367 \307\317\264\302 \271\350\277\362\300\273 \260\241\301\366\264\302 \260\315\270\270\300\314 \261\342\274\372\300\273 \271\350\277\357\274\366 \300\326\264\302 \300\257\300\317\307\321 \261\346\300\314\264\331..";
const char bookProgressInfo[] = "%d \261\307\300\273 \264\365 \300\320\276\356\276\337 \274\366\267\303\300\273 \277\317\267\341 \307\322 \274\366 \300\326\275\300\264\317\264\331.";
const char bookTestSuccess[] = "SUCC";
const char bookFailTalk[] = "\305\251\300\271, \261\342\260\241 \277\252\267\371\307\317\260\355 \300\326\276\356! \300\314\260\305 \274\263\270\266 \301\326\310\255\300\324\270\266\300\316\260\241!? \301\250\300\345!";
const char bookFailInfo[] = "\274\366\267\303\300\314 \275\307\306\320\267\316 \263\241\263\265\275\300\264\317\264\331. \264\331\275\303 \265\265\300\374\307\330\301\326\275\303\261\342 \271\331\266\370\264\317\264\331.";
const char bookSuccessTalk1[] = "\270\366\277\241\274\255 \271\272\260\241 \310\373\300\314 \305\315\301\256 \263\252\277\300\264\302 \261\342\272\320\300\314\276\337!";
const char bookSuccessTalk2[] = "\266\337\260\305\277\356 \271\253\276\371\300\314 \260\350\274\323 \277\353\274\332\300\275\304\241\260\355 \300\326\276\356! \300\314\260\307, \300\314\260\315\300\272!";
const char bookSuccessInfo[] = "\303\245\300\270\267\316 \264\365 \263\364\300\272 \260\346\301\366\300\307 \274\366\267\303\300\273 \274\272\260\370\300\373\300\270\267\316 \263\241\263\273\274\314\275\300\264\317\264\331.";
struct BookWaitMessage
{
    DWORD until;
    const char* first;
    const char* last;
};
const BookWaitMessage bookWaitMessages[] = {
    {180, nullptr, "\270\366 \274\323\300\314 \266\337\260\314\261\272. \307\317\301\366\270\270 \276\306\301\326 \306\355\276\310\307\330. \300\314\264\353\267\316 \261\342\270\246 \276\310\301\244\275\303\305\260\300\332."},
    {300, nullptr, "\261\327\267\241, \303\265\303\265\310\367. \301\273\264\365 \303\265\303\265\310\367, \261\327\267\257\263\252 \270\267\310\373 \276\370\300\314 \272\374\270\243\260\324!"},
    {600, nullptr, "\261\327\267\241, \300\314 \264\300\263\246\300\314\276\337. \303\274\263\273\277\241 \261\342\260\241 \276\306\301\326 \303\346\270\270\307\330."},
    {1800, "\264\331 \300\320\276\372\264\331! \300\314\301\246 \272\361\261\336\277\241 \300\373\307\364\300\326\264\302 \264\353\267\316 \300\374\275\305\277\241 \261\342\270\246 \265\271\270\256\261\342\270\270 \307\317\270\351,", "\261\327\260\315\300\270\267\316 \274\366\267\303\300\272 \263\241\263\255 \260\305\276\337!"},
    {3600, nullptr, "\300\314\301\246 \303\245\300\307 \270\266\301\366\270\267 \300\345\300\314\276\337! \274\366\267\303\300\307 \263\241\300\314 \264\253\277\241 \272\270\300\314\260\355 \300\326\276\356!"},
    {7200, nullptr, "\276\363\270\266 \276\310 \263\262\276\322\276\356! \301\266\261\335\270\270 \264\365!"},
    {10800, nullptr, "\301\301\276\322\276\356! \301\266\261\335\270\270 \264\365 \300\320\300\270\270\351 \263\241\300\314\264\331!"},
    {21600, "\303\245\300\345\265\265 \300\314\301\246 \276\363\270\266 \263\262\301\366 \276\312\276\322\261\272.", "\271\272\260\241 \270\366 \276\310\277\241 \310\373\300\314 \273\375\261\342\264\302 \261\342\272\320\300\316 \260\311."},
    {43200, "\300\314\301\246 \301\273 \275\275\275\275 \260\241\264\332\300\314 \300\342\310\367\264\302 \260\315 \260\260\300\272\265\245.", "\301\301\276\306, \300\314 \261\342\274\274\267\316 \260\350\274\323 \263\252\260\243\264\331!"},
    {64800, "\276\306\264\317 \276\356\266\273\260\324 \265\310 \260\324 \301\276\300\317 \300\320\276\356\265\265 \270\323\270\256\277\241 \276\310 \265\351\276\356\277\300\263\304.", "\260\370\272\316\307\317\261\342 \275\310\276\356\301\366\263\327."},
};
}

void CHARACTER::SkillLearnWaitMoreTimeMessage(DWORD seconds)
{
    for (const auto& message : bookWaitMessages)
    {
        if (seconds >= message.until)
            continue;
        if (message.first)
            ChatPacket(0, "%s", locale_find(message.first, g_iUseLocale));
        ChatPacket(0, "%s", locale_find(message.last, g_iUseLocale));
        return;
    }
    ChatPacket(0, "%s", locale_find("\273\375\260\242\270\270\305\255 \300\320\261\342\260\241 \275\261\301\366\260\241 \276\312\261\272. \300\314\307\330\265\265 \276\356\267\306\260\355 \263\273\277\353\265\265 \263\255\307\330\307\330.", g_iUseLocale));
    ChatPacket(0, "%s", locale_find("\300\314\267\241\274\255\276\337 \260\370\272\316\260\241 \276\310\265\310\264\331\261\270.", g_iUseLocale));
}

bool CHARACTER::SkillLevelDown(DWORD vnum)
{
    if (!m_pSkillLevels || g_bSkillDisable || IsPolymorphed())
        return false;
    const CSkillProto* skill = CSkillManager::instance().Get(vnum);
    if (!skill)
    {
        _sys_err("SkillLevelDown", 0x208, "There is no such skill by number %u", static_cast<unsigned>(vnum));
        return false;
    }
    if (!IsLearnableSkill(vnum))
        return false;
    const DWORD id = skill->dwVnum;
    if (id >= 255 || GetSkillMasterType(id) || !GetSkillGroup() ||
        !m_pSkillLevels[id].bLevel)
        return false;

    BYTE point;
    if (skill->bType < 5)
        point = skill->bType == 0 ? POINT_SUB_SKILL : POINT_SKILL;
    else if (skill->bType == 5)
        point = POINT_HORSE_SKILL;
    else
    {
        _sys_err("SkillLevelDown", 0x22c, "Wrong skill type %d skill vnum %d",
            static_cast<int>(skill->bType), static_cast<int>(id));
        return false;
    }
    PointChange(point, 1, false, false);
    const DWORD currentId = skill->dwVnum;
    if (m_pSkillLevels && currentId < 255)
        SetSkillLevel(currentId, m_pSkillLevels[currentId].bLevel - 1);

    const char* name = GetName();
    const DWORD logId = skill->dwVnum;
    if (m_pSkillLevels && logId < 255)
        sys_log(0, "SkillDown: %s %d %d %d type %d", name, static_cast<int>(logId),
            static_cast<int>(m_pSkillLevels[logId].bMasterType), static_cast<int>(m_pSkillLevels[logId].bLevel),
            static_cast<int>(skill->bType));
    Save();
    ComputePoints();
    SkillLevelPacket();
    return true;
}

bool CHARACTER::LearnSkillByBook(DWORD vnum, BYTE probability)
{
    const CSkillProto* skill = CSkillManager::instance().Get(vnum);
    if (!skill)
        return false;
    if (!IsLearnableSkill(vnum))
    {
        ChatPacket(1, locale_find(bookUnlearnable, g_iUseLocale));
        return false;
    }
    if (!LC_IsYMIR() && !LC_IsCIBN() && GetExp() < 20000)
    {
        ChatPacket(1, locale_find(bookNeedsExp, g_iUseLocale));
        return false;
    }

    const int now = muyeong_detail::bits(static_cast<unsigned>(get_global_time()));
    const int next = GetSkillNextReadTime(vnum);
    if (now < next && !(test_server && quest::CQuestManager::instance().GetEventFlag("no_read_delay")))
    {
        if (!FindAffect(513, 0))
        {
            const auto deadline = static_cast<unsigned>(GetSkillNextReadTime(vnum));
            const auto current = static_cast<unsigned>(get_global_time());
            SkillLearnWaitMoreTimeMessage(deadline - current);
            return false;
        }
        RemoveAffect(513);
        ChatPacket(1, locale_find(bookDelayRemoved, g_iUseLocale));
    }

    const int mastery = GetSkillMasterType(vnum);
    if (skill->bType && mastery != 1)
    {
        ChatPacket(1, locale_find(mastery < 2 ? bookNeedsMaster : bookAlreadyGrand, g_iUseLocale));
        return false;
    }

    const int before = GetSkillLevel(vnum);
    bool forceSuccess = false;
    if (probability)
    {
        if (FindAffect(512, 0))
        {
            probability = static_cast<BYTE>(probability + probability / 2);
            RemoveAffect(512);
        }
        sys_log(0, "LearnSkillByBook Pct %d", static_cast<int>(probability));
        const bool pass = number_ex(1, 100, const_cast<char*>("char_skill.cpp"), 0x17d) <= probability;
        if (test_server)
            sys_log(0, "LearnSkillByBook %s", pass ? bookTestSuccess : "FAIL");
        if (pass)
            SkillLevelUp(vnum, 1);
    }
    else
    {
        const int index = std::max(0, std::min(9, GetSkillLevel(vnum) - 20));
        int chance = aiSkillBookCountForLevelUp[index];
        sys_log(0, "LearnSkillByBook %s table idx %d value %d", GetName(), index, chance);
        if (LC_IsYMIR() || LC_IsCIBN())
        {
            if (FindAffect(512, 0))
            {
                chance = (chance + 1) / 2;
                RemoveAffect(512);
            }
            if (number_ex(1, chance, const_cast<char*>("char_skill.cpp"), 0x1e2) == 2)
                SkillLevelUp(vnum, 1);
        }
        else
        {
            const int required = GetSkillLevel(vnum) - 20;
            PointChange(POINT_EXP, -20000, false, false);
            quest::PC* pc = quest::CQuestManager::instance().GetPC(GetPlayerID());
            if (pc)
            {
                const std::string flag = "traning_master_skill." + std::to_string(vnum) + ".read_count";
                const int read = pc->GetFlag(flag);
                int threshold = 65;
                if (FindAffect(512, 0))
                {
                    threshold = 0;
                    RemoveAffect(512);
                }
                if (number_ex(1, 100, const_cast<char*>("char_skill.cpp"), 0x1a8) > threshold)
                {
                    if (read < required)
                    {
                        pc->SetFlag(flag, read + 1, false);
                        const int message = number_ex(1, 3, const_cast<char*>("char_skill.cpp"), 0x1b8);
                        ChatPacket(0, locale_find(message == 2 ? bookProgressTalk :
                            (message == 1 ? bookProgressTalkAlt : bookProgressTalkThird), g_iUseLocale));
                        ChatPacket(1, locale_find(bookProgressInfo, g_iUseLocale), required - read);
                        return true;
                    }
                    SkillLevelUp(vnum, 1);
                    pc->SetFlag(flag, 0, false);
                    forceSuccess = true;
                }
            }
        }
    }

    if (forceSuccess || GetSkillLevel(vnum) != before)
    {
        if (!forceSuccess)
        {
            ChatPacket(0, locale_find(bookSuccessTalk1, g_iUseLocale));
            ChatPacket(0, locale_find(bookSuccessTalk2, g_iUseLocale));
        }
        ChatPacket(1, locale_find(bookSuccessInfo, g_iUseLocale));
        LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), vnum, "GM_READ_SUCCESS", "");
    }
    else
    {
        ChatPacket(0, locale_find(bookFailTalk, g_iUseLocale));
        ChatPacket(1, locale_find(bookFailInfo, g_iUseLocale));
        LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), vnum, "GM_READ_FAIL", "");
    }
    return true;
}

bool CHARACTER::LearnGrandMasterSkill(DWORD vnum)
{
    const CSkillProto* skill = CSkillManager::instance().Get(vnum);
    if (!skill)
        return false;
    if (!IsLearnableSkill(vnum))
    {
        ChatPacket(1, locale_find(skillUnlearnable, g_iUseLocale));
        return false;
    }

    const time_t nextRead = GetSkillNextReadTime(vnum);
    const time_t now = get_global_time();
    sys_log(0, "learn grand master skill[%d] cur %d, next %d", static_cast<int>(vnum),
        static_cast<int>(now), static_cast<int>(nextRead));
    if (skill->bType == 0)
    {
        ChatPacket(1, locale_find(grandSubSkill, g_iUseLocale));
        return false;
    }
    const int mastery = GetSkillMasterType(vnum);
    if (mastery != 2)
    {
        ChatPacket(1, locale_find(mastery > 2 ? grandAlreadyPerfect : grandRequiresMaster, g_iUseLocale));
        return false;
    }

    const int before = GetSkillLevel(vnum);
    const int index = std::max(0, std::min(9, GetSkillLevel(vnum) - 30));
    const std::string flag = "training_grandmaster_skill.skill" + std::to_string(vnum);
    int chance = aiGrandMasterSkillBookCountForLevelUp[index];
    sys_log(0, "LearnGrandMasterSkill %s table idx %d value %d", GetName(), index, chance);
    const int count = static_cast<int>(static_cast<unsigned int>(GetQuestFlag(flag)) + 1);
    SetQuestFlag(flag, count);
    const int minimum = aiGrandMasterSkillBookMinCount[index];
    const int maximum = aiGrandMasterSkillBookMaxCount[index];
    if (FindAffect(512, 0))
    {
        chance = (chance + 1) / 2;
        RemoveAffect(512);
    }
    const int roll = number_ex(1, chance, const_cast<char*>("char_skill.cpp"), 0x112);
    sys_log(0, "Number(%d)", roll);
    const int nextBase = muyeong_detail::bits(static_cast<unsigned>(get_global_time()));
    const int delay = number_ex(28800, 43200, const_cast<char*>("char_skill.cpp"), 0x115);
    const int next = muyeong_detail::add(nextBase, delay);
    sys_log(0, "GrandMaster SkillBookCount min %d cur %d max %d (next_time=%d)",
        minimum, count, maximum, static_cast<int>(next));
    if (count > maximum || (count >= minimum && roll == 2))
        SkillLevelUp(vnum, 3);
    SetSkillNextReadTime(vnum, next);

    const bool success = GetSkillLevel(vnum) != before;
    if (success)
    {
        ChatPacket(0, locale_find(grandSuccessTalk1, g_iUseLocale));
        ChatPacket(0, locale_find(grandSuccessTalk2, g_iUseLocale));
        ChatPacket(1, locale_find(grandSuccessInfo, g_iUseLocale));
    }
    else
    {
        ChatPacket(0, locale_find(grandFailTalk, g_iUseLocale));
        ChatPacket(1, locale_find(grandFailInfo, g_iUseLocale));
    }
    LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), vnum,
        success ? "GM_READ_SUCCESS" : "GM_READ_FAIL", "");
    return success;
}

int CHARACTER::GetSkillLevel(DWORD dwVnum) {
    if(dwVnum<255) {
        unsigned int level=m_pSkillLevels?m_pSkillLevels[dwVnum].bLevel:0;
        return MIN(40,static_cast<int>(level));
    }
    _sys_err("GetSkillLevel",2614,"%s skill vnum overflow %lu",GetName(),dwVnum);
    sys_log(0,"%s skill vnum overflow %lu",GetName(),dwVnum);
    return 0;
}


#include "char.h"
//08073900: original five-instruction tail jump to actual event_cancel.
void CHARACTER::StopMuyeongEvent() { event_cancel(&m_pkMuyeongEvent); }

#include "char.h"
//0806cd70: actual empty original body; see complete4instruction evidence.
void CHARACTER::ComputeSkillPoints() {}

// Original Muyeong scheduler 08073890 and callback 08073610.
namespace {
struct char_event_info {
    CHARACTER* ch;
};
}

namespace muyeong_detail {
inline int bits(unsigned int value)
{
    int result;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}
inline int add(int a, int b) { return bits(static_cast<unsigned int>(a) + static_cast<unsigned int>(b)); }
inline int sub(int a, int b) { return bits(static_cast<unsigned int>(a) - static_cast<unsigned int>(b)); }
inline int mul(int a, int b) { return bits(static_cast<unsigned int>(a) * static_cast<unsigned int>(b)); }
inline int negate(int value) { return bits(0u - static_cast<unsigned int>(value)); }
}

struct FFindNearVictim {
    LPCHARACTER m_pkChrCenter;
    LPCHARACTER m_pkChrNextTarget;
    LPCHARACTER m_pkChrAttacker;
    int m_count;
    const std::set<CHARACTER*>& m_excepts_set;
    FFindNearVictim(LPCHARACTER center,LPCHARACTER attacker,const std::set<CHARACTER*>& excepts):
        m_pkChrCenter(center),m_pkChrNextTarget(0),m_pkChrAttacker(attacker),m_count(0),m_excepts_set(excepts) {}
    void operator()(LPENTITY ent) {
        if (!ent->IsType(0)) return;
        LPCHARACTER ch=static_cast<LPCHARACTER>(ent);
        if (m_excepts_set.find(ch)!=m_excepts_set.end() || m_pkChrCenter==ch || !battle_is_attackable(m_pkChrAttacker,ch)) return;
        int dx=muyeong_detail::sub(m_pkChrCenter->GetX(),ch->GetX());
        int absolute=dx<0?muyeong_detail::negate(dx):dx;
        if (absolute>=1001) return;
        int dy=muyeong_detail::sub(m_pkChrCenter->GetY(),ch->GetY());
        absolute=dy<0?muyeong_detail::negate(dy):dy;
        if (absolute>=1001) return;
        if (static_cast<float>(DISTANCE_APPROX(dx,dy))<1000.0f) {
            m_count=muyeong_detail::add(m_count,1);
            if (m_count!=1 && number_ex(1,m_count,"char_skill.cpp",0x359)!=1) return;
            m_pkChrNextTarget=ch;
        }
    }
};
long skill_muyoung_event(LPEVENT event)
{
    LPCHARACTER ch=static_cast<char_event_info*>(event->info)->ch;
    if (!ch->IsAffectFlag(27)) { ch->StopMuyeongEvent(); return 0; }
    // The 2009 binary destroys the temporary set before sector traversal (080736cd).
    // The functor retains a dangling reference, so traversal has undefined behavior.
    FFindNearVictim f(ch,ch,std::set<CHARACTER*>());
    if (ch->GetSectree()) {
        ch->GetSectree()->ForEachAround(f);
        if (f.m_pkChrNextTarget) {
            ch->CreateFly(16,f.m_pkChrNextTarget);
            ch->ComputeSkill(78,f.m_pkChrNextTarget,0);
        }
    }
    return muyeong_detail::mul(passes_per_sec,3);
}

void CHARACTER::StartMuyeongEvent()
{
    if (!m_pkMuyeongEvent) {
        void* rawInfo = nullptr;
        const DWORD id = __AllocEventInfo(sizeof(char_event_info), &rawInfo);
        static_cast<char_event_info*>(rawInfo)->ch = this;
        m_pkMuyeongEvent=event_create_ex(skill_muyoung_event,id,passes_per_sec,const_cast<char*>("char_skill.cpp"),0xa60);
    }
}

// 080732c0
int CHARACTER::GetSkillMasterType(DWORD vnum)
{
    if (!GetDesc())
        return 0;
    if (vnum < 255)
        return m_pSkillLevels ? m_pSkillLevels[vnum].bMasterType : 0;
    _sys_err("GetSkillMasterType", 2567, "%s skill vnum overflow %lu", GetName(), vnum);
    return 0;
}

// 08073170
int CHARACTER::GetUsedSkillMasterType(DWORD vnum)
{
    TSkillUseInfo& info = m_SkillUseInfo[muyeong_detail::bits(static_cast<unsigned int>(vnum))];
    if (GetSkillMasterType(vnum) < 2 || info.isGrandMaster)
        return GetSkillMasterType(vnum);
    return MIN(GetSkillMasterType(vnum), 1);
}

int CHARACTER::GetSkillPowerByLevel(int level, bool mob)
{
    const int clampedLevel = MINMAX(0, level, 40);
    const BYTE skillGroup = m_points.skill_group;
    const BYTE job = GetJob();
    return CTableBySkill::instance().GetSkillPowerByLevelFromType(job, skillGroup, clampedLevel, mob);
}

// 08073350
int CHARACTER::GetSkillPower(DWORD vnum, BYTE requestedLevel)
{
    if (vnum - 126 < 3 && IsEquipUniqueGroup(10025))
        return 100;
    if (vnum - 151 < 12)
        return m_pGuild ? m_pGuild->GetSkillLevel(vnum) * 100 / 49 : 0;
    if (!requestedLevel && vnum >= 255) {
        _sys_err("GetSkillPower", 2600, "%s skill vnum overflow %lu", GetName(), vnum);
        return 0;
    }
    return GetSkillPowerByLevel(requestedLevel ? requestedLevel : GetSkillLevel(vnum), requestedLevel != 0);
}

// 0806acd0
bool TSkillUseInfo::HitOnce(DWORD vnum)
{
    if (!bUsed)
        return false;
    const DWORD now = get_dword_time();
    const int splashCount = iSplashCount;
    const int hitCount = iHitCount;
    const DWORD next = dwNextSkillUsableTime;
    sys_log(1, "__HitOnce NextUse %lu current %lu count %d scount %d", next, now, hitCount, splashCount);
    if (dwNextSkillUsableTime) {
        const DWORD expiryCheckTime = get_dword_time();
        if (expiryCheckTime > dwNextSkillUsableTime && vnum != 78 && vnum != 137) {
            sys_log(1, "__HitOnce can't hit");
            return false;
        }
    }
    if (iHitCount == 0)
        return false;
    const bool unlimited = iHitCount == -1;
    const DWORD hitTime = get_dword_time();
    sys_log(1, "__HitOnce OK %d %d %d", muyeong_detail::bits(static_cast<unsigned>(dwNextSkillUsableTime)),
            muyeong_detail::bits(static_cast<unsigned>(hitTime)), iHitCount);
    if (!unlimited)
        iHitCount = muyeong_detail::sub(iHitCount, 1);
    return true;
}

// 0806ade0
bool TSkillUseInfo::UseSkill(bool grandMaster, DWORD vid, DWORD cooldown, int splashCount, int hitCount, int range)
{
    isGrandMaster = grandMaster;
    const DWORD now = get_dword_time();
    const auto cooldown32 = static_cast<unsigned>(cooldown);
    if (bUsed && dwNextSkillUsableTime > now) {
        sys_log(0, "cooltime is not over delta %lu", dwNextSkillUsableTime - now);
        iHitCount = 0;
        return false;
    }
    bUsed = true;
    dwNextSkillUsableTime = cooldown32 ? cooldown32 + static_cast<unsigned>(now) : 0;
    iHitCount = hitCount;
    iMaxHitCount = hitCount;
    iRange = range;
    sys_log(0, "__UseSkill NextUse %lu  current %lu cooltime %d hitcount %d/%d",
            dwNextSkillUsableTime, now, muyeong_detail::bits(cooldown32), hitCount, hitCount);
    dwVID = vid;
    iSplashCount = splashCount;
    return true;
}

// 0806d530
void SetPolyVarForAttack(CHARACTER* ch, CSkillProto* skill, CItem* weapon)
{
    if (!ch->GetDesc()) {
        const TMobTable& mob = ch->GetMobTable();
        const double damage = number_ex(mob.dwDamageRange[0], mob.dwDamageRange[1], "char_skill.cpp", 954);
        skill->SetPointVar("wep", damage);
        skill->SetPointVar("mwep", damage);
        skill->SetPointVar("mtk", damage);
        return;
    }
    double physicalDamage = 0.0;
    double magicDamage = 0.0;
    if (weapon && weapon->GetType() == 1) {
        const long max = weapon->GetValue(4);
        const long min = weapon->GetValue(3);
        const int physical = number_ex(min, max, "char_skill.cpp", 935);
        const long physicalBonus = weapon->GetValue(5);
        const long magicMax = weapon->GetValue(2);
        const long magicMin = weapon->GetValue(1);
        const int magic = number_ex(magicMin, magicMax, "char_skill.cpp", 938);
        const long magicBonus = weapon->GetValue(5);
        physicalDamage = muyeong_detail::add(physical, physicalBonus);
        magicDamage = muyeong_detail::add(magic, magicBonus);
    }
    skill->SetPointVar("wep", physicalDamage);
    skill->SetPointVar("mtk", magicDamage);
    skill->SetPointVar("mwep", magicDamage);
}

struct FuncSplashAffect {
    FuncSplashAffect(LPCHARACTER attacker, int x, int y, int distance, DWORD vnum,
                     BYTE point, int amount, DWORD flag, int duration, int spCost,
                     bool overrideExisting, int maxHit)
        : m_pkChrAttacker(attacker), m_x(x), m_y(y), m_iDist(distance),
          m_dwVnum(vnum), m_bPointOn(point), m_iAmount(amount), m_dwAffectFlag(flag),
          m_iDuration(duration), m_iSPCost(spCost), m_bOverride(overrideExisting),
          m_iMaxHit(maxHit), m_iCount(0) {}

    void operator()(LPENTITY entity);

    LPCHARACTER m_pkChrAttacker;
    int m_x;
    int m_y;
    int m_iDist;
    DWORD m_dwVnum;
    BYTE m_bPointOn;
    int m_iAmount;
    DWORD m_dwAffectFlag;
    int m_iDuration;
    int m_iSPCost;
    bool m_bOverride;
    int m_iMaxHit;
    int m_iCount;
};

// 08077180
void FuncSplashAffect::operator()(LPENTITY entity)
{
    if ((m_iMaxHit && m_iCount >= m_iMaxHit) || !entity->IsType(0))
        return;
    LPCHARACTER ch = static_cast<LPCHARACTER>(entity);
    const long y = ch->GetY();
    const long x = ch->GetX();
    const char* name = ch->GetName();
    const int distance = m_iDist;
    const int centerY = m_y;
    const int centerX = m_x;
    sys_log(0, "FuncSplashAffect pkChr %s %d %d %d %d | m_x %d m_y %d x %d y %d dist %d",
            name, static_cast<unsigned int>(m_bPointOn), m_iAmount, m_iDuration,
            static_cast<unsigned int>(m_bOverride), centerX, centerY, x, y, distance);
    const int dx = muyeong_detail::sub(m_x, ch->GetX());
    const int dy = muyeong_detail::sub(m_y, ch->GetY());
    if (DISTANCE_APPROX(dx, dy) >= m_iDist)
        return;

    sys_log(0, "FuncSplashAffect pkChr %s %d %d %d %d", ch->GetName(),
            static_cast<unsigned int>(m_bPointOn), m_iAmount, m_iDuration,
            static_cast<unsigned int>(m_bOverride));
    if (m_dwVnum == 80 && ch->CanBeginFight())
        ch->BeginFight(m_pkChrAttacker);
    const DWORD type = m_dwVnum;
    const int duration = ch->GetDesc() && type == 80 ? m_iDuration / 3 : m_iDuration;
    ch->AddAffect(type, m_bPointOn, m_iAmount, m_dwAffectFlag, duration, m_iSPCost, m_bOverride, false);
    m_iCount = muyeong_detail::add(m_iCount, 1);
}

// 0806aef0
void CHARACTER::SetAffectedEunhyung()
{
    m_dwAffectedEunhyungLevel = static_cast<DWORD>(GetSkillPower(34, 0));
}

// 08077380
void SkillAttackAffect(LPCHARACTER victim, int percent, DWORD immunity, DWORD type,
                       BYTE point, long amount, DWORD flag, int duration, const char* name)
{
    if (percent == 0 || victim->IsAffectFlag(flag))
        return;
    if (percent < number_ex(1, 1000, "battle.h", 74) || victim->IsImmune(immunity)) {
        if (test_server)
            victim->ChatPacket(3, "%s(%d%%) -> %s FAIL", name, percent, name);
        return;
    }
    victim->AddAffect(type, point, amount, flag, duration, 0, true, false);
    if (test_server)
        victim->ChatPacket(3, "%s(%d%%) -> %s SUCCESS", name, percent, name);
}

// 0806aeb0
int CHARACTER::GetChainLightningMaxCount()
{
    return aiChainLightningCountBySkillLevel[MIN(40, GetSkillLevel(108))];
}

namespace {
int TruncateSkill(long double value)
{
    if (!(value > -2147483649.0L && value < 2147483648.0L))
        return INT_MIN;
    return static_cast<int>(value);
}

struct chain_lightning_event_info {
    DWORD dwVictim;
    DWORD dwChr;
};
}

// 0806d1f0
long ChainLightningEvent(LPEVENT event)
{
    chain_lightning_event_info* info = static_cast<chain_lightning_event_info*>(event->info);
    LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(info->dwVictim);
    LPCHARACTER attacker = CHARACTER_MANAGER::instance().Find(info->dwChr);
    if (!attacker || !victim) {
        sys_log(1, "use chainlighting, but no character");
        return 0;
    }
    sys_log(1, "chainlighting event %s", attacker->GetName());

    LPCHARACTER target = 0;
    CParty* party = victim->GetParty();
    if (party) {
        target = party->GetNextOwnership(0, victim->GetX(), victim->GetY());
        if (target == victim || number_ex(0, 2, "char_skill.cpp", 898) == 0 ||
            attacker->GetChainLightingExcept().find(target) != attacker->GetChainLightingExcept().end())
            target = 0;
    }
    if (!target) {
        FFindNearVictim find(victim, attacker, attacker->GetChainLightingExcept());
        if (SECTREE* tree = victim->GetSectree()) {
            tree->ForEachAround(find);
            target = find.m_pkChrNextTarget;
        }
        if (!target) {
            const char* victimName = victim->GetName();
            sys_log(1, "%s use chainlighting, but find victim failed near %s", attacker->GetName(), victimName);
            return 0;
        }
    }
    victim->CreateFly(14, target);
    attacker->ComputeSkill(108, target, 0);
    attacker->AddChainLightningExcept(target);
    return 0;
}

struct FuncSplashDamage {
    int m_x;
    int m_y;
    CSkillProto* m_pkSk;
    LPCHARACTER m_pkChr;
    int m_iAmount;
    int m_iAG;
    int m_iCount;
    int m_iMaxHit;
    LPITEM m_pkWeapon;
    bool m_bDisableCooltime;
    TSkillUseInfo* m_pInfo;
    BYTE m_bUseSkillPower;

    void operator()(LPENTITY entity);
};

// 08075540
void FuncSplashDamage::operator()(LPENTITY entity)
{
    using namespace muyeong_detail;
    if (!entity->IsType(0))
        return;
    LPCHARACTER victim = static_cast<LPCHARACTER>(entity);
    const int dx = sub(m_x, victim->GetX());
    const int dy = sub(m_y, victim->GetY());
    if (m_pkSk->iSplashRange < DISTANCE_APPROX(dx, dy) || !battle_is_attackable(m_pkChr, victim))
        return;
    if (m_pkChr->GetDesc() && m_pkSk->dwVnum - 151 > 11 && !m_bDisableCooltime &&
        m_pInfo && !m_pInfo->HitOnce(m_pkSk->dwVnum) && m_pkSk->dwVnum != 78)
        return;

    m_iCount = add(m_iCount, 1);
    m_pkSk->SetPointVar("k", static_cast<long double>(m_pkSk->bMaxLevel) * m_bUseSkillPower / 100.0L);
    m_pkSk->SetPointVar("lv", m_pkChr->GetLevel());
    m_pkSk->SetPointVar("iq", m_pkChr->GetPoint(15));
    m_pkSk->SetPointVar("str", m_pkChr->GetPoint(12));
    m_pkSk->SetPointVar("dex", m_pkChr->GetPoint(14));
    m_pkSk->SetPointVar("con", m_pkChr->GetPoint(13));
    m_pkSk->SetPointVar("def", m_pkChr->GetPoint(16));
    const int defense = m_pkChr->GetPoint(16);
    const int defenseBonus = m_pkChr->GetPoint(96);
    m_pkSk->SetPointVar("odef", sub(defense, defenseBonus));
    m_pkSk->SetPointVar("horse_level", m_pkChr->GetHorseLevel());

    bool ignoreDefense = false;
    if (m_pkSk->dwFlag & 0x400) {
        const int chance = TruncateSkill(m_pkSk->kPointPoly2.Eval());
        ignoreDefense = number_ex(1, 100, "char_skill.cpp", 1036) <= chance;
    }
    bool ignoreRating = false;
    if (m_pkSk->dwFlag & 0x800) {
        const int chance = TruncateSkill(m_pkSk->kPointPoly2.Eval());
        ignoreRating = number_ex(1, 100, "char_skill.cpp", 1046) <= chance;
    }
    const float attackRating = CalcAttackRating(m_pkChr, victim, ignoreRating);
    m_pkSk->SetPointVar("ar", attackRating);
    if (m_pkSk->dwFlag & 2) {
        m_pkSk->SetPointVar("atk", CalcMeleeDamage(m_pkChr, victim, true, ignoreRating));
    } else if (m_pkSk->dwFlag & 0x200) {
        LPITEM bow;
        LPITEM arrow;
        const int amount = m_pkChr->GetArrowAndBow(&bow, &arrow, 1) == 1
                         ? CalcArrowDamage(m_pkChr, victim, bow, arrow, true) : 0;
        m_pkSk->SetPointVar("atk", amount);
    }
    if (m_pkSk->bPointOn == 19)
        m_pkSk->kPointPoly.SetVar("maxv", victim->GetLimitPoint(19));
    m_pkSk->SetPointVar("maxhp", victim->m_pointsInstant.iMaxHP);
    m_pkSk->SetPointVar("maxsp", victim->m_pointsInstant.iMaxSP);
    m_pkSk->SetPointVar("chain", m_pkChr->GetChainLightningIndex());
    const bool underEunhyung = m_pkChr->GetAffectedEunhyung();
    m_pkChr->IncChainLightningIndex();
    m_pkSk->SetPointVar("ek", underEunhyung ? 1.0L / 100.0L : 0.0L);
    SetPolyVarForAttack(m_pkChr, m_pkSk, m_pkWeapon);

    long double result = m_pkSk->kPointPoly.Eval();
    if (m_pkChr->GetUsedSkillMasterType(m_pkSk->dwVnum) > 1)
        result = m_pkSk->kMasterBonusPoly.Eval();
    const int raw = TruncateSkill(result);
    if (test_server && raw == 0 && m_pkSk->bPointOn)
        m_pkChr->ChatPacket(1, "\310\277\260\372\260\241\040\276\370\275\300\264\317\264\331\056\040\275\272\305\263\040\260\370\275\304\300\273\040\310\256\300\316\307\317\274\274\277\344");

    int damage = negate(raw);
    if (m_pkSk->dwVnum == 31) {
        const long double delta = GetDegreeDelta(m_pkChr->m_pointsInstant.fRot, victim->m_pointsInstant.fRot);
        float adjustment = 1.0f;
        if (delta < 35.0f) {
            adjustment = underEunhyung ? 2.0f : 1.5f;
            if (m_pkChr->GetWear(4) && m_pkChr->GetWear(4)->GetSubType() == 1)
                adjustment += LC_IsYMIR() ? 1.0f : 0.5f;
        } else if (!LC_IsYMIR()) {
            if (underEunhyung)
                adjustment = 1.5f;
            if (m_pkChr->GetWear(4) && m_pkChr->GetWear(4)->GetSubType() == 1)
                adjustment += 0.5f;
        }
        damage = TruncateSkill(static_cast<long double>(damage) * adjustment);
    } else if (m_pkSk->dwVnum == 32) {
        float adjustment = 1.0f;
        LPITEM weapon = m_pkChr->GetWear(4);
        if (weapon && weapon->GetSubType() == 1)
            adjustment = LC_IsYMIR() ? 1.4f : 1.35f;
        damage = TruncateSkill(static_cast<long double>(damage) * adjustment);
    }
    damage = CalcBattleDamage(damage, m_pkChr->GetLevel(), victim->GetLevel());
    if (m_pkChr->GetDesc()) {
        TSkillUseInfo& info = m_pkChr->m_SkillUseInfo[bits(static_cast<unsigned int>(m_pkSk->dwVnum))];
        if (info.dwVID != static_cast<DWORD>(victim->GetVID()))
            damage = TruncateSkill(static_cast<long double>(m_pkSk->kSplashAroundDamageAdjustPoly.Eval()) * damage);
    }

    EDamageType damageType = DAMAGE_TYPE_NONE;
    switch (m_pkSk->bSkillAttrType) {
    case 1:
        break;
    case 2: {
        damageType = DAMAGE_TYPE_MELEE;
        LPITEM weapon = m_pkChr->GetWear(4);
        if (weapon) {
            BYTE resist = 0;
            switch (weapon->GetSubType()) {
            case 0: resist = 69; break;
            case 1: resist = 71; break;
            case 3: resist = 70; break;
            case 4: resist = 72; break;
            case 5: resist = 73; break;
            }
            if (resist)
                damage = mul(damage, sub(100, victim->GetPoint(resist))) / 100;
        }
        if (!ignoreDefense)
            damage = sub(damage, victim->GetPoint(16));
        break;
    }
    case 3:
        damageType = DAMAGE_TYPE_RANGE;
        damage = mul(damage, sub(100, victim->GetPoint(74))) / 100;
        break;
    case 4:
        damageType = DAMAGE_TYPE_MAGIC;
        damage = CalcAttBonus(m_pkChr, victim, damage);
        damage = mul(damage, sub(100, victim->GetPoint(77))) / 100;
        break;
    default:
        _sys_err("operator()", 1226, "Unknown skill attr type %u vnum %lu",
                 static_cast<unsigned int>(m_pkSk->bSkillAttrType), m_pkSk->dwVnum);
        break;
    }
    if (m_pkSk->dwFlag & 0x1000000)
        damage = mul(damage, sub(100, victim->GetPoint(78))) / 100;
    if (m_pkSk->dwFlag & 0x2000000)
        damage = mul(damage, sub(100, victim->GetPoint(76))) / 100;
    if (m_pkSk->dwFlag & 0x4000000)
        damage = mul(damage, sub(100, victim->GetPoint(75))) / 100;
    if (m_pkSk->dwFlag & 0x40)
        damageType = DAMAGE_TYPE_MAGIC;
    if (victim->CanBeginFight())
        victim->BeginFight(m_pkChr);
    if (m_pkSk->dwVnum == 108) {
        const int index = sub(m_pkChr->GetChainLightningIndex(), 1);
        sys_log(0, "%s CHAIN INDEX %d DAM %d DT %d", m_pkChr->GetName(), index, damage, damageType);
    }

    if (!victim->Damage(m_pkChr, damage, damageType) && !victim->IsStun()) {
        if (m_pkSk->dwFlag & 0x40000) {
            const int chance = TruncateSkill(m_pkSk->kPointPoly2.Eval());
            const int duration = TruncateSkill(m_pkSk->kDurationPoly2.Eval());
            const int partyBonus = m_pkChr->GetPoint(103);
            if (number_ex(1, 100, "char_skill.cpp", 1262) <= chance) {
                victim->RemoveGoodAffect();
                victim->AddAffect(m_pkSk->dwVnum, 0, 0, 33, add(duration, partyBonus), 0, true, false);
            }
        }
        if (m_pkSk->dwFlag & 0x113000) {
            const int chance = TruncateSkill(m_pkSk->kPointPoly2.Eval());
            const int duration = TruncateSkill(m_pkSk->kDurationPoly2.Eval());
            const int partyBonus = m_pkChr->GetPoint(103);
            const int effectDuration = add(duration, partyBonus);
            const DWORD flags = m_pkSk->dwFlag;
            if (flags & 0x2000) {
                sys_log(0, "SKILL_STUN %d", chance);
                SkillAttackAffect(victim, chance, 1, 210, 0, 0, 6, effectDuration, m_pkSk->szName);
            } else if (flags & 0x1000) {
                sys_log(0, "SKILL_SLOW %d", chance);
                SkillAttackAffect(victim, chance, 2, 211, 19, -30, 5, effectDuration, m_pkSk->szName);
            } else if (flags & 0x10000) {
                if (number_ex(1, 100, "char_skill.cpp", 1286) <= effectDuration)
                    victim->AttackedByFire(m_pkChr, chance, 5);
            } else if (flags & 0x100000) {
                if (number_ex(1, 100, "char_skill.cpp", 1291) <= chance)
                    victim->AttackedByPoison(m_pkChr);
            }
        }

        if ((m_pkSk->dwFlag & 0x880000) && !(victim->GetAIFlag() & 2)) {
            float slideLength = m_pkChr->GetCharType() == 6 ? 200.0f : 400.0f;
            if (m_pkSk->dwFlag & 0x800000)
                slideLength += slideLength;
            const long double heading = GetDegreeFromPositionXY(
                m_pkChr->GetX(), m_pkChr->GetY(), victim->GetX(), victim->GetY());
            float degree = static_cast<float>(heading);
            if (m_pkSk->dwVnum == 137) {
                const float rotation = m_pkChr->m_pointsInstant.fRot;
                const double difference = static_cast<double>(heading - rotation);
                const float delta = static_cast<float>(std::fmod(difference, 360.0) - 180.0);
                degree = delta > 0.0f ? rotation + 90.0f : rotation - 90.0f;
            }
            float fx;
            float fy;
            GetDeltaByDegree(degree, slideLength, &fx, &fy);
            const int previousY = victim->GetY();
            const int previousX = victim->GetX();
            const long double logX = static_cast<long double>(previousX) + fx;
            const long double logY = static_cast<long double>(previousY) + fy;
            const char* victimName = victim->GetName();
            const char* attackerName = m_pkChr->GetName();
            sys_log(0, "CRUSH! %s -> %s (%d %d) -> (%d %d)", attackerName, victimName, previousX, previousY,
                    TruncateSkill(logX), TruncateSkill(logY));
            const int x = TruncateSkill(static_cast<long double>(victim->GetX()) + fx);
            const int y = TruncateSkill(static_cast<long double>(victim->GetY()) + fy);
            victim->Sync(x, y);
            victim->Goto(x, y);
            victim->CalculateMoveDuration();
            bool mainTarget = false;
            if (m_pkChr->GetDesc()) {
                TSkillUseInfo& info = m_pkChr->m_SkillUseInfo[bits(static_cast<unsigned int>(m_pkSk->dwVnum))];
                mainTarget = info.dwVID == static_cast<DWORD>(victim->GetVID());
            }
            if (mainTarget) {
                const int duration = LC_IsYMIR() ? 3 : 4;
                SkillAttackAffect(victim, 1000, 1, m_pkSk->dwVnum, 0, 0, 6, duration, m_pkSk->szName);
            } else {
                victim->SyncPacket();
            }
        }
    }

    if (m_pkSk->dwFlag & 0x4000)
        m_pkChr->PointChange(5, mul(damage, TruncateSkill(m_pkSk->kPointPoly2.Eval())) / 100, false, false);
    if (m_pkSk->dwFlag & 0x8000)
        m_pkChr->PointChange(7, mul(damage, TruncateSkill(m_pkSk->kPointPoly2.Eval())) / 100, false, false);
    if (m_pkSk->dwVnum == 108) {
        const int chainIndex = m_pkChr->GetChainLightningIndex();
        if (chainIndex < m_pkChr->GetChainLightningMaxCount()) {
            void* rawInfo = nullptr;
            const DWORD id = __AllocEventInfo(sizeof(chain_lightning_event_info), &rawInfo);
            auto* info = static_cast<chain_lightning_event_info*>(rawInfo);
            info->dwVictim = static_cast<DWORD>(victim->GetVID());
            info->dwChr = static_cast<DWORD>(m_pkChr->GetVID());
            event_create_ex(ChainLightningEvent, id, passes_per_sec / 5, const_cast<char*>("char_skill.cpp"), 1403);
        }
    }
}

// 0x0806f710
int CHARACTER::ComputeSkill(DWORD vnum, LPCHARACTER victim, BYTE skillLevel)
{
    using namespace muyeong_detail;
    const bool riding = IsHorseRiding();
    if ((riding ? GetHorseGrade() != 3 : GetMountVnum() != 0) ||
        IsPolymorphed() || g_bSkillDisable)
        return 0;

    CSkillProto* skill = CSkillManager::instance().Get(vnum);
    if (!skill || riding != (skill->bType == 5))
        return 0;
    if (skill->dwFlag & SKILL_FLAG_SELFONLY)
        victim = this;
    if (!victim) {
        _sys_err("ComputeSkill", 0x775, "Victim is null while using skill %d", static_cast<int>(vnum));
        return 0;
    }
    if (skill->dwTargetRange) {
        const int dx = sub(GetX(), victim->GetX());
        const int dy = sub(GetY(), victim->GetY());
        const float distance = std::sqrt(static_cast<double>(add(mul(dx, dx), mul(dy, dy))));
        const auto limit = static_cast<std::uint32_t>(skill->dwTargetRange) + 50U;
        if (distance >= static_cast<float>(limit)) {
            _sys_err("ComputeSkill", 0x77c,
                     "Victim Too Far skill %d : %s to %s (distance %lu limit %lu)",
                     static_cast<int>(vnum), GetName(), victim->GetName(),
                     static_cast<unsigned long>(distance),
                     static_cast<unsigned long>(skill->dwTargetRange));
            return 0;
        }
    }
    if (!skillLevel) {
        skillLevel = GetSkillLevel(skill->dwVnum);
        if (!skillLevel)
            return 0;
    }
    if (victim->IsAffectFlag(33) && victim->IsGoodAffect(static_cast<BYTE>(vnum)))
        return 0;

    const int powerForDuration = GetSkillPower(skill->dwVnum, skillLevel);
    const float k = static_cast<float>(static_cast<double>(skill->bMaxLevel) * powerForDuration / 100.0);
    const int powerForPoint = GetSkillPower(skill->dwVnum, skillLevel);
    skill->SetPointVar("k", static_cast<double>(skill->bMaxLevel) * powerForPoint / 100.0);
    skill->kSplashAroundDamageAdjustPoly.SetVar("k", k);
    LPITEM bow, arrow;
    if (skill->bType == 5) {
        const int attack = GetArrowAndBow(&bow, &arrow, 1) == 1
            ? CalcArrowDamage(this, victim, bow, arrow, true)
            : CalcMeleeDamage(this, victim, true, false);
        skill->SetPointVar("atk", attack);
    } else if (skill->dwFlag & SKILL_FLAG_USE_MELEE_DAMAGE) {
        skill->SetPointVar("atk", CalcMeleeDamage(this, victim, true, false));
    } else if (skill->dwFlag & SKILL_FLAG_USE_MAGIC_DAMAGE) {
        skill->SetPointVar("atk", CalcMagicDamage(this, victim));
    } else if (skill->dwFlag & SKILL_FLAG_USE_ARROW_DAMAGE) {
        const int attack = GetArrowAndBow(&bow, &arrow, 1) == 1
            ? CalcArrowDamage(this, victim, bow, arrow, true) : 0;
        skill->SetPointVar("atk", attack);
    }

    if (skill->bPointOn == POINT_MOV_SPEED)
        skill->SetPointVar("maxv", victim->GetLimitPoint(POINT_MOV_SPEED));
    skill->SetPointVar("lv", GetLevel());
    skill->SetPointVar("iq", GetPoint(POINT_IQ));
    skill->SetPointVar("str", GetPoint(POINT_ST));
    skill->SetPointVar("dex", GetPoint(POINT_DX));
    skill->SetPointVar("con", GetPoint(POINT_HT));
    skill->SetPointVar("maxhp", GetMaxHP());
    skill->SetPointVar("maxsp", GetMaxSP());
    skill->SetPointVar("chain", 0);
    skill->SetPointVar("ar", CalcAttackRating(this, victim, false));
    skill->SetPointVar("def", GetPoint(POINT_DEF_GRADE));
    const long defense = GetPoint(POINT_DEF_GRADE);
    skill->SetPointVar("odef", sub(defense, GetPoint(POINT_DEF_GRADE_BONUS)));
    skill->SetPointVar("horse_level", GetHorseLevel());
    if (skill->bSkillAttrType != 1)
        OnMove(true);
    LPITEM weapon = GetWear(4);
    SetPolyVarForAttack(this, skill, weapon);
    skill->kDurationPoly.SetVar("k", k);
    skill->kDurationPoly2.SetVar("k", k);

    double amountValue = skill->kPointPoly.Eval();
    const int amount2 = TruncateSkill(skill->kPointPoly2.Eval());
    const int amount3 = TruncateSkill(skill->kPointPoly3.Eval());
    if (GetUsedSkillMasterType(skill->dwVnum) >= 2)
        amountValue = skill->kMasterBonusPoly.Eval();
    const int amount = TruncateSkill(amountValue);
    if (test_server && !amount && skill->bPointOn) {
        ChatPacket(1, "\xc8\xbf\xb0\xfa\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
                      "\xbd\xba\xc5\xb3 \xb0\xf8\xbd\xc4\xc0\xbb \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4");
    }
    char source[] = "char_skill.cpp";
    if ((skill->dwFlag & SKILL_FLAG_REMOVE_BAD_AFFECT) && number_ex(1, 100, source, 0x7f9) <= amount2)
        victim->RemoveBadAffect();

    const auto addBuffer = [&](int duration) {
        return add(duration, GetPoint(POINT_PARTY_BUFFER_BONUS));
    };
    const auto applyTimed = [&](BYTE point, int value, DWORD flag, int duration, bool overrideExisting) {
        duration = addBuffer(duration);
        if (skill->dwFlag & SKILL_FLAG_SPLASH) {
            if (LPSECTREE tree = victim->GetSectree()) {
                FuncSplashAffect apply(this, victim->GetX(), victim->GetY(), skill->iSplashRange,
                                       skill->dwVnum, point, value, flag, duration, 0,
                                       overrideExisting, skill->lMaxHit);
                tree->ForEachAround(apply);
            }
        } else {
            victim->AddAffect(skill->dwVnum, point, value, flag, duration, 0, overrideExisting, false);
        }
    };

    bool added = false;
    const bool attack = (skill->dwFlag & (SKILL_FLAG_ATTACK | SKILL_FLAG_USE_MELEE_DAMAGE |
                                          SKILL_FLAG_USE_MAGIC_DAMAGE)) &&
                        (victim != this || (skill->dwVnum != SKILL_MUYEONG && !skill->IsChargeSkill()));
    if (attack) {
        if (skill->bPointOn == POINT_HP && amount < 0) {
            const int maxHit = skill->lMaxHit;
            const int x = victim->GetX();
            const bool disableCooltime = m_bDisableCooltime;
            const int y = victim->GetY();
            TSkillUseInfo* useInfo = GetDesc() ? &m_SkillUseInfo[vnum] : nullptr;
            const BYTE power = GetSkillPower(vnum, skillLevel);
            FuncSplashDamage apply{x, y, skill, this, amount, 0, 0, maxHit,
                                   weapon, disableCooltime, useInfo, power};
            if (skill->dwFlag & SKILL_FLAG_SPLASH) {
                if (LPSECTREE tree = victim->GetSectree())
                    tree->ForEachAround(apply);
            } else {
                apply(victim);
            }
        } else {
            skill->kDurationPoly.SetVar("k", k);
            const int duration = TruncateSkill(skill->kDurationPoly.Eval());
            if (GetDesc() && (vnum < 151 || vnum > 162) && !m_bDisableCooltime &&
                !m_SkillUseInfo[vnum].HitOnce(vnum) && vnum != SKILL_MUYEONG)
                return 0;
            if (duration > 0) {
                applyTimed(skill->bPointOn, amount, skill->dwAffectFlag, duration, true);
                added = true;
            }
        }
        if (skill->bPointOn2 && !skill->IsChargeSkill()) {
            skill->kDurationPoly2.SetVar("k", k);
            const int duration = TruncateSkill(skill->kDurationPoly2.Eval());
            if (duration > 0) {
                applyTimed(skill->bPointOn2, amount2, skill->dwAffectFlag2, duration, !added);
                added = true;
            } else {
                victim->PointChange(skill->bPointOn2, amount2, false, false);
            }
        }
    } else if (vnum == SKILL_MUYEONG) {
        skill->kDurationPoly.SetVar("k", k);
        skill->kDurationSPCostPoly.SetVar("k", k);
        const int duration = addBuffer(TruncateSkill(skill->kDurationPoly.Eval()));
        if (victim == this)
            AddAffect(SKILL_MUYEONG, 0, 0, 27, duration,
                      TruncateSkill(skill->kDurationSPCostPoly.Eval()), true, false);
        return 0;
    } else {
        skill->kDurationPoly.SetVar("k", k);
        const int duration = TruncateSkill(skill->kDurationPoly.Eval());
        if (duration > 0) {
            const int primaryDuration = addBuffer(duration);
            skill->kDurationSPCostPoly.SetVar("k", k);
            DWORD flag = skill->dwAffectFlag;
            const bool hasSecondary = skill->bPointOn2 != 0;
            if (hasSecondary) {
                victim->RemoveAffect(skill->dwVnum);
                const int secondaryDuration = TruncateSkill(skill->kDurationPoly2.Eval());
                if (secondaryDuration > 0) {
                    victim->AddAffect(skill->dwVnum, skill->bPointOn2, amount2, skill->dwAffectFlag2,
                                      addBuffer(secondaryDuration), 0, false, false);
                } else {
                    victim->PointChange(skill->bPointOn2, amount2, false, false);
                }
                flag = skill->dwAffectFlag;
                const int requiredMastery = LC_IsYMIR() ? 1 : 2;
                if (skill->dwVnum == SKILL_CHUNKEON &&
                    GetUsedSkillMasterType(SKILL_CHUNKEON) < requiredMastery)
                    flag = 34;
            }
            const int spCost = TruncateSkill(skill->kDurationSPCostPoly.Eval());
            victim->AddAffect(skill->dwVnum, skill->bPointOn, amount, flag, primaryDuration,
                              spCost, !hasSecondary, false);
            added = true;
        } else {
            if (!skill->IsChargeSkill())
                victim->PointChange(skill->bPointOn, amount, false, false);
            if (skill->bPointOn2) {
                victim->RemoveAffect(skill->dwVnum);
                const int secondaryDuration = TruncateSkill(skill->kDurationPoly2.Eval());
                if (secondaryDuration > 0) {
                    const DWORD flag = skill->IsChargeSkill() ? 32 : skill->dwAffectFlag2;
                    victim->AddAffect(skill->dwVnum, skill->bPointOn2, amount2, flag,
                                      addBuffer(secondaryDuration), 0, false, false);
                } else {
                    victim->PointChange(skill->bPointOn2, amount2, false, false);
                }
            }
        }
    }

    if (skill->bPointOn3 && !skill->IsChargeSkill() && GetUsedSkillMasterType(skill->dwVnum) >= 2) {
        skill->kDurationPoly3.SetVar("k", k);
        const int duration = TruncateSkill(skill->kDurationPoly3.Eval());
        sys_log(0, attack ? "try third %lu %d %d %d 1751" : "try third %lu %d %d %d 1894",
                skill->dwVnum, skill->bPointOn3, duration, amount3);
        if (duration > 0)
            applyTimed(skill->bPointOn3, amount3, 0, duration, !added);
        else
            victim->PointChange(skill->bPointOn3, amount3, false, false);
    }
    return attack ? 1 : 0;
}

// 0x0806db30
int CHARACTER::ComputeSkillAtPosition(DWORD vnum, const PIXEL_POSITION& position, BYTE skillLevel)
{
    using namespace muyeong_detail;
    if (GetMountVnum() || IsPolymorphed() || g_bSkillDisable)
        return 0;
    CSkillProto* skill = CSkillManager::instance().Get(vnum);
    if (!skill)
        return 0;
    if (test_server)
        sys_log(0, "ComputeSkillAtPosition %s vnum %d x %d y %d level %d", GetName(),
                static_cast<int>(vnum), position.x, position.y, skillLevel);
    if (!(skill->dwFlag & SKILL_FLAG_SPLASH))
        return 0;
    if (!skillLevel) {
        skillLevel = GetSkillLevel(skill->dwVnum);
        if (!skillLevel)
            return 0;
    }

    const int powerForDuration = GetSkillPower(skill->dwVnum, skillLevel);
    const float k = static_cast<float>(static_cast<double>(skill->bMaxLevel) * powerForDuration / 100.0);
    const int powerForPoint = GetSkillPower(skill->dwVnum, skillLevel);
    skill->SetPointVar("k", static_cast<double>(skill->bMaxLevel) * powerForPoint / 100.0);
    skill->kSplashAroundDamageAdjustPoly.SetVar("k", k);
    LPITEM bow, arrow;
    if (skill->dwFlag & SKILL_FLAG_USE_MELEE_DAMAGE) {
        skill->SetPointVar("atk", CalcMeleeDamage(this, this, true, false));
    } else if (skill->dwFlag & SKILL_FLAG_USE_MAGIC_DAMAGE) {
        skill->SetPointVar("atk", CalcMagicDamage(this, this));
    } else if (skill->dwFlag & SKILL_FLAG_USE_ARROW_DAMAGE) {
        const int attack = GetArrowAndBow(&bow, &arrow, 1) == 1
            ? CalcArrowDamage(this, this, bow, arrow, true) : 0;
        skill->SetPointVar("atk", attack);
    }

    if (skill->bPointOn == POINT_MOV_SPEED)
        skill->SetPointVar("maxv", GetLimitPoint(POINT_MOV_SPEED));
    skill->SetPointVar("lv", GetLevel());
    skill->SetPointVar("iq", GetPoint(POINT_IQ));
    skill->SetPointVar("str", GetPoint(POINT_ST));
    skill->SetPointVar("dex", GetPoint(POINT_DX));
    skill->SetPointVar("con", GetPoint(POINT_HT));
    skill->SetPointVar("maxhp", GetMaxHP());
    skill->SetPointVar("maxsp", GetMaxSP());
    skill->SetPointVar("chain", 0);
    skill->SetPointVar("ar", CalcAttackRating(this, this, false));
    skill->SetPointVar("def", GetPoint(POINT_DEF_GRADE));
    const long defense = GetPoint(POINT_DEF_GRADE);
    skill->SetPointVar("odef", sub(defense, GetPoint(POINT_DEF_GRADE_BONUS)));
    skill->SetPointVar("horse_level", GetHorseLevel());
    if (skill->bSkillAttrType != 1)
        OnMove(true);
    LPITEM weapon = GetWear(4);
    SetPolyVarForAttack(this, skill, weapon);
    skill->SetDurationVar("k", k);

    double amountValue = skill->kPointPoly.Eval();
    const int amount2 = TruncateSkill(skill->kPointPoly2.Eval());
    const int amount3 = TruncateSkill(skill->kPointPoly3.Eval());
    if (GetUsedSkillMasterType(skill->dwVnum) >= 2)
        amountValue = skill->kMasterBonusPoly.Eval();
    const int amount = TruncateSkill(amountValue);
    if (test_server && !amount && skill->bPointOn) {
        ChatPacket(1, "\xc8\xbf\xb0\xfa\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
                      "\xbd\xba\xc5\xb3 \xb0\xf8\xbd\xc4\xc0\xbb \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4");
    }
    char source[] = "char_skill.cpp";
    if ((skill->dwFlag & SKILL_FLAG_REMOVE_BAD_AFFECT) && number_ex(1, 100, source, 0x662) <= amount2)
        RemoveBadAffect();

    // The original first trace passes a double duration to an integer format.
    const double traceDuration = skill->kDurationPoly.Eval();
    sys_log(0, "ComputeSkillAtPosion[%s] vnum %d iDur%d PointOn%d Am1=%d am2=%d am3=%d",
            GetName(), static_cast<int>(vnum), TruncateSkill(traceDuration), skill->bPointOn,
            amount, amount2, amount3);
    const auto addBuffer = [&](int duration) {
        return add(duration, GetPoint(POINT_PARTY_BUFFER_BONUS));
    };
    const auto applyTimed = [&](BYTE point, int value, DWORD flag, int duration, bool overrideExisting) {
        duration = addBuffer(duration);
        if (skill->dwFlag & SKILL_FLAG_SPLASH) {
            if (LPSECTREE tree = GetSectree()) {
                FuncSplashAffect apply(this, position.x, position.y, skill->iSplashRange,
                                       skill->dwVnum, point, value, flag, duration, 0,
                                       overrideExisting, skill->lMaxHit);
                tree->ForEachAround(apply);
            }
        } else {
            AddAffect(skill->dwVnum, point, value, flag, duration, 0, overrideExisting, false);
        }
    };

    bool added = false;
    const bool attack = skill->dwFlag & (SKILL_FLAG_ATTACK | SKILL_FLAG_USE_MELEE_DAMAGE |
                                        SKILL_FLAG_USE_MAGIC_DAMAGE);
    if (attack) {
        if (skill->bPointOn == POINT_HP && amount < 0) {
            sys_log(0, "Errrrrrrrr");
            const int maxHit = skill->lMaxHit;
            const int x = position.x;
            const int y = position.y;
            const bool disableCooltime = m_bDisableCooltime;
            TSkillUseInfo* useInfo = GetDesc() ? &m_SkillUseInfo[vnum] : nullptr;
            const BYTE power = GetSkillPower(vnum, skillLevel);
            FuncSplashDamage apply{x, y, skill, this, amount, 0, 0, maxHit,
                                   weapon, disableCooltime, useInfo, power};
            if (skill->dwFlag & SKILL_FLAG_SPLASH) {
                if (LPSECTREE tree = GetSectree())
                    tree->ForEachAround(apply);
            } else {
                apply(this);
            }
        } else {
            const int duration = TruncateSkill(skill->kDurationPoly.Eval());
            sys_log(0, "ComputeSkillAtPosion[%s] vnum %d iDur%d PointOn%d Am1=%d am2=%d am3=%d",
                    GetName(), static_cast<int>(vnum), duration, skill->bPointOn, amount, amount2, amount3);
            if (GetDesc() && (vnum < 151 || vnum > 162) && !m_bDisableCooltime &&
                !m_SkillUseInfo[vnum].HitOnce(vnum) && vnum != SKILL_MUYEONG)
                return 0;
            if (duration > 0) {
                applyTimed(skill->bPointOn, amount, skill->dwAffectFlag, duration, true);
                added = true;
            }
        }
    } else {
        const int duration = TruncateSkill(skill->kDurationPoly.Eval());
        if (duration > 0) {
            const int primaryDuration = addBuffer(duration);
            skill->kDurationSPCostPoly.SetVar("k", k);
            AddAffect(skill->dwVnum, skill->bPointOn, amount, skill->dwAffectFlag, primaryDuration,
                      TruncateSkill(skill->kDurationSPCostPoly.Eval()), true, false);
            added = true;
        } else {
            PointChange(skill->bPointOn, amount, false, false);
        }
    }

    if (skill->bPointOn2) {
        const int duration = TruncateSkill(skill->kDurationPoly2.Eval());
        if (attack)
            sys_log(1, "try second %lu %d %d", skill->dwVnum, skill->bPointOn2, duration);
        if (duration > 0) {
            if (attack)
                applyTimed(skill->bPointOn2, amount2, skill->dwAffectFlag2, duration, !added);
            else
                AddAffect(skill->dwVnum, skill->bPointOn2, amount2, skill->dwAffectFlag2,
                          addBuffer(duration), 0, !added, false);
            added = true;
        } else {
            PointChange(skill->bPointOn2, amount2, false, false);
        }
    }
    if (GetUsedSkillMasterType(skill->dwVnum) >= 2 && skill->bPointOn3) {
        const int duration = TruncateSkill(skill->kDurationPoly3.Eval());
        sys_log(0, attack ? "try third %lu %d %d %d 1372" : "try third %lu %d %d %d 1491",
                skill->dwVnum, skill->bPointOn3, duration, amount3);
        if (duration <= 0)
            PointChange(skill->bPointOn3, amount3, false, false);
        else if (attack)
            applyTimed(skill->bPointOn3, amount3, 0, duration, !added);
        else
            AddAffect(skill->dwVnum, skill->bPointOn3, amount3, 0, addBuffer(duration), 0, !added, false);
    }
    return attack ? 1 : 0;
}

// 0806af80
int CHARACTER::ComputeCooltime(int time)
{
    return CalculateDuration(GetPoint(POINT_CASTING_SPEED), time);
}

// 08071d80
bool CHARACTER::UseSkill(DWORD vnum, LPCHARACTER victim, bool useGrandMaster)
{
    constexpr DWORD dashFlag = 32;
    constexpr DWORD reviveInvisibleFlag = 28;
    constexpr DWORD reviveInvisibleAffect = 215;
    constexpr DWORD horseSkillType = 5;

    if (test_server && quest::CQuestManager::instance().GetEventFlag("no_grand_master"))
        useGrandMaster = false;
    if (g_bSkillDisable || IsObserverMode() || !CanMove() || IsPolymorphed())
        return false;

    if (vnum == SKILL_HORSE_SUMMON) {
        if (!GetSkillLevel(vnum))
            return false;
        const char* message = GetHorseLevel() ? "\270\273 \274\322\310\257 \276\306\300\314\305\333\300\273 \273\347\277\353\307\317\274\274\277\344." : "\270\273\300\314 \276\370\275\300\264\317\264\331. \270\266\261\302\260\243 \260\346\272\361\272\264\300\273 \303\243\276\306\260\241\274\274\277\344.";
        ChatPacket(1, locale_find(message, g_iUseLocale));
        return true;
    }

    const bool onHorse = IsHorseRiding();
    if (onHorse ? GetHorseGrade() != 3 : m_dwMountVnum != 0)
        return false;

    CSkillProto* skill = CSkillManager::instance().Get(vnum);
    sys_log(0, "%s: USE_SKILL: %d pkVictim %p", GetName(), static_cast<int>(vnum), static_cast<void*>(victim));
    if (!skill || onHorse != (skill->bType == horseSkillType) || !GetSkillLevel(vnum))
        return false;
    if (GetSkillMasterType(vnum) < 2)
        useGrandMaster = false;

    LPITEM weapon = GetWear(4);
    if (weapon && (weapon->GetType() == 13 || weapon->GetType() == 24))
        return false;

    const bool charge = skill->IsChargeSkill();
    if (charge && (IsAffectFlag(dashFlag) || (victim && victim != this))) {
        if (!victim || (!IsAffectFlag(dashFlag) && !UseSkill(vnum, this, true)))
            return false;
        m_SkillUseInfo[vnum].dwVID = victim->GetVID();
        ComputeSkill(vnum, victim);
        RemoveAffect(vnum);
        return true;
    }

    if (vnum == SKILL_COMBO) {
        m_bComboIndex = !m_bComboIndex;
        ChatPacket(5, "combo %d", static_cast<int>(m_bComboIndex));
        return true;
    }
    if ((skill->dwAffectFlag || skill->dwVnum == SKILL_MUYEONG) &&
        (skill->dwFlag & SKILL_FLAG_TOGGLE) && RemoveAffect(skill->dwVnum))
        return true;
    if (IsAffectFlag(reviveInvisibleFlag))
        RemoveAffect(reviveInvisibleAffect);

    const int power = GetSkillPower(skill->dwVnum);
    const float k = static_cast<float>(static_cast<double>(skill->bMaxLevel) * power / 100.0);
    const int pointPower = GetSkillPower(skill->dwVnum);
    skill->SetPointVar("k", static_cast<double>(skill->bMaxLevel) * pointPower / 100.0);
    skill->kSplashAroundDamageAdjustPoly.SetVar("k", k);
    skill->kCooldownPoly.SetVar("k", k);
    const int cooldown = TruncateSkill(skill->kCooldownPoly.Eval());
    const int maxHit = skill->lMaxHit ? static_cast<int>(skill->lMaxHit) : -1;
    skill->SetSPCostVar("k", k);

    const DWORD now = get_dword_time();
    if (vnum == SKILL_TERROR) {
        const TSkillUseInfo& info = m_SkillUseInfo[vnum];
        if (info.bUsed && info.dwNextSkillUsableTime > now) {
            sys_log(0, " SKILL_TERROR's Cooltime is not delta over %lu",
                    static_cast<unsigned long>(info.dwNextSkillUsableTime - now));
            return false;
        }
    }

    const bool costsHP = (skill->dwFlag & SKILL_FLAG_USE_HP_AS_COST) != 0;
    skill->SetSPCostVar("maxhp", GetMaxHP());
    if (!costsHP)
        skill->SetSPCostVar("maxv", GetMaxSP());
    skill->SetSPCostVar("v", costsHP ? GetHP() : GetSP());
    int cost = TruncateSkill(skill->kSPCostPoly.Eval());
    if (GetSkillMasterType(vnum) >= 2 && useGrandMaster)
        cost = TruncateSkill(skill->kGrandMasterAddSPCostPoly.Eval());
    if ((costsHP ? GetHP() : GetSP()) < cost)
        return false;
    if (!costsHP && test_server)
        ChatPacket(1, locale_find("%s SP\274\322\270\360: %d", g_iUseLocale), skill->szName, cost);
    PointChange(costsHP ? POINT_HP : POINT_SP, muyeong_detail::negate(cost));

    const bool selfOnly = (skill->dwFlag & SKILL_FLAG_SELFONLY) != 0;
    if (selfOnly || skill->dwVnum == SKILL_MUYEONG || (charge && !IsAffectFlag(dashFlag) && !victim))
        victim = this;

    if (!m_bDisableCooltime) {
        bool used = true;
        if (vnum == SKILL_HORSE_WILDATTACK)
            used = m_SkillUseInfo[vnum].UseSkill(useGrandMaster, 0, ComputeCooltime(muyeong_detail::mul(cooldown, 1000)), 1, maxHit);
        if (used && victim) {
            const int computedCooldown = ComputeCooltime(muyeong_detail::mul(cooldown, 1000));
            const DWORD victimVID = victim->GetVID();
            used = m_SkillUseInfo[vnum].UseSkill(useGrandMaster, victimVID, computedCooldown, 1, maxHit);
        }
        if (!used) {
            if (test_server)
                ChatPacket(2, "cooltime not finished %s %d", skill->szName, cooldown);
            return false;
        }
    }

    if (vnum == SKILL_CHAIN) {
        ResetChainLightningIndex();
        AddChainLightningExcept(victim);
    }
    if (selfOnly || !(skill->dwFlag & SKILL_FLAG_ATTACK) || vnum == SKILL_BYEURAK ||
        vnum == SKILL_MUYEONG || charge)
        ComputeSkill(vnum, victim);

    m_dwLastSkillTime = get_dword_time();
    return true;
}

void CHARACTER::SkillLevelPacket()
{
    if (!GetDesc())
        return;

    static_assert(sizeof(TPacketGCSkillLevel) == 0x5fb, "Original skill level packet size");
    TPacketGCSkillLevel packet;
    packet.bHeader = 0x4c;
    thecore_memcpy(packet.skills, m_pSkillLevels, sizeof(packet.skills));
    GetDesc()->Packet(&packet, sizeof(packet));
}
