#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
typedef char DWORD32[sizeof(DWORD)==4?1:-1];
typedef char WORD16[sizeof(WORD)==2?1:-1];
void NPCContextSignatures(){
 void(CHARACTER::*set)(DWORD)=&CHARACTER::SetQuestNPCID;
 LPCHARACTER(CHARACTER::*get)()=&CHARACTER::GetQuestNPC;
 LPCHARACTER(quest::CQuestManager::*current)()=&quest::CQuestManager::GetCurrentNPCCharacterPtr;
 unsigned int(quest::CQuestManager::*race)()=&quest::CQuestManager::GetCurrentNPCRace;
 (void)set;(void)get;(void)current;(void)race;
}
