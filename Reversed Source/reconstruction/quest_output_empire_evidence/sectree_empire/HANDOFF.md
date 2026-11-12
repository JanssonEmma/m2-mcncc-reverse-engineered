# GetEmpireFromMapIndex

The target SHA-256 was checked before exporting the original 32 instructions, zero-callee body and its caller list. SpawnMob invokes it at 0818d1d3, and SpawnMobRandomPosition invokes it at 0818ccd2. The complete call lists are saved alongside the method evidence.

The original classifies unsigned 32-bit map values 1–20, 21–40 and 41–60 as empires 1, 2 and 3. It classifies 184–185, 186–187 and 188–189 in the same order. Every other input returns zero, including private-map-like indices. There is no manager state read or call. The staged source preserves the original unsigned subtraction and comparison.

This is the remaining spatial manager lookup directly needed by these spawn/event paths. ProcessEventFlag uses the already integrated GetMapBasePositionByMapIndex. SpawnMob and CHARACTER::Show use Get. SpawnMobRandomPosition uses GetMap and SECTREE_MAP::Find. The original callees of those paths contain no GetSpawnPosition or GetMovablePosition. Spawn and event closures still require their own outstanding application dependencies.

The staged source appends the method to the current complete recovered sectree_manager.cpp. The staged regression extends the existing suite with the two ends of each interval and gaps, negative and extreme values, and the private-map non-normalization case. Strict C++03 32-bit syntax and optimized C++03 runtime both passed on the staged files using the real project header (chunk c05627). guards.json records the current project and staged bytes. No project writes or original execution.
