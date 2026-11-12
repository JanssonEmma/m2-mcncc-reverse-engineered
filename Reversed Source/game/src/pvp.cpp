#include "pvp.h"
#include "buffer_manager.h"
#include "char_manager.h"
#include "desc.h"
#include "desc_manager.h"
#include "locale.hpp"
#include "packet.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include "char.h"
#include "config.h"
#include "crc32.h"
#include "sectree_manager.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"

// 0819bfa0 / 0819c030
CPVP::CPVP(DWORD attacker, DWORD victim)
{
    for (int i = 0; i < 2; ++i) {
        m_players[i].dwPID = 0;
        m_players[i].dwVID = 0;
        m_players[i].bAgree = false;
        m_players[i].bCanRevenge = false;
    }
    if (victim < attacker) {
        m_players[0].dwPID = attacker;
        m_players[1].dwPID = victim;
        m_players[0].bAgree = true;
    } else {
        m_players[0].dwPID = victim;
        m_players[1].dwPID = attacker;
        m_players[1].bAgree = true;
    }
    const unsigned int ids[2] = {
        static_cast<unsigned int>(m_players[0].dwPID),
        static_cast<unsigned int>(m_players[1].dwPID)
    };
    m_dwCRC = get_fast_hash(reinterpret_cast<const char*>(ids), sizeof(ids));
    m_bRevenge = false;
    SetLastFightTime();
}

// 0819c1a0 / 0819c1b0: the original destructor has an empty body.
CPVP::~CPVP() {}

bool CPVP::IsFight()
{
    return m_players[0].bAgree == m_players[1].bAgree && m_players[0].bAgree;
}

void CPVP::SetLastFightTime()
{
    m_dwLastFightTime = get_dword_time();
}

// The original manager lifetime constructs and destroys both maps and the singleton.
// The maps hold pointer values and do not own the CPVP allocations at teardown.
CPVPManager::CPVPManager() {}
CPVPManager::~CPVPManager() {}

CPVP* CPVPManager::Find(DWORD crc)
{
    std::map<DWORD, CPVP*>::iterator it = m_map_pkPVP.find(crc);
    return it != m_map_pkPVP.end() ? it->second : 0;
}

// 0819d490
bool CPVPManager::CanAttack(CHARACTER* attacker, CHARACTER* victim)
{
    const BYTE victimType = victim->GetCharType();
    if (victimType == CHAR_TYPE_NPC || victimType == CHAR_TYPE_WARP || attacker == victim)
        return false;
    if (victimType != CHAR_TYPE_PC && attacker->GetCharType() != CHAR_TYPE_PC &&
        !attacker->IsGuardNPC())
        return false;
    if (attacker->GetMountVnum() && !attacker->IsHorseRiding())
        return false;
    if (attacker->GetHorseLevel() && attacker->GetHorseGrade() == 1 &&
        attacker->IsHorseRiding())
        return false;
    if (victim->GetCharType() != CHAR_TYPE_PC || attacker->GetCharType() != CHAR_TYPE_PC)
        return true;
    if (victim->IsObserverMode() || attacker->IsObserverMode())
        return false;
    if (attacker->GetPKMode() == PK_MODE_PROTECT &&
        SECTREE_MANAGER::instance().GetEmpireFromMapIndex(attacker->GetMapIndex()) == attacker->GetEmpire())
        return false;
    if (victim->GetPKMode() == PK_MODE_PROTECT &&
        SECTREE_MANAGER::instance().GetEmpireFromMapIndex(victim->GetMapIndex()) == victim->GetEmpire())
        return false;

    const BYTE victimEmpire = victim->GetEmpire();
    const bool differentEmpires = attacker->GetEmpire() != victimEmpire;
    if (test_server) {
        const char* victimName = victim->GetName();
        const BYTE loggedEmpire = victim->GetEmpire();
        const char* attackerName = attacker->GetName();
        sys_log(0, differentEmpires ? "Not same empire [%s %d] and [%s %d]" :
                                     "same empire [%s %d] and [%s %d]",
                attackerName, static_cast<unsigned int>(loggedEmpire), victimName,
                static_cast<unsigned int>(victimEmpire));
    }
    if (differentEmpires)
        return true;
    if (victim->GetParty() && victim->GetParty() == attacker->GetParty())
        return false;
    if (victim->IsKillerMode())
        return true;

    bool setKiller = false;
    const BYTE mode = attacker->GetPKMode();
    if (mode == PK_MODE_FREE ||
        (mode == PK_MODE_GUILD && (!attacker->GetGuild() || attacker->GetGuild() != victim->GetGuild()))) {
        if (victim->GetAlignment() < 0 &&
            (attacker->GetAlignment() >= 0 || victim->GetAlignment() >= 0))
            return true;
        setKiller = true;
    } else if (mode < PK_MODE_PROTECT &&
               (!victim->GetGuild() || victim->GetGuild() != attacker->GetGuild()) &&
               attacker->GetPKMode() == PK_MODE_REVENGE) {
        if (attacker->GetAlignment() < 0 && victim->GetAlignment() >= 0)
            setKiller = true;
        else if (attacker->GetAlignment() >= 0 && victim->GetAlignment() < 0)
            return true;
    }

    if (setKiller) {
        attacker->SetKillerMode(true);
        return true;
    }
    CPVP key(attacker->GetPlayerID(), victim->GetPlayerID());
    CPVP* pvp = Find(key.m_dwCRC);
    if (!pvp || !pvp->IsFight())
        return false;
    pvp->SetLastFightTime();
    return true;
}

