#include <cstring>
#include <cstdio>
#include <cmath>
#include <climits>
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
#include "mob_manager.h"
#include "item.h"
#include "config.h"
#include "char_manager.h"
#include "battle.h"
#include <cstdint>

DWORD ani_attack_speed(CHARACTER* character);
#include "arena.h"
#include "castle.h"
#include "char.h"
#include "guild.h"
#include "locale_service.h"
#include "locale.hpp"
#include "utils.h"
#include "pvp.h"
#include "sectree.h"

bool battle_distance_valid_by_xy(long x, long y, long targetX, long targetY)
{
    return DISTANCE_APPROX(x - targetX, y - targetY) < 171;
}

bool battle_distance_valid(CHARACTER* character, CHARACTER* victim)
{
    return battle_distance_valid_by_xy(character->GetX(), character->GetY(),
                                       victim->GetX(), victim->GetY());
}

// 080d5ad0
bool timed_event_cancel(CHARACTER* character)
{
    if (!character->m_pkTimedEvent)
        return false;
    event_cancel(&character->m_pkTimedEvent);
    return true;
}

int battle_melee_attack(CHARACTER* attacker, CHARACTER* victim)
{
    if (!victim || attacker == victim || !battle_is_attackable(attacker, victim))
        return 0;

    const int distance = DISTANCE_APPROX(attacker->GetX() - victim->GetX(), attacker->GetY() - victim->GetY());
    if (victim->GetCharType() != 5 && (attacker->GetDesc() ? distance > 299
                                                         : distance >= attacker->GetMobAttackRange() * 1.15)) {
        if (test_server) {
            victim->ChatPacket(1, "VICTIM_FAR: %s distance %d %dx%d -> %dx%d",
                               attacker->GetName(), distance,
                               static_cast<int>(attacker->GetX()), static_cast<int>(attacker->GetY()),
                               static_cast<int>(victim->GetX()), static_cast<int>(victim->GetY()));
        }
        return 0;
    }
    if (timed_event_cancel(attacker))
        attacker->ChatPacket(1, "%s", locale_find("\300\374\305\365\260\241\040\275\303\300\333\040\265\307\276\356\040\303\353\274\322\040\265\307\276\372\275\300\264\317\264\331\056", g_iUseLocale));
    if (timed_event_cancel(victim))
        victim->ChatPacket(1, "%s", locale_find("\300\374\305\365\260\241\040\275\303\300\333\040\265\307\276\356\040\303\353\274\322\040\265\307\276\372\275\300\264\317\264\331\056", g_iUseLocale));

    attacker->SetPosition(5);
    attacker->SetVictim(victim);
    attacker->SetRotationToXY(victim->GetX(), victim->GetY());
    int damage;
    return battle_hit(attacker, victim, &damage);
}

bool battle_is_attackable(CHARACTER* attacker, CHARACTER* victim)
{
    if (victim->IsDead())
        return false;

    SECTREE* tree = attacker->GetSectree();
    if (tree && tree->IsAttr(attacker->GetX(), attacker->GetY(), ATTR_BANPK))
        return false;
    tree = victim->GetSectree();
    if (tree && tree->IsAttr(victim->GetX(), victim->GetY(), ATTR_BANPK))
        return false;
    if (attacker->IsStun() || attacker->IsDead())
        return false;

    if (attacker->GetDesc() && victim->GetDesc()) {
        CGuild* guild = attacker->GetGuild();
        CGuild* other = victim->GetGuild();
        if (guild && other && guild->UnderWar(other->GetID()))
            return true;
        if (LC_IsJapan() && (attacker->IsPolymorphed() || victim->IsPolymorphed()))
            return false;
    }
    const long map = attacker->GetMapIndex();
    if ((map == 181 || map == 182 || map == 183) && !castle_can_attack(attacker, victim))
        return false;
    return CArenaManager::instance().CanAttack(attacker, victim) ||
           CPVPManager::instance().CanAttack(attacker, victim);
}

