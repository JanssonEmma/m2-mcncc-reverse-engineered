# ClientManager authentication and billing

Target is the unpacked FreeBSD/i386 ELF with SHA-256
`85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`.
The staged method definitions use isolated Ghidra DWARF-backed pseudocode
at the addresses below. The three already implemented lookup methods and
`SendLoginToBilling` are excluded from the snippet.

| Routine | Address | Recovered behavior |
|---|---|---|
| `InsertLoginData` | `0x08059c10` | Normalizes the login to 30 characters, inserts by key, login and account ID. The inserts are unique, not assignments. |
| `DeleteLoginData` | `0x08059db0` | Erases the three indices, deletes immediately if the account is absent from the active-login map, otherwise marks the object deleted. |
| `QUERY_AUTH_LOGIN` | `0x08059f00` | Removes the prior entry by normalized login before checking for a duplicate key. Creates and populates the login object on success and responds with header 122 and one result byte using the request handle. Account copies are 30 normalized login characters, 18 social-ID characters plus a terminator, and 17 password bytes padded with zero from `TEMP`. |
| `BillingExpire` | `0x0805a160` | Billing types 3 and 4 parse the incoming login as the IP billing ID and forward the packet to each matching connected account, rewriting the packet login in place. Types 5 and 6 forward by normalized login. Uses header 129 with 36 bytes. |
| `BillingCheck` | `0x0805a370` | Reads the count and keys as native four-byte DWORDs. Reports unknown keys and those idle more than 180 seconds. Always sends header 132 with count and result list when an auth peer exists, including a zero result count. |
| `SendAllLoginToBilling` | `0x0805a730` | Enumerates active logins and sends header 128 with a DWORD count followed by one 51-byte packed record per login. No message is sent for an empty map. Records have a DWORD key, 31-byte login and 16-byte host. |

The code assumes the original ABI's 32-bit DWORD and uses the existing
shared `TPacketGDAuthLogin` and `TPacketBillingExpire`. The original
temporary-password padding and in-place IP expiration packet mutation
are preserved. Count and key reads use memcpy to preserve unaligned
native-width reads without requiring aligned input.

Add matching declarations for the six staged methods to the owning
ClientManager header. The existing translation unit declares
`extern BOOL g_test_server`. The billing repair record is local to the
implementation. The parent task owns dispatcher integration and the
ClientManager source/header. No build or runtime test was run.