// 0819c0c0 / 0819c130
CPVP::CPVP(CPVP& other)
    : m_dwCRC(other.m_dwCRC), m_bRevenge(other.m_bRevenge)
{
    m_players[0] = other.m_players[0];
    m_players[1] = other.m_players[1];
    SetLastFightTime();
}

// 0819c1c0
void CPVP::Packet(bool remove)
{
    TPacketGCPVP packet;
    packet.dwVIDSrc = m_players[0].dwVID;
    packet.dwVIDDst = m_players[1].dwVID;
    if (!packet.dwVIDSrc || !packet.dwVIDDst) {
        if (remove)
            _sys_err("Packet", 56, "null vid when removing %lu %lu", static_cast<unsigned long>(packet.dwVIDSrc), static_cast<unsigned long>(packet.dwVIDSrc));
        return;
    }
    packet.bHeader = 41;
    if (remove) {
        packet.bMode = 0;
    } else if (IsFight()) {
        packet.bMode = 2;
    } else {
        packet.bMode = m_bRevenge ? 3 : 1;
        if (!m_players[0].bAgree)
            std::swap(packet.dwVIDSrc, packet.dwVIDDst);
    }

    const DESC_MANAGER::DESC_SET& clients = DESC_MANAGER::instance().GetClientSet();
    DESC_MANAGER::DESC_SET::const_iterator it = clients.begin();
    while (it != clients.end()) {
        DESC* desc = *it++;
        if (desc->IsPhase(5) || desc->IsPhase(6))
            desc->Packet(&packet, sizeof(packet));
    }
}

// 0819c300
bool CPVP::Agree(DWORD pid)
{
    m_players[m_players[0].dwPID == pid ? 0 : 1].bAgree = true;
    const bool fighting = IsFight();
    if (fighting)
        Packet(false);
    return fighting;
}

// 0819c370
void CPVP::Win(DWORD pid)
{
    const int winner = m_players[0].dwPID == pid ? 0 : 1;
    const int loser = 1 - winner;
    m_bRevenge = true;
    m_players[winner].bAgree = true;
    m_players[loser].bCanRevenge = true;
    m_players[loser].bAgree = false;
    Packet(false);
}

bool CPVP::CanRevenge(DWORD pid)
{
    return m_players[m_players[0].dwPID == pid ? 0 : 1].bCanRevenge;
}

void CPVP::SetVID(DWORD pid, DWORD vid)
{
    m_players[m_players[0].dwPID == pid ? 0 : 1].dwVID = vid;
}

DWORD CPVP::GetLastFightTime()
{
    return m_dwLastFightTime;
}

