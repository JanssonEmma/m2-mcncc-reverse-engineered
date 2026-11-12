# Closed CharLog dependency candidate

Status: recovery/staging complete; no production, status, Makefile, build, or executable changes. Tests are staged and have NOT been compiled or run.

Target identity was reconfirmed before this pass: game SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

## Exact recovered method

080785a0, symbol body192 bytes (56 instructions). Public, nonvirtual, nonconst:
void LogManager::CharLog(DWORD,DWORD,DWORD,DWORD,const char*,const char*).
Old DWORD is unsigned long32. No extra IP argument; the newer supplied reference has a different7-argument API.

Original SQL format:
INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), %lu, %lu, %lu, %lu, '%s', '%s')

Call order:
1. strlen(hint)
2. actual EscapeString(shared __escape_hint,hint,length)
3. get_table_postfix()
4. sys_log(0,format,postfix,pid,x,y,value,text,ORIGINAL hint)
5. get_table_postfix() again
6. actual Query(format,postfix,pid,x,y,value,text,SHARED escaped buffer)

There is no connection guard, text escaping, hint null guard, new length cap, log suppression, or local buffer. Existing old ItemLog and CharLog share1024-byte static __escape_hint at08322600. Preserve this addressed behavior, including ordering across external callbacks. No method may be replaced by a test observer when claiming production closure.

The six direct dependencies are strlen, EscapeString080784c0, get_table_postfix080dac10 (two callsites), sys_log081b82a0, Query08078460. All application callees are already actual production implementations. Query routes to actual CAsyncSQL::AsyncQuery and PushQuery with real SQLMsg ownership/queue/semaphore behavior; native/MySQL/thread boundaries remain external. Thus CharLog is independently integrable without finishing the quest VM.

PointChange0812e340 calls it at0812ee48 in the gold-overflow path. Preserve the previously recovered caller order: _sys_err, fresh current-gold read, CharLog. CharLog recovery removes one real dependency from that larger unresolved SCC; it does not complete PointChange.

## Files and full-data preservation

candidate-log.cpp is the current production log.cpp verbatim plus CharLog, retaining actual ItemLog, Query, Connect, EscapeString and shared buffer. charlog-body.inc is the additive body alone.
candidate-log.h preserves every existing class data member and method, adds CharLog and restores Query to PUBLIC. Original class DWARF is a structure with Query default public, m_sql/m_bIsConnect explicitprivate3. No friend hack or reduced logger class is used.
logger-quest-dwarf.json (copied original evidence), logger-globals-dwarf.json, symbols.json, complete decompile/callgraph/assembly files substantiate signatures, storage, calls and sequence.
audit_candidate.py verifies unchanged production source prefix and full data tail, exports symbol bounds and hashes every artifact. Run stage_charlog.py then audit_candidate.py for final staged artifacts. The latter applies the final nested-hint test correction.
manifest.json records source-only audit results. No compilation success is claimed.

## Meaningful regression strategy

charlog_regression.cpp adapts the existing item_audit_regression.cpp fixture, using the real full LogManager/CAsyncSQL/CSemaphore/SQLMsg and existing actual source paths. External MySQL escape, OS mutex/semaphore/thread calls and sys_log are observer boundaries only, matching the established SQL fixture. Test host thread-create failure avoids a real database/worker; production methods remain actual, queue content is asserted, and teardown drains actual SQLMsg objects.

Run later using the same source grouping/external remaps as the existing item audit target, substituting candidate full log.cpp and the new test translation unit. Reuse sql_connection_fixture.h and its runtime. Do NOT replace Query, EscapeString, AsyncQuery, PushQuery, Connect, or class records to make it link.

Cases:
- Exact CHARACTER format and field order; all DWORD values above signedINTMAX, including0xffffffff, formatted as unsigned32 decimal.
- Distinct raw hint in sys_log versus escaped quote/backslash hint in actual queued SQL.
- Observer actions prove escaping before sys_log and real queue insertion/wakeup while locked (ESLPU).
- Disconnected logger still queues; original connection flags stay unchanged.
- sys_log observer changes g_table_postfix: diagnostic uses_before; SQL uses_after, proving the second actual lookup.
- Empty hint clears prior escaped contents; text remains raw even with a quote.
- Nested ItemLog from sys_log overwrites shared buffer: inner query queues before outer, outer SQL sees nested escaped hint. This is a callback-order regression, not a claim that original production logging itself reenters.
-511 quote bytes expand to1022 bytes plus terminator, exercising the safe boundary of the actual1024-byte global without inventing an oversized-input guarantee.
- Real SQLMsg ID/handle/non-return metadata, semaphore wake and queue ownership are asserted.

charlog_abi.cpp checks exact6-argument nonconst pointer-to-member signature, public Query access, DWORD4 and full LogManager1160. Compile in the target strict32 profile; an LP64 ABI is not equivalent.
After integration run normal, optimized, and strict32 checks once; broader application link/runtime status remains separately unresolved until PointChange dependencies are recovered.

No original ELF execution is needed. Full quest records/APIs and the unresolved VM closure remain unchanged in ../RECOVERY_PLAN.md.
