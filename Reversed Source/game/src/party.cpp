#include "utils.h"
// Staged game/src/party.cpp methods. Requires full party.h.candidate and packet
// additions, plus real BuildUpdatePartyPacket in char.cpp before runtime use.
#include "party.h"
#include "char_manager.h"
#include "config.h"
#include "dungeon.h"
#include "sectree_manager.h"
#include "p2p.h"
#include <utility>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "constants.h"
#include "locale.hpp"
#include "../../common/length.h"
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include "../../libthecore/include/utils.h"
#include "char.h"
#include "desc.h"
#include "desc_client.h"
#include "packet.h"
#include "../../common/tables.h"
#include "../../libthecore/include/log.h"

// 08141a50.
DWORD CParty::GetLeaderPID() { return m_dwLeaderPID; }

// 081424d0
void CParty::P2PSetMemberLevel(DWORD pid, BYTE level)
{
    if (!m_bPCParty)
        return;
    sys_log(0, "PARTY P2PSetMemberLevel leader %d pid %d level %d",
            static_cast<int>(GetLeaderPID()), static_cast<int>(pid), level);
    const auto member = m_memberMap.find(pid);
    if (member != m_memberMap.end())
        member->second.bLevel = level;
}

// 08142480: unconditional DB request. No membership, PC-party or null-client guard.
void CParty::RequestSetMemberLevel(DWORD pid,BYTE level) {
    TPacketPartySetMemberLevel p;
    p.dwLeaderPID=GetLeaderPID();p.dwPID=pid;p.bLevel=level;
    db_clientdesc->DBPacket(56,0,&p,sizeof(p));
}

// 08142c10 (CHARACTER* overload). Deliberately ignores the builder's bool result.
void CParty::SendPartyInfoOneToAll(CHARACTER* ch) {
    if(!ch->GetDesc()) return;
    TPacketGCPartyUpdate p;
    ch->BuildUpdatePartyPacket(p);
    for(TMemberMap::iterator it=m_memberMap.begin();it!=m_memberMap.end();++it) {
        CHARACTER* recipient=it->second.pCharacter;
        if(recipient && recipient->GetDesc()) {
            sys_log(2,"PARTY send info %s[%d] to %s[%d]",ch->GetName(),static_cast<DWORD>(ch->GetVID()),recipient->GetName(),static_cast<DWORD>(recipient->GetVID()));
            // Reload from iterator after sys_log, matching the target call path.
            it->second.pCharacter->GetDesc()->Packet(&p,sizeof(p));
        }
    }
}

bool CParty::IsMember(DWORD pid)
{
    return m_memberMap.find(pid) != m_memberMap.end();
}

// 08143200
bool CParty::IsRole(DWORD pid, BYTE role)
{
    const auto member = m_memberMap.find(pid);
    return member != m_memberMap.end() && member->second.bRole == role;
}

bool CParty::SetRole(DWORD pid, BYTE role, bool enabled)
{
    if (role >= 8)
        return false;
    const auto member = m_memberMap.find(pid);
    if (member == m_memberMap.end())
        return false;

    TMember& value = member->second;
    if (enabled) {
        if (m_anRoleCount[role] >= m_anMaxRole[role] || value.bRole != PARTY_ROLE_NORMAL)
            return false;
        value.bRole = role;
        if (value.pCharacter && m_pkChrLeader)
            ComputeRolePoint(value.pCharacter, role, true);
        ++m_anRoleCount[role];
    } else {
        if (value.bRole == PARTY_ROLE_LEADER || value.bRole == PARTY_ROLE_NORMAL)
            return false;
        value.bRole = PARTY_ROLE_NORMAL;
        if (value.pCharacter && m_pkChrLeader)
            ComputeRolePoint(value.pCharacter, PARTY_ROLE_NORMAL, false);
        --m_anRoleCount[role];
    }
    SendPartyInfoOneToAll(pid);
    return true;
}

// 08142b20
void CParty::SendPartyInfoOneToAll(DWORD pid)
{
    const auto member = m_memberMap.find(pid);
    if (member == m_memberMap.end())
        return;
    if (CHARACTER* character = member->second.pCharacter) {
        SendPartyInfoOneToAll(character);
        return;
    }

    TPacketGCPartyUpdate packet{};
    packet.header = 0x4f;
    packet.pid = pid;
    packet.role = member->second.bRole;
    packet.percent_hp = 255;
    for (const auto& entry : m_memberMap) {
        if (entry.second.pCharacter) {
            if (DESC* desc = entry.second.pCharacter->GetDesc())
                desc->Packet(&packet, sizeof(packet));
        }
    }
}

// 081434a0
void CParty::HealParty()
{
}

