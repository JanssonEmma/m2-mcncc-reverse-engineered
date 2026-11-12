#include "exchange.h"
#include "../../libthecore/include/main.h"
#include <algorithm>
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "locale.hpp"
#include "war_map.h"
#include <cstdio>
#include "char.h"
#include "locale_service.h"
#include "p2p.h"
#include "xmas_event.h"
#include <cstring>
#include "questmanager.h"
#include "motion.h"
#include "char_manager.h"
#include "party.h"
#include "utils.h"
#include <cstdint>
#include "config.h"
#include "mob_manager.h"
#include "sectree_manager.h"
#include "vector.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include <cmath>

// 0807d7c0
bool CHARACTER::IsStoneSkinner() { return (m_pointsInstant.dwAIFlag & (1u << 8)) != 0; }

// 0807d800
bool CHARACTER::IsDeathBlower() { return (m_pointsInstant.dwAIFlag & (1u << 10)) != 0; }

void CHARACTER::SetAggressive()
{
    m_pointsInstant.dwAIFlag|=1;
}

bool CHARACTER::IsAggressive() { return (m_pointsInstant.dwAIFlag & (1u << 0)) != 0; }
void CHARACTER::SetCoward()
{
    m_pointsInstant.dwAIFlag |= 4;
}

bool CHARACTER::IsCoward() { return (m_pointsInstant.dwAIFlag & (1u << 2)) != 0; }
bool CHARACTER::IsBerserker() { return (m_pointsInstant.dwAIFlag & (1u << 7)) != 0; }
bool CHARACTER::IsGodSpeeder() { return (m_pointsInstant.dwAIFlag & (1u << 9)) != 0; }
bool CHARACTER::IsNoAttackShinsu() { return (m_pointsInstant.dwAIFlag & (1u << 3)) != 0; }
bool CHARACTER::IsNoAttackChunjo() { return (m_pointsInstant.dwAIFlag & (1u << 5)) != 0; }
bool CHARACTER::IsNoAttackJinno() { return (m_pointsInstant.dwAIFlag & (1u << 4)) != 0; }
bool CHARACTER::IsAttackMob() { return (m_pointsInstant.dwAIFlag & (1u << 6)) != 0; }

bool CHARACTER::IsReviver()
{
    return (m_pointsInstant.dwAIFlag & (1UL << 11)) != 0;
}

void CHARACTER::SetNoAttackShinsu() { m_pointsInstant.dwAIFlag |= 1u << 3; }
void CHARACTER::SetNoAttackChunjo() { m_pointsInstant.dwAIFlag |= 1u << 5; }
void CHARACTER::SetNoAttackJinno() { m_pointsInstant.dwAIFlag |= 1u << 4; }
void CHARACTER::SetAttackMob() { m_pointsInstant.dwAIFlag |= 1u << 6; }

void CHARACTER::CowardEscape()
{
    const int distances[] = {500, 1000, 3000, 5000};
    for (int band = 2; band >= 0; --band) {
        for (int attempt = 0; attempt < 8; ++attempt) {
            SetRotation(number_ex(0, 359, const_cast<char*>("char_state.cpp"), 0x11d));
            float deltaX, deltaY;
            const int distance = number_ex(distances[band], distances[band + 1],
                                           const_cast<char*>("char_state.cpp"), 0x120);
            GetDeltaByDegree(GetRotation(), distance, &deltaX, &deltaY);
            const int dx = static_cast<int>(deltaX);
            const int dy = static_cast<int>(deltaY);
            int step = 1;
            for (; step <= 100; ++step) {
                if (!SECTREE_MANAGER::instance().IsMovablePosition(
                        GetMapIndex(), GetX() + dx * step / 100, GetY() + dy * step / 100))
                    break;
            }
            if (step <= 100)
                continue;

            m_dwStateDuration = passes_per_sec;
            const long x = GetX() + dx;
            const long y = GetY() + dy;
            if (Goto(x, y))
                SendMovePacket(0, 0, 0, 0, 0, 0, -1);
            sys_log(0, "WAEGU move to %d %d (far)", static_cast<int>(x), static_cast<int>(y));
            return;
        }
    }
}

