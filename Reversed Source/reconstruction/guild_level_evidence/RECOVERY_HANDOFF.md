# Old guild level-change recovery

Target SHA-256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Live /game identification and independent ELF hashing agree. All work is read-only/staged under work/guild_level. No production/status/Makefile edits, builds, tests, child agents or original ELF execution.

## Ready candidates and original placement

guild.cpp contains complete bounded original bodies: both SGuildMember constructors, CGuild::GetMember0808cf20 and CGuild::LevelChange0808f5d0. There are no production lifecycle substitutes. Original DWARF places constructor definitions at guild.cpp20/23, GetMember656 and LevelChange1542. Four ctor machine variants are exported: character ctor C2/C1 at08089f80/08089ff0, explicit-data ctor C2/C1 at0808a060/0808a0b0. ELF linkage names and sizes are preserved in manifest.json; one C++ source body per overload naturally generates the variants.

Insert guild-method-declarations.inc inside the existing complete CGuild public interface. Existing guild.h already holds all312 bytes of old CGuild data, SGuildData220, SGuildGrade10, SGuildWar20, SGuildCreateParameter20 and SGuildMember20. All are independently re-exported with full members/types/offsets in old-types-signatures.json. guild-member-record-reference.inc records the FULL SGuildMember declaration for review, including std::string name; do not duplicate or replace the existing full record with a reduced wire model.

tables-guild-member-change.inc and packet-guild-header.inc provide complete shared14-byte and client4-byte records in original common/tables.h and game/src/packet.h locations. guild_level_abi.cpp checks complete member/guild/record layouts and exact signatures. Use the existing strict32 old-string-ABI flags (-m32 -D_GLIBCXX_USE_CXX11_ABI=0; suppress conditional offsetof warnings as other guild ABI checks do). It has not been compiled here.

## Exact CHARACTER accessor correction needed

Old GetLevel is int GetLevel(), NOT BYTE and not const. Old GetPlayerID is DWORD GetPlayerID(), not const. Linkages are _ZN9CHARACTER8GetLevelEv and _ZN9CHARACTER11GetPlayerIDEv; definition/declaration metadata records return types and non-const pointed-to CHARACTER. Implicit const-this-pointer storage is not a const member function.

Current reconstructed char.h has a pre-existing const GetPlayerID inline accessor and no GetLevel. char-inline-accessors.inc supplies the exact old int level getter and non-const DWORD player-ID getter; replace the old const declaration, not add a synthetic second overload. The member constructor independently proves player ID DWORD at CHARACTER+0xf0 and zero-extended raw level byte at+0x502. GetLevel widens that raw byte to int; SGuildMember narrows it into its BYTE level field. Existing real GetJob/GetName are reused without replacement. CGuild GetID is already the correct non-const inline getter and requires no new behavior.

## Exact member construction and lookup

The character constructor copies GetPlayerID, supplied grade, is_general0, actual GetJob, actual GetLevel, supplied offer and a std::string copy of GetName. The explicit-data constructor copies all supplied fields and string from original char* name. Neither constructor writes _dummy@12 or padding. Do not initialize _dummy to0 as an invented convenience; the level-change method owns its explicit later reset. Tests should explicitly set _dummy before observing its value, rather than read an indeterminate byte.

GetMember uses map.find(pid), returns0 on missing without insertion, otherwise returns the actual mapped TGuildMember address. It is non-const and does not filter online/PC state, allocate a replacement, copy data or validate stored pid against the map key.

## Complete LevelChange ordering and wire semantics

