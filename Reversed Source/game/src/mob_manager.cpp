#include "text_file_loader.h"
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
//Partial original mob module: CMob lifecycle/clone and splash entries.
#include "mob_manager.h"
#include "char.h"
#include "char_manager.h"
#include "locale_service.h"
#include "questmanager.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/memcpy.h"
#include <utility>
#include "../../libthecore/include/log.h"
#include <cstring>
#include "../../libthecore/include/utils.h"

//080f5010/080f5060: default vector construction only; scalar fields remain untouched.
CMob::CMob() {}
//080f53d0/080f5440: compiler-generated vector destruction, no application body.
CMob::~CMob() {}
//080f50b0/080f5240: this pointer-based clone is not the implicit C++ copy constructor.
CMob::CMob(const CMob* mob) {
    std::memcpy(&m_table,&mob->m_table,sizeof m_table);
    for(unsigned i=2;i<5;++i) {
        m_table.Skills[i].dwVnum=256;
        m_table.Skills[i].bLevel=25;
    }
    for(unsigned i=0;i<5;++i) m_mobSkillInfo[i]=mob->m_mobSkillInfo[i];
}
//Field construction corroborated in AddSkillSplash's inlined temporary.
SMobSplashAttackInfo::SMobSplashAttackInfo(DWORD timing,DWORD distance)
    : dwTiming(timing),dwHitDistance(distance) {}
//080f54b0.
void CMob::AddSkillSplash(int index,DWORD timing,DWORD distance) {
    if(static_cast<unsigned int>(index)>=5) return;
    sys_log(0,"SkillIndex %d timing %lu hit_distance %lu",index,timing,distance);
    m_mobSkillInfo[index].vecSplashAttack.push_back(TMobSplashAttackInfo(timing,distance));
}

//080f5560/080f5590: separate clock reads; position is not initialized here.
CMobInstance::CMobInstance() {
    m_dwLastAttackedTime=get_dword_time();
    m_dwLastWarpTime=get_dword_time();
    m_IsBerserk=false; m_IsGodSpeed=false; m_IsRevive=false;
}
//080f55c0 and080f5970/080f5b80/080f5d90: normal singleton/container lifecycle.
//The old destructor frees map nodes but does not delete pointed-to mobs/groups.
CMobManager::CMobManager() {}
CMobManager::~CMobManager() {}
bool CMobManager::Initialize(TMobTable* table, int size)
{
    m_map_pkMobByVnum.clear();
    m_map_pkMobByName.clear();

    for (int i = 0; i < size; ++i) {
        TMobTable& row = table[i];
        CMob* mob = new CMob;
        const bool waterDragon = row.dwVnum == 2493;
        for (int skill = 0; skill < 3; ++skill) {
            row.Skills[skill].dwVnum = waterDragon ? 260 : 0;
            row.Skills[skill].bLevel = waterDragon ? 30 : 0;
        }
        if (waterDragon)
            sys_log(0, "Setup Manually Water dragon's Skill table.");

        thecore_memcpy(&mob->m_table, &row, sizeof row);
        m_map_pkMobByVnum.insert(std::make_pair(row.dwVnum, mob));
        m_map_pkMobByName.insert(std::make_pair(std::string(row.szLocaleName), mob));
        sys_log(0, "MOB: #%-5d %-30s LEVEL %u HP %lu DEF %u EXP %lu DROP_ITEM_VNUM %lu",
                static_cast<int>(row.dwVnum), row.szLocaleName, static_cast<unsigned>(row.bLevel),
                static_cast<unsigned long>(row.dwMaxHP), static_cast<unsigned>(row.wDef),
                static_cast<unsigned long>(row.dwExp), static_cast<unsigned long>(row.dwDropItemVnum));
        if (row.bType == 1 || row.bType == 3)
            CHARACTER_MANAGER::instance().RegisterRaceNum(row.dwVnum);
        quest::CQuestManager::instance().RegisterNPCVnum(row.dwVnum);
    }

    char groupFile[256];
    char groupGroupFile[256];
    ::snprintf(groupFile, sizeof(groupFile) - 1, "%s/group.txt", LocaleService_GetBasePath().c_str());
    ::snprintf(groupGroupFile, sizeof(groupGroupFile) - 1, "%s/group_group.txt", LocaleService_GetBasePath().c_str());

    if (!LoadGroup(groupFile)) {
        _sys_err("Initialize", 0x83, "cannot load %s", groupFile);
        thecore_shutdown();
    }
    if (!LoadGroupGroup(groupGroupFile)) {
        _sys_err("Initialize", 0x88, "cannot load %s", groupGroupFile);
        thecore_shutdown();
    }

    CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
    for (std::map<DWORD, LPCHARACTER>::iterator it = characters.m_map_pkChrByPID.begin();
         it != characters.m_map_pkChrByPID.end(); ++it)
        RebindMobProto(it->second);
    return true;
}

