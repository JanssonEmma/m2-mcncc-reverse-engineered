# Closed helper4 integration: no ComputeBattlePoints / no PointChange

No production changes, builds or binary execution by this agent.

## Placement verified from original DWARF and symbol binding

helper-placement-evidence.json: GetSkillLevel08073560 is STB_GLOBAL,165bytes, concrete char_skill.cpp DIE0x42dd6c decl_file char_skill.cpp2611, no DW_AT_inline. The char.cpp class declaration instead names char.h1279; it has no inline attribute. It is NOT a weak header inline. Preserve as out-of-line partial game/src/char_skill.cpp using helper-char-skill.cpp.candidate (includes+body), or helper-char-skill.cpp.inc when creating that module manually.

GetMobTable concrete char.cpp1847, GetPolymorphPoint char.cpp2673, GetPolymorphPower char.cpp5870, all no inline attribute. Append helper-char.cpp.inc to existing real char.cpp, ensuring includes char.h,mob_manager.h,questmanager.h,constants.h,config.h,coreutils/log headers. helper-declarations.inc supplies only these4 public declarations and preserves nonconst/long/const-reference distinctions. Add prior constants.h/.cpp.additions.candidate once (array41 entries, index40ZERO). Existing constants.cpp includes constants.h first, so extern declaration gives the const definition original external linkage. Do NOT append compute-method or char-battle-complete candidates for this milestone. No CItem getters or new item enums are required by these four helpers.

## Concrete regression/recipe

Copy helper_regression.cpp, helper_desc_fixture.cpp and helper_desc_output_syscalls.c into reconstruction/tests. Makefile.helper.addition.candidate reuses the full currently working TARGET_WALKING_SOURCES/OBJECTS graph, filtering the original desc_fixture.cpp to the SAME descriptor fixture wrapped by helper_desc_fixture.cpp and swapping only the external syscall observer object. The wrapper preserves every existing test-only application lifecycle boundary; no new app method mock is introduced. New actual char_skill.cpp is included exactly once.

Legacy closure: GetPolymorphPower added to char.cpp calls GetSkillLevel, so EVERY existing executable linking char.cpp now also needs actual char_skill.cpp, even when that executable never invokes polymorph helpers. Add the new real module to DESC_PACKET_SOURCES and all older direct-char.cpp prerequisites/recipes. Existing mob/quest/locale/constants sources in the broad character graph remain real. Verify no duplicate module entries. The new recipe filters/deduplicates char_skill.cpp whether shared graph migration occurred yet or not. Register helper_regression in RUNTIME_TESTS/check/clean. Assertions must stay enabled for the two expected singleton-guard tests.

## Narrow external observers

helper_desc_output_syscalls.c accepts only actual GetSkillLevel2614 original overflowformat and GetJob1569 original invalidrawrace format. It captures GetName/value and verifies skill error precedes matching sys_log0. Every unrelated error remains a failing assertion. Other normal sys_log calls preserve the original descriptor observer behavior. No production diagnostics are bypassed.

helper_desc_fixture.cpp renames the existing descriptor fixture's external fw_assert_fail, then exposes an observer that calls helper_test_assert before the original fail handler. In TWO explicitly marked scopes only, quest singleton pointer is temporarily null while actual CMobManager exists; expected assertcondition ms_singleton in singleton.h throws a small C++ exception. The normal C++ stack unwinds (including the real temporary std::string), the test catches it and restores the actual quest singleton. All unrelated assertion paths still use the original abort handler. This is an external diagnostic boundary, not a replacement quest/skill/mob method.

The missing-mob scope proves GetPolymorphPoint STILL enters actual GetPolymorphPower instead of early returning; unsupportedpoint95 with a foundmob proves the same. Real manager lookup and real quest flag APIs are untouched. Map-lookup-before-Power ordering itself is additionally established in original assembly; these pure map calls have no external callback to observe directly. No-race and maintain-stat paths succeed with that same temporarily absent quest singleton and do not enter Power. This tests the required side effect safely without an application mock or indeterminate packet bytes.

## Test coverage

- Every stored skill BYTE0..255: cap40, all41 golden array values including39=100/40=0, unrelated packed skill bytes unchanged. Null skill pointer->skill0/power10. vnum0/254 valid;255/256/UINT_MAX errors+logs0 in order.
- test_server0/1/-1/INT_MIN and actual polyflag0/50/-50/INT_MIN/INT_MAX; exact raw nonzero overrides, absentflag remains absent, normalserver ignores flag.
- Found mob's distinct STR80/CON60/DEX40/INT100; playerST20/HT25/DX30/IQ40; supported12..15 and point95passthrough; all races0..7 and skillgroups0..3 for IQ substitution; rawrace255 emits EXACTLY2 GetJob errors forST and none forHT/DX/IQ. All actual GetJob/RaceToJob modules remain linked.
- Skill40zero and casterIQbase are explicitly checked. A separate wide-integer signed32 oracle covers all four point types, baseINT_MIN/-1/0/20/INT_MAX, mobstat0/1/81/255 and power50/-50/INT_MIN/INT_MAX. This exercises wrapped products, signed truncation and final wrapped addition without calling the candidate unsigned-int helpers.
- GetMobTable returns actual const-reference identity to the full mob table; subsequent table mutation is visible through that reference.
- Missingmob/unsupportedPower guard tests, noRace/maintain bypasses, actual flag map unchanged, mob lookup has no insertion. Full manually populated mob manager registries cleared before teardown; stack skill pointer/mob pointer cleared from CHARACTER. Existing full object lifecycle fixtures used unchanged.

No checks executed here. Parent should compile/run helper target normal/O2 plus strict32/signature checks, then all old character closures after adding char_skill.cpp. This package does not claim any ComputeBattlePoints/PointChange recovery beyond the already staged separate evidence.

Helper-only compile check: helper_signatures.cpp contains exactly the4 promoted method signatures, const-reference return and packed skill/table shape, with NO ComputeBattlePoints reference. Prior signatures.cpp.candidate covers the larger staged SCC and should not be used as a helper-only milestone until its other APIs are promoted.
