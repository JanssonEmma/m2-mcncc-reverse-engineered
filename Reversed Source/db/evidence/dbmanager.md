# CDBManager reconstruction

Target is the unpacked FreeBSD/i386 ELF with SHA-256
`85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`.
The newer source supplies file and member names. The original ELF supplies
disassembly, DWARF layouts and string constants.

DWARF type `0xe793e` gives a 56-byte `CDBManager`, with the singleton
base at offset 0, four-element `CAsyncSQL2*` arrays at offsets 4, 20
and 36, and the unused `int m_quit` at offset 52. Initialization leaves
`m_quit` alone. `CQueryInfo` has 12 bytes with `iType`, `dwIdent`
and `pvData` at offsets 0, 4 and 8. DWARF `eSQL_SLOT` has values
`SQL_PLAYER=0`, `SQL_ACCOUNT=1`, `SQL_COMMON=2`,
`SQL_HOTBACKUP=3`, and `SQL_MAX_NUM=4`.

| Method | Original address | Static evidence |
|---|---|---|
| Constructor | `0x0804ee30`, `0x0804ee90` | Initializes singleton and SQL slots. |
| Destructor | `0x0804eef0`, `0x0804ef50`, `0x0804efb0` | Destroys SQL slots, then generated base cleanup. |
| `Initialize` | `0x0804f020` | Nulls main, direct and async in each slot. |
| `Destroy` | `0x0804f050` | Calls `Clear`. |
| `Clear` | `0x0804f060` | Deletes main, direct and async in each slot, then initializes. |
| `Quit` | `0x0804f0f0` | Quits main, async and direct in each slot, with null checks. |
| `PopResult` | `0x0804f150`, `0x0804f1b0` | Uses main SQL in ascending slot order or a selected slot. |
| `Connect` | `0x0804f1f0` | Rejects null host or database and invalid slots. Creates direct, main and async in that order, clearing all slots on failure. The port parameter exists but is unused by all three six-argument Setup calls. |
| `DirectQuery` | `0x0804f3e0` | Dispatches directly to the selected direct connection. |
| `ReturnQuery` | `0x0804f400` | Checks only the upper slot bound with an assertion, allocates query metadata and increments `g_query_count[0]` after dispatch. |
| `AsyncQuery` | `0x0804f480` | Checks only the upper slot bound with an assertion and increments `g_query_count[1]` after dispatch. |
| `EscapeString` | `0x0804f4e0` | Same assertion, then `mysql_real_escape_string` using the direct connection handle. |
| `FlushQueryQueue` | `0x0804f570` | Dumps the main queue to a file, clones direct settings into a temporary `CAsyncSQL2`, reopens the dump, removes the last semicolon in each line and executes the resulting SQL synchronously. The original assumes that every line has a semicolon. DWARF describes a 32769-byte buffer and the binary passes 32768 to `fgets`. |
| `SetLocale` | `0x0804f860` | Calls `CAsyncSQL2::SetLocale` on main, direct and async for each slot, constructing a string for each call. The original has no per-slot null checks. |

The original header declares eight inline counters and `ResetCounter` in
DWARF at `0xe7b81` through `0xe7cc1`. The counters use the reference
definitions. The main/async reset order is corroborated by the inlined calls
in `CClientManager::Process`. The newer `QueryLocaleSet` member is absent
from the original class and symbol table.

Integration needs the 2009 `stdafx.h` contract for the singleton, aliases and
logging. `g_stLocale` and `g_query_count[2]` are supplied by the surrounding
DB modules. The shared `libsql` snapshot has the six-argument Setup overload and SQL message cleanup. The existing empty DumpQueryQueue matches the five-byte no-op at 0x080bf280. The declaration and implementation of CAsyncSQL2::SetLocale are still needed. Build and
runtime validation are deferred.
