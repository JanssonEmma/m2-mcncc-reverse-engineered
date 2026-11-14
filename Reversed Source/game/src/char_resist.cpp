#include <cstring>
#include <cstdio>
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
#include "locale_service.h"
#include "config.h"
#include "char_manager.h"
#include "char.h"
#include "constants.h"
#include "../../common/tables.h"

// 080640b0
void CHARACTER::ApplyMobAttribute(const TMobTable* table)
{
    for (unsigned int i = 0; i < sizeof(table->cEnchants); ++i) {
        const signed char value = static_cast<signed char>(table->cEnchants[i]);
        if (value)
            ApplyPoint(static_cast<BYTE>(aiMobEnchantApplyIdx[i]), value);
    }
    for (unsigned int i = 0; i < sizeof(table->cResists); ++i) {
        const signed char value = static_cast<signed char>(table->cResists[i]);
        if (value)
            ApplyPoint(static_cast<BYTE>(aiMobResistsApplyIdx[i]), value);
    }
}

// 08064030
void CHARACTER::RemoveFire()
{
    RemoveAffect(216);
    event_cancel(&m_pkFireEvent);
}

// 08064070
void CHARACTER::RemovePoison()
{
    RemoveAffect(209);
    event_cancel(&m_pkPoisonEvent);
}

namespace {
int ResistBits(unsigned int bits)
{
    int value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

const int poison_damage_rate[] = {80, 50, 40, 30, 25, 1};
int poison_level_adjust[] = {100, 90, 80, 70, 50, 30, 10, 5, 0};

struct TPoisonEventInfo {
    LPCHARACTER ch;
    int count;
    DWORD attacker_pid;
};

struct TFireEventInfo {
    LPCHARACTER ch;
    int count;
    int amount;
    DWORD attacker_pid;
};
}

// 08063b20
int GetPoisonDamageRate(LPCHARACTER ch)
{
    const int rate = ch->GetDesc() ? (LC_IsYMIR() ? 40 : 50) : poison_damage_rate[ch->GetMobRank()];
    return MAX(0, ResistBits(static_cast<unsigned int>(rate) - ch->GetPoint(81)));
}

long poison_event(LPEVENT event)
{
    TPoisonEventInfo* info = static_cast<TPoisonEventInfo*>(event->info);
    LPCHARACTER ch = info->ch;
    LPCHARACTER attacker = CHARACTER_MANAGER::instance().FindByPID(info->attacker_pid);
    const unsigned int maximumHP = ch->m_pointsInstant.iMaxHP;
    const int damage = ResistBits(maximumHP * GetPoisonDamageRate(ch)) / 1000;
    if (test_server)
        ch->ChatPacket(2, "Poison Damage %d", damage);
    if (ch->Damage(attacker, damage, DAMAGE_TYPE_POISON) ||
        (info->count = ResistBits(static_cast<unsigned int>(info->count) - 1)) == 0) {
        ch->m_pkPoisonEvent = 0;
        return 0;
    }
    return ResistBits(static_cast<unsigned int>(passes_per_sec) * 3);
}

long fire_event(LPEVENT event)
{
    TFireEventInfo* info = static_cast<TFireEventInfo*>(event->info);
    LPCHARACTER ch = info->ch;
    LPCHARACTER attacker = CHARACTER_MANAGER::instance().FindByPID(info->attacker_pid);
    const int damage = info->amount;
    if (test_server)
        ch->ChatPacket(2, "Fire Damage %d", damage);
    if (ch->Damage(attacker, damage, DAMAGE_TYPE_FIRE) ||
        (info->count = ResistBits(static_cast<unsigned int>(info->count) - 1)) == 0) {
        ch->m_pkFireEvent = 0;
        return 0;
    }
    return ResistBits(static_cast<unsigned int>(passes_per_sec) * 3);
}

// 08063da0
void CHARACTER::AttackedByFire(LPCHARACTER attacker, int amount, int count)
{
    if (m_pkFireEvent)
        return;
    AddAffect(216, 0, 0, 29, ResistBits(static_cast<unsigned int>(count) * 3 + 1), 0, true, false);
    void* rawInfo = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(TFireEventInfo), &rawInfo);
    auto* info = static_cast<TFireEventInfo*>(rawInfo);
    info->count = count;
    info->amount = amount;
    info->ch = this;
    info->attacker_pid = attacker->GetPlayerID();
    m_pkFireEvent = event_create_ex(fire_event, id, 1, const_cast<char*>("char_resist.cpp"), 143);
}

// 08063e70
void CHARACTER::AttackedByPoison(LPCHARACTER attacker)
{
    if (m_pkPoisonEvent || (m_bHasPoisoned && !GetDesc()))
        return;
    if (attacker) {
        const int attackerLevel = attacker->GetLevel();
        const int victimLevel = GetLevel();
        if (attackerLevel < victimLevel) {
            const int delta = MIN(8, victimLevel - attackerLevel);
            const int roll = number_ex(1, 100, "char_resist.cpp", 161);
            if (poison_level_adjust[delta] < roll)
                return;
        }
    }
    m_bHasPoisoned = true;
    AddAffect(209, 0, 0, 4, 31, 0, true, false);
    void* rawInfo = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(TPoisonEventInfo), &rawInfo);
    auto* info = static_cast<TPoisonEventInfo*>(rawInfo);
    info->ch = this;
    info->count = 10;
    info->attacker_pid = attacker ? attacker->GetPlayerID() : 0;
    m_pkPoisonEvent = event_create_ex(poison_event, id, 1, const_cast<char*>("char_resist.cpp"), 179);
    if (test_server && attacker) {
        char message[256];
        const char* victimName = GetName();
        const char* attackerName = attacker->GetName();
        std::sprintf(message, "POISON %s -> %s", attackerName, victimName);
        attacker->ChatPacket(1, "%s", message);
    }
}

// 08064150
bool CHARACTER::IsImmune(DWORD flag)
{
    if (!(m_pointsInstant.dwImmuneFlag & static_cast<std::uint32_t>(flag))) {
        if (test_server && GetDesc())
            ChatPacket(3, "<IMMUNE_FAIL> (%s) NO_IMMUNE_FLAG", GetName());
        return false;
    }
    const int roll = number_ex(1, 100, "char_resist.cpp", 221);
    const bool immune = roll <= (LC_IsJapan() ? 100 : 90);
    if (test_server && GetDesc())
        ChatPacket(3, immune ? "<IMMUNE_SUCCESS> (%s)" : "<IMMUNE_FAIL> (%s)", GetName());
    return immune;
}
