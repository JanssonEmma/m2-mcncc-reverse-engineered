// Old motion lifecycle, unique registries, movement lookups and real asset/monster-skill loading.
#include "motion.h"
#include "text_file_loader.h"
#include "mob_manager.h"
#include "../../libthecore/include/log.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <stdint.h>
#include <strings.h>
CMotion::CMotion() : m_isEmpty(true),m_fDuration(0),m_isAccumulation(false) {
    m_vec3Accumulation.x=m_vec3Accumulation.y=m_vec3Accumulation.z=0;
}
CMotion::~CMotion() {}
float CMotion::GetDuration() const { return m_fDuration; }
const D3DVECTOR& CMotion::GetAccumVector() const { return m_vec3Accumulation; }
bool CMotion::IsEmpty() { return m_isEmpty; }
CMotionSet::CMotionSet() {}
CMotionSet::~CMotionSet() {
    for(TContainer::iterator it=m_map_pkMotion.begin();it!=m_map_pkMotion.end();++it) delete it->second;
    m_map_pkMotion.clear();
}
void CMotionSet::Insert(DWORD key,CMotion* motion) { m_map_pkMotion.insert(std::make_pair(static_cast<std::uint32_t>(key),motion)); }
const CMotion* CMotionSet::GetMotion(DWORD key) const {
    TContainer::const_iterator it=m_map_pkMotion.find(static_cast<std::uint32_t>(key));
    return it==m_map_pkMotion.end() ? 0 : it->second;
}
CMotionManager::CMotionManager() {}
CMotionManager::~CMotionManager() {
    for(TContainer::iterator it=m_map_pkMotionSet.begin();it!=m_map_pkMotionSet.end();++it) delete it->second;
    m_map_pkMotionSet.clear();
}
const CMotionSet* CMotionManager::GetMotionSet(DWORD vnum) {
    TContainer::iterator it=m_map_pkMotionSet.find(static_cast<std::uint32_t>(vnum));
    return it==m_map_pkMotionSet.end() ? 0 : it->second;
}
const CMotion* CMotionManager::GetMotion(DWORD vnum,DWORD key) {
    const CMotionSet* set=GetMotionSet(vnum);
    return set ? set->GetMotion(key) : 0;
}
float CMotionManager::GetMotionDuration(DWORD vnum,DWORD key) {
    const CMotion* motion=GetMotion(vnum,key);
    return motion ? motion->GetDuration() : 0;
}

