# Old item ownership events and ground removal

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

Recovered ownership and ground removal functions remain in game/src/item.cpp with their old CItem declarations in item.h. The30-byte ownership packet remains in packet.h. No new production event or networking substitutes are introduced.

## Behavior

- SetOwnership 08167fd0 creates an ownership event only when there is no active ownership handle. It stores character player ID, allocates one-pointer item_event_info, schedules seconds*passes_per_sec, and emits header31 with item VID and a24-character terminated owner name through PacketAround. Repeated assignment while a handle exists does not refresh the owner or deadline.
- Clearing an active ownership through SetOwnership(null) cancels the real event, resets the stored PID and emits an empty owner name. Calling clear without a handle returns immediately and leaves any stale PID unchanged.
- ownership_event 08167f50 obtains the item from the event's processing info, sets the handle to null, broadcasts the ownership-clear packet and returns0. It does not reset the stored PID. The real event processor frees its info afterward.
- SetOwnershipEvent 08167fb0 only assigns the handle. It does not cancel an old handle or change PID.
- IsOwnership 08167f20 accepts every character when there is no handle; while a handle exists it compares player IDs. Its no-handle branch does not dereference the supplied character pointer.
- RemoveFromGround 08166bc0 acts only when the item has a sector: clear active ownership, remove the entity from its sector, clean reciprocal views, queue Save, and return this. It retains item window, map index, coordinates and identity. Without a sector, it does not clear ownership or save.

Expiry and explicit clear initialize only the first name byte in the packet, leaving the rest unspecified as in the target. Tests inspect the defined header/VID/name terminator bytes, not those unspecified trailing bytes. Non-null owner assignment uses strncpy padding and explicit final termination.

## Evidence

item-addressed-ghidra.json includes all four addressed CItem bodies. item-event-callbacks-ghidra.json includes ownership_event and the still-unreconstructed item_destroy_event callback. item-event-info-dwarf.json proves the old event info is one item pointer at offset0 and size4. Its allocation uses sizeof(item_event_info), preserving the target32-bit size while supporting native host pointers in tests. character-inventory-metadata.json records every ownership packet offset; item_abi.cpp checks its30-byte layout and offsets0/1/5.

## Validation and limits

item_ownership_regression links the actual event allocator/queue/processor/cancellation, item lifecycle, entity view graph, sector insertion/removal, item manager scheduling, descriptor Packet, buffers, fdwatch and core utilities. Its MCNCC_TEST_REAL_EVENTS fixture variant excludes the existing event allocation/create/cancel substitutes. Existing CHARACTER destructor, sector destructor, quest lifecycle, CLIENT_DESC lifecycle, input dispatch and OS boundaries remain explicit.

Tests cover name truncation, no-refresh assignment, positive/zero/negative durations through real event_create_ex clamping, expiry info reclamation, retained PID, explicit cancellation and tombstone removal, pure handle replacement, no-sector behavior, real item destructor cancellation, recipient packets, reciprocal view removal and persistence queue membership. Ground removal emits ownership clear before the ground-delete packet. Focused default and O2 tests pass; packed ABI and strict32-bit item/inventory/manager syntax checks pass. validation-status.json records the completed full-suite checkpoint.

Full ground insertion, item destruction timer/DestroyItem, equipment/expiry modifiers, character cleanup, complete server build/link and Visual Studio integration remain unfinished. Native Windows server support remains deferred by user request.
