// Whole original questnpc.cpp owning lifecycle; original complete40container record retained.
#include "questnpc.h"
#include "questmanager.h"
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
