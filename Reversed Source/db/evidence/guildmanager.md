# GuildManager reconstruction

The target is the unpacked FreeBSD/i386 ELF with SHA-256
`85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`.
This batch covers manager construction, lookup, loading and ranking, plus
boot-time reserve recovery and its SQL dependencies.

| Routine | Original address | Recovered behavior |
|---|---|---|
| Manager constructor and destructor | `0x08079540`, `0x08079c00`; `0x0807a020`, `0x0807a480`, `0x0807a8e0` | Initializes the original owner containers. Destructor drains the war queue and deletes entries marked active. |
| `TouchGuild` | `0x0807ad50` | Looks up by ID. On a miss, zeroes ladder, win, draw, loss and gold, leaves name and level indeterminate, inserts the record, then returns it. |
| `ParseResult` | `0x0807af40` | Uses strtoul base 10 on ID, copies 12 bytes of guild name then writes the terminator, and parses all six numeric columns with atoi. |
| `Initialize` | `0x0807b040` | Loads all guilds, analyzes `POLY_POWER` and `POLY_HANDICAP` with 128-byte copy lengths, and requests ranking. |
| `Load` | `0x0807b390` | Loads a single guild by the original `%lu` ID format and parses a nonempty result. |
| `QueryRanking` | `0x0807b550` | Requests the 20 highest ladder-point records with QID 20. |
| `GetRanking` | `0x0807b5f0` | Returns 20 on a miss and otherwise clamps rank to [0, 20]. |
| `ResultRanking` | `0x0807b660` | Clears existing ranks for a non-null SQL result and increments rank for every row whose score is not -1. The original last-score sentinel is never updated. |
| `GetLadderPoint` | `0x0807e4b0` | Returns the stored points or zero on a miss. |
| `BootReserveWar` | `0x0807ec00` | Loads started-but-undecided reservations before waiting ones. Parses 12 columns with atoi and clears the stored started byte. Draws and deletes prior interrupted or already elapsed reservations; inserts the rest. The cancellation log refers to five minutes but the actual check is a signed negative time difference. |
| Reserve constructor | `0x08080960`, `0x080809e0` | Takes a mutable table reference, copies 46 bytes through thecore_memcpy, sets the last-notice minute to -1 and loads bets. |
| Reserve `Initialize` | `0x08080a60` | Loads login, guild and gold using the original ID format, strncpy of 30 login bytes plus a terminator, and unique insertion. |
| Reserve `Draw` | `0x080814f0` | Updates winner to zero, then asynchronously inserts item awards of vnum 50026 for original bets. Batch SQL uses a 512-byte formatting bound and stops adding rows once the running length exceeds 384. The row count is incremented after that check, as in the ELF. |

The 46-byte, one-byte-packed `SGuildReserve` type is from DWARF
`0x308b9a`. The reserve owner is 64 bytes in the 2009 ABI, with its
bet map at offset 48 and last-notice field at offset 60. Manager DWARF
places its guild map at offset 4, reserve map at 100, formulas at 112 and
200 and rank map at 288. The header uses binary symbol signatures and the reference file structure. The shared source
and headers remain untouched.

No builds or runtime tests were run.

## Score, skill and treasury continuation

| Routine | Original address |
|---|---|
| `GuildWarWin`, `GuildWarLose`, `GuildWarDraw` | `0x0807bec0`, `0x0807bf90`, `0x0807c060` |
| `IsHalfWinLadderPoint`, `ProcessDraw`, `ProcessWinLose` | `0x0807c130`, `0x0807c4b0`, `0x0807c540` |
| `ChangeLadderPoint`, `UseSkill` | `0x0807e500`, `0x0807e6b0` |
| `MoneyChange`, `DepositMoney`, `WithdrawMoney`, `WithdrawMoneyReply` | `0x0807e840`, `0x0807e980`, `0x0807ea50`, `0x0807eb50` |

The ELF confirms 12 hours for a repeat-war winner penalty, and floating
point coefficients 0.05 and 0.07 truncated to integer for winner gain and
loser loss. Ladder change logs before queuing the SQL update and broadcasts
a 20-byte packet with header 101. Money change broadcasts an 8-byte
packet with header 103 before its async SQL update. Withdraw sends an
8-byte header-104 packet to the requesting peer. Its reply either restores
the in-memory gold or persists and broadcasts the successful withdrawal.
The original signed long amounts and unchecked negative withdrawal path
are retained. These routines use evidence from objdump and the isolated
Ghidra decompiles at the addresses above.

## War and reserve continuation

| Routine | Original address |
|---|---|
| `Update`, `OnSetup` | `0x0807b780`, `0x0807bc00` |
| `RemoveWar`, `WarEnd`, `RecvWarOver`, `RecvWarEnd` | `0x0807c660`, `0x0807cf40`, `0x0807d380`, `0x0807d740` |
| `StartWar`, `UpdateScore` | `0x0807d7a0`, `0x0807db20` |
| `AddDeclare`, `RemoveDeclare` | `0x0807dee0`, `0x0807df80` |
| `TakeBetPrice`, `WaitStart` | `0x0807e090`, `0x0807e1b0` |
| `GetAverageGuildMemberLevel`, `GetGuildMemberCount` | `0x0807f0a0`, `0x0807f270` |
| `ReserveWar`, `ProcessReserveWar`, `Bet`, `CancelWar` | `0x0807f430`, `0x0807ff40`, `0x080807c0`, `0x08080910` |
| Reserve `OnSetup`, `Bet`, `End` | `0x08080d10`, `0x08080f00`, `0x08081760` |

War broadcasts use header 97 with the original 18-byte packed
`SPacketGuildWar` (DWARF `0x30733c`). Score broadcasts use header 98
with 16 bytes. The peer functors exclude channel zero. The initial
peer setup sends active wars, declarations and reserves. The original
setup does not send per-side scores. Field wars last 1800 seconds.
Other wars last 172800 seconds. The wait queue advances by five
seconds. The original reads its wait-start entry through a reference
after popping the heap. Its outgoing start packet leaves the final
two long fields unset. These source-level quirks are retained.

The exact level and ranking tables come from `0x080caa40` and
`0x080caaa0`. Reserve creation adds 10 seconds to the local time
while the SQL uses a 180-second interval. Processing announces a
reservation up to 1800 seconds in advance. The minute calculation
converts the signed difference to float, divides by 60 and applies
ceil. The Korean and Chinese notice bytes come from `0x080cb8e0`
and `0x080cb920`. Other locale values send the original empty notice.
The stored reserve is 46 bytes under header 106. Header 107 removes
an ID with a 4-byte body. Header 108 uses a 43-byte bet body with a
31-byte, explicitly terminated login. Settlement applies the original
handicap, stores the outcome, and awards 90 percent of the total pool
in proportion to the winner's wagers. The original SQL formatting
and batching bounds are retained.

The translation contains all 45 manager/reserve method definitions and
the two original member-statistic query helpers. The surrounding game
protocol and ClientManager hooks still determine integrated behavior.
Shared files were left untouched. The reconstructed code has not been
built or executed.
