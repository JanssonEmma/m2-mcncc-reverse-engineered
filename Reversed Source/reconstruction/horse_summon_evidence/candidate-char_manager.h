// Complete old172-byte target data declaration; nonvirtual API recovery is partial.
#ifndef MCNCC_GAME_CHAR_MANAGER_H
#define MCNCC_GAME_CHAR_MANAGER_H
#include <map>
#include <set>
#include <string>
#include <ext/hash_map>
#include "../../common/singleton.h"
#include "vid.h"
#include "../../common/stl.h"
#include "../../libthecore/include/typedef.h"
class CHARACTER;
typedef CHARACTER* LPCHARACTER;
typedef std::set<LPCHARACTER> CHARACTER_SET;
class CHARACTER_MANAGER : public singleton<CHARACTER_MANAGER> {
public:
    typedef __gnu_cxx::hash_map<std::string,LPCHARACTER,stringhash,
        std::equal_to<std::string>,std::allocator<LPCHARACTER> > NAME_MAP;
    CHARACTER_MANAGER();
    virtual ~CHARACTER_MANAGER();
    void Destroy();
    void DestroyCharacter(LPCHARACTER character);
    LPCHARACTER CreateCharacter(const char* name,DWORD playerID=0);
    DWORD AllocVID();
    LPCHARACTER SpawnMob(DWORD vnum,long map,long x,long y,long z,bool spawnMotion,int rotation,bool show);
    LPCHARACTER Find(DWORD vid);
    LPCHARACTER Find(const VID& vid);
    LPCHARACTER FindByPID(DWORD pid);
    LPCHARACTER FindPC(const char* name);
    bool AddToStateList(LPCHARACTER character);
    void RemoveFromStateList(LPCHARACTER character);
    void SelectStone(LPCHARACTER character);
    void DelayedSave(LPCHARACTER character);
    void RegisterForMonsterLog(LPCHARACTER character);
    void PacketMonsterLog(LPCHARACTER source,const void* data,int bytes);
    void UnregisterForMonsterLog(LPCHARACTER character);
    CHARACTER_SET* GetCharacterSetByRaceNum(DWORD race);
    void RegisterRaceNum(DWORD race);
    void RegisterRaceNumMap(LPCHARACTER character);
    void UnregisterRaceNumMap(LPCHARACTER character);
private:
    int m_iMobItemRate;
    int m_iMobDamageRate;
    int m_iMobGoldAmountRate;
    int m_iMobGoldDropRate;
    int m_iMobExpRate;
    int m_iMobItemRatePremium;
    int m_iMobGoldAmountRatePremium;
    int m_iMobGoldDropRatePremium;
    int m_iMobExpRatePremium;
    int m_iUserDamageRate;
    int m_iUserDamageRatePremium;
    int m_iVIDCount;
    std::map<DWORD,LPCHARACTER> m_map_pkChrByVID;
    std::map<DWORD,LPCHARACTER> m_map_pkChrByPID;
    NAME_MAP m_map_pkPCChr;
    CHARACTER_SET m_set_pkChrState;
    CHARACTER_SET m_set_pkChrForDelayedSave;
    CHARACTER_SET m_set_pkChrMonsterLog;
    LPCHARACTER m_pkChrSelectedStone;
    std::map<DWORD,DWORD> m_map_dwMobKillCount;
    std::set<DWORD> m_set_dwRegisteredRaceNum;
    std::map<DWORD,CHARACTER_SET> m_map_pkChrByRaceNum;
};
#endif
