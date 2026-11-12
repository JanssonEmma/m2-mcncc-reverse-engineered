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
