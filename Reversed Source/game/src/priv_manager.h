#ifndef MCNCC_GAME_PRIV_MANAGER_H
#define MCNCC_GAME_PRIV_MANAGER_H
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
#include <ctime>
#include <map>

const char* GetPrivName(int type);
const char* GetEmpireName(int empire);

class CPrivManager : public singleton<CPrivManager> {
public:
    CPrivManager();
    virtual ~CPrivManager();
    int GetPriv(class CHARACTER* character, BYTE type);
    struct SPrivEmpireData {
        int m_value;
        time_t m_end_time_sec;
    };
    struct SPrivGuildData {
        int value;
        time_t end_time_sec;
    };
    void RequestRemoveGuildPriv(DWORD guild, BYTE type);
    void RequestRemoveEmpirePriv(BYTE empire, BYTE type);
    void RequestGiveEmpirePriv(BYTE empire, BYTE type, int value, time_t duration);
    void RequestGiveGuildPriv(DWORD guild, BYTE type, int value, time_t duration);
    void RequestGiveCharacterPriv(DWORD pid, BYTE type, int value);
    void GiveEmpirePriv(BYTE empire, BYTE type, int value, BYTE log, time_t endTime);
    void GiveGuildPriv(DWORD guild, BYTE type, int value, BYTE log, time_t endTime);
    void GiveCharacterPriv(DWORD pid, BYTE type, int value, BYTE log);
    void RemoveGuildPriv(DWORD guild, BYTE type);
    void RemoveEmpirePriv(BYTE empire, BYTE type);
    void RemoveCharacterPriv(DWORD pid, BYTE type);
    SPrivEmpireData* GetPrivByEmpireEx(BYTE empire, BYTE type);
    const SPrivGuildData* GetPrivByGuildEx(DWORD guild, BYTE type) const;
    int GetPrivByEmpire(BYTE empire, BYTE type);
    int GetPrivByGuild(DWORD guild, BYTE type);
    int GetPrivByCharacter(DWORD pid, BYTE type);

private:
    SPrivEmpireData m_aakPrivEmpireData[5][4];
    std::map<DWORD, SPrivGuildData> m_aPrivGuild[5];
    std::map<DWORD, int> m_aPrivChar[5];
};
#endif
