#include "cstring"
#include <stdint.h>
// Whole original questnpc.cpp owning lifecycle; original complete40container record retained.
#include "questnpc.h"
#include "questmanager.h"
#include "locale.hpp"
#include <sstream>
#include "config.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <dirent.h>
#include <strings.h>
namespace quest {
//C2 081adf50/C1 081ae0c0: all20+20maps defaultconstruct BEFORE m_vnum store.
NPC::NPC(){m_vnum=0;}
//D2 081ae180/D1 081ae260: original source-level userbody empty; COMPLETE array/member tear follows.
// This is not a teststub: full nativeSTL destroys all20argumentmaps then20plainmaps and ownedbyvalue records.
NPC::~NPC(){}
// Original C1copy081ab460 remains compiler memberwise-copy: m_vnum +all40containers. No fabricatedcopyctor.

// Original LoadStateScript at 081ae5a0.
void NPC::LoadStateScript(int event_index,const char* filename,const char* script_name) {
    std::ifstream inf(filename);
    const std::string s(script_name);
    std::string::size_type i=s.find('.');
    CQuestManager& q=CQuestManager::instance();
    const std::string quest_name=s.substr(0,i);
    const int quest_index=static_cast<int>(q.GetQuestIndexByName(quest_name));
    if(quest_index==0) {
        std::fprintf(stderr,"cannot find quest index for %s\n",quest_name.c_str());
        assert(!"cannot find quest index");
        return;
    }
    std::string state_name;
    std::string::size_type j=i;
    i=s.find('.',i+1);
    if(i==std::string::npos) state_name=s.substr(j+1,std::string::npos);
    else state_name=s.substr(j+1,i-j-1);
    const int state_index=q.GetQuestStateIndex(quest_name,state_name);
    sys_log(0,"QUEST loading %s : %s [STATE] %s",filename,quest_name.c_str(),state_name.c_str());
    if(i==std::string::npos) {
        std::istreambuf_iterator<char> ib(inf),ie;
        // Original obtains outer quest-map before the SECOND live Lua query (081b0d6c..0ec4).
        AQuestScriptType& states=m_mapOwnQuest[event_index][quest_index];
        const int live_state=q.GetQuestStateIndex(quest_name,state_name);
        std::copy(ib,ie,std::back_inserter(states[live_state].m_code));
    } else {
        j=i;i=s.find('.',i+1);
        if(i==std::string::npos) {
            _sys_err("LoadStateScript",138,"invalid QUEST STATE index [%s] [%s]",filename,script_name);
            return;
        }
        const int index=static_cast<int>(std::strtol(s.substr(j+1,i-j-1).c_str(),0,10));
        j=i;i=s.find('.',i+1);
        if(i!=std::string::npos) {
            _sys_err("LoadStateScript",150,"invalid QUEST STATE name [%s] [%s]",filename,script_name);
            return;
        }
        const std::string type_name=s.substr(j+1,i-j-1);
        std::istreambuf_iterator<char> ib(inf),ie;
        // Original signed MAX and 32-bit INC register wrap. Do not zero AArgScript scalar defaults.
        const int requested=static_cast<int>(static_cast<unsigned int>(index)+1U);
        m_mapOwnArgQuest[event_index][quest_index][state_index].resize(
            static_cast<unsigned int>(MAX(requested,static_cast<int>(m_mapOwnArgQuest[event_index][quest_index][state_index].size()))));
        if(type_name=="when") {
            std::copy(ib,ie,std::back_inserter(m_mapOwnArgQuest[event_index][quest_index][state_index][index].when_condition));
        } else if(type_name=="arg") {
            std::string line;std::getline(inf,line);
            m_mapOwnArgQuest[event_index][quest_index][state_index][index].arg.clear();
            for(std::string::iterator it=line.begin();it!=line.end();++it)
                m_mapOwnArgQuest[event_index][quest_index][state_index][index].arg+=*it;
        } else if(type_name=="script") {
            std::copy(ib,ie,std::back_inserter(m_mapOwnArgQuest[event_index][quest_index][state_index][index].script.m_code));
            m_mapOwnArgQuest[event_index][quest_index][state_index][index].quest_index=quest_index;
            m_mapOwnArgQuest[event_index][quest_index][state_index][index].state_index=state_index;
        }
    }
}

// Original directory traversal at 081ae340.
void NPC::Set(unsigned int vnum, const std::string& script_name)
{
    m_vnum = vnum;

    char path[1024];
    CQuestManager::TEventNameMap::iterator nextEvent =
        CQuestManager::instance().m_mapEventName.begin();
    while (nextEvent != CQuestManager::instance().m_mapEventName.end()) {
        CQuestManager::TEventNameMap::iterator event = nextEvent++;
        for (std::set<std::string>::const_iterator objectDir = g_setQuestObjectDir.begin();
             objectDir != g_setQuestObjectDir.end(); ++objectDir) {
            const int prefixLength = std::sprintf(path, "%s/%s/%s/", objectDir->c_str(),
                                                  script_name.c_str(), event->first.c_str());
            const int eventIndex = event->second;
            DIR* directory = opendir(path);
            if (!directory)
                continue;

            while (dirent* entry = readdir(directory)) {
                if (entry->d_name[0] == '.' || strncasecmp(entry->d_name, "CVS", 3) == 0)
                    continue;

                sys_log(1, "QUEST reading %s", entry->d_name);
                // The original interprets the name as a format and imposes no bound here.
                std::sprintf(path + prefixLength, entry->d_name);
                sys_log(0, "Quest reading %s", path);
                LoadStateScript(eventIndex, path, entry->d_name);
            }
            closedir(directory);
        }
    }
}
}

