#ifndef MCNCC_GAME_PVP_H
#define MCNCC_GAME_PVP_H
#include <map>
#include <set>
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"

class CHARACTER;
class DESC;
class CPVP {
    friend class CPVPManager;
public:
    CPVP(DWORD attacker, DWORD victim);
    CPVP(CPVP& other);
    ~CPVP();
    bool IsFight();
    void SetLastFightTime();
    DWORD GetLastFightTime();
    bool Agree(DWORD pid);
    bool CanRevenge(DWORD pid);
    void Win(DWORD pid);
    void SetVID(DWORD pid, DWORD vid);
    void Packet(bool remove = false);
protected:
    struct TPlayer {
        DWORD dwPID;
        DWORD dwVID;
        bool bAgree;
        bool bCanRevenge;
    };
    TPlayer m_players[2];
    DWORD m_dwCRC;
    bool m_bRevenge;
    DWORD m_dwLastFightTime;
};

class CPVPManager : public singleton<CPVPManager> {
public:
    CPVPManager();
    virtual ~CPVPManager();
    bool CanAttack(CHARACTER* attacker, CHARACTER* victim);
    CPVP* Find(DWORD crc);
    void Insert(CHARACTER* attacker, CHARACTER* victim);
    bool Dead(CHARACTER* ch, DWORD killerPID);
    void GiveUp(CHARACTER* ch, DWORD killerPID);
    void Delete(CPVP* pvp);
    void Connect(CHARACTER* ch);
    void Disconnect(CHARACTER* ch);
    void SendList(DESC* desc);
    void Process();
protected:
    void ConnectEx(CHARACTER* ch, bool disconnect);
    std::map<DWORD, CPVP*> m_map_pkPVP;
    std::map<DWORD, std::set<CPVP*> > m_map_pkPVPSetByID;
};
#endif
