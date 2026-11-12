# Lock, login state and money log

Analyzed the unpacked 2009 FreeBSD/i386 ELF with SHA-256 `85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`. Used `objdump -t -C`, address-bounded `objdump -d -C`, literal bytes in `.rodata`, and `readelf --debug-dump=info`. The M2NewClassic sources supply matching spelling and structure where the 2009 evidence permits. Build and runtime validation are pending.

## Lock.cpp and Lock.h

- Constructor and destructor variants at `0x0804ed00` through `0x0804ed30` return without touching the object.
- `Initialize` `0x0804ed40` clears the flag at offset 4 and initializes the mutex at offset 0. `Destroy` `0x0804ed60` checks the flag before destroying the mutex.
- `Trylock` `0x0804edb0` returns the pthread result and leaves the flag alone. `Lock` `0x0804edc0` sets the flag after calling pthread. `Unlock` `0x0804ede0` checks and clears the flag before calling pthread. The assertion expressions and strings are present at `0x080c3b20` and `0x080c3b60`.
- DWARF `0xbcae2` records the original 8-byte class with `m_lock` at 0 and `m_bLocked` at 4. That class size depends on the FreeBSD/i386 pthread ABI.

## LoginData.cpp and LoginData.h

- The two constructor variants at `0x08050aa0` and `0x08050b30` initialize the keys, peer handle, logon time, 16-byte IP buffer, play/deleted/billing fields, last play time and 333-byte account record. The nine premium slots are left uninitialized.
- DWARF type `0x2c7c9` records a 428-byte class. Client key, peer handle, logon time, IP, flags, billing ID, last play time, premium array and account record start at offsets 4, 20, 24, 28, 44, 48, 52, 56 and 92. The reference's last-player-ID API and field are absent from this class layout.
- `GetRef` `0x08050bc0` returns the embedded account. `SetClientKey` `0x08050bd0` copies 16 bytes through `thecore_memcpy`. Accessors at `0x08050c00` through `0x08050c70` agree with the offsets above, including `get_dword_time` for logon time.
- `SetIP` `0x08050c80` uses `strncpy` for 15 bytes, then terminates the last byte explicitly. `GetIP` `0x08050cc0` returns the buffer.
- `SetPlay` `0x08050cd0` logs both transitions using the original strings at `0x080c42c5` and `0x080c42d7`. The true branch refreshes logon time. Both update last play time through `CClientManager::GetCurrentTime`.
- Accessors at `0x08050da0` through `0x08050dc0` read and write the two flag bytes. `SetPremium` `0x08050dd0` copies 36 bytes. `GetPremium` `0x08050e00` returns zero for types above 8. `GetPremiumPtr` `0x08050e20` returns the array.

## MoneyLog.cpp and MoneyLog.h

- Constructor variants at `0x08088f50` and `0x080890b0` establish the singleton and the array of maps. Destructor variants at `0x08089150`, `0x08089210`, and `0x080892d0` tear down the maps and singleton without a network flush.
- `Save` `0x08089390` returns with the logs intact if `GetAnyPeer` finds no peer. Otherwise it walks types 0 to 8 and each map in ascending key order. It emits opcode 126, handle 0 and a 9-byte `TPacketMoneyLog`, then clears each map. DWARF `0xef55` and the stack stores confirm the packed type, vnum and gold at offsets 0, 1 and 5.
- `AddLog` `0x080894e0` indexes directly by the byte type and adds to the per-vnum accumulator, initializing new map entries to zero.
- The common headers must expose the original `PREMIUM_MAX_NUM = 9`, `MONEY_LOG_TYPE_MAX_NUM = 9` and `HEADER_DG_MONEY_LOG = 126`. These modules also depend on the recovered common account and packet layouts and the DB peer and manager interfaces.

The ELF symbol inventory has no standalone HorseNameManager or ItemIDRangeManager. The 2009 item-ID allocation symbols belong to `CClientManager`.