// 081434b0
void CParty::SummonToLeader(DWORD pid)
{
    static constexpr int offsets[][2] = {
        {250, 0}, {216, 125}, {125, 216}, {0, 250}, {-125, 216}, {-216, 125},
        {-250, 0}, {-216, -125}, {-125, -216}, {0, -250}, {125, -216}, {216, -125},
    };
    SECTREE_MANAGER& sectrees = SECTREE_MANAGER::instance();
    CHARACTER* leader = GetLeaderCharacter();
    if (!leader)
        return;
    const auto member = m_memberMap.find(pid);
    CHARACTER* target = member == m_memberMap.end() ? nullptr : member->second.pCharacter;
    if (!target) {
        leader->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \274\322\310\257\307\317\267\301\264\302 \264\353\273\363\300\273 \303\243\300\273 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }
    if (!target->CanSummon(m_iLeadership)) {
        leader->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \264\353\273\363\300\273 \274\322\310\257\307\322 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }

    PIXEL_POSITION candidates[12];
    int count = 0;
    const long map = leader->GetMapIndex();
    for (const auto& offset : offsets) {
        const auto x = static_cast<std::int32_t>(static_cast<std::uint32_t>(leader->GetX()) + offset[0]);
        const auto y = static_cast<std::int32_t>(static_cast<std::uint32_t>(leader->GetY()) + offset[1]);
        if (sectrees.GetMovablePosition(map, x, y, candidates[count]))
            ++count;
    }
    if (count == 0) {
        leader->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \306\304\306\274\277\370\300\273 \307\366\300\347 \300\247\304\241\267\316 \274\322\310\257\307\322 \274\366 \276\370\275\300\264\317\264\331."));
        return;
    }
    char source[] = "party.cpp";
    const PIXEL_POSITION& position = candidates[number_ex(0, count - 1, source, 0x46f)];
    target->Show(map, position.x, position.y, 0x7fffffff, false);
    target->Stop();
}

// 08143310. Absent member returns role0 without insertion.
BYTE CParty::GetRole(DWORD pid) {
    TMemberMap::iterator it=m_memberMap.find(pid);
    return it==m_memberMap.end()?0:it->second.bRole;
}
// 081430f0. Preserve operator[] insertion for a missing leader; do not harden.
CHARACTER* CParty::GetLeaderCharacter() { return m_memberMap[GetLeaderPID()].pCharacter; }
// 08144620. Unsigned comparison is observable for negative near-member counts.
int CParty::GetExpBonusPercent() {
    if(static_cast<unsigned int>(m_iCountNearPartyMember)<2U) return 0;
    return static_cast<int>(static_cast<unsigned int>(m_iLongTimeExpBonus)+static_cast<unsigned int>(m_iExpBonus));
}
// Inline callsite in08133e50 reads m_iExpBonus directly.
int CParty::GetPartyBonusExpPercent() { return m_iExpBonus; }

// Append to the staged real party.cpp. STL members are constructed normally
// before entering this body, matching the original implicit map construction.
//08141280/08141330.
CParty::CParty() { Initialize(); }
//081416b0. Preserve two separate clock calls and do not clear either map.
void CParty::Initialize() {
    sys_log(2,"Party::Initialize");
    m_iExpDistributionMode=0;m_pkChrExpCentralize=0;m_dwLeaderPID=0;m_eventUpdate=0;
    for(int i=0;i<8;++i)m_anRoleCount[i]=0;
    for(int i=0;i<8;++i)m_anMaxRole[i]=0;
    m_anMaxRole[1]=1;m_anMaxRole[0]=32;
    m_dwPartyStartTime=get_dword_time();m_iLongTimeExpBonus=0;
    m_dwPartyHealTime=get_dword_time();m_iAttBonus=0;m_iDefBonus=0;
    m_bPartyHealReady=false;m_bCanUsePartyHeal=false;m_iLeadership=0;m_iExpBonus=0;
    m_itNextOwner=m_memberMap.begin();m_iCountNearPartyMember=0;m_pkChrLeader=0;
    m_bPCParty=false;m_pkDungeon=0;
}

namespace {
void BroadcastPartyAdd(CParty::TMemberMap& members, TPacketGCPartyAdd& packet)
{
    for (CParty::TMemberMap::iterator it = members.begin(); it != members.end(); ++it) {
        CHARACTER* member = it->second.pCharacter;
        if (member) {
            DESC* desc = member->GetDesc();
            if (desc)
                desc->Packet(&packet, sizeof(packet));
        }
    }
}

CParty::TMember& PartyMemberForAdd(CParty::TMemberMap& members, DWORD pid)
{
    CParty::TMemberMap::iterator it = members.lower_bound(pid);
    if (it == members.end() || pid < it->first) {
        // The old map insertion default-initializes the string and leaves the scalar fields indeterminate.
        CParty::TMember member;
        it = members.insert(it, CParty::TMemberMap::value_type(pid, member));
    }
    return it->second;
}
}

// 08144170
void CParty::UpdateOnlineState(DWORD pid, const char* name)
{
    TMember& member = PartyMemberForAdd(m_memberMap, pid);
    TPacketGCPartyAdd packet;
    packet.header = 78;
    packet.pid = pid;
    member.strName.assign(name);
    std::strncpy(packet.name, name, sizeof(packet.name) - 1);
    packet.name[sizeof(packet.name) - 1] = 0;
    BroadcastPartyAdd(m_memberMap, packet);
}

