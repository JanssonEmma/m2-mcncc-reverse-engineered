// Complete old CParty declaration recovered from party.cpp DWARF.
// Historical GCC3 STL layout: class160/member12; native STL ABI may differ.
#ifndef MCNCC_GAME_PARTY_H
#define MCNCC_GAME_PARTY_H
#include <map>
#include <set>
#include "../../common/singleton.h"
#include <string>
#include "../../libthecore/include/typedef.h"
#include "event.h"
class CHARACTER;
class CDungeon;
enum EPartyRole {
    PARTY_ROLE_NORMAL = 0,
    PARTY_ROLE_LEADER = 1,
    PARTY_ROLE_ATTACKER = 2,
    PARTY_ROLE_TANKER = 3,
    PARTY_ROLE_BUFFER = 4,
    PARTY_ROLE_SKILL_MASTER = 5,
    PARTY_ROLE_HASTE = 6,
    PARTY_ROLE_DEFENDER = 7,
    PARTY_ROLE_MAX_NUM = 8
};
enum EPartyExpDistributionModes {
    PARTY_EXP_DISTRIBUTION_NON_PARITY = 0,
    PARTY_EXP_DISTRIBUTION_PARITY = 1,
    PARTY_EXP_DISTRIBUTION_MAX_NUM = 2
};
enum EPartyMessages {
    PM_ATTACK = 0,
    PM_RETURN = 1,
    PM_ATTACKED_BY = 2,
    PM_AGGRO_INCREASE = 3
};
class CParty;
class CPartyManager : public singleton<CPartyManager> {
public:
    typedef std::map<DWORD, CParty*> TPartyMap;
    typedef std::set<CParty*> TPCPartySet;

    CPartyManager();
    virtual ~CPartyManager();
    void Initialize();

    void EnablePCParty();
    void DisablePCParty();
    bool IsEnablePCParty();
    CParty* CreateParty(CHARACTER* leader);
    void DeleteParty(CParty* party);
    void DeleteAllParty();
    bool SetParty(CHARACTER* ch);
    void SetPartyMember(DWORD pid, CParty* party);

    void P2PLogin(DWORD pid, const char* name);
    void P2PLogout(DWORD pid);
    CParty* P2PCreateParty(DWORD pid);
    void P2PDeleteParty(DWORD pid);
    void P2PJoinParty(DWORD leader, DWORD pid, BYTE role = 0);
    void P2PQuitParty(DWORD pid);
private:
    TPartyMap m_map_pkParty;
    TPartyMap m_map_pkMobParty;
    TPCPartySet m_set_pkPCParty;
    bool m_bEnablePCParty;
};

