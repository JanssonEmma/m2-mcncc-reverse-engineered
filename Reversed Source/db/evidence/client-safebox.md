# ClientManager safebox reconstruction

Target is unpacked 2009 FreeBSD/i386 db ELF, SHA-256 `85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`. Headless Ghidra exports in `work/db-headless/decompiled` were checked with disassembly, DWARF and the newer reference.

| Routine | Address | Behavior |
| --- | --- | --- |
| QUERY_SAFEBOX_LOAD | `0x08055820` | Stores password, account, login and mall flag and queues QID 5 with original SELECT. |
| RESULT_SAFEBOX_LOAD | `0x08055990` | Phase 0 compares password, substitutes 000000 for absent password, selects SAFEBOX or MALL item rows. Phase 1 loads 52-byte item rows, places pending awards using original grid and vnum table, and returns an 11-byte safebox followed by items. Mall forces size one only when a safebox row exists. |
| QUERY_SAFEBOX_CHANGE_SIZE | `0x08056780` | Inserts for size 1, otherwise updates. QID 6. |
| RESULT_SAFEBOX_CHANGE_SIZE | `0x080568a0` | Sends size byte only for a positive result row count. |
| QUERY_SAFEBOX_CHANGE_PASSWORD | `0x08056940` | Queues password SELECT as QID 7, preserving old/new password slots. |
| RESULT_SAFEBOX_CHANGE_PASSWORD | `0x08056a60` | Case-insensitive comparison for nonempty stored passwords and exact default comparison for empty ones. Escapes new password and queues QID 8. Sends failure byte when the old password does not match or the row is absent. |
| RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND | `0x08056d00` | Always sends success byte after receiving callback, independent of affected rows. |
| QUERY_SAFEBOX_SAVE | `0x08057030` | Updates gold via QID 9 with null context. |

Headers 45, 46, 47, 48 and 51 were observed for safebox load, size change, wrong password, password answer and mall load. Item window bytes are 3 and 4. Request structs are 42 bytes for load, 5 for size change and 18 for password change. Unique items use the award socket 2 when present or proto value 0. Other award items default socket 2 to zero, with an original single random skill roll for items 50300 and 70037. Stops on full grid, item ID overflow or failed SQL insert, then marks completed awards. The later real-time limit conversion, wider item special cases and skill reroll are absent.

The original safebox load writes 16 bytes into a 7-byte password slot and copies 16 bytes into a 7-byte stack buffer during the response. The reconstruction bounds both to the six-character password carried by a valid packet and avoids those overflows. It owns the temporary safebox during password validation, releasing it on rejection and handing it off on a successful query. The original rejection path leaves its temporary allocation behind.

Merge `work/client-safebox.cpp` into `ClientManager.cpp`. It requires declaration of four query methods, the two change packet structs, QIDs, `CGrid`, and the existing item load helper. No build or runtime validation was performed.