void CMobManager::RebindMobProto(CHARACTER* character)
{
    if (!character->GetDesc()) {
        if (const CMob* mob = Get(character->GetRaceNum()))
            character->SetProto(mob);
    }
}

//080f6530.
const CMob* CMobManager::Get(DWORD vnum) {
    std::map<DWORD,CMob*>::iterator it=m_map_pkMobByVnum.find(vnum);
    return it==m_map_pkMobByVnum.end()?0:it->second;
}
//080f6580: abbreviation searches sorted keys with a case-sensitive byte prefix.
const CMob* CMobManager::Get(const char* name,bool abbreviation) {
    if(abbreviation) {
        std::size_t length=std::strlen(name);
        for(std::map<std::string,CMob*>::iterator it=m_map_pkMobByName.begin();it!=m_map_pkMobByName.end();++it)
            if(!std::strncmp(it->first.c_str(),name,length)) return it->second;
        return 0;
    }
    std::map<std::string,CMob*>::iterator it=m_map_pkMobByName.find(name);
    return it==m_map_pkMobByName.end()?0:it->second;
}
//080f6b60.
CMobGroup* CMobManager::GetGroup(DWORD vnum) {
    std::map<DWORD,CMobGroup*>::iterator it=m_map_pkMobGroup.find(vnum);
    return it==m_map_pkMobGroup.end()?0:it->second;
}

DWORD CMobManager::GetGroupFromGroupGroup(DWORD vnum)
{
    std::map<DWORD,CMobGroupGroup*>::iterator found=m_map_pkMobGroupGroup.find(vnum);
    if (found==m_map_pkMobGroupGroup.end()) return 0;
    CMobGroupGroup* group=found->second;
    if (group->m_vec_dwMemberVnum.empty()) return 0;
    int roll=number_ex(1,group->m_vec_iProbs.back(),"mob_manager.h",0x51);
    std::vector<int>::iterator selected=std::lower_bound(group->m_vec_iProbs.begin(),group->m_vec_iProbs.end(),roll);
    return group->m_vec_dwMemberVnum[selected-group->m_vec_iProbs.begin()];
}

bool CMobManager::LoadGroup(const char* filename)
{
    CTextFileLoader loader;
    if (!loader.Load(filename)) return false;
    std::string name;
    for (DWORD i=0;i<loader.GetChildNodeCount();++i) {
        loader.SetChildNode(i);
        loader.GetCurrentNodeName(&name);
        int vnum;
        TTokenVector* tokens;
        if (!loader.GetTokenInteger("vnum",&vnum)) {
            _sys_err("LoadGroup",0x15e,"LoadGroup : Syntax error %s : no vnum, node %s",filename,name.c_str());
        } else if (!loader.GetTokenVector("leader",&tokens)) {
            _sys_err("LoadGroup",0x167,"LoadGroup : Syntax error %s : no leader, node %s",filename,name.c_str());
        } else if (tokens->size()<2) {
            _sys_err("LoadGroup",0x16e,"LoadGroup : Syntax error %s : no leader vnum, node %s",filename,name.c_str());
        } else {
            CMobGroup* group=new CMobGroup;
            group->m_dwVnum=static_cast<std::uint32_t>(vnum);
            group->m_stName=name;
            group->m_vec_dwMemberVnum.push_back(static_cast<std::uint32_t>(std::atoi(tokens->at(1).c_str())));
            sys_log(0,"GROUP: %-5d %s",vnum,name.c_str());
            sys_log(0,"               %s %s",tokens->at(0).c_str(),tokens->at(1).c_str());
            for (int member=1;member<256;++member) {
                char key[4];
                std::sprintf(key,"%d",member);
                if (!loader.GetTokenVector(key,&tokens)) break;
                sys_log(0,"               %s %s",tokens->at(0).c_str(),tokens->at(1).c_str());
                group->m_vec_dwMemberVnum.push_back(static_cast<std::uint32_t>(std::atoi(tokens->at(1).c_str())));
            }
            loader.SetParentNode();
            m_map_pkMobGroup.insert(std::make_pair(group->m_dwVnum,group));
            continue;
        }
        loader.SetParentNode();
    }
    return true;
}

