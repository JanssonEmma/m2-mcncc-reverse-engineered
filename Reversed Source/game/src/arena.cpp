#include "char_manager.h"
#include "locale.hpp"
#include "../../libthecore/include/log.h"
#include <cstring>
extern "C" {
#include "../../liblua/include/lua.h"
}
#include <memory>
#include "arena.h"
#include "config.h"
#include "desc.h"
#include "questmanager.h"
#include <cstdint>
#include "char.h"

// 080bc300
bool CArena::CanAttack(DWORD attacker, DWORD victim)
{
    return (m_dwPIDA == attacker && m_dwPIDB == victim) ||
           (m_dwPIDA == victim && m_dwPIDB == attacker);
}

// 080bc280
bool CArenaMap::CanAttack(CHARACTER* attacker, CHARACTER* victim)
{
    if (!attacker || !victim)
        return false;
    const DWORD attackerPID = attacker->GetPlayerID();
    const DWORD victimPID = victim->GetPlayerID();
    for (std::list<CArena*>::iterator it = m_listArena.begin(); it != m_listArena.end(); ++it)
        if ((*it)->CanAttack(attackerPID, victimPID))
            return true;
    return false;
}

// 080bc1f0
bool CArenaManager::CanAttack(CHARACTER* attacker, CHARACTER* victim)
{
    if (!attacker || !victim || attacker == victim ||
        attacker->GetMapIndex() != victim->GetMapIndex())
        return false;
    std::map<DWORD, CArenaMap*>::iterator it = m_mapArenaMap.find(attacker->GetMapIndex());
    return it != m_mapArenaMap.end() && it->second->CanAttack(attacker, victim);
}

void CArena::Clear()
{
    m_dwPIDA=0;
    m_dwPIDB=0;
    if (m_pEvent) event_cancel(&m_pEvent);
    if (m_pTimeOutEvent) event_cancel(&m_pTimeOutEvent);
    m_dwSetCount=0;
    m_dwSetPointOfA=0;
    m_dwSetPointOfB=0;
}

namespace {
void WarpFromArena(CHARACTER* character)
{
    const BYTE empire=character->GetEmpire();
    const long* position=arena_return_position[empire>=1 && empire<=3 ? empire : 0];
    character->WarpSet(position[0],position[1],0);
}

std::int32_t ArenaDelay(unsigned int seconds)
{
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(passes_per_sec) * seconds);
}

int ArenaPointDelta(int maximum, int current)
{
    unsigned int bits=static_cast<unsigned int>(maximum)-current;
    int value;
    std::memcpy(&value,&bits,sizeof(value));
    return value;
}
}

void CArena::EndDuel()
{
    CHARACTER_MANAGER& manager=CHARACTER_MANAGER::instance();
    CHARACTER* players[2]={manager.FindByPID(m_dwPIDA),manager.FindByPID(m_dwPIDB)};
    for (int slot=0;slot<2;++slot) {
        CHARACTER* player=players[slot];
        if (!player) continue;
        player->SetPKMode(0);
        player->StartRecoveryEvent();
        player->SetPosition(7);
        player->PointChange(5,ArenaPointDelta(player->m_pointsInstant.iMaxHP,player->m_points.hp),false,false);
        player->PointChange(7,ArenaPointDelta(player->m_pointsInstant.iMaxSP,player->m_points.sp),false,false);
        player->m_pArena=0;
        WarpFromArena(player);
    }
    for (std::map<DWORD,CHARACTER*>::iterator it=m_mapObserver.begin();it!=m_mapObserver.end();++it) {
        CHARACTER* observer=manager.FindByPID(it->first);
        if (observer) WarpFromArena(observer);
    }
    m_mapObserver.clear();
    sys_log(0,"ARENA : End Duel PID_A(%d) vs PID_B(%d)",m_dwPIDA,m_dwPIDB);
    Clear();
}

