# Old stat dependency recovery scope

Target: game, SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c, x86 little-endian 32-bit GCC. Live Ghidra program identification is retained in program-info.json. All work here is read-only evidence inspection; no original ELF, build or test execution and no production/status edits.

## Evidence and immediate dependency

CItem::ModifyPoints 08167420 calls CHARACTER::ApplyPoint 0812f7d0; ApplyPoint calls CHARACTER::PointChange 0812e340. Complete untruncated decompilations are retained as address-decompile.json and readable address-decompile.c. Full instruction pagination is exhausted for ModifyPoints (335), ApplyPoint (255) and PointChange (1,342). The manifest records 57 complete decompilations and 16 exhaustive direct-call graph roots; the deeper transitive closure is intentionally unfinished.

The complete PointChange interface has much wider behavior than the set ordinarily reached from item applies. Do not replace it with an item-only switch or point-array accumulator to close a link. Conversely, quest/level/gold branches in PointChange are not evidence that an ordinary item apply directly invokes those branches.

## Verified old data first

old-data-metadata.json contains original ELF symbol bytes, per-table SHA-256, full point/apply/job enum values, record members/offsets/types and method signatures from char.cpp/constants.cpp/item.cpp DWARF. extract_metadata.py repeats this inspection using the existing work/python-deps pyelftools (set PYTHONPATH to that directory). No target process is started.

- EApplyTypes belongs in common/length.h. MAX_APPLY_NUM is 82; known applies are 0..81. EPointTypes belongs in game/src/char.h; named point IDs end at 126. POINT_MAX_NUM=255 belongs to the old EMisc in common/length.h, not to EPointTypes.
- SJobInitialPoints/TJobInitialPoints belongs in game/src/constants.h. It contains exactly sixteen signed int32 fields at successive four-byte offsets, size64; fields and original names are staged without invention. JobInitialPoints is mutable, four records/256 bytes at 0823f5e0. hp_per_ht=40, sp_per_iq=20 and stamina_per_con=5 in each of the four old rows.
- SApplyInfo/TApplyInfo is one BYTE bPointType at offset0, size1. const aApplyInfo is82 records/82 bytes at0821ae80. apply-point-map.json lists exact raw mapping and marks special switch entries; zero bytes in the table do not erase special dispatch semantics.
- CHARACTER_POINT is1056 and CHARACTER_POINT_INSTANT is1688 bytes, each with255 long32 point slots. Existing production CHARACTER data already contains these records, m_pSkillLevels and std::map<int,int> m_SkillDamageBonus. Do not shrink or reconstruct an alternative CHARACTER.
- SPlayerSkill is6 bytes. SMobTable is255 bytes; ComputeBattlePoints and GetPolymorphPoint depend on the old mob record and existing registry.
- packet_point_change is packed17 bytes: int32 header@0, DWORD VID@4, BYTE type@8, long32 amount@9, long32 value@13. The four-byte header is intentional; a modern BYTE-header packet is incompatible. PointChange sends literal17 bytes with header17.
- exp_table is a mutable pointer symbol at0832328c, initially stored in the ELF; exp_table_euckr at0821a100 and exp_table_newcibn at0821a440 each occupy400 bytes. Their exact file bytes are saved. Locale-driven pointer selection must be traced before claiming full EXP behavior. No running pointer value is asserted.

Staged insertion fragments: common-length-stat-enums.inc, char-point-enum.inc, point-max-num.inc, constants-stat-declarations.inc, constants-stat-definitions.inc. They preserve existing accessory tables by insertion. Original file placement is confirmed against the modern reference only for location/spelling; all values, record layouts and table contents come from the old ELF/DWARF. stat_data_abi.cpp is a staged C++03-compatible assertion translation unit for insertion under reconstruction/tests; it was not compiled. It checks every job field offset, table sizes/counts, long/int32 and both character point record sizes.

## ApplyPoint exact old behavior

DWARF signature is void ApplyPoint(BYTE bApplyType,long lVal). long is32-bit in this target. No default arguments are inferred.

- APPLY_NONE0 returns.
- APPLY_CON3 changes POINT_HT13, maxHP6 by lVal*job.hp_per_ht, maxStamina10 by lVal*job.stamina_per_con, in that order. Each mutation uses PointChange(...,false,false).
- APPLY_INT4 changes POINT_IQ15 and maxSP8 by lVal*job.sp_per_iq.
- APPLY_SKILL51 first requires m_pSkillLevels!=0. Skill key is unsigned high8 bits; magnitude is low23 bits; bit0x00800000 selects add versus subtract. m_SkillDamageBonus is the old std::map<int,int>. The old map indexing creates a zero entry when needed and erases the key when its final bonus is zero. This path does not call PointChange.
- Remaining explicitly accepted applies use the old aApplyInfo byte mapping and PointChange(...,false,false). Apply77 is excluded from the accepted switch, despite the zero table entry; it reaches the unknown-apply diagnostic. Values82+ also diagnose.
- Item skill removal must toggle0x00800000, not arithmetic-negate the packed value. ModifyPoints owns the iteration, socket/prototype/attribute ordering and appearance effects; this note does not replace that body.

The modern reference uses an unordered_map<BYTE,int> and lacks the old m_pSkillLevels guard and old erase-zero behavior. It also extends apply constants beyond81. Copying it wholesale changes old behavior.

