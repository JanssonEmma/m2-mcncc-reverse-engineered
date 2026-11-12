#include "skill.h"
#include "char.h"
#include "../../common/tables.h"
#include "../../libthecore/include/log.h"
#include <cstdio>
#include <cstring>
#include <strings.h>
extern void SendLog(const char*);
// 080e4360: actual singleton and map default construction, no custom work.
CSkillManager::CSkillManager() {}
// 080e5990: actual map lookup; no insertion, null values preserved.
CSkillProto* CSkillManager::Get(DWORD vnum)
{
    std::map<DWORD,CSkillProto*>::iterator it=m_map_pkSkillProto.find(vnum);
    return it==m_map_pkSkillProto.end()?0:it->second;
}


// Complete original080e44d0; full prototype/CPoly lifecycle candidates accompany this file.
CSkillManager::~CSkillManager()
{
    for(std::map<DWORD,CSkillProto*>::iterator it=m_map_pkSkillProto.begin();it!=m_map_pkSkillProto.end();++it)
        delete it->second;
    m_map_pkSkillProto.clear();
}
// Original CSkillProto default construction initializes all twelve full CPoly
// members and nothing else. No scalar/name zeroing should be invented.
CSkillProto::CSkillProto() {}

// Complete old factory/name lookup/prototype setters and actual point-type table.
struct SPointOnType { const char* c_pszName;int iPointOn; };
SPointOnType kPointOnTypes[]={
    {"NONE",0},
    {"MAX_HP",6},
    {"MAX_SP",8},
    {"HP_REGEN",32},
    {"SP_REGEN",33},
    {"BLOCK",67},
    {"HP",5},
    {"SP",7},
    {"ATT_GRADE",95},
    {"DEF_GRADE",96},
    {"MAGIC_ATT_GRADE",97},
    {"MAGIC_DEF_GRADE",98},
    {"BOW_DISTANCE",34},
    {"MOV_SPEED",19},
    {"ATT_SPEED",17},
    {"POISON_PCT",37},
    {"RESIST_RANGE",74},
    {"CASTING_SPEED",21},
    {"REFLECT_MELEE",79},
    {"ATT_BONUS",93},
    {"DEF_BONUS",94},
    {"RESIST_NORMAL",99},
    {"DODGE",68},
    {"KILL_HP_RECOVER",87},
    {"KILL_SP_RECOVER",82},
    {"HIT_HP_RECOVER",100},
    {"HIT_SP_RECOVER",101},
    {"CRITICAL",40},
    {"MANASHIELD",102},
    {"SKILL_DAMAGE_BONUS",121},
    {"NORMAL_HIT_DAMAGE_BONUS",122},
    {"\n",0},
};
int FindPointType(const char* text)
{
    for(int i=0;*kPointOnTypes[i].c_pszName!='\n';++i)
        if(!strcasecmp(text,kPointOnTypes[i].c_pszName)) return kPointOnTypes[i].iPointOn;
    return -1;
}

void CSkillProto::SetPointVar(const std::string& strName, double dVar)
{
	kPointPoly.SetVar(strName, dVar);
	kPointPoly2.SetVar(strName, dVar);
	kPointPoly3.SetVar(strName, dVar);
	kMasterBonusPoly.SetVar(strName, dVar);
}

void CSkillProto::SetDurationVar(const std::string& strName, double dVar)
{
	kDurationPoly.SetVar(strName, dVar);
	kDurationPoly2.SetVar(strName, dVar);
	kDurationPoly3.SetVar(strName, dVar);
}

void CSkillProto::SetSPCostVar(const std::string& strName, double dVar)
{
	kSPCostPoly.SetVar(strName, dVar);
	kGrandMasterAddSPCostPoly.SetVar(strName, dVar);
}