// 0819c9b0
void CPVPManager::Insert(CHARACTER* attacker, CHARACTER* victim)
{
    if (attacker->IsDead() || victim->IsDead())
        return;
    CPVP key(attacker->GetPlayerID(), victim->GetPlayerID());
    CPVP* pvp = Find(key.m_dwCRC);
    if (pvp) {
        if (pvp->Agree(attacker->GetPlayerID())) {
            const char* name = attacker->GetName();
            victim->ChatPacket(1, locale_find("\045\163\264\324\260\372\300\307\040\264\353\260\341\040\275\303\300\333\041", g_iUseLocale), name);
            name = victim->GetName();
            attacker->ChatPacket(1, locale_find("\045\163\264\324\260\372\300\307\040\264\353\260\341\040\275\303\300\333\041", g_iUseLocale), name);
        }
        return;
    }

    pvp = new CPVP(key);
    pvp->SetVID(attacker->GetPlayerID(), attacker->GetVID());
    pvp->SetVID(victim->GetPlayerID(), victim->GetVID());
    m_map_pkPVP.insert(std::make_pair(pvp->m_dwCRC, pvp));
    m_map_pkPVPSetByID[attacker->GetPlayerID()].insert(pvp);
    m_map_pkPVPSetByID[victim->GetPlayerID()].insert(pvp);
    pvp->Packet(false);

    char message[86];
    const char* name = attacker->GetName();
    snprintf(message, 85, locale_find("\045\163\264\324\300\314\040\264\353\260\341\275\305\303\273\300\273\040\307\337\275\300\264\317\264\331\056\040\275\302\263\253\307\317\267\301\270\351\040\264\353\260\341\265\277\300\307\270\246\040\307\317\274\274\277\344\056", g_iUseLocale), name);
    victim->ChatPacket(1, message);
    name = victim->GetName();
    attacker->ChatPacket(1, locale_find("\045\163\277\241\260\324\040\264\353\260\341\275\305\303\273\300\273\040\307\337\275\300\264\317\264\331\056", g_iUseLocale), name);

    DESC* desc = victim->GetDesc();
    if (desc) {
        const int len = MIN(85, static_cast<int>(std::strlen(message)));
        TPacketGCWhisper packet;
        packet.wSize = static_cast<WORD>(sizeof(packet) + len);
        packet.bHeader = 34;
        packet.bType = 255;
        std::strncpy(packet.szNameFrom, attacker->GetName(), 24);
        packet.szNameFrom[24] = '\0';
        TEMP_BUFFER buffer;
        buffer.write(&packet, sizeof(packet));
        buffer.write(message, len);
        desc->Packet(buffer.read_peek(), buffer.size());
    }
}

// 0819d0b0
void CPVPManager::ConnectEx(CHARACTER* ch, bool disconnect)
{
    std::map<DWORD, std::set<CPVP*> >::iterator owner = m_map_pkPVPSetByID.find(ch->GetPlayerID());
    if (owner == m_map_pkPVPSetByID.end())
        return;
    const DWORD vid = disconnect ? 0 : static_cast<DWORD>(ch->GetVID());
    std::set<CPVP*>::iterator it = owner->second.begin();
    while (it != owner->second.end())
        (*it++)->SetVID(ch->GetPlayerID(), vid);
}

void CPVPManager::Connect(CHARACTER* ch)
{
    ConnectEx(ch, false);
}

// 0819d190 has an empty body.
void CPVPManager::Disconnect(CHARACTER*) {}

// 0819d390
bool CPVPManager::Dead(CHARACTER* ch, DWORD killerPID)
{
    std::map<DWORD, std::set<CPVP*> >::iterator owner = m_map_pkPVPSetByID.find(ch->GetPlayerID());
    if (owner == m_map_pkPVPSetByID.end())
        return false;
    sys_log(1, "PVPManager::Dead %d", ch->GetPlayerID());
    std::set<CPVP*>::iterator it = owner->second.begin();
    while (it != owner->second.end()) {
        CPVP* pvp = *it++;
        DWORD companion = pvp->m_players[0].dwPID;
        if (companion == ch->GetPlayerID())
            companion = pvp->m_players[1].dwPID;
        if (companion != killerPID)
            continue;
        if (pvp->IsFight()) {
            pvp->SetLastFightTime();
            pvp->Win(companion);
            return true;
        }
        const unsigned int now = static_cast<unsigned int>(get_dword_time());
        if (now - static_cast<unsigned int>(pvp->GetLastFightTime()) <= 15000u)
            return true;
    }
    return false;
}