// 08144350
void CParty::UpdateOfflineState(DWORD pid)
{
    TPacketGCPartyAdd packet;
    packet.header = 78;
    packet.pid = pid;
    std::memset(packet.name, 0, sizeof(packet.name));
    BroadcastPartyAdd(m_memberMap, packet);
}

// 08142700
void CParty::SendPartyJoinOneToAll(DWORD pid)
{
    TMember& member = PartyMemberForAdd(m_memberMap, pid);
    TPacketGCPartyAdd packet;
    packet.header = 78;
    packet.pid = pid;
    std::strncpy(packet.name, member.strName.c_str(), sizeof(packet.name) - 1);
    packet.name[sizeof(packet.name) - 1] = 0;
    BroadcastPartyAdd(m_memberMap, packet);
}

CPartyManager::CPartyManager() { Initialize(); }

// 081406e0: native container destruction only. The stored party pointers are not deleted here.
CPartyManager::~CPartyManager() {}

void CPartyManager::Initialize()
{
    m_bEnablePCParty = false;
}

// 08140ce0
void CPartyManager::P2PJoinParty(DWORD leader, DWORD pid, BYTE role)
{
    const auto entry = m_map_pkParty.find(leader);
    if (entry == m_map_pkParty.end()) {
        _sys_err("P2PJoinParty", 0x66, "No such party with leader [%d]", static_cast<int>(leader));
        return;
    }
    CParty* party = entry->second;
    party->P2PJoin(pid);
    party->SetRole(pid, role <= 7 ? role : static_cast<BYTE>(PARTY_ROLE_NORMAL), true);
}

// 08140da0
void CPartyManager::P2PQuitParty(DWORD pid)
{
    const auto entry = m_map_pkParty.find(pid);
    if (entry == m_map_pkParty.end()) {
        _sys_err("P2PQuitParty", 0x74, "No such party with member [%d]", static_cast<int>(pid));
        return;
    }
    entry->second->P2PQuit(pid);
}

// 08140e20
CParty* CPartyManager::P2PCreateParty(DWORD pid)
{
    const auto entry = m_map_pkParty.find(pid);
    if (entry != m_map_pkParty.end())
        return entry->second;
    CParty* party = new CParty;
    m_set_pkPCParty.insert(party);
    SetPartyMember(pid, party);
    party->SetPCParty(true);
    party->P2PJoin(pid);
    return party;
}

// 08140c20
void CPartyManager::P2PLogin(DWORD pid, const char* name)
{
    TPartyMap::iterator it = m_map_pkParty.find(pid);
    if (it != m_map_pkParty.end())
        it->second->UpdateOnlineState(pid, name);
}

// 08140c80
void CPartyManager::P2PLogout(DWORD pid)
{
    TPartyMap::iterator it = m_map_pkParty.find(pid);
    if (it != m_map_pkParty.end())
        it->second->UpdateOfflineState(pid);
}

namespace {
int PartyCoordinateDelta(long coordinate, long origin)
{
    const unsigned int bits = static_cast<unsigned int>(coordinate) - static_cast<unsigned int>(origin);
    int result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}
}

// 08143810
void CParty::IncreaseOwnership()
{
    if (!m_memberMap.empty() && m_itNextOwner != m_memberMap.end()) {
        ++m_itNextOwner;
        if (m_itNextOwner != m_memberMap.end())
            return;
    }
    m_itNextOwner = m_memberMap.begin();
}

// 08143870
CHARACTER* CParty::GetNextOwnership(CHARACTER* fallback, long x, long y)
{
    if (m_itNextOwner == m_memberMap.end())
        return fallback;
    int remaining = static_cast<int>(m_memberMap.size());
    while (remaining-- > 0) {
        CHARACTER* member = m_itNextOwner->second.pCharacter;
        if (member && DISTANCE_APPROX(PartyCoordinateDelta(member->GetX(), x),
                                      PartyCoordinateDelta(member->GetY(), y)) < 3000) {
            IncreaseOwnership();
            return member;
        }
        IncreaseOwnership();
    }
    return fallback;
}

void CParty::SetDungeon(CDungeon* dungeon)
{
    m_pkDungeon = dungeon;
    m_map_iFlag.clear();
}

EVENTFUNC(party_update_event)
{
    const auto pid = *static_cast<std::uint32_t*>(event->info);
    CHARACTER* leader = CHARACTER_MANAGER::instance().FindByPID(pid);
    if (leader && leader->GetDesc() && leader->GetParty())
        leader->GetParty()->Update();
    return passes_per_sec * 3;
}

CParty* CPartyManager::CreateParty(CHARACTER* leader)
{
    if (CParty* party = leader->GetParty())
        return party;

    CParty* party = new CParty;
    if (leader->GetDesc()) {
        TPacketPartyCreate packet = { static_cast<std::uint32_t>(leader->GetPlayerID()) };
        db_clientdesc->DBPacket(36, 0, &packet, sizeof(packet));
        sys_log(0, "PARTY: Create %s pid %lu", leader->GetName(), leader->GetPlayerID());
        party->SetPCParty(true);
        party->Join(leader->GetPlayerID());
        m_set_pkPCParty.insert(party);
    } else {
        party->SetPCParty(false);
        party->Join(static_cast<DWORD>(leader->GetVID()));
    }
    party->Link(leader);
    return party;
}

