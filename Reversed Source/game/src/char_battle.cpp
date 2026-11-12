#include "locale_service.h"
#include "utils.h"
#include <cstring>
#include <cassert>
#include <queue>
#include "vector.h"
#include <algorithm>
#include <cstdlib>
#include "log.h"
#include "mob_manager.h"
#include "desc.h"
#include "dev_log.h"
#include <cstdint>
#include "battle.h"
#include "profiler.h"
#include "cube.h"
#include "../../libthecore/include/utils.h"
#include "item.h"
#include "char.h"
#include "config.h"
#include "party.h"
#include "questmanager.h"
#include "marriage.h"
#include "constants.h"
#include "db.h"
#include "item_manager.h"
#include "priv_manager.h"
#include "sectree.h"
#include "sectree_manager.h"
#include "questlua_forked.h"
#include "regen.h"
#include "arena.h"
#include "castle.h"
#include "dungeon.h"
#include "exchange.h"
#include "guild.h"
#include "guild_manager.h"
#include "pvp.h"
#include "shop_manager.h"

void SendAffectAddPacket(LPDESC descriptor, CAffect* affect);
#include <cstdio>

void SendNotice(const char* text);
#include "../../libthecore/include/log.h"
// Insert in game/src/char_battle.cpp, original char_battle.cpp DWARF.
// Requires char.h's complete old EPositions and existing CHARACTER records.
// 081485e0: load32 CHARACTER+0x92c, compare zero.
bool CHARACTER::IsDead() {
    return m_pointsInstant.position==POS_DEAD;
}
// 08148080: bit3 test CHARACTER+0x930; no HP/position/affect condition.
bool CHARACTER::IsStun() {
    return (m_pointsInstant.instant_flag & 8L)!=0;
}

//081541e0 (original char_battle.cpp). Existing real UpdatePacket fanout required.
void CHARACTER::SetPKMode(BYTE bPKMode) {
    if(bPKMode>=PK_MODE_MAX_NUM || m_bPKMode==bPKMode) return;
    if(bPKMode==PK_MODE_GUILD && !m_pGuild) bPKMode=PK_MODE_FREE;
    // Equality was tested before fallback. FREE->GUILD without guild still
    // emits real update/log even though final value remainsFREE.
    m_bPKMode=bPKMode;
    UpdatePacket();
    sys_log(0,"PK_MODE: %s %d",GetName(),static_cast<unsigned int>(m_bPKMode));
}

#include "char.h"
#include "char_manager.h"
void CHARACTER::ChangeVictimByAggro(int aggro, LPCHARACTER attacker)
{
    const auto elapsed = static_cast<std::uint32_t>(get_dword_time() - m_dwLastVictimSetTime);
    if (elapsed < 3000)
        return;

    if (attacker == GetVictim()) {
        if (m_iMaxAggro < aggro) {
            m_iMaxAggro = aggro;
            return;
        }
        auto selected = m_map_kDamage.end();
        for (auto it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it) {
            if (it->second.iAggro <= aggro)
                continue;
            CHARACTER* candidate = CHARACTER_MANAGER::instance().Find(it->first);
            if (!candidate || candidate->IsDead())
                continue;
            const auto dx = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(candidate->GetX()) - static_cast<std::uint32_t>(GetX()));
            const auto dy = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(candidate->GetY()) - static_cast<std::uint32_t>(GetY()));
            if (DISTANCE_APPROX(dx, dy) < 5000) {
                selected = it;
                aggro = it->second.iAggro;
            }
        }
        if (selected == m_map_kDamage.end())
            return;
        m_iMaxAggro = aggro;
        attacker = CHARACTER_MANAGER::instance().Find(selected->first);
    } else {
        if (aggro <= m_iMaxAggro)
            return;
        m_iMaxAggro = aggro;
    }

    SetVictim(attacker);
    m_dwStateDuration = 1;
}

void CHARACTER::UpdateAggrPointEx(LPCHARACTER attacker, EDamageType type, int damage, TBattleInfo* info)
{
    const auto scale = [](int value, float multiplier) {
        const long double scaled = static_cast<long double>(value) * multiplier;
        if (!(scaled > -2147483649.0L && scaled < 2147483648.0L))
            return static_cast<int>(INT32_MIN);
        return static_cast<int>(scaled);
    };
    if (type == DAMAGE_TYPE_RANGE)
        damage = scale(damage, 1.5f);
    else if (type == DAMAGE_TYPE_NORMAL_RANGE || type == DAMAGE_TYPE_MAGIC)
        damage = scale(damage, 1.2f);
    if (attacker == GetVictim())
        damage = scale(damage, 1.2f);

    const auto sum = static_cast<std::uint32_t>(info->iAggro) + static_cast<std::uint32_t>(damage);
    info->iAggro = std::max(0, static_cast<std::int32_t>(sum));
    if (m_pkParty && damage > 0 && type != DAMAGE_TYPE_SPECIAL) {
        const int distributed = damage / (m_pkParty->GetLeaderPID() == static_cast<DWORD>(GetVID()) ? 2 : 3);
        m_pkParty->SendMessage(this, 3, distributed, static_cast<DWORD>(attacker->GetVID()));
    }
    ChangeVictimByAggro(info->iAggro, attacker);
}

void CHARACTER::UpdateAggrPoint(LPCHARACTER attacker, EDamageType type, int damage)
{
    if (IsDead() || IsStun())
        return;

    TBattleInfo initial;
    initial.iAggro = damage;
    initial.iTotalDamage = 0;
    const auto entry = m_map_kDamage.emplace(attacker->GetVID(), initial).first;
    UpdateAggrPointEx(attacker, type, damage, &entry->second);
}

LPCHARACTER CHARACTER::GetVictim() { return CHARACTER_MANAGER::instance().Find(m_kVIDVictim); }

#include "packet.h"
void CHARACTER::CreateFly(BYTE type,LPCHARACTER victim)
{
    TPacketGCCreateFly packet;
    packet.bHeader=70;packet.bType=type;
    packet.dwStartVID=static_cast<DWORD>(m_vid);packet.dwEndVID=static_cast<DWORD>(victim->m_vid);
    PacketAround(&packet,sizeof(packet),0);
}

#include "../../libthecore/include/main.h"

BYTE CHARACTER::GetPKMode() { return m_bPKMode; }
int CHARACTER::GetAlignment() { return m_iAlignment; }
bool CHARACTER::IsKillerMode() { return (m_bAddChrState & 8) != 0; }

// 081540c0
void CHARACTER::SetKillerMode(bool on)
{
    if (IsKillerMode() == on)
        return;
    if (on)
        m_bAddChrState |= 8;
    else
        m_bAddChrState &= 0xf7;
    m_iKillerModePulse = thecore_pulse();
    UpdatePacket();
    const DWORD pid = m_dwPlayerID;
    sys_log(0, "SetKillerMode Update %s[%d]", GetName(), pid);
}

// 08153960
int CHARACTER::GetArrowAndBow(LPITEM* bow, LPITEM* arrow, int count)
{
    LPITEM weapon = GetWear(4);
    if (!weapon || weapon->GetProto()->bSubType != 2)
        return 0;
    LPITEM ammo = GetWear(9);
    if (!ammo || ammo->GetType() != 1 || ammo->GetProto()->bSubType != 6)
        return 0;
    const int arrows = MIN(count, ammo->GetCount());
    *bow = weapon;
    *arrow = ammo;
    return arrows;
}

// 08147670
void CHARACTER::AggregateMonster()
{
    SECTREE* tree = GetSectree();
    if (!tree)
        return;

    auto aggregate = [this](LPENTITY entity) {
        if (!entity->IsType(0) || entity->GetDesc())
            return;
        auto* monster = static_cast<CHARACTER*>(entity);
        if (monster->m_bCharType != 0 || monster->GetVictim())
            return;
        if (number_ex(1, 100, "char_battle.cpp", 0x103d) > 50)
            return;
        const auto dx = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(monster->GetX()) - static_cast<std::uint32_t>(GetX()));
        const auto dy = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(monster->GetY()) - static_cast<std::uint32_t>(GetY()));
        if (DISTANCE_APPROX(dx, dy) < 5000 && monster->CanBeginFight())
            monster->BeginFight(this);
    };
    tree->ForEachAround(aggregate);
}

void CHARACTER::ForgetMyAttacker()
{
    if (SECTREE* tree = GetSectree())
    {
        auto forget = [this](LPENTITY entity) {
            if (!entity->IsType(0) || entity->GetDesc())
                return;
            auto* ch = static_cast<CHARACTER*>(entity);
            if (ch->m_kVIDVictim == m_vid)
                ch->SetVictim(nullptr);
        };
        tree->ForEachAround(forget);
    }
    ReviveInvisible(5);
}

void CHARACTER::PullMonster()
{
    SECTREE* tree = GetSectree();
    if (!tree)
        return;
    auto pull = [this](LPENTITY entity) {
        if (!entity->IsType(0) || entity->GetDesc())
            return;
        auto* monster = static_cast<CHARACTER*>(entity);
        if (monster->m_bCharType != 0)
            return;
        const auto deltaX = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(GetX()) - static_cast<std::uint32_t>(monster->GetX()));
        const auto deltaY = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(GetY()) - static_cast<std::uint32_t>(monster->GetY()));
        const int distance = DISTANCE_APPROX(deltaX, deltaY);
        if (distance < 100 || distance > 3000)
            return;
        const int remaining = std::max(100, distance - 300);
        const float degree = GetDegreeFromPositionXY(monster->GetX(), monster->GetY(), GetX(), GetY());
        float dx, dy;
        GetDeltaByDegree(degree, distance - remaining, &dx, &dy);
        const auto applyOffset = [](long position, float offset) {
            const long double result = static_cast<std::int32_t>(position) +
                                       static_cast<long double>(offset);
            if (!(result > -2147483649.0L && result < 2147483648.0L))
                return static_cast<std::int32_t>(INT32_MIN);
            return static_cast<std::int32_t>(result);
        };
        const auto x = applyOffset(monster->GetX(), dx);
        const auto y = applyOffset(monster->GetY(), dy);
        monster->Sync(x, y);
        monster->Goto(x, y);
        monster->CalculateMoveDuration();
        monster->SyncPacket();
    };
    tree->ForEachAround(pull);
}

void CHARACTER::AttractRanger()
{
    SECTREE* tree = GetSectree();
    if (!tree)
        return;

    auto attract = [this](LPENTITY entity) {
        if (!entity->IsType(0) || entity->GetDesc())
            return;
        auto* monster = static_cast<CHARACTER*>(entity);
        if (monster->m_bCharType != 0)
            return;
        CHARACTER* victim = monster->GetVictim();
        if (victim && victim != this)
            return;
        const WORD range = monster->GetMobAttackRange();
        if (range > 150)
            monster->AddAffect(225, POINT_BOW_DISTANCE, 150 - range, 0, 180, 0, false, false);
    };
    tree->ForEachAround(attract);
}

bool CHARACTER::Attack(LPCHARACTER victim, BYTE type)
{
    if (!victim || !CanMove())
        return false;
    const long map = GetMapIndex();
    if (map >= 181 && map <= 183 && !castle_can_attack(this, victim))
        return false;

    const DWORD time = get_dword_time();
    if (!GetDesc())
        MonsterChat(1);
    else if (IS_SPEED_HACK(this, victim, time))
        return false;

    victim->SetSyncOwner(this, true);
    if (victim->CanBeginFight())
        victim->BeginFight(this);

    if (test_server)
        sys_log(0, "[TEST_SERVER] Attack : type %d , MobBattleType %d", type,
                GetMobBattleType() ? GetMobAttackRange() : 0);
    int result = 0;
    if (type) {
        sys_log(1, "Attack call ComputeSkill %d %s", type, victim->GetName());
        result = ComputeSkill(type, victim, 0);
    } else {
        const BYTE battleType = GetMobBattleType();
        switch (battleType) {
            case 0:
            case 4:
            case 5:
            case 6:
            case 7:
                result = battle_melee_attack(this, victim);
                break;
            case 1:
            case 2:
                FlyTarget(static_cast<DWORD>(victim->GetVID()), victim->GetX(), victim->GetY(), 0x33);
                result = Shoot(battleType == 2);
                break;
            default:
                sys_err("Unhandled battle type %d", battleType);
                break;
        }
    }
    sys_log(2, "%s Attack %s", GetName(), victim->GetName());
    if (result != 1 && result != 3)
        return false;

    OnMove(true);
    victim->OnMove(false);
    if (result == 3 && GetDesc())
        SetVictim(nullptr);
    return true;
}

LPCHARACTER CHARACTER::GetProtege()
{
    if (m_pkChrStone)
        return m_pkChrStone;
    return m_pkParty ? m_pkParty->GetLeader() : nullptr;
}

namespace {
int SPBits(unsigned int bits);
}

