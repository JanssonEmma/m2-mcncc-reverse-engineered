#include "war_map.h"
#include <cctype>
#include <strings.h>
#include "wedding.h"
#include "dungeon.h"
#include "../../libthecore/include/memcpy.h"
#include "arena.h"
#include "fishing.h"
#include "marriage.h"
#include "messenger_manager.h"
#include "pvp.h"
#include "shop.h"
#include "shop_manager.h"
#include "banword.h"
#include "exchange.h"
#include "target.h"
#include "cassert"
#include "desc_client.h"
#include "desc_manager.h"
#include "db.h"
#include "item_manager.h"
#include "safebox.h"
#include "vector.h"
#include "p2p.h"
#include "map_location.h"
#include "stdio.h"
#include "sectree.h"
#include "regen.h"
#include "sectree_manager.h"
#include "locale_service.h"
#include "log.h"
#include "dev_log.h"
#include "constants.h"
#include <algorithm>
#include <map>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include "mining.h"
#include "locale.hpp"
#include "party.h"
// Partial old CHARACTER recovery; behavior/evidence scope is tracked in reconstruction/.
#include "char.h"
#include "cmd.h"
#include "questmanager.h"
#include "crc32.h"
#include "../../libthecore/include/utils.h"
#include <cstring>
#include <cassert>
#include "desc.h"
#include "gm.h"
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
#include "skill.h"
#include "item.h"
#include "guild.h"
#include "utils.h"
#include <cmath>

namespace {
// Keep regen ownership outside the recovered CHARACTER and regen layouts.
std::multimap<LPREGEN, LPCHARACTER> regenCharacters;

void UntrackRegenCharacter(LPREGEN record, LPCHARACTER character)
{
    const auto range = regenCharacters.equal_range(record);
    for (auto entry = range.first; entry != range.second; ++entry) {
        if (entry->second == character) {
            regenCharacters.erase(entry);
            return;
        }
    }
}
}

// 08125f10/081263f0/081268d0
#ifndef MCNCC_CHARACTER_FIXTURE_DESTRUCTOR
CHARACTER::~CHARACTER()
{
    Destroy();
}
#endif

// 081274f0
void CHARACTER::Destroy()
{
    m_bSkipSave = true;
    CloseMyShop();
    if (m_pkDungeon)
        SetDungeon(nullptr);

    HorseSummon(false, false);
    if (LPCHARACTER rider = GetRider())
        rider->ClearHorseInfo();
    if (LPDESC desc = GetDesc())
        desc->BindCharacter(nullptr);
    if (m_pkExchange)
        m_pkExchange->Cancel();
    SetVictim(nullptr);
    if (m_pkShop) {
        m_pkShop->RemoveGuest(this);
        SetShop(nullptr);
    }

    ClearStone();
    ClearSync();
    ClearTarget();
    ClearItem();

    if (CParty* party = m_pkParty) {
        const DWORD vid = GetVID();
        if (party->GetLeaderPID() == vid && !GetDesc()) {
            delete party;
        } else {
            party->Unlink(this);
            if (!GetDesc())
                party->Quit(vid);
        }
        SetParty(nullptr);
    }

    delete m_pkMobInst;
    m_pkMobInst = nullptr;
    m_pkMobData = nullptr;
    if (m_pkRegen) {
        --m_pkRegen->count;
        UntrackRegenCharacter(m_pkRegen, this);
        m_pkRegen = nullptr;
    }
    delete m_pkSafebox;
    m_pkSafebox = nullptr;
    delete m_pkMall;
    m_pkMall = nullptr;
    m_set_pkChrSpawnedBy.clear();

    StopMuyeongEvent();
    event_cancel(&m_pkWarpNPCEvent);
    event_cancel(&m_pkRecoveryEvent);
    event_cancel(&m_pkDeadEvent);
    event_cancel(&m_pkSaveEvent);
    event_cancel(&m_pkTimedEvent);
    event_cancel(&m_pkStunEvent);
    event_cancel(&m_pkFishingEvent);
    event_cancel(&m_pkPoisonEvent);
    event_cancel(&m_pkFireEvent);
    event_cancel(&m_pkPartyRequestEvent);
    event_cancel(&m_pkWarpEvent);
    event_cancel(&m_pkCheckSpeedHackEvent);
    event_cancel(&m_pkMiningEvent);
    for (const auto& entry : m_mapMobSkillEvent) {
        LPEVENT event = entry.second;
        event_cancel(&event);
    }
    m_mapMobSkillEvent.clear();

    ClearAffect(false);
    delete[] m_pSkillLevels;
    m_pSkillLevels = nullptr;
    CEntity::Destroy();
    if (LPSECTREE tree = GetSectree())
        tree->RemoveEntity(this);
    if (m_bMonsterLog)
        CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
}

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
        m_dwNextStatePulse = static_cast<std::uint32_t>(nextPulse) +
                             static_cast<std::uint32_t>(thecore_heart->pulse);
}
void CHARACTER::StopStateMachine() { CHARACTER_MANAGER::instance().RemoveFromStateList(this); }

//0812fe70. The old formatter return length is retained, including its unchecked truncation behavior.
// 08129ce0
void CHARACTER::Command(const char* argument)
{
    interpret_command(this, argument);
}

void CHARACTER::ChatPacket(BYTE type,const char* format,...) {
    DESC* desc=GetDesc();
    if(!desc) return;
    char chatbuf[CHAT_MAX_LEN+1];
    va_list args; va_start(args,format);
    int length=std::vsnprintf(chatbuf,CHAT_MAX_LEN,format,args);
    va_end(args);
    if (length < 0)
        return;
    length = (std::min)(length, CHAT_MAX_LEN - 1);
    TPacketGCChat packet;
    packet.header=4; packet.size=static_cast<WORD>(sizeof(packet) + length + 1);
    packet.type=type; packet.id=0; packet.bEmpire=desc->GetEmpire();
    if(type==5 && test_server) sys_log(0,"SNED COMMAND %s %s",GetName(),chatbuf);
    TEMP_BUFFER buffer;
    buffer.write(&packet,sizeof packet);
    buffer.write(chatbuf,static_cast<int>(static_cast<unsigned int>(length)+1U));
    desc->Packet(buffer.read_peek(),buffer.size());
}

// 08137010
ESex GET_SEX(CHARACTER* character)
{
    const WORD race = character->GetRaceNum();
    return race == 1 || race == 3 || race == 4 || race == 6 ? SEX_FEMALE : SEX_MALE;
}

bool CHARACTER::OnIdle()
{
    return false;
}

//0812a4e0: polymorph takes precedence, and the public result truncates to WORD.
WORD CHARACTER::GetRaceNum() {
    if(m_dwPolymorphRace) return static_cast<WORD>(m_dwPolymorphRace);
    return m_pkMobData?static_cast<WORD>(m_pkMobData->m_table.dwVnum):static_cast<WORD>(m_points.job);
}

bool CHARACTER::IsSiegeNPC()
{
    if (m_bCharType == CHAR_TYPE_PC)
        return false;
    const WORD race = GetRaceNum();
    return race == 11000 || race == 11002 || race == 11004;
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
    SetKillCount(quest::CQuestManager::instance().GetEventFlag("threeway_war_dead_count"));
}
//08138640: evidenced empty virtual end-state handler.
void CHARACTER::EndStateEmpty() {}

//0812fd10. The old prefix/formatter limits and unchecked lengths are preserved.
void CHARACTER::ToggleMonsterLog()
{
    m_bMonsterLog = !m_bMonsterLog;
    if (m_bMonsterLog)
        CHARACTER_MANAGER::instance().RegisterForMonsterLog(this);
    else
        CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
}

void CHARACTER::MonsterLog(const char* format,...) {
    if(!test_server || GetDesc()) return;
    char text[86];
    int prefix=std::sprintf(text,"%lu)",static_cast<DWORD>(GetVID()));
    va_list args; va_start(args,format);
    const int capacity = sizeof(text) - prefix;
    const int formatted = std::vsnprintf(text + prefix, capacity, format, args);
    va_end(args);
    if (formatted < 0)
        return;
    const int length = prefix + (std::min)(formatted, capacity - 1);
    TPacketGCChat packet;
    packet.header=4; packet.type=0; packet.size=static_cast<WORD>(sizeof(packet) + length + 1);
    packet.id=static_cast<DWORD>(GetVID()); packet.bEmpire=0;
    TEMP_BUFFER buffer;
    buffer.write(&packet,sizeof packet); buffer.write(text,static_cast<int>(length+1U));
    CHARACTER_MANAGER::instance().PacketMonsterLog(this,buffer.read_peek(),buffer.size());
}
//08134be0.
void CHARACTER::BeginStateEmpty() { MonsterLog("!"); }
//08128a60. Position7 clears dead/stun state and cancels events before state selection.
void CHARACTER::Sitdown(int)
{
    if (GetPosition() == 3)
        return;
    SetPosition(3);
    sys_log(1, "SITDOWN: %s", GetName());
    BYTE packet[6] = {0x2b, 0, 0, 0, 0, 4};
    const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(GetVID()));
    std::memcpy(packet + 1, &vid, sizeof(vid));
    PacketAround(packet, sizeof(packet));
}

void CHARACTER::Standup()
{
    if (GetPosition() != 3)
        return;
    SetPosition(7);
    sys_log(1, "STANDUP: %s", GetName());
    BYTE packet[6] = {0x2b};
    const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(GetVID()));
    std::memcpy(packet + 1, &vid, sizeof(vid));
    PacketAround(packet, sizeof(packet));
}

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
    m_fSyncTime=static_cast<float>(get_float_time_extended()-3.0L);
    m_dwPlayerID=0;
    m_posRegen.x=m_posRegen.y=m_posRegen.z=0;
    m_posStart.x=m_posStart.y=0; m_posDest.x=m_posDest.y=0;
    m_fRegenAngle=0;
    m_pkChrShopOwner=0; m_pkMyShop=0; m_pkExchange=0; m_pkParty=0;
    m_bNoOpenedShop = true;
    m_pkPartyRequestEvent=0; m_pGuild=0; m_pkStunEvent=0; m_pkSaveEvent=0;
    m_pkRecoveryEvent=0; m_dwKillerPID=0; m_iMoveCount=0; m_pkRegen=0;
    m_pkTimedEvent=0; m_pkMobData=0; m_pkMobInst=0; m_pkShop=0;
    m_pkFishingEvent=0; m_pkChrTarget=0; m_pkWarpEvent=0; m_pkMuyeongEvent=0;
    m_pkWarpNPCEvent=0; m_pkDeadEvent=0; m_pkMiningEvent=0;
    m_afAffectFlag = TAffectFlag(0, 0);
    m_pkCheckSpeedHackEvent=0; m_pkPoisonEvent=0; m_speed_hack_count=0;
    m_pkChrSyncOwner=0; m_pkFireEvent=0; m_pkAffectEvent=0;
    std::memset(&m_points,0,sizeof m_points);
    std::memset(&m_pointsInstant,0,sizeof m_pointsInstant);
    std::memset(m_quickslot,0,sizeof m_quickslot);
    m_bCharType=0; SetPosition(7);
    DWORD now=get_dword_time(); m_dwLastMoveTime=now; m_dwPlayStartTime=now;
    m_iAvgDelta=0; GotoState(m_stateIdle); m_dwStateDuration=1;
    m_dwLastAttackTime=static_cast<std::uint32_t>(get_dword_time()-20000UL);
    m_pkSafebox=0; m_iSafeboxSize=-1; m_pkChrStone=0; m_pkMall=0; m_iMallLoadTime=0;
    m_posWarp.x=m_posWarp.y=m_posWarp.z=0;
    m_lWarpMapIndex=0; m_lExitMapIndex=0; m_pSkillLevels=0;
    m_dwMoveStartTime=0; m_dwMoveDuration=0; m_iSafeboxLoadTime=0; m_dwFlyTargetID=0;
    m_posExit.x=m_posExit.y=m_posExit.z=0;
    m_bAddChrState=0; m_dwNextStatePulse=0; m_pkDungeon=0;
    m_dwLastDeadTime=static_cast<std::uint32_t>(get_dword_time()-180000UL);
    m_iEventAttr=0; m_kAttackLog.dwVID=0; m_kAttackLog.dwTime=0;
    m_bSkipSave=false; m_bItemLoaded=false; m_bHasPoisoned=false;
    m_bWalking=false; m_bNowWalking=false; m_bDetailLog=false;
    ResetChangeAttackPositionTime();
    m_iAlignment=0; m_iRealAlignment=0; m_iKillerModePulse=0;
    m_dwQuestNPCVID=0; m_pQuestItem=0; m_bMonsterLog=false; m_bDisableCooltime=false;
    m_bComboIndex=0; m_bPKMode=0; m_dwQuestByVnum=0; m_szMobileAuth[0]=0;
    m_bUnderRefine=false; m_dwUnderGuildWarInfoMessageTime=static_cast<std::uint32_t>(get_dword_time()-60000UL);
    m_iChainLightingIndex=0; m_dwRefineNPCVID=0; m_dwPolymorphRace=0;
    m_bStaminaConsume=false; m_setExceptChainLighting.clear();
    m_bChatCounter=0; m_dwMountVnum=0; m_chHorse=0; m_chRider=0;
    m_pWarMap=0; m_pWeddingMap=0; ResetStopTime();
    m_bSendHorseLevel=0; m_dwLastVictimSetTime=static_cast<std::uint32_t>(get_dword_time()-3000UL);
    m_fSpeedHackMultipler=1.0f; m_pkChrMarried=0;
    m_posSafeboxOpen.x=-1000; m_posSafeboxOpen.y=-1000;
    m_iMaxAggro=-100; m_dwLoginPlayTime=0;
    m_bSendHorseHealthGrade=0; m_bSendHorseStaminaGrade=0;
    m_dwLastSkillTime=get_dword_time();
    for(unsigned i=0;i<5;++i) m_adwMobSkillCooltime[i]=0;
    m_isinPCBang=false; m_pArena=0;
    SetPotionLimit(quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count"));
    m_isOpenSafebox=false; m_iSeedTime=0; m_iRefineTime=0; m_iExchangeTime=0;
    m_iSafeboxLoadTime=0; ClearKillCount(); ClearKillingCount(); m_iMyShopTime=0;
    InitMC(); m_eOverTime=OT_NONE; m_deposit_pulse=0; m_strNewName="";
}

//08124f20/08125770: ordinary bases/containers/VIDs plus the old explicit bindings.
CHARACTER::CHARACTER() {
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
        _sys_err("GetPoint",2738,"POINT_ERROR: %s type %d val %d (max: %d)",GetName(),static_cast<int>(type),value,maximum);
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
        _sys_err("GetLimitPoint",2784,"POINT_ERROR: %s type %d val %d (max: %d)",GetName(),static_cast<int>(type),value,diagnosticMaximum);
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
float CHARACTER::GetMoveMotionSpeed()
{
    return static_cast<float>(GetMoveMotionSpeedPrecise());
}

long double CHARACTER::GetMoveMotionSpeedPrecise() {
    DWORD mode=GetMotionMode();
    const auto moveIndex = [this]() -> DWORD {
        return !IsWalking() || !GetDesc() ? 0x300 : 0x200;
    };
    const DWORD mountVnum = m_dwMountVnum;
    const CMotion* motion = mountVnum
        ? CMotionManager::instance().GetMotion(mountVnum, moveIndex()) : nullptr;
    if (!motion) {
        const DWORD key = ((mountVnum ? 7 : mode) << 24) | moveIndex();
        motion=CMotionManager::instance().GetMotion(GetRaceNum(),key);
    }
    if(!motion) {
        const auto race = static_cast<std::int32_t>(GetRaceNum());
        const char* name = GetName();
        _sys_err("GetMoveMotionSpeed", 2601, "cannot find motion (name %s race %d mode %d)",
                 name, race, static_cast<std::int32_t>(mode));
        return 300.0f;
    }
    const float displacement = -motion->GetAccumVector().y;
    const float duration = motion->GetDuration();
    return static_cast<long double>(displacement) / duration;
}
static int MotionTruncate32(long double value) {
    // x87 FISTP with truncation control yields integer-indefinite for NaN/overflow.
    if(!(value>=-2147483648.0L && value<2147483648.0L)) return (-2147483647-1);
    return static_cast<int>(value);
}
void CHARACTER::CalculateMoveDuration() {
    const long destinationY = m_posDest.y;
    const long currentX = GetX();
    const long currentY = GetY();
    const long destinationX = m_posDest.x;
    m_posStart.x = currentX;
    m_posStart.y = currentY;
    const auto dx = static_cast<std::uint32_t>(currentX) - static_cast<std::uint32_t>(destinationX);
    const auto dy = static_cast<std::uint32_t>(currentY) - static_cast<std::uint32_t>(destinationY);
    int square=static_cast<int>(dx*dx+dy*dy);
    float distance=static_cast<float>(std::sqrt(static_cast<double>(square)));
    float motionSpeed=GetMoveMotionSpeed();
    const int moveSpeed=GetLimitPoint(19);
    int baseDuration=MotionTruncate32(static_cast<long double>(distance)/motionSpeed*1000.0L);
    m_dwMoveDuration=static_cast<std::uint32_t>(CalculateDuration(moveSpeed,baseDuration));
    if (m_bCharType != 6) {
        const auto loggedSpeed = static_cast<std::uint32_t>(GetLimitPoint(19));
        const char* name = GetName();
        const auto destY = static_cast<std::int32_t>(m_posDest.y);
        const auto destX = static_cast<std::int32_t>(m_posDest.x);
        const auto startY = static_cast<std::int32_t>(m_posStart.y);
        const auto startX = static_cast<std::int32_t>(m_posStart.x);
        const auto duration = static_cast<std::uint32_t>(m_dwMoveDuration);
        sys_log(1, "%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",
                name, static_cast<double>(distance), loggedSpeed, duration,
                static_cast<double>(motionSpeed), startX, startY, destX, destY);
    }
    m_dwMoveStartTime=get_dword_time();
}
void CHARACTER::SetPoint(BYTE type,long value) {
    m_pointsInstant.points[type]=value;
    if(type==19 && get_dword_time()<static_cast<std::uint32_t>(m_dwMoveStartTime+m_dwMoveDuration)) CalculateMoveDuration();
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
BYTE CHARACTER::GetMobBattleType()
{
    return m_pkMobData ? GetMobTable().bBattleType : 0;
}

WORD CHARACTER::GetMobAttackRange()
{
    if (!m_pkMobData)
        return 0;
    const BYTE type = GetMobBattleType();
    const TMobTable& table = GetMobTable();
    const int distance = type == 1 || type == 2 ? static_cast<short>(GetPoint(POINT_BOW_DISTANCE)) : 0;
    return static_cast<WORD>(table.wAttackRange + distance);
}

DWORD CHARACTER::GetMobDamageMin() { return GetMobTable().dwDamageRange[0]; }
DWORD CHARACTER::GetMobDamageMax() { return GetMobTable().dwDamageRange[1]; }
BYTE CHARACTER::GetMobSize() { return m_pkMobData ? GetMobTable().bSize : 2; }
int CHARACTER::GetLeadershipSkillLevel() { return GetSkillLevel(121); }
bool CHARACTER::IsRefineThroughGuild() { return GetRefineGuild() != nullptr; }

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

#include "char.h"
#include "packet.h"
#include "../../libthecore/include/utils.h"
#include "horse-summon-detail.h"
void CHARACTER::SetRotation(float rotation) { m_pointsInstant.fRot=rotation; }

void CHARACTER::SetEmpire(BYTE empire) { m_bEmpire=empire; }

void EncodeMovePacket(TPacketGCMove& packet,DWORD vid,BYTE function,BYTE argument,
                      DWORD x,DWORD y,DWORD duration,DWORD time,BYTE rotation) {
    packet.bHeader=3;packet.bFunc=function;packet.bArg=argument;packet.dwVID=vid;
    if(!time) time=get_dword_time();
    packet.dwTime=time;packet.bRot=rotation;
    packet.lX=horse_summon_detail::Signed(static_cast<unsigned int>(x));
    packet.lY=horse_summon_detail::Signed(static_cast<unsigned int>(y));
    packet.dwDuration=duration;
}

void CHARACTER::EncodeInsertPacket(CEntity* entity)
{
    DESC* desc = entity->GetDesc();
    if (!desc)
        return;

    TPacketGCCharacterAdd packet{};
    packet.header = 1;
    packet.dwVID = GetVID();
    packet.angle = m_pointsInstant.fRot;
    packet.x = GetX();
    packet.y = GetY();
    packet.z = GetZ();
    packet.bType = GetCharType();
    packet.wRaceNum = GetRaceNum();
    packet.bMovingSpeed = static_cast<BYTE>(GetLimitPoint(19));
    packet.bAttackSpeed = static_cast<BYTE>(GetLimitPoint(17));
    packet.bStateFlag = m_bAddChrState;
    packet.dwAffectFlag[0] = m_afAffectFlag.bits[0];
    packet.dwAffectFlag[1] = m_afAffectFlag.bits[1];

    std::uint32_t remaining = 0;
    if (m_posDest.x != packet.x || m_posDest.y != packet.y) {
        const auto duration = static_cast<std::uint32_t>(m_dwMoveDuration);
        const auto start = static_cast<std::uint32_t>(m_dwMoveStartTime);
        const std::uint32_t deadline = duration + start;
        remaining = deadline - static_cast<std::uint32_t>(get_dword_time());
        if (static_cast<std::int32_t>(remaining) <= 0) {
            packet.x = m_posDest.x;
            packet.y = m_posDest.y;
        }
    }
    desc->Packet(&packet, sizeof(packet));

    if (GetDesc() || m_bCharType == CHAR_TYPE_NPC) {
        TPacketGCCharacterAdditionalInfo info{};
        info.header = 0x88;
        info.dwVID = GetVID();
        std::strncpy(info.name, GetName(), sizeof(info.name) - 1);
        for (BYTE i = 0; i < 4; ++i)
            info.awPart[i] = GetPart(i);
        info.bEmpire = m_bEmpire;
        info.dwGuildID = m_pGuild ? m_pGuild->GetID() : 0;
        info.sAlignment = static_cast<std::int16_t>(m_iAlignment / 10);
        info.bPKMode = m_bPKMode;
        info.dwMountVnum = m_dwMountVnum;
        desc->Packet(&info, sizeof(info));
    }

    if (remaining) {
        TPacketGCMove move;
        const long double encoded = static_cast<long double>(m_pointsInstant.fRot) / 5.0L;
        // An invalid x87 signed-short conversion emits a zero low byte.
        const BYTE rotation = encoded > -32769.0L && encoded < 32768.0L
            ? static_cast<BYTE>(static_cast<std::int16_t>(encoded)) : 0;
        EncodeMovePacket(move, GetVID(), 1, 0, m_posDest.x, m_posDest.y, remaining, 0, rotation);
        desc->Packet(&move, sizeof(move));
        const TPacketGCWalkMode walk{0x6f, packet.dwVID, static_cast<BYTE>(m_bNowWalking)};
        desc->Packet(&walk, sizeof(walk));
    }

    CHARACTER* viewer = entity->IsType(0) ? static_cast<CHARACTER*>(entity) : nullptr;
    if (viewer && GetDesc() && viewer->IsWalking()) {
        const TPacketGCWalkMode walk{0x6f, static_cast<std::uint32_t>(viewer->GetVID()),
                                    static_cast<BYTE>(viewer->m_bNowWalking)};
        GetDesc()->Packet(&walk, sizeof(walk));
    }
    if (m_pkMyShop) {
        TPacketGCShopSign sign{};
        sign.bHeader = 0x27;
        sign.dwVID = GetVID();
        std::strncpy(sign.szSign, m_stShopSign.c_str(), sizeof(sign.szSign) - 1);
        desc->Packet(&sign, sizeof(sign));
    }
    if (viewer) {
        sys_log(3, "EntityInsert %s (RaceNum %d) (%ld %ld) TO %s", GetName(), GetRaceNum(),
                GetX() / 6400, GetY() / 6400, viewer->GetName());
    }
}

void CHARACTER::EncodeRemovePacket(CEntity* entity)
{
    if (!entity->IsType(0))
        return;
    DESC* desc = entity->GetDesc();
    if (!desc)
        return;
    const TPacketGCCharacterDelete packet{2, static_cast<std::uint32_t>(GetVID())};
    desc->Packet(&packet, sizeof(packet));
    sys_log(3, "EntityRemove %s(%d) FROM %s", GetName(), static_cast<int>(packet.id),
            static_cast<CHARACTER*>(entity)->GetName());
}

// 0812e290
long CHARACTER::GetAllowedGold()
{
    const int level = GetLevel();
    if (level <= 10)
        return 100000;
    if (level <= 20)
        return 500000;
    return 50000000;
}

// 0812fbb0
void CHARACTER::MotionPacketEncode(BYTE motion, LPCHARACTER victim, packet_motion* packet)
{
    packet->header = 0x24;
    packet->vid = static_cast<DWORD>(GetVID());
    packet->victim_vid = victim ? static_cast<DWORD>(victim->GetVID()) : 0;
    packet->motion = motion;
}

// 0812fbf0
void CHARACTER::Motion(BYTE motion, LPCHARACTER victim)
{
    packet_motion packet;
    MotionPacketEncode(motion, victim, &packet);
    PacketAround(&packet, sizeof(packet));
}

void CHARACTER::SendMovePacket(BYTE function,BYTE argument,DWORD x,DWORD y,
                                DWORD duration,DWORD time,int rotation) {
    if(!function) { x=m_posDest.x;y=m_posDest.y;duration=m_dwMoveDuration; }
    if(rotation==-1) rotation=horse_summon_detail::Trunc32(m_pointsInstant.fRot)/5;
    TPacketGCMove packet;
    EncodeMovePacket(packet,m_vid,function,argument,x,y,duration,time,static_cast<BYTE>(rotation));
    PacketView(&packet,sizeof(packet),this);
}


// Full nonconst original char.cpp pointer lookups,08133660/08133b70.
CSafebox* CHARACTER::GetSafebox(){return m_pkSafebox;}
CSafebox* CHARACTER::GetMall(){return m_pkMall;}

// Whole originalCHAR leaf08136310: noDESC skip, fullPODpacket, actualfreshDESC send.
void CHARACTER::ConfirmWithMsg(const char* szMsg,int iTimeout,DWORD dwRequestPID){
    if(GetDesc()){
        TPacketGCQuestConfirm p;
        p.header=46;
        p.requestPID=dwRequestPID;
        p.timeout=iTimeout;
        std::strncpy(p.msg,szMsg,64);
        p.msg[64]='\0';
        GetDesc()->Packet(&p,sizeof(p));
    }
}

// Whole original methods 08134f30 / 08134f50. Existing char.cpp includes char_manager.h.
CItem* CHARACTER::GetQuestItemPtr() { return m_pQuestItem; }
void CHARACTER::SetQuestItemPtr(CItem* item) { m_pQuestItem = item; }
void CHARACTER::ClearQuestItemPtr() { m_pQuestItem = nullptr; }

void CHARACTER::SetQuestNPCID(DWORD vid) { m_dwQuestNPCVID=static_cast<std::uint32_t>(vid); }
LPCHARACTER CHARACTER::FindCharacterInView(const char* name, bool pcOnly)
{
    for (const auto& entry : m_map_view) {
        if (!entry.first->IsType(0))
            continue;
        CHARACTER* target = static_cast<CHARACTER*>(entry.first);
        if ((!pcOnly || target->GetCharType() == CHAR_TYPE_PC) && !strcasecmp(target->GetName(), name))
            return target;
    }
    return nullptr;
}

LPCHARACTER CHARACTER::GetQuestNPC() {
    return CHARACTER_MANAGER::instance().Find(m_dwQuestNPCVID);
}

void CHARACTER::DetermineDropMetinStone()
{
    static DWORD c_adwMetin[] = {
        28030, 28031, 28032, 28033, 28034, 28035, 28036,
        28037, 28038, 28039, 28040, 28041, 28042, 28043
    };
    const DWORD race = GetRaceNum();
    const SStoneDropInfo* end = aStoneDrop + STONE_INFO_MAX_NUM;
    const SStoneDropInfo* info = std::lower_bound(aStoneDrop, end, race);
    if (info == end || info->dwMobVnum != race)
    {
        m_dwDropMetinStone = 0;
        return;
    }

    m_bDropMetinStonePct = static_cast<BYTE>(info->iDropPct);
    m_dwDropMetinStone = c_adwMetin[number_ex(0, 13, const_cast<char*>("char.cpp"), 0x174a)];
    int gradeRoll = number_ex(1, 100, const_cast<char*>("char.cpp"), 0x174b);
    for (int grade = 0; grade < STONE_LEVEL_MAX_NUM; ++grade)
    {
        const int portion = info->iLevelPct[grade];
        if (gradeRoll <= portion)
            return;

        m_dwDropMetinStone += 100;
        gradeRoll -= portion;
    }
}

// Original 081304c0 and 08130500.
void CHARACTER::SetShop(LPSHOP shop) {
    m_pkShop = shop;
    if (shop) {
        m_pointsInstant.instant_flag |= 2L;
    } else {
        m_pointsInstant.instant_flag &= ~2L;
        m_pkChrShopOwner = 0;
    }
}

void CHARACTER::SetExchange(CExchange* exchange) {
    m_pkExchange = exchange;
}

void CHARACTER::SetUseSeedOrMoonBottleTime() { m_iSeedTime = thecore_pulse(); }

void CHARACTER::SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }

void CHARACTER::SetRefineTime() { m_iRefineTime = thecore_pulse(); }
void CHARACTER::SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }

void CHARACTER::SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }

namespace point_change_detail {
inline int add(int a,int b){return static_cast<int>(static_cast<unsigned int>(a)+static_cast<unsigned int>(b));}
inline int sub(int a,int b){return static_cast<int>(static_cast<unsigned int>(a)-static_cast<unsigned int>(b));}
inline int mul(int a,int b){return static_cast<int>(static_cast<unsigned int>(a)*static_cast<unsigned int>(b));}
inline int neg(int a){return static_cast<int>(0U-static_cast<unsigned int>(a));}
}
void CHARACTER::PointChange(BYTE type,int amount,bool bAmount,bool bBroadcast) {
    using namespace point_change_detail;
    long val=0;
    switch(type) {
    case 0:return;
    case 1: {
        if(add(amount,m_points.level)>99)return;
        SetLevel(static_cast<BYTE>(add(amount,m_points.level)));
        BYTE level=m_points.level;val=level;
        int next=level<99?static_cast<int>(exp_table[level]):2000000000;
        sys_log(0,"LEVELUP: %s %d NEXT EXP %d",GetName(),static_cast<unsigned int>(level),next);
        unsigned int currentLevel=m_points.level;
        next=currentLevel<99?static_cast<int>(exp_table[currentLevel]):2000000000;
        PointChange(4,next,false,false);
        if(amount) {
            unsigned int pid=static_cast<unsigned int>(m_dwPlayerID);
            quest::CQuestManager::instance().LevelUp(pid);
            long realPlayTime=GetRealPoint(31);
            DWORD ticks=get_dword_time();DWORD start=m_dwPlayStartTime;
            const char* name=GetName();const char* postfix=get_table_postfix();
            int playTime=add(static_cast<int>(realPlayTime),static_cast<int>(static_cast<unsigned int>(ticks-start)/60000U));
            LogManager::instance().Query("REPLACE INTO levellog%s (name, level, time, playtime) VALUES('%s', %lu, NOW(), %d)",postfix,name,static_cast<DWORD>(val),playTime);
            if(m_pGuild)m_pGuild->LevelChange(m_dwPlayerID,m_points.level);
            if(m_pkParty)m_pkParty->RequestSetMemberLevel(m_dwPlayerID,m_points.level);
        }
        break;
    }
    case 2:case 24:val=GetRealPoint(type);break;
    case 3: {
        // Both old locals are signedint32, although the stored exp field isDWORD.
        int previous=static_cast<int>(m_points.exp);
        unsigned int level=m_points.level;
        int next=level<99?static_cast<int>(exp_table[level]):2000000000;
        if(LC_IsNewCIBN()) {
            switch(m_eOverTime) {
            case OT_NONE:dev_log("char.cpp",2920,"PointChange",0x800,const_cast<char*>("<EXP_LOG> %s = NONE"),GetName());break;
            case OT_3HOUR:amount/=2;dev_log("char.cpp",2925,"PointChange",0x800,const_cast<char*>("<EXP_LOG> %s = 3HOUR"),GetName());break;
            case OT_5HOUR:amount=0;dev_log("char.cpp",2930,"PointChange",0x800,const_cast<char*>("<EXP_LOG> %s = 5HOUR"),GetName());break;
            default:break;
            }
        }
        if(amount<0 && previous<neg(amount)) {
            sys_log(1,"%s AMOUNT < 0 %ld, CUR EXP: %ld",GetName(),static_cast<long>(neg(amount)),static_cast<long>(previous));
            amount=neg(previous);SetExp(0);val=0;
        } else {
            if(m_points.level>98)return;
            if(test_server)ChatPacket(1,"You have gained %d exp.",amount);
            int balance=0;
            int exp=add(previous,amount);
            int excess=sub(exp,next);
            sys_log(1,"PRE EXP: %d + %d >= %d (%d)",previous,amount,next,excess);
            if(exp<next)SetExp(static_cast<DWORD>(static_cast<unsigned int>(exp)));
            else {
                amount=sub(next,previous);SetExp(0);exp=next;balance=excess;
            }
            // Only three format operands are stored for this call in assembly;
            // Ghidra's extra operand is a stale priorPRE-log stack slot.
            sys_log(1,"EXP: %s %d / %d",GetName(),exp,next);
            int quarter=next/4;
            long step=GetRealPoint(25);
            if(step>3) {
                _sys_err("PointChange",2975,"%s LEVEL_STEP bigger than 4! (%d)",GetName(),static_cast<int>(step));
                step=4;
            }
            if(exp>=next && step<=3) {
                int count=sub(4,static_cast<int>(step));
                while(count>0){PointChange(25,1,false,true);--count;}
            } else if(exp>=mul(quarter,3) && step<=2) {
                int count=sub(3,static_cast<int>(step));
                while(count>0){PointChange(25,1,false,true);--count;}
            } else if(exp>=mul(quarter,2) && step<=1) {
                int count=sub(2,static_cast<int>(step));
                while(count>0){PointChange(25,1,false,true);--count;}
            } else if(exp>=quarter && step<1)PointChange(25,1,false,false);
            if(balance) {
                sys_log(1,"%s iExpBalance %d",GetName(),balance);
                PointChange(3,balance,false,false);
            }
            val=static_cast<int>(m_points.exp);
        }
        break;
    }
    case 4: {
        unsigned int level=m_points.level;
        val=level<99?static_cast<int>(exp_table[level]):2000000000;
        bAmount=false;break;
    }
    case 5: {
        if(IsDead() || IsStun())return;
        int previous=m_points.hp;
        amount=MIN(sub(m_pointsInstant.iMaxHP,previous),amount);
        val=add(amount,m_points.hp);m_points.hp=static_cast<int>(val);
        BroadcastTargetPacket();
        if(m_pkParty && GetDesc() && val!=previous)m_pkParty->SendPartyInfoOneToAll(this);
        break;
    }
    case 6: {
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));
        long base=GetRealPoint(6);
        long percent=GetPoint(119);
        int bonus=MIN(1500,mul(static_cast<int>(percent),static_cast<int>(base))/100);
        long point=GetPoint(6);long party=GetPoint(92);
        val=add(add(add(static_cast<int>(base),bonus),static_cast<int>(point)),static_cast<int>(party));
        m_pointsInstant.iMaxHP=static_cast<int>(val);break;
    }
    case 7: {
        if(IsDead() || IsStun())return;
        amount=MIN(sub(m_pointsInstant.iMaxSP,m_points.sp),amount);
        val=add(amount,m_points.sp);SetSP(static_cast<int>(val));break;
    }
    case 8: {
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));
        long base=GetRealPoint(8);long percent=GetPoint(120);
        int bonus=MIN(800,mul(static_cast<int>(percent),static_cast<int>(base))/100);
        long point=GetPoint(8);long party=GetPoint(104);
        val=add(add(add(static_cast<int>(base),bonus),static_cast<int>(point)),static_cast<int>(party));
        SetMaxSP(static_cast<int>(val));break;
    }
    case 9: {
        if(IsDead() || IsStun())return;
        int previous=GetStamina();
        amount=MIN(sub(GetMaxStamina(),previous),amount);
        val=add(amount,GetStamina());SetStamina(static_cast<int>(val));
        if(!val)SetNowWalking(true);
        else if(!previous)ResetWalking();
        if(amount<0 && val!=0)return;
        break;
    }
    case 10:val=add(amount,GetMaxStamina());SetMaxStamina(static_cast<int>(val));break;
    case 11: {
        if(add(amount,static_cast<int>(m_points.gold))<2000000000) {
            if(LC_IsNewCIBN() && amount>0) {
                switch(m_eOverTime) {
                case OT_NONE:dev_log("char.cpp",3192,"PointChange",0x800,const_cast<char*>("<GOLD_LOG> %s = NONE"),GetName());break;
                case OT_3HOUR:amount/=2;dev_log("char.cpp",3197,"PointChange",0x800,const_cast<char*>("<GOLD_LOG> %s = 3HOUR"),GetName());break;
                case OT_5HOUR:amount=0;dev_log("char.cpp",3202,"PointChange",0x800,const_cast<char*>("<GOLD_LOG> %s = 5HOUR"),GetName());break;
                default:break;
                }
            }
            val=add(amount,static_cast<int>(m_points.gold));SetGold(val);
        } else {
            const char* name=GetName();
            _sys_err("PointChange",3181,"[OVERFLOW_GOLD] OriGold %ld AddedGold %ld id %lu Name %s ",m_points.gold,static_cast<long>(amount),m_dwPlayerID,name);
            int gold=static_cast<int>(m_points.gold);DWORD y=static_cast<DWORD>(GetY());DWORD pid=m_dwPlayerID;DWORD x=static_cast<DWORD>(GetX());
            LogManager::instance().CharLog(pid,x,y,static_cast<DWORD>(static_cast<unsigned int>(add(amount,gold))),"OVERFLOW_GOLD","");
            // Original val remains initialized0; do not report currentgold here.
        }
        break;
    }
    case 16:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));
        val=GetPoint(type);PointChange(20,amount,false,false);break;
    case 25: {
        if(amount<=0){val=GetPoint(25);break;}
        int step=add(amount,static_cast<int>(GetPoint(25)));
        BYTE level;
        if(step>=1 && step<=3) {
            if(m_points.level<91)PointChange(26,1,false,false);
        } else if(step==4) {
            // Preserve actualGetJob call order: upperbound first, lower second.
            BYTE upperJob=GetJob();BYTE lowerJob=GetJob();
            int hp=number_ex(JobInitialPoints[lowerJob].hp_per_lv_begin,JobInitialPoints[upperJob].hp_per_lv_end,"char.cpp",3023);
            upperJob=GetJob();lowerJob=GetJob();
            int sp=number_ex(JobInitialPoints[lowerJob].sp_per_lv_begin,JobInitialPoints[upperJob].sp_per_lv_end,"char.cpp",3024);
            SetRandomSP(add(GetRandomSP(), sp)); SetRandomHP(add(GetRandomHP(), hp));
            if(m_points.skill_group) {
                level=m_points.level;
                if(level>4){PointChange(28,1,false,false);level=m_points.level;}
                if(level>8)PointChange(27,1,false,false);
            }
            PointChange(6,hp,false,false);PointChange(8,sp,false,false);
            step=0;PointChange(1,1,false,true);
        }
        level=m_points.level;
        DWORD item=level<11?27001:27002;
        AutoGiveItem(item,static_cast<BYTE>(2),-1,true);
        PointChange(5,sub(m_pointsInstant.iMaxHP,m_points.hp),false,false);
        PointChange(7,sub(m_pointsInstant.iMaxSP,m_points.sp),false,false);
        PointChange(9,sub(GetMaxStamina(),GetStamina()),false,false);
        SetPoint(25,step);SetRealPoint(25,step);Save();val=step;break;
    }
    case 26:case 27:case 28:case 112:case 113:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);SetRealPoint(type,val);break;
    case 83:case 84:case 85:case 86:
        if(add(static_cast<int>(GetPoint(type)),amount)>100) {
            _sys_err("PointChange",3355,"BONUS exceeded over 100!! point type: %d name: %s amount %d",static_cast<unsigned int>(type),GetName(),amount);
            amount=sub(100,static_cast<int>(GetPoint(type)));
        }
        goto additive;
    case 88:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);
        if(val==0)m_pointsInstant.dwImmuneFlag&=0xfffffffeUL;else m_pointsInstant.dwImmuneFlag|=1;break;
    case 89:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);
        if(val==0)m_pointsInstant.dwImmuneFlag&=0xfffffffdUL;else m_pointsInstant.dwImmuneFlag|=2;break;
    case 90:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);
        if(val==0)m_pointsInstant.dwImmuneFlag&=0xfffffffbUL;else m_pointsInstant.dwImmuneFlag|=4;break;
    case 95:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));PointChange(18,amount,false,false);val=GetPoint(type);break;
    case 96:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));PointChange(16,amount,false,false);val=GetPoint(type);break;
    case 97:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));PointChange(22,amount,false,false);val=GetPoint(type);break;
    case 98:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));PointChange(23,amount,false,false);val=GetPoint(type);break;
    case 108:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);SetPolymorph(static_cast<DWORD>(static_cast<unsigned int>(val)),false);break;
    case 109:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);MountVnum(static_cast<DWORD>(static_cast<unsigned int>(val)));break;
    case 114:case 115:case 116:case 117:case 118:
        if(add(static_cast<int>(GetPoint(type)),amount)>100) {
            _sys_err("PointChange",3333,"MALL_BONUS exceeded over 100!! point type: %d name: %s amount %d",static_cast<unsigned int>(type),GetName(),amount);
            amount=sub(100,static_cast<int>(GetPoint(type)));
        }
        goto additive;
    case 119:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);PointChange(6,0,false,false);break;
    case 120:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);PointChange(8,0,false,false);break;
    case 125:case 126:SetPoint(type,amount);val=GetPoint(type);break;
    // EXACT original simpleadditive dispatch targets from127-entry jump table.
    case 12:case 13:case 14:case 15:case 17:case 18:case 19:case 20:case 21:case 22:case 23:
    case 32:case 33:case 34:case 35:case 36:case 37:case 38:case 39:case 40:case 41:case 42:case 43:case 44:case 45:case 46:case 47:case 48:
    case 53:case 54:case 55:case 56:case 57:case 59:case 60:case 61:case 62:case 63:case 64:case 65:case 66:case 67:case 68:case 69:case 70:case 71:case 72:case 73:case 74:case 75:case 76:case 77:case 78:case 79:case 80:case 81:case 82:
    case 87:case 91:case 92:case 93:case 94:case 99:case 100:case 101:case 102:case 103:case 104:case 110:case 111:case 121:case 122:case 123:case 124:
        goto additive;
    default:
        _sys_err("PointChange",3445,"CHARACTER::PointChange: %s: unknown point change type %d",GetName(),static_cast<unsigned int>(type));return;
    }
    goto epilogue;
additive:
    SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);
epilogue:
    switch(type){case 1:case 12:case 13:case 14:case 15:ComputeBattlePoints();break;default:break;}
    if(type!=5 || amount!=0) {
        DESC* descriptor=GetDesc();
        if(descriptor) {
            TPacketGCPointChange pack;pack.header=17;pack.dwVID=m_vid;pack.type=type;
            pack.amount=bAmount?amount:0;pack.value=val;
            if(bBroadcast)CEntity::PacketAround(&pack,sizeof(pack),0);
            else descriptor->Packet(&pack,sizeof(pack));
        }
    }
}

// C++03, original int/long32 x86 arithmetic. Include <string.h>.
namespace apply_item_detail {
typedef char require_int32[sizeof(int)==4?1:-1];
typedef char require_long32[sizeof(long)==4?1:-1];
inline int bits(unsigned int value) { int result; memcpy(&result,&value,4); return result; }
inline int add(int a,int b) { return bits(static_cast<unsigned int>(a)+static_cast<unsigned int>(b)); }
inline int sub(int a,int b) { return bits(static_cast<unsigned int>(a)-static_cast<unsigned int>(b)); }
inline int mul(int a,int b) { return bits(static_cast<unsigned int>(a)*static_cast<unsigned int>(b)); }
inline int negate(int value) { return bits(0u-static_cast<unsigned int>(value)); }
inline int removal(int value,bool add) { return add?value:negate(value); }
inline int skill(int value,bool add) { return add?value:bits(static_cast<unsigned int>(value)^0x00800000u); }
}

