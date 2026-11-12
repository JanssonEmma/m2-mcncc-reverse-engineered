#include "char.h"
#include "questmanager.h"
#include "../../common/tables.h"
#include "desc_client.h"
#include <cstring>
#include "questpc.h"
#include "questevent.h"
#include "config.h"
#include "../../libthecore/include/log.h"
namespace quest {
// PC C2 0805d580/C1 0805d790. LastState intentionally remains indeterminate.
// All complete STL members default construct; no VM/coroutine/reward behavior.
PC::PC():m_bIsGivenReward(false),m_bShouldSendDone(false),m_dwID(0),
    m_RunningQuestState(0),m_iSendToClient(0),m_bLoaded(false),
    m_dwWaitConfirmFromPID(0),m_bConfirmWait(false) {}
// D2 0805d900/D1 0805db30; standard complete member teardown follows Destroy.
PC::~PC() { Destroy(); }
// 0805dd60
void PC::Destroy() { ClearTimer(); }
// 0805dd70. Reset done BEFORE storing ID; no loaded/context reset.
void PC::SetID(DWORD id) { m_bShouldSendDone=false; m_dwID=id; }
// 0805dd90. Old API is NONCONST; returns const reference, no copy.
const std::string& PC::GetCurrentQuestName() { return m_stCurQuest; }
// 0805ef10. Every map value must be a non-null event (old precondition).
void PC::ClearTimer() {
    sys_log(0,"QUEST clear timer %d",static_cast<int>(m_TimerMap.size()));
    for(std::map<std::string,LPEVENT>::iterator it=m_TimerMap.begin();it!=m_TimerMap.end();++it)
        CancelTimerEvent(&it->second);
    m_TimerMap.clear();
}
// Complete original questpc.cpp flag and named-timer helpers.
// 0805de00: diagnostic BEFORE map inspection, zero ignores skip-save via DeleteFlag.
void PC::SetFlag(const std::string& name,int value,bool skipSave) {
    sys_log(test_server?0:1,"QUEST Setting flag %s %d",name.c_str(),value);
    if(value==0) { DeleteFlag(name);return; }
    TFlagMap::iterator it=m_FlagMap.find(name);
    if(it==m_FlagMap.end()) m_FlagMap.insert(std::make_pair(name,value));
    else if(it->second==value) skipSave=true;
    else it->second=value;
    if(!skipSave) SaveFlag(name,value);
}
// 0805dfe0: erase before staging zero; do not copy/fix aliasing name arguments.
bool PC::DeleteFlag(const std::string& name) {
    TFlagMap::iterator it=m_FlagMap.find(name);
    if(it==m_FlagMap.end()) return false;
    m_FlagMap.erase(it);
    SaveFlag(name,0);
    return true;
}
// 0805e0b0: debug ADDS a second log; reread value after each external callback.
int PC::GetFlag(const std::string& name) {
    TFlagMap::iterator it=m_FlagMap.find(name);
    if(it==m_FlagMap.end()) return 0;
    sys_log(1,"QUEST getting flag %s %d",name.c_str(),it->second);
    if(test_server) sys_log(0,"QUEST getting flag %s %d",name.c_str(),it->second);
    return it->second;
}
// 0805e180: pending map only, no DB/network/write-through side effects.
void PC::SaveFlag(const std::string& name,int value) {
    TFlagMap::iterator it=m_FlagSaveMap.find(name);
    if(it==m_FlagSaveMap.end()) m_FlagSaveMap.insert(std::make_pair(name,value));
    else if(it->second!=value) it->second=value;
}
// 0805ec00: RemoveTimer first, then INSERT (not assignment), then log.
void PC::AddTimer(const std::string& name,LPEVENT event) {
    RemoveTimer(name);
    m_TimerMap.insert(std::make_pair(name,event));
    sys_log(0,"QUEST add timer %p %d",static_cast<void*>(event),static_cast<int>(m_TimerMap.size()));
}
// 0805ed20: erase only; event and name ownership remain external/live.
void PC::RemoveTimerNotCancel(const std::string& name) {
    TTimerMap::iterator it=m_TimerMap.find(name);
    if(it!=m_TimerMap.end()) {
        sys_log(0,"QUEST remove with no cancel %p",static_cast<void*>(it->second));
        m_TimerMap.erase(it);
    }
    sys_log(1,"QUEST timer map size %d by RemoveTimerNotCancel",static_cast<int>(m_TimerMap.size()));
}
// 0805ee10: log before cancel of LIVE map value, erase afterward.
void PC::RemoveTimer(const std::string& name) {
    TTimerMap::iterator it=m_TimerMap.find(name);
    if(it!=m_TimerMap.end()) {
        sys_log(0,"QUEST remove timer %p",static_cast<void*>(it->second));
        CancelTimerEvent(&it->second);
        m_TimerMap.erase(it);
    }
    sys_log(1,"QUEST timer map size %d by RemoveTimer",static_cast<int>(m_TimerMap.size()));
}

// Original questpc.cpp 080603e0: mask OR, not bit-index shift.
void PC::SetSendFlag(int mask) { m_iSendToClient|=mask; }

}

