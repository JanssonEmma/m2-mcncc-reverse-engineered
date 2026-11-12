#ifndef MCNCC_GAME_WAR_MAP_H
#define MCNCC_GAME_WAR_MAP_H
#include "../../common/singleton.h"
#include <map>
#include "../../common/length.h"
#include "typedef.h"
#include "../../libthecore/include/typedef.h"
#include "constants.h"
#include "event.h"
#include <set>

class CHARACTER;
class CItem;
class CGuild;
class DESC;

struct SWarMapInfo;
class CWarMap;
struct SWarMapInfo
{
    BYTE bType;
    long lMapIndex;
    PIXEL_POSITION posStart[3];
};

class CWarMap
{
public:
    CWarMap(long mapIndex, const TGuildWarInfo& war, SWarMapInfo* map, DWORD first, DWORD second);
    ~CWarMap();
    long GetMapIndex();
    BYTE GetType();
    DWORD GetGuildID(BYTE index);
    DWORD GetGuildOpponent(CHARACTER* character);
    void AddFlagBase(BYTE index, DWORD x, DWORD y);
    void ExitAll();
    bool SetEnded();
    void SetBeginEvent(LPEVENT event);
    void SetEndEvent(LPEVENT event);
    void SetTimeoutEvent(LPEVENT event);
    void Draw();
    void IncMember(CHARACTER* ch);
    void DecMember(CHARACTER* ch);
    void OnKill(CHARACTER* killer, CHARACTER* victim);
    void UsePotion(CHARACTER* character, CItem* item);
    CGuild* GetGuild(BYTE index);
    void RemoveFlag(BYTE index);
    bool IsFlagOnBase(BYTE index);
    void SetResetFlagEvent(LPEVENT event);
    void ResetFlag();
    void AddFlag(BYTE index, DWORD x, DWORD y);
    void CheckWarEnd();
    void Timeout();
    void UpdateScore(DWORD first, int firstScore, DWORD second, int secondScore);
    bool CheckScore();
    DWORD GetWinnerGuild();
    int GetRewardGold(BYTE winner);
    void Notice(const char* text);
    bool GetTeamIndex(DWORD guild, BYTE& index);
    void SendWarPacket(DESC* desc);
    void SendScorePacket(BYTE index, DESC* desc = nullptr);
    void Packet(const void* data, int size);

private:
    struct STeamData
    {
        DWORD dwID;
        CGuild* pkGuild;
        int iMemberCount;
        int iUsePotionPrice;
        int iScore;
        CHARACTER* pkChrFlag;
        CHARACTER* pkChrFlagBase;
        std::set<DWORD> set_pidJoiner;

        void Initialize();
        int GetAccumulatedJoinerCount();
        int GetCurJointerCount();
        void AppendMember(CHARACTER* ch);
        void RemoveMember(CHARACTER* ch);
    };

    void UpdateUserCount();

    SWarMapInfo m_kMapInfo;
    bool m_bEnded;
    LPEVENT m_pkBeginEvent;
    LPEVENT m_pkTimeoutEvent;
    LPEVENT m_pkEndEvent;
    LPEVENT m_pkResetFlagEvent;
    STeamData m_TeamData[2];
    int m_iObserverCount;
    DWORD m_dwStartTime;
    BYTE m_bTimeout;
    TGuildWarInfo m_WarInfo;
    std::set<CHARACTER*> m_set_pkChr;
};

class CWarMapManager : public singleton<CWarMapManager> {
public:
    CWarMapManager();
    virtual ~CWarMapManager();
    long CreateWarMap(const TGuildWarInfo& war, DWORD first, DWORD second);
    void DestroyWarMap(CWarMap* map);
    void OnShutdown();
    CWarMap* Find(long mapIndex);
    SWarMapInfo* GetWarMapInfo(long mapIndex);
    bool LoadWarMapInfo(const char* filename);
    bool GetStartPosition(long mapIndex, BYTE index, PIXEL_POSITION& position);
    bool IsWarMap(long mapIndex);
private:
    std::map<long, SWarMapInfo*> m_map_kWarMapInfo;
    std::map<long, CWarMap*> m_mapWarMap;
};
#endif
