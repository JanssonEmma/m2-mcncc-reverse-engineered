// Complete target guild data; guild database/war lifecycle remains unrecovered.
#ifndef MCNCC_GAME_GUILD_H
#define MCNCC_GAME_GUILD_H
#include <map>
#include <set>
#include <string>
#include "../../libthecore/include/typedef.h"
#include "event.h"
class CHARACTER;
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
    DWORD GetMasterPID() { return m_data.master_pid; }
    DWORD GetID() { return m_data.guild_id; }
    TGuildMember* GetMember(DWORD pid);
    void LevelChange(DWORD pid,BYTE level);
    DWORD UnderAnyWar(BYTE type);
private:
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
