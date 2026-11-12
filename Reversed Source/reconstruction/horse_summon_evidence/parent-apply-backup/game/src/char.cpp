#include "constants.h"
#include <cstdlib>
#include "locale.hpp"
#include "party.h"
// Partial old CHARACTER recovery; behavior/evidence scope is tracked in reconstruction/.
#include "char.h"
#include "questmanager.h"
#include "crc32.h"
#include "../../libthecore/include/utils.h"
#include <cstring>
#include <cassert>
#include "desc.h"
#include "packet.h"
#include "buffer_manager.h"
#include "config.h"
#include "../../common/length.h"
#include "../../libthecore/include/log.h"
#include <cstdarg>
#include <cstdio>
#include "char_manager.h"
#include "../../libthecore/include/main.h"
#include "mob_manager.h"
#include "motion.h"
#include "item.h"
#include "guild.h"
#include "utils.h"
#include <cmath>

const char* CHARACTER::GetName()
{
    if (m_stName.empty())
        return m_pkMobData ? m_pkMobData->m_table.szLocaleName : "";
    return m_stName.c_str();
}

//08130630.
BYTE CHARACTER::GetCharType() { return m_bCharType; }

//08130300 /08130380: only a newly inserted state-list member receives a new pulse.
void CHARACTER::StartStateMachine(int nextPulse) {
    if(CHARACTER_MANAGER::instance().AddToStateList(this))
        m_dwNextStatePulse=static_cast<DWORD>(nextPulse)+static_cast<DWORD>(thecore_heart->pulse);
}
void CHARACTER::StopStateMachine() { CHARACTER_MANAGER::instance().RemoveFromStateList(this); }

//0812fe70. The old formatter return length is retained, including its unchecked truncation behavior.
void CHARACTER::ChatPacket(BYTE type,const char* format,...) {
    DESC* desc=GetDesc();
    if(!desc) return;
    char chatbuf[CHAT_MAX_LEN+1];
    va_list args; va_start(args,format);
    int length=std::vsnprintf(chatbuf,CHAT_MAX_LEN,format,args);
    va_end(args);
    TPacketGCChat packet;
    packet.header=4; packet.size=static_cast<WORD>(static_cast<unsigned int>(length)+10U);
    packet.type=type; packet.id=0; packet.bEmpire=desc->GetEmpire();
    if(type==5 && test_server) sys_log(0,"SNED COMMAND %s %s",GetName(),chatbuf);
    TEMP_BUFFER buffer;
    buffer.write(&packet,sizeof packet);
    buffer.write(chatbuf,static_cast<int>(static_cast<unsigned int>(length)+1U));
    desc->Packet(buffer.read_peek(),buffer.size());
}

//0812a4e0: polymorph takes precedence, and the public result truncates to WORD.
WORD CHARACTER::GetRaceNum() {
    if(m_dwPolymorphRace) return static_cast<WORD>(m_dwPolymorphRace);
    return m_pkMobData?static_cast<WORD>(m_pkMobData->m_table.dwVnum):static_cast<WORD>(m_points.job);
}

