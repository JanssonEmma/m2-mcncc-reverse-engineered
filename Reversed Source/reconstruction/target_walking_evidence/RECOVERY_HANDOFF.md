# Target HP and walking recovery package

Target SHA-256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Live Ghidra program /game and an independent original-file hash agree. This package makes no production/status/Makefile edits and performs no builds, tests or original-ELF execution.

## Evidence and complete candidates

BroadcastTargetPacket08132670 and SetNowWalking081341a0 are complete old non-const CHARACTER methods. Original definition DWARF places them in char.cpp at lines4779 and5437 respectively. Their complete69/45-instruction bodies and untruncated decompilations are retained. old-types-signatures-strings.json preserves definition/declaration files, exact signatures, full packet record fields/aliases, ten-value ECharType, character field offsets and original raw strings.

packet_target/TPacketGCTarget is packed6: BYTE header@0, DWORD dwVID@1, BYTE bHPPercent@5. SPacketGCWalkMode/TPacketGCWalkMode is packed6: BYTE header@0, DWORD vid@1, BYTE mode@5. Original packet.h DWARF declaration lines are1403/1885. ECharType is the complete ten-value old enum from common/length.h line246; CHAR_TYPE_PC=6. Both packet headers are instruction-confirmed byte literals63/111. Walk-mode names and anonymous enum placement follow reference packet.h; old SETNZ instructions independently prove run0/walk1. Those anonymous enumerator names were not retained in inspected old DWARF, and no modern GC-header enum or extra mode values are imported.

Ready insertion files: common-length-char-type.inc, packet-target-walk.inc, char-target-walk-declarations.inc and char-target-walk-bodies.inc. Parent may already have integrated these; retain them as reviewable candidates rather than overwrite newer source. target_walking_abi.cpp checks complete field offsets, sizes, scalar widths, enum values and exact non-const member signatures.

## BroadcastTargetPacket behavior

The empty targeted-by set returns before constructing/sending packets or dividing HP. Source GetDesc!=0 forces percent0 regardless of raw char type, HP or maxHP. Otherwise, the old LEA/SHL sequence multiplies hp by100 in32-bit registers, then performs signed IDIV by iMaxHP and clamps via MINMAX(0,quotient,100). The candidate's unsigned32 multiplication followed by signed conversion preserves register wrap under O2, consistently with the existing recovered party HP calculation.

Packet VID is the raw VID id, not playerID or VID CRC. A single six-byte packet is built before iteration. The actual std::set pointer order controls recipients; insertion/VID order, view maps, sectors, source observer state and distance do not select target recipients. The iterator advances before descriptor lookup and Packet, permitting removal of the current recipient by a packet callback without invalidating the next iterator.

A valid recipient object without a descriptor triggers _sys_err("BroadcastTargetPacket",4801,"%s %p does not have desc",recipient->GetName(),recipient), then actual abort. It is never skipped. Format/function bytes at08226b2a/082249e3 and instruction immediate0x12c1 independently prove the diagnostic. A null set entry is not guarded by the original and is not a supported test input.

Required original division conditions remain unmodified: when a nonempty target set and descriptor-less source take the arithmetic branch, maxHP must be nonzero and the signed quotient must be representable. Wrapped numerator INT_MIN divided by-1 would trap just as the original IDIV; zero and overflow-divisor cases are not executed by ordinary regression rows. No invented maxHP>0 guard, saturation-before-multiply or widened arithmetic is added.

## SetNowWalking behavior

The action condition is m_bNowWalking!=requested OR raw m_bCharType!=6. It is deliberately broader than the newer reference's changed-only condition. Thus PC type6 unchanged calls return, whereas every unchanged non-PC call still acts. Type decisions use raw m_bCharType, independently of GetDesc.

Walking true stores the flag, reads get_dword_time once and overwrites m_dwWalkStartTime, including repeated true NPC calls. Walking false clears only the current flag, retaining the prior start time and making no clock read. m_bWalking preference and stamina do not select packet mode. The method builds header111/VID/mode0-or1 and calls the actual PacketView even on a false transition or repeated NPC flag. Raw type and current walking flag are reread after PacketView before selecting MonsterLog, matching the target call order.

