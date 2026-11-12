#include "char.h"
#include "config.h"
#include "../../libthecore/include/log.h"
// Insert in game/src/char_battle.cpp, original char_battle.cpp DWARF.
// Requires char.h's complete old EPositions and existing CHARACTER records.
// 081485e0: load32 CHARACTER+0x92c, compare zero.
bool CHARACTER::IsDead() {
    return m_pointsInstant.position==POS_DEAD;
}
// 08148080: bit3 test CHARACTER+0x930; no HP/position/affect condition.
bool CHARACTER::IsStun() {
    return (m_pointsInstant.instant_flag & 8L)!=0;
}

//081541e0 (original char_battle.cpp). Existing real UpdatePacket fanout required.
void CHARACTER::SetPKMode(BYTE bPKMode) {
    if(bPKMode>=PK_MODE_MAX_NUM || m_bPKMode==bPKMode) return;
    if(bPKMode==PK_MODE_GUILD && !m_pGuild) bPKMode=PK_MODE_FREE;
    // Equality was tested before fallback. FREE->GUILD without guild still
    // emits real update/log even though final value remainsFREE.
    m_bPKMode=bPKMode;
    UpdatePacket();
    sys_log(0,"PK_MODE: %s %d",GetName(),static_cast<unsigned int>(m_bPKMode));
}

#include "char.h"
#include "char_manager.h"
LPCHARACTER CHARACTER::GetVictim() { return CHARACTER_MANAGER::instance().Find(m_kVIDVictim); }