namespace quest {
bool NPC::OnLevelUp(PC& pc) { return HandleReceiveAllEvent(pc,3); }
bool NPC::OnKill(PC& pc)
{
    return m_vnum ? HandleEvent(pc, 1) : HandleReceiveAllEvent(pc, 1);
}

bool NPC::OnPartyKill(PC& pc)
{
    return m_vnum ? HandleEvent(pc, 18) : HandleReceiveAllEvent(pc, 18);
}
struct FuncMissHandleReceiveAllEvent {
    bool bHandled;
    FuncMissHandleReceiveAllEvent():bHandled(false) {}
    void operator()(PC::QuestInfoIterator&,NPC::QuestMapType::iterator& it) {
        unsigned int index=it->first;
        if (it->second.find(0)==it->second.end() ||
            !CQuestManager::instance().CanStartQuest(index)) return;
        NPC::AQuestScriptType::const_iterator script=it->second.find(0);
        if (script!=it->second.end()) {
            bHandled=true;
            CQuestManager::ExecuteQuestScript(*CQuestManager::instance().GetCurrentPC(),
                index,0,script->second.GetCode(),script->second.GetSize(),0,true);
        }
    }
};
struct FuncMatchHandleReceiveAllEvent {
    bool bHandled;
    FuncMatchHandleReceiveAllEvent():bHandled(false) {}
    void operator()(PC::QuestInfoIterator& pc,NPC::QuestMapType::iterator& it) {
        int state=pc->second.st;
        NPC::AQuestScriptType::const_iterator script=it->second.find(state);
        if (script!=it->second.end()) {
            bHandled=true;
            CQuestManager::ExecuteQuestScript(*CQuestManager::instance().GetCurrentPC(),
                it->first,state,script->second.GetCode(),script->second.GetSize(),0,true);
        }
    }
};
template<class MAP,class MATCH,class MISS>
void NPC::MatchingQuest(PC& pc,MAP& quests,MATCH& match,MISS& miss) {
    if (test_server) sys_log(0,"Click Quest : MatchingQuest");
    PC::QuestInfoIterator ip=pc.quest_begin();
    typename MAP::iterator iq=quests.begin();
    while (iq!=quests.end()) {
        if (ip==pc.quest_end() || iq->first<ip->first) {
            miss(ip,iq);
            ++iq;
        } else if (iq->first==ip->first) {
            match(ip,iq);
            ++ip;
            ++iq;
        } else ++ip;
    }
}
bool IsScriptTrue(const char* code, int size);

namespace {
void AppendChatScripts(NPC::AArgQuestScriptType& states, int state,
                       std::vector<AArgScript*>& scripts)
{
    if (states.find(state) == states.end())
        return;

    for (std::size_t index = 0; index < states[state].size(); ++index) {
        const auto& condition = states[state][index].when_condition;
        if (IsScriptTrue(condition.data(), static_cast<int>(condition.size()))) {
            auto current = states.find(state);
            if (current != states.end() && index < current->second.size())
                scripts.push_back(&current->second[index]);
        }
    }
}

struct FuncMatchChatEvent
{
    std::vector<AArgScript*>& rAvailScript;

    void operator()(PC::QuestInfoIterator& player, NPC::ArgQuestMapType::iterator& quest)
    {
        const int state = player->second.st;
        AppendChatScripts(quest->second, state, rAvailScript);
    }
};

struct FuncMissChatEvent
{
    std::vector<AArgScript*>& rAvailScript;

