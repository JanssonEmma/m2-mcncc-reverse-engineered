# PointChange recovery pause checkpoint

PAUSED at parent request (usage97%). No production/status/Makefile edits, builds, ELF execution or child agents. No further work started after pause request.

## Exact state

Full valid reconstructed C++ PointChange source has NOT yet been written. Do not claim this milestone complete or integrate a reduced implementation. Complete saved Ghidra body + compact readable form + FULL original function assembly and exact API/DWARF evidence are saved here, sufficient to resume translation. No production stubs or placeholder definitions were introduced.

Files:
- saved-ghidra.json: complete saved PointChange export from reconstruction/stat-dependency-addressed-ghidra.json.
- decompiled.txt, decompiled-compact.txt: complete body, compact version4991tokens/roughly600lines.
- assembly.txt: ENTIRE original function0812e340,5259bytes,end0812f7cb (not a truncated export).
- symbol.json and calls.txt: exact bounds and all original direct callsites.
- dwarf.json: PointChange signature, complete nested locals/lexicalblock types, packet/e_overtime and missing character/guild API declarations from char.cpp CU.
- logger-quest-dwarf.json: actual LogManager class/method/access and CQuestManager::LevelUp signatures from their own CUs.
- extraction scripts and summary scripts preserve reproducible evidence collection.

Target confirmed in Ghidra first: SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

## Confirmed exact APIs / corrections for next integration

PointChange: void CHARACTER::PointChange(BYTE,int,bool,bool), NONCONST; val local signed32 long. No new default arguments have been inferred. Header declaration currently absent.

quest::CQuestManager::LevelUp(unsigned int), returnsvoid, NONCONST; symbol081a3310 mangles ...LevelUpEj, NOT DWORD/unsignedlong. Original questmanager.cpp488. Missing header declaration/body: stage DECLARATION only until real quest path recovered.

CHARACTER::AutoGiveItem(DWORD,BYTE,int,bool) returnsLPITEM, NONCONST, symbol0817fb20 ...AutoGiveItemEmhib, char.h1061. Missing declaration/body. PointChange quarterstep invokes vnum27001/27002,countBYTE2,rarePct-1,booltrue.

CHARACTER::Save() void NONCONST,08128b50, char.cpp1001. Missing body/declaration.
CHARACTER::SetPolymorph(DWORD,bool) void NONCONST,08135b10,char.cpp5881. Missing body/declaration.
CHARACTER::MountVnum(DWORD) void NONCONST,08136010,char.cpp6020. Missing body/declaration.
CHARACTER::ComputePoints() void NONCONST,char.cpp2086; transitive SCC via mount/poly, not directly called by PointChange export.
CGuild::LevelChange(DWORD,BYTE) void NONCONST,0808f5d0,guild.h187. Missing declaration/body.
LogManager::CharLog(DWORD,DWORD,DWORD,DWORD,const char*,const char*) void NONCONST,080785a0,log.cpp58. Missing declaration/body.
LogManager::Query(const char*,...) void081? actual08078460 body already recovered in production log.cpp. CURRENT log.h declares it PRIVATE, but original LogManager class DWARF has Query public default(noaccess attribute), while m_sql/m_bIsConnect explicitly private3. Real PointChange calls Query externally; move existing declaration to public when integrating. This is a header recovery correction, NOT a new Query implementation/friend hack. logger-quest-dwarf.json proves it.

Already actual callees: SetLevel/GM/PK, GetName, GetJob/RaceToJob, GetPoint/SetPoint/GetRealPoint/SetRealPoint, IsDead/IsStun, SetNowWalking, BroadcastTargetPacket, actual party RequestSetMemberLevel and SendPartyInfoOneToAll, real DESC::Packet/CEntity::PacketAround, ChatPacket, LC_IsNewCIBN, coreMIN/number_ex/get_dword_time/sys_log/_sys_err/dev_log, get_table_postfix. Query actual SQL path exists. ComputeBattlePoints full candidate staged in work/battle_points but PC branch requires this actual PointChange SCC. ComputePoints/mount/poly/Save/AutoGive/questLevelUp/guildLevelChange/CharLog remain genuinely unrecovered; no stubs may be introduced to link a claimed full SCC.