// Original char.cpp, CHARACTER::ApplyPoint 0812f7d0. Full body; NOT independently link closed.
void CHARACTER::ApplyPoint(BYTE bApplyType,long lVal)
{
    switch (bApplyType) {
    case APPLY_NONE: break;
    case APPLY_CON:
        PointChange(POINT_HT,lVal,false,false);
        PointChange(POINT_MAX_HP,apply_item_detail::mul(lVal,JobInitialPoints[GetJob()].hp_per_ht),false,false);
        PointChange(POINT_MAX_STAMINA,apply_item_detail::mul(lVal,JobInitialPoints[GetJob()].stamina_per_con),false,false);
        break;
    case APPLY_INT:
        PointChange(POINT_IQ,lVal,false,false);
        PointChange(POINT_MAX_SP,apply_item_detail::mul(lVal,JobInitialPoints[GetJob()].sp_per_iq),false,false);
        break;
    case APPLY_SKILL:
        if (m_pSkillLevels) {
            BYTE bSkillVnum=static_cast<BYTE>(static_cast<unsigned long>(lVal)>>24);
            int iAdd=(static_cast<unsigned long>(lVal)&0x00800000u)!=0;
            int iChange=static_cast<unsigned long>(lVal)&0x007fffffu;
            sys_log(1,"APPLY_SKILL skill %d add? %d change %d",static_cast<unsigned int>(bSkillVnum),iAdd,iChange);
            int& bonus=m_SkillDamageBonus[bSkillVnum];
            if (iAdd) bonus=apply_item_detail::add(bonus,iChange);
            else bonus=apply_item_detail::sub(bonus,iChange);
            if (m_SkillDamageBonus[bSkillVnum]==0) m_SkillDamageBonus.erase(bSkillVnum);
        }
        break;
    case APPLY_MAX_HP: // 1
    case APPLY_MAX_SP: // 2
    case APPLY_STR: // 5
    case APPLY_DEX: // 6
    case APPLY_ATT_SPEED: // 7
    case APPLY_MOV_SPEED: // 8
    case APPLY_CAST_SPEED: // 9
    case APPLY_HP_REGEN: // 10
    case APPLY_SP_REGEN: // 11
    case APPLY_POISON_PCT: // 12
    case APPLY_STUN_PCT: // 13
    case APPLY_SLOW_PCT: // 14
    case APPLY_CRITICAL_PCT: // 15
    case APPLY_PENETRATE_PCT: // 16
    case APPLY_ATTBONUS_HUMAN: // 17
    case APPLY_ATTBONUS_ANIMAL: // 18
    case APPLY_ATTBONUS_ORC: // 19
    case APPLY_ATTBONUS_MILGYO: // 20
    case APPLY_ATTBONUS_UNDEAD: // 21
    case APPLY_ATTBONUS_DEVIL: // 22
    case APPLY_STEAL_HP: // 23
    case APPLY_STEAL_SP: // 24
    case APPLY_MANA_BURN_PCT: // 25
    case APPLY_DAMAGE_SP_RECOVER: // 26
    case APPLY_BLOCK: // 27
    case APPLY_DODGE: // 28
    case APPLY_RESIST_SWORD: // 29
    case APPLY_RESIST_TWOHAND: // 30
    case APPLY_RESIST_DAGGER: // 31
    case APPLY_RESIST_BELL: // 32
    case APPLY_RESIST_FAN: // 33
    case APPLY_RESIST_BOW: // 34
    case APPLY_RESIST_FIRE: // 35
    case APPLY_RESIST_ELEC: // 36
    case APPLY_RESIST_MAGIC: // 37
    case APPLY_RESIST_WIND: // 38
    case APPLY_REFLECT_MELEE: // 39
    case APPLY_REFLECT_CURSE: // 40
    case APPLY_POISON_REDUCE: // 41
    case APPLY_KILL_SP_RECOVER: // 42
    case APPLY_EXP_DOUBLE_BONUS: // 43
    case APPLY_GOLD_DOUBLE_BONUS: // 44
    case APPLY_ITEM_DROP_BONUS: // 45
    case APPLY_POTION_BONUS: // 46
    case APPLY_KILL_HP_RECOVER: // 47
    case APPLY_IMMUNE_STUN: // 48
    case APPLY_IMMUNE_SLOW: // 49
    case APPLY_IMMUNE_FALL: // 50
    case APPLY_BOW_DISTANCE: // 52
    case APPLY_ATT_GRADE_BONUS: // 53
    case APPLY_DEF_GRADE_BONUS: // 54
    case APPLY_MAGIC_ATT_GRADE: // 55
    case APPLY_MAGIC_DEF_GRADE: // 56
    case APPLY_CURSE_PCT: // 57
    case APPLY_MAX_STAMINA: // 58
    case APPLY_ATTBONUS_WARRIOR: // 59
    case APPLY_ATTBONUS_ASSASSIN: // 60
    case APPLY_ATTBONUS_SURA: // 61
    case APPLY_ATTBONUS_SHAMAN: // 62
    case APPLY_ATTBONUS_MONSTER: // 63
    case APPLY_MALL_ATTBONUS: // 64
    case APPLY_MALL_DEFBONUS: // 65
    case APPLY_MALL_EXPBONUS: // 66
    case APPLY_MALL_ITEMBONUS: // 67
    case APPLY_MALL_GOLDBONUS: // 68
    case APPLY_MAX_HP_PCT: // 69
    case APPLY_MAX_SP_PCT: // 70
    case APPLY_SKILL_DAMAGE_BONUS: // 71
    case APPLY_NORMAL_HIT_DAMAGE_BONUS: // 72
    case APPLY_SKILL_DEFEND_BONUS: // 73
    case APPLY_NORMAL_HIT_DEFEND_BONUS: // 74
    case APPLY_PC_BANG_EXP_BONUS: // 75
    case APPLY_PC_BANG_DROP_BONUS: // 76
    case APPLY_RESIST_WARRIOR: // 78
    case APPLY_RESIST_ASSASSIN: // 79
    case APPLY_RESIST_SURA: // 80
    case APPLY_RESIST_SHAMAN: // 81
        PointChange(aApplyInfo[bApplyType].bPointType,lVal,false,false);
        break;
    default:
        _sys_err("ApplyPoint",0xe2c,"Unknown apply type %d name %s",static_cast<unsigned int>(bApplyType),GetName());
        break;
    }
}

#include <cstring>
namespace mount_polymorph_detail {
// Explicit target32 arithmetic, with C++03 spelling and no signed overflow.
inline long Signed(unsigned int bits) {
    int value; std::memcpy(&value,&bits,sizeof(value)); return value;
}
inline long Add(long a,long b) { return Signed(static_cast<unsigned int>(a)+static_cast<unsigned int>(b)); }
inline long Sub(long a,long b) { return Signed(static_cast<unsigned int>(a)-static_cast<unsigned int>(b)); }
inline long Mul(long a,long b) { return Signed(static_cast<unsigned int>(a)*static_cast<unsigned int>(b)); }
}

// Append to char-helper-methods.cpp.candidate (uses its modulo32 helpers).
// Full0812bda0 body; PointChange is an actual SCC dependency, NEVER stub it.
void CHARACTER::ComputeBattlePoints() {
    using namespace battle_points_detail;
    if(m_dwPolymorphRace) {
        const CMob* mob=CMobManager::instance().Get(m_dwPolymorphRace);
        int attack=0,defense=0;
        if(mob) {
            BYTE level=m_points.level;
            attack=mul(add(level,static_cast<int>(GetPolymorphPoint(12))),2);
            level=m_points.level;
            defense=add(add(level,static_cast<int>(GetPolymorphPoint(13))),mob->m_table.wDef);
        }
        SetPoint(18,attack);SetPoint(16,defense);
        SetPoint(22,GetPoint(18));SetPoint(23,GetPoint(16));return;
    }
    if(!GetDesc()) {
        BYTE level=m_points.level;int st=static_cast<int>(GetPoint(12));
        BYTE defLevel=m_points.level;int attack=mul(add(level,st),2);
        int ht=static_cast<int>(GetPoint(13));
        int defense=add(add(defLevel,ht),GetMobTable().wDef);
        SetPoint(18,attack);SetPoint(16,defense);
        SetPoint(22,GetPoint(18));SetPoint(23,GetPoint(16));return;
    }
    SetPoint(18,0);SetPoint(16,0);SetPoint(20,0);
    SetPoint(22,GetPoint(18));SetPoint(23,GetPoint(16));
    BYTE attackLevel=m_points.level;
    int statAttack;
    switch(GetJob()) {
    case 0:case 2:statAttack=mul(static_cast<int>(GetPoint(12)),2);break;
    case 1:{int st=static_cast<int>(GetPoint(12));int dx=static_cast<int>(GetPoint(14));statAttack=mul(add(dx,mul(st,2)),2)/3;break;}
    case 3:{int st=static_cast<int>(GetPoint(12));int iq=static_cast<int>(GetPoint(15));statAttack=mul(add(iq,mul(st,2)),2)/3;break;}
    default:
        _sys_err("ComputeBattlePoints",1992,"invalid job %d",static_cast<unsigned int>(GetJob()));
        statAttack=mul(static_cast<int>(GetPoint(12)),2);break;
    }
    DWORD mount=0;
    if(m_dwMountVnum) {
        if(statAttack<mul(static_cast<int>(GetPoint(12)),2))statAttack=mul(static_cast<int>(GetPoint(12)),2);
        mount=m_dwMountVnum;
    }
    int attack=add(statAttack,mul(attackLevel,2));
    if(mount) {
        sys_log(0,"HORSE ATK BONUS Before: %d",attack);
        BYTE job=GetJob();
        int divisor=(job==2 && m_points.skill_group==1)?60:30;
        attack=add(attack,mul(GetHorseLevel(),attack)/divisor);
        sys_log(0,"HORSE ATK BONUS After: %d",attack);
    }
    PointChange(18,attack,false,false);
    BYTE showLevel=m_points.level;int showDefense=add(showLevel,static_cast<int>(GetPoint(13)));
    BYTE defenseLevel=m_points.level;int ht=static_cast<int>(GetPoint(13));
    // Original x87 fild/fdiv(1.25f)/fistp with round-toward-zero. No ROUND
    // routine, no nearest rounding, no intermediate float32 store.
    int defense=add(defenseLevel,static_cast<int>(static_cast<long double>(ht)/1.25L));
    int armor=0;
    for(int i=0;i<19;++i) {
        CItem* item=GetWear(static_cast<BYTE>(i));
        if(item && item->GetType()==2) {
            BYTE sub=item->GetSubType();
            if(sub==0 || sub==1 || sub==4 || sub==2) {
                armor=add(armor,static_cast<int>(item->GetValue(1)));
                armor=add(armor,mul(static_cast<int>(item->GetValue(5)),2));
            }
        }
    }
    if (m_dwMountVnum) {
        const int horseArmor = c_aHorseStat[GetHorseLevel()].iArmor;
        if (armor < horseArmor)
            armor = horseArmor;
    }
    if(LC_IsYMIR())PointChange(16,add(showDefense,armor),false,false);
    else PointChange(16,add(defense,armor),false,false);
    PointChange(20,sub(add(showDefense,armor),static_cast<int>(GetPoint(16))),false,false);
    BYTE magicAttackLevel=m_points.level;int iq=static_cast<int>(GetPoint(15));
    PointChange(22,mul(add(magicAttackLevel,iq),2),false,false);
    BYTE magicDefenseLevel=m_points.level;iq=static_cast<int>(GetPoint(15));ht=static_cast<int>(GetPoint(13));
    PointChange(23,add(add(magicDefenseLevel,add(ht,mul(iq,3))/3),armor/2),false,false);
    PointChange(18,static_cast<int>(GetPoint(95)),false,false);
    PointChange(16,static_cast<int>(GetPoint(96)),false,false);
    PointChange(16,static_cast<int>(GetPoint(111)),false,false);
    PointChange(22,static_cast<int>(GetPoint(97)),false,false);
    PointChange(23,static_cast<int>(GetPoint(98)),false,false);
}

// Full original0812c520 body, extracted without narrowing from saved mount_polymorph candidate.
void CHARACTER::ComputePoints() {
    using namespace mount_polymorph_detail;
    const long stat=GetPoint(26);
    const long reset=GetPoint(112);
    const long skill=GetPoint(28);
    const long subskill=GetPoint(27);
    const long horseSkill=GetPoint(113);
    const long step=GetPoint(25);
    const long attacker=GetPoint(91);
    const long tanker=GetPoint(92);
    const long buffer=GetPoint(103);
    const long skillMaster=GetPoint(104);
    const long haste=GetPoint(110);
    const long defender=GetPoint(111);
    const long hpRecovery=GetPoint(35);
    const long spRecovery=GetPoint(36);
    // Exactly the instant points array; do not clear maxima/parts/other fields.
    std::memset(m_pointsInstant.points,0,sizeof(m_pointsInstant.points));
    SetPoint(26,stat);
    SetPoint(28,skill);
    SetPoint(27,subskill);
    SetPoint(113,horseSkill);
    SetPoint(25,step);
    SetPoint(112,reset);
    long value=GetRealPoint(12);SetPoint(12,value);
    value=GetRealPoint(13);SetPoint(13,value);
    value=GetRealPoint(14);SetPoint(14,value);
    value=GetRealPoint(15);SetPoint(15,value);
    WORD part=GetOriginalPart(0);SetPart(0,part);
    part=GetOriginalPart(1);SetPart(1,part);
    part=GetOriginalPart(2);SetPart(2,part);
    part=GetOriginalPart(3);SetPart(3,part);
    SetPoint(91,attacker);SetPoint(92,tanker);
    SetPoint(103,buffer);SetPoint(104,skillMaster);
    SetPoint(110,haste);SetPoint(111,defender);
    SetPoint(35,hpRecovery);SetPoint(36,spRecovery);
    SetPoint(125,0);SetPoint(126,0);
    int maxHP,maxSP,maxStamina;
    if(!GetDesc()) {
        maxSP=0;maxStamina=0;
        maxHP=static_cast<int>(Signed(static_cast<unsigned int>(m_pkMobData->m_table.dwMaxHP)));
        SetPoint(17,m_pkMobData->m_table.sAttackSpeed);
        SetPoint(19,m_pkMobData->m_table.sMovingSpeed);
        value=m_pkMobData->m_table.sAttackSpeed;
    } else {
        BYTE job=GetJob();
        int random=GetRandomHP();
        int base=JobInitialPoints[job].max_hp;
        value=GetPoint(13);
        job=GetJob();
        maxHP=static_cast<int>(Add(Add(random,base),Mul(value,JobInitialPoints[job].hp_per_ht)));
        job=GetJob();
        random=GetRandomSP();
        base=JobInitialPoints[job].max_sp;
        value=GetPoint(15);
        job=GetJob();
        maxSP=static_cast<int>(Add(Add(random,base),Mul(value,JobInitialPoints[job].sp_per_iq)));
        job=GetJob();
        value=GetPoint(13);
        BYTE coefficientJob=GetJob();
        maxStamina=static_cast<int>(Add(Mul(value,JobInitialPoints[coefficientJob].stamina_per_con),
                                      JobInitialPoints[job].max_stamina));
        SetPoint(19,100);SetPoint(17,100);
        value=GetPoint(110);
        PointChange(17,static_cast<int>(value),false,false);
        value=100;
    }
    SetPoint(21,value);
    if(GetDesc() && m_dwMountVnum) {
        int horse=c_aHorseStat[GetHorseLevel()].iST;
        value=GetPoint(12);
        if(value<horse) {
            horse=c_aHorseStat[GetHorseLevel()].iST;value=GetPoint(12);
            PointChange(12,static_cast<int>(Sub(horse,value)),false,false);
        }
        horse=c_aHorseStat[GetHorseLevel()].iDX;value=GetPoint(14);
        if(value<horse) {
            horse=c_aHorseStat[GetHorseLevel()].iDX;value=GetPoint(14);
            PointChange(14,static_cast<int>(Sub(horse,value)),false,false);
        }
        horse=c_aHorseStat[GetHorseLevel()].iHT;value=GetPoint(13);
        if(value<horse) {
            horse=c_aHorseStat[GetHorseLevel()].iHT;value=GetPoint(13);
            PointChange(13,static_cast<int>(Sub(horse,value)),false,false);
        }
        horse=c_aHorseStat[GetHorseLevel()].iIQ;value=GetPoint(15);
        if(value<horse) {
            horse=c_aHorseStat[GetHorseLevel()].iIQ;value=GetPoint(15);
            PointChange(15,static_cast<int>(Sub(horse,value)),false,false);
        }
    }
    ComputeBattlePoints();
    if(maxHP!=m_pointsInstant.iMaxHP) SetRealPoint(6,maxHP);
    PointChange(6,0,false,false);
    if(maxSP!=m_pointsInstant.iMaxSP) SetRealPoint(8,maxSP);
    PointChange(8,0,false,false);
    SetMaxStamina(maxStamina);
    m_pointsInstant.dwImmuneFlag=0;
    for(int cell=0;cell<19;++cell) {
        BYTE slot=static_cast<BYTE>(cell);
        if(GetWear(slot)) {
            GetWear(slot)->ModifyPoints(true);
            LPITEM item=GetWear(slot); // Reload after item callbacks.
            DWORD flags=item->GetProto() ? item->GetProto()->dwImmuneFlag : 0;
            m_pointsInstant.dwImmuneFlag|=flags;
        }
    }
    int hp=m_points.hp;
    int maximum=m_pointsInstant.iMaxHP;
    if(maximum<hp) PointChange(5,static_cast<int>(Sub(maximum,hp)),false,false);
    int sp=m_points.sp;
    maximum=m_pointsInstant.iMaxSP;
    if(maximum<sp) PointChange(7,static_cast<int>(Sub(maximum,sp)),false,false);
    UpdatePacket();
    ComputeSkillPoints();
    RefreshAffect();
}

// 0812e2c0
void CHARACTER::CheckMaximumPoints()
{
    if (m_pointsInstant.iMaxHP < m_points.hp)
        PointChange(5, mount_polymorph_detail::Sub(m_pointsInstant.iMaxHP, m_points.hp), false, false);
    if (m_pointsInstant.iMaxSP < m_points.sp)
        PointChange(7, mount_polymorph_detail::Sub(m_pointsInstant.iMaxSP, m_points.sp), false, false);
}
//08135b10
void CHARACTER::SetPolymorph(DWORD race,bool maintain) {
    race = static_cast<std::uint32_t>(race);
    if(race<4) { race=0; maintain=false; }
    if(m_dwPolymorphRace==race) return;
    m_dwPolymorphRace=race;
    m_bPolyMaintainStat=maintain;
    sys_log(0,"POLYMORPH: %s race %lu ",GetName(),race);
    if(race) StopRiding(); // Virtual primary vptr slot13; target0816d720.
    m_bAddChrState|=2;
    m_afAffectFlag.bits[0]|=4;
    ViewReencode();
    m_bAddChrState&=0xfd;
    if(!maintain) {
        PointChange(12,0,false,false);
        PointChange(14,0,false,false);
        PointChange(15,0,false,false);
        PointChange(13,0,false,false);
    }
    ComputeBattlePoints();
}
//08136010
void CHARACTER::MountVnum(DWORD vnum) {
    vnum = static_cast<std::uint32_t>(vnum);
    if(m_dwMountVnum==vnum) return;
    m_dwMountVnum=vnum;
    ViewReencode();
    ComputePoints();
}

//08129ae0
bool CHARACTER::Show(long map,long x,long y,long z,bool spawnMotion) {
    SECTREE* tree=SECTREE_MANAGER::instance().Get(static_cast<DWORD>(map),
                                               static_cast<DWORD>(x),static_cast<DWORD>(y));
    if(!tree) { sys_log(0,"cannot find sectree by %dx%d mapindex %d",x,y,map);return false; }
    SetMapIndex(map);
    SECTREE* previous=GetSectree();
    bool changed=!previous || previous!=tree;
    if(changed) { if(previous) previous->RemoveEntity(this);ViewCleanup(); }
    if(m_bCharType==6) {
        sys_log(0,"SHOW: %s %dx%dx%d",GetName(),x,y,z);
        if(GetStamina()<GetMaxStamina()) StartAffectEvent();
    } else if(m_pkMobData) {
        m_pkMobInst->m_posLastAttacked.x=x;
        m_pkMobInst->m_posLastAttacked.y=y;
        m_pkMobInst->m_posLastAttacked.z=z;
    }
    if(spawnMotion) { m_bAddChrState|=2;m_afAffectFlag.bits[0]|=4; }
    SetXYZ(x,y,z);
    m_posDest.x=x;m_posDest.y=y;m_posDest.z=z;
    if(changed) {
        EncodeInsertPacket(this); // Primary virtual slot2.
        tree->InsertEntity(this);
        UpdateSectree();
    } else {
        ViewReencode();
        sys_log(0,"      in same sectree"); // No format fields; stale args omitted.
    }
    m_bAddChrState&=0xfd;
    return true;
}
//0812d810. Existing destination x/y affects whether duration is recomputed.
bool CHARACTER::Goto(long x,long y) {
    const auto destX = static_cast<std::int32_t>(x);
    const auto destY = static_cast<std::int32_t>(y);
    if (static_cast<std::int32_t>(GetX()) != destX || static_cast<std::int32_t>(GetY()) != destY) {
        if (static_cast<std::int32_t>(m_posDest.x) != destX || static_cast<std::int32_t>(m_posDest.y) != destY) {
            m_posDest.x = destX;
            m_posDest.y = destY;
            CalculateMoveDuration();
            m_dwStateDuration=4;
            if(!IsState(m_stateMove)) {
                CHARACTER* victim=GetVictim();
                MonsterLog("[MOVE] %s",victim?"\264\353\273\363\303\337\300\373":
                                               "\261\327\263\311\300\314\265\277");
                victim=GetVictim();
                if(victim) MonsterChat(1);
            }
            GotoState(m_stateMove);
            return true;
        }
        if(!IsState(m_stateMove)) { m_dwStateDuration=4;GotoState(m_stateMove); }
    }
    return false;
}

bool CHARACTER::IsSummonMonster()
{
    return GetSummonVnum() != 0;
}

DWORD CHARACTER::GetSummonVnum()
{
    return m_pkMobData ? m_pkMobData->m_table.dwSummonVnum : 0;
}

// 08136830
BOOL CHARACTER::IsHack(bool sendMessage, bool checkShopOwner, int limitTime)
{
    int pulse=thecore_pulse();
    const int limit = mount_polymorph_detail::Mul(passes_per_sec, limitTime);
    if (test_server) sendMessage=true;
    if (mount_polymorph_detail::Sub(pulse,GetSafeboxLoadTime())<limit) {
        if (sendMessage) ChatPacket(1,locale_find("\303\242\260\355\270\246\040\277\254\310\304\040\045\144\303\312\040\300\314\263\273\277\241\264\302\040\264\331\270\245\260\367\300\270\267\316\040\300\314\265\277\307\322\274\366\040\276\370\275\300\264\317\264\331\056",g_iUseLocale),limitTime);
        if (test_server)
            ChatPacket(1,"[TestOnly]Pulse %d LoadTime %d PASS %d",pulse,GetSafeboxLoadTime(),limit);
        return 1;
    }
    if (m_pkExchange || m_pkMyShop || (checkShopOwner && m_pkChrShopOwner) ||
        m_isOpenSafebox || m_pointsInstant.pCubeNpc) {
        if (sendMessage) ChatPacket(1,locale_find("\260\305\267\241\303\242\054\303\242\260\355\040\265\356\300\273\040\277\254\040\273\363\305\302\277\241\274\255\264\302\040\264\331\270\245\260\367\300\270\267\316\040\300\314\265\277\054\301\276\267\341\040\307\322\274\366\040\276\370\275\300\264\317\264\331",g_iUseLocale));
        return 1;
    }
    const char* message;
    if (mount_polymorph_detail::Sub(pulse,GetExchangeTime())<limit || mount_polymorph_detail::Sub(pulse,GetMyShopTime())<limit)
        message="\260\305\267\241\040\310\304\040\045\144\303\312\040\300\314\263\273\277\241\264\302\040\264\331\270\245\301\366\277\252\300\270\267\316\040\300\314\265\277\040\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056";
    else if (mount_polymorph_detail::Sub(pulse,GetRefineTime())<limit) message="\276\306\300\314\305\333\040\260\263\267\256\310\304\040\045\144\303\312\040\300\314\263\273\277\241\264\302\040\261\315\310\257\272\316\054\261\315\310\257\261\342\276\357\272\316\270\246\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056";
    else return 0;
    if (sendMessage) ChatPacket(1,locale_find(message,g_iUseLocale),limitTime);
    return 1;
}