LPCHARACTER CHARACTER::GetNearestVictim(LPCHARACTER origin)
{
    if (!origin)
        origin = this;
    LPCHARACTER nearest = nullptr;
    float nearestDistance = 99999.0f;
    for (const auto& entry : m_map_kDamage) {
        LPCHARACTER attacker = CHARACTER_MANAGER::instance().Find(entry.first);
        if (!attacker || attacker->IsAffectFlag(19) ||
            attacker->IsAffectFlag(2) || attacker->IsAffectFlag(28))
            continue;
        const int distance = DISTANCE_APPROX(
            SPBits(static_cast<unsigned>(attacker->GetX()) - static_cast<unsigned>(origin->GetX())),
            SPBits(static_cast<unsigned>(attacker->GetY()) - static_cast<unsigned>(origin->GetY())));
        if (static_cast<double>(distance) < nearestDistance) {
            nearest = attacker;
            nearestDistance = static_cast<float>(distance);
        }
    }
    return nearest;
}

bool CHARACTER::CanFight() const
{
    return m_pointsInstant.position >= POS_FIGHTING;
}

bool CHARACTER::CanBeginFight()
{
    return CanMove() && m_pointsInstant.position == POS_STANDING && !IsDead() && !IsStun();
}

// 081476d0
void CHARACTER::BeginFight(LPCHARACTER victim)
{
    SetVictim(victim);
    SetPosition(POS_FIGHTING);
    SetNextStatePulse(1);
}

// 08153e30
void CHARACTER::SetVictim(LPCHARACTER victim)
{
    if (victim) {
        if (m_kVIDVictim != victim->GetVID())
            MonsterLog("\260\370\260\335\040\264\353\273\363\300\273\040\274\263\301\244\040\073\040\045\163", victim->GetName());
        m_kVIDVictim = victim->GetVID();
        m_dwLastVictimSetTime = get_dword_time();
        return;
    }
    if (static_cast<DWORD>(m_kVIDVictim))
        MonsterLog("\260\370\260\335\040\264\353\273\363\300\273\040\307\330\301\246");
    m_kVIDVictim.Reset();
    battle_end(this);
}

// 0814d850
void CHARACTER::SetLastAttacked(DWORD time)
{
    assert(m_pkMobInst);
    m_pkMobInst->m_dwLastAttackedTime = time;
    m_pkMobInst->m_posLastAttacked = GetXYZ();
}

// 0814d8c0
void CHARACTER::SendDamagePacket(LPCHARACTER attacker, int amount, BYTE flag)
{
    DESC* desc = GetDesc();
    if (!desc && (!attacker->GetDesc() || attacker->m_pkChrTarget != this))
        return;
    TPacketGCDamageInfo packet;
    packet.header = 135;
    packet.dwVID = static_cast<DWORD>(GetVID());
    packet.flag = flag;
    packet.damage = amount;
    if (desc)
        desc->Packet(&packet, sizeof(packet));
    desc = attacker->GetDesc();
    if (desc)
        desc->Packet(&packet, sizeof(packet));
}

namespace {
int SPBits(unsigned int bits)
{
    int value;
    std::memcpy(&value,&bits,sizeof(value));
    return value;
}

int SPProduct(int left,int right)
{
    return SPBits(static_cast<unsigned int>(left)*static_cast<unsigned int>(right));
}
}

void CHARACTER::UpdateKillerMode()
{
    if (!IsKillerMode()) return;
    int seconds=LC_IsYMIR() ? 60 : 30;
    int pulse=thecore_pulse();
    if (SPProduct(seconds,passes_per_sec)<=SPBits(static_cast<unsigned int>(pulse)-m_iKillerModePulse))
        SetKillerMode(false);
}

void CHARACTER::DistributeSP(CHARACTER* recipient, int method)
{
    if (recipient->m_pointsInstant.iMaxSP<=recipient->m_points.sp) return;
    bool attacking=static_cast<unsigned int>(get_dword_time()-m_dwLastAttackTime)<3000;
    bool moving=static_cast<unsigned int>(get_dword_time()-m_dwLastMoveTime)<3000;
    int amount;
    if (method==1) {
        if (number_ex(0,3,"char_battle.cpp",0x66)!=0) return;
        int difference=m_points.level-recipient->m_points.level;
        if (difference< -3) return;
        amount=difference>=5 ? 10 : difference>=0 ? 6 : 2;
        amount=SPBits(static_cast<unsigned int>(amount)+SPProduct(recipient->GetPoint(33),amount)/100);
        CreateFly(amount<7 ? 4 : amount<11 ? 5 : 6,recipient);
        recipient->PointChange(7,amount,false,false);
        return;
    }

    int maximum=m_pointsInstant.iMaxSP;
    if (LC_IsJapan()) {
        if (!LC_IsJapan()) return;
        if (recipient->GetJob()==3)
            amount=attacking ? maximum/100 : moving ? SPProduct(maximum,15)/1000 : maximum/50;
        else amount=attacking ? maximum/200 : maximum/100;
    } else if (recipient->GetJob()==3 ||
               (recipient->GetJob()==2 && recipient->m_points.skill_group==2)) {
        amount=attacking ? maximum/100+2 : moving ? maximum/50+3 : SPProduct(maximum,3)/100+10;
    } else {
        maximum=recipient->m_pointsInstant.iMaxSP;
        if (attacking) amount=maximum/200+2;
        else if (moving || recipient->m_points.hp<recipient->m_pointsInstant.iMaxHP) amount=maximum/100+2;
        else amount=maximum/100+9;
    }
    amount=SPBits(static_cast<unsigned int>(amount)+SPProduct(recipient->GetPoint(33),amount)/100);
    recipient->PointChange(7,amount,false,false);
}

// 08148600: gold-roll and pile order from the original char_battle.cpp.
void CHARACTER::RewardGold(LPCHARACTER attacker)
{
    bool autoLoot = attacker->GetPremiumRemainSeconds(3) > 0 || attacker->IsEquipUniqueGroup(0x271b);
    if (LC_IsJapan())
        autoLoot = true;

    PIXEL_POSITION position = GetXYZ();
    if (!autoLoot && !SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), GetX(), GetY(), position))
        return;

    int chance = MobRankStats[GetMobRank()].iGoldPercent;
    const auto add = [](int left, int right) {
        return SPBits(static_cast<unsigned>(left) + static_cast<unsigned>(right));
    };
    CPrivManager& privileges = CPrivManager::instance();
    if (attacker->GetDesc())
        chance = SPProduct(add(privileges.GetPriv(attacker, 2), 100), chance) / 100;
    if (attacker->GetPoint(POINT_MALL_GOLDBONUS))
        chance = add(chance, SPProduct(attacker->GetPoint(POINT_MALL_GOLDBONUS), chance) / 100);

    CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
    chance = SPProduct(characters.GetMobGoldDropRate(attacker), chance) / 100;
    if (attacker->GetPremiumRemainSeconds(6) > 0 || attacker->IsEquipUniqueGroup(0x2728))
        chance = SPProduct(chance, 2);
    if (chance > 100)
        chance = 100;

    const int* deltaRates = GetMobRank() < 4 ? aiPercentByDeltaLev : aiPercentByDeltaLevForBoss;
    const int levelIndex = MINMAX(0, GetLevel() - attacker->GetLevel() + 15, 30);
    const int dropThreshold = SPProduct(chance, deltaRates[levelIndex]) / 100;
    char source[] = "char_battle.cpp";
    if (number_ex(1, 100, source, 0x212) > dropThreshold)
        return;

    int multiplier = distribution_test_server ? 3 : 1;
    if (number_ex(1, 50000, source, 0x217) == 1)
        multiplier *= 10;
    else if (number_ex(1, 10000, source, 0x219) == 1)
        multiplier *= 5;
    if (attacker->GetPoint(POINT_GOLD_DOUBLE_BONUS)) {
        const int roll = number_ex(1, 100, source, 0x21e);
        if (roll <= attacker->GetPoint(POINT_GOLD_DOUBLE_BONUS))
            multiplier *= 2;
    }

    if (test_server)
        attacker->ChatPacket(3, "gold_mul %d rate %d", multiplier, characters.GetMobGoldAmountRate(attacker));

    const int privilegeBase = add(privileges.GetPriv(attacker, 3), 100);
    if (!privilegeBase)
        return;
    const int jackpotRange = 10000 / privilegeBase;

    const auto rollGold = [&](int line) {
        const TMobTable& mob = GetMobTable();
        return number_ex(mob.dwGoldMin, mob.dwGoldMax, source, line);
    };
    const auto scaleGold = [&](int gold) {
        return SPProduct(SPProduct(characters.GetMobGoldAmountRate(attacker), gold) / 100, multiplier);
    };
    const auto scatter = [&](int xLine, int yLine) {
        const int dx = number_ex(-7, 7, source, xLine);
        const int dy = number_ex(-7, 7, source, yLine);
        position.x = GetX() + dx * 20;
        position.y = GetY() + dy * 20;
    };
    const auto putOnGround = [&](CItem* item) {
        item->AddToGround(GetMapIndex(), position);
        item->StartDestroyEvent();
    };

    int totalGold = 0;
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    if (GetMobRank() >= 4 && GetCharType() != CHAR_TYPE_STONE) {
        if (number_ex(1, jackpotRange, source, 0x232) == 1)
            multiplier *= 10;
        const int piles = number_ex(25, 35, source, 0x235);
        for (int i = 1; i <= piles; ++i) {
            const int rolledGold = rollGold(0x239);
            if (test_server)
                sys_log(0, "iGold %d", rolledGold / piles);
            int amount = scaleGold(rolledGold / piles);
            if (!amount)
                amount = 1;
            if (test_server) {
                sys_log(0, "Drop Moeny MobGoldAmountRate %d %d", characters.GetMobGoldAmountRate(attacker), multiplier);
                const int maximum = static_cast<int>(GetMobTable().dwGoldMax);
                sys_log(0, "Drop Money gold %d GoldMin %d GoldMax %d", static_cast<int>(amount), maximum, maximum);
            }
            if (CItem* item = items.CreateItem(1, static_cast<std::uint32_t>(amount), 0, false, -1)) {
                const int dx1 = number_ex(-14, 14, source, 0x24e);
                const int dx2 = number_ex(-14, 14, source, 0x24e);
                const int dy1 = number_ex(-14, 14, source, 0x24f);
                const int dy2 = number_ex(-14, 14, source, 0x24f);
                position.x = GetX() + (dx1 + dx2) * 23;
                position.y = GetY() + (dy1 + dy2) * 23;
                putOnGround(item);
                totalGold = add(totalGold, amount);
            }
        }
    } else if (number_ex(1, jackpotRange, source, 0x25d) == 1) {
        for (int i = 0; i < 10; ++i) {
            int amount = scaleGold(rollGold(0x264));
            if (!amount)
                amount = 1;
            if (CItem* item = items.CreateItem(1, static_cast<std::uint32_t>(amount), 0, false, -1)) {
                if (i != 0)
                    scatter(0x271, 0x272);
                putOnGround(item);
                totalGold = add(totalGold, amount);
            }
        }
    } else {
        totalGold = scaleGold(rollGold(0x284));
        int piles = 1;
        if (totalGold < 3 || LC_IsYMIR()) {
            if (GetMobRank() > 3) {
                piles = number_ex(3, 10, source, 0x28f);
                if (totalGold / piles == 0)
                    piles = 1;
            }
        } else {
            piles = number_ex(1, 3, source, 0x28c);
        }
        if (!totalGold)
            totalGold = 1;

        for (int i = 1; i <= piles; ++i) {
            if (autoLoot) {
                attacker->GiveGold(totalGold / piles);
            } else if (CItem* item = items.CreateItem(1, static_cast<std::uint32_t>(totalGold / piles), 0, false, -1)) {
                scatter(0x2a4, 0x2a5);
                putOnGround(item);
            }
        }
    }

    DBManager::instance().SendMoneyLog(1, GetRaceNum(), totalGold);
}

