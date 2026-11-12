# Read-only semantic and ABI audit

Production guild.cpp, guild_level_regression.cpp and guild_level_desc_output_syscalls.c matched the staged candidates byte-for-byte at audit time. No new semantic defect was found in the reviewed LevelChange/GetMember/constructor behavior. No production edit or build was performed.

Verified against original instructions/data:
- Both constructors leave _dummy storage untouched. Regression inspects unsigned-byte object representation at12 after prefilling placement storage; it does not evaluate the indeterminate _dummy scalar. Stored map records explicitly set _dummy before use/copy.
- Missing GetMember/LevelChange finds do not insert. Missing LevelChange returns before DB/client/dummy work, allowing the tested null DB pointer only on that branch.
- Present member level is written before DB call; DB payload snapshots level/grade/offer and uses requested map key. _dummy is cleared only after DBPacket returns. Client bytes read the live full member prefix, preserving distinct stored pid and post-header grade/offer mutation.
- Actual record layouts are14-byte DB payload,9-byte DB framing,4-byte client header and13-byte member prefix; client total17. Goldens match the saved evidence.
- Online iteration increments after sends, so the test correctly mutates fields only and does not erase iterators.
- DB sys_log caller passes unsigned header, then DWORD handle/size/buffer_size. DWORD is unsigned long in the current source; observer va_arg(unsigned)/va_arg(unsigned long) types match those actual arguments. It does not use malformed printf formatting to inspect values. Unexpected _sys_err remains fatal.

## ABI correction

The whole-class sizeof(CGuild)==312 assertion I staged was too strong for modern libstdc++. This is an ABI compatibility overclaim, not evidence that a logical field is missing. The corrected workspace guild_level_abi.cpp removes that assertion, retains all concrete record/wire-prefix checks, and adds exact member-pointer types for allTEN logical fields plusNINE relative nonoverlap/order checks. There are six full containers; no padding or reduced types were introduced.

Original DWARF offsets/sizes:
m_data0/220; general220/4; bonus224/4; member228/12; online240/12; P2P252/12; enemy264/12; war-end276/12; skill-bools288/12; invite-map300/12; total312.

Header-derived current GCC13.2 strict32 profile:
m_data0/220; general220/4; bonus224/4; member228/24; online252/24; P2P276/24; enemy300/24; war-end324/24; skill-bools348/12; invite-map360/24; total384.

All six old containers are12 bytes, including BOTH sets. Modern32 each is24: embedded _Rb_tree_header node-base16+count4 plus key-comparator/alignment4. Thus six*12 extra=72, giving384 exactly. Native Windows64 map/set48 plus two4-byte guild alignment gaps yields536. These are exact calculations from the installed normal GCC13 headers and scalar/alignment profile, not fresh compiler measurements. No debug-container, short-enum or structure-packing flags are assumed. The old-string ABI switch does not restore the old tree-header representation.

logical-layout-audit.json preserves complete field-name/order agreement, old/modern offsets, source-header hashes and calculation provenance. Every old CGuild logical field/type is present in production. Keep packet/record ABI claims separate from full STL object layout; do not manufacture padding to force312.

## Addressed export normalization

guild-level-addressed-ghidra.json provides twelve address-keyed complete Ghidra decompilations for reconstruction/ledger integration. It carries the exact target hash, program function count7238 versus exported count12, original requested/function addresses, code, truncation status, available signature and per-entry raw-export SHA/path provenance. Original raw structured-result files were left unchanged. Neither presence of evidence nor existing suite passes certify complete function/branch recovery.