void CHARACTER::StateHorse()
{
    if (IsDead())
        return;

    m_dwStateDuration = static_cast<std::uint32_t>(passes_per_sec / 2);
    LPCHARACTER rider = GetRider();
    if (!rider) {
        CHARACTER_MANAGER::instance().DestroyCharacter(this);
        return;
    }

    m_pkMobInst->m_posLastAttacked = GetXYZ();
    const auto riderDx = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(GetX()) - static_cast<std::uint32_t>(rider->GetX()));
    const auto riderDy = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(GetY()) - static_cast<std::uint32_t>(rider->GetY()));
    const int distance = DISTANCE_APPROX(riderDx, riderDy);
    if (distance >= 400) {
        if (distance > 700)
            SetNowWalking(false);
        Follow(rider, number_ex(150, 300, const_cast<char*>("char_state.cpp"), 0x50d));
        m_dwStateDuration = 4;
        return;
    }
    if (get_dword_time() <= m_dwLastAttackTime)
        return;

    const DWORD now = get_dword_time();
    m_dwLastAttackTime = static_cast<std::uint32_t>(now + number_ex(5000, 12000, const_cast<char*>("char_state.cpp"), 0x513));
    SetRotation(number_ex(0, 359, const_cast<char*>("char_state.cpp"), 0x515));
    float dx, dy;
    GetDeltaByDegree(GetRotation(), number_ex(200, 400, const_cast<char*>("char_state.cpp"), 0x518),
                     &dx, &dy);
    const auto x = static_cast<std::uint32_t>(GetX());
    const auto y = static_cast<std::uint32_t>(GetY());
    const auto offsetX = static_cast<std::int32_t>(dx);
    const auto offsetY = static_cast<std::int32_t>(dy);
    const auto targetX = static_cast<std::int32_t>(x + offsetX);
    const auto targetY = static_cast<std::int32_t>(y + offsetY);
    const auto midpointX = static_cast<std::int32_t>(x + offsetX / 2);
    const auto midpointY = static_cast<std::int32_t>(y + offsetY / 2);
    auto& sectors = SECTREE_MANAGER::instance();
    if (sectors.IsMovablePosition(GetMapIndex(), targetX, targetY) &&
        sectors.IsMovablePosition(GetMapIndex(), midpointX, midpointY)) {
        SetNowWalking(true);
        if (Goto(targetX, targetY))
            SendMovePacket(0, 0, 0, 0, 0, 0, -1);
    }
}

bool __CHARACTER_GotoNearTarget(LPCHARACTER character, LPCHARACTER victim)
{
    const BYTE type = character->GetMobBattleType();
    const bool ranged = type == 1 || type == 2;
    const int approach = character->GetMobAttackRange() * (ranged ? 8 : 9) / 10;
    if (!character->Follow(victim, approach))
        return false;
    if (test_server) {
        if (ranged)
            character->MonsterLog("\xb8\xb6\xb9\xfd\xbb\xe7\xb3\xaa \xb1\xc3\xbc\xf6\xb4\xc2 \xbf\xf8\xb0\xc5\xb8\xae \xc1\xa2\xb1\xd9");
        else
            character->MonsterLog("\xc0\xfc\xbb\xe7\xb4\xc2 \xb1\xd9\xb0\xc5\xb8\xae \xc1\xa2\xb1\xd9");
    }
    return true;
}

