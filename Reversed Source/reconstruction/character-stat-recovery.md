# Old character stat reads, appearance and affect lookup

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

Point/appearance bodies remain in game/src/char.cpp and affect lookup in game/src/char_affect.cpp, matching original source placement. affect.h supplies complete old CAffect data; common/tables.h supplies the packed old TPacketAffectElement. No newer pool APIs are imported: this target uses ordinary new/delete for affects.

## Recovered functions

- GetRealPoint0812dec0 and SetRealPoint0812dee0 directly read/write original point storage. They add no index validation. Valid array indices are0..254; tests cover every valid index.
- GetPoint0812e0a0 reads instant point storage and returns the raw value. Types63/64 diagnose values above50 without clamping. Other types use signed32 maximum as diagnostic threshold. It does not validate the index.
- GetLimitPoint0812e120 reads instant points without storing the limited result. Attack speed17 caps at170 with descriptor or250 without; movement speed19 caps at200 with descriptor or250 without. Both speed types floor at0. Types63/64 cap and diagnose above50. Types114/115 cap at13 but diagnose only above50. Other values use upper2147483647 and lower-2147483647; therefore signed minimum-2147483648 becomes-2147483647 even for ordinary types.
- GetPoint and GetLimitPoint contain the original malformed diagnostic call: the format mentions name/type/value/max, but only name/value/max are supplied. This is independently confirmed by the saved instruction listing (three vararg stack writes). Reconstruction preserves the supplied values; the test logger captures only those values and does not format or read a nonexistent argument. This test boundary does not claim safe formatting in the production logger.
- SetPart08130520 and GetPart08130580 assert the four-slot appearance range. GetOriginalPart081305d0 returns base-part byte for body slot0 when a descriptor exists, otherwise current body part; hair slot3 returns current hair. Other indices return0.
- FindAffect0813d3a0 returns the first list entry matching type and optional apply index. Apply0 is a wildcard, not a request specifically for an apply0 entry. Lookup does not mutate or own entries.

## Data and evidence

CAffect is naturally aligned24 bytes: DWORD type@0, BYTE apply@4, long value@8, DWORD flag@12, long duration@16, long SP cost@20. Its packed DB/wire element is21 bytes with offsets0/4/5/9/13/17. affect_abi.cpp verifies every offset and both sizes in32-bit compilation.

character-stat-ghidra.json contains14 addressed stat/affect/mutation dependency bodies; only the functions above are promoted here. equipment-character-dependencies-ghidra.json supplies part access and future appearance/stat/affect removal dependencies. affect-dependency-metadata.json records complete old data. character-stat-instructions.txt verifies unsigned range selection for point types63/64, signed comparisons, descriptor-dependent speed caps and exact diagnostic arguments. These exports use read-only headless Ghidra on a task-owned saved project copy.

character_stat_regression validates all valid original/raw point slots, signed extrema, each cap boundary, nonmutation by limit reads, distinct diagnostic thresholds, appearance fallback with/without descriptor and ordered affect filtering. It uses real character construction/quest flags and existing descriptor integration infrastructure. Existing CHARACTER destructor, quest lifecycle, descriptor manager/client cleanup, OS and diagnostic logger boundaries remain explicit.

Focused default/O2 tests, complete affect layout checks and strict32-bit char.cpp/char_affect.cpp syntax checks pass. validation-status.json records the completed full-suite checkpoint. Original binary execution is never used. Modern STL/full host class ABI limitations remain unchanged.

## Remaining stat and equipment work

SetPoint requires real movement duration recalculation when speed changes during movement. PointChange, ApplyPoint, ComputePoints, ComputeBattlePoints, polymorph/skill scaling, appearance UpdatePacket, affect mutation/removal and equipment teardown remain unfinished. Additional movement/job helper bodies are saved in character-stat-next-ghidra.json to close these dependencies. No partial mutation bodies or production no-op stat/affect substitutes are added. Full server build/link and Visual Studio integration remain incomplete.


Movement follow-up: SetPoint and its real movement mode/speed/duration chain are now reconstructed, backed by actual motion ownership/lookups and the original duration utility. See movement-recovery.md. Asset parsing/build and full point/affect/equipment mutation remain pending.