// Complete original questpc.cpp storage helpers. Requires actual CQM::GetQuestIndexByName.
#include "questmanager.h"
namespace quest {
//08060c10: unknown names resolve0 and can match an existing0-key state; do NOT reject0.
bool PC::HasQuest(const std::string& name)
{
    unsigned int index=CQuestManager::instance().GetQuestIndexByName(name);
    return m_QuestInfo.find(index)!=m_QuestInfo.end();
}
//08060ca0: reference + original operator[] insertion, default counters remain indeterminate.
QuestState& PC::GetQuest(const std::string& name)
{
    unsigned int index=CQuestManager::instance().GetQuestIndexByName(name);
    return m_QuestInfo[index];
}
//0805dda0: return int retains original high-bit conversion.
int PC::GetCurrentQuestIndex()
{
    const std::string& name=GetCurrentQuestName();
    return static_cast<int>(CQuestManager::instance().GetQuestIndexByName(name));
}
//0805e5a0: insert/update, context, flag callback, then CURRENT running pointer/index/done.
void PC::SetQuest(const std::string& name,QuestState& state)
{
    unsigned int index=CQuestManager::instance().GetQuestIndexByName(name);
    QuestInfoIterator it=m_QuestInfo.find(index);
    if(it==m_QuestInfo.end())m_QuestInfo.insert(std::make_pair(index,state));
    else it->second=state;
    m_stCurQuest=name;
    m_RunningQuestState=&m_QuestInfo[index];
    m_iSendToClient=0;
    m_iLastState=state.st;
    {
        std::string flag=name+".__status";
        SetFlag(flag,state.st,false);
    }
    m_RunningQuestState->iIndex=static_cast<int>(index);
    m_bShouldSendDone=false;
}
//0805e400: integer overload only, QUEUE a change; never resolve Lua or directly change state/flags.
void PC::SetQuestState(const std::string& name,int next)
{
    int previous;
    { std::string flag=name+".__status";previous=GetFlag(flag); }
    if(previous!=next)AddQuestStateChange(name,previous,next);
}
//0805e4b0: query BEFORE external diagnostic, then append to CURRENT vector after callback.
void PC::AddQuestStateChange(const std::string& name,int previous,int next)
{
    DWORD index=CQuestManager::instance().GetQuestIndexByName(name);
    sys_log(0,"QUEST reserve Quest State Change quest %s[%lu] from %d to %d",name.c_str(),index,previous,next);
    m_QuestStateChange.push_back(TQuestStateChangeInfo(index,previous,next));
}
//080617f0: full original flag loop/timer clear/state reset; dangling live-key path NOT repaired.
void PC::ClearQuest(const std::string& name)
{
    std::string prefix=name+'.';
    for(TFlagMap::iterator it=m_FlagMap.begin();it!=m_FlagMap.end();) {
        TFlagMap::iterator now=it++;
        if(now->second!=0 && now->first.compare(0,prefix.size(),prefix)==0)
            SetFlag(now->first,0,false);
    }
    ClearTimer();
    QuestInfoIterator it=quest_begin();
    unsigned int index=CQuestManager::instance().GetQuestIndexByName(name);
    while(it!=quest_end()) {
        if(it->first==index){it->second.st=0;break;}
        ++it;
    }
}
}