Non-PC action calls MonsterLog with exact CP949 walking b0c8b4c2b4d9 or running b6dab4d9 bytes. The candidate uses byte escapes, avoiding source-code-page conversion. Existing real MonsterLog additionally gates on test_server and descriptor absence; its separate monitored-character fanout is not suppressed by source PacketView's sector/observer gates.

## Existing dependency closure

Seventeen complete function exports and nine complete call-graph roots are normalized in manifest.json. Direct/core dependencies are actual GetName, MINMAX, get_dword_time, CEntity::PacketView, DESC::Packet, MonsterLog and CHARACTER_MANAGER::PacketMonsterLog. PacketView's concrete for_each/FuncPacketView path, original tree increment, DISTANCE_APPROX and TEMP_BUFFER ctor/write/size/read_peek/dtor are also exported.

The existing recovered PacketView returns without a sector, sends descriptor-bearing view-map recipients in map order unless source observer, then source descriptor. MonsterLog uses the existing fixed old formatter/prefix, TEMP_BUFFER and PacketMonsterLog; no replacement or reduced helper is introduced. The manager filters monitored recipients with actual DISTANCE_APPROX<=6000. Because DISTANCE_APPROX(x,0)=x*246>>8, the integration rows use X6244 ->6000 and X6245 ->6001, not raw X6000/6001. Existing descriptor/buffer/fdwatch/clock implementations and explicit external test seams are reused.

No new broad unrecovered algorithm prevents these two complete candidates. This does not close full PointChange, movement policy, character teardown or all server dependencies. Existing native STL/test execution is distinct from original32-bit ABI or FreeBSD runtime equivalence.

## Concrete regression handoff

Copy target_walking_regression.cpp, target_walk_desc_output_syscalls.c and target_walking_abi.cpp into reconstruction/tests and append/merge Makefile.target-walking.inc once source/data integration is ready. The recipe uses the current DESC_PACKET_SOURCES, including real char/input/battle/party methods plus existing unrelated lifecycle fixtures, and the established ITEM_SOCKET dependencies. It never introduces an application-method mock. The check/check-abi dependencies preserve existing recipes; avoid adding a second invocation to the original check body.

The staged regression covers:
- Empty target set with maxHP0; descriptor actor percent0 with maxHP0; signed clamp, truncation and wrapping HP rows, raw VID bytes, pointer-set order and erase-current transport callback.
- A child host regression process invoking the real missing-descriptor path. The strict external logger checks function/line4801/format/name/pointer and returns. The production method then calls actual abort; SIGABRT handling immediately exits child87 only after the diagnostic marker was flushed. This avoids CRT report dialogs and distinguishes normal return89 or an earlier unexpected abort88. It is not original ELF execution. Windows _spawnl receives an unquoted executable API path and quoted argv0 for absolute F paths containing spaces.
- All256 raw char-type bytes and all four old/requested bool combinations, exact clock counts/start timestamps, untouched preference, stamina-independent raw mode and complete six-byte packet output.
- Real PacketView sector/observer/source descriptor fanout and map order, including an offline view entry.
- Actual MonsterLog -> TEMP_BUFFER -> actual CHARACTER_MANAGER monitor -> actual DESC packet bytes, both CP949 words and distance boundary, source descriptor/PC-type gates, and monitored logging despite missing sector or source observer.
- All packet bytes are observed only at the external thecore_memcpy transport seam. All clock reads are observed at gettimeofday. Any unexpected _sys_err aborts.

The child uses a PID-specific temporary marker under the runtime test working directory and deletes it on success. Assertions are expected enabled, as in the existing suite. ABI/signature and regression sources have not been compiled or run by this subagent. Parent should perform focused default/O2/ABI and then the affected full suite after integration. manifest.json records static validation and artifact hashes only; no suite pass is inferred.