bool CMobManager::LoadGroupGroup(const char* filename)
{
    CTextFileLoader loader;
    if (!loader.Load(filename)) return false;
    std::string name;
    for (DWORD i=0;i<loader.GetChildNodeCount();++i) {
        loader.SetChildNode(i);
        loader.GetCurrentNodeName(&name);
        int vnum;
        if (!loader.GetTokenInteger("vnum",&vnum)) {
            _sys_err("LoadGroupGroup",0x122,"LoadGroupGroup : Syntax error %s : no vnum, node %s",filename,name.c_str());
            loader.SetParentNode();
            continue;
        }
        CMobGroupGroup* group=new CMobGroupGroup;
        group->m_dwVnum=static_cast<std::uint32_t>(vnum);
        for (int member=1;member<256;++member) {
            char key[4];
            std::sprintf(key,"%d",member);
            TTokenVector* tokens;
            if (!loader.GetTokenVector(key,&tokens)) break;
            const DWORD groupVnum=static_cast<std::uint32_t>(std::atoi(tokens->at(0).c_str()));
            if (tokens->size()>1) std::atoi(tokens->at(1).c_str());
            if (!groupVnum) continue;
            int weight=group->m_vec_iProbs.empty() ? 1 : group->m_vec_iProbs.back()+1;
            group->m_vec_iProbs.push_back(weight);
            group->m_vec_dwMemberVnum.push_back(groupVnum);
        }
        loader.SetParentNode();
        m_map_pkMobGroupGroup.insert(std::make_pair(group->m_dwVnum,group));
    }
    return true;
}

void CMobManager::IncRegenCount(BYTE type, DWORD vnum, int count, int seconds)
{
    if (type == 0) {
        double& total = m_mapRegenCount[vnum];
        total = static_cast<long double>(total) + count * 86400.0L / seconds;
        return;
    }

    std::vector<DWORD> singleGroup;
    const std::vector<DWORD>* groups;
    if (type == 1) {
        singleGroup.push_back(vnum);
        groups = &singleGroup;
    } else if (type == 3) {
        std::map<DWORD, CMobGroupGroup*>::iterator it = m_map_pkMobGroupGroup.find(vnum);
        if (it == m_map_pkMobGroupGroup.end())
            return;
        groups = &it->second->m_vec_dwMemberVnum;
    } else {
        return;
    }

    for (std::vector<DWORD>::const_iterator it = groups->begin(); it != groups->end(); ++it) {
        CMobGroup* group = GetGroup(*it);
        if (!group)
            return;
        const long double rate = count * 86400.0L / seconds / groups->size();
        const std::vector<DWORD>& members = group->GetMemberVector();
        for (std::vector<DWORD>::const_iterator member = members.begin(); member != members.end(); ++member) {
            double& total = m_mapRegenCount[*member];
            total = static_cast<long double>(total) + rate;
        }
    }
}

void CMobManager::DumpRegenCount(const char* filename)
{
    FILE* file = std::fopen(filename, "w");
    std::fprintf(file, "MOB_VNUM\tCOUNT\n");
    for (std::map<DWORD, double>::const_iterator it = m_mapRegenCount.begin(); it != m_mapRegenCount.end(); ++it)
        std::fprintf(file, "%d\t%g\n", static_cast<int>(it->first), it->second);
    std::fclose(file);
}