void CArena::OnDisconnect(DWORD pid)
{
    if (pid!=m_dwPIDA && pid!=m_dwPIDB) return;
    const bool first=pid==m_dwPIDA;
    CHARACTER_MANAGER& manager=CHARACTER_MANAGER::instance();
    if (manager.FindByPID(first ? m_dwPIDB : m_dwPIDA)) {
        const char* text=locale_find("\273\363\264\353\271\346\040\304\263\270\257\305\315\260\241\040\301\242\274\323\300\273\040\301\276\267\341\307\317\277\251\040\264\353\267\303\300\273\040\301\337\301\366\307\325\264\317\264\331\056",g_iUseLocale);
        manager.FindByPID(first ? m_dwPIDB : m_dwPIDA)->ChatPacket(1,text);
    }
    DWORD disconnected=first ? m_dwPIDA : m_dwPIDB;
    DWORD opponent=first ? m_dwPIDB : m_dwPIDA;
    sys_log(0,"ARENA : Duel is end because of Opp(%d) is disconnect. MyPID(%d)",disconnected,opponent);
    EndDuel();
}

// 080bd250
void CArena::RemoveObserver(DWORD pid)
{
    const auto found = m_mapObserver.find(pid);
    if (found != m_mapObserver.end())
        m_mapObserver.erase(found);
}

bool CArena::IsObserver(DWORD pid)
{
    return m_mapObserver.find(pid) != m_mapObserver.end();
}

bool CArena::IsMyObserver(WORD x, WORD y)
{
    return x == m_ObserverPoint.x && y == m_ObserverPoint.y;
}

bool CArena::RegisterObserverPtr(CHARACTER* character)
{
    const auto found = m_mapObserver.find(character->GetPlayerID());
    if (found == m_mapObserver.end()) {
        sys_log(0, "ARENA : not in ob list");
        return false;
    }
    found->second = character;
    return true;
}

int CArenaMap::IsMember(DWORD pid)
{
    for (CArena* arena : m_listArena) {
        if (arena->IsObserver(pid))
            return 2;
        if (arena->m_dwPIDA == pid || arena->m_dwPIDB == pid)
            return 1;
    }
    return 0;
}

bool CArenaMap::RegisterObserverPtr(CHARACTER* character, DWORD, WORD x, WORD y)
{
    for (CArena* arena : m_listArena) {
        if (arena->IsMyObserver(x, y))
            return arena->RegisterObserverPtr(character);
    }
    return false;
}

bool CArenaManager::IsArenaMap(DWORD map)
{
    return m_mapArenaMap.find(map) != m_mapArenaMap.end();
}

int CArenaManager::IsMember(DWORD map, DWORD pid)
{
    const auto found = m_mapArenaMap.find(map);
    return found != m_mapArenaMap.end() ? found->second->IsMember(pid) : 0;
}

bool CArenaManager::RegisterObserverPtr(CHARACTER* character, DWORD map, WORD x, WORD y)
{
    if (!character)
        return false;

    const auto found = m_mapArenaMap.find(map);
    if (found == m_mapArenaMap.end()) {
        sys_log(0, "ARENA : Cannot find ArenaMap. %d %d %d",
                static_cast<int>(map), static_cast<int>(x), static_cast<int>(y));
        return false;
    }
    return found->second->RegisterObserverPtr(character, map, x, y);
}

namespace {
struct ArenaReadyEventInfo
{
    CArena* arena;
    BYTE state;
};

const char opponentInvalid[] = "\273\363\264\353\271\346 \304\263\270\257\305\315\300\307 \271\256\301\246\267\316 \300\316\307\317\277\251 \264\353\267\303\300\273 \301\276\267\341\307\325\264\317\264\331.";
const char duelWinner[] = "%s \264\324\300\314 \264\353\267\303\277\241\274\255 \275\302\270\256\307\317\277\264\275\300\264\317\264\331.";
const char roundWinner[] = "%s \264\324\300\314 \275\302\270\256\307\317\277\264\275\300\264\317\264\331.";
const char nextRound[] = "10\303\312\265\332 \264\331\300\275 \306\307\300\273 \275\303\300\333\307\325\264\317\264\331.";
const char returnToTown[] = "10\303\312\265\332 \270\266\300\273\267\316 \265\307\265\271\276\306\260\251\264\317\264\331.";
const char opponentMissing[] = "\264\353\267\303 \273\363\264\353\260\241 \273\347\266\363\301\256 \264\353\267\303\300\273 \301\276\267\341\307\325\264\317\264\331.";
const char duelStarted[] = "\264\353\267\303\300\314 \275\303\300\333\265\307\276\372\275\300\264\317\264\331.";
const char potionLimit[] = "\271\260\276\340\300\273 %d \260\263 \261\356\301\366 \273\347\277\353 \260\241\264\311\307\325\264\317\264\331.";
const char noPotionLimit[] = "\271\260\276\340 \301\246\307\321\300\314 \276\370\275\300\264\317\264\331.";
const char duelCountdown[] = "10\303\312\265\332 \264\353\267\303\300\314 \275\303\300\333\265\313\264\317\264\331.";
const char duelStateInvalid[] = "\264\353\267\303\300\345 \271\256\301\246\267\316 \300\316\307\317\277\251 \264\353\267\303\300\273 \301\276\267\341\307\325\264\317\264\331.";

void SendArenaOpponent(CHARACTER* player, CHARACTER* opponent)
{
    const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(opponent->GetVID()));
    const BYTE packet[] = {
        40, 7, 0,
        static_cast<BYTE>(vid),
        static_cast<BYTE>(vid >> 8),
        static_cast<BYTE>(vid >> 16),
        static_cast<BYTE>(vid >> 24),
    };
    player->GetDesc()->Packet(packet, sizeof(packet));
}
}