## ItemAwardManager.cpp and ItemAwardManager.h

- Constructor variants at `0x08089620` and `0x080897a0` initialize the two map owners and singleton. Destructor variants at `0x08089870`, `0x08089980` and `0x08089a90` destroy the containers without freeing the item-award records.
- `RequestLoad` `0x08089ba0` sends the SQL at `0x080cc3a0` in an 8192-byte buffer to the player DB with QID 18. The query selects all outstanding awards. The later reference filters by a cached ID, which has no counterpart in this routine or the original symbol inventory.
- `Load` `0x08089c40` loops over the signed result row count, skips IDs already in the award map and allocates a zeroed 112-byte record. It uses the original `trim_and_lower(destination, source, 30)` call and seven `atoi` calls, including the ID. A non-null reason is copied with `strncpy` for 50 bytes and explicitly terminated. The record is added to both indexes after field population. The original has no GIFT command dispatch.
- DWARF `0x9b83c` records the 112-byte `SItemAward`. The login buffer is 31 bytes at offset 4, the numeric fields begin at 36, and the 51-byte reason and the flags are at 56, 107 and 108.
- `GetByLogin` `0x0808a1a0` searches the original login string directly. `Taken` `0x0808a230` logs a missing ID and returns. For a known award it sets the flag and queues the update literal at `0x080cc4e0` with QID 19, even if the in-memory flag was already set.
- Query IDs 18 and 19 match DWARF `0xffb40` and `0xffb46`. The original DB manager and QID interface are integration dependencies. Common `LOGIN_MAX_LEN = 30` and `QUERY_MAX_LEN = 8192` already exist in the game reconstruction. `ITEM_AWARD_WHY_MAX_LEN = 50` is present in the old ELF at `0xa935`. The reconstructed libsql interface already exposes the original signed `iNumRows`.

## QID.h

The 25 original query names and values 0 through 24 come from the complete `QID` enum at DWARF `0xffac7`, which ends at `0xffb69`. The final four entries are the item-price operations. The SQL slot enum preceding it belongs to the DB manager interface.

## ClientManagerParty.cpp

The original translation unit appears at DWARF compilation unit `0x28acf4`. These six handlers were compared against their disassembly, container helper calls and original message strings in `0x080c9f34` through `0x080ca267`.

| Handler | Entry | Opcode | Payload bytes | Origin excluded |
| --- | --- | --- | --- | --- |
| QUERY_PARTY_CREATE | `0x080768d0` | 59 | 4 | yes |
| QUERY_PARTY_DELETE | `0x08076e60` | 60 | 4 | yes |
| QUERY_PARTY_ADD | `0x080770e0` | 61 | 9 | yes |
| QUERY_PARTY_REMOVE | `0x080773a0` | 62 | 8 | yes |
| QUERY_PARTY_STATE_CHANGE | `0x08077630` | 63 | 10 | yes |
| QUERY_PARTY_SET_MEMBER_LEVEL | `0x08077880` | 65 | 9 | no |

- The channel is the unsigned byte at peer offset `0x38`. All six use map lookup or insertion by that key, including requests which subsequently report an error. Create inserts an empty member map. Delete erases an existing party. Add inserts an absent member with two zero bytes at `0x08077296` and `0x0807729d`. Remove erases an existing member.
- State change writes role if the packet flag at offset 9 is set and otherwise clears the stored role. It forwards the original packet in either case. Level change updates byte 1 of the stored info and forwards to the entire channel.
- `TPartyInfo` is two bytes in DWARF `0x2f754`, with role at 0 and level at 1. The owning map member is `m_map_pkChannelParty` at offset 312 in `CClientManager` (DIE `0x15d32e`). An implementation of `SendPartyOnSetup` remains in the main manager translation unit.
- The complete class declaration is still an integration dependency. The six method signatures are present in the reconstructed source. The class needs the following declarations and a `void ForwardPacket(BYTE, const void*, int, BYTE, CPeer* = NULL)` interface.

