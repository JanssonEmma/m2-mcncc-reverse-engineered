#include <map>
#include <set>
#include <string>
#include <vector>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
void CheckQuestManagerIndexAPI() {
    unsigned int (quest::CQuestManager::*index)(const std::string&)=&quest::CQuestManager::GetQuestIndexByName;
    const std::string& (quest::CQuestManager::*name)(unsigned int)=&quest::CQuestManager::GetQuestNameByIndex;
    const std::string& (quest::CQuestManager::*current)()=&quest::CQuestManager::GetCurrentQuestName;
    unsigned int (quest::CQuestManager::*npc)(const std::string&)=&quest::CQuestManager::FindNPCIDByName;
    std::size_t (quest::CQuestManager::stringhash::*hash)(const std::string&) const=&quest::CQuestManager::stringhash::operator();
    (void)index;(void)name;(void)current;(void)npc;(void)hash;
}
typedef char old_uint32[sizeof(unsigned int)==4?1:-1];
// Native size_t/STL layout may differ. No whole-manager/hash_map size assertions.
