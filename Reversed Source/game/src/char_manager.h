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
class CDungeon;
namespace quest { class CQuestManager; }
struct regen;
typedef CHARACTER* LPCHARACTER;
typedef std::set<LPCHARACTER> CHARACTER_SET;
class CHARACTER_MANAGER : public singleton<CHARACTER_MANAGER> {
    friend class P2P_MANAGER;
    friend class CMobManager;
    friend class quest::CQuestManager;
public:
    typedef __gnu_cxx::hash_map<std::string,LPCHARACTER,stringhash,
        std::equal_to<std::string>,std::allocator<LPCHARACTER> > NAME_MAP;
    CHARACTER_MANAGER();
    virtual ~CHARACTER_MANAGER();
    void Destroy();
    void GracefulShutdown();
    void ProcessDelayedSave();
    void NoticeToMyEmpireInMap(int mapIndex, BYTE empire, const char* text);
    void SayInMap(int mapIndex, const std::string& script);
    void SayInMapinMyEmpire(int mapIndex, BYTE empire, const std::string& script);
    void Update(int pulse);
    void DestroyCharacter(LPCHARACTER character);
    LPCHARACTER SpawnMobRandomPosition(DWORD vnum, long map);
    LPCHARACTER SpawnGroup(DWORD vnum, long map, int sx, int sy, int ex, int ey,
                           regen* record, bool aggressive, CDungeon* dungeon);
    bool SpawnMoveGroup(DWORD vnum, long map, int sx, int sy, int ex, int ey, int tx, int ty,
                        regen* record, bool aggressive);
    bool SpawnGroupGroup(DWORD vnum, long map, int sx, int sy, int ex, int ey,
                         regen* record, bool aggressive, CDungeon* dungeon);
    LPCHARACTER SpawnMobRange(DWORD vnum, long map, int sx, int sy, int ex, int ey,
                              bool exception, bool spawnMotion, bool aggressive);
    LPCHARACTER CreateCharacter(const char* name,DWORD playerID=0);
    LPCHARACTER SpawnMob(DWORD vnum,long map,long x,long y,long z,bool spawnMotion,int rotation,bool show);
    DWORD AllocVID();
    LPCHARACTER Find(DWORD vid);
    LPCHARACTER Find(const VID& vid);
    LPCHARACTER FindByPID(DWORD pid);
    int GetUserDamageRate(LPCHARACTER character);
    LPCHARACTER FindPC(const char* name);
    LPCHARACTER FindSpecifyPC(unsigned jobFlags, long map, LPCHARACTER except, int minLevel, int maxLevel);
    template <class Func> Func for_each_pc(Func callback)
    {
        for (std::map<DWORD, LPCHARACTER>::iterator it = m_map_pkChrByPID.begin();
                it != m_map_pkChrByPID.end(); ++it)
            callback(it->second);
        return callback;
    }
    bool AddToStateList(LPCHARACTER character);
    void RemoveFromStateList(LPCHARACTER character);
    void SelectStone(LPCHARACTER character);
    int GetMobItemRate(LPCHARACTER character);
    int GetMobGoldAmountRate(LPCHARACTER character);
    int GetMobGoldDropRate(LPCHARACTER character);
    int GetMobExpRate(LPCHARACTER character);
    void KillLog(DWORD vnum);
    int GetMobDamageRate(LPCHARACTER character);
    void DelayedSave(LPCHARACTER character);
    bool FlushDelayedSave(LPCHARACTER character);
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