bool CPartyManager::SetParty(CHARACTER* character)
{
    TPartyMap::iterator it = m_map_pkParty.find(character->GetPlayerID());
    if (it == m_map_pkParty.end())
        return false;
    it->second->Link(character);
    return true;
}

void CPartyManager::SetPartyMember(DWORD pid, CParty* party)
{
    TPartyMap::iterator it = m_map_pkParty.find(pid);
    if (!party) {
        if (it != m_map_pkParty.end())
            m_map_pkParty.erase(it);
    } else if (it == m_map_pkParty.end()) {
        m_map_pkParty.insert(std::make_pair(pid, party));
    } else if (it->second != party) {
        it->second->Quit(pid);
        m_map_pkParty[pid] = party;
    }
}

void CParty::SetPCParty(bool isPC) { m_bPCParty = isPC; }
DWORD CParty::GetMemberCount() { return m_memberMap.size(); }
CDungeon* CParty::GetDungeon() { return m_pkDungeon; }

void CParty::P2PJoin(DWORD pid)
{
    if (m_memberMap.find(pid) == m_memberMap.end()) {
        TMember member; // The original leaves bLevel unset until a level update.
        member.pCharacter = 0;
        member.bNear = false;
        member.bRole = m_memberMap.empty() ? PARTY_ROLE_LEADER : PARTY_ROLE_NORMAL;
        if (member.bRole == PARTY_ROLE_LEADER)
            m_dwLeaderPID = pid;

        if (m_bPCParty) {
            CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid);
            if (character) {
                sys_log(0, "PARTY: Join %s pid %lu leader %lu", character->GetName(), pid, m_dwLeaderPID);
                member.strName = character->GetName();
                if (member.bRole == PARTY_ROLE_LEADER)
                    m_iLeadership = character->GetLeadershipSkillLevel();
            } else if (CCI* remote = P2P_MANAGER::instance().FindByPID(pid)) {
                if (remote->bChannel == g_bChannel)
                    member.strName = remote->szName;
                else
                    _sys_err("P2PJoin", 0x19e,
                             "member is not in same channel PID: %lu channel %d, this channel %d",
                             pid, remote->bChannel, g_bChannel);
            }
        }

        const int count = GetMemberCount();
        sys_log(2, "PARTY[%d] MemberCountChange %d -> %d", static_cast<int>(GetLeaderPID()), count, count + 1);
        m_memberMap.insert(TMemberMap::value_type(pid, member));
        if (m_memberMap.size() == 1)
            m_itNextOwner = m_memberMap.begin();

        if (m_bPCParty) {
            CPartyManager::instance().SetPartyMember(pid, this);
            SendPartyJoinOneToAll(pid);
            if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid))
                SendParameter(character);
        }
    }
    if (m_pkDungeon)
        m_pkDungeon->QuitParty(this);
}

void CParty::Join(DWORD pid)
{
    P2PJoin(pid);
    if (m_bPCParty) {
        TPacketPartyAdd packet{};
        packet.dwLeaderPID = GetLeaderPID();
        packet.dwPID = pid;
        db_clientdesc->DBPacket(38, 0, &packet, sizeof(packet));
    }
}

void CParty::Link(CHARACTER* character)
{
    const DWORD key = character->GetDesc() ? character->GetPlayerID() : static_cast<DWORD>(character->GetVID());
    TMemberMap::iterator it = m_memberMap.find(key);
    if (it == m_memberMap.end()) {
        _sys_err("Link", 0x218, "%s is not member of this party", character->GetName());
        return;
    }

    if (m_bPCParty && !m_eventUpdate) {
        void* data;
        const DWORD infoId = __AllocEventInfo(sizeof(std::uint32_t), &data);
        *static_cast<std::uint32_t*>(data) = m_dwLeaderPID;
        m_eventUpdate = event_create_ex(party_update_event, infoId, passes_per_sec * 3,
                                        const_cast<char*>("party.cpp"), 0x222);
    }
    if (it->second.bRole == PARTY_ROLE_LEADER)
        m_pkChrLeader = character;
    sys_log(2, "PARTY[%d] %s linked to party", static_cast<int>(GetLeaderPID()), character->GetName());
    it->second.pCharacter = character;
    character->SetParty(this);

    if (character->GetDesc()) {
        if (it->second.strName.empty())
            it->second.strName = character->GetName();
        SendPartyJoinOneToAll(character->GetPlayerID());
        SendPartyJoinAllToOne(character);
        SendPartyLinkOneToAll(character);
        SendPartyLinkAllToOne(character);
        SendPartyInfoAllToOne(character);
        SendPartyInfoOneToAll(character);
        SendParameter(character);
        if (m_pkDungeon && m_pkDungeon->GetMapIndex() == character->GetMapIndex())
            character->SetDungeon(m_pkDungeon);
        RequestSetMemberLevel(character->GetPlayerID(), character->GetLevel());
    }
}

