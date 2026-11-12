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
class CMobGroup;
class CMobGroupGroup;
//Complete old manager data (target64 bytes); loading/rebinding API remains partial.
class CMobManager : public singleton<CMobManager> {
public:
    typedef std::map<DWORD,CMob*>::iterator iterator;
    const iterator begin() { return m_map_pkMobByVnum.begin(); }
    const iterator end() { return m_map_pkMobByVnum.end(); }
    CMobManager();
    virtual ~CMobManager();
    const CMob* Get(DWORD vnum);
    const CMob* Get(const char* name,bool abbreviation);
    CMobGroup* GetGroup(DWORD vnum);
private:
    std::map<DWORD,CMob*> m_map_pkMobByVnum;
    std::map<std::string,CMob*> m_map_pkMobByName;
    std::map<DWORD,CMobGroup*> m_map_pkMobGroup;
    std::map<DWORD,CMobGroupGroup*> m_map_pkMobGroupGroup;
    std::map<DWORD,double> m_mapRegenCount;
};
#endif