## PointChange behavior families

Full original signature: void PointChange(BYTE type,int amount,bool bAmount,bool bBroadcast). Preserve every case, return, recursive call and packet rule; resolved goto labels are not a recovery blocker.

1. Level1 / EXP3 / nextEXP4 / level-step25: cap99, selected EXP table and signed arithmetic; quarter thresholds, recursive carry, random job HP/SP, old level-step item reward vnums27001/27002 count2, skill/stat rewards, save, quest LevelUp, guild/party level notifications and logging. NewCIBN overtime branches alter positive EXP/gold.
2. HP5/SP7/stamina9: dead/stun guards and upper-bound changes, target/party HP notifications, stamina walking transitions. Negative nonzero stamina changes can return before a point packet. maxHP6/maxSP8 recompute with old fixed percentage-addition caps1500/800, real/instant/party sources; maxStamina10 mutates the stored maximum.
3. Base stats12..15 and level1 recompute ComputeBattlePoints. That function itself invokes PointChange for battle grades, making a real dependency cycle; it also reads mobs, polymorph, job, equipment values and locale.
4. Stat/skill/reset/horse points synchronize real and instant storage where the old switch does. Defense grade16 also changes client defense20; bonus grades95..98 recursively update base grades18/16/22/23.
5. Bonus cases83..86 and114..118 cap additions at100 with exact diagnostics. Immunity88/89/90 updates dwImmuneFlag bits1/2/4 on nonzero values and clears on zero. Percentage119/120 triggers maxHP/maxSP recomputation. PC-bang125/126 uses assignment, not addition.
6. Polymorph108 calls SetPolymorph; mount109 calls MountVnum. MountVnum calls ComputePoints; ComputePoints calls equipment ModifyPoints, creating the broader stat/equipment cycle. These are full API branches even though ordinary default apply table entries do not map to108/109.
7. Packet tail sends only with descriptor, suppresses HP amount0, uses current VID, sets amount to0 when bAmount=false, and chooses PacketAround versus DESC::Packet using bBroadcast. Unknown types diagnose and return. POINT_NONE returns without packet.

Instruction exports are retained to check signedness, switch holes, packed argument order and decompiler STL argument noise while implementing. This bounded review does not certify every arithmetic overflow or all target paths as implementation-ready.

## Recovery order by dependency

1. Integrate only verified enum/record/table fragments and ABI assertions. Preserve original names and all existing data; compare staged constants bytes with old-data-metadata.json.
2. Reuse already reconstructed Get/SetRealPoint, GetPoint/GetLimitPoint, GetJob/GetWear, movement SetPoint, appearance accessors, mob registry, logger and packet transport after confirming signatures. Existing recovery notes explicitly say the full mutation methods remain unfinished. Name presence alone is not recovery proof.
3. Recover primitive state and wire helpers needed by mutation: IsDead/IsStun, SetLevel, BroadcastTargetPacket/party HP packet chain, SetNowWalking and exact point-change packet. Their complete bodies are exported here; follow saved callees to deeper dependencies before promotion.
4. Recover ComputeBattlePoints together with PointChange's dependent grade cases as a strongly connected component. GetPolymorphPoint and complete mob/job/equipment values must be honored. Do not break this cycle with a no-op or reduced statistic calculation.
5. Recover full PointChange peripheral branches: quest/guild/party LevelUp, AutoGiveItem, Save, logs, EXP pointer selection, overtime and random job increments. Saved root call graphs contain call-site addresses; selected deeper graphs cover quest/party/target/movement/reward paths. AutoGiveItem has30 direct call sites, not a leaf.
6. Close polymorph/mount/ComputePoints/ModifyPoints/affect/skill feedback as another strongly connected component. SetPolymorph has an indirect vtable call: resolve against old virtual layout before promotion. Full ComputePoints is exported; its deeper skill/affect closure is not yet complete.
7. Only after the full old dependencies are implemented and reviewed, promote ApplyPoint and ModifyPoints with exact table dispatch and skill representation. Add focused checks for every accepted apply, unknown77/82+, CON/INT order, guard/zero-erasure/packed-skill removal, packet17-byte layout, state transitions and recursive stat effects. Use explicit fixture boundaries only in tests.

## Remaining limits

This is evidence and ready data staging, not a method implementation. No production stat method is changed. All saved decompilations are complete at the tool boundary; decompiler types/indirect targets still require review. The manifest's graph boundary is explicit and not a claim of whole-server closure. No existing suite pass is treated as proof of a whole function or all branches. The full 2009 reconstruction remains unfinished.

## Complete enum and qualifier follow-up

common-length-emisc.inc stages all40 original EMisc enumerators, replacing the anonymous CHAT_MAX_LEN declaration when integrated. Use it instead of point-max-num.inc alone; do not add duplicate enumerator names. constant-qualifiers-provenance.json confirms exact DWARF tags SJobInitialPoints, TJobInitialPoints, SApplyInfo and TApplyInfo and the typedef-to-record links. Original ELF JobInitialPoints resides in writable .data (flags3); aApplyInfo resides in nonwritable .rodata (flags2). Preserve mutable job table and const apply table. No member was guessed.
