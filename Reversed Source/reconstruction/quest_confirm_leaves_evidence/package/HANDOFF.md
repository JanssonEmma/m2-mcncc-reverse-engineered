# Confirmation output/style leaves — prepared100 only

Original SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c confirmed live get_program_info before boundedreadonlyexports. Whole CHAR::ConfirmWithMsg08136310 (35completeinstructions/2calls) and CQM::SetSkinStyle081a5400 (12/0), alltruncatedfalse/has_morefalse. No originalELF execution/modification, Fmutation or compiler/build by this agent.

## Exact original ownership/data/source

ELF_API_PACKET_AUDIT.json proves rawsymbols _ZN9CHARACTER14ConfirmWithMsgEPKcim and _ZN5quest13CQuestManager12SetSkinStyleEi, constchar*message/inttimeout/DWORDunsignedlong32 API, nonconstmembermethods, originalchar.cpp/questmanager.cpp owners. CompleteexistingCHARdata/virtuals and CQM216data remainintact; onlynonvirtualpublicAPI additions. No guessedfreehelpers, selectorenum or dispatcher skeleton.

Original packet.h812 POD packet_quest_confirm /TPacketGCQuestConfirm is EXACT74packedbytes: BYTEheader@0,charmsg[65]@1,signedlong32timeout@66,DWORDunsignedlong32requestPID@70. Fullrecord retained, no padding/field omission/new ctor. NativeMinGW LLP64 long/DWORD4 and strictm32 asserted; LP64intypewidening intentionallyrejects, not silentlyreplaced. Numericheader46 proven, no inventedpublicenum.

ConfirmWithMsg firstchecksDESC and returnsbefore ANYmessage use ifnull. NPC/monster withvalidDESC stillgetsconfirmation: no IsPCguard. Header/PID/timeout storedBEFORErealstrncpy64, explicitmsg[64]NUL, then FRESHdescriptorloadaftercopy and actualDESC::Packet(pointer,sizeof74). No timer/queststate/VM operation, validation, timeoutnormalization or stylechange. Shortmessageszeropad64; longmessagescopyfirst64; nonNULexact64accessiblebytes VALIDboundedinput. NULLmessage onlysafeondescriptorabsentpath; unboundNULLsourcewithliveDESC excluded withoutrepair.

SetSkinStyle is NOTunboundedstore: originalunsignedcomparison valid0..5 storesinput, EVERYnegative/6+/INTMIN/MAX fallback1. No modulo, signed-only<6predicate, clamp or unrelatedownerchange. Publicsetter selection observed through actualSendScript; sourceSendScript rawBYTEcast itself remainsunchanged.

## Actual graph regression prepared (UNCOMPILED)

quest_confirm_leaves_regression.cpp uses fullactualCHAR/DESC/CQM/PC/script/pool/events and existing explicitnativeOS/unrelatedlifecyclefixtures, no fakepacket serializer or Classmethodreplacement. Cases: NULLmessage/nodescriptor; NPCtype0withDESC;175(7message×5signedtimeout×5PID) cases covering0/1/63/64/65/100length, embeddedNUL andINT/unsignedhighbits; exact64nonNULrawarray; copycallback mutatingoriginalinput/rebindingCHARdescriptor AFTERpacketconstruction; currentcall stayscapturedDESC, NEXTcall usesfreshbinding. Styles INTMIN/-100/-1/0..7/65535/INTMAX verify0..5orfallback1 and no script/context/flags/VMmutation; actualSendScript protocolskinmatchesselection thenClearScript resets1.

Descriptorre-read BETWEENguard/strncpy is originalSTATICinstruction proof; tests doNOT interpose libcstrncpy or claimruntimecallback there. Callback test is insideactualDESCcopy, proving fullyformedpayload and alreadycapturedcallreceiver +nextinvocation'sfreshlookup. Everypacketbyte defined; no borrowedstackpointer retained afterreturn. Allsourcecallbacks/inputs valid; no null/freedDESC/rawmessageoverrun unsafeexecution.

