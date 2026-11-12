# Old hierarchical text loader and node pool

Target SHA256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

Full old CTextFileLoader data/API and method bodies are reconstructed in game/src/text_file_loader.h/.cpp, using the already recovered memory text loader. common/pool.h supplies the original-shaped node/pool template, common/d3dtype.h supplies plain vector/quaternion/color records, common/stl.h supplies the evidenced lowercase helper, and the old signed4-byte BOOL typedef is added to the core typedef header.

## Parser and lifecycle

The constructor starts at global root, line0, null data pointer, root name global and null root parent. Load changes filename/line index, reads binary bytes with stdio into a temporary array, binds the real memory loader, deletes the temporary bytes and parses into the existing global tree. File-open failure returns false after filename/index changes. It does not clear prior tokens/groups or reset the navigation cursor.

Group/list keywords, token keys and group names are lowercased while values retain case. Query methods use exact supplied keys/names. Groups recurse; lists flatten all row tokens; braces are recognized by the first token character. Map insertion is unique, so the first duplicate key wins; duplicate group names remain separate ordered child nodes. Repeated loads retain old values, append groups and preserve the current node cursor.

Malformed group arity logs the old diagnostics and exits1. A bare scalar key logs and stops that group, while Load still returns true. Unclosed groups at EOF and an early root closing brace retain their old acceptance/partial-parse behavior. Original memory-loader quoting, newline pairing and lack of special '#' comment syntax remain unchanged.

Navigation supports exact names, index and a32-byte formatted head+two-digit-index key. Current-node name returns false at root. Token vectors are mutable pointers to stored entries. Scalar getters accept the first token and use old atoi/atof, including partial/invalid conversion behavior and BYTE/WORD narrowing; BOOL is int, not normalized bool. DWORD preserves the integer getter's32-bit result. Vector/color getters require exact arity and retain output on failure; position delegates to vector3. Old diagnostic wording, including vector4/quaternion messages that say vector3, is retained.

Group nodes belong to one static CDynamicPool, not to individual loader lifetime. A loader destructor frees its own strings/containers but does not release pooled child nodes. DestroySystem clears the shared pool; the old pool destructor asserts that used/free lists were already cleared. Calling DestroySystem can invalidate tree pointers in still-live loaders, as in the target.

The full reference pool API is retained. Target data, inline allocation, Clear and destructor match the supplied reference (including old assertion lines40/41/164). Unused Free/FreeAll template bodies are reference-guided rather than separately addressed binary functions; runtime tests exercise their list/reuse/ownership behavior. Nodes preserve data across free/reuse and Clear deletes used and free nodes exactly once.

## Evidence and validation

text-loader-addressed-ghidra.json retains49 parser/compiler/helper bodies;33 are loader methods/variants and the rest are standard-container/pool/initialization helpers. It is not a claim of49 independent hand-written functions. text-loader-metadata.json records complete old loader68, group32, pool24, node44 and vector/color data. Whole loader/group/node sizes differ under modern STL; full host ABI equivalence is not claimed. Plain vector/color offsets and the generic pool's32-bit legacy-COW-string24-byte layout are checked in text_loader_abi.cpp.

text_loader_regression uses actual files, parser, memory loader and pool. It covers groups/lists/quotes, case rules, duplicate insertion, conversions, mutable vectors, failed outputs, reload retention, cursor behavior, deferred node ownership, pool free/reuse/clear, bare keys, EOF and closing-brace behavior. A separate reconstructed-test subprocess verifies malformed-group exit1 and exact diagnostics; it executes no original binary. text-loader-diagnostic-audit.json confirms no old getter diagnostic strings are missing, supplementing runtime tests.

Default and O2 tests pass; packed/plain/pool layout checks and strict32-bit parser syntax checks pass. Logging is a test boundary. Fixtures clean their temporary files; DestroySystem is explicitly called after grouped loader use. See validation-status.json for the completed full-suite checkpoint.
