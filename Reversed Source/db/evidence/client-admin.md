# ClientManager building and admin reconstruction

Target is unpacked 2009 FreeBSD/i386 db ELF, SHA-256 `85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`. Headless Ghidra exports in `work/db-headless/decompiled` were checked against symbols, DWARF, disassembly and the newer source.

| Routine | Address | Evidence |
| --- | --- | --- |
| CreateObject | `0x0805a9e0` | Direct INSERT, positive insert ID required, broadcast 40-byte object with life zero on header 140 before inserting map entry. |
| DeleteObject | `0x0805ad90` | Direct DELETE, positive affected rows required, removes mapped object when present, broadcasts original four-byte ID on header 141. |
| UpdateLand | `0x0805b070` | Finds original 36-byte land record, sends asynchronous guild update, mutates cached record and broadcasts it on header 142. |
| __GetAdminInfo | `0x08060270` | Common SQL query for ALL or requested server IP. Resizes to row count and leaves unknown-authority positions unassigned. Authorities IMPLEMENTOR/GOD/HIGH_WIZARD/WIZARD/LOW_WIZARD map to 5/4/3/2/1. Keeps original account case. |
| __GetHostInfo | `0x08060850` | Common SQL host SELECT, resizes to row count and preserves row order. Both admin queries fail when no rows are returned. |
| ReloadAdmin | `0x08060c70` | Header 158 to nonzero-channel peers. WORD pair for size/count of 16-byte host records, hosts, WORD pair for size/count of 104-byte admins, admins. Sends even if a query returned false. |
| QUERY_RELOAD_PROTO | `0x08059710` | Reloads tables and broadcasts header 120 to nonzero-channel peers. WORD counts and raw vectors in original skill, banword, item, mob order. |

For admin strings, the reconstruction copies up to the original field widths and pads shorter values. The old binary reads fixed widths directly from MySQL row strings and host c_str storage, which could include unrelated bytes past a terminator. The protocol widths remain unchanged.

Wire request DWARF DIE `0xf5a9` identifies the 32-byte create object layout. DIE `0x102cb` identifies the 16-byte reload admin IP. Merge staged source `work/client-admin.cpp` into ClientManager.cpp with matching member and packet declarations. No builds or runtime tests were performed.