void CHARACTER::StateMove()
{
    const auto elapsed = static_cast<std::uint32_t>(get_dword_time() - m_dwMoveStartTime);
    const auto duration = static_cast<std::uint32_t>(m_dwMoveDuration);
    const float progress = duration ? std::min(1.0f, static_cast<float>(
        static_cast<long double>(static_cast<float>(elapsed)) / duration)) : 1.0f;

    const auto truncatePosition = [](long double position) {
        if (!(position > -2147483649.0L && position < 2147483648.0L))
            return static_cast<std::int32_t>(INT32_MIN);
        return static_cast<std::int32_t>(position);
    };
    // The target rounds progress to float, then truncates each wide coordinate expression.
    const auto interpolate = [progress, truncatePosition](long start, long destination) {
        const auto delta = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(destination) - static_cast<std::uint32_t>(start));
        const long double position = static_cast<std::int32_t>(start) +
                                     static_cast<long double>(delta) * progress;
        return truncatePosition(position);
    };
    const int x = interpolate(m_posStart.x, m_posDest.x);
    const int y = interpolate(m_posStart.y, m_posDest.y);
    Move(x, y);

    if (GetDesc() && (thecore_pulse() & 15) == 0) {
        UpdateSectree();
        if (m_pkExchange) {
            LPCHARACTER other = m_pkExchange->GetCompany()->GetOwner();
            const auto dx = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(GetX()) - static_cast<std::uint32_t>(other->GetX()));
            const auto dy = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(GetY()) - static_cast<std::uint32_t>(other->GetY()));
            if (DISTANCE_APPROX(dx, dy) >= 1000)
                m_pkExchange->Cancel();
        }
    }

    if (!GetDesc()) {
        if (GetCharType() == CHAR_TYPE_MONSTER) {
            if (LPCHARACTER victim = GetVictim())
                UpdateAggrPoint(victim, DAMAGE_TYPE_NORMAL, -1 - victim->GetLevel() / 3);
        }
        if (GetCharType() == CHAR_TYPE_MONSTER && GetMobRank() >= 4) {
            if (LPCHARACTER victim = GetVictim()) {
                if (GetRaceNum() == 2191 &&
                    number_ex(1, 20, const_cast<char*>("char_state.cpp"), 0x35a) == 1 &&
                    static_cast<std::uint32_t>(get_dword_time() - m_pkMobInst->m_dwLastWarpTime) > 1000) {
                    float dx, dy;
                    GetDeltaByDegree(victim->GetRotation(), 400.0f, &dx, &dy);
                    const auto targetX = static_cast<std::int32_t>(
                        static_cast<std::uint32_t>(victim->GetX()) + truncatePosition(dx));
                    const auto targetY = static_cast<std::int32_t>(
                        static_cast<std::uint32_t>(victim->GetY()) + truncatePosition(dy));
                    SetRotation(GetDegreeFromPositionXY(targetX, targetY, victim->GetX(), victim->GetY()));
                    Show(victim->GetMapIndex(), targetX, targetY, 0, true);
                    GotoState(m_stateBattle);
                    m_dwStateDuration = 1;
                    ResetMobSkillCooltime();
                    m_pkMobInst->m_dwLastWarpTime = get_dword_time();
                    return;
                }
                if (number_ex(0, 10, const_cast<char*>("char_state.cpp"), 0x36b) == 0 &&
                    __CHARACTER_GotoNearTarget(this, victim))
                    return;
            }
        }
    } else {
        if ((m_bNowWalking || m_points.stamina < 1) && m_points.stamina < m_pointsInstant.iMaxStamina &&
            static_cast<std::uint32_t>(get_dword_time() - m_dwWalkStartTime) >= 5000) {
            PointChange(POINT_STAMINA, m_pointsInstant.iMaxStamina, false, false);
        }
        if (!m_bNowWalking && m_points.stamina > 0 && !IsHorseRiding()) {
            if (static_cast<std::uint32_t>(get_dword_time() - m_dwLastAttackTime) < 20000) {
                StartAffectEvent();
                if (!IsStaminaHalfConsume() || (thecore_pulse() & 1))
                    PointChange(POINT_STAMINA, -1, false, false);
                StartStaminaConsume();
                if (m_points.stamina <= 0) {
                    m_points.stamina = 0;
                    SetNowWalking(true);
                    StopStaminaConsume();
                }
            } else if (IsStaminaConsume()) {
                StopStaminaConsume();
            }
        }
    }

    if (progress != 1.0f)
        return;
    if (GetDesc()) {
        sys_log(1, "\xb5\xb5\xc2\xf8 %s %d %d", GetName(), x, y);
        GotoState(m_stateIdle);
        StopStaminaConsume();
    } else if (!GetVictim() || IsCoward()) {
        if (!IsState(m_stateIdle))
            MonsterLog("[IDLE] \xb4\xeb\xbb\xf3\xc0\xcc \xbe\xf8\xc0\xb8\xb4\xcf \xbd\xac\xc0\xda");
        GotoState(m_stateIdle);
        const int idleSeconds = number_ex(1, 3, const_cast<char*>("char_state.cpp"), 0x3ab);
        m_dwStateDuration = static_cast<std::uint32_t>(idleSeconds) * passes_per_sec;
    } else {
        if (!IsState(m_stateBattle))
            MonsterLog("[BATTLE] \xb1\xd9\xc3\xb3\xbf\xa1 \xbf\xd4\xc0\xb8\xb4\xcf \xb0\xf8\xb0\xdd\xbd\xc3\xc0\xdb ; %s",
                       GetVictim()->GetName());
        GotoState(m_stateBattle);
        m_dwStateDuration = 1;
    }
}

LPCHARACTER FindVictim(LPCHARACTER mob, int maxDistance);