EVENTFUNC(ready_to_start_event);

namespace {
const char timeoutNotice[] = "\264\353\267\303\040\275\303\260\243\040\303\312\260\372\267\316\040\264\353\267\303\300\273\040\301\337\264\334\307\325\264\317\264\331\056";
const char timeoutExit[] = "\061\060\303\312\265\332\040\270\266\300\273\267\316\040\300\314\265\277\307\325\264\317\264\331\056";
const char timeoutMissing[] = "\264\353\267\303\040\273\363\264\353\260\241\040\273\347\266\363\301\256\040\264\353\267\303\300\273\040\301\276\267\341\307\325\264\317\264\331\056";
}

EVENTFUNC(duel_time_out)
{
    if (!event || !event->info)
        return 0;
    auto* info = static_cast<ArenaReadyEventInfo*>(event->info);
    if (!info->arena)
    {
        _sys_err("duel_time_out", 0x17f, "ARENA : Time out event error");
        return 0;
    }
    return info->arena->HandleTimeout(info->state);
}

// 080bd2d0 and 080bd2e0. The original spectator send hooks have empty bodies.
void CArena::SendPacketToObserver(const void*, int) {}
void CArena::SendChatPacketToObserver(BYTE, const char*, ...) {}

long CArena::HandleTimeout(BYTE& state)
{
    CHARACTER_MANAGER& chars = CHARACTER_MANAGER::instance();
    CHARACTER* first = chars.FindByPID(m_dwPIDA);
    CHARACTER* second = chars.FindByPID(m_dwPIDB);
    if (first && second)
    {
        if (state == 0)
        {
            const char* notice = locale_find(timeoutNotice, g_iUseLocale);
            const char* exit = locale_find(timeoutExit, g_iUseLocale);
            SendChatPacketToObserver(2, notice);
            SendChatPacketToObserver(2, exit);
            first->ChatPacket(2, notice);
            first->ChatPacket(2, exit);
            second->ChatPacket(2, notice);
            second->ChatPacket(2, exit);
            if (DESC* desc = first->GetDesc())
            {
                const BYTE packet[] = {40, 3, 0};
                desc->Packet(packet, sizeof(packet));
                desc->Packet(packet, sizeof(packet));
            }
            ++state;
            sys_log(0, "ARENA : Because of time over, duel is end. PIDA(%d) vs PIDB(%d)",
                static_cast<int>(m_dwPIDA), static_cast<int>(m_dwPIDB));
            return ArenaDelay(10);
        }
        if (state != 1)
            return 0;
    }
    else
    {
        const char* message = locale_find(timeoutMissing, g_iUseLocale);
        if (first)
        {
            first->ChatPacket(1, message);
            sys_log(0, "ARENA : Oppernent is disappered. MyPID(%d) OppPID(%d)",
                static_cast<int>(m_dwPIDA), static_cast<int>(m_dwPIDB));
        }
        if (second)
        {
            second->ChatPacket(1, message);
            sys_log(0, "ARENA : Oppernent is disappered. MyPID(%d) OppPID(%d)",
                static_cast<int>(m_dwPIDB), static_cast<int>(m_dwPIDA));
        }
        SendChatPacketToObserver(1, message);
    }
    EndDuel();
    return 0;
}