// 08152a20: experience modifiers are applied in original order.
static void GiveExp(LPCHARACTER from, LPCHARACTER to, int experience)
{
    // The original uses 32-bit integer arithmetic before each signed division.
    const auto add = [](int left, int right) {
        return SPBits(static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right));
    };
    const int level = to->GetLevel();
    const int levelIndex = MINMAX(0, from->GetLevel() - level + 15, 30);
    int base = SPProduct(experience, aiPercentByDeltaLev[levelIndex]) / 100;
    if (distribution_test_server)
        base = SPProduct(base, 3);

    experience = SPProduct(add(100, CPrivManager::instance().GetPriv(to, 4)), base) / 100;
    if (to->IsEquipUniqueItem(0x11174))
        experience = add(experience, experience / 5);
    const long map = to->GetMapIndex();
    if (map >= 660000 && map < 670000)
        experience = add(experience, experience / 5);

    char source[] = "char_battle.cpp";
    const long doubleBonus = to->GetPoint(POINT_EXP_DOUBLE_BONUS);
    if (doubleBonus && number_ex(1, 100, source, 0xc31) <= doubleBonus)
        experience = add(experience, SPProduct(experience, 30) / 100);
    if (to->IsEquipUniqueItem(0x11175))
        experience = add(experience, experience / 2);

    quest::CQuestManager& quests = quest::CQuestManager::instance();
    quests.GetEventFlag("ringofexp");
    const auto applyExpBoost = [&](int divisor) {
        if (to->GetPremiumRemainSeconds(0) > 0)
            experience = add(experience, experience / divisor);
        if (to->IsEquipUniqueGroup(10000))
            experience = add(experience, experience / divisor);
    };
    if (LC_IsHongKong() || LC_IsGermany() || LC_IsEurope() || LC_IsCanada())
        applyExpBoost(2);
    else if (LC_IsNewCIBN() || LC_IsBrazil())
        applyExpBoost(1);
    else
        applyExpBoost(5);

    const int pcBangBonus = to->GetPoint(POINT_PC_BANG_EXP_BONUS);
    if (pcBangBonus > 0 && to->IsInPCBang())
        experience = add(experience, SPProduct(pcBangBonus, experience) / 100);
    const int marriageBonus = to->GetMarriageBonus(0x1159e, true);
    experience = add(experience, SPProduct(marriageBonus, experience) / 100);
    const int rate = CHARACTER_MANAGER::instance().GetMobExpRate(to);
    experience = MIN(to->GetNextExp() / 10, SPProduct(rate, experience) / 100);

    if (test_server && quests.GetEventFlag("exp_bonus_log") && base > 0)
        to->ChatPacket(1, "exp bonus %d%%", SPProduct(add(experience, -base), 100) / base);
    to->PointChange(POINT_EXP, experience, true, false);
    from->CreateFly(1, to);

    LPCHARACTER partner = to->GetMarryPartner();
    const int marriageLevel = to->GetLevel();
    if (partner && marriageLevel > 0) {
        std::uint32_t points = SPProduct(experience, 2000) / marriageLevel / marriageLevel / 3;
        if (to->GetPremiumRemainSeconds(5) > 0 || partner->GetPremiumRemainSeconds(5) > 0)
            points *= 3;
        marriage::TMarriage* record = marriage::CManager::instance().Get(to->GetPlayerID());
        if (record && record->IsNear())
            record->Update(points);
    }
}

namespace NPartyExpDistribute {
struct FPartyTotaler {
    long x;
    long y;
    unsigned int totalWeight = 0;
    unsigned int members = 0;

    void operator()(LPCHARACTER member)
    {
        if (DISTANCE_APPROX(member->GetX() - x, member->GetY() - y) <= 5000) {
            totalWeight += LC_IsYMIR() ? member->GetLevel() : party_exp_distribute_table[member->GetLevel()];
            ++members;
        }
    }
};

struct FPartyDistributor {
    LPCHARACTER victim;
    long distributeX;
    long distributeY;
    int mode;
    unsigned int exp;
    unsigned int totalWeight;
    unsigned int members;

    void operator()(LPCHARACTER member)
    {
        if (DISTANCE_APPROX(member->GetX() - distributeX, member->GetY() - distributeY) > 5000)
            return;

        unsigned int share;
        switch (mode) {
            case PARTY_EXP_DISTRIBUTION_NON_PARITY:
                if (!totalWeight)
                    return;
                if (LC_IsYMIR()) {
                    share = static_cast<unsigned int>(member->GetLevel()) * exp / totalWeight;
                } else {
                    const DWORD weight = party_exp_distribute_table[member->GetLevel()];
                    share = static_cast<unsigned int>(static_cast<long double>(weight) * exp / totalWeight);
                }
                break;
            case PARTY_EXP_DISTRIBUTION_PARITY:
                share = exp / members;
                break;
            default:
                _sys_err("operator()", 0xd07, "Unknown party exp distribution mode %d", mode);
                return;
        }
        GiveExp(victim, member, static_cast<int>(share));
    }
};
}

int CHARACTER::GetRealAlignment()
{
    return m_iRealAlignment;
}

void CHARACTER::ShowAlignment(bool show) {
    if(show) {
        if(m_iAlignment==m_iRealAlignment) return;
        m_iAlignment=m_iRealAlignment;
    } else {
        if(!m_iAlignment) return;
        m_iAlignment=0;
    }
    UpdatePacket();
}

void CHARACTER::UpdateAlignment(int amount)
{
    const int shown=m_iAlignment;
    const int previous=m_iRealAlignment;
    m_iRealAlignment=MINMAX(-200000,SPBits(static_cast<unsigned int>(previous) + static_cast<unsigned int>(amount)),200000);
    if (shown==previous) {
        m_iAlignment=m_iRealAlignment;
        if (shown/10!=m_iAlignment/10) UpdatePacket();
    }
}

struct SDamageInfo {
    int damage;
    LPCHARACTER attacker;
    CParty* party;

    void Distribute(LPCHARACTER victim, int experience)
    {
        if (attacker) {
            GiveExp(victim, attacker, experience);
            return;
        }
        if (!party)
            return;

        const auto total = party->ForEachOnlineMember(
            NPartyExpDistribute::FPartyTotaler{victim->GetX(), victim->GetY()});

        if (party->IsPositionNearLeader(victim)) {
            const auto percent = 100U + static_cast<std::uint32_t>(party->GetExpBonusPercent());
            const auto scaled = static_cast<std::uint32_t>(experience) * percent;
            experience = static_cast<std::int32_t>(scaled) / 100;
        }
        if (test_server && quest::CQuestManager::instance().GetEventFlag("exp_bonus_log") &&
            party->GetExpBonusPercent())
            party->ChatPacketToAllMember(1, "exp party bonus %d%%", party->GetExpBonusPercent());

        if (LPCHARACTER center = party->GetExpCentralizeCharacter()) {
            if (DISTANCE_APPROX(victim->GetX() - center->GetX(), victim->GetY() - center->GetY()) <= 5000) {
                const int share = static_cast<int>(static_cast<long double>(experience) * 0.05f);
                experience -= share;
                GiveExp(victim, center, share);
            }
        }

        const int mode = party->GetExpDistributionMode();
        const long distributeX = victim->GetX();
        const long distributeY = victim->GetY();
        const unsigned int exp = static_cast<unsigned int>(experience);
        const unsigned int members = total.members ? total.members : 1;
        party->ForEachOnlineMember(NPartyExpDistribute::FPartyDistributor{
            victim, distributeX, distributeY, mode, exp, total.totalWeight, members});
    }
};

// 08152a10: the original HP-distribution leaf is empty.
void CHARACTER::DistributeHP(LPCHARACTER)
{
}

// 081530e0: resolve the damage ledger before distributing its experience.
LPCHARACTER CHARACTER::DistributeExp()
{
    int experience = static_cast<int>(m_points.exp);
    if (experience <= 0)
        return 0;

    int totalDamage = 0;
    int highestDamage = 0;
    LPCHARACTER highestAttacker = 0;
    std::vector<SDamageInfo> shares;
    std::map<CParty*, SDamageInfo> partyDamage;

    for (const auto& entry : m_map_kDamage) {
        LPCHARACTER attacker = CHARACTER_MANAGER::instance().Find(entry.first);
        if (!attacker || attacker->GetCharType() != CHAR_TYPE_PC ||
            DISTANCE_APPROX(
                SPBits(static_cast<unsigned>(GetX()) - static_cast<unsigned>(attacker->GetX())),
                SPBits(static_cast<unsigned>(GetY()) - static_cast<unsigned>(attacker->GetY()))) > 5000)
            continue;

        const int damage = entry.second.iTotalDamage;
        totalDamage = SPBits(static_cast<unsigned>(totalDamage) + static_cast<unsigned>(damage));
        if (!highestAttacker || highestDamage < damage) {
            highestAttacker = attacker;
            highestDamage = damage;
        }
        if (CParty* party = attacker->GetParty()) {
            const SDamageInfo first = {damage, 0, party};
            const auto inserted = partyDamage.insert(std::make_pair(party, first));
            if (!inserted.second) {
                int& partyTotal = inserted.first->second.damage;
                partyTotal = SPBits(static_cast<unsigned>(partyTotal) + static_cast<unsigned>(damage));
            }
        } else {
            const SDamageInfo solo = {damage, attacker, 0};
            shares.push_back(solo);
        }
    }

    for (const auto& entry : partyDamage)
        shares.push_back(entry.second);
    m_points.exp = 0;
    if (!totalDamage)
        return 0;

    if (m_pkChrStone) {
        const int transfer = experience >> 1;
        m_pkChrStone->m_points.exp += transfer;
        experience -= transfer;
    }
    sys_log(1, "%s total exp: %d, damage_info_table.size() == %d, TotalDam %d", GetName(),
            experience, static_cast<int>(shares.size()), totalDamage);
    if (shares.empty())
        return 0;

    DistributeHP(highestAttacker);
    auto best = shares.begin();
    for (auto share = shares.begin(); share != shares.end(); ++share) {
        if (best->damage < share->damage)
            best = share;
    }

    const auto clampRatio = [](const SDamageInfo& share, long double ratio, int line) {
        if (ratio > 1.0L) {
            _sys_err("DistributeExp", line, "DistributeExp percent over 1.0 (fPercent %f name %s )",
                     static_cast<double>(static_cast<float>(ratio)),
                     share.attacker ? share.attacker->GetName() : "");
            return 1.0L;
        }
        return ratio;
    };

    const int bonus = experience / 5;
    const int remainder = experience - bonus;
    // The original rounds the best ratio before multiplication and truncates the wide product.
    const float bestRatio = static_cast<float>(
        clampRatio(*best, static_cast<long double>(best->damage) / totalDamage, 0xdbb));
    best->Distribute(this, bonus + static_cast<int>(static_cast<long double>(remainder) * bestRatio));
    if (bestRatio == 1.0f)
        return highestAttacker;

    best->attacker = 0;
    best->party = 0;
    // The remaining shares use single-precision operands for the totals and a wide quotient.
    const float remainingExp = static_cast<float>(remainder);
    const float damageTotal = static_cast<float>(totalDamage);
    for (auto& share : shares) {
        const long double ratio = clampRatio(share, static_cast<long double>(share.damage) / damageTotal, 0xdd8);
        share.Distribute(this, static_cast<int>(remainingExp * ratio));
    }
    return highestAttacker;
}

