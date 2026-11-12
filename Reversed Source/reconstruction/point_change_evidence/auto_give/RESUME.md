# AutoGive recovery checkpoint
Stopped at parent request, usage92%. No builds or production edits. All staged evidence is under this directory; no integration-ready AutoGive C++ source exists yet.

## Evidence saved
Original ELF SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.
- ghidra.json/decompiled.txt/auto-give-assembly.txt: full AutoGiveItem0817fb20.
- immediate-ghidra.json: GetTable, FN_check_item_socket, SendMoneyLog, GetEmptyInventory, GetQuickslot, SetQuickslot, CreateItem.
- deeper-ghidra.json: RealNumber, SetCount, AddToGround, StartDestroyEvent.
- grid-quickslot-ghidra.json: complete DelQuickslot and IsEmptyItemGrid, persisted before stopping.
- get-table-assembly.txt/real-number-assembly.txt/create-item-assembly.txt/create-item-decompiled.txt: complete instruction/compact evidence.
- symbols.json/extract_assembly.py/compact_create.py: boundaries and extraction helpers.

## Proven behavioral traps
AutoGive LPITEM(DWORD,BYTE,int,bool), nonconst. Rare input IGNORED: CreateItem literal-1 at0817fc07. Missing table returns before moneylog; moneylog type8 before stacking. Ordinary stack flag4/type!=27 scans90, socket predicate compares actual3 with proto3. Signed MIN of wrapped200-currentcount and remainingBYTE is truncated toBYTE. Overfull201/request2 contributes255, leaves3, sets456, then creates3. Do not harden.
Blend27 creates first then matches type/vnum/3sockets and oldcount<200, adds whole createdcount and returns existing immediately. NO newlycreated cleanup/chat/audit/finalsyslog. Preserve original.
Ground: AddToGround boolignored, StartDestroyEvent, ownership60, SYSTEM_DROP audit. Inventory: optional translatedchat BEFORE AddToCharacter(boolignored), SYSTEM audit, type3/subtype0 emptyquickslot0 initialization. Successful created paths sys_log format7: %d %d then returncreated.
GetTable08193080 calls RealNumber; negative null else direct vector[index]. RealNumber081930c0 top=vector.size() NOTsize-1, wrapped signed midpoint/arithmeticshift, vector read BEFORE top<=bottom check, unsignedDWORD comparisons. Unsafe empty/above-maxmissing read remains. Parent correction: below-first also reaches vector[-1] after top=-1. For vector10,20,30 use matches and interior misses15,25 only; avoid5/35/empty. GetTable/RealNumber now integrated by parent; RealNumber protected. Exact declaration access still needs DWARF. Parent assigned both exclusively here.
FN_check_item_socket08170df0 compares3 actual/protosockets with no guards; reuse real GetProto/GetSockets.
SendMoneyLog08110620 void(BYTE,DWORD,int): nonzerogold packed9(typeBYTE,vnumDWORD,goldint), actualDBPacket header50 handle0. Full DBManager layout/header is missing: recover whole class before integration, never reduced class.
IsEmptyItemGrid081714d0 cell<90; grid0 or UINT32(exception+1). Size1 immediate true; everyothersize INCLUDING0 executes one do-while rowcheck at BYTE(cell+5), samepage(cell/45), then continues untilj>=size. GetEmptyInventory scans0..89 exception-1.
GetQuickslot08050710 pos<36 writespointer elsefalse/outputuntouched. SetQuickslot08050740 mutableTQuickslot&; duplicate deletion precedes positionvalidation; reload inputtype afterdeletion(aliasing matters). Accepttype1 pos<90,type2 pos!=255,type3all,type0false. Packet4 header28 descguard. DelQuickslot08050840 clear2bytes then packet2 header29 WITHOUT descguard.
SetCount081668a0 NO200cap. Nonzero/ownerless UpdatePacket,Save,true. Ownedzero requires real removal/quickslot chain-or-sync/DestroyItem,false. AddToGround08166c20 requires real sectree manager/Get/InsertEntity/UpdateSectree/Save. StartDestroyEvent08166560 requires real item_event_info/item_destroy_event/event_create_ex300*passes_per_sec/SetDestroyEvent. These deeper dependencies remain open.