void CHARACTER::StartStaminaConsume()
{
    if (m_bStaminaConsume)
        return;
    PointChange(POINT_STAMINA, 0, false, false);
    m_bStaminaConsume = true;
    const bool halfConsume = IsStaminaHalfConsume();
    const int stamina = GetStamina();
    const int rate = halfConsume ? passes_per_sec / 2 : passes_per_sec;
    ChatPacket(5, "StartStaminaConsume %d %d", rate, stamina);
}

bool CHARACTER::IsStaminaConsume()
{
    return m_bStaminaConsume;
}

bool CHARACTER::IsStaminaHalfConsume()
{
    return IsEquipUniqueItem(70040);
}

float CHARACTER::GetMoveSpeed()
{
    const float scaledSpeed = static_cast<float>(GetMoveMotionSpeedPrecise() * 10000.0L);
    const int duration = CalculateDuration(GetLimitPoint(POINT_MOV_SPEED), 10000);
    return static_cast<float>(static_cast<long double>(scaledSpeed) / duration);
}

bool CHARACTER::IsChangeAttackPosition(LPCHARACTER target)
{
    if (GetCharType() == CHAR_TYPE_PC)
        return true;
    using mount_polymorph_detail::Sub;
    const int dx = Sub(GetX(), target->GetX());
    const int dy = Sub(GetY(), target->GetY());
    const int distance = DISTANCE_APPROX(dx, dy);
    const DWORD interval = distance > GetMobAttackRange() + 100 ? 1000 : 10000;
    const auto now = get_dword_time();
    return interval < static_cast<std::uint32_t>(now - m_dwLastChangeAttackPositionTime);
}

bool CHARACTER::Return()
{
    if (GetCharType() == CHAR_TYPE_PC || !m_pkMobInst)
        return false;

    SetVictim(nullptr);
    const long x = m_pkMobInst->m_posLastAttacked.x;
    const long y = m_pkMobInst->m_posLastAttacked.y;
    SetRotationToXY(x, y);
    if (!Goto(x, y))
        return false;

    SendMovePacket(0, 0, 0, 0, 0, 0, -1);
    sys_log(0, "%s %p \306\367\261\342\307\317\260\355 \265\271\276\306\260\241\300\332! %d %d",
            GetName(), static_cast<void*>(this), static_cast<int>(x), static_cast<int>(y));
    if (m_pkParty)
        m_pkParty->SendMessage(this, PM_RETURN, x, y);
    return true;
}

bool CHARACTER::Follow(LPCHARACTER target, float minDistance)
{
    if (GetDesc()) {
        _sys_err("Follow", 0x137d, "CHARACTER::Follow : PC cannot use this method");
        return false;
    }
    if (!target || !m_pkMobInst)
        return false;

    const long startX = GetX();
    const long startY = GetY();
    using namespace mount_polymorph_detail;
    const auto distanceFromStart = [startX, startY](long targetX, long targetY) {
        const long dx = Sub(targetX, startX);
        const long dy = Sub(targetY, startY);
        return static_cast<float>(std::sqrt(static_cast<double>(Add(Mul(dx, dx), Mul(dy, dy)))));
    };
    long x = target->GetX();
    long y = target->GetY();
    const bool noMove = (GetAIFlag() & 2u) != 0;
    if (target->GetDesc() && (!m_pkParty || !m_pkParty->GetLeader() || m_pkParty->GetLeader() == this)) {
        const auto now = get_dword_time();
        const DWORD elapsed = static_cast<std::uint32_t>(now - m_pkMobInst->m_dwLastAttackedTime);
        if (elapsed >= 15000) {
            const int limit = noMove ? GetMobTable().wAttackRange : 5000;
            const int distance = noMove
                ? DISTANCE_APPROX(Sub(target->GetX(), GetX()), Sub(target->GetY(), GetY()))
                : DISTANCE_APPROX(Sub(m_pkMobInst->m_posLastAttacked.x, GetX()),
                                  Sub(m_pkMobInst->m_posLastAttacked.y, GetY()));
            if (distance > limit && Return())
                return true;
        }
    }
    if (noMove)
        return false;
    if (IsGuardNPC()) {
        const auto dx = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(m_pkMobInst->m_posLastAttacked.x) - static_cast<std::uint32_t>(startX));
        const auto dy = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(m_pkMobInst->m_posLastAttacked.y) - static_cast<std::uint32_t>(startY));
        if (DISTANCE_APPROX(dx, dy) > 5000 && Return())
            return true;
    }

    if (target->IsState(target->m_stateMove)) {
        const BYTE battleType = GetMobBattleType();
        if (battleType != 1 && battleType != 2) {
            const float angle = GetDegreeDelta(target->GetRotation(),
                                              GetDegreeFromPositionXY(startX, startY, x, y));
            const float targetSpeed = target->GetMoveSpeed();
            const float ownSpeed = GetMoveSpeed();
            const float distance = distanceFromStart(x, y);
            constexpr double pi = 3.141592653589793;
            const double radians = static_cast<double>(static_cast<long double>(pi) * angle / 180.0L);
            const double approachSpeed = static_cast<double>(
                ownSpeed - static_cast<long double>(std::cos(radians)) * targetSpeed);
            if (static_cast<float>(approachSpeed) > 0.0f) {
                const float lead = static_cast<float>(static_cast<long double>(distance) /
                                                      approachSpeed * targetSpeed);
                float dx, dy;
                GetDeltaByDegree(target->GetRotation(), lead, &dx, &dy);
                x = Add(x, MotionTruncate32(dx));
                y = Add(y, MotionTruncate32(dy));
                const long projectionBaseX = GetX();
                const long projectionBaseY = GetY();
                const long double projectedX = static_cast<long double>(x) - projectionBaseX;
                const long double projectedY = static_cast<long double>(y) - projectionBaseY;
                const float predicted = static_cast<float>(std::sqrt(static_cast<double>(
                    Sub(x, projectionBaseX) * projectedX + Sub(y, projectionBaseY) * projectedY)));
                if (predicted > distance) {
                    const long clampBaseX = GetX();
                    const long clampBaseY = GetY();
                    x = MotionTruncate32(clampBaseX + static_cast<long double>(Sub(x, clampBaseX)) * distance / predicted);
                    y = MotionTruncate32(clampBaseY + static_cast<long double>(Sub(y, clampBaseY)) * distance / predicted);
                }
            }
        }
    }

    SetRotationToXY(x, y);
    const float distance = distanceFromStart(x, y);
    if (distance <= minDistance)
        return false;

    float dx, dy;
    float travel;
    if (IsChangeAttackPosition(target) && GetMobRank() < 4) {
        SetChangeAttackPositionTime();
        int rotation = number_ex(0, 359, const_cast<char*>("char.cpp"), 0x140d);
        if (distance < 500.0f) {
            rotation = MotionTruncate32(GetDegreeFromPositionXY(x, y, startX, startY));
            rotation += number_ex(-90, 90, const_cast<char*>("char.cpp"), 0x1411);
            rotation += number_ex(-90, 90, const_cast<char*>("char.cpp"), 0x1411);
        }
        GetDeltaByDegree(rotation, minDistance, &dx, &dy);
        x = Add(x, MotionTruncate32(dx));
        y = Add(y, MotionTruncate32(dy));
        travel = distanceFromStart(x, y);
    } else {
        travel = distance - minDistance;
        GetDeltaByDegree(GetRotation(), travel, &dx, &dy);
        x = Add(startX, MotionTruncate32(dx));
        y = Add(startY, MotionTruncate32(dy));
    }
    if (!Goto(x, y))
        return false;
    SendMovePacket(0, 0, 0, 0, 0, 0, -1);
    sys_log(1, "Follow! %s -> %s Dist %.1f", GetName(), target->GetName(), travel);
    return true;
}

void CHARACTER::SetBerserk(bool enabled)
{
    if (m_pkMobInst)
        m_pkMobInst->m_IsBerserk = enabled;
}

bool CHARACTER::IsGodSpeed()
{
    return m_pkMobInst && m_pkMobInst->m_IsGodSpeed;
}

void CHARACTER::SetGodSpeed(bool enabled)
{
    if (m_pkMobInst) {
        m_pkMobInst->m_IsGodSpeed = enabled;
        SetPoint(POINT_ATT_SPEED, enabled ? 250 : GetMobTable().sAttackSpeed);
    }
}

long kill_ore_load_event(LPEVENT event)
{
    CHARACTER* character = *static_cast<CHARACTER**>(event->info);
    character->m_pkMiningEvent = 0;
    CHARACTER_MANAGER::instance().DestroyCharacter(character);
    return 0;
}

// 0812ac60
void CHARACTER::SetProto(const CMob* mob)
{
    delete m_pkMobInst;
    m_pkMobData = mob;
    m_pkMobInst = new CMobInstance;
    const TMobTable& table = m_pkMobData->m_table;
    m_bPKMode = 2;
    m_bCharType = table.bType;
    SetLevel(table.bLevel);
    SetEmpire(table.bEmpire);
    SetExp(table.dwExp);
    SetRealPoint(12, table.bStr);
    SetRealPoint(14, table.bDex);
    SetRealPoint(13, table.bCon);
    SetRealPoint(15, table.bInt);
    ComputePoints();
    m_points.hp = m_pointsInstant.iMaxHP;
    SetSP(GetMaxSP());
    m_pointsInstant.dwAIFlag = table.dwAIFlag;
    m_pointsInstant.dwImmuneFlag = table.dwImmuneFlag;
    AssignTriggers(&table);
    ApplyMobAttribute(&table);

    if (IsStone())
        DetermineDropMetinStone();
    if (m_bCharType == 3 || m_bCharType == 9)
        StartWarpNPCEvent();
    CHARACTER_MANAGER::instance().RegisterRaceNumMap(this);

    if (GetRaceNum() == 20031) {
        SetPoint(95, 10);
        SetPoint(96, g_iUseLocale == 0 ? 15 : 6);
        m_dwPlayStartTime = static_cast<std::uint32_t>(get_dword_time() + 30000);
        if (test_server)
            m_dwPlayStartTime = static_cast<std::uint32_t>(get_dword_time() + 30000);
        if (LC_IsJapan())
            m_dwPlayStartTime = static_cast<std::uint32_t>(get_dword_time() + 60000);
    }

    WORD race = GetRaceNum();
    if (race >= 20035 && race <= 20037) {
        m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
        m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
        m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
    }
    if (GetRaceNum() == 20038) {
        m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
        m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
        m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
    }

    if (m_bCharType != 8)
        race = GetRaceNum();
    if (m_bCharType == 8 || (race >= 20101 && race <= 20109)) {
        m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
        m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
        m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
    }

    race = GetRaceNum();
    if (race >= 20047 && race <= 20059) {
        void* raw;
        const DWORD id = __AllocEventInfo(sizeof(CHARACTER*), &raw);
        *static_cast<CHARACTER**>(raw) = this;
        const int seconds = number_ex(420, 900, "char.cpp", 0x731);
        m_pkMiningEvent = event_create_ex(kill_ore_load_event, id,
            mount_polymorph_detail::Mul(seconds, passes_per_sec),
            const_cast<char*>("char.cpp"), 0x731);
    }
}

extern int g_nPortalLimitTime;

namespace {
struct WarpEventInfo { LPCHARACTER character; };

class FuncCheckWarp {
public:
    explicit FuncCheckWarp(LPCHARACTER warp)
        : m_lX(warp->GetX()), m_lY(warp->GetY()), m_bInvalid(false)
    {
        if (sscanf(warp->GetName(), " %*s %ld %ld ", &m_lTargetX, &m_lTargetY) != 2) {
            if (number_ex(1,100,"char.cpp",0x179d)<5)
                _sys_err("FuncCheckWarp",0x179e,"Warp NPC name wrong : %s",warp->GetName());
            m_bInvalid=true;
            return;
        }
        m_lTargetX=mount_polymorph_detail::Mul(m_lTargetX,100);
        m_lTargetY=mount_polymorph_detail::Mul(m_lTargetY,100);
        m_bEmpire=warp->GetEmpire();
        m_bUseWarp=!warp->IsGoto();
        if (!m_bUseWarp) {
            SECTREE_MAP* map=SECTREE_MANAGER::instance().GetMap(warp->GetMapIndex());
            m_lTargetX=mount_polymorph_detail::Add(m_lTargetX,map->m_setting.iBaseX);
            m_lTargetY=mount_polymorph_detail::Add(m_lTargetY,map->m_setting.iBaseY);
        }
    }

    bool IsInvalid() const { return m_bInvalid; }

    void operator()(LPENTITY entity)
    {
        if (m_bInvalid || !entity->IsType(0) || !entity->GetDesc()) return;
        LPCHARACTER ch=static_cast<LPCHARACTER>(entity);
        int dx=mount_polymorph_detail::Sub(ch->GetX(),m_lX);
        int dy=mount_polymorph_detail::Sub(ch->GetY(),m_lY);
        if (DISTANCE_APPROX(dx,dy)>=301) return;
        if (m_bEmpire && ch->GetEmpire() && ch->GetEmpire()!=m_bEmpire) return;
        if (ch->IsHack(true,true,g_nPortalLimitTime) || !ch->CanHandleItem(false,true)) return;
        if (m_bUseWarp) ch->WarpSet(m_lTargetX,m_lTargetY,0);
        else ch->Show(ch->GetMapIndex(),m_lTargetX,m_lTargetY,0x7fffffff,false);
    }

private:
    long m_lX, m_lY, m_lTargetX, m_lTargetY;
    bool m_bInvalid, m_bUseWarp;
    BYTE m_bEmpire;
};
}

long warp_npc_event(LPEVENT event)
{
    LPCHARACTER warp=static_cast<WarpEventInfo*>(event->info)->character;
    if (!warp->GetSectree()) {
        warp->m_pkWarpNPCEvent=0;
        return 0;
    }
    FuncCheckWarp check(warp);
    if (!check.IsInvalid()) warp->GetSectree()->ForEachAround(check);
    return passes_per_sec/2;
}

void CHARACTER::StartWarpNPCEvent()
{
    if (!m_pkWarpNPCEvent &&
        (IsWarp() || IsGoto())) {
        void* rawInfo = nullptr;
        const DWORD id = __AllocEventInfo(sizeof(WarpEventInfo), &rawInfo);
        static_cast<WarpEventInfo*>(rawInfo)->character = this;
        m_pkWarpNPCEvent=event_create_ex(warp_npc_event,id,passes_per_sec/2,
                                         const_cast<char*>("char.cpp"),0x1807);
    }
}

int CHARACTER::GetPremiumRemainSeconds(BYTE type)
{
    if (type>=9) return 0;
    const time_t now = get_global_time();
    return mount_polymorph_detail::Sub(m_aiPremiumTimes[type], now);
}

void CHARACTER::ResetPoint(int level)
{
    using namespace mount_polymorph_detail;

    const TJobInitialPoints& job = JobInitialPoints[GetJob()];
    PointChange(POINT_LEVEL, Sub(level, GetLevel()), false, false);

    const int base[] = {job.st, job.ht, job.dx, job.iq};
    for (BYTE point = POINT_ST; point <= POINT_IQ; ++point)
    {
        SetRealPoint(point, base[point - POINT_ST]);
        SetPoint(point, GetRealPoint(point));
    }
    const int hpUpper = JobInitialPoints[GetJob()].hp_per_lv_end;
    const int hpLower = JobInitialPoints[GetJob()].hp_per_lv_begin;
    SetRandomHP(Mul(number_ex(hpLower, hpUpper, "char.cpp", 0x159c), Sub(level, 1)));

    const int spUpper = JobInitialPoints[GetJob()].sp_per_lv_end;
    const int spLower = JobInitialPoints[GetJob()].sp_per_lv_begin;
    SetRandomSP(Mul(number_ex(spLower, spUpper, "char.cpp", 0x159d), Sub(level, 1)));
    const long levelStep = GetPoint(POINT_LEVEL_STEP);
    const long currentStat = GetPoint(POINT_STAT);
    PointChange(POINT_STAT, Sub(Sub(Add(Mul(level, 3), levelStep), currentStat), 3), false, false);
    ComputePoints();
    PointChange(POINT_HP, Sub(GetMaxHP(), GetHP()), false, false);
    PointChange(POINT_SP, Sub(GetMaxSP(), GetSP()), false, false);
    PointsPacket();
    LogManager::instance().CharLog(GetPlayerID(), GetX(), GetY(), 0, "RESET_POINT", "");
}

void CHARACTER::SetWalkingMode(bool walking)
{
    SetNowWalking(walking);
    SetWalking(walking);
}

void CHARACTER::SetBlockMode(BYTE mode)
{
    SetBlockModeForce(mode);
    const char* const flags[] = {
        "game_option.block_exchange",
        "game_option.block_party_invite",
        "game_option.block_guild_invite",
        "game_option.block_whisper",
        "game_option.block_messenger_invite",
        "game_option.block_party_request",
    };
    for (unsigned bit = 0; bit < sizeof(flags) / sizeof(flags[0]); ++bit)
        SetQuestFlag(flags[bit], (mode >> bit) & 1);
}

void CHARACTER::SetBlockModeForce(BYTE mode)
{
    m_pointsInstant.bBlockMode = mode;
    ChatPacket(5, "setblockmode %d", static_cast<int>(mode));
}

void CHARACTER::SetQuestFlag(const std::string& flag, int value)
{
    quest::CQuestManager::instance().GetPC(m_dwPlayerID)->SetFlag(flag, value, false);
}

int CHARACTER::GetQuestFlag(const std::string& flag)
{
    quest::PC* pc=quest::CQuestManager::instance().GetPC(m_dwPlayerID);
    return pc->GetFlag(flag);
}

void CHARACTER::Stop()
{
    if (!IsState(m_stateIdle)) MonsterLog("[IDLE] \301\244\301\366");
    GotoState(m_stateIdle);
    m_posStart.x=m_posDest.x=GetX();
    m_posStart.y=m_posDest.y=GetY();
}

extern bool IS_SUMMONABLE_ZONE(int map);