void CHARACTER::StateBattle()
{
    if (GetCharType() == CHAR_TYPE_STONE) {
        _sys_err("StateBattle", 0x3b5, "Stone must not use battle state (name %s)", GetName());
        return;
    }
    if (GetDesc() || !CanMove() || IsStun())
        return;

    LPCHARACTER victim = GetVictim();
    if (IsCoward()) {
        if (IsDead())
            return;
        SetVictim(nullptr);
        if (number_ex(1, 50, const_cast<char*>("char_state.cpp"), 0x3cb) == 1) {
            CowardEscape();
        } else {
            GotoState(m_stateIdle);
            m_dwStateDuration = 1;
        }
        return;
    }

    if (!victim || (victim->IsStun() && IsGuardNPC()) || victim->IsDead()) {
        if (victim && victim->IsDead() && !no_wander && IsAggressive() &&
            (!m_pkParty || m_pkParty->GetLeader() == this)) {
            LPCHARACTER next = FindVictim(this, GetMobTable().wAggressiveSight);
            SetVictim(next);
            m_dwStateDuration = passes_per_sec;
            if (next)
                return;
            const BYTE type = GetMobBattleType();
            if (type != 0 && (type < 4 || type > 7))
                return;

            const int distance = number_ex(400, 1500, const_cast<char*>("char_state.cpp"), 0x3eb);
            const int rotation = number_ex(0, 359, const_cast<char*>("char_state.cpp"), 0x3ed);
            float dx, dy;
            GetDeltaByDegree(rotation, distance, &dx, &dy);
            const int ix = static_cast<int>(dx);
            const int iy = static_cast<int>(dy);
            const auto x = static_cast<std::int32_t>(victim->GetX());
            const auto y = static_cast<std::int32_t>(victim->GetY());
            const auto addOffset = [](std::int32_t coordinate, int offset) {
                return static_cast<std::int32_t>(static_cast<std::uint32_t>(coordinate) +
                                                 static_cast<std::uint32_t>(offset));
            };
            auto& sectors = SECTREE_MANAGER::instance();
            if (!sectors.IsMovablePosition(victim->GetMapIndex(), addOffset(x, ix), addOffset(y, iy)) ||
                !sectors.IsMovablePosition(victim->GetMapIndex(), addOffset(x, ix / 2), addOffset(y, iy / 2)))
                return;

            const float destinationX = static_cast<float>(static_cast<double>(x) + dx);
            const float destinationY = static_cast<float>(static_cast<double>(y) + dy);
            const auto truncateDestination = [](float coordinate) {
                return coordinate >= -0x1p31f && coordinate < 0x1p31f
                    ? static_cast<std::int32_t>(coordinate) : INT32_MIN;
            };
            SetRotation(GetDegreeFromPosition(destinationX, destinationY));
            if (Goto(truncateDestination(destinationX), truncateDestination(destinationY))) {
                sys_log(0, "KILL_AND_GO: %s distance %.1f", GetName(), static_cast<double>(distance));
                SendMovePacket(0, 0, 0, 0, 0, 0, -1);
            }
            return;
        }

        SetVictim(nullptr);
        if (IsGuardNPC())
            Return();
        m_dwStateDuration = passes_per_sec;
        return;
    }

    if (IsSummonMonster() && !IsDead() && !IsStun()) {
        CParty* party = m_pkParty ? m_pkParty : CPartyManager::instance().CreateParty(this);
        if (number_ex(0, 3, const_cast<char*>("char_state.cpp"), 0x419) == 0 &&
            party->CountMemberByVnum(GetSummonVnum()) < 3) {
            MonsterLog("\xba\xce\xc7\xcf \xb8\xf3\xbd\xba\xc5\xcd \xbc\xd2\xc8\xaf !");
            LPCHARACTER summoned = CHARACTER_MANAGER::instance().SpawnMobRange(
                GetSummonVnum(), GetMapIndex(), GetX() - 300, GetY() - 300,
                GetX() + 300, GetY() + 300, true, true, false);
            if (summoned) {
                party->Join(summoned->GetVID());
                party->Link(summoned);
            }
        }
    }
    if (m_pkParty)
        m_pkParty->SendMessage(this, 2, 0, 0);

    LPCHARACTER protege = GetProtege();
    const int distance = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());
    if (distance >= 4000) {
        MonsterLog("\xc5\xb8\xb0\xd9\xc0\xcc \xb8\xd6\xbe\xee\xbc\xad \xc6\xf7\xb1\xe2");
        SetVictim(nullptr);
        if (protege && DISTANCE_APPROX(GetX() - protege->GetX(), GetY() - protege->GetY()) > 1000)
            Follow(protege, number_ex(150, 400, const_cast<char*>("char_state.cpp"), 0x43d));
        return;
    }
    if (distance >= GetMobAttackRange() * 1.15) {
        if (!(m_pointsInstant.dwAIFlag & 2u))
            __CHARACTER_GotoNearTarget(this, victim);
        return;
    }

    const DWORD now = get_dword_time();
    const auto elapsed = static_cast<std::uint32_t>(now - m_dwLastAttackTime);
    const auto attackInterval = static_cast<std::uint32_t>(
        CalculateDuration(GetLimitPoint(POINT_ATT_SPEED), 2000));
    if (elapsed < attackInterval) {
        m_dwStateDuration = std::max<DWORD>(1, passes_per_sec * (attackInterval - elapsed) / 1000);
        return;
    }
    if (IsBerserker() && GetHPPct() < GetMobTable().bBerserkPoint && !IsBerserk())
        SetBerserk(true);
    if (IsGodSpeeder() && GetHPPct() < GetMobTable().bGodSpeedPoint && !IsGodSpeed())
        SetGodSpeed(true);

    const auto setMotionDuration = [this](DWORD motion) {
        const float duration = CMotionManager::instance().GetMotionDuration(GetRaceNum(), motion);
        if (duration == 0.0f) {
            m_dwStateDuration = static_cast<std::uint32_t>(passes_per_sec) * 2u;
            return;
        }
        const long double ticks = static_cast<long double>(passes_per_sec) * duration;
        constexpr long double signedLimit = 0x1p63L;
        m_dwStateDuration = ticks >= -signedLimit && ticks < signedLimit
            ? static_cast<std::uint32_t>(static_cast<std::int64_t>(ticks))
            : 0;
    };
    if (HasMobSkill()) {
        for (int i = 0; i < CountMobSkill(); ++i) {
            if (!CanUseMobSkill(i))
                continue;
            SetRotationToXY(victim->GetX(), victim->GetY());
            if (UseMobSkill(i)) {
                if (test_server)
                    sys_log(0, "UseMobSkill[%s] SkillIndex %d %d ", GetName(), i,
                            static_cast<int>(GetMobSkill(i)->dwSkillVnum));
                SendMovePacket(4, static_cast<BYTE>(i), GetX(), GetY(), 0, now, -1);
                setMotionDuration((i + 0x1d) * 0x100);
                return;
            }
            if (test_server)
                sys_log(0, "UseMobSkill Failed[%s] SkillIndex %d %d", GetName(), i,
                        static_cast<int>(GetMobSkill(i)->dwSkillVnum));
        }
    }
    if (!Attack(victim, 0)) {
        m_dwStateDuration = passes_per_sec / 2;
        return;
    }
    SetRotationToXY(victim->GetX(), victim->GetY());
    SendMovePacket(2, 0, GetX(), GetY(), 0, now, -1);
    setMotionDuration(0xd00);
}