CArena::CArena(WORD startAX, WORD startAY, WORD startBX, WORD startBY)
    : m_dwPIDA(0), m_dwPIDB(0), m_pEvent(nullptr), m_pTimeOutEvent(nullptr),
      m_StartPointA{startAX, startAY, 0}, m_StartPointB{startBX, startBY, 0},
      m_ObserverPoint{(startAX + startBX) / 2, (startAY + startBY) / 2, 0},
      m_dwSetCount(0), m_dwSetPointOfA(0), m_dwSetPointOfB(0)
{
}

bool CArena::CheckArea(WORD startAX, WORD startAY, WORD startBX, WORD startBY)
{
    return !(m_StartPointA.x == startAX && m_StartPointA.y == startAY &&
             m_StartPointB.x == startBX && m_StartPointB.y == startBY);
}

bool CArena::StartDuel(CHARACTER* first, CHARACTER* second, int sets)
{
    m_dwPIDA = first->GetPlayerID();
    m_dwPIDB = second->GetPlayerID();
    m_dwSetCount = static_cast<std::uint32_t>(sets);
    first->WarpSet(m_StartPointA.x * 100, m_StartPointA.y * 100, 0);
    second->WarpSet(m_StartPointB.x * 100, m_StartPointB.y * 100, 0);

    void* raw = nullptr;
    DWORD id = __AllocEventInfo(sizeof(ArenaReadyEventInfo), &raw);
    auto* info = static_cast<ArenaReadyEventInfo*>(raw);
    info->arena = this;
    info->state = 0;
    m_pEvent = event_create_ex(ready_to_start_event, id, ArenaDelay(10),
        const_cast<char*>("arena.cpp"), 0x1cd);
    for (CHARACTER* player : {first, second})
    {
        player->PointChange(5, ArenaPointDelta(player->m_pointsInstant.iMaxHP, player->m_points.hp), false, false);
        player->PointChange(7, ArenaPointDelta(player->m_pointsInstant.iMaxSP, player->m_points.sp), false, false);
    }

    id = __AllocEventInfo(sizeof(ArenaReadyEventInfo), &raw);
    info = static_cast<ArenaReadyEventInfo*>(raw);
    info->arena = this;
    info->state = 0;
    m_pTimeOutEvent = event_create_ex(duel_time_out, id, ArenaDelay(300),
        const_cast<char*>("arena.cpp"), 0x1d9);
    sys_log(0, "ARENA : Start Duel with PID_A(%d) vs PID_B(%d)",
        static_cast<int>(m_dwPIDA), static_cast<int>(m_dwPIDB));
    return true;
}

bool CArena::AddObserver(CHARACTER* observer)
{
    m_mapObserver.emplace(observer->GetPlayerID(), nullptr);
    observer->SaveExitLocation();
    observer->WarpSet(m_ObserverPoint.x * 100, m_ObserverPoint.y * 100, 0);
    return true;
}

bool CArenaMap::AddArena(DWORD map, WORD startAX, WORD startAY, WORD startBX, WORD startBY)
{
    for (CArena* arena : m_listArena)
        if (!arena->CheckArea(startAX, startAY, startBX, startBY))
        {
            sys_log(0, "CArenaMap::AddArena - Same Start Position set. stA(%d,%d) stB(%d,%d)",
                startAX, startAY, startBX, startBY);
            return false;
        }
    m_dwMapIndex = map;
    std::unique_ptr<CArena> arena(new CArena(startAX, startAY, startBX, startBY));
    m_listArena.push_back(arena.get());
    arena.release();
    return true;
}

bool CArenaMap::StartDuel(CHARACTER* first, CHARACTER* second, int sets)
{
    for (CArena* arena : m_listArena)
        if (arena->m_dwPIDA == 0 && arena->m_dwPIDB == 0)
            return arena->StartDuel(first, second, sets);
    return false;
}

bool CArenaMap::AddObserver(CHARACTER* observer, WORD x, WORD y)
{
    for (CArena* arena : m_listArena)
        if (arena->IsMyObserver(x, y))
            return arena->AddObserver(observer);
    return false;
}