```cpp
struct TPartyInfo
{
    BYTE bRole;
    BYTE bLevel;
    TPartyInfo() : bRole(0), bLevel(0) {}
};

typedef std::map<DWORD, TPartyInfo> TPartyMember;
typedef std::map<DWORD, TPartyMember> TPartyMap;
typedef std::map<BYTE, TPartyMap> TPartyChannelMap;
TPartyChannelMap m_map_pkChannelParty;
```

- The game-side common tables already contain Create, Delete, Add, Remove and SetMemberLevel. Common needs the remaining 10-byte packed StateChange record, confirmed by DWARF `0xdb00`, and shared opcode declarations.

```cpp
#pragma pack(push, 1)
typedef struct SPacketPartyStateChange
{
    DWORD dwLeaderPID;
    DWORD dwPID;
    BYTE bRole;
    BYTE bFlag;
} TPacketPartyStateChange;
#pragma pack(pop)
```

## PrivManager.cpp and PrivManager.h

- Constructor variants `0x08085f80` and `0x08086470` initialize the singleton, queues, maps and 20 null empire slots. Destructors `0x08086780`, `0x08086970`, `0x08086b60` destroy containers, without a separate pointee cleanup loop. The private owner layout starts character/guild/empire queues at offsets 4, 20 and 36, the empire pointer array at 52, the guild maps at 132 and the character maps at 192. This is the old STL layout.
- `Update` `0x08086d60` expires queues in guild, empire, character order. A current nonzero guild entry is erased and broadcast as zero. A nonzero empire entry broadcasts zero and clears its slot without checking whether the slot still points to that entry. Character expiry broadcasts zero before erasing the matching map node. Queue entries are deleted after processing.
- `AddCharPriv` `0x08087260` ignores duplicate keys and zero values. Positive values use a 7200-second duration and negative values use 3600 seconds. `AddGuildPriv` `0x08087460` queues expiry and replaces the current map pointer without marking the old record removed. `AddEmpirePriv` `0x08087660` clamps negative durations to zero, pushes the new record and overwrites the current pointer. It does not invalidate the old empire record, unlike the newer source. The original methods index by caller-provided type directly. Thus an older empire record can clear a newer slot on expiry. A zero-valued guild or empire record can leave a stale index pointer when it expires. These original behaviors were retained.
- Send methods at `0x080877f0`, `0x08087890` and `0x08087930` use the mutable peer functor template. The instantiations at `0x080888b0`, `0x08088960` and `0x08088a10` iterate all connected peers in list order and clear the log byte after the first emission. The eventual `CClientManager` declaration needs `template<class Func> Func for_each_peer(Func f)` with an available definition. `GetCurrentTime()` and the singleton are also required.
- `SendPrivOnSetup` `0x080879c0` walks types 1 through 4, sending empire slots 0 through 3, guild entries and then character entries for each type. Each setup record has its own log byte set to 1.
- Common needs `MAX_PRIV_NUM = 5` (DWARF `0xa7c3`). `EMPIRE_MAX_NUM = 4` is already present. The packet changes require original packed records and opcodes, with handle zero. The original `time_t` fields in wire records occupy four signed bytes.

| Record | Opcode | Size | Offsets and original types | DWARF name attribute |
| --- | --- | --- | --- | --- |
| TPacketDGChangeEmpirePriv | 124 | 11 | type BYTE 0, value int 1, empire BYTE 5, bLog BYTE 6, end_time_sec signed 32-bit 7 | `0xee97` |
| TPacketDGChangeGuildPriv | 125 | 14 | type BYTE 0, value int 1, guild_id DWORD 5, bLog BYTE 9, end_time_sec signed 32-bit 10 | `0xedd4` |
| TPacketDGChangeCharacterPriv | 127 | 10 | type BYTE 0, value int 1, pid DWORD 5, bLog BYTE 9 | `0xed20` |

Build and runtime validation are deferred.

## ClientManagerLogin.cpp

