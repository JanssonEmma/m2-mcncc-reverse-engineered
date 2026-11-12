#ifndef MCNCC_SKILL_POWER_API_CHECKS_H
#define MCNCC_SKILL_POWER_API_CHECKS_H
// Exact old public API; these reject newer const-qualified signatures.
static void SkillPowerApiChecks()
{
    bool (CTableBySkill::*a)()=&CTableBySkill::Check;
    void (CTableBySkill::*b)()=&CTableBySkill::DeleteAll;
    int (CTableBySkill::*c)(int,int,int,bool)=&CTableBySkill::GetSkillPowerByLevelFromType;
    void (CTableBySkill::*d)(int,int*)=&CTableBySkill::SetSkillPowerByLevelFromType;
    void (CTableBySkill::*e)(int)=&CTableBySkill::DeleteSkillPowerByLevelFromType;
    int (CTableBySkill::*f)(int)=&CTableBySkill::GetSkillDamageByLevel;
    void (CTableBySkill::*g)(int*)=&CTableBySkill::SetSkillDamageByLevelTable;
    void (CTableBySkill::*h)()=&CTableBySkill::DeleteSkillDamageByLevelTable;
    CTableBySkill* t=0; singleton<CTableBySkill>* base=t;
    (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;(void)base;
}
#endif
