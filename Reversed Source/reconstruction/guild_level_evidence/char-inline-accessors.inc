// Original char.h declarations are NON-CONST, verified by old DWARF/linkage.
// Add GetLevel and REPLACE the current const GetPlayerID; do not add a second fake overload.
// Constructor inlining corroborates unsigned-byte level+0x502 and DWORD playerID+0xf0.
int GetLevel() { return m_points.level; }
DWORD GetPlayerID() { return m_dwPlayerID; }
