# Old mob data declaration recovery

Complete CMob and its two by-value support records follow target DWARF in mob-types.json. File placement and naming follow the supplied reference tree, while record members, lengths and offsets follow the older binary.

The packed SMobTable is255 bytes, with DWORD dwVnum at0 and25-byte locale name at29. It does not inherit the newer SEntityTable. SMobSkillLevel is5 bytes; SMobSplashAttackInfo is8; SMobSkillInfo is20 with std::vector at8. CMob is356 bytes and its five skill records start at256. tests/mob_abi.cpp checks the complete record sizes and recovered member offsets under32-bit compilation. Native64-bit vector sizes differ.

Lifecycle, pointer-based cloning and AddSkillSplash are now implemented in game/src/mob_manager.cpp; see mob-recovery.md and mob-lifecycle-ghidra.json. CMob constructor/destructor fixture bodies have been removed. CMobManager, CMobInstance and wider mob loading/gameplay remain pending; this file is not a complete mob subsystem claim.
