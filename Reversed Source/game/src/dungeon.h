#ifndef MCNCC_GAME_DUNGEON_H
#define MCNCC_GAME_DUNGEON_H
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
#include <cstdint>
#include <map>
#include <list>
#include <set>
#include "event.h"
#include "sectree_manager.h"
struct regen;
class CHARACTER;
class CParty;
class CDungeon {
public:
    ~CDungeon();
    long GetMapIndex() const { return m_lMapIndex; }
    void AddRegen(regen* record);
    void ClearRegen();
    void IncMember(CHARACTER* character);
    void DecMember(CHARACTER* character);
    void IncPartyMember(CParty* party, CHARACTER* character);
    void DecPartyMember(CParty* party, CHARACTER* character);
    void QuitParty(CParty* party);
    void IncMonster();
    void IncKillCount(CHARACTER* killer, CHARACTER* victim);
    void DeadCharacter(CHARACTER* character);
    void DecMonster();
    void CheckEliminated();
    void Join(CHARACTER* character);
    void JoinParty(CParty* party);
    void SendDestPositionToParty(CParty* party, long x, long y);
    void ExitAll();
    void ExitAllToStartPosition();
    void KillAll();
    void Purge();
    void Spawn(DWORD vnum, const char* position);
    CHARACTER* SpawnMob(DWORD vnum, int x, int y);
    CHARACTER* SpawnGroup(DWORD vnum, long x, long y, float radius, bool aggressive, int count);
    void SpawnUnique(const char* key, DWORD vnum, const char* position);
    void SpawnStoneDoor(const char* key, const char* position);
    void SpawnWoodenDoor(const char* key, const char* position);
    void SpawnNameMob(DWORD vnum, int x, int y, const char* name);
    void SpawnGotoMob(long fromX, long fromY, long toX, long toY);
    void SpawnMoveUnique(const char* key, DWORD vnum, const char* from, const char* to);
    void SpawnMoveGroup(DWORD vnum, const char* from, const char* to, int count);
    void SetUnique(const char* key, DWORD vid);
    void PurgeUnique(const std::string& key);
    void KillUnique(const std::string& key);
    bool IsUniqueDead(const std::string& key);
    double GetUniqueHpPerc(const std::string& key);
    void UniqueSetDefGrade(const std::string& key, int grade);
    void UniqueSetHP(const std::string& key, int hp);
    void UniqueSetMaxHP(const std::string& key, int maxHP);
    void Notice(const char* message);
    void SetFlag(std::string name, int value);
    int GetFlag(std::string name);
    int GetKillStoneCount();
    int GetKillMobCount();
    int CountMonster() const { return m_iMonsterCount; }
    int CountRealMonster();
    void UsePotion(CHARACTER* character);
    bool IsUsePotion();
    void UseRevive(CHARACTER*);
    bool IsUseRevive();

    template <class Function>
    Function ForEachMember(Function function)
    {
        for (CHARACTER* character : m_set_pkCharacter)
            function(character);
        return function;
    }
    void JumpAll(long from, int x, int y);
    void JumpToEliminateLocation();
    void SpawnRegen(const char* filename, bool once);
    void SetExitAllAtEliminate(long time);
    void SetWarpAtEliminate(long time, long map, int x, int y, const char* filename);

protected:
    CDungeon(long originalMap, long map);
    void Initialize();

private:
    std::uint32_t m_lOrigMapIndex;
    std::uint32_t m_lMapIndex;
    std::set<CHARACTER*> m_set_pkCharacter;
    std::map<std::string, int> m_map_Flag;
    std::map<CParty*, int> m_map_pkParty;
    TAreaMap& m_map_Area;
    std::map<std::string, CHARACTER*> m_map_UniqueMob;
    int m_iMobKill;
    int m_iStoneKill;
    bool m_bUsePotion;
    bool m_bUseRevive;
    int m_iMonsterCount;
    bool m_bExitAllAtEliminate;
    bool m_bWarpAtEliminate;
    int m_iWarpDelay;
    std::int32_t m_lWarpMapIndex;
    std::int32_t m_lWarpX;
    std::int32_t m_lWarpY;
    std::string m_stRegenFile;
    std::list<regen*> m_regen;
    void CheckDestroy();
    LPEVENT deadEvent;

    friend class CDungeonManager;
    friend EVENTFUNC(dungeon_dead_event);
};
class CDungeonManager : public singleton<CDungeonManager> {
public:
    CDungeonManager();
    virtual ~CDungeonManager();
    CDungeon* Create(long originalMap);
    CDungeon* Find(long map);
    void Destroy(CDungeon* dungeon);
private:
    std::map<std::uint32_t, CDungeon*> m_map_pkDungeon;
};
#endif
