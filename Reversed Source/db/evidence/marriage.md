# Marriage manager reconstruction

Target is the unpacked FreeBSD/i386 ELF with SHA-256
`85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`.
The newer reference supplies the corresponding source layout and the binary
determines control flow, ABI and string constants.

DWARF `TMarriage` (DIE `0x438dc0`) occupies 28 bytes in the original ABI.
The player IDs are DWORDs at offsets 0 and 4. The love point is an int at
8, time is DWORD at 12, the married flag is BYTE at 16, and the two strings
start at 20 and 24. `CManager` is 72 bytes with the singleton base at 0,
the marriage set and PID map at 4 and 16, start and end queues at 28 and 44,
and the running wedding map at 60. The compiler and STL determine the
recompiled container layout. The four SQL mutation functions use 513-byte
stack arrays (DIE `0x45efd6`) and pass 512 to `snprintf`. Initialization
uses 1024 for both the array and the call.

| Routine | Original address | Recovered behavior |
|---|---|---|
| `operator<`, `operator>` for `TWedding`, `operator>` for `TWeddingInfo` | `0x0808beb0`, `0x0808bed0`, `0x0808bef0` | Compare the unsigned timestamps. |
| Constructor and destructor | `0x0808bf10`, `0x0808c350`, `0x0808c5c0`, `0x0808c7a0`, `0x0808c980` | Base and container lifecycle with no separate record cleanup in the destructor. |
| `Initialize` | `0x0808cb60` | Deletes uncompleted marriages before the original SELECT. Keeps both auto_ptrs until return, logs row count twice when positive, uses atoi for IDs, love points and flag, and strtoul base 10 for time. Inserts into the owner set and both PID keys. |
| `Get` and `Align` | `0x0808d130`, `0x0808d180` | Lookup by PID and ascending ID normalization. |
| `Add` | `0x0808d1a0` | Reads current time, rejects a duplicate PID, aligns IDs without swapping names, then executes the INSERT. On positive affected rows it inserts the record and broadcasts Add. |
| `Update(DWORD, DWORD, long, BYTE)` | `0x0808d710` | Requires the two IDs to belong to the same marriage, aligns IDs for the broadcast, updates using the record's stored IDs and writes love point/status on positive affected rows. |
| `Remove` | `0x0808daa0` | Logs a mismatched relationship and lists existing records, otherwise aligns IDs, deletes the SQL row, erases all three indices, broadcasts removal and deletes the record. |
| `EngageToMarriage` | `0x0808df20` | Rejects mismatched or already-married records, updates SQL status and sends the marriage update. |
| `OnSetup` | `0x0808e2b0` | Emits Add and Update for existing marriages. Running weddings send the same 12-byte Ready payload twice, using header 154 then 155. |
| `ReadyWedding` and `EndWedding` | `0x0808e4d0`, `0x0808e5e0` | Schedules start at now plus five seconds. Explicit end looks up the exact ordered PID pair, sends end, then erases the running record. |
| Timed `Update()` | `0x0808e720` | Handles due ends before starts. End sends 8 bytes and erases the running wedding before reading its PID for the marriage lookup, preserving the original dangling-reference access. Start sends 8 bytes, adds 3600 seconds to the scheduled timestamp, queues the end and inserts the running record. |

Wire evidence is directly visible in calls to `ForwardPacket` and
`CPeer::EncodeHeader`. Add 150 is 62 bytes, Update 151 is 13,
Remove 152 is 8, Ready 154 is 12, timed Start 155 is 8, and End 156 is 8.
Setup Start 155 is the original 12-byte exception. Add is two 32-bit
PIDs, signed 32-bit original time_t and two 25-byte name arrays. Both
names are copied with `strncpy` for 24 bytes and explicitly terminated.
The DB source uses local fixed-width packet definitions for types absent
from the shared protocol. Update and Ready reuse the declarations already
in shared common/tables.h.

Integration requires `CClientManager::GetCurrentTime`,
`CClientManager::ForwardPacket`, `CDBManager::DirectQuery`, and the original
dispatch and periodic marriage hooks in ClientManager. The original SQL
strings and format widths were recovered from `0x080cc73e` through
`0x080cccaf`. The original binary was inspected statically. Build and
runtime validation are deferred.