bool CHARACTER::WarpToPID(DWORD pid)
{
    if (CHARACTER* target = CHARACTER_MANAGER::instance().FindByPID(pid))
    {
        if (IS_SUMMONABLE_ZONE(target->GetMapIndex()))
        {
            WarpSet(target->GetX(), target->GetY(), 0);
            return true;
        }
    }
    else
    {
        CCI* targetPeer = P2P_MANAGER::instance().FindByPID(pid);
        if (!targetPeer)
        {
            ChatPacket(1, "%s", locale_find("\273\363\264\353\271\346\300\314 \277\302\266\363\300\316 \273\363\305\302\260\241 \276\306\264\325\264\317\264\331.", g_iUseLocale));
            return false;
        }
        if (targetPeer->bChannel != g_bChannel)
        {
            ChatPacket(1, locale_find("\273\363\264\353\271\346\300\314 %d \303\244\263\316\277\241 \300\326\275\300\264\317\264\331. (\307\366\300\347 \303\244\263\316 %d)", g_iUseLocale),
                       static_cast<unsigned>(targetPeer->bChannel), static_cast<unsigned>(g_bChannel));
            return false;
        }
        if (IS_SUMMONABLE_ZONE(targetPeer->bMapIndex) && targetPeer->pkDesc)
        {
            BYTE packet[9] = {12};
            const std::uint32_t from = GetPlayerID();
            const std::uint32_t target = pid;
            std::memcpy(packet + 1, &from, sizeof(from));
            std::memcpy(packet + 5, &target, sizeof(target));
            targetPeer->pkDesc->Packet(packet, sizeof(packet));
            if (test_server)
                ChatPacket(3, "sent find position packet for teleport");
            return true;
        }
    }
    ChatPacket(1, "%s", locale_find("\273\363\264\353\271\346\300\314 \300\326\264\302 \260\367\300\270\267\316 \277\366\307\301\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
    return false;
}

bool CHARACTER::WarpSet(long x, long y, long privateMapIndex)
{
    if (!GetDesc()) return false;
    long map, addr;
    WORD port;
    if (!CMapLocation::instance().Get(x,y,map,addr,port)) {
        _sys_err("WarpSet",0x12f2,"cannot find map location index %d x %d y %d name %s",map,x,y,GetName());
        return false;
    }
    if (privateMapIndex>9999) {
        if (privateMapIndex/10000!=map) {
            _sys_err("WarpSet",0x12fa,"Invalid map inedx %d, must be child of %d",privateMapIndex,map);
            return false;
        }
        map=privateMapIndex;
    }
    Stop();
    Save();
    SECTREE* tree=GetSectree();
    if (tree) {
        tree->RemoveEntity(this);
        ViewCleanup();
        EncodeRemovePacket(this);
    }
    m_lWarpMapIndex=map;
    m_posWarp.x=x;
    m_posWarp.y=y;
    sys_log(0,"WarpSet %s %d %d current map %ld target map %ld",GetName(),x,y,GetMapIndex(),map);
    TPacketGCWarp packet;
    packet.bHeader=65;
    packet.lX=static_cast<std::int32_t>(x);
    packet.lY=static_cast<std::int32_t>(y);
    packet.lAddr=static_cast<std::uint32_t>(addr);
    packet.wPort=port;
    GetDesc()->Packet(&packet,sizeof(packet));
    if (!LC_IsNewCIBN()) {
        char hint[256];
        sprintf(hint,"%s MapIdx %lu DestMapIdx%lu  DestX%lu DestY%lu Empire%d",
                GetName(),GetMapIndex(),privateMapIndex,x,y,static_cast<unsigned int>(m_bEmpire));
        LogManager::instance().ItemLog(GetPlayerID(),GetX(),GetY(),0,"WARP",hint);
    }
    return true;
}

// 081359c0
bool CHARACTER::IsGuardNPC()
{
    if (m_bCharType == 6)
        return false;
    const WORD race = GetRaceNum();
    return race == 11000 || race == 11002 || race == 11004;
}

// 08134c40
void CHARACTER::MonsterChat(BYTE type)
{
    if (GetDesc())
        return;

    char script[256];
    if (IsMonster()) {
        if (number_ex(0, 60, "char.cpp", 0x1629) != 0)
            return;
        const int index = type * 3 + number_ex(1, 3, "char.cpp", 0x162e);
        const int race = GetRaceNum();
        std::snprintf(script, sizeof(script),
            "(locale.monster_chat[%i] and locale.monster_chat[%i][%d] or '')",
            race, race, index);
    } else {
        if (type != 0)
            return;
        const bool guard = IsGuardNPC();
        if (number_ex(0, guard ? 6 : 30, "char.cpp", guard ? 0x1637 : 0x163c) != 0)
            return;
        const int race = GetRaceNum();
        std::snprintf(script, sizeof(script),
            "(locale.monster_chat[%i] and locale.monster_chat[%i][number(1, table.getn(locale.monster_chat[%i]))] or '')",
            race, race, race);
    }

    const std::string message = quest::ScriptToString(script);
    if (message.empty())
        return;

    TPacketGCChat packet;
    packet.header = 4;
    packet.type = 0;
    packet.size = static_cast<WORD>(sizeof(packet) + message.size() + 1);
    packet.id = static_cast<DWORD>(GetVID());
    packet.bEmpire = 0;
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    buffer.write(message.c_str(), message.size() + 1);
    const int size = buffer.size();
    const void* data = buffer.read_peek();
    PacketAround(data, size, 0);
}

void CHARACTER::WarpEnd()
{
    if (test_server) sys_log(0,"WarpEnd %s",GetName());
    if (!m_posWarp.x && !m_posWarp.y) return;
    int map=m_lWarpMapIndex;
    if (map>10000) map/=10000;
    if (!map_allow_find(map)) {
        _sys_err("WarpEnd",0x133d,"location %d %d not allowed to login this server",m_posWarp.x,m_posWarp.y);
        GetDesc()->SetPhase(0);
        return;
    }
    sys_log(0,"WarpEnd %s %d %lu %lu",GetName(),m_lWarpMapIndex,m_posWarp.x,m_posWarp.y);
    Show(m_lWarpMapIndex,m_posWarp.x,m_posWarp.y,0,false);
    Stop();
    m_posWarp.x=m_posWarp.y=m_posWarp.z=0;
    m_lWarpMapIndex=0;
    TPacketGGLogin packet;
    packet.bHeader=1;
    std::strcpy(packet.szName,GetName());
    packet.dwPID=m_dwPlayerID;
    packet.bEmpire=m_bEmpire;
    packet.bMapIndex=static_cast<BYTE>(SECTREE_MANAGER::instance().GetMapIndex(GetX(),GetY()));
    packet.bChannel=g_bChannel;
    P2P_MANAGER::instance().Send(&packet,sizeof(packet),0);
}

CDungeon* CHARACTER::GetDungeonForce()
{
    return m_lWarpMapIndex > 10000 ? CDungeonManager::instance().Find(m_lWarpMapIndex) : m_pkDungeon;
}

void CHARACTER::SaveExitLocation()
{
    m_posExit=GetXYZ();
    m_lExitMapIndex=GetMapIndex();
}

void CHARACTER::ExitToSavedLocation()
{
    WarpSet(m_posWarp.x,m_posWarp.y,m_lWarpMapIndex);
    m_posExit.x=m_posExit.y=m_posExit.z=0;
    m_lExitMapIndex=0;
}

void CHARACTER::SetWarpLocation(long map, long x, long y)
{
    m_posWarp.x=mount_polymorph_detail::Mul(x,100);
    m_posWarp.y=mount_polymorph_detail::Mul(y,100);
    m_lWarpMapIndex=map;
}

// 0812bb90
bool CHARACTER::IsRaceFlag(DWORD bit)
{
    return m_pkMobData && (m_pkMobData->m_table.dwRaceFlag & bit) != 0;
}

bool CHARACTER::IsBerserk()
{
    return m_pkMobInst && m_pkMobInst->m_IsBerserk;
}

// 0812bc00
float CHARACTER::GetMobDamageMultiply()
{
    const float multiplier = GetMobTable().fDamMultiply;
    return IsBerserk() ? multiplier + multiplier : multiplier;
}

// 081361a0
void CHARACTER::SyncPacket()
{
    TEMP_BUFFER buffer;
    TPacketGCSyncPosition packet;
    TPacketCGSyncPositionElement element;
    element.dwVID = static_cast<std::uint32_t>(static_cast<DWORD>(GetVID()));
    element.lX = static_cast<std::int32_t>(GetX());
    element.lY = static_cast<std::int32_t>(GetY());
    packet.bHeader = 5;
    packet.wSize = sizeof(packet) + sizeof(element);
    buffer.write(&packet, sizeof(packet));
    buffer.write(&element, sizeof(element));
    const int bytes = buffer.size();
    const void* data = buffer.read_peek();
    PacketAround(data, bytes, 0);
}

// 0812d3f0 / 0812d420
bool CHARACTER::CannotMoveByAffect() { return IsAffectFlag(6); }
bool CHARACTER::CanMove() { return !CannotMoveByAffect() && !m_pkMyShop; }

// 0812ffc0
void CHARACTER::mining(CHARACTER* vein)
{
    if (m_pkMiningEvent)
    {
        sys_log(0, "XXX EVENT EXIST - MINING CANCEL");
        mining_cancel();
        return;
    }

    sys_log(0, "XXX MINING CHECK");
    if (!vein || !::mining::GetRawOreFromLoad(vein->GetRaceNum()))
        return;
    CItem* pick = GetWear(4);
    if (!pick || !::mining::Pick_Check(*pick))
    {
        ChatPacket(1, locale_find("\260\356\261\252\300\314\270\246\040\300\345\302\370\307\317\274\274\277\344\056", g_iUseLocale));
        return;
    }

    sys_log(0, "XXX MINING START");
    const int count = number_ex(5, 15, const_cast<char*>("char.cpp"), 0xec3);
    const TPacketGCDigMotion motion = {0x86,
        static_cast<std::uint32_t>(static_cast<DWORD>(GetVID())),
        static_cast<std::uint32_t>(static_cast<DWORD>(vein->GetVID())), static_cast<BYTE>(count)};
    PacketAround(&motion, sizeof(motion), nullptr);
    m_pkMiningEvent = ::mining::CreateMiningEvent(this, vein, count);
}

void CHARACTER::mining_take()
{
    m_pkMiningEvent = nullptr;
}

CGuild* CHARACTER::GetRefineGuild()
{
    CHARACTER* npc = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
    return npc ? npc->GetGuild() : nullptr;
}

int CHARACTER::ComputeRefineFee(int cost, int multiplier)
{
    CGuild* guild = GetRefineGuild();
    if (!guild)
        return cost;
    const auto subtotal = static_cast<std::uint32_t>(cost) * static_cast<std::uint32_t>(multiplier);
    if (guild == GetGuild())
        return static_cast<std::int32_t>(subtotal * 9) / 10;
    CHARACTER* npc = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
    return static_cast<std::int32_t>(npc && npc->GetEmpire() != GetEmpire() ? subtotal * 3 : subtotal);
}

void CHARACTER::PayRefineFee(int total)
{
    const int share = total / 10;
    CGuild* guild = GetRefineGuild();
    if (guild && guild != GetGuild())
    {
        total -= share;
        guild->RequestDepositMoney(this, share);
    }
    PointChange(POINT_GOLD, static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(total)), false, false);
}

bool CHARACTER::CanDeposit()
{
    const auto pulse = static_cast<std::uint32_t>(thecore_pulse());
    dev_log("char.cpp", 0x1977, "CanDeposit", 0x800, const_cast<char*>("(%u,%u)"),
        static_cast<unsigned int>(m_deposit_pulse), static_cast<unsigned int>(pulse));
    return !m_deposit_pulse || m_deposit_pulse < static_cast<std::uint32_t>(thecore_pulse());
}

void CHARACTER::UpdateDepositPulse()
{
    m_deposit_pulse = static_cast<std::uint32_t>(thecore_pulse()) +
        static_cast<std::uint32_t>(passes_per_sec) * 300;
}

void CHARACTER::mining_cancel()
{
    if (m_pkMiningEvent) {
        sys_log(0, "XXX MINING CANCEL");
        event_cancel(&m_pkMiningEvent);
        ChatPacket(1, locale_find("\303\244\261\244\300\273\040\301\337\264\334\307\317\277\264\275\300\264\317\264\331\056", g_iUseLocale));
    }
}

// 08130430
void CHARACTER::SetNextStatePulse(int nextPulse)
{
    CHARACTER_MANAGER::instance().AddToStateList(this);
    m_dwNextStatePulse = static_cast<unsigned int>(nextPulse);
    if (nextPulse < 10)
        MonsterLog("\264\331\300\275\273\363\305\302\267\316\276\356\274\255\260\241\300\332");
}

// 08131c70
bool CHARACTER::Move(long x, long y)
{
    const auto destX = static_cast<std::int32_t>(x);
    const auto destY = static_cast<std::int32_t>(y);
    if (static_cast<std::int32_t>(GetX()) == destX && static_cast<std::int32_t>(GetY()) == destY)
        return true;
    if (test_server && m_bDetailLog)
        sys_log(0, "%s position %lu %lu", GetName(), static_cast<unsigned long>(static_cast<std::uint32_t>(destX)),
                static_cast<unsigned long>(static_cast<std::uint32_t>(destY)));
    OnMove(false);
    return Sync(destX, destY);
}

void CHARACTER::StopStaminaConsume()
{
    if (!m_bStaminaConsume)
        return;
    PointChange(9, 0, false, false);
    m_bStaminaConsume = false;
    ChatPacket(5, "StopStaminaConsume %d", GetStamina());
}

void CHARACTER::SetSpeedHackMultipler(float factor)
{
    m_fSpeedHackMultipler = std::max(0.0f, std::min(factor, 1.0f));
}

void CHARACTER::OnMove(bool isAttack)
{
    const DWORD now = get_dword_time();
    m_dwLastMoveTime = now;
    if (isAttack) {
        m_dwLastAttackTime = now;
        if (IsAffectFlag(28))
            RemoveAffect(215);
        if (IsAffectFlag(19)) {
            RemoveAffect(34);
            SetAffectedEunhyung();
        } else {
            ClearAffectedEunhyung();
        }
    }
    mining_cancel();
}

// 0812bce0
BYTE CHARACTER::GetMobRank()
{
    return m_pkMobData ? m_pkMobData->m_table.bRank : 2;
}

void CHARACTER::SetRotationToXY(long x, long y)
{
    const long currentY = GetY();
    const long currentX = GetX();
    SetRotation(GetDegreeFromPositionXY(currentX, currentY, x, y));
}

// 0812d450
bool CHARACTER::Sync(long x, long y)
{
    if (!GetSectree())
        return false;
    LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), x, y);
    if (!tree) {
        if (GetDesc()) {
            _sys_err("Sync", 2435, "cannot find tree at %d %d (name: %s)",
                     static_cast<int>(x), static_cast<int>(y), GetName());
            GetDesc()->SetPhase(0);
        } else {
            const long map = GetMapIndex();
            _sys_err("Sync", 2440, "no tree: %s %d %d %d", GetName(),
                     static_cast<int>(x), static_cast<int>(y), static_cast<int>(map));
            Dead(0, false);
        }
        return false;
    }

    SetRotationToXY(x, y);
    CDungeon* dungeon = m_pkDungeon;
    SetXYZ(x, y, 0);
    if (dungeon) {
        const int previous = m_iEventAttr;
        m_iEventAttr = tree->GetEventAttribute(x, y);
        if (m_iEventAttr != previous) {
            unsigned int pid;
            if (!m_pkParty) {
                quest::CQuestManager::instance().AttrOut(m_dwPlayerID, this, previous);
                pid = m_dwPlayerID;
            } else {
                const DWORD leader = m_pkParty->GetLeaderPID();
                quest::CQuestManager::instance().AttrOut(leader, this, previous);
                pid = m_pkParty->GetLeaderPID();
            }
            quest::CQuestManager::instance().AttrIn(pid, this, m_iEventAttr);
        }
    }
    if (GetSectree() != tree) {
        if (m_bCharType == 6) {
            const SECTREEID next = tree->GetID();
            const SECTREEID old = GetSectree()->GetID();
            sys_log(0, "SECTREE DIFFER: %s %dx%d was %dx%d", GetName(),
                    static_cast<int>(next.coord.x), static_cast<int>(next.coord.y),
                    static_cast<int>(old.coord.x), static_cast<int>(old.coord.y));
        }
        tree->InsertEntity(this);
    }
    return true;
}

// 08134030, 081340d0, 08134190
void CHARACTER::QuerySafeboxSize()
{
    if (m_iSafeboxSize == -1) {
        DBManager::instance().ReturnQuery(0, GetPlayerID(), 0,
            "SELECT size FROM safebox%s WHERE account_id = %lu",
            get_table_postfix(), static_cast<unsigned long>(GetDesc()->GetAccountTable().id));
    }
}

void CHARACTER::SetSafeboxSize(int size)
{
    sys_log(0, "SetSafeboxSize: %s %d", GetName(), size);
    m_iSafeboxSize = size;
    DBManager::instance().Query("UPDATE safebox%s SET size = %d WHERE account_id = %lu",
                               get_table_postfix(), size / 9, static_cast<unsigned long>(GetDesc()->GetAccountTable().id));
}

int CHARACTER::GetSafeboxSize() { return m_iSafeboxSize; }

// 081335f0, 08133630
float CHARACTER::GetDistanceFromSafeboxOpen()
{
    const auto dx = mount_polymorph_detail::Sub(GetX(), m_posSafeboxOpen.x);
    const auto dy = mount_polymorph_detail::Sub(GetY(), m_posSafeboxOpen.y);
    return static_cast<float>(DISTANCE_APPROX(dx, dy));
}

void CHARACTER::SetSafeboxOpenPosition() { m_posSafeboxOpen = GetXYZ(); }

// 08133670
void CHARACTER::ReqSafeboxLoad(const char* password)
{
    const char* message;
    if (!*password || std::strlen(password) > 6) {
        message = "\074\303\242\260\355\076\040\300\337\270\370\265\310\040\276\317\310\243\270\246\040\300\324\267\302\307\317\274\314\275\300\264\317\264\331\056";
    } else if (m_pkSafebox) {
        message = "\074\303\242\260\355\076\040\303\242\260\355\260\241\040\300\314\271\314\040\277\255\267\301\300\326\275\300\264\317\264\331\056";
    } else if (mount_polymorph_detail::Sub(thecore_pulse(), m_iSafeboxLoadTime) <
               mount_polymorph_detail::Mul(passes_per_sec, 10)) {
        message = "\074\303\242\260\355\076\040\303\242\260\355\270\246\040\264\335\300\272\301\366\040\061\060\303\312\040\276\310\277\241\264\302\040\277\255\040\274\366\040\276\370\275\300\264\317\264\331\056";
    } else if (GetDistanceFromSafeboxOpen() <= 1000.0f) {
        if (m_bOpeningSafebox) {
            sys_log(0, "Overlapped safebox load request from %s", GetName());
            return;
        }
        SetSafeboxLoadTime();
        TAccountTable& account = GetDesc()->GetAccountTable();
        m_bOpeningSafebox = true;
        TSafeboxLoadPacket packet;
        packet.dwID = account.id;
        std::strncpy(packet.szLogin, account.login, 30);
        packet.szLogin[30] = '\0';
        std::strncpy(packet.szPassword, password, 6);
        packet.szPassword[6] = '\0';
        db_clientdesc->DBPacket(12, GetDesc()->GetHandle(), &packet, sizeof(packet));
        return;
    } else {
        message = "\074\303\242\260\355\076\040\260\305\270\256\260\241\040\270\326\276\356\274\255\040\303\242\260\355\270\246\040\277\255\040\274\366\040\276\370\275\300\264\317\264\331\056";
    }
    ChatPacket(1, locale_find(message, g_iUseLocale));
}

// 081337f0
void CHARACTER::LoadSafebox(int size, DWORD gold, int count, TPlayerItem* items)
{
    SetOpenSafebox(true);
    const bool wasLoaded = m_pkSafebox != 0;
    if (wasLoaded)
        m_pkSafebox->ChangeSize(size);
    else
        m_pkSafebox = new CSafebox(this, size, gold);
    m_iSafeboxSize = size;
    TPacketCGSafeboxSize packet;
    packet.bHeader = 88;
    packet.bSize = static_cast<BYTE>(size);
    GetDesc()->Packet(&packet, sizeof(packet));

    if (wasLoaded)
        return;
    for (int i = 0; i < count; ++i) {
        TPlayerItem& row = items[i];
        if (!m_pkSafebox->IsValidPosition(row.pos))
            continue;
        LPITEM item = ITEM_MANAGER::instance().CreateItem(row.vnum, row.count, row.id, false, -1);
        if (!item) {
            _sys_err("LoadSafebox", 0x1481, "cannot create item vnum %d id %lu (name: %s)",
                     static_cast<int>(row.vnum), row.id, GetName());
            continue;
        }
        item->SetSkipSave(true);
        long sockets[] = {row.alSockets[0], row.alSockets[1], row.alSockets[2]};
        item->SetSockets(sockets);
        item->SetAttributes(row.aAttr);
        if (m_pkSafebox->Add(row.pos, item))
            item->SetSkipSave(false);
        else
            ITEM_MANAGER::instance().DestroyItem(item);
    }
}

// 08133a50, 08133ad0
void CHARACTER::ChangeSafeboxSize(BYTE size)
{
    TPacketCGSafeboxSize packet;
    packet.bHeader = 88;
    packet.bSize = size;
    GetDesc()->Packet(&packet, sizeof(packet));
    if (m_pkSafebox)
        m_pkSafebox->ChangeSize(size);
    m_iSafeboxSize = size;
}

void CHARACTER::CloseSafebox()
{
    if (!m_pkSafebox)
        return;
    SetOpenSafebox(false);
    m_pkSafebox->Save();
    delete m_pkSafebox;
    m_pkSafebox = 0;
    ChatPacket(5, "CloseSafebox");
    SetSafeboxLoadTime();
    CancelSafeboxLoad();
    Save();
}

// 08133b80, 08133de0
void CHARACTER::LoadMall(int count, TPlayerItem* items)
{
    const bool wasLoaded = m_pkMall != 0;
    if (wasLoaded)
        m_pkMall->ChangeSize(27);
    else
        m_pkMall = new CSafebox(this, 27, 0);
    m_pkMall->SetWindowMode(4);
    TPacketCGSafeboxSize packet;
    packet.bHeader = 122;
    packet.bSize = 27;
    GetDesc()->Packet(&packet, sizeof(packet));

    if (wasLoaded)
        return;
    for (int i = 0; i < count; ++i) {
        TPlayerItem& row = items[i];
        if (!m_pkMall->IsValidPosition(row.pos))
            continue;
        LPITEM item = ITEM_MANAGER::instance().CreateItem(row.vnum, row.count, row.id, false, -1);
        if (!item) {
            _sys_err("LoadMall", 0x14e0, "cannot create item vnum %d id %lu (name: %s)",
                     static_cast<int>(row.vnum), row.id, GetName());
            continue;
        }
        item->SetSkipSave(true);
        long sockets[] = {row.alSockets[0], row.alSockets[1], row.alSockets[2]};
        item->SetSockets(sockets);
        item->SetAttributes(row.aAttr);
        if (m_pkMall->Add(row.pos, item))
            item->SetSkipSave(false);
        else
            ITEM_MANAGER::instance().DestroyItem(item);
    }
}

bool CHARACTER::RegisterMobile(const char* number)
{
    if (!GetDesc())
        return false;
    if (!number[0]) {
        m_szMobileAuth[0] = '\0';
        m_stMobile.clear();
        ChatPacket(5, "nosms");
        DBManager::instance().Query("UPDATE player%s SET mobile=NULL WHERE id=%lu",
                                    get_table_postfix(), static_cast<unsigned long>(GetPlayerID()));
        MessengerManager::instance().UpdateMobile(GetName(), m_stMobile.c_str());
        Save();
        return true;
    }
    if (g_iUseLocale != 0 || !db_clientdesc)
        return false;
    std::string digits;
    for (unsigned i = 0; i < 32 && number[i]; ++i) {
        if (std::isdigit(static_cast<unsigned char>(number[i])))
            digits += number[i];
    }
    static const char* const prefixes[] = {"\060\061\060", "\060\061\061", "\060\061\066", "\060\061\067", "\060\061\070", "\060\061\071"};
    if (std::none_of(std::begin(prefixes), std::end(prefixes), [&digits](const char* prefix) {
            return digits.compare(0, 3, prefix) == 0;
        }))
        return false;

    for (unsigned i = 0; i < 4; ++i)
        m_szMobileAuth[i] = static_cast<char>(number_ex('0', '9', const_cast<char*>("char.cpp"), 0x16a1));
    m_szMobileAuth[4] = '\0';
    m_stMobile = digits;
    ChatPacket(5, "mobile_auth");
    struct {
        char from[25], to[25], mobile[33], message[81];
    } packet{};
    static_assert(sizeof(packet) == 164);
    std::strcpy(packet.from, "YMIR");
    std::strncpy(packet.to, GetName(), sizeof(packet.to) - 1);
    std::strncpy(packet.mobile, digits.c_str(), sizeof(packet.mobile) - 1);
    std::snprintf(packet.message, 64, locale_find("\270\336\306\276\062\300\316\301\365\271\370\310\243\072\040\045\163", g_iUseLocale), m_szMobileAuth);
    db_clientdesc->DBPacket(45, 0, &packet, sizeof(packet));
    return true;
}

void CHARACTER::AuthMobile(const char* code)
{
    if (!m_szMobileAuth[0])
        return;
    const bool accepted = std::strcmp(code, m_szMobileAuth) == 0;
    m_szMobileAuth[0] = '\0';
    if (!accepted) {
        ChatPacket(1, "%s", locale_find("\300\316\301\365\271\370\310\243\260\241\040\306\262\267\310\275\300\264\317\264\331\056\040\264\331\275\303\040\275\303\265\265\307\317\274\274\277\344\056", g_iUseLocale));
        m_stMobile.clear();
        return;
    }

    DBManager::instance().Query("UPDATE player%s SET mobile='%s' WHERE id=%lu",
                                get_table_postfix(), m_stMobile.c_str(), static_cast<unsigned long>(GetPlayerID()));
    ChatPacket(5, "sms");
    ChatPacket(1, "%s", locale_find("\307\332\265\345\306\371\040\271\370\310\243\260\241\040\265\356\267\317\040\265\307\276\372\275\300\264\317\264\331\056", g_iUseLocale));
    MessengerManager::instance().UpdateMobile(GetName(), m_stMobile.c_str());
    Save();
}

void CHARACTER::ReqMallLoad(const char* password)
{
    const char* notice;
    if (!password[0] || std::strlen(password) > 6) {
        notice = "\074\303\242\260\355\076\040\300\337\270\370\265\310\040\276\317\310\243\270\246\040\300\324\267\302\307\317\274\314\275\300\264\317\264\331\056";
    } else if (m_pkMall) {
        notice = "\074\303\242\260\355\076\040\303\242\260\355\260\241\040\300\314\271\314\040\277\255\267\301\300\326\275\300\264\317\264\331\056";
    } else {
        const int pulse = thecore_pulse();
        if (pulse - GetMallLoadTime() < passes_per_sec * 10) {
            notice = "\074\303\242\260\355\076\040\303\242\260\355\270\246\040\264\335\300\272\301\366\040\061\060\303\312\040\276\310\277\241\264\302\040\277\255\040\274\366\040\276\370\275\300\264\317\264\331\056";
        } else if (GetDistanceFromSafeboxOpen() > 1000.0f) {
            notice = "\074\303\242\260\355\076\040\260\305\270\256\260\241\040\270\326\276\356\274\255\040\303\242\260\355\270\246\040\277\255\040\274\366\040\276\370\275\300\264\317\264\331\056";
        } else {
            DESC* desc = GetDesc();
            if (!desc || !db_clientdesc)
                return;
            SetMallLoadTime(pulse);
            const TAccountTable& account = desc->GetAccountTable();
            TSafeboxLoadPacket packet{};
            packet.dwID = account.id;
            std::strncpy(packet.szLogin, account.login, sizeof(packet.szLogin) - 1);
            std::strncpy(packet.szPassword, password, sizeof(packet.szPassword) - 1);
            db_clientdesc->DBPacket(107, desc->GetHandle(), &packet, sizeof(packet));
            return;
        }
    }
    ChatPacket(1, "%s", locale_find(notice, g_iUseLocale));
}