The original unit contains nine CClientManager methods and two SQL result helpers. Its original login payloads rely on the packed i386 account layout, with a 333-byte account and four 63-byte character entries.

| Function | Address | 2009 behavior |
| --- | --- | --- |
| InsertLogonAccount | `0x0806d7e0` | Normalizes the map key with `trim_and_lower(dst, src, 30)`. Rejects an existing entry or missing login data, then sets the peer and IP before insertion. |
| DeleteLogonAccount | `0x0806d970` | Uses the normalized map key and rejects a mismatched peer. Clears play state and sends billing logout if playing, deletes already-deleted login data and erases the map entry. |
| FindLogonAccount | `0x0806db80` | Checks the same normalized key. |
| QUERY_LOGIN_BY_KEY | `0x0806dc30` | Looks up by key, checks already-online before checking the login or client key. Compares the raw packet login case-insensitively and checks all 16 client-key bytes. The already-online packet copies the raw login. On success, copies the account identifiers with old padding/termination semantics, sets OK and queues the player-index SELECT. |
| RESULT_LOGIN_BY_KEY | `0x0806e000` | Sends not-exist on SQL error. A zero-row result directly queues INSERT IGNORE and QID 17, without an extra SELECT. Otherwise parses four IDs and empire, then queues QID 4 with an alignment column for gb2312 and playtime otherwise. |
| RESULT_PLAYER_INDEX_CREATE | `0x0806e2b0` | Queues the player-index SELECT again with QID 16. |
| CreateAccountTableFromRes | `0x0806e380` | Builds the zeroed account from the first joined row, comparing the two copied password strings before returning it. |
| CreateAccountPlayerDataFromRes | `0x0806e500` | Matches returned IDs to the four account slots. Prefers a cached player without updating the cache access timestamp. The database fallback handles null name/numerics, preserves the 2009 column order and logs the resulting location. |
| RESULT_LOGIN | `0x0806e990` | Phase zero parses the account, returns not-exist/wrong-password as appropriate, then queries the character list. Phase one registers the logon and returns opcode 34 or copies and sends the complete account with opcode 30. Releases the temporary account and context. |
| QUERY_LOGOUT | `0x0806ed50` | Queues all nonzero character IDs for delayed logout before attempting peer-checked logon deletion. |
| QUERY_CHANGE_NAME | `0x0806ee90` | Uses the original sjis collation branch and raw name SQL formatting. Checks the first count character against ASCII zero, queues a synchronous update and sends opcode 121 with the 29-byte result. |

- Original format and SQL literals run from `0x080c8820` through `0x080c8d68`. The rebuilt functions use the old `CLoginData::GetRef()`, signed SQL `iNumRows`, and `strncpy` with explicit termination. The C++ array helper preserves the exact bounded copies.
- The nested `ClientHandleInfo` type is 76 bytes in DWARF `0x2f65e`. Fields used here are handle offset 0, phase byte offset 12, IP offset 51 and account pointer offset 68. The destructor must follow the 2009 ownership rules.
- `CClientManager` also needs `TLogonAccountMap` as `std::map<std::string, CLoginData*>` and the map at original offset 72, `GetLoginData(DWORD)`, `GetLoginDataByLogin(const char*)`, `SendLoginToBilling(CLoginData*, bool)`, `InsertLogoutPlayer(int)`, and `GetPlayerCache(DWORD)`. The globals `g_stLocale`, `g_test_server`, `g_log` and `GetTablePostfix()` belong to the startup/main integration.
- Shared common already includes `TPacketGDLoginByKey`, `TLogoutPacket`, `TPacketGDChangeName` and `TPacketDGChangeName`. It also needs the 31-byte `TPacketDGLoginAlready` with member `char szLogin[LOGIN_MAX_LEN + 1]` and opcode constants `HEADER_DG_LOGIN_SUCCESS = 30`, `HEADER_DG_LOGIN_NOT_EXIST = 31`, `HEADER_DG_LOGIN_WRONG_PASSWD = 33`, `HEADER_DG_LOGIN_ALREADY = 34`, `HEADER_DG_PLAYER_CREATE_ALREADY = 38`, `HEADER_DG_PLAYER_CREATE_FAILED = 39`, and `HEADER_DG_CHANGE_NAME = 121`.
- The parsing code expects the original cache interface and shared `PART_MAIN` and `PART_HAIR` indexes. Build and runtime validation remain deferred.

