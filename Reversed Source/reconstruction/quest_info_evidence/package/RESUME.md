# Bounded PC quest-info/title preparation — SAVE/STOP, no integration or pass claim

Current parent verified92/full102. Workspace-only metadata, fullbody candidates, exactclosure and nexttestplan. NO Fwrites/builds/targetexecution/status/coverage updates. AppliedPCstorage payload untouched. No broadVM/DoQuestStateChange/reward work. Thirteenoriginal exports complete/untruncated in evidence/quest-info-addressed-ghidra.json,13exhaustivecallgraphs, originalstaticassembly, metadata.json. GhidraFIRSTlivecall confirmedoriginalSHA61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

## Complete coherent next group

questpc.cpp.candidate contains ENTIRE13oldquestpc.cpp bodies: SetCurrentQuestStartFlag0805efc0, DoneFlag0805eff0, ClearBegin08060400, SetBegin08060430, GetBegin080604b0, SetCurrentTitle080604d0, SetQuestTitle08060510, ClockName08060650, ClockValue08060690, CounterName080606c0, CounterValue08060700, IconFile08060730, SendQuestInfoPakcet0805f020 (ORIGINAL typo). Exact original alreadydeclared61API qualifiers retained: NONCONST, void setters/stringconstrefs/int counters, GetBeginINT. No partialsender or fakegetter/packetbody.

Datafirst: packet.h.additions.candidate FULLpacked6byte packet_quest_info originalpacket.h: BYTEheader0, WORDsize1, WORDindex3, BYTEflag5. Old index is WORD16, not modernDWORD. metadata.json preservesexactfieldtypes/offsets/size/declaration placement. Existing fullPC/QuestState records unchanged, no reduced/context model.

Sender genuine neededcontextgetter: questmanager.h.additions.candidate publicNONCONST CHARACTER* GetCurrentCharacterPtr(){return m_pCurrentCharacter;}, originalquestmanager.h122, proveninlinecaller0805f12c..138 and rawload0805f135 +0x7c. Complete current CQM fieldalreadyactual. This is a real original accessor, no fallback toPC ID/managerGetPC or fakecontext. No standaloneoriginalexport/status invented.

## Exact closure

dependency-matrix.json recordsEVERYoriginalcallee/callsite/currentactualsourceanchors+SHA andimplicitgetter/dataedges. All substantivecallee paths alreadyactual: PC::SetSendFlag OR, fullmanager GetQuestIndexByName query/hash, currentQuestState/map/string storage, full TEMP_BUFFER ctor/dtor/write/size/read_peek andBUFFER_MANAGER/Cbuffers, actualCHAR GetDesc + DESC::Packet, realcorelogging/assert. SetQuestTitle calls othercompletebodiesingroup. No LuaVM/name-to-state resolver/reward/contextmutationmethod needed. Conceptually closed AFTER genuinecontextgetter/packetmerge, but NOT integration-readyverified: no guardapply/testfixture rendered, no normal/O2/strict passes. This bounded task stops at fullcandidates/data/provenance/testplan.

ClearSendFlag is onlyDECLARED in oldmetadata/currentheader, no emittedfunction orinlinebody found. Sender hasdirectfieldzero at0805f14b. DoNOTinvent ClearSendFlagbody/status fromunuseddeclaration; it is not required by thisgroup. No extra unrelatedmethods added.

## Critical old behavior preserved

GetCurrentQuestBeginFlag returns m_RunningQuestState->iIndex, NOT bStart, and0ifnostate. Start queriesBeginINDEX; nonzero (includingnegative) returnswithoutchange evenifbStartfalse. Index0 causes SetBegin everytime, evenifbStartalreadytrue. SetBegin actualnamequery->CURRENT running iIndex (rawuinttoint), bStart=true, THEN sendflagOR1. Done queriesINDEX; zero isno-op, nonzero callsClearBegin: sendOR1 BEFORE bStart=false, indexNOTcleared, repeatedDone stillsetsflag. Startwithoutstate eventuallydereferencesnull; no modernIsRunningguard added.

Title/clock/counter/icon setters firstOR exactmask, THEN dereferenceCURRENT state/assignfullstringorint. Noclamping/truncating/padding/extraSend calls. Setterstringassignment/allocatorcallbacks can affectlaterreadcontext; no savedstatepointer introduced. Counterints untouched untilsetter, sendercounterbranches requireinitializedvalues; no constructorzero repair.

SetQuestTitle queriesname/index, findonly(no insertion); missreturns. Onhit cachesOLD runningpointer/sendmask BEFORE copyingoldquestname; assigns LIVE name tocurrentname, mask=2, running=&existingrecord, copiesGetBeginINDEX intoexistingrecord.iIndex (usuallyselfvalue, notqueryindex), callsCurrentTitle and fullSender, then restoresoldname, oldpointer, oldmask. Callerargaliases remainlive, no early snapshot. Oldname destructor AFTERrestore. Sendercallbackcontextchanges are overwritten byoriginalrestore; unsafe erasedrecord/argumentalias domains not hardened. Unknownname0canaddressalready-existing0state.

## Sender wire/read ordering and unsafe domains