void CHARACTER::CloseMallFromClient()
{
    if (!m_pkMall)
        return;
    SetMallLoadTime(thecore_pulse());
    CloseMall();
    Save();
}

void CHARACTER::CloseMall()
{
    if (!m_pkMall)
        return;
    m_pkMall->Save();
    delete m_pkMall;
    m_pkMall = 0;
    ChatPacket(5, "CloseMall");
}

// 0812bcc0
DWORD CHARACTER::GetMonsterDrainSPPoint()
{
    return m_pkMobData ? m_pkMobData->m_table.dwDrainSP : 0;
}

// 08137050
int CHARACTER::GetHPPct()
{
    return mount_polymorph_detail::Mul(m_points.hp, 100) / m_pointsInstant.iMaxHP;
}

// 08137120
bool CHARACTER::IsDeathBlow()
{
    const int roll = number_ex(1, 100, "char.cpp", 6617);
    return roll <= m_pkMobData->m_table.bDeathBlowPoint;
}

void PCBangPlayTimeWriter_Log(const char* host, DWORD pid, int playTime);

void CHARACTER::FlushDelayedSaveItem()
{
    for (int cell=0;cell<109;++cell) {
        LPITEM item=GetItem(cell);
        if (item) ITEM_MANAGER::instance().FlushDelayedSave(item);
    }
}

void CHARACTER::Disconnect()
{
    assert(GetDesc());
    sys_log(0,"DISCONNECT: %s",GetName());
    if (m_pkShop) {
        m_pkShop->RemoveGuest(this);
        SetShop(0);
    }
    if (m_pArena) m_pArena->OnDisconnect(m_dwPlayerID);
    marriage::CManager::instance().Logout(this);

    TPacketGGLogout logout;
    logout.bHeader=2;
    std::strncpy(logout.szName,GetName(),24);
    logout.szName[24]=0;
    P2P_MANAGER::instance().Send(&logout,26,0);
    LogManager::instance().CharLog(m_dwPlayerID,GetX(),GetY(),0,"LOGOUT","");

    bool logPCBang=LC_IsYMIR() || LC_IsKorea();
    if (logPCBang || LC_IsBrazil()) {
        DWORD playTime=static_cast<DWORD>(GetRealPoint(31))-m_dwLoginPlayTime;
        LogManager::instance().LoginLog(false,GetDesc()->GetAccountTable().id,m_dwPlayerID,
                                        m_points.level,GetJob(),playTime);
        if (logPCBang) PCBangPlayTimeWriter_Log(GetDesc()->GetHostName(),m_dwPlayerID,playTime);
    }

    if (m_pWarMap) SetWarMap(0);
    if (GetWeddingMap()) SetWeddingMap(0);
    if (m_pGuild) m_pGuild->LogoutMember(this);
    if (m_pkParty) m_pkParty->Unlink(this);
    if (IsStun() || IsDead()) {
        DeathPenalty(0);
        PointChange(5,mount_polymorph_detail::Sub(50,m_points.hp),false,false);
    }
    quest::CQuestManager::instance().LogoutPC(this);
    if (!CHARACTER_MANAGER::instance().FlushDelayedSave(this)) SaveReal();
    FlushDelayedSaveItem();
    SaveAffect();
    m_bSkipSave=true;
    quest::CQuestManager::instance().DisconnectPC(this);
    CloseSafebox();
    CloseMall();
    CPVPManager::instance().Disconnect(this);
    CTargetManager::instance().Logout(m_dwPlayerID);
    MessengerManager::instance().Logout(std::string(GetName()));

    if (g_TeenDesc) {
        char packet[245]={0};
        packet[0]=17;
        DWORD account=GetDesc()->GetAccountTable().id;
        std::memcpy(packet+3,&account,4);
        g_TeenDesc->Packet(packet,7);
    }
    GetDesc()->BindCharacter(0);
    CHARACTER_MANAGER::instance().DestroyCharacter(this);
}

bool CHARACTER::ChangeSex()
{
    const WORD race = GetRaceNum();
    if (race >= 8)
    {
        _sys_err("ChangeSex", 0x5f4, "CHARACTER(%s)::ChangeSex().UNKNOWN_SRC_RACE(%d)",
            GetName(), static_cast<int>(race));
        return false;
    }
    m_points.job = static_cast<BYTE>(race ^ 4);
    sys_log(0, "CHARACTER(%s)::ChangeSex(%d -> %d)", GetName(),
        static_cast<int>(race), static_cast<int>(m_points.job));
    return true;
}

void CHARACTER::ResetPlayTime(DWORD remaining)
{
    m_dwPlayStartTime=static_cast<std::uint32_t>(get_dword_time()-remaining);
}

void CHARACTER::CreatePlayerProto(TPlayerTable& table)
{
    std::memset(&table,0,0x71a);
    const char* name=m_strNewName.empty() ? GetName() : m_strNewName.c_str();
    std::strncpy(table.name,name,24);
    table.name[24]=0;
    std::strncpy(table.ip,GetDesc()->GetHostName(),15);
    table.ip[15]=0;
    table.id=m_dwPlayerID;
    table.voice=GetPoint(2);
    table.level=m_points.level;
    table.level_step=GetPoint(25);
    table.exp=m_points.exp;
    table.gold=m_points.gold;
    table.job=m_points.job;
    table.part_base=m_pointsInstant.bBasePart;
    table.skill_group=m_points.skill_group;
    const unsigned int elapsed=get_dword_time()-m_dwPlayStartTime;
    if (elapsed>60000) {
        SECTREE* tree=GetSectree();
        if (tree && !tree->IsAttr(GetX(), GetY(), ATTR_BANPK)) {
            int multiplier=5;
            if (GetRealAlignment()<0) multiplier=IsEquipUniqueItem(0x111a2) ? 120 : 60;
            UpdateAlignment((elapsed/60000)*multiplier);
        }
        SetRealPoint(31,mount_polymorph_detail::Add(GetRealPoint(31),elapsed/60000));
        ResetPlayTime(elapsed%60000);
    }
    table.playtime=GetRealPoint(31);
    table.lAlignment=m_iRealAlignment;
    if (!m_posWarp.x && !m_posWarp.y) {
        table.x=GetX();
        table.y=GetY();
        table.z=GetZ();
        table.lMapIndex=GetMapIndex();
    } else {
        table.x=m_posWarp.x;
        table.y=m_posWarp.y;
        table.z=0;
        table.lMapIndex=m_lWarpMapIndex;
    }
    if (!m_lExitMapIndex) {
        table.lExitMapIndex=table.lMapIndex;
        table.lExitX=table.x;
        table.lExitY=table.y;
    } else {
        table.lExitMapIndex=m_lExitMapIndex;
        table.lExitX=m_posExit.x;
        table.lExitY=m_posExit.y;
    }
    sys_log(0,"SAVE: %s %dx%d",GetName(),table.x,table.y);
    table.st=GetRealPoint(12);
    table.ht=GetRealPoint(13);
    table.dx=GetRealPoint(14);
    table.iq=GetRealPoint(15);
    table.stat_point=GetPoint(26);
    table.skill_point=GetPoint(28);
    table.sub_skill_point=GetPoint(27);
    table.horse_skill_point=GetPoint(113);
    table.stat_reset_count=GetPoint(112);
    table.hp=m_points.hp;
    table.sp=m_points.sp;
    table.stamina=GetStamina();
    table.sRandomHP=GetRandomHP();
    table.sRandomSP=GetRandomSP();
    for (int i=0;i<36;++i) table.quickslot[i]=m_quickslot[i];
    if (!m_stMobile.empty() && !m_szMobileAuth[0]) {
        std::strncpy(table.szMobile,m_stMobile.c_str(),32);
        table.szMobile[32]=0;
    }
    thecore_memcpy(table.parts,m_pointsInstant.parts,8);
    thecore_memcpy(table.skills,m_pSkillLevels,0x5fa);
    table.horse=GetHorseData();
}

void CHARACTER::SaveReal()
{
    if (m_bSkipSave) return;
    if (!GetDesc()) {
        _sys_err("SaveReal",0x46c,"Character::Save : no descriptor when saving (name: %s)",GetName());
        return;
    }
    TPlayerTable table;
    CreatePlayerProto(table);
    db_clientdesc->DBPacket(4,GetDesc()->GetHandle(),&table,0x71a);
    quest::PC* pc=quest::CQuestManager::instance().GetPCForce(m_dwPlayerID);
    if (pc) pc->Save();
    else _sys_err("SaveReal",0x478,"CHARACTER::Save : null quest::PC pointer! (name %s)",GetName());

    marriage::TMarriage* record=marriage::CManager::instance().Get(m_dwPlayerID);
    if (record) record->Save();
}

// 08134810
void CHARACTER::GiveRandomSkillBook()
{
    CSkillManager& skills = CSkillManager::instance();
    bool hasSkill = false;
    for (DWORD vnum = 1; vnum <= 120; ++vnum) {
        if (skills.Get(vnum)) {
            hasSkill = true;
            break;
        }
    }
    // A partially loaded skill table may have no eligible entry for the original reroll loop.
    if (!hasSkill)
        return;
    LPITEM book = AutoGiveItem(50300, 1, -1, true);
    if (!book)
        return;

    char* const source = const_cast<char*>("char.cpp");
    DWORD jobFilter = 0;
    if (number_ex(0, 1, source, 0x15ca) == 0) {
        const BYTE ownJob = static_cast<BYTE>(GetJob() + 1);
        for (DWORD vnum = 1; vnum <= 120; ++vnum) {
            const CSkillProto* proto = skills.Get(vnum);
            if (proto && proto->bType == ownJob) {
                jobFilter = ownJob;
                break;
            }
        }
    }

    for (;;) {
        const DWORD vnum = number_ex(1, 120, source, 0x15d1);
        const CSkillProto* proto = skills.Get(vnum);
        if (proto && (!jobFilter || proto->bType == jobFilter)) {
            book->SetSocket(0, vnum);
            return;
        }
    }
}

void CHARACTER::SetMarryPartner(CHARACTER* partner)
{
    m_pkChrMarried=partner;
}

namespace {
struct RecoveryEventInfo { CHARACTER* character; };
const int aiRecoveryPercents[10]={1,5,5,5,5,5,5,5,5,5};
}

long recovery_event(LPEVENT event)
{
    using namespace mount_polymorph_detail;
    CHARACTER* ch=static_cast<RecoveryEventInfo*>(event->info)->character;
    if (!ch->GetDesc()) {
        if (!ch->IsAffectFlag(4)) {
            if (ch->m_bCharType!=4) {
                const int loggedMaxHP=ch->m_pointsInstant.iMaxHP;
                int amount=MAX(1,Mul(loggedMaxHP,ch->GetMobTable().bRegenPercent)/100);
                ch->MonsterLog("HP_REGEN +%d",amount);
                const int appliedMaxHP=ch->m_pointsInstant.iMaxHP;
                amount=MAX(1,Mul(appliedMaxHP,ch->GetMobTable().bRegenPercent)/100);
                ch->PointChange(5,amount,false,false);
            }
            if (ch->m_points.hp>=ch->m_pointsInstant.iMaxHP) {
                ch->m_pkRecoveryEvent=0;
                return 0;
            }
        }
        return Mul(MAX(1,ch->GetMobTable().bRegenCycle),passes_per_sec);
    }
    ch->CheckTarget();
    ch->UpdateSectree();
    ch->UpdateKillerMode();
    if (ch->IsAffectFlag(4)) return 3;
    const auto now = static_cast<std::uint32_t>(get_dword_time());
    const auto lastMove = static_cast<std::uint32_t>(ch->GetLastMoveTime());
    const auto periods = (now - lastMove) / 3000u;
    ch->DistributeSP(ch,0);
    if (ch->m_points.hp<ch->m_pointsInstant.iMaxHP) {
        int percent=aiRecoveryPercents[MIN(9, periods)];
        int amount=Add(Mul(percent,ch->m_pointsInstant.iMaxHP)/100,15);
        amount=Add(amount,Mul(ch->GetPoint(32),amount)/100);
        const int regen = ch->GetPoint(32);
        sys_log(1,"RECOVERY_EVENT: %s %d HP_REGEN %d HP +%d",ch->GetName(),percent,regen,amount);
        ch->PointChange(5,amount,false,false);
    }
    return Mul(passes_per_sec,3);
}

namespace {
struct SaveEventInfo {
    CHARACTER* character;
};
}

EVENTFUNC(save_event)
{
    CHARACTER* character = static_cast<SaveEventInfo*>(event->info)->character;
    sys_log(1, "SAVE_EVENT: %s", character->GetName());
    character->Save();
    character->FlushDelayedSaveItem();
    return save_event_second_cycle;
}

// 0812fca0
void CHARACTER::StartSaveEvent()
{
    if (m_pkSaveEvent)
        return;

    SaveEventInfo* info;
    const DWORD id = __AllocEventInfo(sizeof(SaveEventInfo), reinterpret_cast<void**>(&info));
    info->character = this;
    m_pkSaveEvent = event_create_ex(save_event, id, save_event_second_cycle,
                                   const_cast<char*>("char.cpp"), 0xe55);
}

// 08137210
long check_speedhack_event(LPEVENT event)
{
    CHARACTER* character = static_cast<SaveEventInfo*>(event->info)->character;
    if (!character)
        return 0;

    const int count = character->m_speed_hack_count;
    if (count > SPEEDHACK_LIMIT_COUNT) {
        LogManager::instance().SpeedHackLog(character->GetPlayerID(),
            character->GetX(), character->GetY(), count);
        if (DESC* desc = character->GetDesc()) {
            DESC_MANAGER::instance().DestroyDesc(desc);
            return 0;
        }
    }
    character->m_speed_hack_count = 0;
    return passes_per_sec * 60;
}

// 08137340
void CHARACTER::StartCheckSpeedHackEvent()
{
    if (m_pkCheckSpeedHackEvent)
        return;

    SaveEventInfo* info;
    const DWORD id = __AllocEventInfo(sizeof(SaveEventInfo), reinterpret_cast<void**>(&info));
    info->character = this;
    m_pkCheckSpeedHackEvent = event_create_ex(check_speedhack_event, id, passes_per_sec * 60,
                                             const_cast<char*>("char.cpp"), 0x1a45);
}

// 08136b30
void CHARACTER::Say(const std::string& script)
{
    static_assert(sizeof(packet_script) == 6);
    if (script.size() > 0xffff - sizeof(packet_script))
        return;

    packet_script packet{};
    packet.header = 45;
    packet.skin = 1;
    packet.src_size = static_cast<WORD>(script.size());
    packet.size = static_cast<WORD>(script.size() + sizeof(packet));
    TEMP_BUFFER buffer;
    buffer.write(&packet, sizeof(packet));
    if (!script.empty())
        buffer.write(script.data(), static_cast<int>(script.size()));
    if (DESC* desc = GetDesc())
        desc->Packet(buffer.read_peek(), buffer.size());
}

// 08134a80
void CHARACTER::SendGreetMessage()
{
    const auto messages = DBManager::instance().GetGreetMessage();
    for (const auto& message : messages)
        ChatPacket(2, "%s", message.c_str());
}

// 08134940
void CHARACTER::EffectPacket(int effect)
{
    BYTE packet[6] = {0x72, static_cast<BYTE>(effect)};
    const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(GetVID()));
    std::memcpy(packet + 2, &vid, sizeof(vid));
    PacketAround(packet, sizeof(packet));
}

void CHARACTER::ReviveInvisible(int duration)
{
    AddAffect(0xd7, 0, 0, 0x1c, duration, 0, true, false);
}

void CHARACTER::UpdateStateMachine(DWORD pulse)
{
    const auto currentPulse = static_cast<std::uint32_t>(pulse);
    if (m_dwNextStatePulse <= currentPulse && !IsDead()) {
        const VID vid = GetVID();
        CFSM::Update();
        // Horse expiry and local Santa warps can destroy the character in its state callback.
        if (CHARACTER_MANAGER::instance().Find(vid) == this)
            m_dwNextStatePulse = currentPulse + static_cast<std::uint32_t>(GetStateDuration());
    }
}

void CHARACTER::StartRecoveryEvent()
{
    if (m_pkRecoveryEvent || IsDead() || IsStun()) return;
    if (m_bCharType!=6 && m_points.hp>=m_pointsInstant.iMaxHP) return;
    void* rawInfo = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(RecoveryEventInfo), &rawInfo);
    DESC* desc=GetDesc();
    static_cast<RecoveryEventInfo*>(rawInfo)->character = this;
    int seconds=desc ? 3 : MAX(1,GetMobTable().bRegenCycle);
    long delay=mount_polymorph_detail::Mul(seconds,passes_per_sec);
    m_pkRecoveryEvent=event_create_ex(recovery_event,id,delay,const_cast<char*>("char.cpp"),0x932);
}

BOOL CHARACTER::IsGM()
{
    return m_pointsInstant.gm_level!=0 || test_server!=0;
}

void CHARACTER::CheckTarget()
{
    if (m_pkChrTarget &&
        DISTANCE_APPROX(mount_polymorph_detail::Sub(GetX(),m_pkChrTarget->GetX()),
                        mount_polymorph_detail::Sub(GetY(),m_pkChrTarget->GetY()))>4799)
        SetTarget(0);
}

// 08132360
void CHARACTER::ClearTarget()
{
    if (m_pkChrTarget) {
        m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
        m_pkChrTarget = nullptr;
    }

    TPacketGCTarget packet{};
    packet.header = 63;
    auto it = m_set_pkChrTargetedBy.begin();
    while (it != m_set_pkChrTargetedBy.end()) {
        CHARACTER* character = *it++;
        character->m_pkChrTarget = nullptr;
        DESC* desc = character->GetDesc();
        if (!desc) {
            _sys_err("ClearTarget", 0x125d, "%s %p does not have desc",
                     character->GetName(), static_cast<void*>(character));
            std::abort();
        }
        desc->Packet(&packet, sizeof(packet));
    }
    m_set_pkChrTargetedBy.clear();
}

void CHARACTER::SetTarget(CHARACTER* target)
{
    if (m_pkChrTarget==target) return;
    long map=GetMapIndex();
    if (map>=181 && map<=183 && !IsGM()) return;
    if (m_pkChrTarget) m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
    m_pkChrTarget=target;

    TPacketGCTarget packet;
    packet.header=63;
    packet.dwVID=0;
    packet.bHPPercent=0;
    if (target) {
        target->m_set_pkChrTargetedBy.insert(this);
        packet.dwVID=target->GetVID();
        int maxHP=target->m_pointsInstant.iMaxHP;
        if ((!target->GetDesc() || target->m_dwPolymorphRace) && maxHP>0) {
            WORD race=target->GetRaceNum();
            if (race>=20101 && race<=20109) {
                CHARACTER* owner=target->GetVictim();
                if (owner) {
                    int health=owner->GetHorseHealth();
                    int maximum=owner->GetHorseMaxHealth();
                    packet.bHPPercent=maximum ? MINMAX(0,health*100/maximum,100) : 100;
                } else packet.bHPPercent=100;
            } else packet.bHPPercent=MINMAX(0,mount_polymorph_detail::Mul(target->m_points.hp,100)/maxHP,100);
        }
    }
    GetDesc()->Packet(&packet, sizeof(packet));
}

void CHARACTER::SetWarMap(CWarMap* map)
{
    if (CWarMap* previous = m_pWarMap)
        previous->DecMember(this);
    m_pWarMap = map;
    if (map)
        map->IncMember(this);
}

void CHARACTER::ResetChangeAttackPositionTime()
{
    m_dwLastChangeAttackPositionTime = static_cast<std::uint32_t>(get_dword_time() - 10000UL);
}

void CHARACTER::SetChangeAttackPositionTime()
{
    m_dwLastChangeAttackPositionTime = get_dword_time();
}

void CHARACTER::SetWeddingMap(marriage::WeddingMap* map)
{
    if (marriage::WeddingMap* previous = GetWeddingMap()) previous->DecMember(this);
    m_pWeddingMap=map;
    if (map) map->IncMember(this);
}

void CHARACTER::InvalidateRegen(LPREGEN record)
{
    const auto range = regenCharacters.equal_range(record);
    for (auto entry = range.first; entry != range.second; ++entry)
        entry->second->m_pkRegen = nullptr;
    regenCharacters.erase(range.first, range.second);
}

void CHARACTER::SetRegen(LPREGEN record)
{
    if (m_pkRegen != record) {
        if (record)
            regenCharacters.emplace(record, this);
        if (m_pkRegen)
            UntrackRegenCharacter(m_pkRegen, this);
        m_pkRegen = record;
    }
    m_fRegenAngle=m_pointsInstant.fRot;
    m_posRegen=GetXYZ();
}

void CHARACTER::SetStone(CHARACTER* stone)
{
    m_pkChrStone=stone;
    if (stone && stone->m_set_pkChrSpawnedBy.find(this)==stone->m_set_pkChrSpawnedBy.end())
        stone->m_set_pkChrSpawnedBy.insert(this);
}

void CHARACTER::SetDungeon(CDungeon* dungeon)
{
    if (dungeon && m_pkDungeon)
        _sys_err("SetDungeon", 0x1130, "%s is trying to reassigning dungeon (current %p, new party %p)",
                 GetName(), static_cast<void*>(m_pkDungeon), static_cast<void*>(dungeon));

    if (m_pkDungeon) {
        if (GetDesc()) {
            if (m_pkParty)
                m_pkDungeon->DecPartyMember(m_pkParty, this);
            else
                m_pkDungeon->DecMember(this);
        } else if (IsMonster() || IsStone()) {
            m_pkDungeon->DecMonster();
        }
    }

    m_pkDungeon = dungeon;
    if (dungeon) {
        sys_log(0, "%s DUNGEON set to %p, PARTY is %p", GetName(),
                static_cast<void*>(dungeon), static_cast<void*>(m_pkParty));
        if (GetDesc()) {
            if (m_pkParty)
                m_pkDungeon->IncPartyMember(m_pkParty, this);
            else
                m_pkDungeon->IncMember(this);
        } else if (IsMonster() || IsStone()) {
            m_pkDungeon->IncMonster();
        }
    }
}

// 08131810
static bool __party_can_join_by_level(CHARACTER* leader, CHARACTER* guest)
{
    const int difference = static_cast<int>(leader->GetLevel()) - guest->GetLevel();
    const int limit = LC_IsCanada() ? 15 : 30;
    return difference >= -limit && difference <= limit;
}

