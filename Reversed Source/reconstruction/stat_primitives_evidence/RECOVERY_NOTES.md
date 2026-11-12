# Old stat primitives: staged recovery

Target SHA-256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Ghidra program game is x86 little-endian32 GCC. Original-file hash was independently read while extracting DWARF. No production/status/Makefile edits, builds, tests or original-ELF execution were performed.

## Ready insertion candidates

- common-tables-player-skill.inc: complete old SPlayerSkill/TPlayerSkill, packed6 bytes, bMasterType BYTE@0, bLevel BYTE@1, tNextRead signed int32@2.
- game-packet-point-change.inc: complete packet_point_change/TPacketGCPointChange, packed17 bytes, header int32@0, dwVID DWORD@4, type BYTE@8, amount long32@9, value long32@13. Four-byte header is original, not padding.
- char-positions-enum.inc: complete old EPositions (eight enumerators0..7), original char.h location. POS_DEAD=0.
- char-state-declarations.inc: three original non-const CHARACTER declarations. Do not copy the newer reference's const qualifiers.
- input-login-race-to-job.inc: complete RaceToJob body for original game/src/input_login.cpp; global C++ signature bool RaceToJob(unsigned,unsigned*).
- char-get-job.inc: complete original GetJob body for game/src/char.cpp and original extern RaceToJob declaration.
- char-battle-state-predicates.inc: complete IsDead/IsStun bodies for original game/src/char_battle.cpp. No mutation methods are introduced.
- stat_primitives_abi.cpp: staged C++03-compatible compile assertions for all record sizes/member offsets, header width and target scalar widths. Integrate under reconstruction/tests before compiling; it was not built here.

Existing common/tables.h already includes BYTE/DWORD definitions. Keep pack(push,1)/pop balanced. char.h already includes tables.h; its current forward class SPlayerSkill and redundant TPlayerSkill alias can be removed after the complete record is present, or the forward tag should be spelled struct to match the original. Do not introduce a second competing definition.

## Target time_t provenance and host portability

Old char.cpp DWARF chain is time_t DIE0x1e8b233 -> __time_t0x1e8a898 -> __int32_t0x1e8a766 -> base signed int4 DIE0x1e8a771. This is NOT signed long in the old target. The same chain is independently present in constants.cpp, input_login.cpp, char_battle.cpp and item.cpp. Full chains are retained in old-types-signatures.json.

The staged record deliberately spells the terminal storage type int tNextRead. This preserves the exact original scalar representation and field name while avoiding Windows native CRT time_t64 enlarging this record to10 packed bytes. It neither replaces nor globally redefines CRT time_t. Old time_t-based APIs, when recovered, must convert between host CRT values and target signed32 storage explicitly; they are outside this primitive slice. The ABI source asserts int32 and the6-byte skill record. It imposes no requirement that host sizeof(time_t)==4.

Old BYTE is unsigned char1; DWORD is unsigned long4; packet amount/value are signed long4. Their ABI assertions will intentionally reject an LP64 build without an appropriate target-scalar adaptation. Windows native64 retains int32/long32/DWORD32 and remains layout-compatible for these two records even with time_t64.

## Exact helper behavior and proof

RaceToJob080f2d50 (_Z9RaceToJobjPj): unsigned4 race, unsigned4* ret_job, bool1 result. It writes output0 before validating input. Races0/4 map0,1/5 map1,2/6 map2,3/7 map3; each returns true. Every other unsigned value returns false with output0. ret_job is required nonnull; no null-output behavior or guard is added. Instructions compare unsigned race with7 (JA), clear the output before the branch and use an eight-entry jump table at0821da30. Exact table bytes and targets are retained in evidence-summary.json. Modern MAIN_RACE symbol names are not invented where old DWARF did not retain them; staged switch uses exact numeric race cases and verified EJobs names.

GetJob0812a580 (_ZN9CHARACTER6GetJobEv) is BYTE GetJob(), non-const. Instruction0812a592 zero-extends byte CHARACTER+0x500, which original DWARF resolves as m_points@260 + job@1020. It calls RaceToJob and returns the result byte on success. It does NOT call GetRaceNum: polymorph race, mob race, descriptor state and other appearance overrides do not select the job. m_points.job stores a raw race code, so returning that field directly is also wrong for values4..7.

On conversion failure GetJob calls GetName, then _sys_err("GetJob",1569,"CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE",name,race), then returns0. Instruction0812a5ba loads0x621=1569; exact function-label and format bytes at08224494/08225000 are independently read from the ELF. Calling a sys_err macro at the new source line would lose this original diagnostic identity; the staged code uses _sys_err explicitly.

