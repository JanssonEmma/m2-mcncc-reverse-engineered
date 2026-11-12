# CQM coroutine registration and script output — prepared98 only

Target SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c confirmed live Ghidra get_program_info before read-only exports. Complete6original decompiles/assemblies/callgraphs retained: OpenState0805cd80(28instructions/3calls), CloseState0805d000(22/1), GotoEndState0805cd10(34/4), ClearScript081a4ee0(21/1), AddScript081a4f30(7/1), SendScript081a4f50(122/15). All has_morefalse/truncatedfalse. No originalELF execution/modification, Fwrites, compiler/build/runtime passes by this agent.

## Decisive owner/file/API proof

ELF_OWNER_AUDIT.json reads RAW ORIGINAL ELF symbols plus low_pc DWARF/artificialthis. Low0805 methods ARE quest::CQuestManager members in original questlua.cpp, not freequest.cpp functions. Manglednames explicitly _ZN5quest13CQuestManager9OpenStateERKSsi /10CloseStateERNS_10QuestStateE /12GotoEndStateERNS_10QuestStateE. Original lowpcDIEs declare questlua.cpp2136/2214/2124 and artificialthis typedCQM*. Address-neighborhood/freefunction heuristic was raised by root and disproved; no speculative sourcechange/getter was made. Preserve this audit with restart/evidence to avoid repeating the wrong ownerguess.

New originalowner game/src/questlua.cpp.candidate contains completeOpen/Close/GotoEnd only, no fakeRunState/VM/startup methods. Existing game/src/questmanager.cpp gains completeClear/Add/Send. Header adds originalpublic OpenState(conststring&,int)->QuestState, CloseState(QuestState&), Clear/Add/Send; GotoEndState(QuestState&) is PRIVATE (DWARFaccess3), not public/newfreehelper. Full216byteoldCQMdata and priorAPI remainintact; no GetLuaState getter invented. Original L member at0x34, directmemberloads supported by actualinstructions and fullrecord; no rawhostoffset emulation.

## Whole behavior preserved

OpenState: unusedquest_name, actualQuestState defaultconstructor→args0→stinput→lua_newthread(CURRENT L)→co→luaL_ref(CURRENT L,LUA_REGISTRYINDEX=-10000)→ico→return. L is reread for ref, no cachedparent tiedtoco, no quest lookup/script execution/extra initialization. Original assembly constructsDIRECTLYcaller returnstorage (sret) and performs NO defaultstatecopy. Source namedlocalreturn permits optionalNRVO; originaldefaultclock/counter remainINDETERMINATE. Do not zero counters, addfakecopyctor or changebyvalueAPI. Runtime OpenState deliberatelySTATICONLY because sourcecopy-elision portability not proven; fullbody compiles with others. Test manuallyconstructing actualSDKthreads is CloseState INPUTsetup, not a replacementOpenState or fakeapplicationmethod.

CloseState: co is only guard. Ifnonnull, actualluaL_unref(CURRENT owner L,registry,ico), THENcoNULL. Ifnull, noLuaAPI evenownerLNULL. Retainico,args,st,index,start,strings,chat_scripts,counters and allother fields. No lua_close, directthreadfree, vectorcleanup, identitycheck, reset or errorguard. RealSDK refsetup/unref tests prove registryrelease, idreuse, currentL B clearingBregistry evenco fromA, negativeLUA_NOREF no-op followedbycoNULL. They avoid freelistcorruption/invalidVM contexts. Allstatefieldsinitializedbeforetests, no defaultstatecopies occur.

ClearScript clearsfullstring BEFOREskin1; leavesm_bNoSend/m_bError/PCdone/context unchanged. AddScript fullstdstringappend supports selfalias/embeddedNUL usingcomplete nativeSDK. No publicskinenum invented; code-backed0/1 constants retained.

SendScript: m_bNoSend true→false→ClearScript→return, noPCdone consumption. Exact["[DONE]","[NEXT]"] trigger GetAndResetDoneFlag onlyifcurrentPCnonnull. SuppressONLYDONE+PC+previousdonefalse+skin1+!error; consumesdonefalse, ClearScript thenreturn. Allothertriggercases skin0; ordinarystrings retainoriginalskin. No branch narrowed, no fakePCdone helper.

Complete original packet_script packet.h1279 has6packedbytes: headerBYTE0,sizeWORD1,skinBYTE3,src_sizeWORD4. Header45/skinBYTEtrunc/src_sizeWORDtrunc are captured BEFORE actualTEMP_BUFFERconstructor; sizeWORD(src_size+6) assignedAFTERconstructor. Headerwrittenonce. Payload length is freshlyread fullstringlength AFTERheaderwrite, BEFORE nonconst &m_strScript[0]/oldCOWunshare role, then actualpayloadbyteswritten. Headerlengths are NOT patched after callbacks or16bitwrap. Cachedbufsize precedesread_peek; currentCHAR/DESC loadedlate. ActualDESC::Packet completes BEFOREfreshtest_server check/debugusingLIVEscript andbufsize; ClearScript thenTEMP_BUFFERdestructorpoolreturn. Callback-addedscript is cleared; callbackNoSend remainsbecauseClearScript onlystring/skin.