// 08131860
PartyJoinError CHARACTER::IsPartyJoinableMutableCondition(CHARACTER* leader, CHARACTER* guest)
{
    if (!CPartyManager::instance().IsEnablePCParty())
        return PartyJoinError::Server;
    if (leader->GetDungeon())
        return PartyJoinError::Dungeon;
    if (guest->IsObserverMode())
        return PartyJoinError::Observer;
    if (!__party_can_join_by_level(leader, guest))
        return PartyJoinError::LevelBoundary;
    if (guest->GetParty())
        return PartyJoinError::AlreadyJoined;
    if (CParty* party = leader->GetParty()) {
        if (LC_IsCIBN()) {
            if (guest->GetLevel() < party->GetMemberMaxLevel() - 15)
                return PartyJoinError::LowLevel;
            if (guest->GetLevel() > party->GetMemberMinLevel() + 15)
                return PartyJoinError::HighLevel;
        } else if (party->GetMemberCount() == 8) {
            return PartyJoinError::Full;
        }
    }
    return PartyJoinError::None;
}

// 081317e0
PartyJoinError CHARACTER::IsPartyJoinableCondition(CHARACTER* leader, CHARACTER* guest)
{
    return leader->GetEmpire() == guest->GetEmpire()
        ? IsPartyJoinableMutableCondition(leader, guest) : PartyJoinError::Empire;
}

namespace {
struct TPartyJoinEventInfo {
    DWORD guestPid;
    DWORD leaderPid;
};

long party_invite_event(LPEVENT event)
{
    const auto* info = static_cast<TPartyJoinEventInfo*>(event->info);
    if (CHARACTER* leader = CHARACTER_MANAGER::instance().FindByPID(info->leaderPid)) {
        sys_log(0, "PartyInviteEvent %s", leader->GetName());
        leader->PartyInviteDeny(info->guestPid);
    }
    return 0;
}

const char* PartyJoinMessage(PartyJoinError error, bool accepting)
{
    switch (error) {
    case PartyJoinError::Server: return "<\306\304\306\274> \274\255\271\366 \271\256\301\246\267\316 \306\304\306\274 \260\374\267\303 \303\263\270\256\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331.";
    case PartyJoinError::Dungeon: return accepting ? "<\306\304\306\274> \264\370\300\374 \276\310\277\241\274\255\264\302 \306\304\306\274 \303\312\264\353\277\241 \300\300\307\322 \274\366 \276\370\275\300\264\317\264\331." : "<\306\304\306\274> \264\370\300\374 \276\310\277\241\274\255\264\302 \306\304\306\274 \303\312\264\353\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331.";
    case PartyJoinError::Observer: return "<\306\304\306\274> \260\374\300\374 \270\360\265\345\277\241\274\261 \306\304\306\274 \303\312\264\353\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331.";
    case PartyJoinError::LevelBoundary: return "<\306\304\306\274> -30 ~ +30 \267\271\272\247 \300\314\263\273\300\307 \273\363\264\353\271\346\270\270 \303\312\264\353\307\322 \274\366 \300\326\275\300\264\317\264\331.";
    case PartyJoinError::LowLevel: return "<\306\304\306\274> \306\304\306\274\263\273 \303\326\260\355 \267\271\272\247 \272\270\264\331 30\267\271\272\247\300\314 \263\267\276\306 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    case PartyJoinError::HighLevel: return "<\306\304\306\274> \306\304\306\274\263\273 \303\326\300\372 \267\271\272\247 \272\270\264\331 30\267\271\272\247\300\314 \263\364\276\306 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    case PartyJoinError::AlreadyJoined: return accepting ? "<\306\304\306\274> \306\304\306\274 \303\312\264\353\277\241 \300\300\307\322 \274\366 \276\370\275\300\264\317\264\331." : "<\306\304\306\274> \300\314\271\314 %s\264\324\300\272 \306\304\306\274\277\241 \274\323\307\330 \300\326\275\300\264\317\264\331.";
    case PartyJoinError::Full: return accepting ? "<\306\304\306\274> \306\304\306\274\300\307 \300\316\277\370\301\246\307\321\300\314 \303\312\260\372\307\317\277\251 \306\304\306\274\277\241 \302\374\260\241\307\322 \274\366 \276\370\275\300\264\317\264\331." : "<\306\304\306\274> \264\365 \300\314\273\363 \306\304\306\274\277\370\300\273 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.";
    case PartyJoinError::Empire: return "<\306\304\306\274> \264\331\270\245 \301\246\261\271\260\372 \306\304\306\274\270\246 \300\314\267\352 \274\366 \276\370\275\300\264\317\264\331.";
    default: return nullptr;
    }
}
} // namespace

namespace
{
const char* PartyRequestMessage(PartyJoinError error)
{
    switch (error)
    {
        case PartyJoinError::Server: return "<\306\304\306\274> \274\255\271\366 \271\256\301\246\267\316 \306\304\306\274 \260\374\267\303 \303\263\270\256\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331.";
        case PartyJoinError::Dungeon: return "<\306\304\306\274> \264\370\300\374 \276\310\277\241\274\255\264\302 \306\304\306\274 \303\312\264\353\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331.";
        case PartyJoinError::Observer: return "<\306\304\306\274> \260\374\300\374 \270\360\265\345\277\241\274\261 \306\304\306\274 \303\312\264\353\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331.";
        case PartyJoinError::LevelBoundary: return "<\306\304\306\274> -30 ~ +30 \267\271\272\247 \300\314\263\273\300\307 \273\363\264\353\271\346\270\270 \303\312\264\353\307\322 \274\366 \300\326\275\300\264\317\264\331.";
        case PartyJoinError::LowLevel: return "<\306\304\306\274> \306\304\306\274\263\273 \303\326\260\355 \267\271\272\247 \272\270\264\331 30\267\271\272\247\300\314 \263\267\276\306 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.";
        case PartyJoinError::HighLevel: return "<\306\304\306\274> \306\304\306\274\263\273 \303\326\300\372 \267\271\272\247 \272\270\264\331 30\267\271\272\247\300\314 \263\364\276\306 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.";
        case PartyJoinError::Full: return "<\306\304\306\274> \264\365 \300\314\273\363 \306\304\306\274\277\370\300\273 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.";
        case PartyJoinError::Empire: return "<\306\304\306\274> \264\331\270\245 \301\246\261\271\260\372 \306\304\306\274\270\246 \300\314\267\352 \274\366 \276\370\275\300\264\317\264\331.";
        default: return nullptr;
    }
}

bool MatchesPartyRequest(LPEVENT event, DWORD guest, DWORD leader)
{
    if (!event)
        return false;
    const auto* info = static_cast<const TPartyJoinEventInfo*>(event_info(event));
    return info && info->guestPid == guest && info->leaderPid == leader;
}
}

long party_request_event(LPEVENT event)
{
    const auto* info = static_cast<const TPartyJoinEventInfo*>(event->info);
    CHARACTER* guest = CHARACTER_MANAGER::instance().FindByPID(info->guestPid);
    if (guest && guest->m_pkPartyRequestEvent == event)
    {
        sys_log(0, "PartyRequestEvent %s", guest->GetName());
        guest->ChatPacket(5, "PartyRequestDenied");
        guest->SetPartyRequestEvent(nullptr);
    }
    return 0;
}