// 0x0814d970
bool CHARACTER::Damage(CHARACTER* attacker, int damage, EDamageType type)
{
    char source[] = "char_battle.cpp";
    if (GetRaceNum() == 5001 && GetMaxHP()) {
        const int hpPercent = SPProduct(GetHP(), 100) / GetMaxHP();
        const int stage = hpPercent <= 10 ? 5 : hpPercent <= 20 ? 4 :
                          hpPercent <= 40 ? 3 : hpPercent <= 60 ? 2 : hpPercent <= 80 ? 1 : 0;
        if (stage && stage > m_pointsInstant.iMaxSP) {
            m_pointsInstant.iMaxSP = stage;
            const int piles = number_ex(10, 13, source, 0x7aa);
            sys_log(0, "WAEGU DropGoldOnHit %d times", stage);
            ITEM_MANAGER& items = ITEM_MANAGER::instance();
            for (int index = 0; index < piles; ++index) {
                if (CItem* gold = items.CreateItem(1, 1000 / piles, 0, false, -1)) {
                    const int x1 = number_ex(-14, 14, source, 0x7b7);
                    const int x2 = number_ex(-14, 14, source, 0x7b7);
                    const int y1 = number_ex(-14, 14, source, 0x7b8);
                    const int y2 = number_ex(-14, 14, source, 0x7b8);
                    PIXEL_POSITION pos = GetXYZ();
                    pos.x = SPBits(static_cast<unsigned>(pos.x) + (x1 + x2) * 20);
                    pos.y = SPBits(static_cast<unsigned>(pos.y) + (y1 + y2) * 20);
                    gold->AddToGround(GetMapIndex(), pos);
                    gold->StartDestroyEvent();
                }
            }
        }
    }

    const int currentHP = GetHP();
    const int currentSP = GetSP();

    const auto add = [](int left, int right) {
        return SPBits(static_cast<unsigned>(left) + static_cast<unsigned>(right));
    };
    const auto subtract = [](int left, int right) {
        return SPBits(static_cast<unsigned>(left) - static_cast<unsigned>(right));
    };
    const auto percent = [](int value, long rate) {
        return SPProduct(value, static_cast<int>(rate)) / 100;
    };
    const auto rollPercent = [&](long chance, int line) {
        return chance && number_ex(1, 100, source, line) <= chance;
    };

    bool critical = false;
    bool penetrating = false;
    const auto criticalAndPenetrate = [&](bool skill) {
        const auto roll = [&](BYTE point, DWORD marriageItem, int line) {
            int value = attacker->GetPoint(point);
            if (!GetDesc())
                value = add(value, attacker->GetMarriageBonus(marriageItem, true));
            const int chance = !skill ? value : value < 10 ? value / 2 : (value - 10) / 4 + 5;
            return value && number_ex(1, 100, source, line) <= chance;
        };

        if (roll(POINT_CRITICAL_PCT, 0x1159f, skill ? 0x7f1 : 0x85a)) {
            damage = SPProduct(damage, 2);
            critical = true;
            EffectPacket(5);
            if (skill && IsAffectFlag(26))
                RemoveAffect(26);
        }
        if (roll(POINT_PENETRATE_PCT, 0x1159d, skill ? 0x80a : 0x868)) {
            penetrating = true;
            const auto penetrationDefense = [&] {
                const int grade = GetPoint(POINT_DEF_GRADE);
                const int bonus = GetPoint(POINT_DEF_BONUS);
                return percent(grade, add(bonus, 100));
            };
            if (test_server) {
                const int defense = penetrationDefense();
                ChatPacket(1, LC_TEXT("\260\374\305\353 \303\337\260\241 \265\245\271\314\301\366 %d"), defense);
            }
            damage = add(damage, penetrationDefense());
            if (skill && IsAffectFlag(26))
                RemoveAffect(26);
        }
    };

    if (attacker && (type == DAMAGE_TYPE_MELEE || type == DAMAGE_TYPE_RANGE || type == DAMAGE_TYPE_MAGIC))
        criticalAndPenetrate(true);

    const bool normal = type == DAMAGE_TYPE_NORMAL || type == DAMAGE_TYPE_NORMAL_RANGE;
    if (normal) {
        const bool melee = type == DAMAGE_TYPE_NORMAL;
        const long avoidance = GetPoint(melee ? POINT_BLOCK : POINT_DODGE);
        if (rollPercent(avoidance, melee ? 0x825 : 0x832)) {
            if (attacker) {
                if (test_server) {
                    const char* message = melee ? LC_TEXT("%s \272\355\267\260! (%ld%%)") :
                                                  LC_TEXT("%s \310\270\307\307! (%ld%%)");
                    attacker->ChatPacket(1, message, GetName(), avoidance);
                    ChatPacket(1, message, GetName(), avoidance);
                }
                SendDamagePacket(attacker, 0, melee ? 8 : 4);
            }
            return false;
        }

        if (IsAffectFlag(15))
            damage = percent(damage, 100 + GetSkillPower(3) / 4);
        if (IsAffectFlag(21))
            damage = percent(damage, 95 - GetSkillPower(64) / 5);
        if (IsAffectFlag(23))
            damage = percent(damage, 100 - GetPoint(POINT_RESIST_NORMAL_DAMAGE));

        if (attacker) {
            if (melee) {
                if (const long reflect = GetPoint(POINT_REFLECT_MELEE))
                    attacker->Damage(this, percent(damage, reflect), DAMAGE_TYPE_SPECIAL);
            }
            criticalAndPenetrate(false);

            if (attacker->GetPoint(POINT_STEAL_HP)) {
                const int chance = LC_IsJapan() ? 10 : 1;
                if (number_ex(1, 10, source, 0x877) <= chance) {
                    CreateFly(15, attacker);
                    const int hp = add(percent(std::min(damage, currentHP),
                                               attacker->GetPoint(POINT_STEAL_HP)), 1);
                    attacker->PointChange(POINT_HP, hp, false, false);
                    PointChange(POINT_HP, subtract(0, hp), false, false);
                }
            }
            if (attacker->GetPoint(POINT_STEAL_SP)) {
                const int chance = LC_IsJapan() ? 10 : 1;
                if (number_ex(1, 10, source, 0x888) <= chance) {
                    CreateFly(4, attacker);
                    const bool victimHasDescriptor = GetDesc() != nullptr;
                    const int sp = add(percent(std::min(damage, victimHasDescriptor ? currentSP : currentHP),
                                               attacker->GetPoint(POINT_STEAL_SP)), 1);
                    if (victimHasDescriptor)
                        PointChange(POINT_SP, subtract(0, sp), false, false);
                    attacker->PointChange(POINT_SP, sp, false, false);
                }
            }
            if (rollPercent(attacker->GetPoint(POINT_STEAL_GOLD), 0x89d)) {
                const int gold = number_ex(1, GetLevel(), source, 0x89f);
                attacker->PointChange(POINT_GOLD, gold, false, false);
                DBManager::instance().SendMoneyLog(6, 1, gold);
            }
            if (attacker->GetPoint(POINT_HIT_HP_RECOVERY)) {
                if (number_ex(0, 4, source, 0x8a6)) {
                    if (const int hp = percent(std::min(damage, currentHP),
                                               attacker->GetPoint(POINT_HIT_HP_RECOVERY))) {
                        CreateFly(15, attacker);
                        attacker->PointChange(POINT_HP, hp, false, false);
                    }
                }
            }
            if (attacker->GetPoint(POINT_HIT_SP_RECOVERY)) {
                if (number_ex(0, 4, source, 0x8c5)) {
                    if (const int sp = percent(std::min(damage, currentHP),
                                               attacker->GetPoint(POINT_HIT_SP_RECOVERY))) {
                        CreateFly(4, attacker);
                        attacker->PointChange(POINT_SP, sp, false, false);
                    }
                }
            }
            if (rollPercent(attacker->GetPoint(POINT_MANA_BURN_PCT), 0x8e6))
                PointChange(POINT_SP, -50, false, false);
        }
    } else if (IsAffectFlag(21)) {
        const int chance = GetSkillPower(64) / 400;
        if (number_ex(1, 100, source, 0x8f2) <= chance)
            return false;
    }

    if (type >= DAMAGE_TYPE_NORMAL && type <= DAMAGE_TYPE_MAGIC) {
        if (attacker) {
            if (const long bonus = attacker->GetPoint(normal ? POINT_NORMAL_HIT_DAMAGE_BONUS : POINT_SKILL_DAMAGE_BONUS))
                damage = percent(damage, 100 + bonus);
        }
        const long defense = GetPoint(normal ? POINT_NORMAL_HIT_DEFEND_BONUS : POINT_SKILL_DEFEND_BONUS);
        damage = percent(damage, 100 - std::min(99L, defense));
    }

    if (IsAffectFlag(26)) {
        const int shield = GetPoint(POINT_MANASHIELD);
        const int cost = percent(damage / 3, shield);
        const int sp = GetSP();
        if (sp < cost && shield) {
            PointChange(POINT_SP, subtract(0, sp), false, false);
            damage = subtract(damage, SPProduct(sp, 100) / shield);
        } else {
            PointChange(POINT_SP, subtract(0, cost), false, false);
            damage = subtract(damage, damage / 3);
        }
    }
    const long mallDefense = GetPoint(POINT_MALL_DEFBONUS);
    if (mallDefense > 0) {
        const int reduction = std::min(200, percent(damage, mallDefense));
        if (test_server)
            ChatPacket(3, "\270\364\276\306\300\314\305\333 \305\270\260\335\304\241\260\250\274\322: %d", reduction);
        damage = subtract(damage, reduction);
    }

    if (attacker) {
        if (attacker->GetPoint(POINT_MALL_ATTBONUS) > 0) {
            const int bonus = std::min(300, percent(damage, attacker->GetLimitPoint(POINT_MALL_ATTBONUS)));
            if (test_server)
                attacker->ChatPacket(3, "\270\364\276\306\300\314\305\333 \305\270\260\335\304\241\301\365\260\241: %d", bonus);
            damage = add(damage, bonus);
        }
        if (!attacker->GetDesc()) {
            if (attacker->IsGuardNPC()) {
                m_pointsInstant.instant_flag |= 0x10;
                Stun();
                return true;
            }
        } else {
            const long map = attacker->GetMapIndex();
            const BYTE empire = attacker->GetEmpire();
            const BYTE mapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(map);
            if (empire && mapEmpire && empire != mapEmpire && !attacker->GetGMLevel())
                damage = SPProduct(damage, map >= 184 && map <= 189 ? 7 : 9) / 10;

            if (!GetDesc() && GetMonsterDrainSPPoint()) {
                const int drain = SPBits(static_cast<unsigned>(GetMonsterDrainSPPoint()));
                const int amount = std::min(drain, attacker->GetSP());
                attacker->PointChange(POINT_SP, subtract(0, amount), false, false);
            }
        }
    }

    if (!GetSectree() || GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
        return false;
    if (!GetDesc()) {
        CParty* party = GetParty();
        CHARACTER* leader = party ? party->GetLeader() : nullptr;
        (leader ? leader : this)->SetLastAttacked(get_dword_time());
        MonsterChat(3);
    }
    if (IsStun()) {
        Dead(attacker, false);
        return true;
    }
    if (IsDead())
        return true;

    if (type == DAMAGE_TYPE_POISON && GetHP() <= damage)
        damage = subtract(GetHP(), 1);
    if (LC_IsGermany() && attacker && attacker->GetDesc())
        damage = percent(damage, CHARACTER_MANAGER::instance().GetUserDamageRate(attacker));

    if (GetCharType() == CHAR_TYPE_MONSTER && IsStoneSkinner() && GetMaxHP() &&
        GetHPPct() < GetMobTable().bStoneSkinPoint)
        damage = subtract(damage, damage / 2);

    if (attacker) {
        if (attacker->GetCharType() == CHAR_TYPE_MONSTER && attacker->IsDeathBlower() &&
            attacker->IsDeathBlow() && number_ex(1, 4, source, 0x9cd) == GetJob())
            damage = SPProduct(damage, 4);
        BYTE flags = type == DAMAGE_TYPE_POISON ? 2 : 1;
        if (critical)
            flags |= 0x20;
        if (penetrating)
            flags |= 0x10;
        SendDamagePacket(attacker, damage, flags);
        if (test_server) {
            const int hpPercent = GetMaxHP() ? SPProduct(GetHP(), 100) / GetMaxHP() : 0;
            attacker->ChatPacket(1, "-> %s, DAM %d HP %d(%d%%)", GetName(), damage, GetHP(), hpPercent);
            ChatPacket(3, "<- %s, DAM %d HP %d(%d%%)", attacker->GetName(), damage, GetHP(), hpPercent);
        }
        if (m_bDetailLog) {
            ChatPacket(1, LC_TEXT("%s[%ld]\260\241 \260\370\260\335 \300\247\304\241: %d %d"),
                       attacker->GetName(), static_cast<long>(SPBits(static_cast<DWORD>(attacker->GetVID()))),
                       static_cast<int>(attacker->GetX()), static_cast<int>(attacker->GetY()));
        }
    }

    PointChange(POINT_HP, subtract(0, damage), false, false);
    if (attacker && damage > 0 && GetCharType() != CHAR_TYPE_PC) {
        TBattleInfo& info = m_map_kDamage[attacker->GetVID()];
        info.iTotalDamage = add(info.iTotalDamage, damage);
        StartRecoveryEvent();
        UpdateAggrPointEx(attacker, type, damage, &info);
    }
    if (GetHP() < 1) {
        Stun();
        m_dwKillerPID = attacker && attacker->GetCharType() == CHAR_TYPE_PC ? attacker->GetPlayerID() : 0;
    }
    return false;
}

// 0814f710. This dormant profiler retains the older combat rules.
bool CHARACTER::__Profile__Damage(LPCHARACTER attacker, int damage, EDamageType type)
{
    char source[] = "char_battle.cpp";
    const auto add = [](int left, int right) {
        return SPBits(static_cast<unsigned>(left) + static_cast<unsigned>(right));
    };
    const auto subtract = [](int left, int right) {
        return SPBits(static_cast<unsigned>(left) - static_cast<unsigned>(right));
    };
    const auto percent = [](int value, long rate) {
        return SPProduct(value, static_cast<int>(rate)) / 100;
    };
    const auto rollPercent = [&](long chance, int line) {
        return chance && number_ex(1, 100, source, line) <= chance;
    };

    if (GetRaceNum() == 5001 && GetMaxHP()) {
        const int hpPercent = SPProduct(GetHP(), 100) / GetMaxHP();
        const int stage = hpPercent <= 10 ? 5 : hpPercent <= 20 ? 4 :
                          hpPercent <= 40 ? 3 : hpPercent <= 60 ? 2 : hpPercent <= 80 ? 1 : 0;
        if (stage && stage > m_pointsInstant.iMaxSP) {
            m_pointsInstant.iMaxSP = stage;
            const int piles = number_ex(10, 13, source, 0xa50);
            sys_log(0, "WAEGU DropGoldOnHit %d times", stage);
            ITEM_MANAGER& items = ITEM_MANAGER::instance();
            for (int index = 0; index < piles; ++index) {
                if (CItem* gold = items.CreateItem(1, 1000 / piles, 0, false, -1)) {
                    const int x1 = number_ex(-14, 14, source, 0xa5d);
                    const int x2 = number_ex(-14, 14, source, 0xa5d);
                    const int y1 = number_ex(-14, 14, source, 0xa5e);
                    const int y2 = number_ex(-14, 14, source, 0xa5e);
                    PIXEL_POSITION position = GetXYZ();
                    position.x = add(position.x, (x1 + x2) * 20);
                    position.y = add(position.y, (y1 + y2) * 20);
                    gold->AddToGround(GetMapIndex(), position);
                    gold->StartDestroyEvent();
                }
            }
        }
    }

    struct ProfileScope {
        const char* name;
        explicit ProfileScope(const char* label) : name(label)
        {
            CProfiler::instance().PushAccum(name);
        }
        ~ProfileScope()
        {
            CProfiler::instance().PopAccum(name);
        }
    };

    const int currentHP = GetHP();
    const int currentSP = GetSP();
    {
        const ProfileScope profile("Attr");
        const auto criticalAndPenetrate = [&](bool skill) {
            const auto roll = [&](BYTE point, int line) {
                const int value = attacker->GetPoint(point);
                const int chance = !skill ? value : value < 10 ? value / 2 : (value - 10) / 4 + 5;
                return value && number_ex(1, 100, source, line) <= chance;
            };
            if (roll(POINT_CRITICAL_PCT, skill ? 0xa81 : 0xad2)) {
                damage = SPProduct(damage, 2);
                EffectPacket(5);
            }
            if (roll(POINT_PENETRATE_PCT, skill ? 0xa91 : 0xadc)) {
                const int defense = percent(GetPoint(POINT_DEF_GRADE), add(GetPoint(POINT_DEF_BONUS), 100));
                if (test_server)
                    ChatPacket(1, LC_TEXT("\260\374\305\353 \303\337\260\241 \265\245\271\314\301\366 %d"), defense);
                damage = add(damage, defense);
            }
        };

        if (attacker && (type == DAMAGE_TYPE_RANGE || type == DAMAGE_TYPE_MAGIC))
            criticalAndPenetrate(true);

        if (type == DAMAGE_TYPE_NORMAL || type == DAMAGE_TYPE_NORMAL_RANGE) {
            const bool melee = type == DAMAGE_TYPE_NORMAL;
            const long avoidance = GetPoint(melee ? POINT_BLOCK : POINT_DODGE);
            if (rollPercent(avoidance, melee ? 0xaa4 : 0xaaf)) {
                if (test_server && attacker) {
                    const char* message = melee ? LC_TEXT("%s \272\355\267\260! (%ld%%)") :
                                                  LC_TEXT("%s \310\270\307\307! (%ld%%)");
                    attacker->ChatPacket(1, message, GetName(), avoidance);
                    ChatPacket(1, message, GetName(), avoidance);
                }
                return false;
            }

            if (IsAffectFlag(15))
                damage = percent(damage, 100 + percent(GetSkillPower(3), 80));
            if (IsAffectFlag(21))
                damage = percent(damage, 95 - GetSkillPower(64) / 5);
            if (IsAffectFlag(23))
                damage = percent(damage, 100 - GetPoint(POINT_RESIST_NORMAL_DAMAGE));

            if (attacker) {
                if (melee) {
                    if (const long reflect = GetPoint(POINT_REFLECT_MELEE))
                        attacker->Damage(this, percent(damage, reflect), DAMAGE_TYPE_SPECIAL);
                }
                criticalAndPenetrate(false);

                if (const long rate = attacker->GetPoint(POINT_STEAL_HP)) {
                    if (number_ex(1, 10, source, 0xae7) == 1) {
                        CreateFly(15, attacker);
                        const int hp = add(percent(std::min(damage, currentHP), rate), 1);
                        attacker->PointChange(POINT_HP, hp, false, false);
                        PointChange(POINT_HP, subtract(0, hp), false, false);
                    }
                }
                if (const long rate = attacker->GetPoint(POINT_STEAL_SP)) {
                    if (number_ex(1, 10, source, 0xaf4) == 1) {
                        CreateFly(4, attacker);
                        const bool victimHasDescriptor = GetDesc() != nullptr;
                        const int sp = add(percent(std::min(damage, victimHasDescriptor ? currentSP : currentHP), rate), 1);
                        if (victimHasDescriptor)
                            PointChange(POINT_SP, subtract(0, sp), false, false);
                        attacker->PointChange(POINT_SP, sp, false, false);
                    }
                }
                if (rollPercent(attacker->GetPoint(POINT_STEAL_GOLD), 0xb09)) {
                    const int gold = number_ex(1, GetLevel(), source, 0xb0b);
                    attacker->PointChange(POINT_GOLD, gold, false, false);
                    DBManager::instance().SendMoneyLog(6, 1, gold);
                }
                if (const long rate = attacker->GetPoint(POINT_HIT_HP_RECOVERY)) {
                    if (number_ex(0, 4, source, 0xb12)) {
                        if (const int hp = percent(std::min(damage, currentHP), rate)) {
                            CreateFly(15, attacker);
                            attacker->PointChange(POINT_HP, hp, false, false);
                        }
                    }
                }
                if (const long rate = attacker->GetPoint(POINT_HIT_SP_RECOVERY)) {
                    if (number_ex(0, 4, source, 0xb31)) {
                        if (const int sp = percent(std::min(damage, currentHP), rate)) {
                            CreateFly(4, attacker);
                            attacker->PointChange(POINT_SP, sp, false, false);
                        }
                    }
                }
                if (rollPercent(attacker->GetPoint(POINT_MANA_BURN_PCT), 0xb52))
                    PointChange(POINT_SP, -50, false, false);
            }
        }

        if (IsAffectFlag(26)) {
            const int shield = GetPoint(POINT_MANASHIELD);
            const int cost = percent(damage / 2, shield);
            const int sp = GetSP();
            if (sp < cost && shield) {
                PointChange(POINT_SP, subtract(0, sp), false, false);
                damage = subtract(damage, SPProduct(sp, 100) / shield);
            } else {
                PointChange(POINT_SP, subtract(0, cost), false, false);
                damage = subtract(damage, damage / 2);
            }
        }
        if (const long mallDefense = GetPoint(POINT_MALL_DEFBONUS); mallDefense > 0) {
            const int reduction = std::min(200, percent(damage, mallDefense));
            if (test_server)
                ChatPacket(3, "\270\364\276\306\300\314\305\333 \305\270\260\335\304\241\260\250\274\322: %d", reduction);
            damage = subtract(damage, reduction);
        }

        if (attacker) {
            if (attacker->GetPoint(POINT_MALL_ATTBONUS) > 0) {
                const int bonus = std::min(300, percent(damage, attacker->GetLimitPoint(POINT_MALL_ATTBONUS)));
                if (test_server)
                    attacker->ChatPacket(3, "\270\364\276\306\300\314\305\333 \305\270\260\335\304\241\301\365\260\241: %d", bonus);
                damage = add(damage, bonus);
            }
            if (attacker->GetDesc()) {
                const BYTE empire = attacker->GetEmpire();
                const BYTE mapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(attacker->GetMapIndex());
                if (empire && mapEmpire && empire != mapEmpire && !attacker->GetGMLevel())
                    damage = SPProduct(damage, 9) / 10;
                if (!GetDesc() && GetMonsterDrainSPPoint()) {
                    const int drain = SPBits(static_cast<unsigned>(GetMonsterDrainSPPoint()));
                    attacker->PointChange(POINT_SP, subtract(0, std::min(drain, attacker->GetSP())), false, false);
                }
            } else if (attacker->IsGuardNPC()) {
                m_pointsInstant.instant_flag |= 0x10;
                Stun();
                return true;
            }
        }
    }

    {
        const ProfileScope profile("mapAttr");
        if (!GetSectree() || GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
            return false;
    }

    const ProfileScope profile("Rest");
    if (!GetDesc()) {
        CParty* party = GetParty();
        CHARACTER* leader = party ? party->GetLeader() : nullptr;
        (leader ? leader : this)->SetLastAttacked(get_dword_time());
        MonsterChat(3);
    }
    if (IsStun()) {
        Dead(attacker, false);
        return true;
    }
    if (IsDead())
        return true;

    if (attacker) {
        if (test_server) {
            const int hpPercent = GetMaxHP() ? SPProduct(GetHP(), 100) / GetMaxHP() : 0;
            attacker->ChatPacket(1, "-> %s, DAM %d HP %d(%d%%)", GetName(), damage, GetHP(), hpPercent);
            ChatPacket(3, "<- %s, DAM %d HP %d(%d%%)", attacker->GetName(), damage, GetHP(), hpPercent);
        }
        if (m_bDetailLog) {
            ChatPacket(1, LC_TEXT("%s[%ld]\260\241 \260\370\260\335 \300\247\304\241: %d %d"),
                       attacker->GetName(), static_cast<long>(SPBits(static_cast<DWORD>(attacker->GetVID()))),
                       static_cast<int>(attacker->GetX()), static_cast<int>(attacker->GetY()));
        }
    }

    PointChange(POINT_HP, subtract(0, damage), false, false);
    if (attacker && damage > 0 && GetCharType() != CHAR_TYPE_PC) {
        TBattleInfo& info = m_map_kDamage[attacker->GetVID()];
        info.iTotalDamage = add(info.iTotalDamage, damage);
        StartRecoveryEvent();
        UpdateAggrPointEx(attacker, type, damage, &info);
    }
    if (GetHP() < 1) {
        Stun();
        m_dwKillerPID = attacker && attacker->GetCharType() == CHAR_TYPE_PC ? attacker->GetPlayerID() : 0;
    }
    return false;
}

void CHARACTER::Reward(bool itemDrop)
{
    char source[] = "char_battle.cpp";
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
    SECTREE_MANAGER& sectors = SECTREE_MANAGER::instance();
    PIXEL_POSITION position = GetXYZ();
    const auto scatter = [&](int xLine, int yLine) {
        position.x = GetX() + number_ex(-7, 7, source, xLine) * 20;
        position.y = GetY() + number_ex(-7, 7, source, yLine) * 20;
    };

    if (GetRaceNum() == 5001) {
        if (!sectors.GetMovablePosition(GetMapIndex(), GetX(), GetY(), position))
            return;
        const TMobTable& proto = GetMobTable();
        int gold = number_ex(proto.dwGoldMin, proto.dwGoldMax, source, 0x2be);
        gold = SPProduct(gold, characters.GetMobGoldAmountRate(0)) / 100;
        if (distribution_test_server)
            gold = SPProduct(gold, 3);
        const int piles = number_ex(25, 35, source, 0x2c1);
        sys_log(0, "WAEGU Dead gold %d split %d", gold, piles);
        for (int i = 0; i < piles; ++i) {
            if (CItem* item = items.CreateItem(1, gold / piles, 0, false, -1)) {
                scatter(0x2cb, 0x2cc);
                item->AddToGround(GetMapIndex(), position);
                item->StartDestroyEvent();
            }
        }
        return;
    }

    CHARACTER* attacker = DistributeExp();
    if (!attacker)
        return;
    if (attacker->GetDesc()) {
        if (GetLevel() - attacker->GetLevel() >= -10) {
            const int alignment = attacker->GetRealAlignment() < 0
                ? (attacker->IsEquipUniqueItem(0x111a3) ? 14 : 7) : 2;
            attacker->UpdateAlignment(alignment);
        }
        attacker->SetQuestNPCID(GetVID());
        quest::CQuestManager::instance().Kill(attacker->GetPlayerID(), GetRaceNum());
        characters.KillLog(GetRaceNum());
        if (number_ex(0, 9, source, 0x2f1) == 0) {
            if (attacker->GetPoint(87)) {
                const int maximum = attacker->m_pointsInstant.iMaxHP;
                const int recovery = attacker->GetPoint(87);
                attacker->PointChange(5, SPProduct(maximum, recovery) / 100, false, false);
                CreateFly(15, attacker);
            }
            if (attacker->GetPoint(82)) {
                const int maximum = attacker->m_pointsInstant.iMaxSP;
                const int recovery = attacker->GetPoint(82);
                attacker->PointChange(7, SPProduct(maximum, recovery) / 100, false, false);
                CreateFly(4, attacker);
            }
        }
    }
    position = GetXYZ();
    if (!itemDrop || !sectors.GetMovablePosition(GetMapIndex(), position.x, position.y, position))
        return;

    if (test_server)
        sys_log(0, "Drop money : Attacker %s", attacker->GetName());
    RewardGold(attacker);

    std::vector<LPITEM> drops;
    if (items.CreateDropItem(this, attacker, drops)) {
        if (drops.size() == 1) {
            LPITEM item = drops.front();
            item->AddToGround(GetMapIndex(), position);
            item->SetOwnership(attacker, 10);
            item->StartDestroyEvent();
            scatter(0x328, 0x329);
            sys_log(0, "DROP_ITEM: %s %d %d from %s", item->GetName(),
                    static_cast<int>(position.x), static_cast<int>(position.y), GetName());
        } else if (!drops.empty()) {
            std::priority_queue<std::pair<int, LPCHARACTER> > contributors;
            int totalDamage = 0;
            for (const auto& entry : m_map_kDamage) {
                const int damage = entry.second.iTotalDamage;
                if (damage > 0) {
                    if (CHARACTER* character = characters.Find(entry.first)) {
                        contributors.push(std::make_pair(damage, character));
                        totalDamage = SPBits(static_cast<unsigned>(totalDamage) + static_cast<unsigned>(damage));
                    }
                }
            }
            std::vector<LPCHARACTER> owners;
            while (!contributors.empty() && SPProduct(contributors.top().first, 10) >= totalDamage) {
                owners.push_back(contributors.top().second);
                contributors.pop();
            }

            std::size_t ownerIndex = 0;
            for (std::size_t i = drops.size(); i > 0;) {
                LPITEM item = drops[--i];
                if (!item) {
                    _sys_err("Reward", owners.empty() ? 0x357 : 0x373, "item null in vector idx %d", static_cast<int>(i));
                    continue;
                }
                item->AddToGround(GetMapIndex(), position);
                if (!owners.empty()) {
                    CHARACTER* owner = owners[ownerIndex];
                    ownerIndex = (ownerIndex + 1) % owners.size();
                    if (CParty* party = owner->GetParty())
                        owner = party->GetNextOwnership(owner, GetX(), GetY());
                    item->SetOwnership(owner, 10);
                }
                item->StartDestroyEvent();
                if (owners.empty())
                    scatter(0x360, 0x361);
                else
                    scatter(0x386, 0x387);
                sys_log(0, "DROP_ITEM: %s %d %d by %s", item->GetName(),
                        static_cast<int>(position.x), static_cast<int>(position.y), GetName());
            }
        }
    }
    m_map_kDamage.clear();
}

// Original dead_event at 08148280. Event info owns a raw character pointer.
struct CharacterDeathEventInfo
{
    LPCHARACTER character;
};

EVENTFUNC(dead_event)
{
    LPCHARACTER character = static_cast<CharacterDeathEventInfo*>(event->info)->character;
    sys_log(1, "DEAD_EVENT: %s %p %p", character->GetName(),
            static_cast<void*>(character), static_cast<void*>(character->m_pkDeadEvent));
    character->m_pkDeadEvent = 0;

    if (LPDESC descriptor = character->GetDesc()) {
        descriptor->SetPhase(5);
        character->SetPosition(POS_STANDING);
        const BYTE empire = character->GetEmpire();
        PIXEL_POSITION position;
        if (!SECTREE_MANAGER::instance().GetRecallPositionByEmpire(character->GetMapIndex(),
                                                                    empire, position)) {
            _sys_err("dead_event", 0x1ab, "cannot find spawn position (name %s)", character->GetName());
            const unsigned index = empire >= 1 && empire <= 3 ? empire : 0;
            position.x = g_start_position[index][0];
            position.y = g_start_position[index][1];
        }

        character->WarpSet(position.x, position.y, 0);
        character->PointChange(POINT_HP,
            SPBits(static_cast<unsigned>(character->m_pointsInstant.iMaxHP / 2) -
                   static_cast<unsigned>(character->m_points.hp)), true, false);
        character->DeathPenalty(0);
        character->StartRecoveryEvent();
        character->ChatPacket(5, "CloseRestartWindow");
        return 0;
    }

    CHARACTER_MANAGER& manager = CHARACTER_MANAGER::instance();
    if (character->GetCharType() == 0 && !character->IsRevive() &&
        character->HasReviverInParty()) {
        LPCHARACTER revived = manager.SpawnMob(character->GetMobTable().dwVnum,
            character->GetMapIndex(), character->GetX(), character->GetY(), character->GetZ(),
            false, -1, true);
        CParty* party = character->GetParty();
        if (party && revived) {
            party->Join(revived->GetVID());
            party->Link(revived);
            const DWORD victimVID = character->GetVID();
            const bool leaderLeaves = party->GetLeaderPID() == victimVID;
            party->Quit(victimVID);
            if (!leaderLeaves)
                party->Unlink(character);
            revived->SetAggressive();
            revived->SetRevive(true);
        }
    }

    manager.DestroyCharacter(character);
    return 0;
}

// Original DeathPenalty at 08147dc0.
void CHARACTER::DeathPenalty(BYTE town)
{
    static const char protectionMessage[] =
        "\277\353\275\305\300\307\040\260\241\310\243\267\316\040"
        "\260\346\307\350\304\241\260\241\040\266\263\276\356\301\366"
        "\301\366\040\276\312\276\322\275\300\264\317\264\331.";
    sys_log(0, "DEATH_PERNALY_CHECK(%s) town(%d)", GetName(), town);
    Cube_close(this);
    if (GetLevel() < 10) {
        sys_log(0, "NO_DEATH_PENALTY_LESS_LV10(%s)", GetName());
        ChatPacket(1, locale_find(protectionMessage, g_iUseLocale));
        return;
    }
    if (number_ex(0, 2, const_cast<char*>("char_battle.cpp"), 0x11f)) {
        sys_log(0, "NO_DEATH_PENALTY_LUCK(%s)", GetName());
        ChatPacket(1, locale_find(protectionMessage, g_iUseLocale));
        return;
    }
    if (!(m_pointsInstant.instant_flag & 1))
        return;

    m_pointsInstant.instant_flag &= ~1L;
    if ((LC_IsYMIR() || !town) && FindAffect(0x1ff, 0)) {
        sys_log(0, "NO_DEATH_PENALTY_AFFECT(%s)", GetName());
        ChatPacket(1, locale_find(protectionMessage, g_iUseLocale));
        RemoveAffect(static_cast<DWORD>(0x1ff));
        return;
    }

    const int level = MIN(99, GetLevel());
    const int percent = aiExpLossPercents[level];
    // The original multiplies and divides unsigned 32-bit values.
    int loss = static_cast<std::uint32_t>(GetNextExp()) * static_cast<std::uint32_t>(percent) / 100U;
    if (LC_IsYMIR())
        loss = MIN(200000, loss);
    if (town)
        loss = g_iUseLocale ? 0 : loss - loss / 3;
    if (IsEquipUniqueItem(0x1117c))
        loss /= 2;

    sys_log(0, "DEATH_PENALTY(%s) EXP_LOSS: %d percent %d%%", GetName(), loss,
            aiExpLossPercents[MIN(99, GetLevel())]);
    PointChange(POINT_EXP, -loss, true, false);
}

namespace {
struct ItemDropPenaltyRates
{
    int inventoryPct;
    int inventoryQty;
    int equipmentPct;
    int equipmentQty;
};

// Original aItemDropPenalty_chn and aItemDropPenalty_kor.
const ItemDropPenaltyRates chinesePenalty[9] = {
    { 5, 1, 0, 0 },
    { 10, 2, 0, 0 },
    { 20, 3, 0, 0 },
    { 30, 4, 0, 0 },
    { 50, 5, 0, 0 },
    { 200, 5, 5, 1 },
    { 300, 10, 10, 2 },
    { 400, 20, 30, 3 },
    { 800, 40, 40, 4 },
};
const ItemDropPenaltyRates koreanPenalty[9] = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 25, 1, 5, 1 },
    { 50, 2, 10, 1 },
    { 75, 4, 15, 1 },
    { 100, 8, 20, 1 },
};

void ShufflePenaltySlots(std::vector<BYTE>& slots)
{
    for (std::size_t index = 1; index < slots.size(); ++index)
        std::swap(slots[index], slots[static_cast<std::size_t>(::lrand48()) % (index + 1)]);
}
}