    void operator()(PC::QuestInfoIterator&, NPC::ArgQuestMapType::iterator& quest)
    {
        if (CQuestManager::instance().CanStartQuest(quest->first))
            AppendChatScripts(quest->second, 0, rAvailScript);
    }
};
}

bool NPC::OnClick(PC& pc)
{
    return HandleEvent(pc, 0);
}

bool NPC::OnTakeItem(PC& pc)
{
    return HandleEvent(pc, 16);
}

bool NPC::OnUseItem(PC& pc, bool receiveAll)
{
    return receiveAll ? HandleReceiveAllEvent(pc, 11) : HandleEvent(pc, 11);
}

bool NPC::HasChat()
{
    return !m_mapOwnArgQuest[8].empty();
}

bool NPC::OnChat(PC& pc)
{
    if (pc.IsRunning()) {
        _sys_err("OnChat", 0x32e, "QUEST There's suspended quest state, can't run new quest state.");
        return false;
    }

    std::vector<AArgScript*> scripts;
    FuncMatchChatEvent match{scripts};
    FuncMissChatEvent miss{scripts};
    MatchingQuest(pc, m_mapOwnArgQuest[8], match, miss);
    if (scripts.empty())
        return false;

    std::ostringstream menu;
    menu << "select(\"" << ScriptToString(scripts.front()->arg.c_str()) << '"';
    for (size_t index = 1; index < scripts.size(); ++index)
        menu << ",\"" << ScriptToString(scripts[index]->arg.c_str()) << '"';
    menu << ", '" << locale_find("\264\335\261\342", g_iUseLocale) << "')";
    const std::string code = menu.str();
    CQuestManager::ExecuteQuestScript(pc, "QUEST_CHAT_TEMP_QUEST", 0,
                                      code.c_str(), code.size(), &scripts, false);
    return true;
}

bool NPC::HandleReceiveAllEvent(PC& pc,int event) {
    if (static_cast<unsigned int>(event)>=20) {
        _sys_err("HandleReceiveAllEvent",0x225,"QUEST invalid EventIndex : %d",event);
        return false;
    }
    if (pc.IsRunning()) {
        _sys_err("HandleReceiveAllEvent",0x22b,
            "QUEST There's suspended quest state, can't run new quest state.");
        return false;
    }
    FuncMissHandleReceiveAllEvent miss;
    FuncMatchHandleReceiveAllEvent match;
    MatchingQuest(pc,m_mapOwnQuest[event],match,miss);
    return miss.bHandled || match.bHandled;
}
//081b1a00. Last valid slot of original20-event NPC.
bool NPC::OnUnmount(PC& pc) { return HandleReceiveAllEvent(pc,19); }
}

namespace quest {
// 081b12e0, 081b1320 and 081b1360
bool NPC::OnButton(PC& player, unsigned index)
{
    if (player.IsRunning()) {
        if (test_server)
            _sys_err("OnButton", 0x2d4, "QUEST There's suspended quest state, can't run new quest state.");
        return false;
    }
    const auto quest = m_mapOwnQuest[6].find(index);
    if (quest == m_mapOwnQuest[6].end())
        return false;

    const auto current = player.quest_find(index);
    int state = 0;
    if (current != player.quest_end())
        state = current->second.st;
    else if (!CQuestManager::instance().CanStartQuest(index))
        return false;
    const auto script = quest->second.find(state);
    if (script == quest->second.end())
        return false;
    CQuestManager::ExecuteQuestScript(player, index, state, script->second.GetCode(), script->second.GetSize());
    return true;
}

bool NPC::OnInfo(PC& player, unsigned index)
{
    if (player.IsRunning()) {
        if (test_server)
            _sys_err("OnInfo", 0x2ab, "QUEST There's suspended quest state, can't run new quest state.");
        return false;
    }
    const auto current = player.quest_find(index);
    if (current == player.quest_end()) {
        _sys_err("OnInfo", 0x2b3, "QUEST no quest by (quest %u)", index);
        return false;
    }
    const auto quest = m_mapOwnQuest[7].find(index);
    const auto name = CQuestManager::instance().GetQuestNameByIndex(index);
    if (quest == m_mapOwnQuest[7].end()) {
        _sys_err("OnInfo", 0x2be, "QUEST no info event (quest %s)", name.c_str());
        return false;
    }
    const int state = current->second.st;
    const auto script = quest->second.find(state);
    if (script == quest->second.end()) {
        _sys_err("OnInfo", 0x2c6, "QUEST no info script by state %d (quest %s)", state, name.c_str());
        return false;
    }
    CQuestManager::ExecuteQuestScript(player, index, state, script->second.GetCode(), script->second.GetSize());
    return true;
}

bool NPC::OnEnterState(PC& pc, DWORD index, int state) { return ExecuteEventScript(pc, 13, index, state); }
bool NPC::OnLeaveState(PC& pc, DWORD index, int state) { return ExecuteEventScript(pc, 14, index, state); }
bool NPC::OnLetter(PC& pc, DWORD index, int state) { return ExecuteEventScript(pc, 15, index, state); }

// 081b2a10
bool NPC::ExecuteEventScript(PC& pc, int event, DWORD index, int state)
{
    QuestMapType& quests = m_mapOwnQuest[event];
    QuestMapType::iterator quest = quests.find(index);
    if (quest == quests.end()) {
        sys_log(0, "ExecuteEventScript ei %d qi %lu is %d - NO QUEST", event, index, state);
        return false;
    }
    AQuestScriptType::iterator script = quest->second.find(state);
    if (script == quest->second.end()) {
        sys_log(0, "ExecuteEventScript ei %d qi %lu is %d - NO STATE", event, index, state);
        return false;
    }
    sys_log(0, "ExecuteEventScript ei %d qi %lu is %d", event, index, state);
    CQuestManager::instance().SetCurrentEventIndex(event);
    return CQuestManager::ExecuteQuestScript(pc, index, state, script->second.GetCode(), script->second.GetSize());
}
}

