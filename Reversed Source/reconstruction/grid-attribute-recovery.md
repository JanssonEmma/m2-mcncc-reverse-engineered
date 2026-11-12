# CGrid and CAttribute recovery

Target: `game`, SHA-256 `61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c`, x86 little-endian 32-bit GCC ELF. Read-only Ghidra identity confirmed before inspection. Full retained pseudocode for all 27 emitted functions is in grid-attribute-ghidra.json. No original target execution or Ghidra database changes.

The newer `REFERENCE_PROJECT/Source/srv/server/libgame` files supplied reference syntax. Every implementation was checked against target pseudocode. These are behavior-oriented reconstructions, not claims of exact original source text. Requested placement retains `libgame/src/*.cc` and `include/*.h`; target assertion names `attribute.cpp`.

## Observations and differences

* CGrid: 13 emitted functions, 10 source-level methods/constructor overloads, addresses 081f4cb0 through 081f5150. Constructors and destructor each have ABI duplicate bodies. Layout is width at 0, height at 4, grid pointer at 8; no vtable. The copy/resize constructor copies `min(old area,new area)` contiguous bytes using thecore_memcpy, without row repacking or initialization of an expanded tail.
* CGrid preserves original edge behavior: IsEmpty rejects negative positions and crossing the right or bottom edge; Get only validates the starting position. Width zero still accesses the first cell for a positive height, while height zero performs no access. Full rectangle safety is a caller precondition for Get. No new validation was added.
* CAttribute: 14 emitted functions, 11 source-level methods/constructor overloads, 081f5180 through 081f59d0. Dimensions, coordinates, and CopyRow row are signed **int**, unlike the newer unsigned-DWORD reference. Target Get assembly at 081f597d..081f5985 uses JG/JLE signed comparisons. Its full assembly is retained in attribute-get-assembly.json. Layout: int dataType 0; DWORD defaultAttr 4; int width 8, height 12; data pointer 16; BYTE** 20, WORD** 24, DWORD** 28. Expected target size 32 bytes; pointer-bearing host size differs.
* Attribute type IDs are DWORD=0, WORD=1, BYTE=2. Uniform input has no allocated buffer and retains the default value. Mixed input chooses byte/word/dword from the OR of all attributes. Set ORs flags without promoting a compressed allocation, so wider flags truncate; Remove ANDs the complement and does nothing on a uniform unallocated map. Set on a uniform map lazily allocates DWORD storage filled with its default.
* Bounds use strict `x > width || y > height`, without checking negative coordinates, and CopyRow has no row check. Those defects are preserved. Tests exercise equal/negative coordinates only on an unallocated uniform map, where no invalid memory access occurs. Zero/negative dimensions, invalid allocated coordinates, arithmetic overflow, allocation failure, and C++ implicit shallow copies remain unsupported caller inputs. No hardened variants were substituted.

## Validation

Both focused regressions compile warning-free with `C:/Strawberry/c/bin/g++.exe -std=c++11 -Wall -Wextra` and exit 0 on the Windows x64 MinGW host (DWORD remains 32 bits). Both implementation units also compile with `-m32 -c`. Tests cover rectangular occupancy/search/removal, bounds, copy independence, linear resize semantics, zero dimensions of operations, all storage formats, row conversion, lazy allocation, bit removal/truncation, and signed comparison behavior without invoking invalid memory accesses.

These checks execute reconstructed source only. They are not binary differential tests or proof of FreeBSD ABI/runtime compatibility. No expanded-copy uninitialized bytes are read. Print is matched statically to `Grid %p\n`, decimal cell bytes and row newlines; it is not captured by regression.

Integration commands from the reconstructed tree root:

```
g++ -std=c++11 -Wall -Wextra -I. reconstruction/tests/grid_regression.cpp libgame/src/grid.cc -o reconstruction/tests/grid_regression
g++ -std=c++11 -Wall -Wextra -I. reconstruction/tests/attribute_regression.cpp libgame/src/attribute.cc -o reconstruction/tests/attribute_regression
```

Each regression supplies its own thecore_memcpy pointer backed by libc memcpy, so do not additionally link the production selector implementation into those test executables.