// 0814a310. The old slots use absolute inventory and relative wear indices.
void CHARACTER::ItemDropPenalty(LPCHARACTER killer)
{
    if (m_pkMyShop || (!LC_IsYMIR() && GetLevel() < 50) || GetLevel() < 10)
        return;

    const bool cibn = LC_IsCIBN();
    const int alignment = GetRealAlignment();
    int tier = 4;
    if (!(cibn && killer && killer->GetDesc() && killer->GetEmpire() != GetEmpire())) {
        const int thresholds[] = {120000, 80000, 40000, 10000, 0, -39999, -79999, -119999};
        tier = 0;
        while (tier < 8 && alignment < thresholds[tier])
            ++tier;
    }

    const ItemDropPenaltyRates& rate = (cibn ? chinesePenalty : koreanPenalty)[tier];
    sys_log(0, "%s align %d inven_pct %d equip_pct %d", GetName(), tier,
            rate.inventoryPct, rate.equipmentPct);
    char* const file = const_cast<char*>("char_battle.cpp");
    const bool dropInventory = number_ex(1, 1000, file, 0x401) <= rate.inventoryPct;
    const bool dropEquipment = number_ex(1, 100, file, 0x402) <= rate.equipmentPct;
    const DWORD protectiveItem = 0x111a1;
    const bool dropProtectiveItem =
        (dropInventory || dropEquipment) && IsEquipUniqueItem(protectiveItem);
    std::vector<std::pair<LPITEM, int>> drops;

    const auto dropSlot = [&](BYTE slot, LPITEM item, int window, bool checkFlags) {
        if (!item)
            return;
        const TItemTable* proto = item->GetProto();
        if (checkFlags && proto && (proto->dwAntiFlags & 0x6000))
            return;
        SyncQuickslot(1, slot, 255);
        drops.emplace_back(item->RemoveFromCharacter(), window);
    };

    bool allEquipmentEligible = false;
    if (!dropProtectiveItem && dropInventory) {
        std::vector<BYTE> slots;
        for (BYTE slot = 0; slot < 90; ++slot)
            if (GetItem(slot))
                slots.push_back(slot);
        if (slots.empty()) {
            allEquipmentEligible = tier == 8;
        } else {
            ShufflePenaltySlots(slots);
            const int available = static_cast<int>(slots.size());
            int quantity;
            if (cibn) {
                quantity = MIN(available, number_ex(1, rate.inventoryQty, file, 0x41d));
            } else {
                quantity = MIN(available, rate.inventoryQty);
                if (quantity)
                    quantity = number_ex(1, quantity, file, 0x423);
            }
            for (int index = 0; index < quantity; ++index)
                dropSlot(slots[index], GetItem(slots[index]), 1, true);
        }
    }

    if (!dropProtectiveItem && dropEquipment) {
        std::vector<BYTE> slots;
        for (BYTE slot = 0; slot < 19; ++slot)
            if (GetWear(slot))
                slots.push_back(slot);
        if (!slots.empty()) {
            ShufflePenaltySlots(slots);
            const int available = static_cast<int>(slots.size());
            int quantity = allEquipmentEligible
                ? available : MIN(available, number_ex(1, rate.equipmentQty, file, 0x446));
            if (quantity)
                quantity = number_ex(1, quantity, file, 0x449);
            for (int index = 0; index < quantity; ++index)
                dropSlot(slots[index], GetWear(slots[index]), 2, true);
        }
    }

    if (dropProtectiveItem) {
        for (BYTE slot = 7; slot <= 8; ++slot) {
            LPITEM item = GetWear(slot);
            if (item && item->GetVnum() == protectiveItem)
                dropSlot(slot, item, 2, false);
        }
    }

    PIXEL_POSITION position = {GetX(), GetY(), 0};
    for (const auto& drop : drops) {
        LPITEM item = drop.first;
        item->AddToGround(GetMapIndex(), position);
        item->StartDestroyEvent();
        position.x = GetX() + number_ex(-7, 7, file, 0x47c) * 20;
        position.y = GetY() + number_ex(-7, 7, file, 0x47d) * 20;
        sys_log(0, "DROP_ITEM_PK: %s %d %d from %s", item->GetName(),
                static_cast<int>(position.x), static_cast<int>(position.y), GetName());
        LogManager::instance().ItemLog(GetPlayerID(), GetX(), GetY(), item->GetID(),
                "DEAD_DROP", drop.second == 1 ? "INVENTORY" : "EQUIPMENT");
    }
}

