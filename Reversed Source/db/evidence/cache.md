# Cache evidence

- CItemCache constructor variants at 0x0804fae0 and 0x0804fb40 select MIN(1800, item flush seconds). Delete at 0x0804fbe0 sets vnum to zero, marks the record dirty, timestamps it and invokes the flush.
- OnFlush at 0x0804fc50 uses query 11 for DELETE and 10 for REPLACE. It appends socket and attribute SQL only when the corresponding packed bytes differ from zero (12 socket bytes and 21 attribute bytes). It increments g_item_count only for REPLACE and clears the dirty bit on both paths.
- CPlayerTableCache::OnFlush at 0x080502d0 calls the old two-argument CreatePlayerSaveQuery and sends QID 13. This override does not clear the dirty bit.
- CItemPriceListTableCache::UpdateList at 0x08050470 retains non-overlapping old records up to a total of 40. The original copies incoming records before testing the 40-entry capacity. The source guards the zero-byte append so an empty vector does not need a dereference.
- 0x080506a0 deletes with QID 22 and inserts with QID 21, logs the final count and clears the dirty bit. It sends INSERT, the newer source sends REPLACE.
- Pending shared integration: common/cache.h and the packed price list records. The original price list fields occupy 325 bytes (4-byte owner, 1-byte count, 40 records of 8 bytes). Build and ABI validation are deferred.
- DB-local CacheBase.h stages the common template from the newer reference. Its constructor defaults, Get/Put timestamps, dirty bit and Flush semantics were corroborated in the constructor, MainLoop and PutItemCache code. Timeout predicates and cache-base file integration are still pending direct verification.
