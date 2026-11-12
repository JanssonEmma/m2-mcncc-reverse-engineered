//0818d010 full SpawnMob. All actual classes/data retained; see unresolved closure ledger.
#include "char_manager.h"
#include "char.h"
#include "mob_manager.h"
#include "sectree.h"
#include "sectree_manager.h"
#include "questmanager.h"
#include "utils.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include "horse-summon-detail.h"
LPCHARACTER CHARACTER_MANAGER::SpawnMob(DWORD vnum,long map,long x,long y,long z,
                                      bool spawnMotion,int rotation,bool show) {
    const CMob* mob=CMobManager::instance().Get(vnum);
    if(!mob) { _sys_err("SpawnMob",0x132,"SpawnMob: no mob data for vnum %lu",vnum);return 0; }
    if(mob->m_table.bType!=1 && mob->m_table.bType!=3) {
        SECTREE* tree=SECTREE_MANAGER::instance().Get(static_cast<DWORD>(map),
                                                   static_cast<DWORD>(x),static_cast<DWORD>(y));
        if(!tree) {
            sys_log(0,"no sectree for spawn at %d %d mobvnum %d mapindex %d",x,y,vnum,map);
            return 0;
        }
        DWORD attributes=tree->GetAttribute(x,y);
        if(attributes&0x81) {
            static bool log=quest::CQuestManager::instance().GetEventFlag("spawn_block_log")!=0;
            static DWORD next=static_cast<DWORD>(horse_summon_detail::Add(get_global_time(),10000));
            DWORD now=static_cast<DWORD>(get_global_time());
            if(next<now) { next=now;log=quest::CQuestManager::instance().GetEventFlag("spawn_block_log")!=0; }
            if(log) sys_log(0,"SpawnMob: BLOCKED position for spawn %s %lu at %d %d (attr %lu)",
                           mob->m_table.szName,vnum,x,y,attributes);
            return 0;
        }
        if(attributes&4) {
            sys_log(0,"SpawnMob: BAN_PK position for mob spawn %s %lu at %d %d",
                    mob->m_table.szName,vnum,x,y);
            return 0;
        }
    }
    CHARACTER* character=CHARACTER_MANAGER::instance().CreateCharacter(mob->m_table.szLocaleName,0);
    if(!character) { sys_log(0,"SpawnMob: cannot create new character");return 0; }
    if(rotation==-1) rotation=number_ex(0,360,"char_manager.cpp",0x167);
    character->SetProto(mob);
    if(mob->m_table.bType==1 && character->GetEmpire()==0)
        character->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(map));
    character->SetRotation(static_cast<float>(rotation));
    if(!show) return character;
    if(character->Show(map,x,y,z,spawnMotion)) return character;
    CHARACTER_MANAGER::instance().DestroyCharacter(character);
    sys_log(0,"SpawnMob: cannot show monster");
    return 0;
}