EVENTFUNC(StunEvent)
{
    LPCHARACTER character = static_cast<CharacterDeathEventInfo*>(event->info)->character;
    character->m_pkStunEvent = 0;
    character->Dead(0, false);
    return 0;
}

void CHARACTER::Stun()
{
    if (IsStun() || IsDead())
        return;

    if (!GetDesc() && m_pkParty)
        m_pkParty->SendMessage(this, 2, 0, 0);
    sys_log(1, "%s: Stun %p", GetName(), static_cast<void*>(this));
    PointChange(POINT_HP_RECOVERY, SPBits(0u - static_cast<unsigned>(GetPoint(POINT_HP_RECOVERY))),
                false, false);
    PointChange(POINT_SP_RECOVERY, SPBits(0u - static_cast<unsigned>(GetPoint(POINT_SP_RECOVERY))),
                false, false);
    ClearAffect(true);
    CloseMyShop();
    event_cancel(&m_pkRecoveryEvent);
    const TPacketGCStun packet = {13, static_cast<std::uint32_t>(static_cast<DWORD>(GetVID()))};
    PacketAround(&packet, sizeof(packet));
    m_pointsInstant.instant_flag |= 8;
    if (m_pkStunEvent)
        return;

    void* raw = 0;
    const DWORD id = __AllocEventInfo(sizeof(CharacterDeathEventInfo), &raw);
    static_cast<CharacterDeathEventInfo*>(raw)->character = this;
    m_pkStunEvent = event_create_ex(StunEvent, id, passes_per_sec * 3,
                                    const_cast<char*>("char_battle.cpp"), 0x194);
}

