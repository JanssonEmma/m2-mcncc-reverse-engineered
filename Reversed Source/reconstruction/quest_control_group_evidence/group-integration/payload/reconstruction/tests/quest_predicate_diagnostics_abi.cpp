#include "../../game/src/questmanager.h"
void PredicateDiagnosticAPI(){bool(quest::CQuestManager::*predicate)(unsigned int)=&quest::CQuestManager::CanStartQuest;void(quest::CQuestManager::*diagnostic)()=&quest::CQuestManager::WriteRunningStateToSyserr;(void)predicate;(void)diagnostic;}
typedef char predicate_unsigned32[sizeof(unsigned int)==4?1:-1];
typedef char predicate_bool8[sizeof(bool)==1?1:-1];
