// Insert into original game/src/constants.h before its final guard.
// Include ../../common/singleton.h outside this class.
// Old constants.h:193; complete logical class, target32 size40.
class CTableBySkill : public singleton<CTableBySkill>
{
public:
    CTableBySkill()
    {
        for (int job=0; job<JOB_MAX_NUM*2; ++job)
            m_aiSkillPowerByLevelFromType[job]=0;
        m_aiSkillDamageByLevel=0;
    }
    virtual ~CTableBySkill() { DeleteAll(); }
    bool Check();
    void DeleteAll();
    int GetSkillPowerByLevelFromType(int job,int skillgroup,int skilllevel,bool bMob);
    void SetSkillPowerByLevelFromType(int idx,int* aTable);
    void DeleteSkillPowerByLevelFromType(int idx);
    int GetSkillDamageByLevel(int Level);
    void SetSkillDamageByLevelTable(int* aTable);
    void DeleteSkillDamageByLevelTable();
private:
    int* m_aiSkillPowerByLevelFromType[JOB_MAX_NUM*2];
    int* m_aiSkillDamageByLevel;
};
