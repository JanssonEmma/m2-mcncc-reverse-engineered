# GOST recovery

All three emitted functions were compared with the target; gost-ghidra.json, gost-constants.json and gost-round-schedule.json retain the evidence. All eight active substitution tables match the supplied reference, and the 32 key indices are 0..7 repeated three times then 7..0. The code is reference-guided, with explicit unsigned shifts to preserve target bit operations on modern compilers.

Encryption transforms the feedback block and XORs plaintext, feeding ciphertext into the next block. Decryption feeds the source ciphertext. The supplied IV is not modified. In-place operations are supported.

Length handling preserves the emitted loop: it tests Length >> 2 while stepping two DWORDs at a time. Length 4 processes eight bytes; length 9 processes eight; length 12 processes sixteen. No new padding or range checks were added. Callers must provide the storage the original algorithm accesses. Return value is original length converted to int.

Regression checks use a fixed ciphertext calculated independently from target tables, round trips, both in-place operations, IV preservation and partial-length behavior with sufficiently backed buffers. No original binary execution or broader cryptographic security claim is made. DWORD must be four bytes.
