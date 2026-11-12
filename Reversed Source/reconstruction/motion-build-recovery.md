# Old motion-table construction

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

CMotionManager::Build08113440 and its three file-local filename helpers08112cc0/08112e00/08112e50 are reconstructed in game/src/motion.cpp. motion.h now contains exact old mode/public-motion enums from DWARF. mob_manager.h adds the old const-by-value iterator begin/end accessors, preserving all manager data.

## Filename selection

A missing motlist.txt selects legacy02.msa walk,03.msa run,20.msa normal attack,24.msa special1 and25.msa special2; other legacy motion names are empty. A present motlist suppresses legacy fallback, even when no entry matches. The old reader discards the mode/fourth columns, compares the second token case-insensitively and returns the third token. Three tokens suffice. A malformed or blank row terminates scanning rather than skipping to later entries. Static1024-byte token and256-byte path buffers, strtok behavior and original fixed-buffer formatting are retained.

## Table construction

Eight character sets load run/walk across general, twohand, onehand, dualhand, bow, bell, fan and horse modes in that order. Missing PC assets do not fail Build. Race slots3 and7 both load data/pc2/shaman; the older binary does not use data/pc/shaman for slot3. Maps insert uniquely and Build does not reset existing registrations.

Monster registry iteration skips empty folders and registers a set using table vnum. It reuses an empty motion candidate after failed loads, allocating another only after the candidate becomes nonempty. Walk/run/normal attack use general-mode keys; walk/run logs include an unmodified speed ratio against450 (exact ELF constant retained in motion-build-constants-elf.json).

Special motions are29..33 in this old enum, not the newer33..37. Every monster attempts the first skill. If that load fails with a zero declared skill, processing skips the rest of that monster. A missing declared skill fails Build and leaves partial tables in place. The second skill is normally the last attempted skill; slots2..4 are attempted only for vnum2493. For those slots the old failure diagnostics mistakenly print slot1's skill vnum. Exact original error lines330/286/299/310/321 and values are preserved.

Candidate lifetime is retained: the no-qualifying-monster case deletes its unused candidate; qualifying-monster/failure paths can leave unowned candidates allocated, as in the target. Existing duplicate-registration allocation behavior is not silently repaired. The tests exercise error cases for semantic fidelity and are not a heap-leak-free claim.

## Evidence and validation

motion-addressed-ghidra.json supplies Build; motion-build-helpers-ghidra.json supplies the three helpers, two owning-map deletion helpers and compiler initialization wrappers. The latter only initialize standard iostream machinery, not additional application tables. motion-enums-dwarf.json retains all old enum values. motion-build-instructions.txt independently confirms the2493 branch, old special keys and wrong-slot error arguments. motion-build-helper-types.json records helper signatures and motion-build-constants-elf.json records the exact450 constant.

motion_build_regression creates a new private fixture directory and actual PC/legacy/motlist/monster-skill assets, then uses the real Build, parser, pool, motion/mob classes and file I/O. It checks all character/mode keys, the shared shaman path, legacy naming, ignored mode/fourth columns, blank-row termination, no legacy fallback with a present motlist, skipped folders, skill0 short-circuit, mob2493 slots, all five declared-skill error paths, wrong-slot diagnostics, retained partial sets, no-monster behavior and tolerated missing PC files. The unused CMobInstance clock is an explicit aborting boundary. Created files and directories are cleaned non-recursively; no original assets/binary are executed or modified.

Default and O2 tests pass. Old enums are four-byte layout checked; motion/vector/pool layout checks pass under their documented32-bit/legacy-string scope. Updated motion/mob sources pass strict32-bit C++03 syntax. validation-status.json records the completed full-suite checkpoint.

All addressed motion application methods and filename helpers now have reconstructed bodies. This completes this module's current addressed behavior, not the server objective. Broad job/stat/affect/equipment logic, character teardown, startup, complete server build/link and Visual Studio integration remain unfinished. Native Windows server support remains deferred by user request.
