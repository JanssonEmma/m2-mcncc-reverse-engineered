# ClientManager shop pricelist reconstruction

Target is the unpacked FreeBSD/i386 ELF with SHA-256
`85e673aba60da66b68a6844c9e29de5090769249718a9046f868cc5e128ff111`.
The isolated Ghidra pseudocode preserves original control flow and DWARF
types. The staged code covers the six missing methods.

| Routine | Address | Recovered behavior |
|---|---|---|
| `RESULT_PRICELIST_LOAD` | `0x08056d90` | Reads both SQL columns with atoi, caches the table, replies on the stored handle and deletes the request pair. |
| `RESULT_PRICELIST_LOAD_FOR_UPDATE` | `0x08056f30` | Loads and caches SQL data, then applies the queued table to the matching cache and deletes the update payload. |
| `GetItemPriceListCache` | `0x08058ee0` | Looks up by owner ID and returns null on a miss. |
| `PutItemPriceListCache` | `0x08058f30` | Allocates and inserts a cache if necessary. Copies the full 325-byte original record and refreshes the timestamp without setting the dirty flag. |
| `MyshopPricelistUpdate` | `0x0805b920` | Reads entries directly after the packed 5-byte header, then updates the cache or stages a QID-23 request with the pending table. Original control flow has no packet count limit check. |
| `MyshopPricelistRequest` | `0x0805ba60` | Replies from the cache without updating the timestamp, otherwise submits QID 24 with the peer handle as ident and a heap-owned pair of response handle and player ID. |

The original price record consists of two DWORDs and each wire response
has an 8-byte record per entry after the packed 5-byte header. The
response opcode is 157. SQL queries use a 8193-byte buffer and 8192-byte
format bound. The original SQL rows are not bounded by the code. The
cache put uses skip-query true, which leaves any previous dirty flag
unchanged.

For integration, replace the forward declaration of
`TPacketMyshopPricelistHeader` with this exact layout and add declarations
for the four methods which are absent from the owner header. The DWARF
mangled type is `SPacketMyshopPricelistHeader`.

```cpp
#pragma pack(push, 1)
typedef struct SPacketMyshopPricelistHeader
{
    DWORD dwOwnerID;
    BYTE byCount;
} TPacketMyshopPricelistHeader;
#pragma pack(pop)

CItemPriceListTableCache* GetItemPriceListCache(DWORD id);
void PutItemPriceListCache(const TItemPriceListTable* table);
void MyshopPricelistUpdate(const TPacketMyshopPricelistHeader* packet);
void MyshopPricelistRequest(CPeer* peer, DWORD handle, DWORD playerID);
```

The six definitions are staged in `work/client-price.cpp`. Existing
`UpdateItemPriceListCache` is outside this snippet. No build or runtime
test was run.