void CHARACTER::__StateIdle_Stone()
{
    m_dwStateDuration = passes_per_sec;
    if (GetMaxHP() <= 0)
        return;

    struct SpawnWave {
        int maxHPPercent;
        int stage;
        int randomLine;
        int radii[3];
    };
    static const SpawnWave waves[] = {
        {10, 10, 0x18a, { 500, 1000, 1500}},
        {20,  9, 0x196, { 500, 1000, 1500}},
        {30,  8, 0x1a2, { 500, 1000, 1000}},
        {40,  7, 0x1ae, {1000, 1000, 1000}},
        {50,  6, 0x1ba, {1000, 1000,    0}},
        {60,  5, 0x1c5, {1000,  500,    0}},
        {70,  4, 0x1d0, { 500, 1000,    0}},
        {80,  3, 0x1db, {1000, 1000,    0}},
        {90,  2, 0x1e6, { 500,    0,    0}},
        {99,  1, 0x1f0, {1000,    0,    0}},
    };

    const auto scaledHP = static_cast<std::int32_t>(static_cast<std::uint32_t>(GetHP()) * 100U);
    const int hpPercent = scaledHP / GetMaxHP();
    for (const SpawnWave& wave : waves) {
        if (hpPercent > wave.maxHPPercent || m_pointsInstant.iMaxSP >= wave.stage)
            continue;
        m_pointsInstant.iMaxSP = wave.stage;
        SendMovePacket(2, 0, GetX(), GetY(), 0, 0, -1);
        auto& manager = CHARACTER_MANAGER::instance();
        manager.SelectStone(this);
        const long lastGroup = GetPoint(POINT_MOV_SPEED);
        const long firstGroup = GetPoint(POINT_ATT_SPEED);
        const DWORD group = number_ex(firstGroup, lastGroup,
                                      const_cast<char*>("char_state.cpp"), wave.randomLine);
        for (int radius : wave.radii) {
            if (radius)
                manager.SpawnGroup(group, GetMapIndex(), GetX() - radius, GetY() - radius,
                                   GetX() + radius, GetY() + radius, nullptr, false, nullptr);
        }
        manager.SelectStone(nullptr);
        UpdatePacket();
        return;
    }
}

