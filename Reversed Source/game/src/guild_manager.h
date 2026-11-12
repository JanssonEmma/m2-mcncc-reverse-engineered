#ifndef MCNCC_GAME_GUILD_MANAGER_H
#define MCNCC_GAME_GUILD_MANAGER_H

#include "../../common/singleton.h"
#include "../../libguild/GuildLib.h"
#include "../../libthecore/include/typedef.h"
#include <map>
#include <set>
#include <utility>
#include <vector>

class CHARACTER;
void SendGuildWarScore(DWORD guild, DWORD opponent, int delta, int betDelta);
class CGuild;
struct SGuildCreateParameter;
typedef SGuildCreateParameter TGuildCreateParameter;
#include "../../common/tables.h"
#include <string>

class CGuildWarReserveForGame
{
public:
    TGuildWarReserve data;
    std::map<std::string, std::pair<DWORD, DWORD>> mapBet;
};

class CGuildUtil : public Guild::IUtil
{
public:
    virtual ~CGuildUtil();
    virtual Guild::IImage* NewImage();
    virtual void DeleteImage(Guild::IImage* image);
    virtual Guild::CRC CalculateCRC32(const void* data, unsigned int size);
    virtual bool Compress(const void* source, unsigned int sourceSize,
                          void* destination, unsigned int& destinationSize);
    virtual bool Uncompress(const void* source, unsigned int sourceSize,
                            void* destination, unsigned int& destinationSize);
};

class CGuildManager : public singleton<CGuildManager>
{
public:
    CGuildManager();
    void Initialize();
    Guild::IMarkManager* GetMarkManager() const { return m_pMarkManager; }
    virtual ~CGuildManager();
    void StopAllGuildWar();

    template <class Func> Func for_each_war(Func callback)
    {
        for (const auto& war : m_GuildWar)
            callback(war.first, war.second);
        return callback;
    }
    void GetHighRankString(DWORD guild, char* buffer);
    void GetAroundRankString(DWORD guild, char* buffer);
    void ReserveWarAdd(TGuildWarReserve* packet);
    void ReserveWarDelete(DWORD id);
    void ReserveWarBet(TPacketGDGuildWarBet* packet);
    bool IsBet(DWORD id, const char* login);
    std::vector<CGuildWarReserveForGame*>& GetReserveWarRef();
    int GetRank(CGuild* guild);
    CGuild* FindGuild(DWORD guildID);
    void DisbandGuild(DWORD guildId);
    DWORD CreateGuild(TGuildCreateParameter& parameter);
    int GetDisbandDelay();
    void SkillRecharge();
    int GetWithdrawDelay();
    CGuild* FindGuildByName(const std::string& name);
    void Link(DWORD pid, CGuild* guild);
    void Unlink(DWORD pid);
    CGuild* GetLinkedGuild(DWORD pid);
    void DeclareWar(DWORD first, DWORD second, BYTE type);
    void RefuseWar(DWORD first, DWORD second);
    void ReserveWar(DWORD first, DWORD second, BYTE type);
    void WaitStartWar(DWORD first, DWORD second);
    void StartWar(DWORD first, DWORD second);
    bool EndWar(DWORD first, DWORD second);
    void WarOver(DWORD first, DWORD second, bool draw);
    void CancelWar(DWORD first, DWORD second);
    void SendGuildNames(CHARACTER* ch);
    void SendGuildWar(CHARACTER* ch);
    CGuild* TouchGuild(DWORD guildId);
    void LoginMember(CHARACTER* ch);
    void Kill(CHARACTER* killer, CHARACTER* victim);
    void RequestWarOver(DWORD guild1, DWORD guild2, DWORD winner, long reward);
    void RequestEndWar(DWORD guild1, DWORD guild2);
    void RequestCancelWar(DWORD first, DWORD second);
    void ShowGuildWarList(CHARACTER* character);
    void P2PLoginMember(DWORD pid);
    void P2PLogoutMember(DWORD pid);

private:
    typedef std::map<DWORD, CGuild*> TGuildMap;
    typedef std::set<std::pair<DWORD, DWORD> > TGuildWarContainer;
    typedef std::map<std::pair<DWORD, DWORD>, DWORD> TGuildWarEndTimeContainer;

    TGuildMap m_mapGuild;
    TGuildWarContainer m_GuildWar;
    TGuildWarEndTimeContainer m_GuildWarEndTime;
    TGuildMap m_map_pkGuildByPID;
    std::map<DWORD, CGuildWarReserveForGame*> m_map_kReserveWar;
    std::vector<CGuildWarReserveForGame*> m_vec_kReserveWar;
    Guild::IMarkManager* m_pMarkManager;
    CGuildUtil m_util;

    void LoadGuild(DWORD guildID);
    friend class CGuild;
struct SGuildCreateParameter;
typedef SGuildCreateParameter TGuildCreateParameter;
};
#endif