// 081447e0
void CParty::SetParameter(int mode)
{
    if (mode >= 2) {
        _sys_err("SetParameter", 0x620, "Invalid exp distribution mode %d", mode);
        return;
    }
    m_iExpDistributionMode = mode;
    SendParameterToAll();
}

void CParty::SendParameterToAll()
{
    if (!m_bPCParty)
        return;
    for (const auto& entry : m_memberMap) {
        if (entry.second.pCharacter)
            SendParameter(entry.second.pCharacter);
    }
}

void CParty::SendParameter(CHARACTER* character)
{
    const TPacketGCPartyParameter packet = { 83, static_cast<BYTE>(m_iExpDistributionMode) };
    if (DESC* desc = character->GetDesc())
        desc->Packet(&packet, sizeof(packet));
}

void CParty::SendPartyJoinAllToOne(CHARACTER* character)
{
    if (!character->GetDesc())
        return;
    TPacketGCPartyAdd packet;
    packet.header = 78;
    for (TMemberMap::iterator it = m_memberMap.begin(); it != m_memberMap.end(); ++it) {
        packet.pid = it->first;
        std::strncpy(packet.name, it->second.strName.c_str(), sizeof(packet.name) - 1);
        packet.name[sizeof(packet.name) - 1] = 0;
        character->GetDesc()->Packet(&packet, sizeof(packet));
    }
}

void CParty::SendPartyLinkOneToAll(CHARACTER* character)
{
    if (!character->GetDesc())
        return;
    const TPacketGCPartyLink packet = { 91, static_cast<std::uint32_t>(character->GetPlayerID()),
        static_cast<std::uint32_t>(static_cast<DWORD>(character->GetVID())) };
    for (TMemberMap::iterator it = m_memberMap.begin(); it != m_memberMap.end(); ++it) {
        CHARACTER* member = it->second.pCharacter;
        if (member && member->GetDesc())
            member->GetDesc()->Packet(&packet, sizeof(packet));
    }
}

void CParty::SendPartyLinkAllToOne(CHARACTER* character)
{
    if (!character->GetDesc())
        return;
    for (TMemberMap::iterator it = m_memberMap.begin(); it != m_memberMap.end(); ++it) {
        if (CHARACTER* member = it->second.pCharacter) {
            const TPacketGCPartyLink packet = { 91, static_cast<std::uint32_t>(member->GetPlayerID()),
        static_cast<std::uint32_t>(static_cast<DWORD>(member->GetVID())) };
            character->GetDesc()->Packet(&packet, sizeof(packet));
        }
    }
}

void CParty::SendPartyInfoAllToOne(CHARACTER* character)
{
    TPacketGCPartyUpdate packet;
    for (TMemberMap::iterator it = m_memberMap.begin(); it != m_memberMap.end(); ++it) {
        if (CHARACTER* member = it->second.pCharacter) {
            member->BuildUpdatePartyPacket(packet);
            sys_log(2, "PARTY send info %s[%d] to %s[%d]", member->GetName(), static_cast<DWORD>(member->GetVID()),
                    character->GetName(), static_cast<DWORD>(character->GetVID()));
        } else {
            packet = {};
            packet.header = 79;
            packet.percent_hp = 0xff;
            packet.role = it->second.bRole;
            packet.pid = it->first;
        }
        character->GetDesc()->Packet(&packet, sizeof(packet));
    }
}

namespace {
template<class Packet>
void BroadcastPartyPacket(CParty::TMemberMap& members, const Packet& packet)
{
    for (CParty::TMemberMap::iterator it = members.begin(); it != members.end(); ++it) {
        if (CHARACTER* character = it->second.pCharacter) {
            if (DESC* desc = character->GetDesc())
                desc->Packet(&packet, sizeof(packet));
        }
    }
}
}

CParty::~CParty()
{
    Destroy();
}

void CParty::Destroy()
{
    sys_log(2, "Party::Destroy");
    if (m_bPCParty) {
        for (TMemberMap::iterator it = m_memberMap.begin(); it != m_memberMap.end(); ++it)
            CPartyManager::instance().SetPartyMember(it->first, 0);
    }

    event_cancel(&m_eventUpdate);
    RemoveBonus();
    const DWORD time = get_dword_time();
    for (TMemberMap::iterator it = m_memberMap.begin(); it != m_memberMap.end(); ++it) {
        CHARACTER* character = it->second.pCharacter;
        if (!character)
            continue;

        if (DESC* desc = character->GetDesc()) {
            const TPacketGCPartyRemove packet = { 80, static_cast<std::uint32_t>(character->GetPlayerID()) };
            desc->Packet(&packet, sizeof(packet));
            character->ChatPacket(1, LC_TEXT("<\306\304\306\274> \306\304\306\274\260\241 \307\330\273\352 \265\307\276\372\275\300\264\317\264\331."));
        } else if (character->GetCharType() != CHAR_TYPE_PC) {
            character->SetLastAttacked(time);
        }
        character->SetParty(0);
    }
    m_memberMap.clear();
}

void CPartyManager::DeleteParty(CParty* party)
{
    const TPacketPartyDelete packet = { static_cast<std::uint32_t>(party->GetLeaderPID()) };
    db_clientdesc->DBPacket(37, 0, &packet, sizeof(packet));
    m_set_pkPCParty.erase(party);
    delete party;
}