Nonconstoperator[](size) on UNSUPPRESSEDEMPTYstring is original C++03 unsafe domain. Fullbody preserves &str[0], no c_str workaround. Runtime excludes that domain, tests emptyNoSend earlyreturn only. Wordheaderwrap/fullpayload for65535/65536/65537 retained (no clamp). Other undefined string/map/context/allocator lifetime domains excludedwithoutrepair. RawSDKcopy/unshare implementations differoldGCC3 COW; no binarySDK/equivalence claim.

GotoEndState ignoresQuestState&, appends original[DONE] temporary, destroys temp before realSendScript. It doesNOTCloseState, resetqs/co/suspend, save/reward/EndRunning or executeVM. Test uses live actualSDKregistered coroutine and verifiesitremainslive after prefix[DONE] output; actualCloseState laterreleasesit.

## Regression / ABI prepared, UNCOMPILED

quest_state_lifecycle_regression.cpp uses actualSDK lua_open/newthread/ref/unref/registry API and TESTownsVMlifetime (manager.L0 beforeexistingowningLua/NPC destructor boundary; actualSDKlua_close teststates). Uses completePC/CQM/CHAR/ChatPacket/DESC/TEMP_BUFFER/BUFFER_MANAGER/real events and existing explicitnativeOS/unrelatedlifecyclefixtures. No shadowclass, reducedrecords or applicationfakes. Existing staticBUFFER_MANAGER used, neversecondSingleton. Prime actualpool with defined131072bytewrite toavoid observerreallocation ambiguity.

Runtimeplan covers5 of6 methods dynamically plus staticOpenbody/API: CloseNULL/currentL/negative/idreuse/allfields; Clear/Addbinary+selfalias;240DONE/NEXT/ordinary×PC×done×skin×error×drop combinations; WORDwrap/fullbinarypayload; headercallback changingpayload aftercachedheader, payloadcallback changinglateCHARreceiver, GCcallback changingcurrentScript/skin/NoSend/test_server, debugcallback changingScript/skin, finalClearandpoolreturn; emptydrop; GotoEnd retainsrealregisteredcoroutine. Observers deep-copy beforemutatingborrowedCstring; no stackheaderpointer retained afterSend returns. Actualregistry afterunref may be freelistnumber ratherthannil, tests use lua_type==LUA_TTHREAD (Lua5 has no lua_isthread macro; preparationcorrectedTESTpredicateafterheaderread, no sourcefix).

ABI: full6bytepacket offsets0/1/3/4, WORD2/BYTE1 and exact6original nonconstmemberfunction pointer signatures inclprivateGotoEnd(ref) and byvalueOpen. No wholeSTL/class/nativeLua internal-size gates. SuppliedactualLua5.0.3 externaldependency is not claimed exactoldadvertised5.0 or originalinternalABI; priorpublicopaqueAPI policy remains.

## Guarded application / actual proof required

integration/plan.json8paths+66supportguards from currentapplied109source: manager.cpp/h, NEWquestlua.cpp, packet.h,3tests,Makefile. ExactlyONE new canonical suite quest_state_lifecycle_regression ->110, orderedregistry/execution once. Onlynewtest sourcegraph adds actualquestlua.cpp; broadRunState/InitializeLua/NPC/Reward remainpending. BeforeFwrites waitroot97full109terminal/record, inspectdiffs, verifyallguards; neverforce stalepayload.

prepare.py workspace-only renderer +PythonAST parsed. apply.py DEFAULT dryrun PASSED8paths66dependencies; --apply notcalled. save_review independentwholemethod/asm/testdomainreview foundnoactionablesemanticmismatch beforefinalfreeze; requiredexplicitCQMcppincludes char.h/desc.h/buffer_manager.h/packet.h supplied in renderedpayload.

    PY work/quest_state_lifecycle/apply.py
    PY work/quest_state_lifecycle/apply.py --apply
    canonicalfocused quest_state_lifecycle_regression +relevantPC/Lua/network +ABI withuniquelog
    PY work/quest_state_lifecycle/validate.py --skip-normal --root "PROJECT_ROOT\Reversed Source" --out "PROJECT_ROOT\Reversed Source\reconstruction\tests\quest_state_lifecycle_validation" --sdk-o2-dir "PROJECT_ROOT\Reversed Source\reconstruction\tests\quest_lua_queries_validation"

Helper reuses VERIFIEDactualSDK28O2objects/twoarchives aftereveryqueryreceipt/log/object/archiveSHA +current59SDKsource/header/buildguard validation. It doesnot rebuildSDK or claimconsumerbuiltit. StrictC++03/m32actualmanager+questlua+regression+typedABI;5distinctSQL/observerO2objects; actualfullappgraphO2/noLTO +SDKO2archives; runtime; explicitSDKreuse receipt (8compile/runtime+1reuse withskipnormal). Existingactualbuffer.c normalcoreobject remains, no entireSDK/runtimeO2claim. Then canonicalfull110+ABI/exactorderedexecution/terminalexit before6boundedaddressmappings. OpenState runtimeSTATICONLY noted in coverage, no fullVM/server/returncopyportability inference.

MANIFEST freezes payload/evidence/source guards and exactownerproof. No actual98compile/runtimeproof yet; fixonlyevidencedminimalissuesafterparentrun, retainfailedreceipts/backups if any. Original ELF remainsunchanged/unexecuted.