bool CSkillManager::Initialize(TSkillTable * pTab, int iSize)
{
	char buf[1024];
	std::map<DWORD, CSkillProto *> map_pkSkillProto;

	TSkillTable * t = pTab;
	bool bError = false;

	for (int i = 0; i < iSize; ++i, ++t)
	{
		CSkillProto * pkProto = new CSkillProto;

		pkProto->dwVnum = t->dwVnum;
		std::strcpy(pkProto->szName,t->szName);
		pkProto->bType = t->bType;
		pkProto->bMaxLevel = t->bMaxLevel;
		pkProto->dwFlag = t->dwFlag;
		pkProto->dwAffectFlag = t->dwAffectFlag;
		pkProto->dwAffectFlag2 = t->dwAffectFlag2;

		pkProto->bLevelStep = t->bLevelStep;
		pkProto->bLevelLimit = t->bLevelLimit;
		pkProto->iSplashRange = t->dwSplashRange;
		pkProto->dwTargetRange = t->dwTargetRange;
		pkProto->preSkillVnum = t->preSkillVnum;
		pkProto->preSkillLevel = t->preSkillLevel;

		pkProto->lMaxHit = t->lMaxHit;

		pkProto->bSkillAttrType = t->bSkillAttrType;

		int iIdx = FindPointType(t->szPointOn);

		if (iIdx < 0)
		{
			std::sprintf(buf, "SkillManager::Initialize : cannot find point type on skill: %s szPointOn: %s", t->szName, t->szPointOn);
			_sys_err("Initialize",122,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		pkProto->bPointOn = iIdx;

		int iIdx2 = FindPointType(t->szPointOn2);

		if (iIdx2 < 0)
		{
			std::sprintf(buf, "SkillManager::Initialize : cannot find point type on skill: %s szPointOn2: %s", t->szName, t->szPointOn2);
			_sys_err("Initialize",135,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		pkProto->bPointOn2 = iIdx2;

		int iIdx3 = FindPointType(t->szPointOn3);

		if (iIdx3 < 0)
		{
			if (t->szPointOn3[0] == 0)
			{
				iIdx3 = POINT_NONE;
			}
			else
			{
				std::sprintf(buf, "SkillManager::Initialize : cannot find point type on skill: %s szPointOn3: %s", t->szName, t->szPointOn3);
				_sys_err("Initialize",154,"%s",buf);
				SendLog(buf);
				bError = true;
				delete pkProto;
				continue;
			}
		}

		pkProto->bPointOn3 = iIdx3;

		if (!pkProto->kSplashAroundDamageAdjustPoly.Analyze(t->szSplashAroundDamageAdjustPoly))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szSplashAroundDamageAdjustPoly: %s", t->szName, t->szSplashAroundDamageAdjustPoly);
			_sys_err("Initialize",166,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kPointPoly.Analyze(t->szPointPoly))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szPointPoly: %s", t->szName, t->szPointPoly);
			_sys_err("Initialize",175,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kPointPoly2.Analyze(t->szPointPoly2))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szPointPoly2: %s", t->szName, t->szPointPoly2);
			_sys_err("Initialize",184,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kPointPoly3.Analyze(t->szPointPoly3))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szPointPoly3: %s", t->szName, t->szPointPoly3);
			_sys_err("Initialize",193,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kSPCostPoly.Analyze(t->szSPCostPoly))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szSPCostPoly: %s", t->szName, t->szSPCostPoly);
			_sys_err("Initialize",202,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kGrandMasterAddSPCostPoly.Analyze(t->szGrandMasterAddSPCostPoly))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szGrandMasterAddSPCostPoly: %s", t->szName, t->szGrandMasterAddSPCostPoly);
			_sys_err("Initialize",211,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kDurationPoly.Analyze(t->szDurationPoly))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szDurationPoly: %s", t->szName, t->szDurationPoly);
			_sys_err("Initialize",220,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kDurationPoly2.Analyze(t->szDurationPoly2))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szDurationPoly2: %s", t->szName, t->szDurationPoly2);
			_sys_err("Initialize",229,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kDurationPoly3.Analyze(t->szDurationPoly3))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szDurationPoly3: %s", t->szName, t->szDurationPoly3);
			_sys_err("Initialize",238,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kDurationSPCostPoly.Analyze(t->szDurationSPCostPoly))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szDurationSPCostPoly: %s", t->szName, t->szDurationSPCostPoly);
			_sys_err("Initialize",247,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}

		if (!pkProto->kCooldownPoly.Analyze(t->szCooldownPoly))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szCooldownPoly: %s", t->szName, t->szCooldownPoly);
			_sys_err("Initialize",256,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}


		sys_log(0, "Master %s", t->szMasterBonusPoly);

		if (!pkProto->kMasterBonusPoly.Analyze(t->szMasterBonusPoly))
		{
			std::sprintf(buf, "SkillManager::Initialize : syntax error skill: %s szMasterBonusPoly: %s", t->szName, t->szMasterBonusPoly);
			_sys_err("Initialize",266,"%s",buf);
			SendLog(buf);
			bError = true;
			delete pkProto;
			continue;
		}


		sys_log(0, "#%-3d %-24s type %d flag %u affect %u point_poly: %s",
			pkProto->dwVnum, pkProto->szName, pkProto->bType, pkProto->dwFlag, pkProto->dwAffectFlag, t->szPointPoly);


		map_pkSkillProto.insert(std::map<DWORD, CSkillProto *>::value_type(pkProto->dwVnum, pkProto));
	}

	if (!bError)
	{
		std::map<DWORD,CSkillProto*>::iterator it=m_map_pkSkillProto.begin();

		while (it != m_map_pkSkillProto.end()) {
			std::map<DWORD,CSkillProto*>::iterator doomed=it++;
			delete doomed->second;
		}

		m_map_pkSkillProto.clear();

		it = map_pkSkillProto.begin();

		while (it != map_pkSkillProto.end())
		{
			m_map_pkSkillProto.insert(std::map<DWORD, CSkillProto *>::value_type(it->first, it->second));
			++it;
		}

		SendLog("Skill Prototype reloaded!");
	}
	else
		SendLog("There were erros when loading skill table");

	return !bError;
}

CSkillProto * CSkillManager::Get(const char * c_pszSkillName)
{
	std::map<DWORD, CSkillProto *>::iterator it = m_map_pkSkillProto.begin();

	while (it != m_map_pkSkillProto.end())
	{
		if (!strcasecmp(it->second->szName, c_pszSkillName))
			return it->second;

		it++;
	}

	return NULL;
}