//08127400. Counter arithmetic follows the target's32-bit register wrap.
void CHARACTER::Create(const char* name,DWORD vid,bool isPC) {
    static int crc_counter=0x0002a30e; // ELF data at082409a0:172814, not a zero-initialized counter.
    crc_counter=static_cast<int>(static_cast<unsigned int>(crc_counter)+1U);
    char identity[129];
    std::snprintf(identity,128,"%s%p%d",name,static_cast<void*>(this),crc_counter);
    m_vid=VID(vid,get_crc32(identity,std::strlen(identity)));
    if(isPC) m_stName=name;
}
//081343c0/081343e0.
void CHARACTER::ResetStopTime() { m_dwStopTime=get_dword_time(); }
DWORD CHARACTER::GetStopTime() { return m_dwStopTime; }
//08136be0. The original stores pulse-scaled limits, then scales again on subtraction.
void CHARACTER::InitMC() {
    for(int i=0;i<MI_MAX;++i) m_dwMonarchCooltime[i]=static_cast<DWORD>(thecore_pulse());
    DWORD rate=static_cast<DWORD>(passes_per_sec);
    m_dwMonarchCooltimelimit[MI_HEAL]=rate*10U;
    m_dwMonarchCooltimelimit[MI_WARP]=rate*60U;
    m_dwMonarchCooltimelimit[MI_TRANSFER]=rate*60U;
    m_dwMonarchCooltimelimit[MI_TAX]=rate*604800U;
    m_dwMonarchCooltimelimit[MI_SUMMON]=rate*3600U;
    for(int i=0;i<MI_MAX;++i) m_dwMonarchCooltime[i]-=GetMCL(static_cast<MONARCH_INDEX>(i))*rate;
}
//08136cf0/08136e30/08136d10. Enum bounds are an original caller precondition.
DWORD CHARACTER::GetMC(MONARCH_INDEX index) { return m_dwMonarchCooltime[index]; }
DWORD CHARACTER::GetMCL(MONARCH_INDEX index) { return m_dwMonarchCooltimelimit[index]; }
void CHARACTER::SetMC(MONARCH_INDEX index) { m_dwMonarchCooltime[index]=static_cast<DWORD>(thecore_pulse()); }
//08136d30.
bool CHARACTER::IsMCOK(MONARCH_INDEX index) {
    int pulse=thecore_pulse();
    bool ready=static_cast<DWORD>(pulse)-GetMC(index)>=GetMCL(index);
    if(test_server) sys_log(0," Pulse %d cooltime %d , limit %d",pulse,static_cast<int>(GetMC(index)),static_cast<int>(GetMCL(index)));
    return ready;
}
//08136e50: unsigned division and subtraction, with no expiry clamp.
DWORD CHARACTER::GetMCLTime(MONARCH_INDEX index) {
    int pulse=thecore_pulse();
    if(test_server) sys_log(0," Pulse %d cooltime %d , limit %d",pulse,static_cast<int>(GetMC(index)),static_cast<int>(GetMCL(index)));
    DWORD rate=static_cast<DWORD>(passes_per_sec);
    return GetMCL(index)/rate-(static_cast<DWORD>(pulse)-GetMC(index))/rate;
}

//08136790: use the actual quest flag lookup; do not assume a default configuration.
void CHARACTER::ClearKillCount() {
    m_iKillCount=quest::CQuestManager::instance().GetEventFlag("threeway_war_dead_count");
}
//08138640: evidenced empty virtual end-state handler.
void CHARACTER::EndStateEmpty() {}

//0812fd10. The old prefix/formatter limits and unchecked lengths are preserved.
void CHARACTER::MonsterLog(const char* format,...) {
    if(!test_server || GetDesc()) return;
    char text[86];
    int prefix=std::sprintf(text,"%lu)",static_cast<DWORD>(GetVID()));
    va_list args; va_start(args,format);
    int formatted=std::vsnprintf(text+prefix,85,format,args);
    va_end(args);
    unsigned int length=static_cast<unsigned int>(prefix)+static_cast<unsigned int>(formatted);
    TPacketGCChat packet;
    packet.header=4; packet.type=0; packet.size=static_cast<WORD>(length+10U);
    packet.id=static_cast<DWORD>(GetVID()); packet.bEmpire=0;
    TEMP_BUFFER buffer;
    buffer.write(&packet,sizeof packet); buffer.write(text,static_cast<int>(length+1U));
    CHARACTER_MANAGER::instance().PacketMonsterLog(this,buffer.read_peek(),buffer.size());
}
//08134be0.
void CHARACTER::BeginStateEmpty() { MonsterLog("!"); }
//08128a60. Position7 clears dead/stun state and cancels events before state selection.
void CHARACTER::SetPosition(int position) {
    if(position==7) {
        m_bAddChrState=static_cast<BYTE>(m_bAddChrState & 0xfe);
        m_pointsInstant.instant_flag=static_cast<long>(static_cast<unsigned long>(m_pointsInstant.instant_flag)&0xfffffff7UL);
        event_cancel(&m_pkDeadEvent); event_cancel(&m_pkStunEvent);
    } else if(position==0) m_bAddChrState=static_cast<BYTE>(m_bAddChrState | 1);
    if(m_bCharType!=2) {
        CState& state=position==5?static_cast<CState&>(m_stateBattle):static_cast<CState&>(m_stateIdle);
        if(!IsState(state)) {
            if(position==5) MonsterLog("[BATTLE] \xbd\xce\xbf\xec\xb4\xc2 \xbb\xf3\xc5\xc2");
            else MonsterLog("[IDLE] \xbd\xac\xb4\xc2 \xbb\xf3\xc5\xc2");
        }
        GotoState(state);
    }
    m_pointsInstant.position=position;
}