namespace {
const char scoreNotice[] = "\307\366\300\347 \275\272\304\332\276\356 \275\305\274\366\261\271:%d \303\265\301\266\261\271:%d \301\370\263\353\261\271:%d";
const char debugDeath[] = "\301\327\300\272 \301\244\272\270 %s deadcount %d Empire %d killcount %d(Purpose %d) killmobcount %d, RegenFlag %d";
const char exitScriptPrefix[] = "\274\272\301\366\300\307 \270\266\277\325: \263\312\310\361 ";
const char exitScriptText[] = "\263\340\274\256\265\351\300\272 \300\314\260\367 \274\272\301\366\277\241 \300\326\300\273 \300\332\260\335\300\273 \300\322\276\372\264\331. \270\360\265\316 \274\272\301\366\277\241\274\255 \271\260\267\257\263\252\260\305\266\363~~[ENTER][ENTER] 10\303\312 \310\304\277\241 \270\360\265\316 \270\266\300\273\267\316 \300\314\265\277\307\317\260\324 \265\313\264\317\264\331. ";
const char exitNotice[] = "\273\357\260\305\270\256 \300\374\305\365\277\241\274\255 %s \261\271\260\241\260\241 \260\241\300\345\270\325\300\372 \305\273\266\364\300\273 \307\317\277\264\275\300\264\317\264\331";
const char debugRegen[] = "\273\357\260\305\270\256 \270\367 \270\256\301\250";
const char victoryScriptText[] = ". \263\312\310\361\260\241 \274\272\301\366\300\307 \274\366\310\243\300\332\270\246 \300\342\260\324 \265\310\264\331\270\351 \263\312\310\361\264\302 \274\272\301\366\300\307 \301\326\300\316\300\314 \265\310\264\331.[ENTER][ENTER] ";
const char debugSpawn[] = "Spawn \301\370\261\270\271\314\310\243";

const char nationNames[4][32] = {
    "", "\275\305\274\366\261\271", "\303\265\301\266\261\271", "\301\370\263\353\261\271",
};
const int startMaps[4] = {0, 1, 21, 41};

struct WarpEmpireEventInfo
{
    int empire;
    int fromMap;
    int toMap;
    int x;
    int y;
};

template <class Callback>
void ForEachThreeWayPlayer(long mapIndex, const Callback& callback)
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(mapIndex);
    if (!map)
        return;
    for (const auto& entry : *map) {
        entry.second->ForEachEntity([&](LPENTITY entity) {
            if (entity->IsType(0) && entity->GetDesc())
                callback(static_cast<LPCHARACTER>(entity));
        });
    }
}

void SendThreeWayScore()
{
    char text[64];
    std::snprintf(text, sizeof(text), locale_find(scoreNotice, g_iUseLocale),
                  get_kill_count_per_empire(1), get_kill_count_per_empire(2),
                  get_kill_count_per_empire(3));
    ForEachThreeWayPlayer(GetSungziMapIndex(), [&](LPCHARACTER player) {
        player->ChatPacket(2, "%s", text);
    });
}

}

EVENTFUNC(warp_all_to_map_my_empire_event)
{
    const auto* info = static_cast<WarpEmpireEventInfo*>(event->info);
    if (!SECTREE_MANAGER::instance().GetMap(info->fromMap))
        return 0;
    sys_log(0, "%d %d %d %d", info->toMap, info->x, info->y, info->empire);
    ForEachThreeWayPlayer(info->fromMap, [&](LPCHARACTER player) {
        if (player->GetEmpire() == info->empire)
            player->WarpSet(info->x, info->y, info->toMap);
    });
    return 0;
}

EVENTFUNC(regen_mob_event)
{
    g_bRegenFlag = 1;
    const int mapIndex = *static_cast<int*>(event->info);
    const char* path = GetSungziMapPath();
    const auto* map = SECTREE_MANAGER::instance().GetMap(mapIndex);
    if (!path || !map)
        return 0;
    const std::string filename = std::string(path) + "regen.txt";
    if (test_server)
        sys_log(0, "Regen Path %s", filename.c_str());
    regen_load_in_file(filename.c_str(), mapIndex, map->m_setting.iBaseX, map->m_setting.iBaseY);
    return 0;
}

namespace {
void ScheduleEmpireWarp(int fromMap, int empire, int delay, int line)
{
    void* raw = 0;
    const DWORD id = __AllocEventInfo(sizeof(WarpEmpireEventInfo), &raw);
    *static_cast<WarpEmpireEventInfo*>(raw) = {
        empire, fromMap, startMaps[empire], static_cast<int>(g_start_position[empire][0]),
        static_cast<int>(g_start_position[empire][1]),
    };
    event_create_ex(warp_all_to_map_my_empire_event, id, passes_per_sec * delay,
                    const_cast<char*>("char_battle.cpp"), line);
}

int ThreeWayKillWeight(int level)
{
    if (LC_IsYMIR() || LC_IsCIBN()) {
        if (level >= 50 && level <= 59) return 1;
        if (level >= 60 && level <= 69) return 2;
        if (level >= 70 && level <= 90) return 3;
    } else {
        if (level >= 30 && level <= 39) return 1;
        if (level >= 40 && level <= 49) return 2;
        if (level >= 50 && level <= 90) return 3;
    }
    return 0;
}
}

// Called from Dead after the forked-map, killer-presence and GM gates.
void CHARACTER::ProcessThreeWayDeath(LPCHARACTER killer)
{
    m_iKillCount = SPBits(static_cast<unsigned>(m_iKillCount) - 1u);
    const int mapIndex = GetMapIndex();
    if (!killer->GetDesc() || !IsSungziMapIndex(mapIndex) || g_bRegenFlag == -1)
        return;

    const int killerEmpire = killer->GetEmpire();
    int score = get_kill_count_per_empire(killerEmpire);
    if (score >= 0)
        score = inc_kill_count_per_empire(killerEmpire, ThreeWayKillWeight(GetLevel()));
    if (score != 0 && score % 5 == 0)
        SendThreeWayScore();

    killer->m_iKillingCount = SPBits(static_cast<unsigned>(killer->m_iKillingCount) + 1u);
    auto& quests = quest::CQuestManager::instance();
    const int target = quests.GetEventFlag("threeway_war_kill_count");
    if (test_server) {
        ChatPacket(1, locale_find(debugDeath, g_iUseLocale), GetName(), m_iKillCount, GetEmpire(),
                   get_kill_count_per_empire(GetEmpire()), target,
                   get_mob_kill_count_per_empire(GetEmpire()), g_bRegenFlag);
    }
    if (quests.GetEventFlag("threeway_war") != 1)
        return;

    const bool firstRound = g_bRegenFlag == 0;
    if (!firstRound && g_bRegenFlag != 1)
        return;
    int behind = 0;
    int losingEmpire = 0;
    int winningEmpire = 0;
    for (int empire = 1; empire <= 3; ++empire) {
        const int kills = get_kill_count_per_empire(empire);
        if (!firstRound && kills == -1)
            continue;
        if (kills < target) {
            ++behind;
            losingEmpire = empire;
        } else {
            winningEmpire = empire;
        }
    }
    if (behind != 1) {
        if (firstRound)
            sys_log(0, "No Exit %d %d", GetEmpire(), 0);
        return;
    }

    if (firstRound) {
        sys_log(0, "Exit %d %d", GetEmpire(), 1);
        for (int empire = 1; empire <= 3; ++empire)
            set_kill_count_per_empire(empire, 0);
        set_kill_count_per_empire(losingEmpire, -1);
        sys_log(0, "[FORKED_LOAD] Exit %d", losingEmpire);
        ScheduleEmpireWarp(mapIndex, losingEmpire, 10, 0x5a3);
        ScheduleEmpireWarp(GetPassMapIndex(static_cast<BYTE>(losingEmpire)), losingEmpire, 10, 0x5b2);
        const char* nation = locale_find(nationNames[losingEmpire], g_iUseLocale);
        const std::string script = std::string(locale_find(exitScriptPrefix, g_iUseLocale)) + nation +
            locale_find(exitScriptText, g_iUseLocale) + "[ENTER][DONE]";
        CHARACTER_MANAGER::instance().SayInMap(mapIndex, script);
        char notice[128];
        std::snprintf(notice, sizeof(notice), locale_find(exitNotice, g_iUseLocale), nation);
        SendNotice(notice);
        if (g_bRegenFlag != 0)
            return;
        if (test_server)
            ChatPacket(1, locale_find(debugRegen, g_iUseLocale));
        void* raw = 0;
        const DWORD id = __AllocEventInfo(sizeof(int), &raw);
        *static_cast<int*>(raw) = mapIndex;
        event_create_ex(regen_mob_event, id, passes_per_sec * 10,
                        const_cast<char*>("char_battle.cpp"), 0x5da);
        return;
    }

    g_bRegenFlag = -1;
    SendThreeWayScore();
    ScheduleEmpireWarp(mapIndex, losingEmpire, 5, 0x61e);
    const std::string script = std::string(locale_find(nationNames[winningEmpire], g_iUseLocale)) +
        locale_find(victoryScriptText, g_iUseLocale) + "[ENTER][DONE]";
    CHARACTER_MANAGER::instance().SayInMapinMyEmpire(mapIndex, static_cast<BYTE>(winningEmpire), script);

    char* const source = const_cast<char*>("char_battle.cpp");
    for (int index = 0; index < quests.GetEventFlag("threeway_war_boss_count"); ++index) {
        const long x = GetX() - number_ex(200, 750, source, 0x642);
        const long y = GetY() - number_ex(200, 750, source, 0x643);
        LPCHARACTER boss = CHARACTER_MANAGER::instance().SpawnMob(GetSungziBossVnum(), mapIndex,
            x < 0 ? GetX() : x, y < 0 ? GetY() : y, 0, false, -1, true);
        if (boss)
            boss->SetAggressive();
        sys_log(0, "%s", debugSpawn);
    }
}

namespace {
const char alignmentProtection[] = "\277\353\275\305\300\307 \272\270\310\243\267\316 \276\306\300\314\305\333\300\314 \266\263\276\356\301\366\301\366 \276\312\276\322\275\300\264\317\264\331.";
const char warDropNotice[] = "<\261\346\265\345> \261\346\265\345\300\374\301\337\277\241\264\302 \273\347\263\311\277\241 \265\373\270\245 \300\314\300\315\300\314 \276\370\275\300\264\317\264\331.";

void ApplyDeathAlignmentLoss(LPCHARACTER killer)
{
    constexpr int amount = -20000;
    if (g_iUseLocale && killer->GetParty()) {
        const long x = killer->GetX();
        const long y = killer->GetY();
        const auto nearby = [&](LPCHARACTER member) {
            return DISTANCE_APPROX(member->GetX() - x, member->GetY() - y) < 5000;
        };
        int count = 0;
        CParty* party = killer->GetParty();
        party->ForEachOnlineMember([&](LPCHARACTER member) {
            if (nearby(member))
                ++count;
        });
        if (count != 0) {
            sys_log(0, "ALIGNMENT PARTY count %d amount %d", count, amount);
            party->ForEachOnlineMember([&](LPCHARACTER member) {
                if (nearby(member))
                    member->UpdateAlignment(amount / count);
            });
            return;
        }
    }
    killer->UpdateAlignment(amount);
}
}

