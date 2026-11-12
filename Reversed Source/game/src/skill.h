#ifndef MCNCC_SKILL_MANAGER_H
#define MCNCC_SKILL_MANAGER_H
#include <map>
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
#include "../../libpoly/Poly.h"
// Original skill.h enum, DWARF4-byte storage, line5
enum ESkillFlags {
    SKILL_FLAG_ATTACK = 1,
    SKILL_FLAG_USE_MELEE_DAMAGE = 2,
    SKILL_FLAG_COMPUTE_ATTGRADE = 4,
    SKILL_FLAG_SELFONLY = 8,
    SKILL_FLAG_USE_MAGIC_DAMAGE = 16,
    SKILL_FLAG_USE_HP_AS_COST = 32,
    SKILL_FLAG_COMPUTE_MAGIC_DAMAGE = 64,
    SKILL_FLAG_SPLASH = 128,
    SKILL_FLAG_GIVE_PENALTY = 256,
    SKILL_FLAG_USE_ARROW_DAMAGE = 512,
    SKILL_FLAG_PENETRATE = 1024,
    SKILL_FLAG_IGNORE_TARGET_RATING = 2048,
    SKILL_FLAG_SLOW = 4096,
    SKILL_FLAG_STUN = 8192,
    SKILL_FLAG_HP_ABSORB = 16384,
    SKILL_FLAG_SP_ABSORB = 32768,
    SKILL_FLAG_FIRE_CONT = 65536,
    SKILL_FLAG_REMOVE_BAD_AFFECT = 131072,
    SKILL_FLAG_REMOVE_GOOD_AFFECT = 262144,
    SKILL_FLAG_CRUSH = 524288,
    SKILL_FLAG_POISON = 1048576,
    SKILL_FLAG_TOGGLE = 2097152,
    SKILL_FLAG_DISABLE_BY_POINT_UP = 4194304,
    SKILL_FLAG_CRUSH_LONG = 8388608,
    SKILL_FLAG_WIND = 16777216,
    SKILL_FLAG_ELEC = 33554432,
    SKILL_FLAG_FIRE = 67108864
};

// Original skill.h enum, DWARF4-byte storage, line42
enum ESkillIndexes {
    SKILL_RESERVED = 0,
    SKILL_SAMYEON = 1,
    SKILL_PALBANG = 2,
    SKILL_JEONGWI = 3,
    SKILL_GEOMKYUNG = 4,
    SKILL_TANHWAN = 5,
    SKILL_GIGONGCHAM = 16,
    SKILL_GYOKSAN = 17,
    SKILL_DAEJINGAK = 18,
    SKILL_CHUNKEON = 19,
    SKILL_GEOMPUNG = 20,
    SKILL_AMSEOP = 31,
    SKILL_GUNGSIN = 32,
    SKILL_CHARYUN = 33,
    SKILL_EUNHYUNG = 34,
    SKILL_SANGONG = 35,
    SKILL_YEONSA = 46,
    SKILL_KWANKYEOK = 47,
    SKILL_HWAJO = 48,
    SKILL_GYEONGGONG = 49,
    SKILL_GIGUNG = 50,
    SKILL_SWAERYUNG = 61,
    SKILL_YONGKWON = 62,
    SKILL_GWIGEOM = 63,
    SKILL_TERROR = 64,
    SKILL_JUMAGAP = 65,
    SKILL_PABEOB = 66,
    SKILL_MARYUNG = 76,
    SKILL_HWAYEOMPOK = 77,
    SKILL_MUYEONG = 78,
    SKILL_MANASHILED = 79,
    SKILL_TUSOK = 80,
    SKILL_MAHWAN = 81,
    SKILL_BIPABU = 91,
    SKILL_YONGBI = 92,
    SKILL_PAERYONG = 93,
    SKILL_HOSIN = 94,
    SKILL_REFLECT = 95,
    SKILL_GICHEON = 96,
    SKILL_NOEJEON = 106,
    SKILL_BYEURAK = 107,
    SKILL_CHAIN = 108,
    SKILL_JEONGEOP = 109,
    SKILL_KWAESOK = 110,
    SKILL_JEUNGRYEOK = 111,
    SKILL_LEADERSHIP = 121,
    SKILL_COMBO = 122,
    SKILL_CREATE = 123,
    SKILL_MINING = 124,
    SKILL_LANGUAGE1 = 126,
    SKILL_LANGUAGE2 = 127,
    SKILL_LANGUAGE3 = 128,
    SKILL_POLYMORPH = 129,
    SKILL_HORSE = 130,
    SKILL_HORSE_SUMMON = 131,
    SKILL_HORSE_WILDATTACK = 137,
    SKILL_HORSE_CHARGE = 138,
    SKILL_HORSE_ESCAPE = 139,
    SKILL_HORSE_WILDATTACK_RANGE = 140,
    GUILD_SKILL_START = 151,
    GUILD_SKILL_EYE = 151,
    GUILD_SKILL_BLOOD = 152,
    GUILD_SKILL_BLESS = 153,
    GUILD_SKILL_SEONGHWI = 154,
    GUILD_SKILL_ACCEL = 155,
    GUILD_SKILL_BUNNO = 156,
    GUILD_SKILL_JUMUN = 157,
    GUILD_SKILL_TELEPORT = 158,
    GUILD_SKILL_DOOR = 159,
    GUILD_SKILL_END = 162,
    GUILD_SKILL_COUNT = 12
};

// COMPLETE original field list from skill.cpp DWARF; original size1184.
// Requires complete CPoly declaration/type closure; not a reduced placeholder.
class CSkillProto {
public:
    CSkillProto();
    bool IsChargeSkill() { return dwVnum==SKILL_TANHWAN || dwVnum==SKILL_HORSE_CHARGE; }
    void SetPointVar(const std::string&,double);
    void SetDurationVar(const std::string&,double);
    void SetSPCostVar(const std::string&,double);
    char szName[64];
    DWORD dwVnum;
    DWORD bType;
    BYTE bMaxLevel;
    BYTE bLevelLimit;
    int iSplashRange;
    BYTE bPointOn;
    CPoly kPointPoly;
    CPoly kSPCostPoly;
    CPoly kDurationPoly;
    CPoly kDurationSPCostPoly;
    CPoly kCooldownPoly;
    CPoly kMasterBonusPoly;
    CPoly kSplashAroundDamageAdjustPoly;
    DWORD dwFlag;
    DWORD dwAffectFlag;
    BYTE bLevelStep;
    DWORD preSkillVnum;
    BYTE preSkillLevel;
    long int lMaxHit;
    BYTE bSkillAttrType;
    BYTE bPointOn2;
    CPoly kPointPoly2;
    CPoly kDurationPoly2;
    DWORD dwFlag2;
    DWORD dwAffectFlag2;
    DWORD dwTargetRange;
    BYTE bPointOn3;
    CPoly kPointPoly3;
    CPoly kDurationPoly3;
    CPoly kGrandMasterAddSPCostPoly;
};

struct SSkillTable;
typedef SSkillTable TSkillTable;
// COMPLETE original manager data/API. No prototype stand-in: forward pointer
// only is sufficient for lookup; separate full prototype/type metadata staged.
class CSkillManager : public singleton<CSkillManager> {
public:
    CSkillManager();
    virtual ~CSkillManager();
    bool Initialize(TSkillTable*,int);
    CSkillProto* Get(DWORD);
    CSkillProto* Get(const char*);
private:
    std::map<DWORD,CSkillProto*> m_map_pkSkillProto;
};
#endif
