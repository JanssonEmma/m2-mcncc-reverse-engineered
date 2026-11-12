# Old item manager and persistence

Target game SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

game/src/item_manager.h preserves all old manager data (172 bytes with the original GCC3.3 STL) and the singleton base. Modern STL changes this size; the host class is not claimed ABI equivalent. Drop group pointers remain forward declarations because these maps do not require owning pointee definitions. game/src/item_manager.cpp contains recovered lifecycle, lookups and persistence. Item Save and UpdatePacket remain in game/src/item.cpp, matching reference organization.

## Recovered behavior

- Constructors 08190630/08190a40 zero VID/current-ID counters and construct every container, leaving m_iTopOfTable untouched.
- Destructor variants 08190da0/08191280/08191760 call Destroy, then ordinary member/base destruction. They do not flush saves.
- Destroy 08191c50 calls the target wipe_second helper 08199210: delete each VID-map pointer, then clear that map. Other ID/delayed-save/drop registries are retained by the body. Deleting their std::map/std::set nodes at manager destruction does not delete pointed-to objects.
- Find 08192fe0 and FindByVID 08193030 search distinct ID and VID registries without insertion. GetRefineFromVnum 08198c30 returns zero on a missing mapping.
- Item Save 08167c40 enqueues only when skip-save is false. DelayedSave 081928a0 inserts uniquely by pointer only if the current item ID is nonzero. Setting skip-save later does not remove a queued item.
- FlushDelayedSave 081928f0 removes membership before SaveSingleItem, and does nothing for an unqueued pointer.
- Update 08192b80 advances its iterator, then saves/removes unowned items or owned items without flag 0x10. Owned flag-0x10 items stay queued. It saves before erasing that entry.
- GracefulShutdown 08191c60 saves all queued items regardless of that flag, then clears the set.
- SaveSingleItem 08192980 directly serializes even if skip-save is set. Unowned items send DB header31 with ID and last owner PID (8 bytes). Owned items send header30 with packed TPlayerItem (52 bytes). Equipment window2 subtracts90 from the cell. Windows3/4 use descriptor account ID; all others use character player ID. Count remains DWORD, and sockets/attributes retain their exact signed/raw values. These calls use actual CLIENT_DESC::DBPacketHeader and Packet.
- Item UpdatePacket 081667d0 requires both owner and descriptor, sends header25 with cell, BYTE-narrowed count, three sockets and seven attributes (36 bytes), and logs before sending. Added item inline accessors and DESC::GetAccountTable use old DWARF signatures and field accesses corroborated by these callers.

## Evidence and validation

item-manager-addressed-ghidra.json contains all32 addressed manager symbols, each successfully decompiled with no truncation. This is an evidence archive, not a claim that all32 are reconstructed. item-manager-wipe-ghidra.json retains the actual target deletion helper. item-interface-dwarf.json and item-save-metadata.json record complete declarations, method signatures and packed offsets. Read-only headless Ghidra uses a task-owned saved project copy; original project and ELF are unchanged.

item_persistence_regression links actual item, manager, character initialization, descriptor, CLIENT_DESC, buffer, fdwatch, TEA and core utility code. It validates complete DB framing and payloads, owner/account windows, count narrowing versus DWORD persistence, socket/attribute bytes, unique queue membership, deferral/flush/update/shutdown and VID-owned virtual deletion. It checks the constructor's retained table-index bytes and singleton lifetime. Existing explicit test boundaries remain for CHARACTER destructor, quest lifecycle, sector destructor, CLIENT_DESC Destroy/SetPhase, input dispatch and OS calls. No original binary execution occurs.

The isolated item_regression links a separate aborting unused DB transport boundary because it does not exercise persistence; the persistence suite links real transport writers instead. This boundary is never production source.

Focused default and O2 tests pass. check-abi includes every offset/size for the52-byte item record and36-byte update record; item.cpp and item_manager.cpp pass strict32-bit C++03 syntax checks. validation-status.json records the completed full-suite checkpoint.

## Outstanding work

Inventory transfer/unequip, item expiry, drop group loading, CreateItem/DestroyItem, prototype loading, ID allocation, character teardown, broader gameplay and a complete server build/link remain unfinished. Newly saved character-inventory-ghidra.json supplies the next eight dependency bodies. SetItem's old signed flag threshold and ownership assignment after packet output require preserving when that module is reconstructed. Visual Studio integration remains pending; native Windows runtime support is deferred by user request.


Inventory follow-up: character slot placement/lookups and CItem AddToCharacter are now reconstructed and tested. See character-inventory-recovery.md. Equipment removal, expiry and character teardown remain unfinished.


Ownership follow-up: real ownership timers/callbacks, ownership queries and RemoveFromGround are now reconstructed and tested with the actual event queue. See item-ownership-recovery.md. Ground insertion, DestroyItem and equipment teardown remain unfinished.


Socket/value follow-up (2026-10-25): CItem::GetValue at 08167be0 asserts an unsigned index below six and reads the signed prototype value without a null-prototype fallback. CItem::SetSockets at 08167d70 copies the three socket longs through thecore_memcpy and calls Save; it does not invoke UpdatePacket. The old DWARF signature accepts long*, whereas the newer reference accepts const long*. The reconstructed declaration follows the binary. Existing item-addressed-ghidra.json retains both bodies, and the live MCP decompiler corroborated them against the matching game program.

The real persistence integration now checks all six signed prototype reads, minimum/maximum signed socket values, input-copy independence, skip-save suppression, duplicate scheduling, no immediate database/client output and the eventual database socket payload. The 32-bit syntax check uses the established socket/fdwatch/POSIX shims and excludes only the existing long-long extension diagnostic. Broader item expiry, single-socket logging, equipment cleanup and full server linking remain unfinished.

Single-socket follow-up: actual SetSocket now preserves visible update, delayed save, developer log and old item-audit ordering, including signed-to-DWORD audit conversion and unchanged-value updates. See item-socket-expiry-recovery.md for real backend/event integration and remaining boundaries.
