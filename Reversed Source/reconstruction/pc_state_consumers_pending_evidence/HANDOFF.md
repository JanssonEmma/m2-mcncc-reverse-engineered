# PC state-query consumers — NEXT97, UNAPPLIED/UNCOMPILED

Originaltarget SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c confirmed live get_program_info before targeted readonlycalls. Complete originaldecompile/assembly/callgraphs saved for all3: SetCurrentQuestStateName0805e2a0 (61instructions/7directcalls), string/stringSetQuestState0805e380 (35/3), SendFlagList08061980 (121/8), alltruncatedfalse/has_morefalse. Original PC61declarations alreadycontainall3; no newAPI/records or inventedconstchar overload implementation.

Root has chosen to leave this package PREPARED ONLY aftereventflag+querygroup96/full108. Next97 wouldaddONE canonical pc_state_consumers_regression ->109, not join96. Stop expansion. No Fsourcewrites, builds/compiler/runtime/ABI claims by this agent. Root allowanceprimary72% atstopinstruction; save restart before limit.

## Whole original source / dependency closure

questpc.cpp.fragment contains WHOLE3namespacequest methods. Candidate prepends actualchar.h/questmanager.h includes and appends completefragment to existing originalgame/src/questpc.cpp; preserves complete108byteold PCdata/fullAPI and realQuestState. GetQuestStateName081a5130/GetQuestStateIndex081a5240 depend on root96's actualsuppliedLua5.0.3 CSDK (work/quest_lua_queries), never a mockmap/VM. NativeSDK publicopaqueAPI compatible role only; oldadvertisedLua5.0 and oldinternalstate92/TObject12 are NOT asserted equivalent to supplied5.0.3 internals (MinGW32 state96/TObject16). No packing/owner/type replacements or fakeInitializeLua ownership.

SetCurrentQuestStateName calls original uppercase CQM::Instance().GetQuestStateIndex(m_stCurQuest,state_name) FIRST, then builds flag from CURRENT m_stCurQuest after realLua/debug callbacks and calls actualSetFlag(...,index,false). It does NOT assign m_RunningQuestState->st or queue transition, and has no runningstate requirement. Original uppercaseInstance assertrole retained; no addedguards. Zeroindex takes realDeleteFlag/SaveFlag path, no queryerror propagation added.

String/stringSetQuestState similarly gets index through original uppercaseInstance, THEN passes live constref quest_name and cachedindex into complete existing integeroverload0805e400. That integeroverload gets previousflag and queues via realAddQuestStateChange, not directflag/statechange. If quest_name aliases mutable PCcurrentname and Lua changesit, subsequentactualintegeroverload sees changedname; do not snapshot/refcopy beforequery. Standalone constchar*/constchar* overload remains unresolved and not silentlysupplied; tests use explicitstd::string parameters.

SendFlagList traverses LIVE sorted m_FlagMap, advances AFTER actualChatPacket. Nonstrictsuffix (<10 or final9bytes!=.__status) outputs actualtype1 `%s %d`. Matching branch COPIES questprefix first, calls actualGetQuestStateName(prefix,it->second), then retains rawborrowed constchar* and rereads it->second for actualChatPacket `%s %s (%d)`. Thus Lua lookup can receive17 while finalnumericfield becomes99 aftermetamethod; namepointer remains raw/unprotected afterstackrestore. No nullcheck/copy/GCroot/normalization added. Empty map accepts nullreceiver because no deref; othernull/dangling/invalidformat domains remainoriginal.

## ActualSDK/network test (prepared only)

pc_state_consumers_regression.cpp owns a real lua_open state and real qtable with forward/reverseentries plus actualLuaC __index callback. It lends manager.L then resetsL0 before existing testCQM lifecycle boundary and closes SDKstate itself; no fakeLuaClose/productionlifecycle change. Existing actualfullTARGET_WALKING/event/event_queue/PC/CQM/CHAR/ChatPacket/DESC/TEMP_BUFFER/buffer graph remains; explicitnativeOS/diagnostic and unrelatedlifecyclefixtures remainhonest.

Cases: nonzeroCurrentstate→realflags/pendingflags with runningst41unchanged; zeroCurrentstate→realDeleteFlag/pendingzero; __index changescurrentname BEFOREpostqueryflagcreation; namedsetter queues23from5, same5noqueue, absentstatezeroqueue; aliasedquest_name changesq→other duringLua, queueusesotherindex9/previous3/next-2. Displayincludesplainflags, exact9byte.__status keyplain, embeddedNULdisplayprefix, actualreverseLua negativeindex/name, lookup17/display99 readaftercallback, sortedLIVEfutureinsert/pastskip/futurevalue change/descriptorrebinding actualnetwork. Everycall restores2sentinelLuastackprefix. No C++automaticownedobjects cross a forcedLuaerror/longjmp.