## Data / signed arithmetic proof

Existing packet_point_change/TPacketGCPointChange is correct packed17 bytes: INT4 header, DWORD4VID, BYTEtype, LONG4amount, LONG4value. header assigned17; do NOT change to a BYTE header/14-byte record. Existing full CHARACTER data and e_overtime0/1/2 already declared.

EXP local exp and next_exp are SIGNED int32 (DWARF), despite m_points.exp raw unsignedDWORD. Assembly compares signed jl/jg and wraps add/sub in32-bit registers. Amount negative underflow guard compares signed prevEXP < wrappedNeg(amount). Preserve INT_MIN unary-neg wrap explicitly; do not introduce C++ UB. Quarter nextEXP/4 signed trunc towardzero; clamp local step to4 when GetRealPoint25>3 and emit original error, NOT a lowerbound clamp. Balance int32 can be nonzero/negative after wrapping and must recursively call EXP exactly.

exp_table is mutable pointer symbol0832328c, currently NOT in production constants.h/.cpp. Existing full arrays/pointer evidence in reconstruction/stat-dependency-old-data-metadata.json and stat-dependency-RECOVERY_PLAN.md: exp_table_euckr0821a100 and exp_table_newcibn0821a440, each400bytes. Locale selection remains pending; don't define a fake/fixed pointer. Candidate should stage original declaration with provenance separately; reference supports constDWORD* mutablepointer. Original code loads exp_table[level] only level<99, otherwise2000000000. Read table/level again where assembly does; no hidden cache across callbacks.

Use a self-contained point_change_detail add/sub/mul/neg modulo32 namespace or already staged battle_points_detail. Signed divisions by2/4/100, unsigned playtime tick difference/60000, all wrap semantics must be explicit. Avoid signed overflow in level/gold/EXP/HP additions or max-value subtraction.

## Branch/call invariants established (retain all in source)

Type0 returns with no side effects. Level1: upper signed wrapped sum guard>99 only (no lower bound); SetLevel byte trunc; cache val from postSetLevel; log LEVELUP; recursive NEXT_EXP4(false,false); amount!=0 triggers questLevelUp, realplaytime/ticks/Query SQL, guildLevelChange ifguild and partyRequest ifparty. At end ComputeBattlePoints for originaltype1/12/13/14/15 only.

Types2 and24 return GetRealPoint only. Type4 returns GetNextExp policy and forces bAmountfalse. Type3 EXP has LC_NewCIBN overtime0none/1half signed/2zero + original dev_log; negative-underflowreset; maxlevel>=99 return unless underflowreset path; testserver expchat; signed sum/next compare; quarters1/2/3/4 with exact recursive LEVEL_STEP25 flags (firstquarter usesbroadcastfalse; others true); balance recursion; final rawexp packetvalue. PRE EXP log has4formatargs. EXP log format has3args; Ghidra shows a fourth stale stackslot from priorPRE log, but assembly stores only format/name/exp/nextexp before that call. Next translation should omit an invented semantic extra argument and document stale variadic inference.

HP5: dead/stun return; MIN(maxHP-oldHP,amount), raw store, cachedval, BroadcastTargetPacket unconditionally (even amount0), partyInfo only party+desc+cachedval!=oldHP. Outer HP packet suppressed iff adjustedamount0. SP7 similarlydead/stun+upperMIN but no target/party. Stamina9 dead/stun; upperMIN; zero->SetNowWalkingtrue, priorzero/nonzero->restore m_bWalking; ifamount<0 andnewstamina!=0 return after changes WITHOUT pointpacket.

MaxHP6 stores additive point modifier then recomputes instant max from realbase + MIN(1500,wrapped(point119*base)/100) + currentpoint6 + partybonus92. MaxSP8 cap800 and percent120 + partybonus104. Maxstamina10 simply changes instant maximum (no realpoint/modifier sync). No HP/SP lower clamp added.

Gold11: signed wrapped oldgold+amount>=2000000000 triggers _sys_err+actual CharLog overflow; gold not changed and packet val remains initial0 (do not replace with currentgold). Positive gains only receive NewCIBN overtimehalf/zero/devlog AFTER overflowguard. Negative gains bypass Gold overtime. Overflow branch reads currentgold again AFTER _sys_err before CharLog, so retain callback ordering.

