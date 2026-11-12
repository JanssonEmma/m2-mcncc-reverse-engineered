# Old motion runtime and movement timing

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

Production placement follows the reference tree: complete old motion data/API in game/src/motion.h, runtime bodies in motion.cpp, character movement functions in char.cpp, and CalculateDuration in utils.cpp. common/d3dtype.h currently declares the complete old three-float vector used by motions. No newer motion data layout is substituted: the old CMotion includes a filename string that is absent from the supplied newer header.

## Recovered runtime

CMotion initializes empty=true, filename default string, duration0, accumulation=false and zero XYZ. Its destructor is ordinary string destruction. GetDuration/GetAccumVector/IsEmpty use their old const signatures. CMotionSet constructs its map, performs unique insertion (duplicates retain the first pointer), provides non-inserting lookup, and deletes all owned motions at destruction. CMotionManager constructs singleton/map state, provides set/motion/duration lookup (null/zero on missing keys), and owns/deletes all sets. Compiler-generated base/member work is left to C++, not application no-ops.

Full original data are retained: vector12 bytes; CMotion28 with the old4-byte string; motion-set12 and manager16 with old GCC3.3 maps. motion_abi.cpp verifies every CMotion/vector field and size under32-bit legacy COW-string mode. Modern map/host class sizes still differ and are not asserted equivalent.

## Character movement

- GetMotionMode0812d940 returns general0 during polymorph or without a weapon. Weapon subtypes0..5 map to modes1/3/4/2/5/6; other subtypes return0. As in the target, a non-null weapon is expected to have a prototype.
- GetMoveMotionSpeed0812d9c0 selects run when not walking with positive stamina, or whenever no descriptor exists; otherwise walk. It looks up a mounted creature's general walk/run motion first. Missing mount data falls back to character race horse-mode7. Unmounted movement uses current weapon mode and character race. Missing motion logs and returns300; existing motion speed is negative accumulation Y divided by duration, with no new zero-duration/accumulation checks.
- CalculateDuration08147420 applies the old integer percentage transformation: speed100 uses100%, below100 uses200-speed, above100 uses integer10000/speed. It preserves signed32 register wrapping in intermediate subtraction/multiplication and truncating signed division.
- CalculateMoveDuration0812dc10 copies current X/Y into movement start while retaining start Z, computes squared XY distance using wrapped32-bit arithmetic, stores sqrt as float, derives a base millisecond duration, applies GetLimitPoint(19)/CalculateDuration, optionally logs except character type6, then records the current clock. The binary's x87 conversion sets truncation control; it does not round16.66ms to17. Its integer-indefinite conversion behavior is retained for invalid/out-of-range float conversions.
- SetPoint0812e230 stores the instant point directly. Changing type19 recalculates only when current time is strictly below the unsigned32 sum of old start+duration. Equality does not recalculate. Original clock wrap behavior remains, including the simple wrapped comparison.

## Evidence

motion-addressed-ghidra.json contains25 addressed motion symbols, including const accessors and constructor/destructor variants. LoadFromFile, LoadMobSkillFromFile, CMotionSet::Load and CMotionManager::Build remain declarations while their parser/build dependencies are recovered; this archive is not a claim that those methods are implemented. motion-dependency-metadata.json retains complete old classes/signatures. character-stat-next-ghidra.json supplies movement methods, character-stat-ghidra.json supplies SetPoint, and movement-duration-util-ghidra.json supplies CalculateDuration.

movement-duration-instructions.txt confirms x87 truncation and integer wrapping/division. movement-constants-elf.json records exact ELF bytes/offsets for fallback300 and millisecond factor1000. All binary analysis uses the saved project read-only; the server binary is never executed.

## Validation

movement_regression uses actual motion objects/maps/lifecycle, character construction, inventory lookup, duration utility, clock, item/manager and descriptor integration. Motion fixtures seed full real objects with known duration/accumulation; they do not claim asset-file loading. Tests cover unique lookup/insertion, weapon/polymorph modes, walk/stamina/no-descriptor selection, mount-first and horse fallback, missing-motion300, zero/infinite speed behavior, duration truncation/overflow, retained Z and speed changes at/after deadlines and around clock wrap. Manager teardown executes real owned set/motion deletion and clears singleton lifetime; it is not a standalone heap-leak proof.

Existing isolated character suites now link actual motion and char_item lookups. movement_duration_impl.cpp compiles the unchanged production CalculateDuration body from utils.cpp; unrelated clock functions are renamed in that test translation unit and their unused clock dependencies abort. Existing scripted clock fixtures remain explicit. No production duration/motion stand-ins are used. The final harness links all required bodies normally.

Default and O2 movement tests pass; the duration body is also compiled at O2 for that focused run. Motion layout and strict32-bit char/motion/utils syntax checks pass. Existing CHARACTER/quest/sector/application lifecycle/OS fixture limitations remain. validation-status.json records the completed whole-suite checkpoint.

## Remaining work

The hierarchical text loader and its node pool are the next dependency: text-loader-addressed-ghidra.json saves49 parser/helper bodies, and text-loader-metadata.json saves complete loader/group/pool/vector data. Motion file loading, table Build, full job/stat/affect mutation, equipment cleanup, character teardown, full server build/link and Visual Studio integration remain unfinished. Native Windows server support is deferred by user request.


Asset-loading follow-up: the full old hierarchical parser/pool and motion/file/monster-skill loading are now reconstructed with real-file tests. See text-loader-recovery.md and motion-file-recovery.md. Motion table Build remains pending.


Table-build follow-up: the old Build and filename helpers are now reconstructed and tested with actual fixture assets. Exact old enums, shaman-path duplication and mob2493 skill rules are preserved. See motion-build-recovery.md. This supersedes earlier notes saying Build was pending.
