// TEST ONLY unrelated OWNERSHIP lifecycle boundary; never production recovery.
#include <map>
#define private public
#include "../../game/src/skill.h"
#undef private
#include <cassert>
CSkillManager::~CSkillManager()
{
    // Test must clear ALL borrowed opaque pointer tokens before scope exit.
    // This proves no wipe_second/prototype/CPoly destruction behavior.
    assert(m_map_pkSkillProto.empty());
    // Full real map and singleton base destruct after this narrow boundary.
}