Type16 DEF: SetPoint(type,GetPoint+amount), cacheval, RECURSIVE PointChange20 SAMEamount(false,false). This corrects prior battle TEST_PLAN golden clientdef assumption: full realDEF changes clientdef too, so resulting clientdef is NOT merely0/5 formula difference. All laterDEF bonuses also add toclientdef. Parent was notified; previous battleplan explicitly required auditing realPointChange.

Type25 LEVEL_STEP: nonpositiveamount doesNOT mutate, returns GetPoint25. Positive amount adds GetPoint25; sum1..3 and currentlevel<91 grants ONE stat26 recursively (not amount-many); sum4 rollsHP/SP using FOUR actual GetJob calls (upperboundjob first, lower second pernumber_ex), updates randomSP THENrandomHP; ifskillgroup nonzero andlevel>4 grant28, reloadlevelthen>8 grant27; recurseMAXHP6,MAXSP8; localstep0; recurseLEVEL1 amount1(false,true). Other sums (including >4 or wrappednegative) skip growth but STILL potion/fill/save path. After growth/no-growth choose vnum27001 if CURRENTlevel<11 else27002; AutoGiveItem count2,-1,true; refill HP5/SP7/stamina9 from fresh current fields in that sequence; SetPoint25/localstep,SetRealPoint25,Save. Packetvalue cached localstep.

Types26/27/28/112/113 additivepoint + mirrorRealPoint. Types83..86 and114..118 cap only upper100, using wrappedsum then diagnostic and reread currentpoint to adjustamount=100-current; no lower clamp. Error lines3355 and3333 respectively. Types88/89/90 additivepoints then clear/set immune flags1/2/4 on result==0/nonzero (negative counts stillset).

Types95/96/97/98 update bonuspoint then recurse realgrade18/16/22/23 sameamountfalsefalse and return ownpointvalue. Type108 changes point then realSetPolymorph(newvalue,false). Type109 changes point then MountVnum(newvalue). Percent119/120 additivepoint then recurseMAXHP6/MAXSP8 amount0 falsefalse; outerpacket uses ownpctcachedvalue. Types125/126 are ABSOLUTE SetPoint(amount), not additive. Remaining exact simpleadditive typegroup must be transcribed from compactbody and crosschecked original jump table127DWORDs at08224524; sourcebyte>126 defaultunknown. Do not collapse missing/unknown cases into accumulator.

Packet epilogue: only if originaltype!=HP5 OR adjustedamount!=0, AND currentdescriptor exists; set header17, VID current, originaltype, amount=bAmount?adjustedamount:0 (NEXT_EXP forcedfalse), cachedval; bBroadcast sends actual PacketAround(...,17,0), otherwiseactual DESC::Packet(...,17). Derived grade recursion produces childpackets first. Unknown type errors PointChange3445 with GetName/type thenreturn; type0silentreturn.

## Next exact work (not performed)

1. Build direct-callgraph.json from full assembly, and decode switch jump table08224524 for ALL127 entries to verify groups.
2. Translate complete compact export to compilable C++ with explicit wrap32, preserving every branch/callback order above. Use full existing class records. No app definitions for missing callees; separate ORIGINAL declaration additions only.
3. Reconcile case groups, variable caching and side effects against assembly; record any unresolved ambiguity instead of inventing behavior. Pay particular attention to EXP logs/stepbool flags, Goldoverflowval0, HP cachedval vs callback mutations and level logging.
4. Stage dependency closure/testplan. No fullruntime green claim until actual quest/SQL/guild/save/autogive/mount/poly/ComputePoints/ComputeBattlePoints SCC dependencies are genuinely recovered. Bounded tests can later exercise meaningful actual closed paths with explicit external observer/lifecycle boundaries, neverfakePointChange.

Prior workspace package work/battle_points still contains fullbattle candidate and independent helper4 split. Parent reported helper4normal/O2/strict32 green before this task. Parent was integrating helperfull70/targetwalking while this bounded read-only recovery ran.