// 08140ee0
void CPartyManager::P2PDeleteParty(DWORD pid)
{
    TPartyMap::iterator it = m_map_pkParty.find(pid);
    if (it == m_map_pkParty.end()) {
        _sys_err("P2PDeleteParty", 0x93, "PARTY P2PDeleteParty Cannot find party [%lu]", pid);
        return;
    }
    CParty* party = it->second;
    m_set_pkPCParty.erase(party);
    delete party;
}

void CPartyManager::DeleteAllParty()
{
    TPCPartySet::iterator it = m_set_pkPCParty.begin();
    while (it != m_set_pkPCParty.end())
        DeleteParty(*it++);
    m_set_pkPCParty.clear();
}

void CParty::P2PQuit(DWORD pid)
{
    TMemberMap::iterator it = m_memberMap.find(pid);
    if (it == m_memberMap.end())
        return;

    if (m_bPCParty)
        SendPartyRemoveOneToAll(pid);
    if (it == m_itNextOwner)
        IncreaseOwnership();

    // Retain the member fields that the original reads after freeing the map node.
    CHARACTER* member = it->second.pCharacter;
    const BYTE role = it->second.bRole;
    m_memberMap.erase(it);

    if (m_bPCParty)
        RemoveBonusForOne(pid);
    if (member)
        member->SetParty(0);

    const int count = GetMemberCount();
    sys_log(2, "PARTY[%d] MemberCountChange %d -> %d", static_cast<int>(GetLeaderPID()), count, count - 1);
    --m_anRoleCount[role];
    if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid))
        ComputeRolePoint(character, role, false);
    if (m_bPCParty)
        CPartyManager::instance().SetPartyMember(pid, 0);
    if (role == PARTY_ROLE_LEADER)
        CPartyManager::instance().DeleteParty(this);
}

void CParty::Quit(DWORD pid)
{
    // P2PQuit can delete the party when the leader leaves.
    const bool pcParty = m_bPCParty;
    const DWORD leader = m_dwLeaderPID;
    P2PQuit(pid);
    if (pcParty && pid != leader) {
        const TPacketPartyRemove packet = { static_cast<std::uint32_t>(leader), static_cast<std::uint32_t>(pid) };
        db_clientdesc->DBPacket(39, 0, &packet, sizeof(packet));
    }
}

void CParty::Unlink(CHARACTER* character)
{
    const DWORD key = character->GetDesc() ? character->GetPlayerID() : static_cast<DWORD>(character->GetVID());
    TMemberMap::iterator it = m_memberMap.find(key);
    if (it == m_memberMap.end()) {
        _sys_err("Unlink", 0x278, "%s is not member of this party", character->GetName());
        return;
    }

    const bool leader = it->second.bRole == PARTY_ROLE_LEADER;
    if (character->GetDesc()) {
        SendPartyUnlinkOneToAll(character);
        if (leader) {
            RemoveBonus();
            if (it->second.pCharacter->GetDungeon()) {
                for (TMemberMap::iterator member = m_memberMap.begin(); member != m_memberMap.end(); ++member) {
                    if (member->second.bNear && member->second.pCharacter)
                        member->second.pCharacter->ExitToSavedLocation();
                }
            }
        }
    }
    if (leader)
        m_pkChrLeader = 0;
    it->second.pCharacter = 0;
    character->SetParty(0);
}

void CParty::SendPartyUnlinkOneToAll(CHARACTER* character)
{
    if (!character->GetDesc())
        return;
    const TPacketGCPartyLink packet = { 92, static_cast<std::uint32_t>(character->GetPlayerID()),
        static_cast<std::uint32_t>(static_cast<DWORD>(character->GetVID())) };
    BroadcastPartyPacket(m_memberMap, packet);
}

void CParty::SendPartyRemoveOneToAll(DWORD pid)
{
    const TPacketGCPartyRemove packet = { 80, static_cast<std::uint32_t>(pid) };
    BroadcastPartyPacket(m_memberMap, packet);
}

void CParty::RemoveBonus()
{
    for (TMemberMap::iterator it = m_memberMap.begin(); it != m_memberMap.end(); ++it) {
        if (CHARACTER* character = it->second.pCharacter)
            ComputeRolePoint(character, it->second.bRole, false);
        it->second.bNear = false;
    }
}

void CParty::RemoveBonusForOne(DWORD pid)
{
    TMemberMap::iterator it = m_memberMap.find(pid);
    if (it != m_memberMap.end() && it->second.pCharacter)
        ComputeRolePoint(it->second.pCharacter, it->second.bRole, false);
}

