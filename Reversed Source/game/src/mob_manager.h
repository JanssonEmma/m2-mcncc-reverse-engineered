// Complete old CMob data layout with recovered lifecycle, pointer clone and splash entries.
#ifndef MCNCC_GAME_MOB_MANAGER_H
#define MCNCC_GAME_MOB_MANAGER_H
#include <vector>
#include <map>
#include <string>
#include "typedef.h"
#include "../../common/singleton.h"
#include "../../common/tables.h"
typedef struct SMobSplashAttackInfo {
    DWORD dwTiming;
    DWORD dwHitDistance;
    SMobSplashAttackInfo(DWORD dwTiming, DWORD dwHitDistance);
} TMobSplashAttackInfo;
typedef struct SMobSkillInfo {
    DWORD dwSkillVnum;
    BYTE bSkillLevel;
    std::vector<TMobSplashAttackInfo> vecSplashAttack;
} TMobSkillInfo;
class CMob {
public:
    CMob();
    explicit CMob(const CMob* mob);
    ~CMob();
    TMobTable m_table;
    TMobSkillInfo m_mobSkillInfo[5];
    void AddSkillSplash(int iIndex, DWORD dwTiming, DWORD dwHitDistance);
};
//All old by-value instance fields (target24 bytes).
class CMobInstance {
public:
    CMobInstance();
    PIXEL_POSITION m_posLastAttacked;
    DWORD m_dwLastAttackedTime;
    DWORD m_dwLastWarpTime;
    bool m_IsBerserk;
    bool m_IsGodSpeed;
    bool m_IsRevive;
};
class CMobGroup {
    friend class CMobManager;
public:
    const std::vector<DWORD>& GetMemberVector() { return m_vec_dwMemberVnum; }
protected:
    DWORD m_dwVnum;
    std::string m_stName;
    std::vector<DWORD> m_vec_dwMemberVnum;
};

class CMobGroupGroup {
    friend class CMobManager;
protected:
    DWORD m_dwVnum;
    std::vector<DWORD> m_vec_dwMemberVnum;
    std::vector<int> m_vec_iProbs;
};
class CHARACTER;
// Complete old manager data (target 64 bytes).
class CMobManager : public singleton<CMobManager> {
public:
    typedef std::map<DWORD,CMob*>::iterator iterator;
    const iterator begin() { return m_map_pkMobByVnum.begin(); }
    const iterator end() { return m_map_pkMobByVnum.end(); }
    CMobManager();
    virtual ~CMobManager();
    bool Initialize(TMobTable* table, int size);
    const CMob* Get(DWORD vnum);
    const CMob* Get(const char* name,bool abbreviation);
    CMobGroup* GetGroup(DWORD vnum);
    bool LoadGroup(const char* filename);
    bool LoadGroupGroup(const char* filename);
    DWORD GetGroupFromGroupGroup(DWORD vnum);
    void IncRegenCount(BYTE type, DWORD vnum, int count, int seconds);
    void DumpRegenCount(const char* filename);
private:
    void RebindMobProto(CHARACTER* character);
    std::map<DWORD,CMob*> m_map_pkMobByVnum;
    std::map<std::string,CMob*> m_map_pkMobByName;
    std::map<DWORD,CMobGroup*> m_map_pkMobGroup;
    std::map<DWORD,CMobGroupGroup*> m_map_pkMobGroupGroup;
    std::map<DWORD,double> m_mapRegenCount;
};
#endif
