// Full actual Show/Goto body translations; unresolved methods remain real declarations.
#include "char.h"
#include "sectree.h"
#include "sectree_manager.h"
#include "mob_manager.h"
#include "../../libthecore/include/log.h"
//08129ae0
bool CHARACTER::Show(long map,long x,long y,long z,bool spawnMotion) {
    SECTREE* tree=SECTREE_MANAGER::instance().Get(static_cast<DWORD>(map),
                                               static_cast<DWORD>(x),static_cast<DWORD>(y));
    if(!tree) { sys_log(0,"cannot find sectree by %dx%d mapindex %d",x,y,map);return false; }
    SetMapIndex(map);
    SECTREE* previous=GetSectree();
    bool changed=!previous || previous!=tree;
    if(changed) { if(previous) previous->RemoveEntity(this);ViewCleanup(); }
    if(m_bCharType==6) {
        sys_log(0,"SHOW: %s %dx%dx%d",GetName(),x,y,z);
        if(m_points.stamina<m_pointsInstant.iMaxStamina) StartAffectEvent();
    } else if(m_pkMobData) {
        m_pkMobInst->m_posLastAttacked.x=x;
        m_pkMobInst->m_posLastAttacked.y=y;
        m_pkMobInst->m_posLastAttacked.z=z;
    }
    if(spawnMotion) { m_bAddChrState|=2;m_afAffectFlag.bits[0]|=4; }
    SetXYZ(x,y,z);
    m_posDest.x=x;m_posDest.y=y;m_posDest.z=z;
    if(changed) {
        EncodeInsertPacket(this); // Primary virtual slot2.
        tree->InsertEntity(this);
        UpdateSectree();
    } else {
        ViewReencode();
        sys_log(0,"      in same sectree"); // No format fields; stale args omitted.
    }
    m_bAddChrState&=0xfd;
    return true;
}
//0812d810. Existing destination x/y affects whether duration is recomputed.
bool CHARACTER::Goto(long x,long y) {
    if(GetX()!=x || GetY()!=y) {
        if(m_posDest.x!=x || m_posDest.y!=y) {
            m_posDest.x=x;m_posDest.y=y;
            CalculateMoveDuration();
            m_dwStateDuration=4;
            if(!IsState(m_stateMove)) {
                CHARACTER* victim=GetVictim();
                MonsterLog("[MOVE] %s",victim?"\264\353\273\363\303\337\300\373":
                                               "\261\327\263\311\300\314\265\277");
                victim=GetVictim();
                if(victim) MonsterChat(1);
            }
            GotoState(m_stateMove);
            return true;
        }
        if(!IsState(m_stateMove)) { m_dwStateDuration=4;GotoState(m_stateMove); }
    }
    return false;
}
