// Complete target guild data; guild database/war lifecycle remains unrecovered.
#ifndef MCNCC_GAME_GUILD_H
#define MCNCC_GAME_GUILD_H
#include <map>
#include <set>
#include <string>
#include "../../libthecore/include/typedef.h"
#include "event.h"
struct _SQLMsg;
class CHARACTER;
struct TPacketDGGuildMember;
typedef struct SGuildMember {
    DWORD pid;
    BYTE grade,is_general,job,level;
    DWORD offer_exp;
    BYTE _dummy;
    std::string name;
    SGuildMember(CHARACTER* ch,BYTE grade,DWORD offer_exp);
    SGuildMember(DWORD pid,BYTE grade,BYTE is_general,BYTE job,BYTE level,DWORD offer_exp,char* name);
} TGuildMember;
typedef struct SGuildGrade {
    char grade_name[9];
    BYTE auth_flag;
} TGuildGrade;
typedef struct SGuildData {
    DWORD guild_id,master_pid,exp;
    BYTE level;
    char name[13];
    TGuildGrade grade_array[15];
    BYTE skill_point,abySkill[12];
    int power,max_power,ladder_point,win,draw,loss,gold;
} TGuildData;
typedef struct SGuildWar {
    DWORD war_start_time,score,state;
    BYTE type;
    DWORD map_index;
    SGuildWar(BYTE type);
} TGuildWar;
typedef struct SGuildCreateParameter {
    CHARACTER* master;
    char name[13];
} TGuildCreateParameter;
class CGuild {
public:
    CGuild(TGuildCreateParameter& parameter);
    explicit CGuild(DWORD guild_id);
    ~CGuild();
    // Old linkage is _ZN6CGuild5GetIDEv (non-const).
    int GetGuildWarWinCount() const { return m_data.win; }
    int GetGuildWarDrawCount() const { return m_data.draw; }
    int GetGuildWarLossCount() const { return m_data.loss; }
    int GetLadderPoint() const { return m_data.ladder_point; }
    void SetLadderPoint(int point);
    void SetWarRecords(int wins, int draws, int losses);
    void SkillLevelUp(DWORD vnum);
    void SendDBSkillUpdate(int amount);
    int GetMemberCount() { return static_cast<int>(m_member.size()); }
    int GetTotalLevel();
    void GuildWarEntryAccept(DWORD opponent, CHARACTER* ch);
    DWORD GetMasterPID() { return m_data.master_pid; }
    BYTE GetLevel() const { return m_data.level; }
    void SetMemberCountBonus(int bonus);
    int GetMaxMemberCount();
    bool HasLand();
    void SendGuildInfoPacket(CHARACTER* ch);
    DWORD GetID() { return m_data.guild_id; }
    const char* GetName() const { return m_data.name; }
    TGuildMember* GetMember(DWORD pid);
    void LevelChange(DWORD pid,BYTE level);
    DWORD UnderAnyWar(BYTE type);
    int GetGuildWarType(DWORD opponent);
    int GetGuildWarState(DWORD opponent);
    DWORD GetWarStartTime(DWORD opponent);
    int GetGuildGold() const { return m_data.gold; }
    static int GetWarPrice(BYTE type);
    bool CanStartWar(BYTE type);
    void RequestDeclareWar(DWORD opponent, BYTE type);
    void RequestRefuseWar(DWORD opponent);
    bool UnderWar(DWORD opponent);
    void Chat(const char* text);
    void P2PChat(const char* text);
    bool DeclareWar(DWORD opponent, BYTE type, BYTE state);
    void RefuseWar(DWORD opponent);
    void ReserveWar(DWORD opponent, BYTE type);
    bool CheckStartWar(DWORD opponent);
    bool WaitStartWar(DWORD opponent);
    void StartWar(DWORD opponent);
    void EndWar(DWORD opponent);
    DWORD GetGuildWarMapIndex(DWORD opponent);
    BYTE GetMemberAuth(DWORD pid);
    void RequestWithdrawMoney(CHARACTER* ch, int gold);
    CHARACTER* GetMasterCharacter();
    void RecvMoneyChange(int gold);
    void RecvWithdrawMoneyGive(int gold);
    void AddComment(CHARACTER* ch, const std::string& comment);
    void DeleteComment(CHARACTER* ch, DWORD id);
    void RefreshComment(CHARACTER* ch);
    void ChangeGradeName(BYTE grade, const char* name);
    void ChangeGradeAuth(BYTE grade, BYTE auth);
    void SendGradeName(BYTE grade);
    void SendGradeAuth(BYTE grade);
    void P2PChangeGrade(BYTE grade);
    void ChangeMemberGrade(DWORD pid, BYTE grade);
    void ChangeMemberData(DWORD pid, DWORD offer, BYTE level, BYTE grade);
    bool ChangeMemberGeneral(DWORD pid, BYTE isGeneral);
    void Invite(CHARACTER* inviter, CHARACTER* invitee);
    void InviteAccept(CHARACTER* invitee);
    void InviteDeny(DWORD pid);
    bool ChargeSP(CHARACTER* ch, int amount);
    void RequestDepositMoney(CHARACTER* ch, int gold);
    void RequestDisband(DWORD pid);
    bool RequestRemoveMember(DWORD pid);
    bool RemoveMember(DWORD pid);
    void Disband();
    void RequestAddMember(CHARACTER* character, int grade);
    void AddMember(TPacketDGGuildMember* packet);
    void SetGuildWarMapIndex(DWORD opponent, long mapIndex);
    int GetSkillLevel(DWORD vnum);
    void RestoreGuildPower() { m_data.power = m_data.max_power; }
    void UpdateSkill(BYTE points, BYTE* levels);
    void UseSkill(DWORD vnum, CHARACTER* character, DWORD targetPid);
    void SkillUsableChange(DWORD vnum, bool usable);
    void GuildPointChange(BYTE type, int amount, bool save);
    bool OfferExp(CHARACTER* character, long amount);
    void SkillRecharge();
    void LoginMember(CHARACTER* ch);
    void SetWarScoreAgainstTo(DWORD opponent, int score);
    void Packet(const void* data, int size);
    int GetWarScoreAgainstTo(DWORD opponent);
    void P2PLoginMember(DWORD pid);
    void P2PLogoutMember(DWORD pid);
    void LogoutMember(CHARACTER* ch);
private:
    void SendAllGradePacket(CHARACTER* ch);
    void SendListPacket(CHARACTER* ch);
    void SendMemberData(TGuildMember& member);
    void SendOnlineRemoveOnePacket(DWORD pid);
    void SendListOneToAll(CHARACTER* character);
    void SendListOneToAll(DWORD pid);
    void SendSkillInfoPacket(CHARACTER* ch) const;
    void Initialize();
    void Load(DWORD guildId);
    void LoadGuildData(_SQLMsg* message);
    void LoadGuildGradeData(_SQLMsg* message);
    void LoadGuildMemberData(_SQLMsg* message);
    void ComputeGuildPoints();
    void SaveSkill();
    void SaveLevel();
    void __P2PUpdateGrade(_SQLMsg* message);
    enum class GuildJoinErrCode {
        None = 0,
        WithdrawPenalty = 1,
        DisbandPenalty = 2,
        AlreadyJoined = 3,
        GuildFull = 4,
    };
    GuildJoinErrCode VerifyGuildJoinableCondition(CHARACTER* invitee);
    bool CanJoinGuild(CHARACTER& invitee, CHARACTER& recipient);
    void RefreshCommentForce(CHARACTER* ch);
    void SaveMember(DWORD pid);
    void AdvanceLevel(int level);
    void NotifyGuildMaster(const char* message);
    void ChangeLadderPoint(int amount);
    void ChangeGuildPower(int amount, bool save);
    void GuildWarPacket(DWORD opponent, BYTE type, BYTE state);
    void GuildWarEntryAsk(DWORD opponent);
    void SendEnemyGuild(CHARACTER* ch);
    void SendLoginPacket(CHARACTER* character, CHARACTER* login);
    void SendLoginPacket(CHARACTER* character, DWORD pid);
    void SendLogoutPacket(CHARACTER* character, CHARACTER* logout);
    void SendLogoutPacket(CHARACTER* character, DWORD pid);
    TGuildData m_data;
    int m_general_count;
    int m_iMemberCountBonus;
    std::map<DWORD,TGuildMember> m_member;
    std::set<CHARACTER*> m_memberOnline;
    std::set<DWORD> m_memberP2POnline;
    std::map<DWORD,TGuildWar> m_EnemyGuild;
    std::map<DWORD,DWORD> m_mapGuildWarEndTime;
    bool abSkillUsable[12];
    std::map<DWORD,LPEVENT> m_GuildInviteEventMap;
};
#endif