ABI checks74bytes/offsets0/1/66/70/msg65/long4/DWORD4 and exact2memberfunction types. No wholeSTL/nativeCHARsize gate. No completeconfirmation/VM/serverequivalence claim.

## Guarded integration / proof plan

integration/plan.json9paths69readguards from currentapplied112:char.cpp/h,manager.cpp/h,packet.h,3tests,Makefile. ExactlyONE newcanonical quest_confirm_leaves_regression ->113, orderedregistry/execution/newonce staticallychecked. PythonAST and defaultapplydryrun PASSED9/69; --apply NEVERcalled. Independent save_review source+35/12instructions/testdomain reviewPASS, reportoutsidefrozenpackage work/quest_confirm_leaves_independent_review.json. p.msg[64] literal actualbytes quote/ONEbackslash/zero/quote; serializedrepr doublesescaping but sourcecorrect, testbyte65mustzero.

Waitroot99FULL112terminal/record/noactivebuild, thenreviewguards/diffs:

    PY work/quest_confirm_leaves/apply.py
    PY work/quest_confirm_leaves/apply.py --apply
    canonicalfocused confirmleaves/lifecycle/relatedCHARpacket +ABI, uniquelog
    PY work/quest_confirm_leaves/validate.py --skip-normal --root "PROJECT_ROOT\Reversed Source" --out "PROJECT_ROOT\Reversed Source\reconstruction\tests\quest_confirm_leaves_validation" --sdk-o2-dir "PROJECT_ROOT\Reversed Source\reconstruction\tests\quest_lua_queries_validation"

Helperprefixquest-confirm-leaves: SDKreuse(noncompile,statuspassed,noexit_code) verifiesoldqueryPROOF_INDEX/receipts/logs/28objects/2archives/current59SDKguards; strictC++03/m32actualCHARcpp+manager.cpp+questlua.cpp/regression/ABI with existingSQLshim/MySQLinclude/sql_socket_platform flags;5separateSQL/observerO2objects; fullactualappgraphO2build/runtime +verifiedSDKO2archives/noLTO.8realcompile/runtimeexit0receipts +1SDKreuse, PROOF_INDEX links. Actualbuffer.c coreobject remainsnormalcompiled; no entireSDK/VMoptimization claim. Full113+ABI/exactorderedcommands/terminalexit required before2boundedmappingpromotion. SuppliedLua5.0.3 SDK externalrole remains not provenold5.0internalABI/sourceequivalence.

## Confirm/RunState frontier (NO partialsource)

Saved FRONTIERdecompile/calls for confirm_timeout_event0805c650,CQMConfirm081a1ff0. ExistingfullGotoConfirm0805c720/RunState0805cde0 evidence in work/quest_yield_bridges. Wholecycle: GotoConfirm→actual8byteeventinfo/timer→timeoutcallback→Confirm→RunState→GotoConfirm; completedConfirm→CloseState→PC::EndRunning0805f4d0→Reward/DoQuestStateChange→PointChange/AutoGiveItem/NPCdispatch. Confirm immediatelydereferences GetPCresult (originalnullhazard), no inventedguard or timeout-onlypartialshortcut. Eventcallback onlychecksWaitPIDcharacterexistence then callsConfirmtimeout; ReplyPIDunusedthere. FullRunState cannot be linked by omittingconfirm/completion branches. No RunState/Confirmbody/prototype/enum promoted here.

ExtraNPClookup exports saved asFRONTIERONLY: GetCurrentNPCCharacterPtr081a5700→missingCHARGetQuestNPC08134f50→actualCHAR_MANAGER::Find; GetCurrentNPCRace081a5660 callsgetterTWICE thenactualWORDGetRaceNum. They are NOTpartofthis2methodintegration/maps. ParentseparateownershipNPC/CQMdtor work belongs save_review; no overlapexports or guessedNPCcopy/defaultctors here.

MANIFEST freezes package/evidence/payload/sourceSHAs. Rootcanfixonlyactualevidencedcompile/runtimeissues, retainingfailureproof/backups; no currentcompiler/runtimepassesclaimed by agent.