bool CMotion::LoadFromFile(const char* filename) {
    m_stFileName=filename;
    CTextFileLoader loader;
    if(!loader.Load(filename)) return false;
    if(!loader.GetTokenFloat("motionduration",&m_fDuration)) {
        _sys_err("LoadFromFile",515,"%s does not have a duration",filename);
        return false;
    }
    if(loader.GetTokenPosition("accumulation",&m_vec3Accumulation)) m_isAccumulation=true;
    const double accumulationY = GetAccumVector().y;
    const double duration = GetDuration();
    const char* separator = std::strchr(filename, '/');
    sys_log(1, "%-48s %.3f %f", separator ? separator + 1 : filename, duration, accumulationY);
    m_isEmpty=false;
    return true;
}
bool CMotionSet::Load(const char* filename,int mode,int motion) {
    CMotion* loaded=new CMotion;
    if(!loaded->LoadFromFile(filename)) { delete loaded; return false; }
    const float displacement = -loaded->GetAccumVector().y;
    const float duration = loaded->GetDuration();
    const long double speed = static_cast<long double>(displacement) / duration;
    sys_log(0,"Motion: mode %d motion %d dist/sec %4.0f filename %s",mode,motion,static_cast<double>(speed),filename);
    Insert(MAKE_MOTION_KEY(mode,motion),loaded);
    return true;
}
static uint32_t MotionFileUnsigned32(long double value) {
    // Original unsigned casts use x87 truncated signed64 conversion and retain its low32 bits.
    if(!(value>=-9223372036854775808.0L && value<9223372036854775808.0L)) return 0;
    return static_cast<uint32_t>(static_cast<long long>(value));
}
bool CMotion::LoadMobSkillFromFile(const char* filename,CMob* mob,int skill) {
    CTextFileLoader loader;
    if(!loader.Load(filename)) return false;
    loader.SetTop();
    if(!loader.GetTokenFloat("motionduration",&m_fDuration)) {
        _sys_err("LoadMobSkillFromFile",407,"no motion duration %s",filename);
        return false;
    }
    std::string name;
    for(DWORD i=0;i<loader.GetChildNodeCount();++i) {
        loader.SetChildNode(i); loader.GetCurrentNodeName(&name);
        if(name=="motioneventdata") {
            DWORD count;
            if(!loader.GetTokenDoubleWord("motioneventdatacount",&count)) continue;
            for(DWORD j=0;j<count;++j) {
                if(!loader.SetChildNode("event",j)) {
                    _sys_err("LoadMobSkillFromFile",433,"no event data %d %s",j,filename); return false;
                }
                int type;
                if(!loader.GetTokenInteger("motioneventtype",&type)) {
                    _sys_err("LoadMobSkillFromFile",440,"no motioneventtype data %s",filename); return false;
                }
                switch(type) {
                    case 1:case 2:case 5:case 6:case 7:case 8:case 9:case 10:
                        loader.SetParentNode(); continue;
                    case 4:break;
                    default:assert(!"CRaceMotionData::LoadMotionData - Strange Event Type"); return false;
                }
                if(!loader.SetChildNode("spheredata",0)) {
                    _sys_err("LoadMobSkillFromFile",463,"no sphere data %s",filename); return false;
                }
                D3DVECTOR position;
                if(!loader.GetTokenPosition("position",&position)) {
                    _sys_err("LoadMobSkillFromFile",471,"no position data %s",filename); return false;
                }
                loader.SetParentNode();
                float starting;
                if(!loader.GetTokenFloat("startingtime",&starting)) {
                    _sys_err("LoadMobSkillFromFile",486,"no startingtime data %s",filename); return false;
                }
                const uint32_t timing = MotionFileUnsigned32(static_cast<long double>(starting) * 1000.0L) + 100u;
                const uint32_t distance = 0u - MotionFileUnsigned32(position.y);
                mob->AddSkillSplash(skill,timing,distance);
                loader.SetParentNode();
            }
        }
        loader.SetParentNode();
    }
    mob->m_mobSkillInfo[skill].dwSkillVnum=mob->m_table.Skills[skill].dwVnum;
    mob->m_mobSkillInfo[skill].bSkillLevel=mob->m_table.Skills[skill].bLevel;
    m_isEmpty=false;
    return true;
}

