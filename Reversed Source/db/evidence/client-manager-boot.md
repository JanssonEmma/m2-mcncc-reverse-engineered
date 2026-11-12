# ClientManagerBoot reconstruction

The unpacked 2009 DB ELF SHA-256 is `85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`. The original compilation unit is DWARF DIE `0x244f2d`. Disassembly and data pointers were checked against the newer reference. All observed non-Monarch routines in this translation unit are covered.

| Function | Entry | Recovered behavior |
| --- | --- | --- |
| InitializeTables | `0x0806f420` | Calls mob, item, shop, skill, refine, normal attributes, rare attributes, banword, land, object proto and object loaders in order, stopping and logging on first failure. The final Monarch call is excluded by scope. |
| InitializeMobTable | `0x0806fa00` | SQL at `0x080c9180`; a zero row count succeeds and retains prior data. Nonempty results replace the vector with 255-byte rows in query order. Only the two selected skill slots are populated. |
| InitializeRefineTable | `0x0806f5c0` | 2048-byte query buffer and postfix, 53-byte records, first zero material vnum sets count. The zero-initialized count remains zero for a row with all five materials nonzero. |
| InitializeItemTable | `0x08070d40` | SQL at `0x080c9620` and pointer `0x080d305c`. Empty result logs formatted query and fails. Nonempty results replace the vector with 154-byte records in query order. The original inserts into the vnum map in the parse pass, clears it, and inserts again in a separate logging pass. |
| InitializeShopTable | `0x08070250` | Left join from query pointer `0x080d3054`. Zero rows fail. Rows are grouped in numeric shop order. Null item skips only item population. Allocation/copy size is 449 bytes, item stride 11. Original pointer ownership and item count behavior are retained. |
| InitializeQuestItemTable | `0x080708d0` | Query pointer `0x080d3058`, locale column, 154-byte records, names truncated to 24 bytes, duplicate check against vnum map, type 18 and size 1. Sorts the item vector by vnum at the end without rebuilding the vnum map in this routine. |
| InitializeSkillTable | `0x08071530` | 30 columns, exact SQL template at `0x080c9820` and pointer `0x080d3060`. Copies strings with the original unbounded strcpy into a zero-initialized 1475-byte record. |
| InitializeBanwordTable | `0x08071b50` | Clears existing list, selects word without postfix, skips null values, truncates to 24 bytes with explicit terminator. |
| InitializeItemAttrTable | `0x08071de0` | Query pointer `0x080d3064`. 69-byte packed rows, 32-byte apply-name cap, eight set levels. Empty result logs the literal query template and fails before replacing existing entries. |
| InitializeItemRareTable | `0x08072330` | Same row layout for the rare variant, with pointer `0x080d3068` and separate vector. |
| InitializeLandTable | `0x08072880` | Query pointer `0x080d306c`, only enabled land, nine SQL values mapped to 36-byte record. Clears previous rows and allows empty results. |
| parse_pair_number_string | `0x08072c30` | Slash-delimited pairs with an optional comma and default count zero. Earlier chunks copy to a bounded buffer before splitting. Final chunk writes a terminator into the original mutable SQL data at its comma. |
| InitializeObjectProto | `0x08072e30` | Query pointer `0x080d3070`, 96-byte records, first five materials, NPC y is maximum region y plus 300. |
| InitializeObjectTable | `0x080732b0` | Query pointer `0x080d3074`, 40-byte records, float rotations parsed with atof, stored in a map by ID. |

The newer CSV/mirroring/ranking flows are absent from the original initialization chain.

Integration requires matching `ClientManager.h` member declarations and original shared constants, including `ITEM_QUEST` and the eight `ATTRIBUTE_SET_*` values. Shared table layouts and MySQL interfaces are expected from the reconstruction tree. Builds and runtime tests are deferred.
