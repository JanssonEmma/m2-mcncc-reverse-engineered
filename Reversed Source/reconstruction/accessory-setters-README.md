# Accessory setter recovery (candidate only)

Target ELF SHA-256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.
No production changes and no builds were performed.

## Evidence

- `decomp.json`: saved addressed Ghidra bodies for seven accessory methods.
- `dwarf.json`: selected item.cpp/constants.cpp DIEs including all setter argument types, inline getter signatures and inline address ranges.
- `setters-assembly.txt`: original objdump instructions. Grade getter at 08168888 reads +0x64; max getter at 0816885c reads +0x68. Both are raw, signed int-returning getters. The inline time getter in accessory_socket_expire_event at 081689d0..d3 reads +0x6c; see expiry-getter-assembly.txt.
- SetGrade calls MINMAX with raw slot1, calls SetSocket(0,...), THEN reloads slot0 for the table index. Preserve callback ordering. Signed division by 60 is proved by imul/sar/sign correction, and DWARF local iDownTime is int.
- SetTime argument is DWORD (unsigned long, 4 bytes), writes through SetSocket(2,...), then reloads test_server/owner and resolves locale_find(raw CP949 message,g_iUseLocale) for ChatPacket type1.
- `constant-symbols.txt`: all four names are global object symbols in .rodata; sizes 12/16/16/16. Exact bytes/values are already saved in parent work/accessory-constants.json.
- `constants-dwarf-all.json`: exhaustive raw DW_AT_name scan over ALL CUs found no matching aiAccessorySocket* or ITEM_ACCESSORY_SOCKET_MAX_NUM DIEs, including DW_TAG_constant. Original const int type and enum spelling cannot be directly proved by retained DWARF; external const int declarations and the enum spelling match the supplied reference source and ELF read-only symbol sizes/32-bit indexed arithmetic. Numeric max3 is independently proved at 081688d1.

## Candidate integration

constants.h.candidate and constants.cpp.candidate are proposed new files (production has no constants.h). Add item.h.additions.candidate declarations/getters and item.cpp.additions.candidate bodies; add constants.cpp to actual build input list if required. Existing SetAccessorySocketMaxGrade can use the enum value. Do not replace real SetSocket or introduce stat/equipment methods as stubs. The candidate uses explicit octal escaped CP949 bytes (C++03 safe; no hex escape run-on).

## Required integration test design

Use existing item regression harness with real CItem::SetSocket, UpdatePacket, Save, LogManager::ItemLog and real CHARACTER::ChatPacket. Observe external packet/SQL/devlog/save sinks already used by those paths, not replacement CItem methods.

1. For max grades0..3, requested grades -1,0,1,2,3,4: expected grade MINMAX(0,request,raw slot1), with table-time cases restricted to valid index0..3. Assert TWO socket mutations even when values already match, indices0 then2. Each must retain both update/save and SET_SOCKET audit effects. Normal server times:0,86400,43200,21600; test server:0,1440,720,360.
2. Owning character + connected descriptor: normal server yields two item update packets; test server adds one real type1 chat packet AFTER both socket update sequences. Capture raw locale key and time argument, with g_iUseLocale0 and a real loaded translation in g_iUseLocale1. Owner absent or test_server0 suppresses chat.
3. Direct SetTime with0 and0xffffffffUL preserves raw target32-bit socket bits. Under test_server and owner, chat formats %d as original signed32 representation. Avoid host64 expectations: target uses4-byte DWORD/long.
4. At external sink after first SetSocket callback, alter slot0 to a different VALID index using an existing test seam if available; verify time uses reloaded grade. Do not cache clamped grade in candidate. Raw getters should expose negative or >3 socket values unchanged; do NOT call SetGrade for invalid table indices because original can read out-of-bounds.
5. Preserve raw max behavior: getter may report4; SetGrade request1 still uses table index1. No hardened max getter is allowed. Direct SetTime must read owner/test_server after SetSocket callbacks.

No tests executed here by explicit parent restriction. Full accessory expiry/degrade integration remains blocked on real ModifyPoints/ApplyPoint; do not fake those dependencies.


