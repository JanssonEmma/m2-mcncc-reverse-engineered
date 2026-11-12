# Descriptor helpers and cleanup prerequisites

Recovered 18 additional addressed DESC methods in game/src/desc.cpp, with declarations in desc.h. Complete per-method pseudocode is retained in desc-helpers-ghidra.json; Log and CheckMatrixTryCount have focused assembly corroboration in desc-helpers-assembly.json.

Observed behavior: Log ignores calls without a file. With a file it uses game get_global_time(), copies localtime, writes month/day/time prefix, formats variadic content directly to the file, appends a newline and flushes. It has no formatting buffer or truncation limit. The game clock is declared only in partial game/src/utils.h; its body remains unresolved. Descriptor tests use an explicitly isolated fixed-clock fixture.

CheckMatrixTryCount increments and stores a BYTE before testing <3. Therefore calls 1/2 pass, 3..255 fail, then wrapped values 0/1/2 pass. No saturation or lifetime lockout was added.

CLoginKey is recovered in desc.h, matching reference file placement. DWARF establishes 12 bytes and offsets 0/4/8; CreateLoginKey at 0x08159340 shows constructor field assignments; Destroy at 0x08139340 shows inlined Expire. Conversion returns the key, corroborated by DESC::GetLoginKey. The attached key takes precedence over scalar fallback; expiration clears its descriptor and records get_dword_time(), without changing the key or the descriptor's attached pointer. SetLoginKey(pointer) retains the old unconditional dereference for logging; it is not a null-detach API.

Validation: extended existing descriptor unit suite checks logging null path, prefix, varargs and 2048-byte untruncated payload; 258 consecutive matrix attempts; login-key construction, attached precedence, mutation visibility and expiration. Unit suite passes default and O2; real output integration passes. All ABI checks pass, including CLoginKey size and offsets. These are reconstructed-code tests, not execution of the original binary or FreeBSD runtime validation.

Cleanup prerequisites remain partial. desc-cleanup-dependencies.md and desc-cleanup-types.json record old logout packet, manager, character and database signatures and layouts. Destroy and SetPhase still have test-only bodies, not production substitutes. No complete server link or binary equivalence is claimed.

Independent read-only review found no discrepancies against saved binary evidence. Strict 32-bit source syntax compilation also passes with -Werror and the existing GNU long-long extension exception.