// 0819d8f0
void CPVPManager::Delete(CPVP* pvp)
{
    std::map<DWORD, CPVP*>::iterator it = m_map_pkPVP.find(pvp->m_dwCRC);
    if (it == m_map_pkPVP.end())
        return;
    m_map_pkPVP.erase(it);
    m_map_pkPVPSetByID[pvp->m_players[0].dwPID].erase(pvp);
    m_map_pkPVPSetByID[pvp->m_players[1].dwPID].erase(pvp);
    delete pvp;
}

// 0819dbc0
void CPVPManager::SendList(DESC* desc)
{
    std::map<DWORD, CPVP*>::iterator it = m_map_pkPVP.begin();
    const DWORD vid = desc->GetCharacter()->GetVID();
    TPacketGCPVP packet;
    packet.bHeader = 41;
    while (it != m_map_pkPVP.end()) {
        CPVP* pvp = (it++)->second;
        if (!pvp->m_players[0].dwVID || !pvp->m_players[1].dwVID)
            continue;
        packet.dwVIDSrc = pvp->m_players[0].dwVID;
        packet.dwVIDDst = pvp->m_players[1].dwVID;
        if (pvp->IsFight()) {
            packet.bMode = 2;
        } else {
            packet.bMode = pvp->m_bRevenge ? 3 : 1;
            if (!pvp->m_players[0].bAgree)
                std::swap(packet.dwVIDSrc, packet.dwVIDDst);
        }
        desc->Packet(&packet, sizeof(packet));
        sys_log(1, "PVPManager::SendList %d %d", packet.dwVIDSrc, packet.dwVIDDst);
        DWORD otherVID = pvp->m_players[0].dwVID;
        if (otherVID == vid)
            otherVID = pvp->m_players[1].dwVID;
        else if (pvp->m_players[1].dwVID != vid)
            continue;
        CHARACTER* other = CHARACTER_MANAGER::instance().Find(otherVID);
        if (other && other->GetDesc())
            other->GetDesc()->Packet(&packet, sizeof(packet));
    }
}

// 0819dda0
void CPVPManager::Process()
{
    std::map<DWORD, CPVP*>::iterator it = m_map_pkPVP.begin();
    while (it != m_map_pkPVP.end()) {
        CPVP* pvp = (it++)->second;
        const unsigned int now = static_cast<unsigned int>(get_dword_time());
        if (now - static_cast<unsigned int>(pvp->GetLastFightTime()) > 600000u) {
            pvp->Packet(true);
            Delete(pvp);
        }
    }
}

// 0819d1a0
void CPVPManager::GiveUp(CHARACTER* ch, DWORD killerPID)
{
    std::map<DWORD, std::set<CPVP*> >::iterator owner = m_map_pkPVPSetByID.find(ch->GetPlayerID());
    if (owner == m_map_pkPVPSetByID.end())
        return;
    sys_log(1, "PVPManager::Dead %d", ch->GetPlayerID());
    std::set<CPVP*>::iterator it = owner->second.begin();
    while (it != owner->second.end()) {
        CPVP* pvp = *it++;
        DWORD companion = pvp->m_players[0].dwPID;
        if (companion == ch->GetPlayerID())
            companion = pvp->m_players[1].dwPID;
        if (companion != killerPID)
            continue;
        pvp->SetVID(ch->GetPlayerID(), 0);
        m_map_pkPVPSetByID.erase(companion);
        owner->second.erase(pvp);
        if (owner->second.empty())
            m_map_pkPVPSetByID.erase(owner);
        m_map_pkPVP.erase(pvp->m_dwCRC);
        pvp->Packet(true);
        delete pvp;
        return;
    }
}
