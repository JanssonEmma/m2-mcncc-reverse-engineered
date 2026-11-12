#define private public
#include "../../game/src/constants.h"
#undef private
#include <cstddef>
#include "skill_power_api_checks.h"
#define SP_ASSERT(n,x) typedef char sp_assert_##n[(x)?1:-1]
template<class A,class B> struct SpSame { enum {value=0}; };
template<class A> struct SpSame<A,A> { enum {value=1}; };
typedef int* SpEightPointers[8];
SP_ASSERT(int4,sizeof(int)==4);
SP_ASSERT(ptr4,sizeof(void*)==4);
SP_ASSERT(base4,sizeof(singleton<CTableBySkill>)==4);
SP_ASSERT(class40,sizeof(CTableBySkill)==40);
SP_ASSERT(power4,offsetof(CTableBySkill,m_aiSkillPowerByLevelFromType)==4);
SP_ASSERT(damage36,offsetof(CTableBySkill,m_aiSkillDamageByLevel)==36);
SP_ASSERT(eight,JOB_MAX_NUM*2==8);
SP_ASSERT(level41,SKILL_MAX_LEVEL+1==41);
SP_ASSERT(damage99,PLAYER_MAX_LEVEL==99);
static SpEightPointers CTableBySkill::* const power=&CTableBySkill::m_aiSkillPowerByLevelFromType;
static int* CTableBySkill::* const damage=&CTableBySkill::m_aiSkillDamageByLevel;
void skill_power_abi_signatures() { SkillPowerApiChecks();(void)power;(void)damage; }
