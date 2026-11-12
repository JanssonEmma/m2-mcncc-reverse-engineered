// Append to original game/src/constants.cpp, with <stdio.h> included.
// Complete old methods; mutable inputs and nonconst getters are intentional.
bool CTableBySkill::Check()
{
    for(int job=0;job<JOB_MAX_NUM*2;++job)
        if(!m_aiSkillPowerByLevelFromType[job]) {
            fprintf(stderr,"[NO SETTING SKILL] aiSkillPowerByLevelFromType[%d]",job);
            return false;
        }
    return true;
}
void CTableBySkill::DeleteAll()
{
    for(int job=0;job<JOB_MAX_NUM*2;++job)
        DeleteSkillPowerByLevelFromType(job);
    DeleteSkillDamageByLevelTable();
}
int CTableBySkill::GetSkillPowerByLevelFromType(int job,int skillgroup,int skilllevel,bool bMob)
{
    if(bMob) return m_aiSkillPowerByLevelFromType[0][skilllevel];
    if(job>3 || skillgroup==0) return 0;
    // Preserve original 32-bit LEA arithmetic even when signed inputs wrap.
    const unsigned int idx=static_cast<unsigned int>(job)*2U+static_cast<unsigned int>(skillgroup)-1U;
    return m_aiSkillPowerByLevelFromType[idx][skilllevel];
}
void CTableBySkill::SetSkillPowerByLevelFromType(int idx,int* aTable)
{
    DeleteSkillPowerByLevelFromType(idx);
    int* aiSkillTable=new int[SKILL_MAX_LEVEL+1];
    for(int i=0;i<SKILL_MAX_LEVEL+1;++i) aiSkillTable[i]=aTable[i];
    m_aiSkillPowerByLevelFromType[idx]=aiSkillTable;
    fprintf(stderr,"AddedSkill[%d][%d]\n",idx/2,idx%2);
}
void CTableBySkill::DeleteSkillPowerByLevelFromType(int idx)
{
    // Original 080e6960: JNZ skips NULL store. No delete[] or nonnull clear.
    if(m_aiSkillPowerByLevelFromType[idx]==0)
        m_aiSkillPowerByLevelFromType[idx]=0;
}
int CTableBySkill::GetSkillDamageByLevel(int Level)
{
    if(static_cast<unsigned int>(Level)<99U) return m_aiSkillDamageByLevel[Level];
    return 0;
}
void CTableBySkill::SetSkillDamageByLevelTable(int* aTable)
{
    DeleteSkillDamageByLevelTable();
    int* aiSkillDamageTable=new int[PLAYER_MAX_LEVEL];
    for(int i=0;i<PLAYER_MAX_LEVEL;++i) aiSkillDamageTable[i]=aTable[i];
    m_aiSkillDamageByLevel=aiSkillDamageTable;
}
void CTableBySkill::DeleteSkillDamageByLevelTable()
{
    if(m_aiSkillDamageByLevel!=0) {
        delete[] m_aiSkillDamageByLevel;
        m_aiSkillDamageByLevel=0;
    }
}