// File-local original helpers: a present motlist suppresses legacy-name fallback even without a match.
static char* FN_get_new_motion_file(FILE* file,EPublicMotion motion) {
    const char* field=0;
    switch(motion) {
        case MOTION_WALK:field="WALK";break;
        case MOTION_RUN:field="RUN";break;
        case MOTION_NORMAL_ATTACK:field="NORMAL_ATTACK";break;
        case MOTION_SPECIAL_1:field="SPECIAL";break;
        case MOTION_SPECIAL_2:field="SPECIAL1";break;
        case MOTION_SPECIAL_3:field="SPECIAL2";break;
        case MOTION_SPECIAL_4:field="SPECIAL3";break;
        case MOTION_SPECIAL_5:field="SPECIAL4";break;
        default:return const_cast<char*>("");
    }
    static char buffer[1024];
    while(std::fgets(buffer,sizeof(buffer),file)) {
        std::strtok(buffer," \t\r\n");
        char* type=std::strtok(0," \t\r\n");
        char* name=std::strtok(0," \t\r\n");
        std::strtok(0," \t\r\n");
        if(!type || !name) break;
        if(!strcasecmp(type,field)) return name;
    }
    return const_cast<char*>("");
}
static char* FN_get_old_motion_file(EPublicMotion motion) {
    switch(motion) {
        case MOTION_WALK:return const_cast<char*>("02.msa");
        case MOTION_RUN:return const_cast<char*>("03.msa");
        case MOTION_NORMAL_ATTACK:return const_cast<char*>("20.msa");
        case MOTION_SPECIAL_1:return const_cast<char*>("24.msa");
        case MOTION_SPECIAL_2:return const_cast<char*>("25.msa");
        default:return const_cast<char*>("");
    }
}
static char* FN_get_monster_motion_file(char* folder,EPublicMotion motion) {
    char motlist[256]={0};
    std::sprintf(motlist,"data/monster/%s/motlist.txt",folder);
    static char filename[256];
    FILE* file=std::fopen(motlist,"r");
    if(file) {
        std::sprintf(filename,"data/monster/%s/%s",folder,FN_get_new_motion_file(file,motion));
        std::fclose(file);
    } else std::sprintf(filename,"data/monster/%s/%s",folder,FN_get_old_motion_file(motion));
    return filename;
}
bool CMotionManager::Build() {
    const char* folders[8]={"data/pc/warrior","data/pc/assassin","data/pc/sura","data/pc2/shaman", "data/pc2/warrior","data/pc2/assassin","data/pc2/sura","data/pc2/shaman"};
    const char* modes[8]={"general","onehand_sword","twohand_sword","dualhand_sword","bow","bell","fan","horse"};
    const int loadOrder[8]={0,2,1,3,4,5,6,7};
    char filename[256];
    for(DWORD race=0;race<8;++race) {
        CMotionSet* set=new CMotionSet;
        m_map_pkMotionSet.insert(std::make_pair(race,set));
        for(int i=0;i<8;++i) {
            int mode=loadOrder[i];
            std::sprintf(filename,"%s/%s/run.msa",folders[race],modes[mode]); set->Load(filename,mode,MOTION_RUN);
            std::sprintf(filename,"%s/%s/walk.msa",folders[race],modes[mode]); set->Load(filename,mode,MOTION_WALK);
        }
    }
    CMotion* motion=new CMotion;
    bool found=false;
    for(CMobManager::iterator it=CMobManager::instance().begin();it!=CMobManager::instance().end();) {
        CMob* mob=(it++)->second;
        if(!mob->m_table.szFolder[0]) continue;
        found=true;
        CMotionSet* set=new CMotionSet;
        m_map_pkMotionSet.insert(std::make_pair(mob->m_table.dwVnum,set));
        const EPublicMotion ordinary[3]={MOTION_WALK,MOTION_RUN,MOTION_NORMAL_ATTACK};
        for(int i=0;i<3;++i) {
            if(!motion->IsEmpty()) motion=new CMotion;
            std::sprintf(filename,"%s",FN_get_monster_motion_file(mob->m_table.szFolder,ordinary[i]));
            if(motion->LoadFromFile(filename)) {
                set->Insert(MAKE_MOTION_KEY(MOTION_MODE_GENERAL,ordinary[i]),motion);
                if(i<2) {
                    const long double speed = -static_cast<long double>(motion->GetAccumVector().y) / motion->GetDuration();
                    sys_log(0,i==0 ? "Motion: #%05d %-24s walk dist/sec %3.0f ratio %.2f" : "Motion: #%05d %-24s run dist/sec %3.0f ratio %.2f",mob->m_table.dwVnum,mob->m_table.szLocaleName,static_cast<double>(speed),static_cast<double>(static_cast<float>(speed / 450.0L)));
                }
            }
        }
        const int lines[5]={330,286,299,310,321};
        const char* errors[5]={
            "Skill exist but no motion data for index 0 mob %lu skill %lu",
            "Skill exist but no motion data for index 1 mob %lu skill %lu",
            "Skill exist but no motion data for index 2 mob %lu skill %lu",
            "Skill exist but no motion data for index 3 mob %lu skill %lu",
            "Skill exist but no motion data for index 4 mob %lu skill %lu"};
        int skills=mob->m_table.dwVnum==2493 ? 5 : 2;
        for(int i=0;i<skills;++i) {
            if(!motion->IsEmpty()) motion=new CMotion;
            EPublicMotion index=static_cast<EPublicMotion>(MOTION_SPECIAL_1+i);
            std::sprintf(filename,"%s",FN_get_monster_motion_file(mob->m_table.szFolder,index));
            if(motion->LoadMobSkillFromFile(filename,mob,i)) set->Insert(MAKE_MOTION_KEY(MOTION_MODE_GENERAL,index),motion);
            else if(mob->m_table.Skills[i].dwVnum) {
                // The old diagnostics for slots2..4 mistakenly print slot1's skill vnum.
                _sys_err("Build",lines[i],errors[i],static_cast<unsigned long>(mob->m_table.dwVnum),
                         static_cast<unsigned long>(mob->m_table.Skills[i>=2 ? 1 : i].dwVnum));
                return false;
            } else if(i==0) break;
        }
    }
    // Preserve the old candidate lifetime: only the no-monster case deletes an unowned candidate here.
    if(!found) delete motion;
    return true;
}
