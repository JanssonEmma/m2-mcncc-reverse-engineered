// Complete old motion data/API; runtime, file/skill loading and table construction recovered.
#ifndef MCNCC_GAME_MOTION_H
#define MCNCC_GAME_MOTION_H
#include <string>
#include <map>
#include "../../libthecore/include/typedef.h"
#include "../../common/singleton.h"
#include "../../common/d3dtype.h"
enum EMotionMode {
    MOTION_MODE_GENERAL=0,
    MOTION_MODE_ONEHAND_SWORD=1,
    MOTION_MODE_TWOHAND_SWORD=2,
    MOTION_MODE_DUALHAND_SWORD=3,
    MOTION_MODE_BOW=4,
    MOTION_MODE_BELL=5,
    MOTION_MODE_FAN=6,
    MOTION_MODE_HORSE=7,
    MOTION_MODE_MAX_NUM=8
};
enum EPublicMotion {
    MOTION_NONE=0,
    MOTION_WAIT=1,
    MOTION_WALK=2,
    MOTION_RUN=3,
    MOTION_CHANGE_WEAPON=4,
    MOTION_DAMAGE=5,
    MOTION_DAMAGE_FLYING=6,
    MOTION_STAND_UP=7,
    MOTION_DAMAGE_BACK=8,
    MOTION_DAMAGE_FLYING_BACK=9,
    MOTION_STAND_UP_BACK=10,
    MOTION_DEAD=11,
    MOTION_DEAD_BACK=12,
    MOTION_NORMAL_ATTACK=13,
    MOTION_COMBO_ATTACK_1=14,
    MOTION_COMBO_ATTACK_2=15,
    MOTION_COMBO_ATTACK_3=16,
    MOTION_COMBO_ATTACK_4=17,
    MOTION_INTRO_WAIT=18,
    MOTION_INTRO_SELECTED=19,
    MOTION_INTRO_NOT_SELECTED=20,
    MOTION_SPAWN=21,
    MOTION_FISHING_THROW=22,
    MOTION_FISHING_WAIT=23,
    MOTION_FISHING_STOP=24,
    MOTION_FISHING_REACT=25,
    MOTION_FISHING_CATCH=26,
    MOTION_FISHING_FAIL=27,
    MOTION_STOP=28,
    MOTION_SPECIAL_1=29,
    MOTION_SPECIAL_2=30,
    MOTION_SPECIAL_3=31,
    MOTION_SPECIAL_4=32,
    MOTION_SPECIAL_5=33,
    PUBLIC_MOTION_END=34,
    MOTION_MAX_NUM=34
};
class CMob;
class CMotion {
public:
    CMotion();
    ~CMotion();
    bool LoadFromFile(const char* filename);
    bool LoadMobSkillFromFile(const char* filename,CMob* mob,int skill);
    float GetDuration() const;
    const D3DVECTOR& GetAccumVector() const;
    bool IsEmpty();
protected:
    bool m_isEmpty;
    std::string m_stFileName;
    float m_fDuration;
    bool m_isAccumulation;
    D3DVECTOR m_vec3Accumulation;
};
typedef DWORD MOTION_KEY;
#define MAKE_MOTION_KEY(mode,index) ((static_cast<DWORD>(mode)<<24)|(static_cast<DWORD>(index)<<8))
class CMotionSet {
public:
    typedef std::map<DWORD,CMotion*> TContainer;
    CMotionSet();
    ~CMotionSet();
    void Insert(DWORD key,CMotion* motion);
    bool Load(const char* filename,int mode,int motion);
    const CMotion* GetMotion(DWORD key) const;
protected:
    TContainer m_map_pkMotion;
};
class CMotionManager : public singleton<CMotionManager> {
public:
    typedef std::map<DWORD,CMotionSet*> TContainer;
    CMotionManager();
    virtual ~CMotionManager();
    bool Build();
    const CMotionSet* GetMotionSet(DWORD vnum);
    const CMotion* GetMotion(DWORD vnum,DWORD key);
    float GetMotionDuration(DWORD vnum,DWORD key);
protected:
    TContainer m_map_pkMotionSet;
};
#endif