//08126dc0: old initialization scope; deliberately does not reset every data member.
void CHARACTER::Initialize() {
    CEntity::Initialize(0);
    m_bNoOpenedShop=true; m_bOpeningSafebox=false;
    m_fSyncTime=get_float_time()-3.0f;
    m_dwPlayerID=0;
    m_posRegen.x=m_posRegen.y=m_posRegen.z=0;
    m_posStart.x=m_posStart.y=0; m_posDest.x=m_posDest.y=0;
    m_fRegenAngle=0;
    m_pkChrShopOwner=0; m_pkMyShop=0; m_pkExchange=0; m_pkParty=0;
    m_pkPartyRequestEvent=0; m_pGuild=0; m_pkStunEvent=0; m_pkSaveEvent=0;
    m_pkRecoveryEvent=0; m_dwKillerPID=0; m_iMoveCount=0; m_pkRegen=0;
    m_pkTimedEvent=0; m_pkMobData=0; m_pkMobInst=0; m_pkShop=0;
    m_pkFishingEvent=0; m_pkChrTarget=0; m_pkWarpEvent=0; m_pkMuyeongEvent=0;
    m_pkWarpNPCEvent=0; m_pkDeadEvent=0; m_pkMiningEvent=0;
    m_afAffectFlag.bits[0]=m_afAffectFlag.bits[1]=0;
    m_pkCheckSpeedHackEvent=0; m_pkPoisonEvent=0; m_speed_hack_count=0;
    m_pkChrSyncOwner=0; m_pkFireEvent=0; m_pkAffectEvent=0;
    std::memset(&m_points,0,sizeof m_points);
    std::memset(&m_pointsInstant,0,sizeof m_pointsInstant);
    std::memset(m_quickslot,0,sizeof m_quickslot);
    m_bCharType=0; SetPosition(7);
    DWORD now=get_dword_time(); m_dwLastMoveTime=now; m_dwPlayStartTime=now;
    m_iAvgDelta=0; GotoState(m_stateIdle); m_dwStateDuration=1;
    m_dwLastAttackTime=get_dword_time()-20000UL;
    m_pkSafebox=0; m_iSafeboxSize=-1; m_pkChrStone=0; m_pkMall=0; m_iMallLoadTime=0;
    m_posWarp.x=m_posWarp.y=m_posWarp.z=0;
    m_lWarpMapIndex=0; m_lExitMapIndex=0; m_pSkillLevels=0;
    m_dwMoveStartTime=0; m_dwMoveDuration=0; m_iSafeboxLoadTime=0; m_dwFlyTargetID=0;
    m_posExit.x=m_posExit.y=m_posExit.z=0;
    m_bAddChrState=0; m_dwNextStatePulse=0; m_pkDungeon=0;
    m_dwLastDeadTime=get_dword_time()-180000UL;
    m_iEventAttr=0; m_kAttackLog.dwVID=0; m_kAttackLog.dwTime=0;
    m_bSkipSave=false; m_bItemLoaded=false; m_bHasPoisoned=false;
    m_bWalking=false; m_bNowWalking=false; m_bDetailLog=false;
    m_dwLastChangeAttackPositionTime=get_dword_time()-10000UL;
    m_iAlignment=0; m_iRealAlignment=0; m_iKillerModePulse=0;
    m_dwQuestNPCVID=0; m_pQuestItem=0; m_bMonsterLog=false; m_bDisableCooltime=false;
    m_bComboIndex=0; m_bPKMode=0; m_dwQuestByVnum=0; m_szMobileAuth[0]=0;
    m_bUnderRefine=false; m_dwUnderGuildWarInfoMessageTime=get_dword_time()-60000UL;
    m_iChainLightingIndex=0; m_dwRefineNPCVID=0; m_dwPolymorphRace=0;
    m_bStaminaConsume=false; m_setExceptChainLighting.clear();
    m_bChatCounter=0; m_dwMountVnum=0; m_chHorse=0; m_chRider=0;
    m_pWarMap=0; m_pWeddingMap=0; ResetStopTime();
    m_bSendHorseLevel=0; m_dwLastVictimSetTime=get_dword_time()-3000UL;
    m_fSpeedHackMultipler=1.0f; m_pkChrMarried=0;
    m_posSafeboxOpen.x=-1000; m_posSafeboxOpen.y=-1000;
    m_iMaxAggro=-100; m_dwLoginPlayTime=0;
    m_bSendHorseHealthGrade=0; m_bSendHorseStaminaGrade=0;
    m_dwLastSkillTime=get_dword_time();
    for(unsigned i=0;i<5;++i) m_adwMobSkillCooltime[i]=0;
    m_isinPCBang=false; m_pArena=0;
    m_nPotionLimit=quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
    m_isOpenSafebox=false; m_iSeedTime=0; m_iRefineTime=0; m_iExchangeTime=0;
    m_iSafeboxLoadTime=0; ClearKillCount(); m_iKillingCount=0; m_iMyShopTime=0;
    InitMC(); m_eOverTime=OT_NONE; m_deposit_pulse=0; m_strNewName="";
}