// COMPLETE original quest-info group; staging only, no production changes/builds.
#include "questpc.h"
#include "questmanager.h"
#include "char.h"
#include "desc.h"
#include "packet.h"
#include "buffer_manager.h"
#include "../../libthecore/include/log.h"
#include <cassert>
namespace quest {
//0805efc0/0805eff0: begin getter means iIndex, NOT bStart.
void PC::SetCurrentQuestStartFlag(){if(!GetCurrentQuestBeginFlag())SetCurrentQuestBeginFlag();}
void PC::SetCurrentQuestDoneFlag(){if(GetCurrentQuestBeginFlag())ClearCurrentQuestBeginFlag();}
//08060400
void PC::ClearCurrentQuestBeginFlag(){SetSendFlag(1);m_RunningQuestState->bStart=false;}
//08060430: real query first, then CURRENT state index/begin, then sendmask.
void PC::SetCurrentQuestBeginFlag()
{
    unsigned int index=CQuestManager::instance().GetQuestIndexByName(m_stCurQuest);
    m_RunningQuestState->iIndex=static_cast<int>(index);
    m_RunningQuestState->bStart=true;
    SetSendFlag(1);
}
//080604b0, original nullsafe INDEX getter.
int PC::GetCurrentQuestBeginFlag(){return m_RunningQuestState?m_RunningQuestState->iIndex:0;}
//080604d0/08060650/08060690/080606c0/08060700/08060730.
void PC::SetCurrentQuestTitle(const std::string& title){SetSendFlag(2);m_RunningQuestState->_title=title;}
void PC::SetCurrentQuestClockName(const std::string& name){SetSendFlag(4);m_RunningQuestState->_clock_name=name;}
void PC::SetCurrentQuestClockValue(int value){SetSendFlag(8);m_RunningQuestState->_clock_value=value;}
void PC::SetCurrentQuestCounterName(const std::string& name){SetSendFlag(16);m_RunningQuestState->_counter_name=name;}
void PC::SetCurrentQuestCounterValue(int value){SetSendFlag(32);m_RunningQuestState->_counter_value=value;}
void PC::SetCurrentQuestIconFile(const std::string& file){SetSendFlag(64);m_RunningQuestState->_icon_file=file;}
//08060510, full temporary context swap/send/restore. No new quest record on miss.
void PC::SetQuestTitle(const std::string& name,const std::string& title)
{
    unsigned int index=CQuestManager::instance().GetQuestIndexByName(name);
    QuestInfoIterator it=m_QuestInfo.find(index);
    if(it==m_QuestInfo.end())return;
    QuestState* old=m_RunningQuestState;
    int oldMask=m_iSendToClient;
    std::string oldName=m_stCurQuest;
    m_stCurQuest=name;
    m_iSendToClient=2;
    m_RunningQuestState=&it->second;
    it->second.iIndex=GetCurrentQuestBeginFlag();
    SetCurrentQuestTitle(title);
    SendQuestInfoPakcet();
    m_stCurQuest=oldName;
    m_RunningQuestState=old;
    m_iSendToClient=oldMask;
}
//0805f020. Full original sender: qi.size is NOT patched back into already-written header.
void PC::SendQuestInfoPakcet()
{
    assert(m_iSendToClient);
    assert(m_RunningQuestState);
    packet_quest_info qi;
    qi.header=81;qi.index=static_cast<WORD>(m_RunningQuestState->iIndex);
    qi.size=6;qi.flag=static_cast<BYTE>(m_iSendToClient);
    TEMP_BUFFER buf;
    buf.write(&qi,6);
    if(m_iSendToClient&1){
        BYTE temp=m_RunningQuestState->bStart?1:0;
        buf.write(&temp,1);qi.size=static_cast<WORD>(qi.size+1);
        sys_log(1,"QUEST BeginFlag %d",static_cast<unsigned int>(temp));
    }
    if(m_iSendToClient&2){
        m_RunningQuestState->_title.reserve(31);
        buf.write(m_RunningQuestState->_title.c_str(),31);qi.size=static_cast<WORD>(qi.size+31);
        sys_log(1,"QUEST Title %s",m_RunningQuestState->_title.c_str());
    }
    if(m_iSendToClient&4){
        m_RunningQuestState->_clock_name.reserve(17);
        buf.write(m_RunningQuestState->_clock_name.c_str(),17);qi.size=static_cast<WORD>(qi.size+17);
        sys_log(1,"QUEST Clock Name %s",m_RunningQuestState->_clock_name.c_str());
    }
    if(m_iSendToClient&8){
        buf.write(&m_RunningQuestState->_clock_value,4);qi.size=static_cast<WORD>(qi.size+4);
        sys_log(1,"QUEST Clock Value %d",m_RunningQuestState->_clock_value);
    }
    if(m_iSendToClient&16){
        m_RunningQuestState->_counter_name.reserve(17);
        buf.write(m_RunningQuestState->_counter_name.c_str(),17);qi.size=static_cast<WORD>(qi.size+17);
        sys_log(1,"QUEST Counter Name %s",m_RunningQuestState->_counter_name.c_str());
    }
    if(m_iSendToClient&32){
        buf.write(&m_RunningQuestState->_counter_value,4);qi.size=static_cast<WORD>(qi.size+4);
        sys_log(1,"QUEST Counter Value %d",m_RunningQuestState->_counter_value);
    }
    if(m_iSendToClient&64){
        m_RunningQuestState->_icon_file.reserve(25);
        buf.write(m_RunningQuestState->_icon_file.c_str(),25);qi.size=static_cast<WORD>(qi.size+25);
        sys_log(1,"QUEST Icon File %s",m_RunningQuestState->_icon_file.c_str());
    }
    int bytes=buf.size();
    const void* data=buf.read_peek();
    CQuestManager::instance().GetCurrentCharacterPtr()->GetDesc()->Packet(data,bytes);
    m_iSendToClient=0;
}
// Original questpc.cpp PC::Save08060770; requires complete common/tables.h record.
// Add includes common/tables.h, desc_client.h and <cstring> to actual questpc.cpp.
void PC::Save() {
    if (m_FlagSaveMap.empty()) return;
    static std::vector<TQuestTable> s_table;
    s_table.resize(m_FlagSaveMap.size());
    int i=0;
    TFlagMap::iterator it=m_FlagSaveMap.begin();
    while (it!=m_FlagSaveMap.end()) {
        const std::string& stComp=it->first;
        long lValue=it->second;
        ++it;
        int iPos=static_cast<int>(stComp.find("."));
        if (iPos<0) {
            _sys_err("Save",537,"quest::PC::Save : cannot find . in FlagMap");
            continue;
        }
        std::string stName;
        stName.assign(stComp,0,iPos);
        std::string stState;
        stState.assign(stComp,iPos+1,stComp.length());
        if (stName.length()==0 || stState.length()==0) {
            _sys_err("Save",549,"quest::PC::Save : invalid quest data: %s",stComp.c_str());
            continue;
        }
        sys_log(1,"QUEST Save Flag %s, %s %ld (%d)",stName.c_str(),stState.c_str(),lValue,i);
        if (stName.length()>=32) {
            _sys_err("Save",557,"quest::PC::Save : quest name overflow");
            continue;
        }
        if (stState.length()>=64) {
            _sys_err("Save",563,"quest::PC::Save : quest state overflow");
            continue;
        }
        TQuestTable& r=s_table[i++];
        r.dwPID=m_dwID;
        std::strcpy(r.szName,stName.c_str());
        std::strcpy(r.szState,stState.c_str());
        r.lValue=lValue;
    }
    if (i>0) {
        sys_log(1,"QuestPC::Save %d",i);
        db_clientdesc->DBPacketHeader(11,0,sizeof(TQuestTable)*i);
        db_clientdesc->Packet(&s_table[0],sizeof(TQuestTable)*i);
    }
    m_FlagSaveMap.clear();
}


// Original complete PC::Build08061310. Default-state copy unsafe domain preserved.
void PC::Build() {
    TFlagMap::iterator it;
    for (it=m_FlagMap.begin();it!=m_FlagMap.end();++it) {
        if (it->first.size()>9 && it->first.compare(it->first.size()-9,9,".__status")==0) {
            DWORD dwQuestIndex=CQuestManager::instance().GetQuestIndexByName(it->first.substr(0,it->first.size()-9));
            int state=it->second;
            QuestState qs;
            qs.st=state;
            m_QuestInfo.insert(std::make_pair(dwQuestIndex,qs));
        }
    }
}
// Original complete PC::CancelRunning080603c0; no save/reward/state destruction.
void PC::CancelRunning() {
    m_RunningQuestState=0;
    m_iSendToClient=0;
    m_bShouldSendDone=false;
}

}

