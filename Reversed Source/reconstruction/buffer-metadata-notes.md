# Buffer metadata recovery

Read-only evidence: original binary SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. `extract.py` reads the buffer.c DWARF compilation unit directly using pyelftools; `metadata.json` retains all function DIE attributes/type chains and structure fields. `globals.py` reads ELF symbols; `globals.json` records pool head locations. No target execution.

Observed public signatures (all C, no reference parameters):
```
LPBUFFER buffer_new(void *dest, int size);
void buffer_delete(LPBUFFER buffer);
DWORD buffer_size(LPBUFFER buffer);
void buffer_reset(LPBUFFER buffer);
void buffer_write(LPBUFFER buffer, const void *src, int length);
void buffer_read(LPBUFFER buffer, void *buf, int bytes);
BYTE buffer_byte(LPBUFFER buffer);
WORD buffer_word(LPBUFFER buffer);
DWORD buffer_dword(LPBUFFER buffer);
const void *buffer_read_peek(LPBUFFER buffer);
void buffer_read_proceed(LPBUFFER buffer, int length);
void *buffer_write_peek(LPBUFFER buffer);
void buffer_write_proceed(LPBUFFER buffer, int length);
int buffer_has_space(LPBUFFER buffer);
void buffer_adjust_size(LPBUFFER buffer, int add_size);
void buffer_realloc(LPBUFFER buffer, int length);
```

The first `buffer_new` parameter is real, corroborated by Ghidra at 081b7800: nonnull dest enters the external-storage branch and sets flag bit1. It is not a compiler-hidden argument. Parent separately confirmed EBP+8/EBP+c assembly accesses.

Observed 32-bit struct buffer layout: size32, next0 pointer, write_point4 char*, write_point_pos8 signedint, read_point12 constchar*, length16 signedint, mem_data20 char*, mem_size24 signedint, flag28 signedlong. int/long/pointers4 bytes; DWORD is unsignedlong4, WORD unsignedshort2, BYTE unsignedchar1.

Observed ELF local static symbols: buffer_poll0842d31c, buffer_poll_big0842d320, buffer_nodelete_poll0842d324, each4 bytes in section20 .bss SHT_NOBITS (zero initialized). Unlike newer reference's 32 normalized pool entries, this binary has three single linked-list head variables. Pool split threshold8192 is established by buffer_new's size<0x2001 comparison.

Key differences from supplied newer reference header: buffer_new has external-storage dest; write/adjust/realloc take LPBUFFER by value, not LPBUFFER&; getter names are buffer_byte/word/dword, not buffer_get_byte/word/dword. Struct field layout agrees. The legacy binary DWARF provides no variables in this CU, hence global declarations/types rely on ELF symbol sizes, decompiled use and layout inference rather than source variable DIEs.

Limitations: this metadata pass establishes types and static layout; the parent is reconstructing function behavior and independently checking assembly/tests. Source spelling/macros/comments are not recoverable from this pass.
