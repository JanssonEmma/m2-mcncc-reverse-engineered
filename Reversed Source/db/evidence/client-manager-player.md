# ClientManagerPlayer reconstruction

The unpacked 2009 DB ELF SHA-256 is `85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`. The original compilation unit is DWARF DIE `0x1d45f1`. The newer source informed method names and file structure. Disassembly, data and DWARF govern this translation.

| Routine | Entry | Recovered behavior |
| --- | --- | --- |
| CreateItemTableFromRes | `0x08067390` | Clears vector for null or nonpositive row count. Populates 52-byte item rows, three sockets, seven attributes and supplied owner PID. |
| CreatePlayerSaveQuery | `0x080675b0` | Original signed int return and two parameters. Exact sprintf format at `0x080c73e0`. Escapes 1530 skill bytes and 72 quickslot bytes into an 8193-byte shared scratch buffer and returns cumulative length. |
| GetPlayerCache / PutPlayerCache | `0x08067890`, `0x08067910` | Reads elapsed time without refreshing the cache on get. Allocates and indexes a new entry when needed, sends optional hot backup before saving. |
| QUERY_PLAYER_LOAD | `0x08067a20` | Looks up login and visits all four player slots before loading. Uses cached player data or queues SQL player, item, quest and affect in order. Cache hit emits old packet headers and filters zero-vnum item records. Cached item branch queries nonzero quest values only. No login-log packet is emitted. |
| CreatePlayerTableFromRes | `0x08068550` | Clears and loads one 1818-byte player row, includes two nullable fixed-size blobs and optional mobile string. Reads sub-skill points as stored. |
| RESULT_COMPOSITE_PLAYER | `0x08068930` | Takes ownership of ClientHandleInfo and dispatches QIDs 0?3. Logs null SQL result and releases the handle. |
| RESULT_PLAYER_LOAD | `0x08068b00` | Sends failure for missing row, missing login, or login already playing. On success sets play state, sends billing event, copies nine premiums and returns 1818-byte record. |
| RESULT_ITEM_LOAD | `0x08068c50` | Sends count and rows, creates cache set and inserts returned records with skip-query flag. |
| RESULT_AFFECT_LOAD | `0x08068d90` | No packet for zero rows. Otherwise sends player ID, count and packed 21-byte affect rows. Flags parsed by base-ten strtoul. |
| RESULT_QUEST_LOAD | `0x08069000` | Sends zero-count packet for empty results. Otherwise sends count and 106-byte quest rows. |
| QUERY_PLAYER_SAVE | `0x08069280` | Logs name for test server, then updates cache. |
| Player create query | `0x080692d0` | Checks account slot and name, including the original sjis collation path. Inserts the original columns and escaped blobs. Updates the index, rolls back the inserted row if the index fails and emits the original 64-byte success packet. The later creation throttle is absent. |
| Player delete query and result | `0x0806a960`, `0x0806ac20` | Validates login, slot, locale/event bypass, private code and original level limits. Archives before dropping caches, clears the player index and issues the original six delete statements. Returns packet 40 or 41 with a one-byte slot. |
| QUERY_ADD_AFFECT / QUERY_REMOVE_AFFECT | `0x0806b980`, `0x0806ba40` | Uses original REPLACE and DELETE SQL literals with 8192-byte buffers and sends async query. |
| Highscore query and result | `0x0806baf0`, `0x0806bc20` | Looks up board/PID, inserts absent rows and replaces only when score improves in the selected direction. The reconstructed query bounds its internal board copy to the login array. The original copy spans login and adjacent scratch fields, which the highscore result never reads. |
| Logout cache methods | `0x0806bda0`, `0x0806bea0`, `0x0806bf80`, `0x0806c040` | Insert or refresh timer, log delete, flush expired items then player, and remove timer. Player flush occurs before map erase and entry destruction. |

Outgoing headers 35, 36, 42, 44, and 50 are the original player success, player failure, item, quest, and affect IDs. The logout erase captures its payload before erase for compatibility with current standard library iterators. The original binary reads the payload from the old tree node after erase.

Item cache helpers and item save/flush/destroy dispatch belong to the separate original ClientManager.cpp unit. Builds and runtime tests are deferred.