A real defaultQuestState is constructed only as live runningpointer testinput; BOTH scalarcounters explicitlyassigned BEFORE any use, no statecopy/default-mapQuestState insertion occurs. Production defaultconstructor remains untouched/indeterminate. SendFlagList matching branch is fullypresent and tested plan usesactualLua; this is not a flagsonlyimplementation. Unsafe borrowerGC/unrootedLuaStringcollection, nullreturnedstate_name, maperase/clear duringborrowediterator, non-tableLuaerrors, nonfinite/outofrangeindexcasts, concurrentcorruption remain excluded withoutrepair. SDK/reversequerymissbehavior alreadybounded in separatequerysuite, no fullVM/server claim.

## Frozen guards / delayed integration

MANIFEST.json freezes wholePCsource before/afterSHA,5currentreadguards PCheader/QuestState/singleton/CHARsource/header, completeSDKexpectedtree (currently59files:58supplied+projectSDKnote) and everypackagefileSHA. prepare.py produced workspacecandidate/manifest; PythonAST parsed. No currentintegrationplan/dryrunclaim: querymethods/SDK/108registryare stillfuturedependencies while root96validates.

PARENT ONLY after verified108 and no activebuild:

    PY work/pc_state_consumers/prepare_integration.py
    PY work/pc_state_consumers/apply.py
    PY work/pc_state_consumers/apply.py --apply

Deferredrenderer REFUSES missing/driftedSDKfiles, absentrealqueryAPI, stalePCsource/readguard or registrycountnot108. It thenproduces5path plan: actualPCsource,3tests,Makefile. Complete109orderedcanonical registry/executions withnewsuiteonce; readguards include actualfinalmanager.cpp/h and SDKfiles. Neverregeneratefrozenbeforeguards blindly afterrootcompilerfixes; independentlyreview thenrebaseexplicitly. CurrentMakefilefragment references querygroup's realLUA_SDK_LIBRARIES/globalLDFLAGS, no duplicateSDKarchive recipes.

## Validation helper: reuse verifiedSDKO2

    canonical focused pc_state_consumers_regression +query/event/PCrelated +ABI, uniquelog/actualterminalexit
    PY work/pc_state_consumers/validate.py --skip-normal --root "PROJECT_ROOT\Reversed Source" --out "PROJECT_ROOT\Reversed Source\reconstruction\tests\pc_state_consumers_validation" --sdk-o2-dir "PROJECT_ROOT\Reversed Source\reconstruction\tests\quest_lua_queries_validation"

Helper firstverifies queryPROOF_INDEX passed, EVERYreceiptSHA/status0/logSHA,28SDKobjectSHA/uniqueobjects,2archiveSHA, allcurrentSDKsource/header/build guards and28SDKsourcehashes againstoriginalqueryproof. It deliberatelydoesnotrequireoldPCappsourceSHA fromqueryindex toremainunchanged because applyingTHISreviewedconsumer changesPCsource; SDKsubset remains strict. It writes explicitSDKreuse receipt, doesNOTpretendconsumerrebuilt28SDKobjects. No rebuildredundancy, sameactualaux/coreO2archives linked in originalorder.

Then strictC++03/m32actualPCsource/regression/typedABI,5newdistinctO2SQL/nativeOSobserverobjects, actualappgraphO2build/runtime replacing normalSDKarchives with verifiedactualO2SDKarchives. NoLTO; existingactualbuffer.c coreobject normalcompiled, no entireSDK/runtimeoptimizationclaim. Without --skip-normal adds forcednormalmodulebuild/runtime/ABI onlyifcanonicalfocusedproofnotalreadyexists. Need actual9consumerreceipts (8compile/runtime+1SDKreuse withskipnormal) andcanonicalfull109+ABI/exactorderedexecution before3addressmappingpromotion. No full109dispatch/build/proofyet.

RootSDKhelperwasalreadyfrozenwithsdkarchive/objectSHAs for this reuse. Do notmodifyitshelper or fakePROOF_INDEX ifSDKcompilefixesneeded; rootmustregenerateactualSDKproofwithnewguards. Consumerhelper remainsunexecuted, source/testcompilefixes mayberequired nextwindow.