//08124f20/08125770: ordinary bases/containers/VIDs plus the old explicit bindings.
CHARACTER::CHARACTER() {
    m_afAffectFlag.bits[0]=m_afAffectFlag.bits[1]=0;
    m_triggerOnClick.bType=0; m_triggerOnClick.pFunc=0;
    m_stateIdle.Set(this,&CHARACTER::BeginStateEmpty,&CHARACTER::StateIdle,&CHARACTER::EndStateEmpty);
    m_stateMove.Set(this,&CHARACTER::BeginStateEmpty,&CHARACTER::StateMove,&CHARACTER::EndStateEmpty);
    m_stateBattle.Set(this,&CHARACTER::BeginStateEmpty,&CHARACTER::StateBattle,&CHARACTER::EndStateEmpty);
    Initialize();
}

// 0812dec0/0812dee0: raw original-point access, without added bounds checks.
long CHARACTER::GetRealPoint(BYTE type) { return m_points.points[type]; }
void CHARACTER::SetRealPoint(BYTE type,long value) { m_points.points[type]=value; }
long CHARACTER::GetPoint(BYTE type) {
    int maximum=(type==63 || type==64) ? 50 : 0x7fffffff;
    int value=m_pointsInstant.points[type];
    if(value>maximum)
        // Original call omits the type argument despite the format; preserve its three supplied values.
        _sys_err("GetPoint",2738,"POINT_ERROR: %s type %d val %d (max: %d)",GetName(),value,maximum);
    return value;
}
long CHARACTER::GetLimitPoint(BYTE type) {
    int value=m_pointsInstant.points[type];
    int limit=0x7fffffff,diagnosticMaximum=0x7fffffff,minimum=-0x7fffffff;
    if(type==17) { minimum=0; limit=GetDesc() ? 170 : 250; }
    else if(type==19) { minimum=0; limit=GetDesc() ? 200 : 250; }
    else if(type==63 || type==64) { limit=50; diagnosticMaximum=50; }
    else if(type==114 || type==115) { limit=13; diagnosticMaximum=50; }
    if(value>diagnosticMaximum)
        _sys_err("GetLimitPoint",2784,"POINT_ERROR: %s type %d val %d (max: %d)",GetName(),value,diagnosticMaximum);
    if(value>limit) value=limit;
    if(value<minimum) value=minimum;
    return value;
}
void CHARACTER::SetPart(BYTE part,WORD value) { assert(part<4); m_pointsInstant.parts[part]=value; }
WORD CHARACTER::GetPart(BYTE part) { assert(part<4); return m_pointsInstant.parts[part]; }
WORD CHARACTER::GetOriginalPart(BYTE part) {
    if(part==0) return GetDesc() ? static_cast<WORD>(m_pointsInstant.bBasePart) : GetPart(0);
    if(part==3) return GetPart(3);
    return 0;
}