namespace quest {
// 081b1970, 081b19d0, 081b1870, 081b1840
bool NPC::OnLogin(PC& pc, const char* quest)
{
    (void)quest;
    const bool handled = HandleReceiveAllNoWaitEvent(pc, 4);
    HandleReceiveAllEvent(pc, 15);
    return handled;
}

bool NPC::OnLogout(PC& pc) { return HandleReceiveAllEvent(pc, 5); }
bool NPC::OnTimer(PC& pc) { return HandleEvent(pc, 2); }
bool NPC::OnServerTimer(PC& pc) { return HandleReceiveAllEvent(pc, 12); }

namespace {
struct FuncMatchHandleEvent {
    bool matched;
    DWORD index;
    int state;
    const char* code;
    int size;

    FuncMatchHandleEvent() : matched(false) {}

    void operator()(PC::QuestInfoIterator& pc, NPC::QuestMapType::iterator& quest)
    {
        if (matched)
            return;
        NPC::AQuestScriptType::iterator script = quest->second.find(pc->second.st);
        if (script != quest->second.end()) {
            matched = true;
            index = quest->first;
            state = pc->second.st;
            code = script->second.GetCode();
            size = script->second.GetSize();
        }
    }
};

struct FuncMissHandleEvent {
    DWORD index;

    FuncMissHandleEvent() : index(0) {}

    void operator()(PC::QuestInfoIterator&, NPC::QuestMapType::iterator& quest)
    {
        if (quest->second.find(0) != quest->second.end() &&
            CQuestManager::instance().CanStartQuest(quest->first))
            index = quest->first;
    }
};

void ExecuteNoWaitScript(DWORD index, int state, const AStateScriptType& script,
                         NPC::QuestMapType::iterator& quest, int line)
{
    CQuestManager& manager = CQuestManager::instance();
    PC* pc = manager.GetCurrentPC();
    if (CQuestManager::ExecuteQuestScript(*pc, index, state, script.GetCode(), script.GetSize())) {
        _sys_err("operator()", line, "QUEST NOT END RUNNING on Login/Logout - %s",
                 manager.GetQuestNameByIndex(quest->first).c_str());
        manager.CloseState(*pc->GetRunningQuestState());
        pc->EndRunning();
    }
}

struct FuncMatchHandleReceiveAllNoWaitEvent {
    bool handled;

    FuncMatchHandleReceiveAllNoWaitEvent() : handled(false) {}

    void operator()(PC::QuestInfoIterator& pc, NPC::QuestMapType::iterator& quest)
    {
        const int state = pc->second.st;
        NPC::AQuestScriptType::const_iterator script = quest->second.find(state);
        if (script != quest->second.end()) {
            handled = true;
            ExecuteNoWaitScript(quest->first, state, script->second, quest, 0x281);
        }
    }
};

struct FuncMissHandleReceiveAllNoWaitEvent {
    bool handled;