int CArenaMap::GetDuelList(lua_State* state, int index)
{
    CHARACTER_MANAGER& chars = CHARACTER_MANAGER::instance();
    for (CArena* arena : m_listArena)
    {
        if (!arena || (arena->m_dwPIDA == 0 && arena->m_dwPIDB == 0))
            continue;
        CHARACTER* first = chars.FindByPID(arena->m_dwPIDA);
        CHARACTER* second = chars.FindByPID(arena->m_dwPIDB);
        if (!first || !second)
            continue;
        lua_newtable(state);
        lua_pushstring(state, first->GetName());
        lua_rawseti(state, -2, 1);
        lua_pushstring(state, second->GetName());
        lua_rawseti(state, -2, 2);
        lua_pushnumber(state, m_dwMapIndex);
        lua_rawseti(state, -2, 3);
        lua_pushnumber(state, arena->m_ObserverPoint.x);
        lua_rawseti(state, -2, 4);
        lua_pushnumber(state, arena->m_ObserverPoint.y);
        lua_rawseti(state, -2, 5);
        lua_rawseti(state, -2, index++);
    }
    return index;
}

void CArenaManager::SendArenaMapListTo(CHARACTER* character)
{
    for (const auto& entry : m_mapArenaMap)
        entry.second->SendArenaMapListTo(character, entry.first);
}

void CArenaMap::SendArenaMapListTo(CHARACTER* character, DWORD map)
{
    if (!character)
        return;
    for (const CArena* arena : m_listArena)
    {
        character->ChatPacket(1, "ArenaMapInfo Map: %d stA(%d, %d) stB(%d, %d)",
            static_cast<int>(map), static_cast<int>(arena->m_StartPointA.x),
            static_cast<int>(arena->m_StartPointA.y), static_cast<int>(arena->m_StartPointB.x),
            static_cast<int>(arena->m_StartPointB.y));
    }
}

void CArenaManager::EndAllDuel()
{
    for (const auto& entry : m_mapArenaMap)
        if (entry.second)
            entry.second->EndAllDuel();
}

void CArenaMap::EndAllDuel()
{
    for (CArena* arena : m_listArena)
        if (arena)
            arena->EndDuel();
}

bool CArenaManager::EndDuel(DWORD pid)
{
    for (const auto& entry : m_mapArenaMap)
        if (entry.second->EndDuel(pid))
            return true;
    return false;
}

bool CArenaMap::EndDuel(DWORD pid)
{
    for (CArena* arena : m_listArena)
    {
        if (arena->m_dwPIDA == pid || arena->m_dwPIDB == pid)
        {
            arena->EndDuel();
            return true;
        }
    }
    return false;
}

void CArenaMap::Destroy()
{
    sys_log(0, "ARENA : ArenaMap will be destroy. mapIndex(%d)", static_cast<int>(m_dwMapIndex));
    for (CArena* arena : m_listArena)
    {
        arena->EndDuel();
        delete arena;
    }
    m_listArena.clear();
}

bool CArenaManager::AddArena(DWORD map, WORD startAX, WORD startAY, WORD startBX, WORD startBY)
{
    const auto found = m_mapArenaMap.find(map);
    if (found != m_mapArenaMap.end())
    {
        if (found->second->AddArena(map, startAX, startAY, startBX, startBY))
            return true;
        sys_log(0, "CArenaManager::AddArena - AddMap Error MapID: %d", static_cast<int>(map));
        return false;
    }

    std::unique_ptr<CArenaMap> entry(new CArenaMap);
    const auto inserted = m_mapArenaMap.emplace(map, entry.get());
    try
    {
        entry->AddArena(map, startAX, startAY, startBX, startBY);
    }
    catch (...)
    {
        m_mapArenaMap.erase(inserted.first);
        throw;
    }
    entry.release();
    return true;
}

bool CArenaManager::StartDuel(CHARACTER* first, CHARACTER* second, int sets)
{
    if (!first || !second)
        return false;
    for (const auto& entry : m_mapArenaMap)
        if (entry.second->StartDuel(first, second, sets))
            return true;
    return false;
}

bool CArenaManager::AddObserver(CHARACTER* observer, DWORD map, WORD x, WORD y)
{
    const auto found = m_mapArenaMap.find(map);
    return observer && found != m_mapArenaMap.end() && found->second->AddObserver(observer, x, y);
}

void CArenaManager::GetDuelList(lua_State* state)
{
    lua_newtable(state);
    int index = 1;
    for (const auto& entry : m_mapArenaMap)
        if (entry.second)
            index = entry.second->GetDuelList(state, index);
}

void CArenaManager::Destroy()
{
    for (const auto& entry : m_mapArenaMap)
    {
        entry.second->Destroy();
        delete entry.second;
    }
    m_mapArenaMap.clear();
}