Originalasserts nonzeroSENDMASK andnonnullrunningstate, no descriptor/context guard. Header81, WORDindex truncation, BYTEflag truncation fromINITIAL rawmask, size6. Headerfieldsassigned BEFORE actualTEMP_BUFFER constructor andfirstwrite. Subsequentseven IFs read CURRENT rawmask afterpriorbuffer/logger/allocatorcallbacks, whileheaderflag remainsoriginalcachedbyte. Each selectedfield: currentstate afterreserve whereapplicable, writefixedbytes, localqi.sizeadd, thenlogFRESHcurrentstatefield, allowingcallbackchangestoaffectlaterbranches. Fieldorder begin1/title31/clockname17/clockvalue4/countername17/countervalue4/icon25. No validmaskfilter: rawhighbits/nonzero256 canemit headerflag0/bodyempty.

IMPORTANT: the6byteheaderiswrittenONCE (0805f07c). qi.size incrementsONLYlocalstackcopy; NO patch/rewrite intoTEMP_BUFFER. Packetwire.size STAYS6 evenwith105actualbytes(maxall7fields). Verified805f118 size(),805f125read_peek(),805f146 DESC::Packet, noheaderpatch. DoNOTmodernfix it. Sourcepreservesalllocalqi.sizeincrements too, since originalstackobjectaddressescapes throughbufferwrite/memcpyobserver.

Strings use reserve(31/17/25), notresize/zero-fill; fixedwidthread mayinclude stale/uninitializedcapacitytail forshortstrings. No deterministicpaddingclaim orrepair. Longstrings sendexactfixedprefix, maylackNUL. Fieldslogs fullCstr afterwrite, possiblydifferentstateaftercallback. Readonly initializeddomain testing should use strings length>=N-1 soalltransmittedbytesinitialized (exactN-1includesNUL, >N-1doesnot). Indeterminatepadding and allocatorABI differences remainexplicitlimitations; no unsafechildread executed.

Atend cachesactualbuf.size BEFOREread_peek, thenreadsCURRENT CQM character/desc, callsactualPacket(data,bytes), clearsENTIREmask0 AFTERpacketreturn (callbacksetbitsalsocleared), thenTEMP_BUFFER destructor/poolrelease. No PCIDbased contextlookup. No apppacket/statfake. Assertdiagnostic filenames/hostmacrotext differfromoldFreeBSD; noassertabortpassclaimed.

## Enum/type evidence boundary

All7USEDmaskvalues are instructionproven 1/2/4/8/16/32/64 (methodimmediates/senderbit tests). Modernreference has anonymousPCenum namedQUEST_SEND_ISBEGIN/TITLE/CLOCK_NAME/CLOCK_VALUE/COUNTER_NAME/COUNTER_VALUE/ICON_FILE. Originalscans ofquestpc/questmanager DWARF ALLtags foundNONAMEentries forQUEST_SEND_ (send-mask-metadata.json empty), despitefullclass/APIdebug. Originalenum/type/names thereforeNOTclaimed orpromoted fromreferenceonly. mask-values.json separates provenvalues fromreference-only names. Candidateusesnumericoldimmediates, no fabricatedpublicenum orstandaloneaddress. FuturebroaderDWARF/sourcecorrelation mayresolve names; doesnotjustifyguessingnow.

## Next meaningful regression when allowance resets

UseactualfullPC/state/manager/CHAR/DESC/network/bufferpool graph andexisting explicitunrelatedowning-Lua/NPC fixtureboundary (L0/NPCempty). ConstructREAL BUFFER_MANAGER singleton (oldpoolrequired), real CQM with currentcharacter borrowedpointer, realCHAR withboundDesc/output buffer; clearborrowedcontext beforeteardown. No fake GetCurrentCharacterPtr/SendQuestInfoPakcet/TEMP_BUFFER. Directfullrecord fieldassignment maysetinputcontext, allobjects/constructorsreal. QuestStates counters explicitlyinitialize beforeselectedcounterbit; defaultindeterminatefields notread. No AArgScriptctor needed.

Cover all7individualbits/all127combinations and rawhigh/negative masks; literalheader.size6 vsactualbytelen, WORDindex -1/highbit/max truncation, BYTEflag capture, initialheaderbeforeTEMPconstructor callback, fieldorder/31/17/25prefixes +signedINTMIN/MAX. Strings exactN-1/N/>N withknowninitializedcontent only. Logger/copy callbacks switchrunningstate/sendmask/currentcharacter toprove freshlaterreads/cachedheader anddescriptorlatebinding, thenPacketcallbacksetsbitsproving finalzero. Bufferpoolrelease observedafterzero, no reallocationcountassertions acrossold/newSTL. DifferentactualcontextcharacterreceivespacketevenPCIDdifferent.

Begin getter null0/iIndex0/nonzero/bStartboth; unknownnameindex0 andhighbits; repeatedStart/Done quirks; settersORexistingmasks/noSend effects/fullstringcopy/counterextrema; invalidnullsetterdomain staticonly. SetQuestTitle mapmissnoinsert/zero-keyhit, savedcontextrestore includingoldmask, titlealias/mutableinput, existingindexstaysold ratherthanmanagerkey, sendcallbackmutatescontextbutrestorewins. Fullrealpaths only. SeparatetypedABI packet6/offset0,1,3,5 andnonconstmethods, no oldSTLwholeclasssizeof. No tests written/run inthisboundedpreparation.

## Resume/stop

Saved allcompletecandidates/exact13callgraphs+metadata/proof andcurrentdependencySHA. No appliedstoragepayload edits, integration/coverage/statusor runtimeclaim. Parentfuturework shouldreview/prepareguardedgroup+concreteregression andnormal/O2/strict/fullbaselinevalidation; no broadVM/reward expansion. This task intentionally stops promptly toreserveweeklyrootstate.