    FuncMissHandleReceiveAllNoWaitEvent() : handled(false) {}

    void operator()(PC::QuestInfoIterator&, NPC::QuestMapType::iterator& quest)
    {
        const DWORD index = quest->first;
        if (quest->second.find(0) == quest->second.end() ||
            !CQuestManager::instance().CanStartQuest(index))
            return;
        NPC::AQuestScriptType::const_iterator script = quest->second.find(0);
        if (script != quest->second.end()) {
            handled = true;
            ExecuteNoWaitScript(index, 0, script->second, quest, 0x25b);
        }
    }
};
}

// 081b1a30. The first running-state match wins. The fallback is the last eligible start script.
bool NPC::HandleEvent(PC& pc, int event)
{
    if (test_server)
        sys_log(0, "HandleEvent  EventIndex %d ", event);
    if (static_cast<unsigned int>(event) >= 20) {
        _sys_err("HandleEvent", 0x1c7, "QUEST invalid EventIndex : %d", event);
        return false;
    }
    if (pc.IsRunning()) {
        _sys_err("HandleEvent", 0x1cd, "QUEST There's suspended quest state, can't run new quest state. %d",
                 static_cast<int>(reinterpret_cast<uintptr_t>(pc.GetRunningQuestState())));
        return false;
    }
    QuestMapType& quests = m_mapOwnQuest[event];
    FuncMatchHandleEvent match;
    FuncMissHandleEvent miss;
    MatchingQuest(pc, quests, match, miss);
    if (match.matched) {
        CQuestManager::ExecuteQuestScript(pc, match.index, match.state, match.code, match.size);
        return true;
    }
    if (!miss.index)
        return false;
    AStateScriptType& script = quests[miss.index][0];
    CQuestManager::ExecuteQuestScript(pc, miss.index, 0, script.GetCode(), script.GetSize());
    return true;
}

// 081b1eb0
bool NPC::HandleReceiveAllNoWaitEvent(PC& pc, int event)
{
    if (static_cast<unsigned int>(event) >= 20) {
        _sys_err("HandleReceiveAllNoWaitEvent", 0x291, "QUEST invalid EventIndex : %d", event);
        return false;
    }
    FuncMatchHandleReceiveAllNoWaitEvent match;
    FuncMissHandleReceiveAllNoWaitEvent miss;
    MatchingQuest(pc, m_mapOwnQuest[event], match, miss);
    return match.handled || miss.handled;
}
}

namespace quest {
bool NPC::OnAttrIn(PC& pc) { return HandleEvent(pc, 9); }
bool NPC::OnAttrOut(PC& pc) { return HandleEvent(pc, 10); }
}

namespace quest {
bool IsScriptTrue(const char* code, int size);

bool NPC::OnTarget(PC& pc, DWORD index, const char* name, const char* verb, bool& result)
{
    sys_log(1,"OnTarget begin %s verb %s qi %lu",name,verb,index);
    result=false;
    PC::QuestInfoIterator state=pc.quest_find(index);
    if (state==pc.quest_end()) {
        sys_log(1,"no quest");
        return false;
    }
    const int current=state->second.st;
    AArgQuestScriptType& states=m_mapOwnArgQuest[17][index];
    AArgQuestScriptType::iterator found=states.find(current);
    if (found==states.end()) {
        sys_log(1,"no target event, state %d",current);
        return false;
    }
    const size_t length=std::strlen(name);
    std::vector<AArgScript>::iterator it=found->second.begin();
    while (it!=found->second.end()) {
        AArgScript& script=*it++;
        const char* arg=script.arg.c_str();
        sys_log(1,"OnTarget compare %s %d",arg,static_cast<int>(script.arg.size()));
        if (std::strncmp(arg,name,length)!=0) continue;
        const char* suffix=std::strchr(arg,'.');
        if (!suffix || std::strcmp(suffix+1,verb)!=0) continue;
        const char* condition=script.when_condition.data();
        const int size=static_cast<int>(script.when_condition.size());
        if (size)
            sys_log(1,"OnTarget when %.*s size %d",size,condition,size);
        else
            sys_log(1,"OnTarget when %s size %d","(null)",size);
        if (!IsScriptTrue(condition,size)) continue;
        sys_log(1,"OnTarget execute qi %lu st %d code %s",index,current,script.script.GetCode());
        CQuestManager::ExecuteQuestScript(pc,index,current,script.script.GetCode(),script.script.GetSize());
        result=true;
        return true;
    }
    return false;
}
}