EVENTFUNC(ready_to_start_event)
{
    if (!event || !event->info)
        return 0;
    auto* info = static_cast<ArenaReadyEventInfo*>(event->info);
    if (!info->arena) {
        _sys_err("ready_to_start_event", 0xd1, "ARENA : Arena start event info is null.");
        return 0;
    }
    return info->arena->HandleReadyEvent(info->state);
}

long CArena::HandleReadyEvent(BYTE& state)
{
    auto& manager = CHARACTER_MANAGER::instance();
    CHARACTER* playerA = manager.FindByPID(m_dwPIDA);
    CHARACTER* playerB = manager.FindByPID(m_dwPIDB);
    if (!playerA || !playerB || !playerA->GetDesc() || !playerB->GetDesc()) {
        _sys_err("ready_to_start_event", 0xda, "ARENA : Player err in event func ready_start_event");
        const char* message = locale_find(opponentMissing, g_iUseLocale);
        if (playerA) {
            playerA->ChatPacket(1, message);
            sys_log(0, "ARENA : Oppernent is disappered. MyPID(%d) OppPID(%d)",
                    static_cast<int>(m_dwPIDA), static_cast<int>(m_dwPIDB));
        }
        if (playerB) {
            playerB->ChatPacket(1, message);
            sys_log(0, "ARENA : Oppernent is disappered. MyPID(%d) OppPID(%d)",
                    static_cast<int>(m_dwPIDB), static_cast<int>(m_dwPIDA));
        }
        SendChatPacketToObserver(2, message);
    } else {
        switch (state) {
        case 0: {
            playerA->m_pArena = this;
            playerB->m_pArena = this;
            const int count = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
            if (count < 10001) {
                playerA->m_nPotionLimit = count;
                playerB->m_nPotionLimit = count;
                const char* message = locale_find(potionLimit, g_iUseLocale);
                playerA->ChatPacket(1, message, count);
                playerB->ChatPacket(1, message, count);
            } else {
                const char* message = locale_find(noPotionLimit, g_iUseLocale);
                playerA->ChatPacket(1, message);
                playerB->ChatPacket(1, message);
            }
            const char* message = locale_find(duelCountdown, g_iUseLocale);
            playerA->ChatPacket(1, message);
            playerB->ChatPacket(1, message);
            SendChatPacketToObserver(1, message);
            ++state;
            return ArenaDelay(10);
        }
        case 1: {
            const char* message = locale_find(duelStarted, g_iUseLocale);
            playerA->ChatPacket(1, message);
            playerB->ChatPacket(1, message);
            SendChatPacketToObserver(1, message);
            SendArenaOpponent(playerA, playerB);
            SendArenaOpponent(playerB, playerA);
            m_pEvent = 0;
            return 0;
        }
        case 2:
            break;
        case 3: {
            playerA->Show(playerA->GetMapIndex(), m_StartPointA.x * 100, m_StartPointA.y * 100,
                          0x7fffffff, false);
            playerB->Show(playerB->GetMapIndex(), m_StartPointB.x * 100, m_StartPointB.y * 100,
                          0x7fffffff, false);
            for (CHARACTER* player : {playerA, playerB}) {
                player->GetDesc()->SetPhase(5);
                player->StartRecoveryEvent();
                player->SetPosition(POS_STANDING);
                player->PointChange(POINT_HP, ArenaPointDelta(player->m_pointsInstant.iMaxHP, player->m_points.hp),
                                    false, false);
                player->PointChange(POINT_SP, ArenaPointDelta(player->m_pointsInstant.iMaxSP, player->m_points.sp),
                                    false, false);
                player->ViewReencode();
            }
            SendArenaOpponent(playerA, playerB);
            SendArenaOpponent(playerB, playerA);
            const char* message = locale_find(duelStarted, g_iUseLocale);
            playerA->ChatPacket(1, message);
            playerB->ChatPacket(1, message);
            SendChatPacketToObserver(1, message);
            m_pEvent = 0;
            return 0;
        }
        default: {
            const char* message = locale_find(duelStateInvalid, g_iUseLocale);
            playerA->ChatPacket(1, message);
            playerB->ChatPacket(1, message);
            SendChatPacketToObserver(1, message);
            sys_log(0, "ARENA : Something wrong in event func. info->state(%d)", state);
            break;
        }
        }
    }
    m_pEvent = 0;
    EndDuel();
    return 0;
}