class CParty {
public:
    struct SMember {
        CHARACTER* pCharacter; // +0
        bool bNear;           // +4
        BYTE bRole;           // +5
        BYTE bLevel;          // +6
        std::string strName;  // +8 (historical4-byte COW string)
    };
    typedef SMember TMember;
    typedef std::map<DWORD,TMember> TMemberMap;
    typedef std::map<std::string,int> TFlagMap;
public:
    // Template shape proved by emitted return-T/argument-T instantiation.
    template<class Func>
    Func ForEachNearMember(Func function)
    {
        for (const auto& entry : m_memberMap) {
            if (entry.second.pCharacter && entry.second.bNear)
                function(entry.second.pCharacter);
        }
        return function;
    }
    // 08137f90 passes each slot, including unlinked members, and returns the functor.
    template<class Func>
    Func ForEachMemberPtr(Func function)
    {
        for (const auto& entry : m_memberMap)
            function(entry.second.pCharacter);
        return function;
    }
    template<class Func>
    Func ForEachOnlineMember(Func function)
    {
        for (TMemberMap::iterator it = m_memberMap.begin(); it != m_memberMap.end(); ++it) {
            if (it->second.pCharacter)
                function(it->second.pCharacter);
        }
        return function;
    }
    CParty();
    virtual ~CParty();
    void P2PJoin(long unsigned int);
    void P2PQuit(long unsigned int);
    virtual void Join(long unsigned int);
    void Quit(long unsigned int);
    void Link(CHARACTER*);
    void Unlink(CHARACTER*);
    void ChatPacketToAllMember(unsigned char, const char*, ...);
    void UpdateOnlineState(long unsigned int, const char*);
    void UpdateOfflineState(long unsigned int);
    DWORD GetLeaderPID();
    CHARACTER* GetLeaderCharacter();
    CHARACTER* GetLeader() { return m_pkChrLeader; }
    DWORD GetMemberCount();
    DWORD GetNearMemberCount();
    bool IsMember(long unsigned int);
    bool IsNearLeader(long unsigned int);
    bool IsPositionNearLeader(CHARACTER*);
    void SendMessage(CHARACTER*, unsigned char, long unsigned int, long unsigned int);
    void SendPartyJoinOneToAll(long unsigned int);
    void SendPartyJoinAllToOne(CHARACTER*);
    void SendPartyRemoveOneToAll(long unsigned int);
    void SendPartyInfoOneToAll(long unsigned int);
    void SendPartyInfoOneToAll(CHARACTER*);
    void SendPartyInfoAllToOne(CHARACTER*);
    void SendPartyLinkOneToAll(CHARACTER*);
    void SendPartyLinkAllToOne(CHARACTER*);
    void SendPartyUnlinkOneToAll(CHARACTER*);
    int GetPartyBonusExpPercent();
    int GetPartyBonusAttackGrade();
    int GetPartyBonusDefenseGrade();
    int ComputePartyBonusExpPercent();
    int ComputePartyBonusAttackGrade();
    int ComputePartyBonusDefenseGrade();
    void Update();
    int GetExpBonusPercent();
    bool SetRole(long unsigned int, unsigned char, bool);
    BYTE GetRole(long unsigned int);
    bool IsRole(long unsigned int, unsigned char);
    BYTE GetMemberMaxLevel();
    BYTE GetMemberMinLevel();
    void ComputeRolePoint(CHARACTER*, unsigned char, bool);
    void HealParty();
    void SummonToLeader(long unsigned int);
    void SetPCParty(bool);
    CHARACTER* GetNextOwnership(CHARACTER*, long int, long int);
    void SetFlag(const std::string&, int);
    int GetFlag(const std::string&);
    void SetDungeon(CDungeon*);
    CDungeon* GetDungeon();
    BYTE CountMemberByVnum(long unsigned int);
    void SetParameter(int);
    int GetExpDistributionMode();
    void SetExpCentralizeCharacter(long unsigned int);
    CHARACTER* GetExpCentralizeCharacter();
    void RequestSetMemberLevel(long unsigned int, unsigned char);
    void P2PSetMemberLevel(long unsigned int, unsigned char);
protected:
    void IncreaseOwnership();
    virtual void Initialize();
    void Destroy();
    void RemovePartyBonus();
    void RemoveBonus();
    void RemoveBonusForOne(long unsigned int);
    void SendParameter(CHARACTER*);
    void SendParameterToAll();
protected:
    TMemberMap m_memberMap; // +4
    DWORD m_dwLeaderPID; // +16
    CHARACTER* m_pkChrLeader; // +20
    LPEVENT m_eventUpdate; // +24
    TMemberMap::iterator m_itNextOwner; // +28
private:
    int m_iExpDistributionMode; // +32
    CHARACTER* m_pkChrExpCentralize; // +36
    DWORD m_dwPartyStartTime; // +40
    DWORD m_dwPartyHealTime; // +44
    bool m_bPartyHealReady; // +48
    bool m_bCanUsePartyHeal; // +49
    int m_anRoleCount[8]; // +52
    int m_anMaxRole[8]; // +84
    int m_iLongTimeExpBonus; // +116
    int m_iLeadership; // +120
    int m_iExpBonus; // +124
    int m_iAttBonus; // +128
    int m_iDefBonus; // +132
    int m_iCountNearPartyMember; // +136
    bool m_bPCParty; // +140
    TFlagMap m_map_iFlag; // +144
    CDungeon* m_pkDungeon; // +156
};
#endif