IsDead081485e0 (_ZN9CHARACTER6IsDeadEv) is bool IsDead(), non-const. It compares the signed long32 position field with POS_DEAD0 and returns the boolean. DWARF gives m_pointsInstant@1316 + position@1032 =2348 (0x92c), matching the load32 instruction. It does not inspect HP, death events, stun, affects or descriptors.

IsStun08148080 (_ZN9CHARACTER6IsStunEv) is bool IsStun(), non-const. It tests bit3 (8) of instant_flag and returns the boolean. m_pointsInstant@1316 + instant_flag@1036 =2352 (0x930); the target uses TEST byte [this+0x930],8. Testing long32&8 is exactly equivalent. No additional conditions or mutations occur. The staged literal8 is evidence-backed; no unverified full flag enum is invented.

The implicit-this type on definition DIEs may be a const-qualified pointer object. That does not make the pointed-to CHARACTER const. Declaration DIEs and all old mangled names lack the method-const K marker. Newer reference signatures are const and must not override this old evidence.

## Caller and dependency coverage

Complete decompilations and full instruction bodies are retained for the four helpers: RaceToJob21 instructions, GetJob31, IsDead9, IsStun8. No instruction pages remain. Calls/callers are saved with offsets, totals and has_more=false: RaceToJob two caller sites/no callees; GetJob54 caller sites/three callees; IsDead36 caller sites/no callees; IsStun19 caller sites/no callees. These are call-site counts, not a claim of the same number of unique caller functions.

RaceToJob's only two direct callers are GetJob and NewPlayerTable2 at080f2da0. Full NewPlayerTable2 export is retained: it converts raw race to a job for JobInitialPoints, while storing raw race into table->job. That function's larger player-creation behavior and full TPlayerTable are not promoted here. GetJob's dependencies are RaceToJob, existing GetName08127930 and existing _sys_err081b8150. Complete GetName export is saved as dependency evidence. All caller sites, including stat/skill/equipment consumers, are retained in evidence-summary.json and address-calls.json; name presence is not treated as implementation proof.

## Focused validation plan for parent integration

1. Compile the staged ABI assertions in target32 mode and the existing Windows native runtime mode. Verify both6-byte SPlayerSkill and17-byte point packet; do not change record types to match host time_t. Check every field offset/type width, pack restoration and typedef alias identity.
2. Serialize a skill with master3,level40,nextRead=-2 and compare exact bytes0328feffffff; deserialize signed-time boundaries INT_MIN/-1/0/INT_MAX without using CRT time_t storage. Change the source buffer after copy to catch accidental pointer storage.
3. Serialize a point packet with header17,VID0x89abcdef,type13,amount=-0x01020304,value0x12345678; expected bytes11000000efcdab890dfcfcfdfe78563412. Explicitly ensure header occupies four bytes and both signed longs remain four bytes. This validates layout, not the unfinished PointChange sender.
4. Call the real RaceToJob with all values0..255, sentinel-filled output, and UINT_MAX/256/0x80000000. Valid0..7 must return true and race%4; all invalid values false and output0. Do not call with null, which the target unconditionally dereferences.
5. With an actual existing CHARACTER fixture and real RaceToJob, exercise m_points.job0..255. Expect job=code%4 for0..7 and0 for8..255. Capture _sys_err only at the test boundary: exactly248 failures, functionGetJob,line1569,exactformat,GetName's name and raw zero-extended race. Valid calls must not diagnose. Check state before/after to ensure helper has no field mutation.
6. Prove independence from GetRaceNum: set raw job6 and polymorph race101, then check GetRaceNum returns101 while GetJob remains2. Repeat with a real mob record's different dwVnum and with no descriptor; job selection stays raw-code-based. Use existing constructors/fixtures and restore state; add no production stubs.
7. For IsDead, check all EPositions values0..7 plus negative and signed32 extrema: only0 is dead. Vary HP through0/positive/negative and instant_flag independently; outputs depend only on position.
8. For IsStun, test0,8,all other individual bits,combined masks,INT_MIN and-1; result is(flag&8)!=0 regardless of position/HP. Capture state bytes or the two fields to prove no mutation. Use real CHARACTER data, not a reduced test-only class.
9. Run appropriate existing affected character/stat suites and O2 only when parent integration is ready. No current pass is claimed, and no original binary execution is needed.

evidence-summary.json contains exact offsets, original strings/jump bytes, complete exports/callers and expected vectors. This bounded slice can be reviewed independently of PointChange, ApplyPoint or other unfinished mutation methods. Full target reconstruction remains unfinished.