DWORD CHARACTER::GetMotionMode() {
    if(m_dwPolymorphRace) return 0;
    LPITEM weapon=GetWear(4);
    if(!weapon) return 0;
    switch(weapon->GetProto()->bSubType) {
        case 0:return 1; case 1:return 3; case 2:return 4;
        case 3:return 2; case 4:return 5; case 5:return 6;
    }
    return 0;
}
float CHARACTER::GetMoveMotionSpeed() {
    DWORD mode=GetMotionMode();
    bool run=(!m_bNowWalking && m_points.stamina>0) || !GetDesc();
    DWORD index=run ? 0x300 : 0x200;
    const CMotion* motion=0;
    if(m_dwMountVnum) motion=CMotionManager::instance().GetMotion(m_dwMountVnum,index);
    if(!motion) {
        DWORD key=(m_dwMountVnum ? 7 : mode)<<24 | index;
        motion=CMotionManager::instance().GetMotion(GetRaceNum(),key);
    }
    if(!motion) {
        _sys_err("GetMoveMotionSpeed",2601,"cannot find motion (name %s race %d mode %d)",GetName(),static_cast<unsigned>(GetRaceNum()),mode);
        return 300.0f;
    }
    return -motion->GetAccumVector().y/motion->GetDuration();
}
static int MotionTruncate32(long double value) {
    // x87 FISTP with truncation control yields integer-indefinite for NaN/overflow.
    if(!(value>=-2147483648.0L && value<2147483648.0L)) return (-2147483647-1);
    return static_cast<int>(value);
}
void CHARACTER::CalculateMoveDuration() {
    m_posStart.x=GetX(); m_posStart.y=GetY();
    unsigned int dx=static_cast<unsigned int>(GetX())-static_cast<unsigned int>(m_posDest.x);
    unsigned int dy=static_cast<unsigned int>(GetY())-static_cast<unsigned int>(m_posDest.y);
    int square=static_cast<int>(dx*dx+dy*dy);
    float distance=static_cast<float>(std::sqrt(static_cast<double>(square)));
    float motionSpeed=GetMoveMotionSpeed();
    int baseDuration=MotionTruncate32(static_cast<long double>(distance)/motionSpeed*1000.0L);
    m_dwMoveDuration=CalculateDuration(GetLimitPoint(19),baseDuration);
    if(m_bCharType!=6)
        sys_log(1,"%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",GetName(),static_cast<double>(distance),GetLimitPoint(19),m_dwMoveDuration,static_cast<double>(motionSpeed),m_posStart.x,m_posStart.y,m_posDest.x,m_posDest.y);
    m_dwMoveStartTime=get_dword_time();
}
void CHARACTER::SetPoint(BYTE type,long value) {
    m_pointsInstant.points[type]=value;
    if(type==19 && get_dword_time()<m_dwMoveStartTime+m_dwMoveDuration) CalculateMoveDuration();
}

// 081288a0: the old character update is a packed 35-byte view packet.
void CHARACTER::UpdatePacket() {
    TPacketGCCharacterUpdate packet;
    packet.header=19; packet.dwVID=m_vid;
    for(BYTE i=0;i<4;++i) packet.awPart[i]=GetPart(i);
    packet.bMovingSpeed=static_cast<BYTE>(GetLimitPoint(19));
    packet.bAttackSpeed=static_cast<BYTE>(GetLimitPoint(17));
    packet.bStateFlag=m_bAddChrState;
    packet.dwAffectFlag[0]=m_afAffectFlag.bits[0];
    packet.dwAffectFlag[1]=m_afAffectFlag.bits[1];
    packet.dwGuildID=m_pGuild ? m_pGuild->GetID() : 0;
    packet.sAlignment=static_cast<short>(m_iAlignment/10);
    packet.bPKMode=m_bPKMode; packet.dwMountVnum=m_dwMountVnum;
    PacketAround(&packet,sizeof(packet));
}
// 08153fd0: already-visible/hidden alignment does not trigger a packet.
void CHARACTER::ShowAlignment(bool show) {
    if(show) {
        if(m_iAlignment==m_iRealAlignment) return;
        m_iAlignment=m_iRealAlignment;
    } else {
        if(!m_iAlignment) return;
        m_iAlignment=0;
    }
    UpdatePacket();
}