// Original CHARACTER::Dead at 0814af70.
void CHARACTER::Dead(LPCHARACTER killer, bool immediate)
{
    if (IsDead())
        return;

    if (m_dwMountVnum) {
        if (IsHorseRiding())
            StopRiding();
        else {
            m_dwMountVnum = 0;
            UpdatePacket();
        }
    }

    if (!killer && m_dwKillerPID)
        killer = CHARACTER_MANAGER::instance().FindByPID(m_dwKillerPID);
    m_dwKillerPID = 0;

    bool agreedPvp = false;
    bool guildWar = false;
    bool duel = false;
    if (killer && killer->GetDesc()) {
        if (killer->m_pkChrTarget == this)
            killer->SetTarget(nullptr);
        if (!GetDesc() && killer->m_pkDungeon)
            killer->m_pkDungeon->IncKillCount(killer, this);
        agreedPvp = CPVPManager::instance().Dead(this, killer->GetPlayerID());
        duel = CArenaManager::instance().OnDead(killer, this);
        if (GetDesc()) {
            guildWar = m_pGuild && killer->m_pGuild && m_pGuild->UnderWar(killer->m_pGuild->GetID());
            killer->SetQuestNPCID(GetVID());
            quest::CQuestManager::instance().Kill(killer->GetPlayerID(), 0);
            CGuildManager::instance().Kill(killer, this);
        }
    }

    const long map = GetMapIndex();
    const bool forked = GetDesc() && IsForkedMapIndex(map);
    const bool castle = map >= 181 && map <= 183;
    if (!killer || agreedPvp || guildWar || !GetDesc() || duel || forked || castle) {
        if (test_server)
            sys_log(0, "[FORKED_LOAD] No Penalty! %s", GetName());
    } else {
        if (test_server)
            sys_log(0, "[FORKED_LOAD] Penalty! %s", GetName());
        if (!GetGMLevel() || test_server)
            ItemDropPenalty(killer);
    }

    if (castle) {
        const WORD race = GetRaceNum();
        if (race == 11505)
            castle_frog_die(this, killer);
        else if (castle_is_guard_vnum(race))
            castle_guard_die(this, killer);
        else if (castle_is_tower_vnum(race))
            castle_tower_die(this, killer);
    }

    SetPosition(POS_DEAD);
    ClearAffect(true);
    const bool protectedGm = GetGMLevel() && !test_server;
    if (GetDesc() && forked && killer && !protectedGm)
        ProcessThreeWayDeath(killer);

    if (killer && GetDesc()) {
        if (!killer->GetDesc()) {
            if (!forked) {
                sys_log(1, "DEAD: %s %p WITH PENALTY", GetName(), static_cast<void*>(this));
                m_pointsInstant.instant_flag |= 1L;
                LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), killer->GetRaceNum(),
                                               "DEAD_BY_NPC", killer->GetName());
            }
        } else {
            sys_log(1, "DEAD_BY_PC: %s %p KILLER %s %p", GetName(), static_cast<void*>(this),
                    killer->GetName(), static_cast<void*>(killer));
            m_pointsInstant.instant_flag &= ~1L;
            char detail[50];
            if (GetEmpire() == killer->GetEmpire()) {
                if (!agreedPvp && !guildWar && !IsKillerMode() && GetAlignment() >= 0 && !duel && !forked) {
                    const int chance = g_iUseLocale ? (killer->GetAlignment() >= 0 ? 33 : 20) : 0;
                    if (number_ex(1, 100, const_cast<char*>("char_battle.cpp"), 0x691) < chance)
                        killer->ChatPacket(1, locale_find(alignmentProtection, g_iUseLocale));
                    else
                        ApplyDeathAlignmentLoss(killer);
                }
                std::snprintf(detail, sizeof(detail), "%d %d %d %d %d %d",
                              GetEmpire(), GetAlignment(), GetPKMode(),
                              killer->GetEmpire(), killer->GetAlignment(), killer->GetPKMode());
            } else {
                const long empirePoints = MIN(GetPoint(POINT_EMPIRE_POINT), killer->GetPoint(POINT_EMPIRE_POINT));
                PointChange(POINT_EMPIRE_POINT, -(empirePoints / 10), false, false);
                killer->PointChange(POINT_EMPIRE_POINT, empirePoints / 5, false, false);
                GetPoint(POINT_EMPIRE_POINT);
                std::snprintf(detail, sizeof(detail), "%d %d %d %s %d %d %d %s",
                              GetEmpire(), GetAlignment(), GetPKMode(), killer->GetName(),
                              killer->GetEmpire(), killer->GetAlignment(), killer->GetPKMode(), killer->GetName());
            }
            LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), killer->GetPlayerID(), "DEAD_BY_PC", detail);
            if (test_server)
                ChatPacket(1, "%s killed you", killer->GetName());
        }
    } else {
        sys_log(1, "DEAD: %s %p", GetName(), static_cast<void*>(this));
        m_pointsInstant.instant_flag &= ~1L;
    }

    ClearSync();
    event_cancel(&m_pkStunEvent);
    if (LPDESC descriptor = GetDesc()) {
        m_dwLastDeadTime = get_dword_time();
        SetKillerMode(false);
        descriptor->SetPhase(6);
    } else if (!(m_pointsInstant.instant_flag & 0x10L)) {
        if (killer && killer->GetDesc() && killer->m_pGuild && killer->m_pGuild->UnderAnyWar(0)) {
            if (killer->m_dwUnderGuildWarInfoMessageTime < get_dword_time()) {
                killer->m_dwUnderGuildWarInfoMessageTime = static_cast<std::uint32_t>(get_dword_time() + 60000);
                killer->ChatPacket(1, locale_find(warDropNotice, g_iUseLocale));
            }
        } else {
            const DWORD resurrectionVnum = GetMobTable().dwResurrectionVnum;
            if (resurrectionVnum) {
                LPCHARACTER resurrected = CHARACTER_MANAGER::instance().SpawnMob(resurrectionVnum,
                    GetMapIndex(), GetX(), GetY(), GetZ(), true, static_cast<int>(m_pointsInstant.fRot), true);
                if (m_pkDungeon && resurrected)
                    resurrected->SetDungeon(m_pkDungeon);
                Reward(false);
            } else {
                Reward(!IsRevive());
            }
        }
    }

    if (GetMobRank() > 3 && killer && killer->GetDesc()) {
        char detail[50];
        std::snprintf(detail, sizeof(detail), "%d %ld", g_bChannel, killer->GetMapIndex());
        LogManager::instance().CharLog(killer->GetPlayerID(), GetX(), GetY(), GetRaceNum(),
                                       GetCharType() == CHAR_TYPE_STONE ? "STONE_KILL" : "BOSS_KILL", detail);
    }

    const TPacketGCDead packet = {14, static_cast<std::uint32_t>(static_cast<DWORD>(GetVID()))};
    PacketAround(&packet, sizeof(packet));
    m_pointsInstant.instant_flag &= ~8L;
    if (LPDESC descriptor = GetDesc()) {
        for (CAffect* affect : m_list_pkAffect)
            SendAffectAddPacket(descriptor, affect);
    }

    if (!duel) {
        if (m_pkDeadEvent) {
            sys_log(1, "DEAD_EVENT_CANCEL: %s %p %p", GetName(), static_cast<void*>(this),
                    static_cast<void*>(m_pkDeadEvent));
            event_cancel(&m_pkDeadEvent);
        }

        void* raw = nullptr;
        const DWORD eventId = __AllocEventInfo(sizeof(CharacterDeathEventInfo), &raw);
        static_cast<CharacterDeathEventInfo*>(raw)->character = this;
        if (GetCharType() == CHAR_TYPE_STONE)
            ClearStone();
        assert(!m_pkDeadEvent);
        if (m_pkDungeon)
            m_pkDungeon->DeadCharacter(this);

        int delay;
        int line;
        if (GetDesc()) {
            delay = passes_per_sec * 180;
            line = 0x725;
        } else {
            const bool revive = !IsRevive() && HasReviverInParty();
            delay = immediate ? 1 : passes_per_sec * (revive ? 3 : 10);
            line = revive ? 0x72b : 0x730;
        }
        m_pkDeadEvent = event_create_ex(dead_event, eventId, delay,
                                        const_cast<char*>("char_battle.cpp"), line);
        sys_log(1, "DEAD_EVENT_CREATE: %s %p %p", GetName(), static_cast<void*>(this),
                static_cast<void*>(m_pkDeadEvent));
    }

    if (m_pkExchange)
        m_pkExchange->Cancel();
    if (IsCubeOpen())
        Cube_close(this);
    CShopManager::instance().StopShopping(this);
    CloseMyShop();
    CloseSafebox();
}

void CHARACTER::FlyTarget(DWORD targetVID, long x, long y, BYTE header)
{
    LPCHARACTER target = CHARACTER_MANAGER::instance().Find(targetVID);
    const bool mainTarget = header == 51;
    TPacketGCFlyTargeting packet = {
        static_cast<BYTE>(mainTarget ? 71 : 69),
        static_cast<std::uint32_t>(static_cast<DWORD>(GetVID())),
        0, static_cast<std::int32_t>(x), static_cast<std::int32_t>(y),
    };
    if (target) {
        packet.dwTargetVID = static_cast<std::uint32_t>(static_cast<DWORD>(target->GetVID()));
        packet.x = static_cast<std::int32_t>(target->GetX());
        packet.y = static_cast<std::int32_t>(target->GetY());
        if (mainTarget)
            m_dwFlyTargetID = targetVID;
        else
            m_vec_dwFlyTargets.push_back(targetVID);
    }

    sys_log(1, "FlyTarget %s vid %d x %d y %d", GetName(),
            static_cast<int>(packet.dwTargetVID), packet.x, packet.y);
    PacketAround(&packet, sizeof(packet), this);
}

void CHARACTER::UseArrow(LPITEM arrow, DWORD count)
{
    const DWORD vnum = arrow->GetVnum();
    const auto available = static_cast<std::uint32_t>(arrow->GetCount());
    const auto used = std::min(static_cast<std::int32_t>(available), static_cast<std::int32_t>(count));
    const auto remaining = available - static_cast<std::uint32_t>(used);
    arrow->SetCount(remaining);
    if (remaining != 0)
        return;

    LPITEM replacement = FindSpecifyItem(vnum);
    sys_log(0, "UseArrow : FindSpecifyItem %lu %p", vnum, static_cast<void*>(replacement));
    if (replacement)
        EquipItem(replacement);
}

bool CHARACTER::Shoot(BYTE type)
{
    sys_log(1, "Shoot %s type %u flyTargets.size %u", GetName(),
            static_cast<unsigned>(type), static_cast<unsigned>(m_vec_dwFlyTargets.size()));
    if (!CanMove())
        return false;

    bool success = false;
    const auto shootAt = [&](DWORD targetVID) {
        if (type > 1) {
            if (g_bSkillDisable)
                return;
            m_SkillUseInfo[type].dwVID = targetVID;
        }

        LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(targetVID);
        if (!victim || !battle_is_attackable(this, victim))
            return;
        if (GetCharType() != CHAR_TYPE_PC) {
            const int dx = SPBits(static_cast<unsigned>(GetX()) - static_cast<unsigned>(victim->GetX()));
            const int dy = SPBits(static_cast<unsigned>(GetY()) - static_cast<unsigned>(victim->GetY()));
            if (DISTANCE_APPROX(dx, dy) > 5000)
                return;
        }

        const auto beginAttack = [&]() {
            OnMove(true);
            victim->OnMove(false);
            if (victim->CanBeginFight())
                victim->BeginFight(this);
        };

        LPITEM bow;
        LPITEM arrow;
        switch (type) {
        case 0:
        case 1: {
            int damage;
            if (type == 1) {
                if (GetDesc())
                    return;
                damage = CalcMagicDamage(this, victim);
            } else if (!GetDesc()) {
                damage = CalcMeleeDamage(this, victim, false, false);
            } else {
                if (GetJob() != 1 || GetArrowAndBow(&bow, &arrow, 1) == 0)
                    return;
                if (GetSkillGroup() && GetCharType() == 6 && GetSkillGroup() != 2) {
                    if (m_points.sp < 5)
                        return;
                    PointChange(POINT_SP, -5, false, false);
                }
                damage = CalcArrowDamage(this, victim, bow, arrow, false);
                UseArrow(arrow, 1);
                if (IS_SPEED_HACK(this, victim, get_dword_time()))
                    damage = 0;
            }

            NormalAttackAffect(this, victim);
            const BYTE resist = type == 0 ? POINT_RESIST_BOW : POINT_RESIST_MAGIC;
            const int resistanceFactor = SPBits(100u - static_cast<unsigned>(victim->GetPoint(resist)));
            damage = SPProduct(damage, resistanceFactor) / 100;
            sys_log(0, "%s arrow %s dam %d", GetName(), victim->GetName(), damage);
            beginAttack();
            victim->Damage(this, damage, type == 0 ? DAMAGE_TYPE_NORMAL_RANGE : DAMAGE_TYPE_MAGIC);
            break;
        }
        case 20: case 35: case 66: case 76: case 80: case 81: case 91: case 106: case 108:
            beginAttack();
            sys_log(0, "%s - Skill %d -> %s", GetName(), type, victim->GetName());
            ComputeSkill(type, victim, 0);
            break;
        case 46: case 47: case 48: case 50: case 140:
            if (GetArrowAndBow(&bow, &arrow, 1) == 1) {
                beginAttack();
                if (type != 46) {
                    const char* format = type == 47 ? "%s kwankeyok %s" : type == 48 ? "%s hwajo %s"
                        : type == 50 ? "%s gigung %s" : "%s horse_wildattack %s";
                    sys_log(0, format, GetName(), victim->GetName());
                }
                ComputeSkill(type, victim, 0);
                UseArrow(arrow, 1);
            }
            break;
        case 92:
            OnMove(true);
            break;
        default:
            _sys_err("operator()", 0xf29, "CFuncShoot: I don't know this type [%d] of range attack.", type);
            break;
        }
        success = true;
    };

    if (m_dwFlyTargetID) {
        shootAt(m_dwFlyTargetID);
        m_dwFlyTargetID = 0;
    }
    for (const DWORD target : m_vec_dwFlyTargets)
        shootAt(target);
    m_vec_dwFlyTargets.clear();
    return success;
}