void CParty::ComputeRolePoint(CHARACTER* character, BYTE role, bool add)
{
    if (!add) {
        const BYTE points[] = { 91, 92, 103, 104, 111, 110 };
        for (unsigned i = 0; i < sizeof(points); ++i)
            character->PointChange(points[i], -character->GetPoint(points[i]), false, false);
        character->ComputePoints();
        return;
    }

    const int level = std::max(0, std::min(m_iLeadership, 40));
    const long double power = CTableBySkill::instance().GetSkillPowerByLevelFromType(
        character->GetJob(), character->GetSkillGroup(), level, false) / 100.0L;
    BYTE point;
    long double value;
    switch (role) {
        case PARTY_ROLE_ATTACKER: point = 91; value = power * 60.0f + 10.0f; break;
        case PARTY_ROLE_TANKER: point = 92; value = power * 1450.0f + 50.0f; break;
        case PARTY_ROLE_BUFFER: point = 103; value = power * 45.0f + 5.0f; break;
        case PARTY_ROLE_SKILL_MASTER: point = 104; value = power * 600.0f + 25.0f; break;
        case PARTY_ROLE_HASTE: point = 110; value = power * 5.0f + 1.0f; break;
        case PARTY_ROLE_DEFENDER: point = 111; value = power * 30.0f + 5.0f; break;
        default: return;
    }
    const int target = static_cast<int>(value);
    if (character->GetPoint(point) != target) {
        character->PointChange(point, target - character->GetPoint(point), false, false);
        if (role != PARTY_ROLE_BUFFER)
            character->ComputePoints();
    }
}

bool CPartyManager::IsEnablePCParty()
{
    return m_bEnablePCParty;
}

void CPartyManager::EnablePCParty()
{
    m_bEnablePCParty = true;
    sys_log(0, "PARTY Enable");
}

void CPartyManager::DisablePCParty()
{
    m_bEnablePCParty = false;
    sys_log(0, "PARTY Disable");
}

// 081445d0: bonus eligibility is a strict distance check to the linked leader.
void CParty::SendMessage(CHARACTER* sender, BYTE message, DWORD first, DWORD second)
{
    if (sender->GetParty() != this) {
        _sys_err("SendMessage", 0x355, "%s is not member of this party %p", sender->GetName(), static_cast<void*>(this));
        return;
    }
    if (message == 1) {
        ForEachOnlineMember([&](CHARACTER* member) {
            if (member == sender)
                return;
            char file[] = "party.cpp";
            const auto x = static_cast<std::int32_t>(first + number_ex(-500, 500, file, 0x36b));
            const auto y = static_cast<std::int32_t>(second + number_ex(-500, 500, file, 0x36c));
            member->SetVictim(nullptr);
            member->SetRotationToXY(x, y);
            if (member->Goto(x, y)) {
                sys_log(0, "%s %p RETURN victim %p", member->GetName(), static_cast<void*>(member),
                        static_cast<void*>(member->GetVictim()));
                member->SendMovePacket(0, 0, 0, 0, 0, 0, -1);
            }
        });
    } else if (message == 2) {
        if (CHARACTER* victim = sender->GetVictim()) {
            ForEachOnlineMember([&](CHARACTER* member) {
                if (member != sender && member->CanBeginFight())
                    member->BeginFight(victim);
            });
        }
    } else if (message == 3) {
        if (CHARACTER* victim = CHARACTER_MANAGER::instance().Find(second)) {
            ForEachOnlineMember([&](CHARACTER* member) {
                if (member != sender)
                    member->UpdateAggrPoint(victim, DAMAGE_TYPE_SPECIAL, static_cast<int>(first));
            });
        }
    }
}

bool CParty::IsNearLeader(DWORD pid)
{
    const auto member = m_memberMap.find(pid);
    return member != m_memberMap.end() && member->second.bNear;
}

bool CParty::IsPositionNearLeader(CHARACTER* character)
{
    return m_pkChrLeader &&
           DISTANCE_APPROX(PartyCoordinateDelta(character->GetX(), m_pkChrLeader->GetX()),
                           PartyCoordinateDelta(character->GetY(), m_pkChrLeader->GetY())) < 5000;
}

DWORD CParty::GetNearMemberCount()
{
    return m_iCountNearPartyMember;
}

// 08144920
BYTE CParty::GetMemberMinLevel()
{
    BYTE level = 99;
    for (const auto& entry : m_memberMap) {
        if (entry.second.bLevel)
            level = std::min(level, entry.second.bLevel);
    }
    return level;
}

BYTE CParty::GetMemberMaxLevel()
{
    BYTE level = 0;
    for (const auto& entry : m_memberMap)
        level = std::max(level, entry.second.bLevel);
    return level;
}

int CParty::GetFlag(const std::string& name)
{
    const auto flag = m_map_iFlag.find(name);
    return flag == m_map_iFlag.end() ? 0 : flag->second;
}

void CParty::SetFlag(const std::string& name, int value)
{
    m_map_iFlag[name] = value;
}

int CParty::ComputePartyBonusExpPercent()
{
    if (static_cast<unsigned>(m_iCountNearPartyMember) < 2)
        return 0;
    const int count = std::min(8, m_iCountNearPartyMember);
    CHARACTER* leader = GetLeaderCharacter();
    const bool unique = leader && (leader->IsEquipUniqueItem(0x11173) || leader->IsEquipUniqueItem(0x11564));
    return g_iUseLocale == 0 ? KOR_aiPartyBonusExpPercentByMemberCount[count] + (unique ? KOR_aiUniqueItemPartyBonusExpPercentByMemberCount[count] : 0)
                            : CHN_aiPartyBonusExpPercentByMemberCount[count] + (unique ? 30 : 0);
}

