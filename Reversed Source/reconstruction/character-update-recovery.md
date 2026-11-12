# Character update and alignment recovery

Target SHA-256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

CHARACTER::UpdatePacket (081288a0) and ShowAlignment (08153fd0) are implemented in game/src/char.cpp. Saved bodies are in equipment-character-dependencies-ghidra.json; the restored live Ghidra connection independently corroborates the target and alignment body. character-update-packet-dwarf.json establishes every offset of the packed35-byte packet_update_char/TPacketGCCharacterUpdate record.

UpdatePacket emits header19, numeric VID (excluding its CRC), four current parts, limited movement/attack speeds, state/affect flags, guild ID or zero, signed alignment divided by10 then narrowed to16 bits, PK mode and mount. Real entity PacketAround governs sector eligibility, observer suppression and recipient/self fanout. ShowAlignment copies real alignment or zero into displayed alignment and emits an update only when that displayed value changes. It retains real alignment.

The new guild.h preserves every old guild data field, full member/grade/war/create records and actual constructor/destructor declarations. Its inline GetID follows old non-const linkage _ZN6CGuild5GetIDEv, unlike the newer reference's const method. guild-dependency-metadata.json and guild-create-parameter-dwarf.json retain type evidence. Old CGuild occupies312 bytes; modern STL containers change its full layout, so full host guild ABI equivalence is not claimed. Standalone old records are checked with32-bit COW-string mode; all data-record member offsets and packet offsets are checked.

character_update_regression exercises actual character construction/stat reads, packed encoding, entity fanout, DESC::Packet and buffers. It checks absent-sector suppression, exact bytes and speed limits, zero/nonzero guild IDs, signed division/narrowing, show/hide no-op behavior, observer-only self output and descriptor-free NPC caps. Guild construction/destruction is an explicitly test-only DB lifecycle boundary using the full class; no production guild lifecycle stub is supplied. Existing CHARACTER teardown, quest/sector lifecycle and unused protocol/OS boundaries remain explicit. Guild DB/war behavior, complete item expiry/equipment teardown, startup and full server/Visual Studio build integration remain unfinished.

Validation checkpoint54: focused default/O2 integration, strict32-bit character syntax, complete record/packet ABI checks and all55 runtime executables pass. Full output is retained in work/checkpoint-54-validation.log in the task workspace.
