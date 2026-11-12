# Old inventory placement and transfer

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

Recovered CHARACTER storage and lookup bodies remain in game/src/char_item.cpp, matching the reference tree. CItem::AddToCharacter remains in game/src/item.cpp. Old API declarations are added to existing full character/item data classes. Packet declarations remain in packet.h; no newer inventory systems are imported.

## Recovered functions

- GetItem 08170f60: accepts absolute slots0..108; invalid BYTE slots log original GetItem:212 and return null.
- GetWear 081712b0: maps equipment indices0..18 onto absolute slots90..108. Invalid indices log GetWear:350 and return null.
- FindSpecifyItem 0817f870: returns the first matching vnum among inventory slots0..89. Equipment is excluded. A non-null item without prototype has vnum0 and can match zero.
- SetItem 08170fb0: requires an unowned incoming pointer. It logs SetItem:229 and returns on an invalid slot. Inventory grid footprints use a five-column stride and prototype size. Removing an old footprint preserves grid cells containing another anchor item. Footprints are clipped at90; equipment occupies one grid cell. Size-zero items and clearing an already empty slot leave those grid bytes unchanged. Replacing/removing a slot does not clear the outgoing item's owner/cell.
- SetItem sends a normal packed header20 packet (40 bytes) for null items or signed flags below0x4000. Otherwise it sends header21 with flags (44 bytes). This is a signed magnitude comparison, not a bit test: negative flags choose the normal packet. Count narrows to BYTE; sockets and attributes preserve raw bytes. Deletion sends the normal packet with zero item data.
- SetItem updates the character slot before sending, then assigns incoming item owner/cell and window1 or2 after sending. The old inline SetCell/SetWindow signatures and field operations are retained.
- AddToCharacter 08166af0 asserts no sector/owner, then checks the item's existing cell against90. If the character has a descriptor, it records player ID as last owner; otherwise it retains the previous PID. It cancels the destroy event, calls SetItem, assigns owner, queues Save and returns true. It does not independently validate the requested cell. Therefore an invalid destination can log inside SetItem while AddToCharacter still assigns ownership/queues saving and returns true; this old behavior is preserved, not silently corrected.

## Evidence and checks

character-inventory-ghidra.json retains eight target functions, including not-yet-promoted SetWear, ClearItem and quickslot dependencies. character-inventory-metadata.json retains old packet layouts and method signatures from char_item.cpp. item-addressed-ghidra.json supplies AddToCharacter and its surrounding lifecycle. Full data declarations remain old-version definitions rather than reduced subsystem substitutes.

character_inventory_regression links actual character construction, item manager, descriptor, buffers and persistence scheduling. It checks grid overlap/clipping/zero-size/empty-slot behavior, bounds diagnostics, inventory-only lookup, signed flag thresholds, both packet formats, deleted-item zeros, owner/window assignment, event cancellation order, last owner PID retention, queue membership and the old invalid-destination behavior. A temporary memcpy observation hook inspects state at the actual buffer copy, verifying slot insertion precedes output and incoming ownership follows output. Only the destroy-event cancellation is an explicit scripted boundary in this suite; existing CHARACTER/quest/sector lifecycle and OS fixture limits still apply. No original server execution occurs.

Focused inventory, persistence and item lifecycle tests pass. Inventory passes at O2. Every field offset and size of the40/44-byte packets passes the32-bit layout gate; char_item.cpp, item.cpp and item_manager.cpp pass strict32-bit C++03 syntax checks. See validation-status.json for the completed full-suite checkpoint. Modern STL still prevents a claim of complete host class ABI equivalence.

## Remaining dependency chain

SetWear needs real affect removal. RemoveFromCharacter and ClearItem need real Unequip, expiration stop/point recomputation, quickslots and DestroyItem. Unequip evidence identifies StopUniqueExpireEvent, StopAccessorySocketExpireEvent, ModifyPoints, SetWear, ComputeBattlePoints and character UpdatePacket. Those dependencies remain unfinished; no production no-op replacements have been added. Full server build/link and Visual Studio integration remain pending.


Instruction audit: character-inventory-instructions.txt independently confirms the decompiler's unusual cases. At08166b3e, CMP byte ptr [EBX+0x50],0x59 reads the item cell field, while the requested cell is kept on the stack. At08171117/0817111d, CMP ECX,0x3fff followed by signed JLE selects the normal packet. The descriptor Packet call at08171196 precedes owner/cell/window writes at081711b0/081711b3/081711b8. The listing was exported read-only with executable hash verification.


Ownership follow-up: real ownership timers/callbacks, ownership queries and RemoveFromGround are now reconstructed and tested with the actual event queue. See item-ownership-recovery.md. Ground insertion, DestroyItem and equipment teardown remain unfinished.
