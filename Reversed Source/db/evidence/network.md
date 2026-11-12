# DB 2009 networking reconstruction

The target is the unpacked FreeBSD/i386 ELF at `PROJECT_ROOT\Original Binaries\db-2009-unpacked.elf`. SHA-256 `85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`. Source organization and candidate names came from the newer `REFERENCE_PROJECT\Source\srv\server\db\src` tree. Function bodies were checked against `objdump -d -C`, symbols and `.rodata`. DWARF data describes the original types and declaration sites.

| Module | Binary evidence | Recovered scope |
| --- | --- | --- |
| NetBase | `0x0804c230` through `0x0804c580` and zero-initialized watcher at `0x080d3010` | `CNetBase` lifetime and shared watcher. `CNetPoller` lifetime, creation with 512 descriptors, destroy and unconditional `thecore_destroy`. |
| PeerBase | `0x0804cc10` through `0x0804d428` and DWARF `0x633c7` | Lifetime, disconnect and destroy, accept and connect, close dispatch, encoding, receive and send. The class occupies 52 bytes on the original ABI. Host, socket, unread-byte counter and buffer offsets are 4, 36, 40, 44 and 48. |
| Peer | `0x0804c660` through `0x0804cc08` and DWARF `0x51e19` | Lifetime, connection state callbacks, handle and count, packet framing, send, port and map handling, and item-range methods. The class occupies 116 bytes on the original ABI. |

- Accept configures the socket buffers to 233016 bytes, copies 30 characters with `strncpy` and writes a terminator at index 30. Both 2 MiB buffers use the original `buffer_new(NULL, size)` API. Connect allocates only the outgoing buffer. The later loopback filter and extra accept log have no corresponding paths in the target.
- `CPeerBase::Send` takes the minimum of the watcher capacity and queued bytes. The empty check tests equality with zero. It consumes queued data after a zero socket return and rearms the write notification when bytes remain.
- `CPeer::HEADER` is packed to 9 bytes with fields at offsets 0, 1 and 5 (DWARF `0x51e29`). The parser publishes the header fields even when the payload is incomplete. Fixed-size `memcpy` expresses the original unaligned reads.
- `CPeer::SetMaps` copies 32 bytes to `m_abMaps` at offset 72. DWARF `0x4e5f8` identifies its 32-byte array. The original range at offset 104 has two DWORDs, `start` and `end`, and no spare range. `SetItemRange` and the reversed-range guard in `IsItemRangeCollision` compare unsigned values. The overlap checks use signed comparisons with strict endpoints. Close logs and changes state.
- `ProcessInput` is a declaration in DWARF `0x5213a` without a corresponding code symbol in this ELF. The header retains its declaration. No implementation is inferred.

The code expects the shared 32-bit `DWORD`, libthecore networking/buffer interfaces, and DB `stdafx.h`. Original source spelling and formatting are inferred. Builds and runtime validation are deferred to the later integration phase.