void CHARACTER::__StateIdle_Monster()
{
    if (IsStun() || !CanMove())
        return;
    if (IsCoward()) {
        if (!IsDead())
            CowardEscape();
        return;
    }
    if (IsBerserker() && IsBerserk())
        SetBerserk(false);
    if (IsGodSpeeder() && IsGodSpeed())
        SetGodSpeed(false);

    LPCHARACTER victim = GetVictim();
    if (!victim || victim->IsDead()) {
        victim = nullptr;
        SetVictim(nullptr);
        m_dwStateDuration = passes_per_sec;
    }
    if (!victim || victim->GetCharType() == CHAR_TYPE_BUILDING) {
        if (m_pkChrStone) {
            victim = m_pkChrStone->GetNearestVictim(m_pkChrStone);
        } else if (!no_wander && IsAggressive() &&
                   (GetMapIndex() != 61 || !quest::CQuestManager::instance().GetEventFlag("xmas_tree"))) {
            victim = FindVictim(this, GetMobTable().wAggressiveSight);
        }
    }
    if (victim && !victim->IsDead()) {
        if (CanBeginFight())
            BeginFight(victim);
        return;
    }

    const bool aggressiveIdle = !victim && IsAggressive();
    m_dwStateDuration = number_ex(aggressiveIdle ? 1 : 3, aggressiveIdle ? 3 : 5,
                                  const_cast<char*>("char_state.cpp"), aggressiveIdle ? 0x2b7 : 0x2b9)
        * passes_per_sec;
    LPCHARACTER protege = GetProtege();
    if (protege && DISTANCE_APPROX(GetX() - protege->GetX(), GetY() - protege->GetY()) > 1000 &&
        Follow(protege, number_ex(150, 400, const_cast<char*>("char_state.cpp"), 0x2c2))) {
        MonsterLog("[IDLE] \xb8\xae\xb4\xf5\xb7\xce\xba\xce\xc5\xcd \xb3\xca\xb9\xab \xb8\xd6\xb8\xae \xb6\xb3\xbe\xee\xc1\xb3\xb4\xd9! \xba\xb9\xb1\xcd\xc7\xd1\xb4\xd9.");
        return;
    }
    if (no_wander || (m_pointsInstant.dwAIFlag & 2u) ||
        number_ex(0, 6, const_cast<char*>("char_state.cpp"), 0x2cf) != 0) {
        MonsterChat(0);
        return;
    }

    SetRotation(number_ex(0, 359, const_cast<char*>("char_state.cpp"), 0x2d1));
    float dx, dy;
    GetDeltaByDegree(GetRotation(), number_ex(300, 700, const_cast<char*>("char_state.cpp"), 0x2d4),
                     &dx, &dy);
    const int offsetX = static_cast<int>(dx);
    const int offsetY = static_cast<int>(dy);
    auto& sectors = SECTREE_MANAGER::instance();
    if (sectors.IsMovablePosition(GetMapIndex(), GetX() + offsetX, GetY() + offsetY) &&
        sectors.IsMovablePosition(GetMapIndex(), GetX() + offsetX / 2, GetY() + offsetY / 2) &&
        Goto(GetX() + offsetX, GetY() + offsetY)) {
        SendMovePacket(0, 0, 0, 0, 0, 0, -1);
    }
}

void CHARACTER::StateIdle()
{
    switch (GetCharType()) {
        case CHAR_TYPE_STONE:
            __StateIdle_Stone();
            return;
        case CHAR_TYPE_WARP:
        case CHAR_TYPE_GOTO:
            m_dwStateDuration = passes_per_sec * 60;
            return;
        default:
            if (!GetDesc()) {
                if (GetCharType() == CHAR_TYPE_MONSTER)
                    __StateIdle_Monster();
                else
                    __StateIdle_NPC();
            }
            return;
    }
}