namespace {
int FromBits(unsigned int bits)
{
    int value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

int Add(int lhs, int rhs) { return FromBits(static_cast<unsigned int>(lhs) + rhs); }
int Subtract(int lhs, int rhs) { return FromBits(static_cast<unsigned int>(lhs) - rhs); }
int Multiply(int lhs, int rhs) { return FromBits(static_cast<unsigned int>(lhs) * rhs); }

int TruncateDamage(long double value)
{
    if (!(value > -2147483649.0L && value < 2147483648.0L))
        return INT_MIN; // x87's integer-indefinite result
    return static_cast<int>(value);
}
}

int battle_hit(CHARACTER* attacker, CHARACTER* victim, int* resultDamage)
{
    int damage = CalcMeleeDamage(attacker, victim, false, false);
    if (damage < 1)
        return 1;

    NormalAttackAffect(attacker, victim);
    if (CItem* weapon = attacker->GetWear(4)) {
        BYTE resistance = 0;
        switch (weapon->GetSubType()) {
            case 0: resistance = 69; break;
            case 1: resistance = 71; break;
            case 3: resistance = 70; break;
            case 4: resistance = 72; break;
            case 5: resistance = 73; break;
        }
        if (resistance)
            damage = Multiply(Subtract(100, victim->GetPoint(resistance)), damage) / 100;
    }
    *resultDamage = damage;
    return victim->Damage(attacker, damage, DAMAGE_TYPE_NORMAL) ? 3 : 1;
}

// 080d5ff0
long double CalcAttackRating(CHARACTER* attacker, CHARACTER* victim, bool ignoreTargetRating)
{
    int attack;
    int target;
    if (LC_IsYMIR()) {
        attack = MIN(90, attacker->GetPolymorphPoint(14));
        target = victim->GetPolymorphPoint(14);
    } else if (LC_IsJapan()) {
        const int attackerDX = attacker->GetPolymorphPoint(14);
        const int attackerST = attacker->GetPolymorphPoint(12);
        const int attackerIQ = attacker->GetPolymorphPoint(15);
        attack = MIN(90, Add(Add(Multiply(attackerDX, 4), attackerST), attackerIQ) / 6);
        const int victimDX = victim->GetPolymorphPoint(14);
        const int victimST = victim->GetPolymorphPoint(12);
        const int victimIQ = victim->GetPolymorphPoint(15);
        target = Add(Add(Multiply(victimDX, 4), victimST), victimIQ) / 6;
    } else {
        const int attackerDX = attacker->GetPolymorphPoint(14);
        const int attackerLevel = attacker->GetLevel();
        const int victimDX = victim->GetPolymorphPoint(14);
        const int levelForTarget = attacker->GetLevel();
        attack = MIN(90, Add(attackerLevel, Multiply(attackerDX, 2)) / 3);
        target = Add(levelForTarget, Multiply(victimDX, 2)) / 3;
    }
    target = MIN(90, target);
    const long double attackRating = (attack + 210.0L) / 300.0L;
    if (ignoreTargetRating)
        return attackRating;
    const long double evasion = static_cast<long double>(Add(Multiply(target, 2), 5)) / Add(target, 95);
    return attackRating - evasion * 3.0L / 10.0L;
}

// 080d61e0
int CalcAttBonus(CHARACTER* attacker, CHARACTER* victim, int attack)
{
    if (!victim->GetDesc())
        attack = Add(attack, attacker->GetMarriageBonus(71073, true));
    if (!attacker->GetDesc()) {
        const int modifier = victim->GetMarriageBonus(71072, true);
        attack = Multiply(Add(modifier, 100), attack) / 100;
    }
    if (attacker->GetCharType() != 6 && victim->GetDesc()) {
        const int rate = CHARACTER_MANAGER::instance().GetMobDamageRate(attacker);
        attack = Multiply(rate, attack) / 100;
    }
    if (victim->GetCharType() == 6) {
        if (victim->GetDesc()) {
            attack = Add(attack, Multiply(attacker->GetPoint(43), attack) / 100);
            const BYTE job = victim->GetJob();
            if (job < 4)
                attack = Add(attack, Multiply(attacker->GetPoint(54 + job), attack) / 100);
        }
    } else {
        static const BYTE racePoints[] = {44, 47, 48, 43, 45, 46, 49, 50, 51, 52, 58};
        for (int i = 0; i < 11; ++i) {
            if (victim->IsRaceFlag(1u << i)) {
                attack = Add(attack, Multiply(attacker->GetPoint(racePoints[i]), attack) / 100);
                break;
            }
        }
        attack = Add(attack, Multiply(attacker->GetPoint(53), attack) / 100);
    }
    if (attacker->GetDesc()) {
        const BYTE job = attacker->GetJob();
        if (job < 4)
            attack = Subtract(attack, Multiply(victim->GetPoint(59 + job), attack) / 100);
    }
    return attack;
}

// 080d65e0
void Item_GetDamage(CItem* item, int* minimum, int* maximum)
{
    *minimum = 0;
    *maximum = 1;
    if (!item || item->GetType() == 13 || item->GetType() == 24)
        return;
    if (item->GetType() != 1)
        _sys_err("Item_GetDamage", 437, "Item_GetDamage - !ITEM_WEAPON vnum=%d, type=%d",
                 item->GetVnum(), static_cast<unsigned int>(item->GetType()));
    *minimum = item->GetValue(3);
    *maximum = item->GetValue(4);
}

int CalcBattleDamage(int damage, int, int)
{
    return damage < 3 ? number_ex(1, 5, "battle.cpp", 262) : damage;
}

// 080d6f40
int CalcArrowDamage(CHARACTER* attacker, CHARACTER* victim, CItem* bow, CItem* arrow, bool ignoreDefense)
{
    if (!bow || bow->GetType() != 1 || bow->GetSubType() != 2 || !arrow)
        return 0;
    const int dx = Subtract(attacker->GetX(), victim->GetX());
    const int dy = Subtract(attacker->GetY(), victim->GetY());
    const float distance = std::sqrt(static_cast<double>(Add(Multiply(dx, dx), Multiply(dy, dy))));
    const int dist = TruncateDamage(distance);
    const int gap = Subtract(Subtract(dist / 100, attacker->GetPoint(34)), 5);
    int percent = Add(Multiply(gap, -5), 100);
    if (percent < 1)
        return 0;
    if (percent > 100)
        percent = 100;

    const float rating = CalcAttackRating(attacker, victim, false);
    const int maximum = bow->GetValue(4);
    const int minimum = bow->GetValue(3);
    const int weaponDamage = number_ex(minimum, maximum, "battle.cpp", 658);
    const int arrowDamage = arrow->GetValue(3);
    const int grade = attacker->GetPoint(18);
    const int levelDamage = attacker->GetLevel() * 2;
    const int graded = Subtract(Add(Add(grade, Multiply(weaponDamage, 2)), arrowDamage), levelDamage);
    const int attack = Add(TruncateDamage(static_cast<long double>(graded) * rating), levelDamage);
    const int refineBonus = Multiply(bow->GetValue(5), 2);
    const int flatBonus = attacker->GetPoint(91);
    const int percentBonus = attacker->GetPoint(93);
    int damage = CalcAttBonus(attacker, victim,
                              Multiply(Add(Add(attack, refineBonus), flatBonus), Add(percentBonus, 100)) / 100);
    int defense = 0;
    if (!ignoreDefense) {
        const int defenseGrade = victim->GetPoint(16);
        const int defenseBonus = attacker->GetPoint(94);
        defense = Multiply(Add(defenseBonus, 100), defenseGrade) / 100;
    }
    if (attacker->GetCharType() != 6)
        damage = TruncateDamage(static_cast<long double>(attacker->GetMobDamageMultiply()) *
                                static_cast<float>(damage));
    damage = Multiply(MAX(0, Subtract(damage, defense)), percent) / 100;
    if (test_server) {
        const char* victimName = victim->GetName();
        const char* attackerName = attacker->GetName();
        attacker->ChatPacket(1, "ARROW %s -> %s, DAM %d DIST %d GAP %d %% %d",
                             attackerName, victimName, damage, dist, gap, percent);
    }
    return damage;
}

// 080d66b0
int CalcMeleeDamage(CHARACTER* attacker, CHARACTER* victim, bool ignoreDefense, bool ignoreTargetRating)
{
    CItem* weapon = attacker->GetWear(4);
    const bool polymorphed = attacker->IsPolymorphed();
    if (weapon && (!polymorphed || attacker->IsPolyMaintainStat())) {
        if (weapon->GetType() != 1)
            return 0;
        switch (weapon->GetSubType()) {
            case 0: case 1: case 3: case 4: case 5: case 7:
                break;
            case 2:
                _sys_err("CalcMeleeDamage", 464, "CalcMeleeDamage should not handle bows (name: %s)", attacker->GetName());
                return 0;
            default:
                return 0;
        }
    }

    const float rating = CalcAttackRating(attacker, victim, ignoreTargetRating);
    int minimum = 0;
    int maximum = 0;
    if (polymorphed && !attacker->IsPolyMaintainStat()) {
        Item_GetDamage(weapon, &minimum, &maximum);
        const CMob* mob = CMobManager::instance().Get(attacker->GetPolymorphVnum());
        if (mob) {
            const unsigned int power = attacker->GetPolymorphPower();
            minimum = Add(minimum, static_cast<unsigned int>(mob->m_table.dwDamageRange[0] * power) / 100u);
            maximum = Add(maximum, static_cast<unsigned int>(mob->m_table.dwDamageRange[1] * power) / 100u);
        }
    } else if (weapon) {
        Item_GetDamage(weapon, &minimum, &maximum);
    } else if (attacker->GetCharType() != 6) {
        minimum = attacker->GetMobDamageMin();
        maximum = attacker->GetMobDamageMax();
    }
    const int weaponDamage = number_ex(minimum, maximum, "battle.cpp", 509);
    const int attackGrade = attacker->GetPoint(18);
    const int levelDamage = attacker->GetLevel() * 2;
    const int graded = Subtract(Add(attackGrade, Multiply(weaponDamage, 2)), levelDamage);
    int attack = Add(TruncateDamage(static_cast<long double>(graded) * rating), levelDamage);
    int refineBonus = 0;
    if (weapon) {
        attack = Add(attack, Multiply(weapon->GetValue(5), 2));
        refineBonus = Multiply(weapon->GetValue(5), 2);
    }
    const int flatBonus = attacker->GetPoint(91);
    const int percentBonus = attacker->GetPoint(93);
    attack = CalcAttBonus(attacker, victim, Multiply(Add(percentBonus, 100), Add(attack, flatBonus)) / 100);

    int defense = 0;
    if (!ignoreDefense) {
        const int defenseGrade = victim->GetPoint(16);
        const int defenseBonus = victim->GetPoint(94);
        defense = Multiply(Add(defenseBonus, 100), defenseGrade) / 100;
        if (!attacker->GetDesc())
            defense = Add(defense, victim->GetMarriageBonus(71074, true));
    }
    if (attacker->GetCharType() != 6)
        attack = TruncateDamage(static_cast<long double>(attacker->GetMobDamageMultiply()) *
                                static_cast<float>(attack));
    const int rawDamage = Subtract(attack, defense);
    const int damage = MAX(0, rawDamage);

    if (test_server) {
        const int debugLevel = attacker->GetLevel() * 2;
        const int debugGrade = attacker->GetPoint(18);
        const int debugStat = TruncateDamage(static_cast<long double>(Subtract(debugGrade, debugLevel)) * rating);
        const int debugFlat = attacker->GetPoint(91);
        const int debugWeapon = TruncateDamage(static_cast<long double>(Multiply(weaponDamage, 2)) * rating);
        int pureAttack = Add(Add(Add(debugLevel, debugStat), debugWeapon), refineBonus);
        char mobAttack[32] = {};
        if (attacker->GetCharType() != 6) {
            const float multiplier = attacker->GetMobDamageMultiply();
            snprintf(mobAttack, 31, "=%d*%.1f", pureAttack, static_cast<double>(multiplier));
            pureAttack = TruncateDamage(static_cast<long double>(attacker->GetMobDamageMultiply()) *
                                        static_cast<float>(pureAttack));
        }
        char refine[32] = {};
        snprintf(refine, 31, "+RB(%d)", refineBonus);
        char point[32] = {};
        if (debugFlat > 0)
            snprintf(point, 31, ", PT=%d", debugFlat);
        char unknownAttack[32] = {};
        if (attack != pureAttack)
            snprintf(unknownAttack, 31, "+?(%d)", Subtract(attack, pureAttack));
        char unknownDamage[32] = {};
        if (damage != rawDamage)
            snprintf(unknownDamage, 31, "+?(%d)", Subtract(damage, rawDamage));

        const char* victimName = victim->GetName();
        const char* attackerName = attacker->GetName();
        char message[128];
        snprintf(message, 127, "%s(%d)-%s(%d)%s=%d%s, ATK=LV(%d)+ST(%d)+WP(%d)%s%s%s, AR=%.3g%s",
                 attackerName, attack, victimName, defense, "", damage, unknownDamage, debugLevel,
                 debugStat, debugWeapon, refine, unknownAttack, mobAttack, static_cast<double>(rating), point);
        attacker->ChatPacket(0, "%s", message);
        victim->ChatPacket(0, "%s", message);
    }
    return CalcBattleDamage(damage, attacker->GetLevel(), victim->GetLevel());
}

// 080d5f50
int CalcMagicDamageWithValue(int damage, CHARACTER* attacker, CHARACTER* victim)
{
    return CalcBattleDamage(damage, attacker->GetLevel(), victim->GetLevel());
}

// 080d5f80
int CalcMagicDamage(CHARACTER* attacker, CHARACTER* victim)
{
    const int melee = attacker->GetCharType() != 6 ? CalcMeleeDamage(attacker, victim, false, false) : 0;
    return CalcMagicDamageWithValue(Add(melee, attacker->GetPoint(91)), attacker, victim);
}

// 080d5ed0
void battle_end_ex(CHARACTER* ch)
{
    if (ch->GetPosition() == POS_FIGHTING)
        ch->SetPosition(POS_STANDING);
}

// 080d5f00
void battle_end(CHARACTER* ch)
{
    battle_end_ex(ch);
}

namespace {
void AttackAffect(CHARACTER* attacker, CHARACTER* victim, BYTE point, DWORD immunity,
                  DWORD affect, BYTE apply, long amount, DWORD flag, int duration, const char* name)
{
    if (!attacker->GetPoint(point) || victim->IsAffectFlag(flag))
        return;
    const int roll = number_ex(1, 100, const_cast<char*>("battle.h"), 0x2e);
    const bool success = roll <= attacker->GetPoint(point) && !victim->IsImmune(immunity);
    if (success)
        victim->AddAffect(affect, apply, amount, flag, duration, 0, true, false);
    if (test_server) {
        const long chance = attacker->GetPoint(point);
        const char* attackerName = attacker->GetName();
        victim->ChatPacket(3, success ? "%s %s(%ld%%) SUCCESS" : "%s %s(%ld%%) FAIL",
                           attackerName, name, chance);
    }
}
}

void NormalAttackAffect(CHARACTER* attacker, CHARACTER* victim)
{
    if (attacker->GetPoint(37) && !victim->IsAffectFlag(4)) {
        const int roll = number_ex(1, 100, const_cast<char*>("battle.cpp"), 0x2c3);
        if (roll <= attacker->GetPoint(37))
            victim->AttackedByPoison(attacker);
    }
    const int stunDuration = attacker->GetDesc() && !victim->GetDesc() ? 4 : 2;
    AttackAffect(attacker, victim, 38, 1, 210, POINT_NONE, 0, 6, stunDuration, "STUN");
    AttackAffect(attacker, victim, 39, 2, 211, 19, -30, 5, 20, "SLOW");
}

DWORD GET_ATTACK_SPEED(CHARACTER* character)
{
    if (!character)
        return 1000;

    LPITEM weapon = character->GetWear(4);
    const int riding = character->IsHorseRiding() ? 50 : 0;
    const auto divisor = static_cast<std::uint32_t>(character->GetPoint(POINT_ATT_SPEED) +
        SPEEDHACK_LIMIT_BONUS + riding);
    if (!divisor)
        return 1000;
    DWORD speed = static_cast<std::uint32_t>(ani_attack_speed(character)) * 100U / divisor;
    if (weapon && weapon->GetSubType() == 1)
        speed >>= 1;
    return speed;
}

void SET_ATTACK_TIME(CHARACTER* attacker, CHARACTER* victim, DWORD now)
{
    if (attacker && victim && attacker->GetDesc()) {
        attacker->m_kAttackLog.dwVID = victim->GetVID();
        attacker->m_kAttackLog.dwTime = now;
    }
}

void SET_ATTACKED_TIME(CHARACTER* attacker, CHARACTER* victim, DWORD now)
{
    if (attacker && victim && attacker->GetDesc()) {
        victim->m_AttackedLog.dwPID = attacker->GetPlayerID();
        victim->m_AttackedLog.dwAttackedTime = now;
    }
}

bool IS_SPEED_HACK(CHARACTER* attacker, CHARACTER* victim, DWORD now)
{
    if (LC_IsNewCIBN())
        return false;

    const auto outgoing = static_cast<std::uint32_t>(now - attacker->m_kAttackLog.dwTime);
    if (attacker->m_kAttackLog.dwVID == static_cast<DWORD>(victim->GetVID()) &&
        outgoing < GET_ATTACK_SPEED(attacker)) {
        ++attacker->m_speed_hack_count;
        if (test_server) {
            _sys_err("IS_SPEED_HACK", 0x345, "%s attack hack! time (delta,limit)=(%lu,%lu) hack_count %d",
                     attacker->GetName(), static_cast<unsigned long>(outgoing), GET_ATTACK_SPEED(attacker),
                     attacker->m_speed_hack_count);
            attacker->ChatPacket(1, "%s attack hack! time (delta,limit)=(%lu,%lu) hack_count %d",
                attacker->GetName(), static_cast<unsigned long>(outgoing), GET_ATTACK_SPEED(attacker),
                attacker->m_speed_hack_count);
        }
        SET_ATTACK_TIME(attacker, victim, now);
    } else {
        SET_ATTACK_TIME(attacker, victim, now);
        const auto incoming = static_cast<std::uint32_t>(now - victim->m_AttackedLog.dwAttackedTime);
        if (victim->m_AttackedLog.dwPID != attacker->GetPlayerID() || incoming >= GET_ATTACK_SPEED(attacker)) {
            SET_ATTACKED_TIME(attacker, victim, now);
            return false;
        }

        ++attacker->m_speed_hack_count;
        if (test_server) {
            _sys_err("IS_SPEED_HACK", 0x361, "%s Attack Speed HACK! time (delta,limit) = (%lu,%lu), hack_count = %d",
                     attacker->GetName(), static_cast<unsigned long>(incoming), GET_ATTACK_SPEED(attacker),
                     attacker->m_speed_hack_count);
            attacker->ChatPacket(1, "Attack Speed Hack(%s), (delta,limit) = (%lu,%lu)",
                attacker->GetName(), static_cast<unsigned long>(incoming), GET_ATTACK_SPEED(attacker));
        }
    }
    SET_ATTACKED_TIME(attacker, victim, now);
    return true;
}
