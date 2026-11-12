# Skill and polymorph helpers — checkpoint70

Checkpoint70: GetMobTable, GetSkillLevel (original char_skill.cpp placement), GetPolymorphPower and GetPolymorphPoint are integrated with exact nonconst signatures and original 41-entry power table (index39=100,index40=0). Tests cover all skill bytes, overflow diagnostics, raw quest overrides, call ordering on missing mob/default type, IQ substitution and modulo32 arithmetic. Actual quest/mob/point application paths are used; narrowly scoped external assertion interception tests call order. Full67 runtime commands plus ABI, focused/O2 and strict32 checks pass. ComputeBattlePoints and full PointChange remain staged and unfinished; full server/Visual Studio linking remains unfinished. See polymorph-helpers-recovery.md.

Evidence: battle_points_evidence/ and battle-points-addressed-ghidra.json. Production: game/src/char.cpp, char_skill.cpp, char.h, constants.cpp/.h. Tests: polymorph_helpers_regression.cpp, helper_desc_fixture.cpp, helper_desc_output_syscalls.c, polymorph_helpers_abi.cpp.

Validation receipts/logs: workspace work/checkpoint-70-validation.receipt.json, polymorph-helpers-focused.receipt.json, polymorph-helpers-o2.log, polymorph-helpers-m32.log. Full67 commands and ABI passed. Modern STL layout, native database/runtime integration, and whole server linking are not established.
