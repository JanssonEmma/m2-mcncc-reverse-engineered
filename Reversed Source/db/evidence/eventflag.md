# ClientManager event flags

Target is the unpacked FreeBSD/i386 ELF with SHA-256
`85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`.
Restored the three source methods in `ClientManagerEventFlag.cpp`.

| Method | Original address | Static evidence |
|---|---|---|
| `LoadEventFlag` | `0x0808b320` | Issues the original PID-zero SELECT on the default direct DB slot. The DWARF local at `0x4259b9` owns the message as `std::auto_ptr<SQLMsg>`. For each row it copies 32 name bytes with `strncpy`, writes NUL at byte 32, parses with `atoi`, logs, performs unique insertion and broadcasts a 37-byte packet. |
| `SetEventFlag` | `0x0808b6c0` | Broadcasts even if the value is unchanged. An insertion or changed value issues the original REPLACE on the default async DB slot. The original appends an explicit NUL at query index 1024 and uses the exact changed/unchanged format strings. |
| `SendEventFlagsOnSetup` | `0x0808b960` | Sends each flag in map order to the peer with header 105, handle zero and length 37. Uses the same 32-byte name copy and explicit terminator. |

Both query arrays are `char[1025]` in DWARF type `0x425e26`.
The binary passes 1024 to both `snprintf` calls. No new guard was added
for an absent query result or malformed row, matching the original.

The shared packet is `TPacketSetEventFlag` (`SPacketSetEventFlag`, DIE
`0x408421`), with a 33-byte name at offset zero, a signed 32-bit `long` at
offset 33, and one-byte packing. This declaration already exists in the
shared restored `common/tables.h`. The outgoing header is
`HEADER_DG_SET_EVENT_FLAG = 105` from all three call sites. The receiving
packet is `HEADER_GD_SET_EVENT_FLAG = 55`, whose jump table entry points to
the call site at `0x0805cab1`.

## ClientManager integration

Add the following to the owning `CClientManager` declaration, retaining
the visibility of its event flag helpers and map alongside the other client
manager state. The original red-black tree for this map is accessed at
`this + 0x144`.

```cpp
void LoadEventFlag();
void SetEventFlag(TPacketSetEventFlag* packet);
void SendEventFlagsOnSetup(CPeer* peer);

typedef std::map<std::string, long> TEventFlagMap;
TEventFlagMap m_map_lEventFlag;
```

Call `LoadEventFlag` during initialization (original call `0x08053fb6`),
`SendEventFlagsOnSetup` after sending privileges (call `0x08057c27`), and
`SetEventFlag` from the incoming header 55 dispatcher (call `0x0805cabb`).
The forwarding dependency is
`ForwardPacket(unsigned char, const void*, int, unsigned char, CPeer*)`
at `0x0805e9c0`. It receives channel zero and a null excluded peer for these
broadcasts. Ensure the original header constants are declared in the common
protocol header, then integrate with `Main.h`, `DBManager.h` and `Peer.h`.
Build and runtime validation are deferred.
