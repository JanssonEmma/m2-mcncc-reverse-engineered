#ifndef MCNCC_GAME_ARENA_H
#define MCNCC_GAME_ARENA_H
#include <cstdint>
#include <list>
#include <map>
#include "char_manager.h"
#include "event.h"
#include "typedef.h"
#include "../../common/singleton.h"

struct lua_State;
#include "start_position.h"

class CArena {
    friend class CArenaMap;

public:
    CArena(WORD startAX, WORD startAY, WORD startBX, WORD startBY);
    bool CheckArea(WORD startAX, WORD startAY, WORD startBX, WORD startBY);
    bool StartDuel(CHARACTER* first, CHARACTER* second, int sets);
    bool AddObserver(CHARACTER* observer);
    bool CanAttack(DWORD attacker, DWORD victim);
    bool OnDead(DWORD killerPID, DWORD victimPID);
    void OnDisconnect(DWORD pid);
    void EndDuel();
    void Clear();
    bool IsObserver(DWORD pid);
    void RemoveObserver(DWORD pid);
    void SendPacketToObserver(const void* data, int size);
    void SendChatPacketToObserver(BYTE type, const char* format, ...);
    bool IsMyObserver(WORD x, WORD y);
    bool RegisterObserverPtr(CHARACTER* character);
    DWORD GetPlayerAPID() { return m_dwPIDA; }
    DWORD GetPlayerBPID() { return m_dwPIDB; }
    CHARACTER* GetPlayerA() { return CHARACTER_MANAGER::instance().FindByPID(m_dwPIDA); }
    CHARACTER* GetPlayerB() { return CHARACTER_MANAGER::instance().FindByPID(m_dwPIDB); }
    PIXEL_POSITION GetStartPointA() { return m_StartPointA; }
    PIXEL_POSITION GetStartPointB() { return m_StartPointB; }
    PIXEL_POSITION GetObserverPoint() { return m_ObserverPoint; }
    void ClearEvent() { m_pEvent = nullptr; }
private:
    friend long ready_to_start_event(LPEVENT event);
    friend long duel_time_out(LPEVENT event);
    bool IsEmpty() { return m_dwPIDA == 0 && m_dwPIDB == 0; }
    long HandleTimeout(BYTE& state);
    long HandleReadyEvent(BYTE& state);
    std::uint32_t m_dwPIDA;
    std::uint32_t m_dwPIDB;
    LPEVENT m_pEvent;
    LPEVENT m_pTimeOutEvent;
    PIXEL_POSITION m_StartPointA;
    PIXEL_POSITION m_StartPointB;
    PIXEL_POSITION m_ObserverPoint;
    std::uint32_t m_dwSetCount;
    std::uint32_t m_dwSetPointOfA;
    std::uint32_t m_dwSetPointOfB;
    std::map<std::uint32_t, CHARACTER*> m_mapObserver;
};

class CArenaMap {
public:
    bool AddArena(DWORD map, WORD startAX, WORD startAY, WORD startBX, WORD startBY);
    bool StartDuel(CHARACTER* first, CHARACTER* second, int sets);
    bool AddObserver(CHARACTER* observer, WORD x, WORD y);
    int GetDuelList(lua_State* state, int index);
    void SendArenaMapListTo(CHARACTER* character, DWORD map);
    void EndAllDuel();
    bool EndDuel(DWORD pid);
    void Destroy();
    int IsMember(DWORD pid);
    bool RegisterObserverPtr(CHARACTER* character, DWORD map, WORD x, WORD y);
    bool CanAttack(CHARACTER* attacker, CHARACTER* victim);
    bool OnDead(CHARACTER* killer, CHARACTER* victim);
private:
    std::uint32_t m_dwMapIndex;
    std::list<CArena*> m_listArena;
};

class CArenaManager : public singleton<CArenaManager> {
public:
    void Destroy();
    bool AddArena(DWORD map, WORD startAX, WORD startAY, WORD startBX, WORD startBY);
    bool StartDuel(CHARACTER* first, CHARACTER* second, int sets);
    bool AddObserver(CHARACTER* observer, DWORD map, WORD x, WORD y);
    void GetDuelList(lua_State* state);
    void SendArenaMapListTo(CHARACTER* character);
    void EndAllDuel();
    bool EndDuel(DWORD pid);
    bool IsArenaMap(DWORD map);
    int IsMember(DWORD map, DWORD pid);
    bool RegisterObserverPtr(CHARACTER* character, DWORD map, WORD x, WORD y);
    bool CanAttack(CHARACTER* attacker, CHARACTER* victim);
    bool OnDead(CHARACTER* killer, CHARACTER* victim);
private:
    std::map<std::uint32_t, CArenaMap*> m_mapArenaMap;
};
#endif