1. Find the requested map key. Missing returns immediately before touching db_clientdesc, any member or any online entry. No insertion.
2. Store the new BYTE level in the mapped member. There is no equality guard, max-level clamp, authorization check or character lookup.
3. Snapshot DB fields: guild ID, REQUESTED lookup pid, current offer_exp, supplied level and current grade. Call actual db_clientdesc->DBPacket(opcode24,handle0,record14). A valid DB client with output buffer is an original precondition for present members, even if there are no online entries.
4. After DBPacket returns, set client header75 and member._dummy=0. A DB log/copy observer can see new level with the prior _dummy; the reset must not be moved before the DB call.
5. Traverse actual m_memberOnline std::set pointer order. Each valid CHARACTER with GetDesc!=0 receives BufferedPacket(header4) then Packet(the mapped member's LIVE first13 bytes). Descriptor-less entries are silently skipped, unlike BroadcastTargetPacket. The iterator advances AFTER these calls. Do not import the target-broadcast preincrement/current-entry-erasure behavior here.
6. Header is BYTE75, packed WORD size17, BYTE subheader2 (list). Member prefix is pid@0,grade@4,is_general@5,job@6,level@7,offer_exp@8,_dummy@12. std::string name@16 is never sent. Do not serialize a name, invent a reduced production struct, copy sizeof(TGuildMember), cache a client payload snapshot before DB or replace the two descriptor calls with a single direct Packet.

DB record SPacketGuildChangeMemberData is guild_id DWORD@0,pid DWORD@4,offer DWORD@8,level BYTE@12,grade BYTE@13, packed14. The real DB framing adds opcode1+handle4+length4 =9 bytes, for total23. Client packet_guild header is BYTE@0,WORD size@1,BYTE subheader@3, packed4; combined member list entry is17 bytes. Constant values24/0/14/75/2/4/13/17 are verified from original LevelChange instructions and retained in wire-constants-and-vectors.json. Inspected old DWARF did not retain the anonymous header/subheader enums, so candidates use evidence-backed literals and do not import a partial/newer global enum.

The requested map key and stored member.pid need not match in the algorithm: DB pid uses the request; client prefix uses the stored member pid. No consistency hardening is introduced. Recipients need not be members matching that pid, have descriptors in a particular phase, be PC raw type or be in the same view/sector. A null pointer entry or erased member/iterator during callbacks violates the original object/iterator preconditions; no new guard is added.

## Existing direct/transitive closure

LevelChange's five direct targets are map find08090e20, CLIENT_DESC::DBPacket0813fbd0, DESC::BufferedPacket08139cb0, DESC::Packet08139d10 and std tree increment080514e0. These are exported complete, plus DBPacketHeader0813fb40. The current production DB/client/descriptor/buffer implementations already implement this path; no Guild SendDB wrapper, GetMember wrapper call, CHARACTER GetLevel/GetPlayerID lookup or other online-PC dispatcher is called by LevelChange itself. The optional recovered GetMember and member constructors make real state setup/access available.

Twelve complete function exports, twelve call-graph records and full88/26/34/26 instruction bodies for LevelChange/GetMember/two constructor bases are indexed in manifest.json. Decompiler STL parameter-storage noise is resolved by the saved instructions and standard full-container operations. Program data and exact source locations are preserved.

## Real DB/client integration test plan

Use the current shared DESC_PACKET_SOURCES and ITEM_SOCKET dependencies, actual new guild.cpp and the existing test-only guild_lifecycle_fixture.cpp. Keep full CGuild data/std::map/std::set/std::string and actual CHARACTER constructors/accessors; no reduced classes, fake LevelChange/GetMember, fake DBPacket or descriptor methods. Create real CLIENT_DESC with its output buffer through an existing probe and real normal DESC buffers/phases for online recipients. Observe only external thecore_memcpy/sys_log seams; object lifecycle boundaries remain explicitly test-only.

A. Missing member with db_clientdesc=0: expect no output, no map insertion, unchanged other members/online sets. GetMember missing also leaves map size unchanged. Present GetMember must return the exact mapped object's address and expose live changes.

B. Real member constructors: actual CHARACTER raw job6 converts via real GetJob to2, playerID differs from VID, level255 is widened by int GetLevel then stored BYTE, GetName is copied. Cover high-bit DWORDs/all raw byte constructor fields, string source mutation after construction and empty name. Do not expect _dummy initialization; explicitly set it to0xa5 before subsequent level tests. Use typed member-function pointers to verify exact accessor constness/return types.

C. Present member with empty online set still writes real23-byte DB frame and clears _dummy only after DB call. No equality shortcut: call the same level twice and expect two frames. Exercise all256 requested levels. Snapshot state at the DB payload/log observation: level is already new while _dummy remains0xa5. Following return, _dummy must be0.

D. Use map key0x12345678, stored pid0x87654321, guildID0x89abcdef, offer0xfedcba98, grade225,is_general167,job242,level255. Independent golden vectors are saved: DB23=18000000000e000000efcdab897856341298badcfeffe1; client17=4b11000221436587e1a7f2ff98badcfe00. Assert all DB header fragments/handle0/length14 and full client header+prefix bytes, including DB level-before-grade layout and absence of name bytes.

E. Multiple online CHARACTER entries inserted out of pointer order, plus an offline entry and an online character whose playerID is not the changed member: expect all descriptor-bearing recipients in std::set pointer order, offline skipped. Verify real BufferedPacket4 precedes actual Packet13 for each recipient and real client transport emits the combined17 bytes. Do not reorder by PID or use a snapshot vector.

F. Pre-existing buffered client bytes must remain before the guild header/prefix; real Packet aggregation is observable. A phase0 descriptor still gets BufferedPacket(header4), then real Packet returns before sending/appending the13-byte payload: output stays empty and the existing buffered header remains. Clean it with the existing probe afterward. DBPacket is a separate direct-buffer API and does not gain a normal DESC phase guard.

G. External DB observation may mutate member.grade/offer after the local DB snapshot: DB packet retains old snapshot while subsequent client member-prefix reads reflect current fields, with _dummy reset0 after DB return. A completed first-client output observation may mutate member fields before a later recipient, whose prefix must reflect the change. Detaching a later recipient descriptor without erasing its set entry should cause the later lookup to skip it. Do NOT erase the current online entry/member iterator during these callbacks; unlike BroadcastTargetPacket, old LevelChange advances afterward and offers no erasure safety.

H. After default/O2 focused runtime and strict32 ABI pass, run the affected shared full suite. No tests are run or pass inferred in this package.

## Remaining broader dependencies

No unrecovered application algorithm blocks the complete LevelChange/GetMember/member-constructor bodies once the staged packets and exact CHARACTER accessors are integrated. Existing DB/client paths provide the real sends. Production CGuild creation/destruction/loading, wars, member ownership/teardown and online membership management remain pending; constructing a live guild in the eventual full server is NOT solved by this slice. The existing full-data test lifecycle fixture is retained solely for integration tests and is not production recovery. Complete PointChange and the full2009 server goal remain unfinished.
