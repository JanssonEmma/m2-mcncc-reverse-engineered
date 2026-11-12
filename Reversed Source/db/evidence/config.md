# CConfig reconstruction

The analysis target was the unpacked FreeBSD/i386 ELF at
`PROJECT_ROOT\Original Binaries\db-2009-unpacked.elf` with SHA-256
`85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`.
The newer `REFERENCE_PROJECT\Source\srv\server\db\src` files supplied naming and
structure. The original ELF supplied the control flow, types and constants.

The first DWARF v2 compilation unit identifies `Config.cpp` and GNU C++ 3.3.3.
The `CConfig` type DIE at `0x12146` describes a 16-byte class. Its public
`singleton<CConfig>` base occupies offset 0 and has a vptr. The private
`m_valueMap` begins at offset 4 and has type `TValueMap`, with
`std::string` keys and values. The destructor is virtual through this base.
DIE `0x12292` and mangled symbol `_ZN7CConfig8GetValueEPKcPci` confirm the
original signed `int` buffer length.

| Method | Original address | Static evidence |
|---|---|---|
| Constructor | `0x0804acd0`, `0x0804ae10` | Base and map initialization only. |
| Destructor | `0x0804aeb0`, `0x0804af80`, `0x0804b050` | Calls `Destroy` before generated member and base cleanup. |
| `Destroy` | `0x0804b120` | Clears the map through its red-black tree. |
| `NextLine` | `0x0804b180` | Stops at LF or EOF. |
| `GetWord` | `0x0804b1b0` | Skips CR, handles leading delimiters and quoted words. A partial token at EOF is returned without a trailing NUL. |
| `GetLine` | `0x0804b250` | Copies until LF or EOF, always returns true and leaves the destination unterminated. |
| `LoadFile` | `0x0804b2a0` | Opens `rb`, skips a line on exact token `//`, stores a key using `strcpy`, advances on the first byte `=`, and inserts with first-value-wins semantics. `ITEM_ID_RANGE` reads the remainder of its line into the token buffer. |
| `Search` | `0x0804b680` | Returns the mapped string address, or null on a miss. |
| `GetParam` | `0x0804b710` | Reads five unbounded `%s` fields into 32-byte slots, calls `atoi` on the selected slot and logs `GetParam %d` at level zero. |
| `Get` | `0x0804b7d0` | Returns the mapped `c_str()`, or a function-static empty string's `c_str()`. |
| `GetValue(int*)` | `0x0804b860` | Searches first, then uses `atoi` on `Get(key)`. |
| `GetValue(float*)` | `0x0804b8b0` | Searches first, then uses `atof` with a float store. |
| `GetValue(DWORD*)` | `0x0804b900` | Searches first, then stores the `atoi` result as the original 32-bit unsigned long. |
| `GetValue(BYTE*)` | `0x0804b950` | Searches first, then stores the first string byte. |
| `GetValue(char*, int)` | `0x0804b9a0` | Calls `strncpy` for the requested length, writes NUL at that index, then checks whether byte zero is nonzero. |
| `GetTwoValue` | `0x0804ba10` | Calls `GetParam` for indices zero and one, returning early on failure. |

The original unchecked buffer behavior is retained. The newer source's
`strlcpy` and `size_t` variant were excluded based on the ELF. Source text
and formatting are reconstructed. Compiler-generated ABI routines and tree
internals are left to the C++ implementation. The modern STL's layout is
toolchain-dependent. Build and runtime validation are deferred.
