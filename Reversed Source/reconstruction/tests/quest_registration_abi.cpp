#include <map>
#include <set>
#include <string>
#include <vector>
#include <ext/hash_map>
#define private public
#include "../../game/src/questmanager.h"
#undef private
void CheckQuestRegistrationAPI() {
    void (quest::CQuestManager::*load)(const std::string&,unsigned int)=&quest::CQuestManager::LoadStartQuest;
    void (quest::CQuestManager::*reg)(const std::string&,unsigned int)=&quest::CQuestManager::RegisterQuest;
    (void)load;(void)reg;
}
typedef char registration_int32[sizeof(int)==4?1:-1];
// No old complete STL class-size assertions.