bool CArena::OnDead(DWORD killerPID, DWORD victimPID)
{
    auto& manager = CHARACTER_MANAGER::instance();
    CHARACTER* playerA = manager.FindByPID(m_dwPIDA);
    CHARACTER* playerB = manager.FindByPID(m_dwPIDB);
    bool restart = false;
    if (playerA && playerB) {
        if (killerPID == m_dwPIDA || killerPID == m_dwPIDB) {
            const bool firstWon = killerPID == m_dwPIDA;
            auto& score = firstWon ? m_dwSetPointOfA : m_dwSetPointOfB;
            restart = ++score < m_dwSetCount;
            const char* winner = (firstWon ? playerA : playerB)->GetName();
            const char* message = locale_find(restart ? roundWinner : duelWinner, g_iUseLocale);
            for (CHARACTER* player : {playerA, playerB}) {
                player->ChatPacket(restart ? 1 : 2, message, winner);
                if (restart)
                    player->ChatPacket(2, "%s %d : %d %s", playerA->GetName(),
                                       static_cast<int>(m_dwSetPointOfA), static_cast<int>(m_dwSetPointOfB),
                                       playerB->GetName());
            }
            if (restart)
                SendChatPacketToObserver(2, "%s %d : %d %s", playerA->GetName(),
                                         static_cast<int>(m_dwSetPointOfA), static_cast<int>(m_dwSetPointOfB),
                                         playerB->GetName());
            else
                SendChatPacketToObserver(2, message, winner);
            sys_log(0, restart ? "ARENA : PID(%d) won a round. Opp(%d)" :
                                 "ARENA : Duel is end. Winner(%d) Loser(%d)",
                    static_cast<int>(killerPID), static_cast<int>(firstWon ? m_dwPIDB : m_dwPIDA));
        } else {
            sys_log(0, "ARENA : OnDead Error (%d, %d) (%d,%d)",
                    static_cast<int>(m_dwPIDA), static_cast<int>(m_dwPIDB),
                    static_cast<int>(killerPID), static_cast<int>(victimPID));
        }
        const int count = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
        playerA->m_nPotionLimit = count;
        playerB->m_nPotionLimit = count;
    } else if (CHARACTER* remaining = playerA ? playerA : playerB) {
        remaining->ChatPacket(2, locale_find(opponentInvalid, g_iUseLocale));
        static constexpr char observerOpponentInvalid[] =
            "\264\353\267\303\300\332 \271\256\301\246\267\316 \300\316\307\317\277\251 \264\353\267\303\300\273 \301\276\267\341\307\325\264\317\264\331.";
        SendChatPacketToObserver(2, locale_find(observerOpponentInvalid, g_iUseLocale));
    }

    const char* message = locale_find(restart ? nextRound : returnToTown, g_iUseLocale);
    if (playerA)
        playerA->ChatPacket(1, message);
    if (playerB)
        playerB->ChatPacket(1, message);
    SendChatPacketToObserver(1, message);

    event_cancel(&m_pEvent);
    void* raw = 0;
    const DWORD id = __AllocEventInfo(sizeof(ArenaReadyEventInfo), &raw);
    auto* info = static_cast<ArenaReadyEventInfo*>(raw);
    info->arena = this;
    info->state = restart ? 3 : 2;
    m_pEvent = event_create_ex(ready_to_start_event, id, ArenaDelay(10),
                              const_cast<char*>("arena.cpp"), restart ? 0x331 : 0x320);
    return true;
}

bool CArenaMap::OnDead(CHARACTER* killer, CHARACTER* victim)
{
    const DWORD killerPID = killer->GetPlayerID();
    const DWORD victimPID = victim->GetPlayerID();
    for (CArena* arena : m_listArena) {
        if ((arena->m_dwPIDA == killerPID || arena->m_dwPIDB == killerPID) &&
            (arena->m_dwPIDA == victimPID || arena->m_dwPIDB == victimPID)) {
            arena->OnDead(killerPID, victimPID);
            return true;
        }
    }
    return false;
}

bool CArenaManager::OnDead(CHARACTER* killer, CHARACTER* victim)
{
    if (!killer || !victim || killer->GetMapIndex() != victim->GetMapIndex())
        return false;
    const auto arena = m_mapArenaMap.find(killer->GetMapIndex());
    return arena != m_mapArenaMap.end() && arena->second->OnDead(killer, victim);
}
