# PC integration READY — current payload frozen
This supersedes older bounded-save paragraphs saying not integration-ready.
No child F writes/builds. Root owns apply/proof.
prepare_integration.py rendered against CURRENT checkpoint87/full95 tree; integration-manifest.json guards14 files. apply_integration.py preflights ALL original/staged SHA, backs up, then applies. Re-render ONLY before apply if baseline changed.
Source/data review passes integration-review.json: complete PC and manager fields preserved,61 old API declarations, old LastState and managerrawfields (exceptL) remain indeterminate, all preexisting fixture hooks/shutdown boundaries retained,95 canonical runtime names/order preserved plus new96th suite.
Manager C2ctor0819f0f0 now fully audited with code/calls/assembly and confirms onlyL scalar initialized. DestructorOwnedTimer now verifies scope-exit populated timer cleanup without explicit Destroy. item_regression prereq corrected to include actual manager/PC/event source used by recipe.
Frozen payload: PC/event production files+API/manager ctor+lookup; honest L==0/emptyNPC TEST manager destructor; retire recovered ctor/PCabort; explicit unused legacy FindEventInfo abort conditional; actual scheduler in new test; old questflag target expands to real CHAR/PID graph. No Lua close/VM fake or new LTO workaround.
Original implicit PC copy copies indeterminate LastState too. Avoiding direct test reads does NOT establish definedness/portability. Preserve original source behavior; never zero/deepcopy to repair it.
No old whole STL class sizeof gates.

Proof helpers:
work/quest_pc_lookup_o2.py: selected actual PC/event/manager C++ graph +4 DISTINCT SQL/config objects; real scheduler flag; source hashes -> work/quest-pc-lookup-o2.receipt.json.
Strict32 cwd reconstruction/tests:
g++.exe -std=c++03 -Wall -Wextra -pedantic -m32 -Ifdwatch_shim -Isocket_shim -Iposix_shim -Isql_shim -I../dependencies/mysql-4.1.3/include -include sql_socket_platform.h -Wno-long-long -fsyntax-only ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/questmanager.cpp quest_pc_lookup_abi.cpp quest_pc_lookup_regression.cpp
Root focused4 planned: quest_pc_lookup_regression quest_flag_regression character_manager_regression target_walking_regression +ABI, then O2/strict/full96. Child did not run these.

Evidence/ledger:
pc-addressed-ghidra.json + pc-coverage-mappings.pending.json:13 distinct addressed variants,10 semantic source methods. Implicit copy preserved separately as compiler evidence; inline IsLoaded has no invented address. No duplicate semantic feature count.
ONE gated recorder work/checkpoint88_quest_pc/prepare_record.py pending-mode passed. At root final proof:
python work/checkpoint88_quest_pc/prepare_record.py --root "PROJECT_ROOT\Reversed Source" --record --full work/checkpoint-88-validation.receipt.json --focused work/quest-pc-focused.receipt.json --o2 work/quest-pc-lookup-o2.receipt.json --strict32-log work/quest-pc-m32.log --strict32-exit-code 0
Supply actual receipt basenames. Gate full96 actually executed+ABI, exactfocused4+ABI, source-matched optimized real graph/4SQL, explicit strict32exit0+log, exact14 applied SHA. Optional --source-guards requires reviewed same14 paths, no automatic relaxed acceptance. Prior checkpoint87full95 must be recorded.
Only after proof: prepared workspace copies merge13 explicit associations/status/docs with all previous baseline entries. NO automatic production/ledger writes.
Full PC remaining VM/state/reward methods remain genuine undefined declarations, no fake bodies. Owning Lua/NPC/LevelUp dispatch and full PointChange/stat/server closure unfinished.