// Full original questpc.cpp queue producers; application execution remains in separate Reward.
namespace quest {
//08060e60: log before reading live quest/label; a nonzero flag marks duplicate, otherwise queue unchanged inputs.
void PC::GiveItem(const std::string& label,DWORD dwVnum,int count) {
    sys_log(1,"QUEST GiveItem %s %d %d",label.c_str(),dwVnum,count);
    if (!GetFlag(m_stCurQuest+"."+label))
        m_vRewardData.push_back(RewardData(RewardData::REWARD_TYPE_ITEM,dwVnum,count));
    else
        m_bIsGivenReward=true;
}
//08060fd0: original default third RewardData argument is0; no EXP grant or flag write occurs here.
void PC::GiveExp(const std::string& label,DWORD exp) {
    sys_log(1,"QUEST GiveExp %s %d",label.c_str(),exp);
    if (!GetFlag(m_stCurQuest+"."+label))
        m_vRewardData.push_back(RewardData(RewardData::REWARD_TYPE_EXP,exp));
    else
        m_bIsGivenReward=true;
}
}

// Complete original PC state-query consumers; true Lua-backed CQM methods are dependencies.
namespace quest {
//0805e2a0: query first; then use CURRENT questname for flag; no running-state write.
void PC::SetCurrentQuestStateName(const std::string& state_name) {
    int next=CQuestManager::Instance().GetQuestStateIndex(m_stCurQuest,state_name);
    SetFlag(m_stCurQuest+".__status",next,false);
}
//0805e380: complete string/string overload only; integer overload queues actual transition.
void PC::SetQuestState(const std::string& quest_name,const std::string& state_name) {
    int next=CQuestManager::Instance().GetQuestStateIndex(quest_name,state_name);
    SetQuestState(quest_name,next);
}
//08061980: full LIVE flag traversal and actual reverse Lua query for status suffixes.
void PC::SendFlagList(CHARACTER* ch) {
    for(TFlagMap::iterator it=m_FlagMap.begin();it!=m_FlagMap.end();++it) {
        if(it->first.size()>9 && it->first.compare(it->first.size()-9,9,".__status")==0) {
            std::string quest_name=it->first.substr(0,it->first.size()-9);
            const char* state_name=CQuestManager::instance().GetQuestStateName(quest_name,it->second);
            ch->ChatPacket(1,"%s %s (%d)",quest_name.c_str(),state_name,it->second);
        } else ch->ChatPacket(1,"%s %d",it->first.c_str(),it->second);
    }
}
}