// Staged char.cpp addition; packet type forward declaration required in char.h.
// Required includes: party.h, packet.h, locale.hpp, utils.h,
// ../../libthecore/include/utils.h, ../../libthecore/include/log.h, <cstring>.
// Exact public signature: bool BuildUpdatePartyPacket(TPacketGCPartyUpdate& out).
// 08133e50. Caller preconditions retain a nonzero maxHP and valid party state.
bool CHARACTER::BuildUpdatePartyPacket(TPacketGCPartyUpdate& out) {
    if(!m_pkParty) return false; // Output is untouched on this path.
    std::memset(&out,0,sizeof(out));
    out.header=79;out.pid=m_dwPlayerID;
    int hp100=static_cast<int>(static_cast<unsigned int>(m_points.hp)*100U);
    out.percent_hp=static_cast<BYTE>(MINMAX(0,hp100/m_pointsInstant.iMaxHP,100));
    out.role=m_pkParty->GetRole(m_dwPlayerID);
    sys_log(1,"PARTY %s role is %d",GetName(),static_cast<unsigned int>(out.role));
    CHARACTER* leader=m_pkParty->GetLeaderCharacter();
    if(leader) {
        int dx=static_cast<int>(static_cast<unsigned int>(GetX())-static_cast<unsigned int>(leader->GetX()));
        int dy=static_cast<int>(static_cast<unsigned int>(GetY())-static_cast<unsigned int>(leader->GetY()));
        if(DISTANCE_APPROX(dx,dy)<5000) {
            out.affects[0]=static_cast<short>(g_iUseLocale==0?m_pkParty->GetExpBonusPercent():m_pkParty->GetPartyBonusExpPercent());
            out.affects[1]=static_cast<short>(GetPoint(91));
            out.affects[2]=static_cast<short>(GetPoint(92));
            out.affects[3]=static_cast<short>(GetPoint(103));
            out.affects[4]=static_cast<short>(GetPoint(104));
            out.affects[5]=static_cast<short>(GetPoint(110));
            out.affects[6]=static_cast<short>(GetPoint(111));
        }
    }
    return true;
}

// Insert in game/src/char.cpp; 0812a580, old non-const signature.
// Requires libthecore/include/log.h and this original cross-unit declaration.
extern bool RaceToJob(unsigned race, unsigned* ret_job);
BYTE CHARACTER::GetJob() {
    const unsigned race=static_cast<unsigned>(m_points.job);
    unsigned job;
    if(RaceToJob(race,&job)) return static_cast<BYTE>(job);
    _sys_err("GetJob",1569,"CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE",GetName(),race);
    return 0;
}

// Add to char.cpp with common/length.h, config.h and log.h available.
//0812a5f0: raw byte write precedes descriptor and privilege decisions.
void CHARACTER::SetLevel(BYTE level) {
    m_points.level=level;
    if(!GetDesc()) return;
    if(level<PK_PROTECT_LEVEL || GetGMLevel()!=GM_PLAYER)
        SetPKMode(PK_MODE_PROTECT);
    else if(m_bPKMode==PK_MODE_PROTECT)
        SetPKMode(PK_MODE_PEACE);
}
//08132170: non-const old API, all nonzero test_server values override storedGM.
BYTE CHARACTER::GetGMLevel() {
    return test_server?GM_IMPLEMENTOR:m_pointsInstant.gm_level;
}