namespace {
struct FuncFindGuardVictim
{
    explicit FuncFindGuardVictim(LPCHARACTER guard) : x(guard->GetX()), y(guard->GetY()) {}

    void operator()(LPENTITY entity)
    {
        if (!entity->IsType(0) || entity->GetDesc())
            return;

        LPCHARACTER target = static_cast<LPCHARACTER>(entity);
        const BYTE type = target->GetCharType();
        if ((type != CHAR_TYPE_MONSTER && type != CHAR_TYPE_PC) || target->IsDead() ||
            target->IsAffectFlag(19) || target->IsAffectFlag(2) || target->IsAffectFlag(28) ||
            target->GetRaceNum() == 5001)
            return;

        const auto dx = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(x) - static_cast<std::uint32_t>(target->GetX()));
        const auto dy = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(y) - static_cast<std::uint32_t>(target->GetY()));
        const int distance = DISTANCE_APPROX(dx, dy);
        if (distance < nearest && distance <= 50000) {
            nearest = distance;
            victim = target;
        }
    }

    const long x;
    const long y;
    int nearest = 0x7fffffff;
    LPCHARACTER victim = nullptr;
};
}

void CHARACTER::__StateIdle_NPC()
{
    MonsterChat(0);
    m_dwStateDuration = passes_per_sec * 5;
    if (IsGuardNPC()) {
        if (quest::CQuestManager::instance().GetEventFlag("noguard") || !GetSectree())
            return;
        FuncFindGuardVictim findGuardVictim(this);
        GetSectree()->ForEachAround(findGuardVictim);
        if (findGuardVictim.victim) {
            m_dwStateDuration = passes_per_sec / 2;
            if (CanBeginFight())
                BeginFight(findGuardVictim.victim);
        }
        return;
    }

    if (GetRaceNum() == 20031 && get_dword_time() > m_dwPlayStartTime) {
        const int delay = LC_IsJapan() ? 60000 : 2000;
        m_dwPlayStartTime = static_cast<std::uint32_t>(get_dword_time() + delay);
        static const int santaMaps[] = {61, 62, 63, 64, 3, 23, 43};
        const int nextMap = santaMaps[number_ex(1, 7, const_cast<char*>("char_state.cpp"), 0x22c) - 1];
        if (map_allow_find(nextMap)) {
            CHARACTER_MANAGER::instance().DestroyCharacter(this);
            xmas::SpawnSanta(nextMap, LC_IsYMIR() ? 1200 : 3000);
        } else {
            BYTE packet[6] = {17, g_bChannel};
            const std::int32_t wireMap = nextMap;
            std::memcpy(packet + 2, &wireMap, sizeof(wireMap));
            P2P_MANAGER::instance().Send(packet, sizeof(packet), nullptr);
        }
        return;
    }
    if (m_pointsInstant.dwAIFlag & 2u)
        return;

    LPCHARACTER protege = GetProtege();
    if (protege && DISTANCE_APPROX(GetX() - protege->GetX(), GetY() - protege->GetY()) > 500 &&
        Follow(protege, number_ex(100, 300, const_cast<char*>("char_state.cpp"), 0x251)))
        return;
    if (number_ex(0, 6, const_cast<char*>("char_state.cpp"), 0x256) != 0)
        return;

    MonsterLog("\xb3\xaa\xb4\xc2 \xbf\xf2\xc1\xf7\xc0\xcc\xb4\xc2 NPC\xb4\xcf \xbf\xf2\xc1\xf7\xbf\xa9 \xba\xbc\xb1\xee\xb3\xaa?");
    SetRotation(number_ex(0, 359, const_cast<char*>("char_state.cpp"), 0x25a));
    float dx, dy;
    GetDeltaByDegree(GetRotation(), number_ex(200, 400, const_cast<char*>("char_state.cpp"), 0x25d),
                     &dx, &dy);
    const int offsetX = static_cast<int>(dx);
    const int offsetY = static_cast<int>(dy);
    auto& sectors = SECTREE_MANAGER::instance();
    if (sectors.IsMovablePosition(GetMapIndex(), GetX() + offsetX, GetY() + offsetY) &&
        sectors.IsMovablePosition(GetMapIndex(), GetX() + offsetX / 2, GetY() + offsetY / 2)) {
        SetNowWalking(true);
        if (Goto(GetX() + offsetX, GetY() + offsetY))
            SendMovePacket(0, 0, 0, 0, 0, 0, -1);
    }
}