## ClientManagerGuild.cpp

The 2009 unit has 13 manager methods and `__GetWarType`. The latter returns the four original encoded strings from `0x080ca349` through `0x080ca361`, preserved byte for byte using hexadecimal C++ escapes. SQL literals and log formats were read directly from `0x080ca2b8` through `0x080ca980`.

| Method | Entry | Forwarded packet |
| --- | --- | --- |
| GuildCreate | `0x08077f60` | load opcode 100, 4-byte ID |
| GuildChangeGrade | `0x08078000` | opcode 94, 8 bytes |
| GuildAddMember | `0x08078070` | opcode 92, 41 bytes |
| GuildRemoveMember | `0x08078800` | opcode 93, 8 bytes |
| GuildSkillUpdate | `0x08078980` | opcode 56, 22 bytes |
| GuildExpUpdate | `0x080789e0` | opcode 58, 8 bytes, excluding sender |
| GuildChangeMemberData | `0x08078a40` | opcode 95, 14 bytes, excluding sender |
| GuildDisband | `0x08078ac0` | opcode 96, 8 bytes |
| GuildWar | `0x08078df0` | opcode 97, 18 bytes, except delegated END case |
| GuildWarScore | `0x08079300` | delegated to guild manager score update |
| GuildChangeLadderPoint | `0x08079370` | delegated to guild manager ladder update |
| GuildUseSkill | `0x08079400` | uses skill and sends unusable state via the method below |
| SendGuildSkillUsable | `0x080794b0` | opcode 102, 9 bytes |

- Create forwards first, then loads the guild. AddMember touches the guild, performs the original direct insert and joined SELECT, handles zero result rows and null PID/grade, then forwards the parsed member. PID and offer use decimal `strtoul`, grade/general/level/job use `atoi`. The name copy uses 24-byte `strncpy` and a final NUL.
- RemoveMember queues two async statements in delete/withdrawal timestamp order. Disband queues the five original statements in guild, grade, withdrawal timestamps, members, comments order before broadcasting.
- GuildWar's jump table at `0x080ca290` confirms the original protocol states 1 declare, 2 refuse, 4 wait-start, 5 cancel, 6 on-war, 7 end, 8 over, 9 reserve. Wait-start flows into reserve handling while suppressing the reserve log. Reserve success writes 9 and failure writes 5. End tail-calls `CGuildManager::RecvWarEnd` and omits the direct forward. Other states, including unknown values, forward the packet. The newer GuildChangeMaster method has no original symbol in this unit.
- Dependencies are `CClientManager` method declarations, `GetCurrentTime`, `ForwardPacket`, `CDBManager::DirectQuery/AsyncQuery`, `GetTablePostfix` and the original `CGuildManager` API. The latter is needed for `Load`, `TouchGuild`, `AddDeclare`, `RemoveDeclare`, `ReserveWar`, `StartWar`, `RecvWarOver`, `RecvWarEnd`, `CancelWar`, `UpdateScore`, `ChangeLadderPoint` and `UseSkill`.
- Common needs the opcode constants listed above and the packed guild packet definitions. `TPacketGuildChangeMemberData` is already present. The input `TPacketGDGuildAddMember` is 9 bytes at DWARF `0xde00`; the output `TPacketDGGuildMember` is 41 at `0xdea5`; `TPacketGuildWarScore` is 16 at `0xe067`. `TPacketGuildLadderPoint` is 8 and `TPacketGuildUseSkill` is 12 by the original argument loads. Original signed longs in wire records are 32-bit.