## Exact pending steps
1 Extract DWARF signatures/access plus FULL DBManager fields/bases/virtual API and nestedrecords (db.cpp), money9/quickslot4/2 packets. Reuse full CHAR/item data.
2 Extract raw CP949chat at0822ddf9; octal C++03bytes. Capture itemname BEFORE locale callback/chat.
3 Stage exact GetTable/RealNumber source, declarations and safe-domain testplan. No builds.
4 Stage FULL AutoGive source/socketpredicate and metadata-proven inventory/quickslot methods. No fake app bodies.
5 Read remainder FULL CreateItem body and export fullcallgraph/records. Reviewed only firsthalf: vnum0, Korea/YMIR50300 randomskillloop, GetTable, duplicateID, newCItem/init/proto, gold/unique IDs/sockets. Do not narrow to potion path.
6 Recover or explicitly classify whole SetCount removal/quickslot/DestroyItem and ground/event closure. No substitute methods.
Parent may promote exact CItem GetType/GetSubType from work/battle_points/item.h.additions.candidate; do NOT duplicate definitions here.

## PointChange syntax receipt
Parent C++03 -m32 -fsyntax-only exit0 using C:/Strawberry/c/bin/g++.exe. Candidate ../PointChange.cpp.candidate SHA256092626bb34a7be0be67d58fade511744afab77d3c646df8b1118b96e55cbfcdd. Full158-header hashes ../parent-syntax-validation.json; overlay ../syntax_tree_20261025T161932567965Z/SYNTAX_MANIFEST.json and SYNTAX_RECEIPT.json; log work/point-change-syntax.log. Syntax/API proof ONLY, functional_proof=false. Full candidate/quarter-loop proof/frontier remain in parent package.

## Parent follow-up (99% usage)
Full GetTable/RealNumber bodies now STAGED in item-lookup.cpp.candidate; read LOOKUP_CANDIDATE_REVIEW.md. Declarations/access metadata, compilation and runtime validation remain pending; no production integration. Earlier statement no lookup candidate is superseded for these two methods only. AutoGive/CreateItem candidates still absent.


## Resumed work completed (supersedes pending steps above)
Parent owns GetTable/RealNumber and integrated them; no duplicate bodies here. Full AutoGiveItem.cpp.candidate and CreateItem.cpp.candidate now staged (allbranches, unresolvedactualedges explicitly retained). Raw chat-format.json extracted; exact CP949 octal literal used. DBManager entire own layout/base/API and complete required records exported dbmanager-dwarf.json/required-records-dwarf.json, with method API metadata. DBManager.declarations.candidate retains allfields/methods and solevirtualdestructor, original2372bytes. Real DB lifecycle recovered from complete exports db-lifecycle-ghidra.json: ctorfalseconnection, all otherbase/member default lifecycle; destructor only member/base teardown. CAsyncSQL full actual implementation alreadyproduction. Closed leaf source candidates SendMoneyLog, inventory-grid, quickslot, plus fullrecords/API declarations. HANDOFF.md and TEST_PLAN.md current integration/dependencyfrontier. No syntaxcompile/build/runtimecheck by thisagent. Parent syntaxpass remains ONLYPointChange.
Next recovery should take actual CreateItem missingdependencies (CSkillManager/Get fullrecords, Blend functions, uniqueexpire/addon/magic/socket), SetCount zeroowner/remove/quickslot/DestroyItem, groundsectree/eventclosure. DB closedbatch readyparentreview/tests, not testedhere. Generated render_db.py is raw ABIrenderer and outputs hidden destructorint; DBManager.declarations.candidate corrected to source destructor(), std qualification, implicitcopy comment. Do not regenerate over correctedcandidate without preserving these fixes.
