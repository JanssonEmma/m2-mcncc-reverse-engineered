# Input and phase recovery

Evidence: input-core-ghidra.json, input-handshake-ghidra.json, input-concrete-ghidra.json, desc-controls-ghidra.json, desc-input-ghidra.json, desc-flush-ghidra.json, character-name-ghidra.json and the associated DWARF layout files.

The base input class is12 bytes on the target; concrete handshake adds one packet-table pointer at12, while login/main/dead/auth add no members. Modern containers affect pointed-to registries, not these input class layouts. All application Analyze implementations are still declaration-only. Empty concrete destructors are evidenced and deliberately do not free the handshake's private registry.

SetPhase0813a240 initializes six local static processors. It stores the phase, emits packed253/phase bytes through Packet, then changes processor and enables encryption for phases2/3/4/5/6/10. Phase6 uses main; dead is constructed but never selected here. Unknown phases retain prior processor. Close sends no bytes because Packet sees phase0; close/handshake retain encryption. Shared static initialization uses normal C++ source lifetime semantics, with current compiler guards rather than GCC3.3's exact machine implementation.

Tests execute real reconstructed packet/buffer/TEA/input/descriptor methods under OS fixtures. Concrete Analyze, DESC::Destroy and character lifecycle/virtuals are explicitly isolated test substitutes. Phase counting is a test subclass override that forwards to the real base implementation. No runtime success is claimed for unrecovered application handlers.
