# ClientManager core evidence

Partial source recovery. The original class declaration, lifecycle and dispatch are outstanding.

| Method | ELF entry | Details |
| --- | --- | --- |
| SetCacheFlushCountLimit | 0x08059130 | MAX(10, limit), store at +0x158, log the selected value |
| GetAnyPeer | 0x0805d250 | First peer in the list at +0x10 or NULL when empty |
| ForwardPacket | 0x0805e9c0 | Skip the excluded peer and channel zero. A nonzero channel argument restricts forwarding. Always encode the header with handle zero. Encode a payload only when its length is positive and its pointer is non-null. |
| GetCurrentTime | 0x0805eae0 | Tail call to time(0) |

The remaining SendNotice implementation at 0x0805ea70 uses a 255-byte formatting limit and the returned length as both a terminator index and packet length basis. It is still pending integration with the dispatcher.

CAsyncSQL2::SetLocale at 0x080bfab0 logs "SetLocale %s " with level 0 and assigns the supplied string to the inherited locale member. The shared libsql currently exposes an empty CAsyncSQL2 subclass. Its declaration and implementation must be integrated jointly with the game source tree.

- The partial header reproduces the member names and order of the original 384-byte DWARF record at 0x15cf0b. Modern STL sizes differ. Method declarations and shared table types are still being integrated.
- SetPlayerIDStart at 0x08053d40 writes at +0x3c. GetPeer at 0x0805d1f0 searches the peer list in order, returning the first matching handle or NULL.
- Constructor entries 0x08050e40 and 0x08051770 initialize both cache counters (0 and 200), clear item IDs and zero the two global query counters. Destructor variants call Destroy at 0x08053d50. Destroy closes each peer connection, clears the peer list and closes a strictly positive listener descriptor.
- Initialize at 0x08053dc0 calls localization, item-ID initialization, table initialization, and guild reservation boot in that order. It binds the original literal "0" with default port 5300, registers the listener, reads BACKUP_LIMIT_SEC with default 600 (local value unused), defaults player deletion limit to 100, and loads event flags. Quit at 0x08054570 sets only the shutdown flag.
- Login lookup entries 0x08059ad0, 0x08059b20 and 0x08059bc0 search the key, normalized login and account-ID maps. The login lookup calls trim_and_lower with maximum length 30.
- InitializeNowItemID at 0x0805eaf0 uses one inclusive SQL range, fails on zero rows or when last ID equals the endpoint, and starts one past the lower bound if MAX returns NULL. GainItemID at 0x0805ef80 returns the value before incrementing it.
- MainLoop at 0x08054160 drains and destroys SQL messages, processes network work, rotates logs, and flushes/destroys player, item, and price-list caches on exit after disabling the signal timer.
- InitializeLocalization at 0x0805efb0 copies 32 bytes per SQL field, applies the original 17 region cases, logs the original charset text, sends the assigned charset to CDBManager, and preserves row order in the locale vector. Unknown locale exits with status zero. The internal __CheckPlayerSlot at 0x08050e30 is a no-op.
- Cache lookup/create at 0x08058780, 0x080587d0 and 0x08058aa0 use the original keyed maps. PutItemCache at 0x08058af0 moves existing membership on owner change, writes through the cache, and flushes immediately if no set exists for the new owner.
- Thin dispatch handlers at 0x08059900-0x08059acf, 0x0805a570-0x0805a72f and 0x0805b570-0x0805b913 reproduce the privilege, guild money, bet and marriage calls. BreakMarriage at 0x08060f20 removes the two decoded PIDs. BlockException at 0x080622d0 preserves the SQL strings (including the original DELETE form) and forwards to peers with a channel.
- QUERY_FLUSH_CACHE at 0x08059520 flushes player and item caches and removes the player cache entry only if it exists.
- Item requests at 0x08058440, 0x080584c0 and 0x080593d0 preserve the original window-3/window-4 SQL bypass, owner-set unlink and separate async deletion when no player ID was supplied. Quest saves at 0x08055690 reject a partial row and use original QID 12 for each delete or replace. Guild skill recharge at 0x0805e930 is opcode 57 with no payload.
- Empire selection at 0x080570f0 queries four PIDs but updates the first three only, with original map and coordinate constants. SMS at 0x080595d0 escapes the message before enqueueing. VCard at 0x0805b1d0 and 0x0805b280 buffers until an auth peer exists. BlockChat at 0x0805b330 uses the locale-dependent lookup and adds affect 223.
