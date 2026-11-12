// Add #include "constants.h" outside the class in char.h.
// Original inline char.h584; nonconstDWORD GetNextExp(), no extra NULL guard.
DWORD GetNextExp() {
    if(m_points.level>=99) return 2000000000UL;
    return exp_table[m_points.level];
}