namespace {
const DWORD warFlagAffect = 222;
const char flagTaken[] = "%s \xb1\xe6\xb5\xe5\xc0\xc7 \xb1\xea\xb9\xdf\xc0\xbb %s \xb4\xd4\xc0\xcc \xc8\xb9\xb5\xe6\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.";
const char flagCaptured[] = "%s \xb1\xe6\xb5\xe5\xb0\xa1 %s \xb1\xe6\xb5\xe5\xc0\xc7 \xb1\xea\xb9\xdf\xc0\xbb \xbb\xa9\xbe\xd1\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9!";

bool HasWarFlag(LPCHARACTER character)
{
    return character->IsAffectFlag(36) || character->IsAffectFlag(37) ||
           character->IsAffectFlag(38);
}

LPCHARACTER GetFlagCandidate(LPENTITY entity)
{
    if (!entity->IsType(0) || entity->IsObserverMode() || !entity->GetDesc())
        return nullptr;
    LPCHARACTER character = static_cast<LPCHARACTER>(entity);
    return character->GetGuild() ? character : nullptr;
}
}

void CHARACTER::StateFlag()
{
    m_dwStateDuration = passes_per_sec / 2;
    CWarMap* map = GetWarMap();
    if (!map || !GetSectree())
        return;

    const DWORD owner = GetPoint(POINT_STAT);
    LPCHARACTER closest = nullptr;
    int minDistance = 501;
    auto findCarrier = [&](LPENTITY entity) {
        LPCHARACTER character = GetFlagCandidate(entity);
        if (!character || character->IsDead())
            return;
        const int distance = DISTANCE_APPROX(character->GetX() - GetX(), character->GetY() - GetY());
        if (distance >= minDistance || HasWarFlag(character))
            return;
        if (owner == character->GetGuild()->GetID()) {
            CWarMap* characterMap = character->GetWarMap();
            BYTE team;
            if (!characterMap || !characterMap->GetTeamIndex(owner, team) ||
                characterMap->IsFlagOnBase(team))
                return;
        }
        minDistance = distance;
        closest = character;
    };
    GetSectree()->ForEachAround(findCarrier);
    BYTE team;
    if (!closest || !closest->GetGuild() || !map->GetTeamIndex(owner, team))
        return;

    closest->AddAffect(warFlagAffect, POINT_NONE, owner, 36 + (team != 0), 0x1fffffff, 0, false, false);
    closest->AddAffect(warFlagAffect, POINT_MOV_SPEED, 50 - closest->GetPoint(POINT_MOV_SPEED),
                       0, 0x1fffffff, 0, false, false);
    map->RemoveFlag(team);
    char notice[256];
    std::snprintf(notice, sizeof(notice), locale_find(flagTaken, g_iUseLocale),
                  map->GetGuild(team)->GetName(), closest->GetName());
    map->Notice(notice);
}

void CHARACTER::StateFlagBase()
{
    m_dwStateDuration = passes_per_sec / 2;
    if (!GetSectree())
        return;

    auto capture = [&](LPENTITY entity) {
        LPCHARACTER character = GetFlagCandidate(entity);
        if (!character)
            return;
        const int distance = DISTANCE_APPROX(character->GetX() - GetX(), character->GetY() - GetY());
        if (distance > 500 || !HasWarFlag(character))
            return;

        CAffect* flag = character->FindAffect(warFlagAffect, POINT_NONE);
        const DWORD owner = GetPoint(POINT_STAT);
        const DWORD guild = character->GetGuild()->GetID();
        sys_log(0, "FlagBase %s dist %d aff %p flag gid %ld chr gid %lu",
                character->GetName(), distance, static_cast<void*>(flag), static_cast<long>(owner), guild);
        if (!flag || owner != guild || static_cast<long>(owner) == flag->lApplyValue)
            return;
        CWarMap* map = character->GetWarMap();
        BYTE team;
        if (!map || !map->GetTeamIndex(guild, team))
            return;

        SendGuildWarScore(owner, flag->lApplyValue, 1, 0);
        map->ResetFlag();
        char notice[256];
        std::snprintf(notice, sizeof(notice), locale_find(flagCaptured, g_iUseLocale),
                      map->GetGuild(team)->GetName(), map->GetGuild(1 - team)->GetName());
        map->Notice(notice);
    };
    GetSectree()->ForEachAround(capture);
}
