# Old motion and monster-skill file loading

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

Three further addressed functions are reconstructed in game/src/motion.cpp and use the actual hierarchical parser, shared node pool and CMob::AddSkillSplash. There are no production file-loader/skill substitutes.

LoadFromFile08114b20 records the filename even on failure. It requires a motionduration token; accumulation is optional. Omitted/invalid accumulation does not reset an existing vector/flag. Failure does not reset prior duration/empty state. Success marks nonempty and logs using the first slash in the path, preserving the old slash-containing path contract. CMotionSet::Load08114270 constructs a motion, deletes it on failure, logs speed and uniquely inserts the composite mode/motion key. Duplicate insertion retains the previous pointer and does not dispose of the rejected allocation, matching the old behavior; tests avoid intentionally leaking duplicate loads.

LoadMobSkillFromFile08114440 requires duration, traverses motioneventdata/eventNN groups, ignores supported non-attack event types1/2/5..10, and handles type4 using spheredata00 position and startingtime. It appends splash timing100+truncatedMilliseconds and distance negative-truncatedY, then copies skill vnum/level from the mob table on complete success. It does not load filename/accumulation fields and does not clear prior splash vectors. Partial failures retain updated duration and appended entries; empty flag and final skill metadata update only on success. Original missing-count cursor/early-success behavior remains.

The target's unsigned casts use x87 truncated signed64 conversion followed by low32-bit extraction, including wrapped negative values and values above UINT32_MAX. motion-skill-instructions.txt independently confirms both qword FISTP conversions, control-word truncation, negation and +100. The source explicitly preserves those bits without depending on undefined float-to-unsigned conversion. Original error paths/lines and unknown-event-type assertion remain.

motion_file_regression reads real temporary asset files using actual parser/motion/mob code. It covers fresh/reloaded/failed motions, retained fields, keyed load failure/success, ignored events, append behavior, negative timing, above32-bit positions, partial results and every normal false-return error path. The unused CMobInstance clock is an explicit aborting test boundary; no motion-loading clock substitution is exercised. Shared group nodes are cleared through DestroySystem. Default and O2 tests pass; updated parser/motion sources pass strict32-bit syntax.

motion-addressed-ghidra.json supplies these bodies. Motion runtime/layout limits remain described in movement-recovery.md. CMotionManager::Build is still unimplemented; full motion-table construction, broader gameplay, complete server build/link and Visual Studio integration remain unfinished. Native Windows server support is deferred by user request.


Table-build follow-up: the old Build and filename helpers are now reconstructed and tested with actual fixture assets. Exact old enums, shaman-path duplication and mob2493 skill rules are preserved. See motion-build-recovery.md. This supersedes earlier notes saying Build was pending.