// Insert both complete bodies in game/src/char.cpp, original source placement.
// Requires packet.h, common/length.h ECharType, log.h, utils.h, <cstdlib>.
//08132670. Nonempty NPC target sets require valid signed division operands.
void CHARACTER::BroadcastTargetPacket() {
    if(m_set_pkChrTargetedBy.empty()) return;
    TPacketGCTarget packet;
    packet.header=63;
    packet.dwVID=static_cast<DWORD>(GetVID());
    if(GetDesc()) packet.bHPPercent=0;
    else {
        // The original LEA/SHL sequence multiplies in32-bit registers before IDIV.
        int hp100=static_cast<int>(static_cast<unsigned int>(m_points.hp)*100U);
        packet.bHPPercent=static_cast<BYTE>(MINMAX(0,hp100/m_pointsInstant.iMaxHP,100));
    }
    std::set<CHARACTER*>::iterator it=m_set_pkChrTargetedBy.begin();
    while(it!=m_set_pkChrTargetedBy.end()) {
        CHARACTER* recipient=*it++; // Advance BEFORE Packet, preserving current-entry removal.
        DESC* descriptor=recipient->GetDesc();
        if(!descriptor) {
            _sys_err("BroadcastTargetPacket",4801,"%s %p does not have desc",recipient->GetName(),recipient);
            std::abort();
        }
        descriptor->Packet(&packet,sizeof(packet));
    }
}
//081341a0. Repeated unchanged flags still act for raw non-PC char types.
void CHARACTER::SetNowWalking(bool bWalkFlag) {
    if(m_bNowWalking!=bWalkFlag || m_bCharType!=CHAR_TYPE_PC) {
        if(bWalkFlag) {
            m_bNowWalking=true;
            m_dwWalkStartTime=get_dword_time();
        } else m_bNowWalking=false;
        TPacketGCWalkMode packet;
        packet.vid=static_cast<DWORD>(GetVID());
        packet.header=111;
        packet.mode=m_bNowWalking?WALKMODE_WALK:WALKMODE_RUN;
        PacketView(&packet,sizeof(packet));
        if(m_bCharType!=CHAR_TYPE_PC) {
            if(m_bNowWalking) MonsterLog("\xb0\xc8\xb4\xc2\xb4\xd9");
            else MonsterLog("\xb6\xda\xb4\xd9");
        }
    }
}

// Original concrete bodies all in char.cpp; append after required includes.
// Staged real old dependency methods; add existing includes char.h,
// mob_manager.h, questmanager.h, constants.h, config.h and core utils/log.
// Explicit modulo32 arithmetic matches the ELF's add/lea/imul registers and
// avoids introducing signed-overflow UB into reconstructed native code.
namespace battle_points_detail {
inline int add(int a,int b) {return static_cast<int>(static_cast<unsigned int>(a)+static_cast<unsigned int>(b));}
inline int sub(int a,int b) {return static_cast<int>(static_cast<unsigned int>(a)-static_cast<unsigned int>(b));}
inline int mul(int a,int b) {return static_cast<int>(static_cast<unsigned int>(a)*static_cast<unsigned int>(b));}
}
//0812bb80: actual const-reference return, non-const method. No null guard.
const TMobTable& CHARACTER::GetMobTable() {return m_pkMobData->m_table;}
//08135a20: no positivity or range clamp on a nonzero test-server override.
int CHARACTER::GetPolymorphPower() {
    if(test_server) {
        int value=quest::CQuestManager::instance().GetEventFlag("poly");
        if(value) return value;
    }
    int level=GetSkillLevel(129);
    level=MINMAX(0,level,40);
    return aiPolymorphPowerByLevel[level];
}
//0812df00: long return and non-const method are old DWARF, not modern API.
long CHARACTER::GetPolymorphPoint(BYTE type) {
    using namespace battle_points_detail;
    if(!m_dwPolymorphRace || m_bPolyMaintainStat) return GetPoint(type);
    const CMob* mob=CMobManager::instance().Get(m_dwPolymorphRace);
    int power=GetPolymorphPower(); // Must happen even when mob lookup failed.
    if(!mob) return GetPoint(type);
    BYTE base=type;int stat;
    switch(type) {
    case 12:
        if(GetJob()==3 || (GetJob()==2 && m_points.skill_group==2))base=15;
        stat=mob->m_table.bStr;break;
    case 13:stat=mob->m_table.bCon;break;
    case 14:stat=mob->m_table.bDex;break;
    case 15:stat=mob->m_table.bInt;break;
    default:return GetPoint(type);
    }
    int bonus=mul(stat,power)/100; // Signed truncation after wrapped product.
    return static_cast<long>(add(static_cast<int>(GetPoint(base)),bonus));
}

//08128b50: register a deferred save; skip guard precedes singleton access.
void CHARACTER::Save() {
    if(!m_bSkipSave) CHARACTER_MANAGER::instance().DelayedSave(this);
}
