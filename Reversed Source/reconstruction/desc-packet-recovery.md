# Descriptor packet writing recovery

Scope: DESC::BufferedPacket (0x08139cb0), Packet (0x08139d10), LargePacket (0x0813a1d0), packed relay definition in game/src/packet.h, and partial inline encoder in game/src/protocol.h. Evidence: desc-packet-ghidra.json (three complete bodies), desc-packet-assembly.json (complete assembly), desc-packet-types.json (DWARF), tools/desc_packet_types.py. Target identity matches validation-status.json. Independent read-only review found no behavioral discrepancies.

Observed flow:
- BufferedPacket allocates 1024 bytes lazily and appends, independent of phase.
- Packet asserts positive size before the phase-zero early return. Closed phase retains buffered bytes and relay name.
- Ordinary packets append into an existing temporary buffer, then profile the combined packet using its first byte. Plain encoding requires existing capacity and does not request growth. Failure directly closes the phase. Buffered data is deleted and writable interest is registered even after ordinary failure.
- Encrypted packets reserve size+8 with original 32-bit wrapped signed comparison, copy then TEA-encrypt in place, and advance by any padding. Capacity failure logs three diagnostics and closes the phase. No encryption key generation is recovered by this slice.
- Relay packets use packed header=3, 25-byte name (24 characters plus terminator), and signed long payload length. Relay bypasses buffering, encryption and profiling. Header-capacity failure keeps the name; payload-capacity failure retains the already written header and clears the name. Neither failure registers output. Pending buffered data survives either relay success or failure.
- LargePacket logs and requests buffer growth before calling Packet, including before the closed-phase early return.

Modern std::string APIs replace compiler-generated GCC COW details. Packed relay layout is checked as 30 bytes, offsets 0/1/26. No binary equivalence or original-runtime validation is claimed. Existing production Destroy/SetPhase remain unresolved, with explicit test-only bodies.

Verification results are recorded after the accompanying tests complete in validation-status.json. The packet integration suite links reconstructed buffer, TEA, watcher/socket and descriptor code; configuration, OS calls and unrecovered lifecycle remain explicit fixture boundaries.

Validation completed: all24 suites and structural ABI checks pass. Packet integration passes default and O2 against actual recovered descriptor/buffer/TEA/profiler/watcher/socket/utils. Tests verify phase gating, two buffered appends and flush, real socket transmission, plaintext and encrypted capacity failures, ordinary failure cleanup/rearm, exact-capacity admission (underlying owned buffer then grows), encryption with a preceding output prefix, relay truncation and partial failures, growth before closed-phase return, combined-header profiling, and profiling before capacity failure. Profiler has its own regression suite. Strict32 source syntax compilation passes. Test fixtures still replace configuration storage, OS boundaries, game clock, Destroy and SetPhase.
