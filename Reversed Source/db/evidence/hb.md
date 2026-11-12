# HB evidence

Target SHA-256 85E673ABA60DA66B68A6844C9E29DE5090769249718A9046F868CC5E128FF111.

- Constructor entries 0x08085260 and 0x080853c0 initialize the expiration to 3600 at +0x18. Destructor entries 0x08085480, 0x08085590 and 0x080856a0 implement member and singleton teardown. The observed singleton/map/string offsets are +0, +0x4, +0x10 and +0x14.
- Initialize at 0x080857c0 uses a 128-byte SHOW CREATE buffer and direct slot 0. It rejects a zero row count, otherwise copies column 1. Both paths release the SQL result message.
- Put at 0x08085a70 issues the first backup before inserting a timestamp from get_dword_time. Existing rows use time(0) for the comparison and are never updated by this method.
- Query at 0x08085b20 generates an hourly 64-byte name, finds the 32-byte CREATE prefix and uses a 4096-byte limit for CREATE. Its SQL text starts after the length of the prefix measured from the beginning of the stored definition. The final REPLACE uses a 256-byte formatting limit and %lu. Both hot-backup calls use slot 3. SQL text is observed at 0x080cc0c1 to 0x080cc19d.
- C++ source expresses the observed SQL message lifetime with explicit deletion. Original ownership wrapper spelling is unconfirmed. Build and ABI checks are deferred.