// 0x08143df0
void CParty::Update()
{
    sys_log(1, "PARTY::Update");
    CHARACTER* leader = GetLeaderCharacter();
    if (!leader)
        return;

    CDungeon* dungeon = leader->GetDungeon();
    int nearCount = 0;
    for (auto& entry : m_memberMap) {
        TMember& member = entry.second;
        CHARACTER* character = member.pCharacter;
        member.bNear = character && (dungeon ? character->GetDungeon() == dungeon :
            DISTANCE_APPROX(PartyCoordinateDelta(leader->GetX(), character->GetX()),
                            PartyCoordinateDelta(leader->GetY(), character->GetY())) < 5000);
        nearCount += member.bNear;
    }
    if (nearCount < 2 && !dungeon) {
        for (auto& entry : m_memberMap)
            entry.second.bNear = false;
        nearCount = 0;
    }

    const bool nearChanged = m_iCountNearPartyMember != nearCount;
    m_iCountNearPartyMember = nearCount;
    m_iLeadership = leader->GetLeadershipSkillLevel();
    const int expBonus = ComputePartyBonusExpPercent();
    bool resendAll = nearChanged || m_iExpBonus != expBonus;
    m_iExpBonus = expBonus;
    m_iAttBonus = 0;
    m_iDefBonus = 0;

    const bool gainedTimeBonus = !m_iLongTimeExpBonus &&
        static_cast<unsigned>(get_dword_time() - m_dwPartyStartTime) > (g_iUseLocale ? 3600000U : 1800000U);
    if (gainedTimeBonus) {
        m_iLongTimeExpBonus = 5;
        resendAll = true;
    }
    for (auto& entry : m_memberMap) {
        const TMember& member = entry.second;
        CHARACTER* character = member.pCharacter;
        if (!character)
            continue;
        if (gainedTimeBonus && character->GetDesc()) {
            character->ChatPacket(1, LC_TEXT(
                "\306\304\306\274\300\307\040\307\371\265\277\267\302\300\314\040\263\364\276\306\301\256\040"
                "\301\366\261\335\272\316\305\315\040\303\337\260\241\040\260\346\307\350\304\241\040"
                "\272\270\263\312\275\272\270\246\040\271\336\275\300\264\317\264\331\056"));
        }
        ComputeRolePoint(character, member.bRole, member.bNear);
        if (member.bNear && !resendAll)
            SendPartyInfoOneToAll(character);
    }

    m_anMaxRole[PARTY_ROLE_ATTACKER] = (m_iLeadership >= 10) + (m_iLeadership >= 40);
    m_anMaxRole[PARTY_ROLE_HASTE] = m_iLeadership >= 20;
    m_anMaxRole[PARTY_ROLE_TANKER] = m_iLeadership >= 20;
    m_anMaxRole[PARTY_ROLE_BUFFER] = m_iLeadership >= 25;
    m_anMaxRole[PARTY_ROLE_SKILL_MASTER] = m_iLeadership >= 35;
    m_anMaxRole[PARTY_ROLE_DEFENDER] = m_iLeadership >= 40;

    if (!m_bPartyHealReady) {
        if (!m_bCanUsePartyHeal && m_iLeadership >= 18)
            m_dwPartyHealTime = get_dword_time();
        m_bCanUsePartyHeal = m_iLeadership >= 18;
        if (m_bCanUsePartyHeal) {
            const unsigned cooldown = m_iLeadership >= 40 ? 1800000U : 3600000U;
            if (static_cast<unsigned>(m_dwPartyHealTime + cooldown) < static_cast<unsigned>(get_dword_time()))
                m_bPartyHealReady = true;
        }
    }

    if (resendAll)
        ForEachOnlineMember([this](CHARACTER* character) { SendPartyInfoOneToAll(character); });
}

int CParty::GetExpDistributionMode()
{
    return m_iExpDistributionMode;
}

void CParty::SetExpCentralizeCharacter(DWORD pid)
{
    const auto member = m_memberMap.find(pid);
    if (member != m_memberMap.end())
        m_pkChrExpCentralize = member->second.pCharacter;
}

CHARACTER* CParty::GetExpCentralizeCharacter()
{
    return m_pkChrExpCentralize;
}

void CParty::ChatPacketToAllMember(BYTE type, const char* format, ...)
{
    char buffer[86];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, 0x55, format, args);
    va_end(args);

    ForEachOnlineMember([&](CHARACTER* member) {
        if (member->GetDesc())
            member->ChatPacket(type, "%s", buffer);
    });
}

BYTE CParty::CountMemberByVnum(DWORD vnum)
{
    if (m_bPCParty)
        return 0;
    BYTE count = 0;
    for (const auto& entry : m_memberMap) {
        LPCHARACTER character = entry.second.pCharacter;
        if (character && !character->GetDesc() && character->GetMobTable().dwVnum == vnum)
            ++count;
    }
    return count;
}
