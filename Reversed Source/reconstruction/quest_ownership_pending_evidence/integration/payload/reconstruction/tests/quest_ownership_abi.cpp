// Full existing owner records; no dynamicSTL oldsize compatibility gate.
#include <map>
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/questmanager.h"
#undef protected
#undef private
void OwnershipAPI(){void(quest::CQuestManager::*destroy)()=&quest::CQuestManager::Destroy;(void)destroy;}
typedef char ownership_DWORD32[sizeof(DWORD)==4?1:-1];
typedef char ownership_plain20[sizeof(((quest::NPC*)0)->m_mapOwnQuest)/sizeof(quest::NPC::QuestMapType)==20?1:-1];
typedef char ownership_args20[sizeof(((quest::NPC*)0)->m_mapOwnArgQuest)/sizeof(quest::NPC::ArgQuestMapType)==20?1:-1];
// Exact by-value nested types, not a pointer-vector substitution or reduced owner.
void OwnershipMemberTypes(quest::NPC& npc){
 const std::map<unsigned int,std::map<int,std::vector<quest::AArgScript> > >* args=npc.m_mapOwnArgQuest;
 const std::map<unsigned int,std::map<int,quest::AStateScriptType> >* plain=npc.m_mapOwnQuest;
 unsigned int* vnum=&npc.m_vnum;
 (void)args;(void)plain;(void)vnum;
}
