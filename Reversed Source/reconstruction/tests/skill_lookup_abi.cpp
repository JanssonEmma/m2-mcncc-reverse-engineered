#include "../../game/src/skill.h"
static CSkillProto* (CSkillManager::*get_by_id)(DWORD)=&CSkillManager::Get;
static CSkillProto* (CSkillManager::*get_by_name)(const char*)=&CSkillManager::Get;
static bool (CSkillManager::*initialize)(TSkillTable*,int)=&CSkillManager::Initialize;
typedef char DWORD_is_four[sizeof(DWORD)==4?1:-1];