bool CHARACTER::RequestToParty(CHARACTER* leader)
{
    if (leader->GetParty())
        leader = leader->GetParty()->GetLeaderCharacter();
    if (!leader)
    {
        ChatPacket(1, "%s", locale_find("\306\304\306\274\300\345\300\314 \301\242\274\323 \273\363\305\302\260\241 \276\306\264\317\266\363\274\255 \277\344\303\273\300\273 \307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return false;
    }
    if (leader == this || m_pkPartyRequestEvent || !GetDesc() || !leader->GetDesc() || leader->IsBlockMode(32))
        return false;

    const PartyJoinError error = IsPartyJoinableCondition(leader, this);
    if (error != PartyJoinError::None)
    {
        if (error == PartyJoinError::LowLevel)
            sys_log(0, "PartyMemberMaxLevelLimit %d %d", GetLevel(), leader->GetParty()->GetMemberMaxLevel());
        else if (error == PartyJoinError::HighLevel)
            sys_log(0, "PartyMemberMinLevelLimit %d %d", GetLevel(), leader->GetParty()->GetMemberMinLevel());
        if (const char* message = PartyRequestMessage(error))
            ChatPacket(1, "%s", locale_find(message, g_iUseLocale));
        else if (error != PartyJoinError::AlreadyJoined)
            _sys_err("RequestToParty", 0x1017, "Do not process party join error(%d)", static_cast<int>(error));
        return false;
    }

    void* data;
    const DWORD id = __AllocEventInfo(sizeof(TPartyJoinEventInfo), &data);
    auto* info = static_cast<TPartyJoinEventInfo*>(data);
    info->guestPid = GetPlayerID();
    info->leaderPid = leader->GetPlayerID();
    SetPartyRequestEvent(event_create_ex(party_request_event, id, passes_per_sec * 10,
                                          const_cast<char*>("char.cpp"), 0x101f));
    if (!m_pkPartyRequestEvent)
        return false;
    leader->ChatPacket(5, "PartyRequest %lu", static_cast<unsigned long>(static_cast<DWORD>(GetVID())));
    ChatPacket(1, locale_find("%s \264\324\277\241\260\324 \306\304\306\274\260\241\300\324 \275\305\303\273\300\273 \307\337\275\300\264\317\264\331.", g_iUseLocale), leader->GetName());
    return true;
}

void CHARACTER::AcceptToParty(CHARACTER* member)
{
    sys_log(0, "AcceptToParty %s member %s %p", GetName(), member->GetName(),
            static_cast<void*>(member->m_pkPartyRequestEvent));
    if (!MatchesPartyRequest(member->m_pkPartyRequestEvent, member->GetPlayerID(), GetPlayerID()))
        return;
    event_cancel(&member->m_pkPartyRequestEvent);

    const char* notice;
    if (!m_pkParty)
        notice = "\273\363\264\353\271\346\300\314 \306\304\306\274\277\241 \274\323\307\330\300\326\301\366 \276\312\275\300\264\317\264\331.";
    else
    {
        if (m_pkParty->GetLeaderPID() != GetPlayerID())
            return;
        const PartyJoinError error = IsPartyJoinableCondition(this, member);
        if (error == PartyJoinError::None)
        {
            member->PartyJoin(this);
            return;
        }
        if (error == PartyJoinError::Full)
        {
            ChatPacket(1, "%s", locale_find("<\306\304\306\274> \264\365 \300\314\273\363 \306\304\306\274\277\370\300\273 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
            notice = "<\306\304\306\274> \306\304\306\274\300\307 \300\316\277\370\301\246\307\321\300\314 \303\312\260\372\307\317\277\251 \306\304\306\274\277\241 \302\374\260\241\307\322 \274\366 \276\370\275\300\264\317\264\331.";
        }
        else if (error == PartyJoinError::Empire)
        {
            _sys_err("AcceptToParty", 0x1066, "Do not process party join error(%d)", static_cast<int>(error));
            notice = nullptr;
        }
        else
            notice = PartyRequestMessage(error);
    }
    if (notice)
        member->ChatPacket(1, "%s", locale_find(notice, g_iUseLocale));
    member->ChatPacket(5, "PartyRequestDenied");
}

void CHARACTER::DenyToParty(CHARACTER* member)
{
    sys_log(0, "DenyToParty %s member %s %p", GetName(), member->GetName(),
            static_cast<void*>(member->m_pkPartyRequestEvent));
    if (MatchesPartyRequest(member->m_pkPartyRequestEvent, member->GetPlayerID(), GetPlayerID()))
    {
        event_cancel(&member->m_pkPartyRequestEvent);
        member->ChatPacket(5, "PartyRequestDenied");
    }
}

// 081310e0
void CHARACTER::PartyInvite(CHARACTER* guest)
{
    if (m_pkParty && m_pkParty->GetLeaderPID() != GetPlayerID()) {
        ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \306\304\306\274\277\370\300\273 \303\312\264\353\307\322 \274\366 \300\326\264\302 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331."));
        return;
    }
    if (guest->IsBlockMode(2)) {
        ChatPacket(1, LC_TEXT("<\306\304\306\274> %s \264\324\300\314 \306\304\306\274 \260\305\272\316 \273\363\305\302\300\324\264\317\264\331."), guest->GetName());
        return;
    }
    const PartyJoinError error = IsPartyJoinableCondition(this, guest);
    if (error != PartyJoinError::None) {
        const char* message = PartyJoinMessage(error, false);
        if (!message) {
            _sys_err("PartyInvite", 0x1095, "Do not process party join error(%d)", static_cast<int>(error));
        } else if (error == PartyJoinError::AlreadyJoined) {
            ChatPacket(1, LC_TEXT(message), guest->GetName());
        } else {
            ChatPacket(1, "%s", LC_TEXT(message));
        }
        return;
    }

    const DWORD guestPid = guest->GetPlayerID();
    if (m_PartyInviteEventMap.find(guestPid) != m_PartyInviteEventMap.end())
        return;

    TPartyJoinEventInfo* info;
    const DWORD infoId = __AllocEventInfo(sizeof(TPartyJoinEventInfo), reinterpret_cast<void**>(&info));
    info->guestPid = guestPid;
    info->leaderPid = GetPlayerID();
    m_PartyInviteEventMap.emplace(guestPid, event_create_ex(party_invite_event, infoId, passes_per_sec * 10,
                                                           const_cast<char*>("char.cpp"), 0x10a4));
    BYTE packet[5] = {0x4d};
    const auto leaderVid = static_cast<std::uint32_t>(static_cast<DWORD>(GetVID()));
    std::memcpy(packet + 1, &leaderVid, sizeof(leaderVid));
    guest->GetDesc()->Packet(packet, sizeof(packet));
}

// 08131340
void CHARACTER::PartyInviteAccept(CHARACTER* guest)
{
    const auto invite = m_PartyInviteEventMap.find(guest->GetPlayerID());
    if (invite == m_PartyInviteEventMap.end()) {
        sys_log(0, "PartyInviteAccept from not invited character(%s)", guest->GetName());
        return;
    }
    event_cancel(&invite->second);
    m_PartyInviteEventMap.erase(invite);

    if (m_pkParty && m_pkParty->GetLeaderPID() != GetPlayerID()) {
        ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \306\304\306\274\277\370\300\273 \303\312\264\353\307\322 \274\366 \300\326\264\302 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331."));
        return;
    }
    const PartyJoinError error = IsPartyJoinableMutableCondition(this, guest);
    if (error != PartyJoinError::None) {
        if (error == PartyJoinError::Full)
            ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \264\365 \300\314\273\363 \306\304\306\274\277\370\300\273 \303\312\264\353\307\322 \274\366 \276\370\275\300\264\317\264\331."));
        if (const char* message = PartyJoinMessage(error, true))
            guest->ChatPacket(1, "%s", LC_TEXT(message));
        else
            _sys_err("PartyInviteAccept", 0x10cf, "ignore party join error(%d)", static_cast<int>(error));
        return;
    }

    if (m_pkParty) {
        guest->PartyJoin(this);
    } else {
        CParty* party = CPartyManager::instance().CreateParty(this);
        party->Join(guest->GetPlayerID());
        party->Link(guest);
        party->SendPartyInfoAllToOne(this);
    }
}

// 081315e0
void CHARACTER::PartyInviteDeny(DWORD guestPid)
{
    const auto invite = m_PartyInviteEventMap.find(guestPid);
    if (invite == m_PartyInviteEventMap.end()) {
        sys_log(0, "PartyInviteDeny to not exist event(inviter PID: %d, invitee PID: %d)",
                static_cast<int>(GetPlayerID()), static_cast<int>(guestPid));
        return;
    }
    event_cancel(&invite->second);
    m_PartyInviteEventMap.erase(invite);
    if (CHARACTER* guest = CHARACTER_MANAGER::instance().FindByPID(guestPid))
        ChatPacket(1, LC_TEXT("<\306\304\306\274> %s\264\324\300\314 \306\304\306\274 \303\312\264\353\270\246 \260\305\300\375\307\317\274\314\275\300\264\317\264\331."), guest->GetName());
}

// 08135f80
bool CHARACTER::CanSummon(int leadership)
{
    return leadership > 19 ||
           (leadership > 11 && get_dword_time() < static_cast<std::uint32_t>(m_dwLastDeadTime + 180));
}

// 08131730
void CHARACTER::PartyJoin(CHARACTER* leader)
{
    leader->ChatPacket(1, LC_TEXT("<\306\304\306\274> %s\264\324\300\314 \306\304\306\274\277\241 \302\374\260\241\307\317\274\314\275\300\264\317\264\331."), GetName());
    ChatPacket(1, LC_TEXT("<\306\304\306\274> %s\264\324\300\307 \306\304\306\274\277\241 \302\374\260\241\307\317\274\314\275\300\264\317\264\331."), leader->GetName());
    CParty* party = leader->GetParty();
    party->Join(GetPlayerID());
    party->Link(this);
}

void CHARACTER::SetParty(CParty* party)
{
    if (party == m_pkParty)
        return;
    if (party && m_pkParty)
        _sys_err("SetParty", 0xfc6, "%s is trying to reassigning party (current %p, new party %p)",
                 GetName(), static_cast<void*>(m_pkParty), static_cast<void*>(party));

    sys_log(1, "PARTY set to %p", static_cast<void*>(party));
    m_pkParty = party;
    if (GetDesc()) {
        if (party)
            m_bAddChrState |= 0x10;
        else
            m_bAddChrState &= 0xef;
        UpdatePacket();
    }
}

DWORD CHARACTER::GetMobDropItemVnum()
{
    return GetMobTable().dwDropItemVnum;
}

DWORD CHARACTER::GetPolymorphItemVnum()
{
    return m_pkMobData ? GetMobTable().dwPolymorphItemVnum : 0;
}

void CHARACTER::SetGMLevel()
{
    m_pointsInstant.gm_level = gm_get_level(GetName(), GetDesc()->GetHostName());
}

void CHARACTER::SetRace(BYTE race)
{
    if (race < 8)
        m_points.job = race;
    else
        _sys_err("SetRace", 0x613, "CHARACTER::SetRace(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
}

void CHARACTER::SetPlayerProto(const TPlayerTable* table)
{
    if (GetDesc() && GetDesc()->GetHostName()[0])
        SetGMLevel();
    else
        _sys_err("SetPlayerProto", 0x64c, "cannot get desc or hostname");

    m_bCharType = 6;
    m_dwPlayerID = table->id;
    m_iAlignment = m_iRealAlignment = table->lAlignment;
    m_points.voice = table->voice;
    m_points.skill_group = table->skill_group;
    m_pointsInstant.bBasePart = table->part_base;
    SetPart(3, table->parts[3]);
    SetRandomHP(table->sRandomHP);
    SetRandomSP(table->sRandomSP);

    TPlayerSkill* skills = new TPlayerSkill[255];
    delete[] m_pSkillLevels;
    m_pSkillLevels = skills;
    thecore_memcpy(m_pSkillLevels, table->skills, sizeof(table->skills));
    if (table->lMapIndex > 9999) {
        m_posWarp.x = table->lExitX;
        m_posWarp.y = table->lExitY;
        m_lWarpMapIndex = table->lExitMapIndex;
    }
    SetRealPoint(31, table->playtime);
    m_dwLoginPlayTime = table->playtime;
    SetRealPoint(12, table->st);
    SetRealPoint(13, table->ht);
    SetRealPoint(14, table->dx);
    SetRealPoint(15, table->iq);
    SetPoint(12, table->st);
    SetPoint(13, table->ht);
    SetPoint(14, table->dx);
    SetPoint(15, table->iq);
    SetPoint(26, table->stat_point);
    SetPoint(28, table->skill_point);
    SetPoint(27, table->sub_skill_point);
    SetPoint(113, table->horse_skill_point);
    SetPoint(112, table->stat_reset_count);
    SetPoint(25, table->level_step);
    SetRealPoint(25, table->level_step);
    SetRace(static_cast<BYTE>(table->job));
    SetLevel(table->level);
    SetExp(table->exp);
    SetGold(table->gold);
    SetMapIndex(table->lMapIndex);
    SetXYZ(table->x, table->y, table->z);
    ComputePoints();
    m_points.hp = table->hp;
    SetSP(table->sp);
    SetStamina(table->stamina);
    if (!test_server && GetGMLevel() > 1) {
        m_afAffectFlag.bits[0] |= 1;
        m_bPKMode = 3;
    }
    if (m_points.level < PK_PROTECT_LEVEL)
        m_bPKMode = 3;
    m_stMobile.assign(table->szMobile, std::find(table->szMobile, table->szMobile + sizeof(table->szMobile), '\0'));
    SetHorseData(table->horse);
    if (GetHorseLevel())
        UpdateHorseDataByLogoff(table->logoff_interval);
    thecore_memcpy(m_aiPremiumTimes, table->aiPremiumTimes, sizeof(m_aiPremiumTimes));
    if (const BYTE level = GetGMLevel()) {
        _sys_err("SetPlayerProto", 0x6b1, "GM_LOGIN(gmlevel=%d, name=%s(%d), pos=(%d, %d)", level,
                 GetName(), static_cast<int>(m_dwPlayerID), static_cast<int>(GetX()), static_cast<int>(GetY()));
        LogManager::instance().CharLog(m_dwPlayerID, GetX(), GetY(), level, "GM_LOGIN", "");
    }
    sys_log(0, "%s PREMIUM %d %d, LOGGOFF_INTERVAL %lu", table->name,
            m_aiPremiumTimes[0], m_aiPremiumTimes[1], static_cast<unsigned long>(table->logoff_interval));
}

struct BGMInfo {
    std::string name;
    float vol;
};

namespace {
std::map<unsigned, BGMInfo> gs_bgmInfoMap;
bool gs_bgmVolEnable = false;
}

void CHARACTER_SetBGMVolumeEnable()
{
    gs_bgmVolEnable = true;
    sys_log(0, "bgm_info.set_bgm_volume_enable");
}

void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float vol)
{
    gs_bgmInfoMap[mapIndex] = BGMInfo{name, vol};
    sys_log(0, "bgm_info.add_info(%d, '%s', %f)", static_cast<int>(mapIndex), name, vol);
}

const BGMInfo& CHARACTER_GetBGMInfo(unsigned mapIndex)
{
    static const BGMInfo empty = {"", 0.0f};
    const auto found = gs_bgmInfoMap.find(mapIndex);
    return found == gs_bgmInfoMap.end() ? empty : found->second;
}

bool CHARACTER_IsBGMVolumeEnable()
{
    return gs_bgmVolEnable;
}

void CHARACTER::MainCharacterPacket()
{
    const auto mapIndex = static_cast<unsigned>(GetMapIndex());
    const BGMInfo& bgm = CHARACTER_GetBGMInfo(mapIndex);

    if (bgm.name.empty()) {
        sys_log(0, "bgm_info.play(%d, DEFAULT_BGM_NAME)", static_cast<int>(mapIndex));
        const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(m_vid));
        const WORD race = GetRaceNum();
        const auto x = static_cast<std::int32_t>(GetX());
        const auto y = static_cast<std::int32_t>(GetY());
        const auto z = static_cast<std::int32_t>(GetZ());
        const BYTE empire = GetDesc()->GetEmpire();
        const BYTE skillGroup = m_points.skill_group;
        TPacketGCMainCharacter packet = {0x71, vid, race, {}, x, y, z, empire, skillGroup};
        std::strncpy(packet.szName, GetName(), sizeof(packet.szName) - 1);
        GetDesc()->Packet(&packet, sizeof(packet));
        if (!m_stMobile.empty())
            ChatPacket(5, "sms");
    } else if (CHARACTER_IsBGMVolumeEnable()) {
        sys_log(0, "bgm_info.play_bgm_vol(%d, name='%s', vol=%f)",
                static_cast<int>(mapIndex), bgm.name.c_str(), bgm.vol);
        const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(m_vid));
        const WORD race = GetRaceNum();
        const auto x = static_cast<std::int32_t>(GetX());
        const auto y = static_cast<std::int32_t>(GetY());
        const auto z = static_cast<std::int32_t>(GetZ());
        const BYTE empire = GetDesc()->GetEmpire();
        const BYTE skillGroup = m_points.skill_group;
        TPacketGCMainCharacter4_BGM_VOL packet = {0x8a, vid, race, {}, {}, bgm.vol, x, y, z, empire, skillGroup};
        std::strncpy(packet.szChrName, GetName(), sizeof(packet.szChrName) - 1);
        std::strncpy(packet.szBGMName, bgm.name.c_str(), sizeof(packet.szBGMName) - 1);
        sys_log(0, "bgm_info.packet_size[%d]", static_cast<int>(sizeof(packet)));
        GetDesc()->Packet(&packet, sizeof(packet));
    } else {
        sys_log(0, "bgm_info.play(%d, '%s')", static_cast<int>(mapIndex), bgm.name.c_str());
        const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(m_vid));
        const WORD race = GetRaceNum();
        const auto x = static_cast<std::int32_t>(GetX());
        const auto y = static_cast<std::int32_t>(GetY());
        const auto z = static_cast<std::int32_t>(GetZ());
        const BYTE empire = GetDesc()->GetEmpire();
        const BYTE skillGroup = m_points.skill_group;
        TPacketGCMainCharacter3_BGM packet = {0x89, vid, race, {}, {}, x, y, z, empire, skillGroup};
        std::strncpy(packet.szChrName, GetName(), sizeof(packet.szChrName) - 1);
        std::strncpy(packet.szBGMName, bgm.name.c_str(), sizeof(packet.szBGMName) - 1);
        GetDesc()->Packet(&packet, sizeof(packet));
    }
}

void CHARACTER::PointsPacket()
{
    DESC* desc = GetDesc();
    if (!desc)
        return;

    TPacketGCPoints packet = {};
    packet.header = 0x10;
    packet.points[1] = m_points.level;
    packet.points[3] = m_points.exp;
    packet.points[4] = m_points.level < 99 ? exp_table[m_points.level] : 2000000000;
    packet.points[5] = m_points.hp;
    packet.points[6] = m_pointsInstant.iMaxHP;
    packet.points[7] = m_points.sp;
    packet.points[8] = m_pointsInstant.iMaxSP;
    packet.points[9] = GetStamina();
    packet.points[10] = GetMaxStamina();
    packet.points[11] = m_points.gold;
    for (int i = 12; i < POINT_MAX_NUM; ++i)
        packet.points[i] = GetPoint(static_cast<BYTE>(i));
    desc->Packet(&packet, sizeof(packet));
}

// 08135e60
void CHARACTER::SendEquipment(CHARACTER* viewer)
{
    DESC* desc = viewer->GetDesc();
    if (!desc)
        return;
    constexpr unsigned slots = 19;
    constexpr unsigned entrySize = 38;
    BYTE packet[5 + slots * entrySize] = {99};
    static_assert(sizeof(packet) == 0x2d7, "2009 equipment view packet");
    const std::uint32_t vid = static_cast<DWORD>(GetVID());
    std::memcpy(packet + 1, &vid, sizeof(vid));
    for (BYTE slot = 0; slot < slots; ++slot)
    {
        CItem* item = GetWear(slot);
        if (!item)
            continue;
        BYTE* entry = packet + 5 + slot * entrySize;
        const std::uint32_t vnum = item->GetVnum();
        std::memcpy(entry, &vnum, sizeof(vnum));
        entry[4] = static_cast<BYTE>(item->GetCount());
        for (unsigned socket = 0; socket < 3; ++socket)
        {
            const std::int32_t value = item->GetSocket(socket);
            std::memcpy(entry + 5 + socket * 4, &value, sizeof(value));
        }
        for (unsigned index = 0; index < 7; ++index)
        {
            const auto& attr = item->GetAttribute(index);
            BYTE* attribute = entry + 17 + index * 3;
            attribute[0] = attr.bType;
            const std::int16_t value = attr.sValue;
            std::memcpy(attribute + 1, &value, sizeof(value));
        }
    }
    desc->Packet(packet, sizeof(packet));
}

void CHARACTER::SetGuild(CGuild* guild)
{
    if (m_pGuild != guild) {
        m_pGuild = guild;
        UpdatePacket();
    }
}

class PCBang_CPlayTimeWriter {
public:
    bool Log(const char* ip, DWORD pid, int playTime);

    std::map<std::uint32_t, DWORD> ips;
    time_t lastRequest = 0;
    time_t updateCycle = 10;
    time_t minPlayTime = 10;
};

namespace {
PCBang_CPlayTimeWriter gs_kPCBangPlayTimeWriter;

bool ParsePCBangIP(const char* text, std::uint32_t& key)
{
    unsigned octets[4];
    char trailing;
    if (!text || std::sscanf(text, "%u.%u.%u.%u%c", &octets[0], &octets[1],
                             &octets[2], &octets[3], &trailing) != 4)
        return false;
    key = 0;
    for (unsigned octet : octets) {
        if (octet > 255)
            return false;
        key = (key << 8) | octet;
    }
    return true;
}
}

bool PCBangPlayTimeWriter_RequestUpdateIPList(const char* who, bool force)
{
    if (!LC_IsYMIR() && !LC_IsKorea())
        return false;
    const time_t now = std::time(0);
    if (!force && gs_kPCBangPlayTimeWriter.lastRequest + gs_kPCBangPlayTimeWriter.updateCycle > now)
        return false;
    gs_kPCBangPlayTimeWriter.lastRequest = now;
    sys_log(0, "pcbang_playtime_writer.request_update_ip_list(who=%s)", who);
    DBManager::instance().ReturnQuery(11, 0, 0, "show table status");
    return true;
}

std::string PCBangPlayTimeWriter_GetInfo()
{
    char info[256];
    ::snprintf(info, sizeof(info) - 1, "len(ip_list) = %d, update_cycle = %d, min_play_time = %d",
               static_cast<int>(gs_kPCBangPlayTimeWriter.ips.size()), static_cast<int>(gs_kPCBangPlayTimeWriter.updateCycle),
               static_cast<int>(gs_kPCBangPlayTimeWriter.minPlayTime));
    return info;
}

bool PCBangPlayTimeWriter_SetEnvironmentTime(const char* who, const std::string& type, time_t value)
{
    if (type == "update_cycle")
        gs_kPCBangPlayTimeWriter.updateCycle = value;
    else if (type == "min_play_time")
        gs_kPCBangPlayTimeWriter.minPlayTime = value;
    else
        return false;
    sys_log(0, "pcbang_playtime_writer.set_env_time(who=%s, type=%s, value=%d)",
            who, type.c_str(), static_cast<int>(value));
    return true;
}

void PCBangPlayTimeWriter_ClearIPList()
{
    gs_kPCBangPlayTimeWriter.ips.clear();
    sys_log(0, "pcbang_playtime_writer.clear_ip_list");
}

void PCBangPlayTimeWriter_InsertIP(const char* id, const char* ip)
{
    std::uint32_t key;
    if (!id || !ParsePCBangIP(ip, key))
        return;
    gs_kPCBangPlayTimeWriter.ips.insert(std::make_pair(key, static_cast<DWORD>(std::atoi(id))));
    sys_log(0, "pcbang_playtime_writer.insert_ip(id=%s, ip=%s)", id, ip);
}

bool IsPCBangIP(const char* ip)
{
    std::uint32_t key;
    return ParsePCBangIP(ip, key) && gs_kPCBangPlayTimeWriter.ips.find(key) != gs_kPCBangPlayTimeWriter.ips.end();
}

bool PCBang_CPlayTimeWriter::Log(const char* ip, DWORD pid, int playTime)
{
    if (playTime < minPlayTime) {
        if (test_server)
            sys_log(0, "pcbang_playtime_writer.log(pcbang_id=??, ip=%s, pid=%d, playtime=%d) - TIMELESS",
                    ip, static_cast<int>(pid), playTime);
        return false;
    }

    std::uint32_t key;
    const auto it = ParsePCBangIP(ip, key) ? ips.find(key) : ips.end();
    if (it == ips.end()) {
        if (test_server)
            sys_log(0, "pcbang_playtime_writer.log(pcbang_id=??, ip=%s, pid=%d, playtime=%d) - NOT_EXIST_IP",
                    ip, static_cast<int>(pid), playTime);
        return false;
    }
    const DWORD cafeId = it->second;
    sys_log(0, "pcbang_playtime_writer.log(pcbang_id=%d, ip=%s, pid=%d, playtime=%d)",
            static_cast<int>(cafeId), ip, static_cast<int>(pid), playTime);
    LogManager::instance().PCBangLoginLog(cafeId, ip, pid, static_cast<DWORD>(playTime));
    return true;
}

void PCBangPlayTimeWriter_Log(const char* ip, DWORD pid, int playTime)
{
    gs_kPCBangPlayTimeWriter.Log(ip, pid, playTime);
}

LPCHARACTER CHARACTER::GetMarryPartner()
{
    return m_pkChrMarried;
}

int CHARACTER::GetMarriageBonus(DWORD itemVnum, bool sum)
{
    if (IsNPC())
        return 0;
    marriage::TMarriage* record = marriage::CManager::instance().Get(GetPlayerID());
    return record ? record->GetBonus(itemVnum, sum, this) : 0;
}

namespace {
struct FFindReviver
{
    LPCHARACTER reviver;
    bool hasReviver;

    void operator()(LPCHARACTER member)
    {
        if (!member || !member->IsMonster() || !member->IsReviver() ||
            member == reviver || member->IsDead())
            return;

        char source[] = "char.cpp";
        const int roll = number_ex(1, 100, source, 0x19f4);
        if (roll <= member->GetMobTable().bRevivePoint) {
            hasReviver = true;
            reviver = member;
        }
    }
};
}

// 08137170
bool CHARACTER::HasReviverInParty()
{
    CParty* party = GetParty();
    if (!party || party->GetMemberCount() == 1)
        return false;
    return party->ForEachMemberPtr(FFindReviver{}).hasReviver;
}

bool CHARACTER::IsRevive()
{
    return m_pkMobInst && m_pkMobInst->m_IsRevive;
}

void CHARACTER::SetRevive(bool mode)
{
    if (m_pkMobInst)
        m_pkMobInst->m_IsRevive = mode;
}

// 08130950: a different owner may take over after three seconds.
bool CHARACTER::IsSyncOwner(LPCHARACTER owner)
{
    return m_pkChrSyncOwner == owner || get_float_time_extended() - m_fSyncTime >= 3.0L;
}

// 08130640: link the owner and victim, then broadcast the original ownership packet.
bool CHARACTER::SetSyncOwner(LPCHARACTER owner, bool removeFromList)
{
    if (GetAIFlag() & 2)
        return false;
    if (owner == this) {
        _sys_err("SetSyncOwner", 0xf61, "SetSyncOwner owner == this (%p)", static_cast<void*>(this));
        return false;
    }

    if (owner) {
        if (!IsSyncOwner(owner))
            return false;
        const int dx = static_cast<int>(static_cast<std::uint32_t>(GetX()) -
                                        static_cast<std::uint32_t>(owner->GetX()));
        const int dy = static_cast<int>(static_cast<std::uint32_t>(GetY()) -
                                        static_cast<std::uint32_t>(owner->GetY()));
        if (DISTANCE_APPROX(dx, dy) > 250) {
            sys_log(1, "SetSyncOwner distance over than 250 %s %s", GetName(), owner->GetName());
            return m_pkChrSyncOwner == owner;
        }
        if (m_pkChrSyncOwner != owner) {
            if (m_pkChrSyncOwner) {
                sys_log(1, "SyncRelease %s %p from %s", GetName(),
                        static_cast<void*>(this), m_pkChrSyncOwner->GetName());
                m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
            }
            m_pkChrSyncOwner = owner;
            owner->m_kLst_pkChrSyncOwned.push_back(this);
            sys_log(1, "SetSyncOwner set %s %p to %s", GetName(),
                    static_cast<void*>(this), owner->GetName());
        }
        m_fSyncTime = get_float_time();
    } else {
        if (m_pkChrSyncOwner) {
            if (removeFromList)
                m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
            sys_log(1, "SyncRelease %s %p from %s", GetName(),
                    static_cast<void*>(this), m_pkChrSyncOwner->GetName());
        }
        m_pkChrSyncOwner = 0;
    }

    const TPacketGCOwnership packet = {
        62,
        owner ? static_cast<std::uint32_t>(static_cast<DWORD>(owner->GetVID())) : 0U,
        static_cast<std::uint32_t>(static_cast<DWORD>(GetVID())),
    };
    PacketAround(&packet, sizeof(packet));
    return true;
}

// 081308e0: release our owner, then release the characters owned by us.
namespace {
struct FuncClearSync
{
    void operator()(LPCHARACTER character)
    {
        assert(character);
        character->SetSyncOwner(nullptr, false);
    }
};
}

void CHARACTER::ClearSync()
{
    SetSyncOwner(nullptr, true);
    FuncClearSync clear;
    for (LPCHARACTER character : m_kLst_pkChrSyncOwned)
        clear(character);
    m_kLst_pkChrSyncOwned.clear();
}

// 081322a0: kill stone-spawned characters and unlink from our parent stone.
namespace {
struct FuncDeadSpawnedByStone
{
    void operator()(LPCHARACTER child)
    {
        child->Dead(nullptr, false);
        child->SetStone(nullptr);
    }
};
}

void CHARACTER::ClearStone()
{
    FuncDeadSpawnedByStone cleanup;
    while (!m_set_pkChrSpawnedBy.empty()) {
        LPCHARACTER child = *m_set_pkChrSpawnedBy.begin();
        cleanup(child);
        m_set_pkChrSpawnedBy.erase(child);
    }
    if (m_pkChrStone) {
        m_pkChrStone->m_set_pkChrSpawnedBy.erase(this);
        m_pkChrStone = 0;
    }
}

void CHARACTER::OpenMyShop(const char* sign, TShopItemTable* table, BYTE count)
{
    if (m_pkMyShop) {
        CloseMyShop();
        return;
    }
    quest::PC* pc = quest::CQuestManager::instance().GetPCForce(GetPlayerID());
    if (pc && pc->IsRunning()) {
        sys_log(0, "<FAIL> OpenMyShop() %s's quest state is running\n", GetName());
        return;
    }
    if (!count || count > SHOP_HOST_ITEM_MAX_NUM || count > SHOP_PRICELIST_MAX_NUM)
        return;

    sys_log(0, "OpenShop id %lu name %s ", static_cast<unsigned long>(GetPlayerID()), GetName());
    std::int64_t total = GetGold();
    for (unsigned i = 0; i < count; ++i)
        total += table[i].price;
    if (total < 0 || total >= 2000000000) {
        _sys_err("OpenMyShop", 0x289, "[OVERFLOW_GOLD] Overflow ( GOLD_MAX ) id %lu name %s",
                 static_cast<unsigned long>(GetPlayerID()), GetName());
        ChatPacket(1, "%s", locale_find("\062\060\276\357\040\263\311\300\273\040\303\312\260\372\307\317\277\251\040\273\363\301\241\300\273\040\277\255\274\366\260\241\040\276\370\275\300\264\317\264\331", g_iUseLocale));
        return;
    }

    char safeSign[33]{};
    std::strncpy(safeSign, sign, sizeof(safeSign) - 1);
    m_stShopSign = safeSign;
    if (m_stShopSign.empty())
        return;
    if (CBanwordManager::instance().CheckString(m_stShopSign.c_str())) {
        ChatPacket(1, "%s", locale_find("\272\361\274\323\276\356\263\252\040\300\272\276\356\260\241\040\306\367\307\324\265\310\040\273\363\301\241\040\300\314\270\247\300\270\267\316\040\273\363\301\241\300\273\040\277\255\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }

    std::map<std::uint32_t, std::uint32_t> prices;
    bool listed[256]{};
    for (unsigned i = 0; i < count; ++i) {
        const BYTE pos = table[i].pos;
        if (listed[pos]) {
            _sys_err("OpenMyShop", 0x2a6, "MYSHOP: duplicate shop item detected! (name: %s)", GetName());
            return;
        }
        listed[pos] = true;
        CItem* item = GetItem(pos);
        if (!item)
            continue;
        if (item->GetProto() && (item->GetProto()->dwAntiFlags & 0x12000)) {
            ChatPacket(1, "%s", locale_find("\300\257\267\341\310\255\040\276\306\300\314\305\333\300\272\040\260\263\300\316\273\363\301\241\277\241\274\255\040\306\307\270\305\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
            return;
        }
        if (!item->GetCount())
            return;
        prices[item->GetVnum()] = table[i].price / item->GetCount();
    }

    if (CountSpecifyItem(71049)) {
        const std::uint32_t owner = GetPlayerID();
        const BYTE priceCount = static_cast<BYTE>(prices.size());
        TEMP_BUFFER buffer;
        buffer.write(&owner, sizeof(owner));
        buffer.write(&priceCount, sizeof(priceCount));
        for (const auto& price : prices) {
            buffer.write(&price.first, sizeof(price.first));
            buffer.write(&price.second, sizeof(price.second));
        }
        if (db_clientdesc)
            db_clientdesc->DBPacket(0x6c, 0, buffer.read_peek(), buffer.size());
    } else if (CountSpecifyItem(50200)) {
        RemoveSpecifyItem(50200, 1);
    } else {
        return;
    }

    if (m_pkExchange)
        m_pkExchange->Cancel();
    TPacketGCShopSign packet{};
    packet.bHeader = 0x27;
    packet.dwVID = GetVID();
    std::memcpy(packet.szSign, safeSign, sizeof(safeSign));
    PacketAround(&packet, sizeof(packet));
    m_pkMyShop = CShopManager::instance().CreatePCShop(this, table, count);
}

void CHARACTER::CloseMyShop()
{
    if (!m_pkMyShop)
        return;

    m_stShopSign.clear();
    CShopManager::instance().DestroyPCShop(this);
    m_pkMyShop = nullptr;
    TPacketGCShopSign packet = {};
    packet.bHeader = 39;
    packet.dwVID = static_cast<std::uint32_t>(static_cast<DWORD>(GetVID()));
    PacketAround(&packet, sizeof(packet));
}

void CHARACTER::fishing()
{
    if (m_pkFishingEvent) {
        fishing_take();
        return;
    }

    LPITEM rod = GetWear(4);
    if (!rod || rod->GetType() != 13) {
        ChatPacket(1, locale_find("\263\254\275\303\264\353\270\246 \300\345\302\370 \307\317\274\274\277\344.", g_iUseLocale));
        return;
    }
    if (!static_cast<std::uint32_t>(rod->GetSocket(2))) {
        ChatPacket(1, locale_find("\271\314\263\242\270\246 \263\242\260\355 \264\370\301\256 \301\326\274\274\277\344.", g_iUseLocale));
        return;
    }

    float x;
    float y;
    GetDeltaByDegree(m_pointsInstant.fRot, 400.0f, &x, &y);
    m_pkFishingEvent = ::fishing::CreateFishingEvent(this);
}

void CHARACTER::fishing_take()
{
    LPITEM rod = GetWear(4);
    if (rod && rod->GetType() == 13) {
        if (m_pkFishingEvent) {
            auto* info = static_cast<::fishing::fishing_event_info*>(FindEventInfo(m_pkFishingEvent->dwInfoID));
            if (info)
                ::fishing::Take(info, this);
        }
    } else {
        ChatPacket(1, locale_find("\263\254\275\303\264\353\260\241 \276\306\264\321 \271\260\260\307\300\270\267\316 \263\254\275\303\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331!", g_iUseLocale));
    }
    event_cancel(&m_pkFishingEvent);
}

// 08131d10: private shop clicks and quest/NPC trigger dispatch.
void CHARACTER::OnClick(LPCHARACTER causer)
{
    if (!causer) {
        _sys_err("OnClick", 0x119c, "OnClick %s by NULL", GetName());
        return;
    }

    const DWORD vid = GetVID();
    sys_log(0, "OnClick %s[vnum %d ServerUniqueID %d,pid %d] by %s",
            GetName(), GetRaceNum(), static_cast<int>(vid), static_cast<int>(GetPlayerID()), causer->GetName());
    sys_log(0, "OnClick %s by %s", GetName(), causer->GetName());
    if (causer->m_pkMyShop && causer != this) {
        _sys_err("OnClick", 0x11a9, "OnClick Fail (%s->%s) - pc has shop", causer->GetName(), GetName());
        return;
    }

    if (GetDesc() && !CTargetManager::instance().GetTargetInfo(causer->GetPlayerID(), TARGET_TYPE_VID, static_cast<int>(vid))) {
        if (!m_pkMyShop) {
            if (test_server)
                _sys_err("OnClick", 0x11dd, "%s.OnClickFailure(%s) - target is PC", causer->GetName(), GetName());
            return;
        }

        const bool self = causer == this;
        if (causer->m_pkExchange || causer->m_isOpenSafebox || causer->m_pkChrShopOwner ||
            causer->IsCubeOpen() || (!self && causer->m_pkMyShop)) {
            causer->ChatPacket(1, locale_find("\264\331\270\245 \260\305\267\241\301\337(\303\242\260\355,\261\263\310\257,\273\363\301\241)\277\241\264\302 \260\263\300\316\273\363\301\241\300\273 \273\347\277\353\307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
            return;
        }
        if (!self && (m_pkExchange || m_isOpenSafebox || IsCubeOpen())) {
            causer->ChatPacket(1, locale_find("\273\363\264\353\271\346\300\314 \264\331\270\245 \260\305\267\241\270\246 \307\317\260\355 \300\326\264\302 \301\337\300\324\264\317\264\331.", g_iUseLocale));
            return;
        }

        if (causer->m_pkShop) {
            causer->m_pkShop->RemoveGuest(causer);
            causer->SetShop(nullptr);
        }
        m_pkMyShop->AddGuest(causer, vid, false);
        causer->m_pkChrShopOwner = this;
        return;
    }

    if (LC_IsNewCIBN() && (causer->IsOverTime(OT_3HOUR) || causer->IsOverTime(OT_5HOUR))) {
        const int hours = causer->IsOverTime(OT_3HOUR) ? 3 : 5;
        sys_log(0, "Teen OverTime : name = %s, hour = %d)", causer->GetName(), hours);
        return;
    }

    causer->SetQuestNPCID(vid);
    const bool handled = quest::CQuestManager::instance().Click(causer->GetPlayerID(), this);
    if (!handled && !GetDesc() && m_triggerOnClick.pFunc)
        m_triggerOnClick.pFunc(this, causer);
}
