#ifndef MCNCC_TEST_CHARACTER_FIXTURE_H
#define MCNCC_TEST_CHARACTER_FIXTURE_H
#include "../../game/src/char.h"
#include "../../game/src/mob_manager.h"
class CharacterProbe : public CHARACTER {
public:
    void PreparePosition(BYTE kind,BYTE added,long flags,LPEVENT dead,LPEVENT stun) {
        m_bCharType=kind; m_bAddChrState=added; m_pointsInstant.instant_flag=flags;
        m_pkDeadEvent=dead; m_pkStunEvent=stun;
    }
    BYTE AddedState() const { return m_bAddChrState; }
    long InstantFlags() const { return m_pointsInstant.instant_flag; }
    long Position() const { return m_pointsInstant.position; }
    bool PendingIdle() const { return m_pNewState==&m_stateIdle; }
    bool PendingBattle() const { return m_pNewState==&m_stateBattle; }
    bool HasPendingState() const { return m_pNewState!=0; }
    void ForceIdle() { m_pCurrentState=&m_stateIdle; m_pNewState=0; }
    void ForceBattle() { m_pCurrentState=&m_stateBattle; m_pNewState=0; }
    void Race(BYTE job,DWORD polymorph) { m_points.job=job; m_dwPolymorphRace=polymorph; }
    void PlayerID(DWORD id) { m_dwPlayerID=id; }
    void Identity(DWORD id,DWORD crc) { m_vid=VID(id,crc); }
    void Name(const char* name) { m_stName=name; }
    void Mob(const CMob* mob) { m_pkMobData=mob; }
};
#endif
