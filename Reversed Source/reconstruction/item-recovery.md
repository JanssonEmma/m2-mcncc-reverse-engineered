# Old item lifecycle recovery

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

Production files retain the reference structure: game/src/item.h, game/src/item.cpp, common/tables.h and game/src/packet.h. CItem has its complete old data and virtual interface; the nonvirtual API is still partial. The target has no independent vnum field: its prototype supplies vnum. This old class is 160 bytes with the original GCC3.3 containers; modern STL changes the inherited entity size, so full host class ABI equivalence is not claimed.

## Recovered bodies

- CItem constructors 08166260 / 08166290 construct CEntity and leave item scalars untouched. There is no implicit call to item Initialize.
- Destructor variants 081662c0 / 081662f0 / 08166320 call Destroy and then the entity destructor; ordinary C++ supplies deleting-destructor behavior.
- Initialize 08166350 explicitly initializes entity type 1 and resets all item data, sockets, seven attributes and four event handles. It does not cancel existing handles before resetting them.
- Destroy 08166410 cancels destroy, ownership, unique expiry and accessory expiry in that order, then calls CEntity::Destroy and removes the item from its sector if present. It retains prototype, owner and identity fields. Calling Destroy before Initialize remains invalid for an ordinary newly constructed item, as in the target.
- SetProto 08166700 requires a non-null table and copies dwFlags into m_lFlag.
- EncodeInsertPacket 081665e0 sends packed header 26, signed coordinates, VID and prototype vnum (21 bytes) to entities with a descriptor, then logs using CHARACTER::GetName. It dereferences the prototype for the packet, preserving the valid-prototype requirement.
- EncodeRemovePacket 08166680 sends packed header 27 and VID (5 bytes), then logs. Both packet paths preserve the target CHARACTER receiver assumption.

Packed DWARF declarations cover TPlayerItemAttribute (3 bytes), item limit/apply (5 each), SItemTable (154), and the ground packets (21/5). item_abi.cpp checks every field offset and size in 32-bit compilation. No newer item table extensions are imported.

## Evidence and validation

item-interface-dwarf.json retains full old records and method signatures from item.cpp/item_manager.cpp. item-constructors-ghidra.json, item-destructors-ghidra.json and item-lifecycle-ghidra.json were exported by Ghidra 12.1.4 from the saved game analysis in a task-owned project copy, read-only and without reanalysis. Each batch verifies the executable SHA256. This fallback does not require the GUI MCP endpoint and does not modify the original project.

item_regression exercises the real constructor, Initialize, SetProto, packet encoders, Destroy and destructor, with actual entity/sector/name code. Explicit test-only descriptor capture and cancellation hooks verify packet bytes and handle order; they do not establish event subsystem integration. Existing event suites validate event cancellation separately. CHARACTER destructor, quest manager lifecycle and sector destructor remain explicit pre-existing test boundaries.

The focused regression passes at default optimization and O2. The packed layout gate passes, and item.cpp passes strict 32-bit C++03 syntax checks with the existing POSIX header shims. Full server compilation/linking, item ownership/inventory/expiry behavior, remaining gameplay and Visual Studio integration remain unfinished. See validation-status.json for the latest complete-suite checkpoint.


Persistence follow-up: Save, UpdatePacket and old inline persistence accessors are now reconstructed with the full manager data and real persistence scheduling/DB serialization. See item-persistence-recovery.md; inventory transfer and expiry remain pending.


Inventory follow-up: character slot placement/lookups and CItem AddToCharacter are now reconstructed and tested. See character-inventory-recovery.md. Equipment removal, expiry and character teardown remain unfinished.


Ownership follow-up: real ownership timers/callbacks, ownership queries and RemoveFromGround are now reconstructed and tested with the actual event queue. See item-ownership-recovery.md. Ground insertion, DestroyItem and equipment teardown remain unfinished.

Unique-expiry follow-up: real StopUniqueExpireEvent now restores special-item alignment, stores signed remaining seconds and cancels through the actual event engine. Prototype value2 nonzero retains the timer. See item-socket-expiry-recovery.md. Broader accessory/equipment cleanup remains unfinished.
