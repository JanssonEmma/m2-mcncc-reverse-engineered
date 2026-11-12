//Full declared quest objects with explicit test-only lifecycle boundaries.
//No Lua engine or gameplay dispatch is substituted in production.
#include <map>
#include <set>
#include <string>
#include <vector>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstdlib>
int max_bytes_written,current_bytes_written,total_bytes_written;
int main() {
    assert(!quest::CQuestManager::instance_ptr());
    {
        quest::CQuestManager manager;
        assert(quest::CQuestManager::instance_ptr()==&manager);
        assert(manager.GetEventFlag("missing")==0 && manager.m_mapEventFlag.empty());
        manager.m_mapEventFlag["arena_potion_limit_count"]=37;
        manager.m_mapEventFlag["threeway_war_dead_count"]=-4;
        manager.m_mapEventFlag[""]=INT_MIN;
        manager.m_mapEventFlag["maximum"]=INT_MAX;
        manager.m_mapEventFlag["zero"]=0;
        assert(manager.GetEventFlag("arena_potion_limit_count")==37);
        assert(manager.GetEventFlag("threeway_war_dead_count")==-4);
        assert(manager.GetEventFlag("")==INT_MIN && manager.GetEventFlag("maximum")==INT_MAX);
        assert(manager.GetEventFlag("zero")==0 && manager.GetEventFlag("ZERO")==0);
        assert(manager.GetEventFlag("Arena_potion_limit_count")==0 && manager.m_mapEventFlag.size()==5);
        std::string embedded("A\0B",3); manager.m_mapEventFlag[embedded]=9;
        assert(manager.GetEventFlag(embedded)==9 && manager.GetEventFlag("A")==0);
        manager.m_mapEventFlag["arena_potion_limit_count"]=41;
        assert(manager.GetEventFlag("arena_potion_limit_count")==41);
        manager.m_mapEventFlag.erase(embedded); assert(manager.GetEventFlag(embedded)==0);
    }
    assert(!quest::CQuestManager::instance_ptr());
    std::puts("quest event flags passed: exact names, no insertion, signed values and full string keys");
}
