# Initial connection and boot

Original unpacked DB ELF SHA-256 `85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`. The three methods are staged in `work/client-setup.cpp` for integration with `ClientManager.cpp`. Used original symbols and isolated Ghidra exports at `0x08057720` and `0x08054580`, along with DWARF and the existing 2009 shared input declarations.

## QUERY_SETUP, 0x08057720

The packed input header is 58 bytes. Public IP is at 0, channel at 16, listen/P2P ports at 17/19, 32 map bytes at 21, login count at 53 and auth flag at 57. Each following login record is 90 bytes, with account ID at 0, login at 4, social ID at 35, host at 54, key at 70 and four client keys at 74. The auth branch registers the peer, sends all billing logins and returns before reading any game-peer fields.

Game-peer setup stores the announced identity and produces these lists in peer-list order. Channel-zero peers and the incoming peer are skipped in the existing-peer loop. The incoming peer's record is appended to its own list.

| Incoming channel | Existing records sent to incoming peer | Existing peers notified about incoming peer |
| --- | --- | --- |
| 1 | Channel 1 and 99 | Channel 1 and 99 |
| 99 | Channel 1 and 99 | All nonzero channels |
| Other | Same channel and 99 | Same channel |

| Outgoing opcode | Exact payload |
| --- | --- |
| 254 | One-byte record count, then 50-byte map records. The 32 map bytes, 16-byte host and 2-byte port begin at offsets 0, 32 and 48. DWARF name DIE `0xd0ac`. |
| 255 | 19-byte P2P address, with 16-byte host, two-byte port and channel byte at offsets 0, 16 and 18. DWARF name DIE `0xd151`. For each existing ready peer, the original random call chooses which peer receives the other peer's address. |
| 128 | Four-byte record count, then 51-byte billing repairs sent to the auth peer. Key, 31-byte login and 16-byte host begin at offsets 0, 4 and 35. DWARF name DIE `0xf392`. |

The old login import creates a CLoginData, sets keys/IP, copies and terminates account fields and sets the temporary password before inserting the login. Successful logons set play and populate billing repairs. The final callbacks are party, guild, privileges, event flags and marriage, in that order.

## QUERY_BOOT, 0x08054580

The boot request is 24 bytes with two 32-bit item ID range values and a 16-byte IP. Opcode 43 sends handle zero and boot version 5. Its first fields are the four-byte total size and one-byte version. Each of the following table sections has two two-byte fields for record size and count.

| Order | Section | Original record size |
| --- | --- | --- |
| 1 | Mob | 255 |
| 2 | Item | 154 |
| 3 | Shop | 449 |
| 4 | Skill | 1475 |
| 5 | Refine | 53 |
| 6 | Item attribute | 69 |
| 7 | Rare item attribute | 69 |
| 8 | Banword | 25 |
| 9 | Land | 36 |
| 10 | Object prototype | 96 |
| 11 | Object, in map order | 40 |

Next are signed four-byte server time and four-byte maximum item ID. The original calls `__GetMaxItemID` with the peer and request range, sends the returned value even on zero, and sets the peer's range only when nonzero. This precedes sized/counted 16-byte GM hosts, 104-byte admin records, two legacy Monarch sections and the final word 65535.

The wire layout keeps the excluded Monarch sections structurally valid by emitting one zero-filled 304-byte record and zero 68-byte candidate records. The resulting fixed overhead is the original `0x17b` bytes. No Monarch state or functionality is implemented. The staged code also zero-pads short GM host names to avoid the original fixed-length read past the string, and supplies deterministic operands for two original diagnostics that had a missing format argument. These changes do not alter any record sizes or field order.

## QUERY_PLAYER_COUNT, 0x08055670

The method sets the receiving peer's user count directly from the single incoming DWORD.

## Integration

Required headers include LoginData.h, GuildManager.h, PrivManager.h, Marriage.h, Peer.h, vector and cstdint. The manager class needs the three method declarations, `__GetHostInfo(std::vector<std::string>&)`, `__GetAdminInfo(const char*, std::vector<tAdminInfo>&)`, `__GetMaxItemID(CPeer*, DWORD, DWORD)`, `SendAllLoginToBilling()`, `InsertLoginData(CLoginData*)`, `InsertLogonAccount(const char*, DWORD, const char*)`, `SendPartyOnSetup(CPeer*)`, and `SendEventFlagsOnSetup(CPeer*)`. The local output structs are deliberately separate from the shared headers. Static layout checks enforce the binary's original table sizes when the integrated source is compiled. Build and runtime validation remain deferred.

## Item ranges and party replay

- `__IsItemRangeCollision` `0x08055240` checks all peers except the current peer and returns true on the first collision. Its final call on the current peer passes the manager's global start/stop and discards the result, as in the ELF.
- `__GetMaxItemID` `0x0805bbf0` rejects a zero pair or a peer collision, queries the maximum in the inclusive SQL range, treats a null SQL maximum as the range start, and rejects maximum equal to the end. It then increments the maximum, scans cached payload IDs (without refreshing their timestamps), considers only IDs strictly inside the requested endpoints, and checks that the remaining inclusive SQL interval is empty. It preserves the differing end tests of the SQL and cache branches, the original decimal conversions, the 256-byte query buffer and the original messages.
- `SendPartyOnSetup` `0x080552d0` indexes the selected channel map and emits all parties in key order. Each leader gets opcode 59 with four bytes. Each member gets opcode 61 (leader, member, role) and opcode 65 (leader, member, level), nine bytes each and both with handle zero.
- These methods additionally require Cache.h, DBManager.h, memory, GetTablePostfix, TItemCacheMap/m_map_itemCache, TPartyMap/TPartyMember/m_map_pkChannelParty, and m_dwItemIDStart/m_dwItemIDStop. No source integration, build or runtime validation was performed here.
