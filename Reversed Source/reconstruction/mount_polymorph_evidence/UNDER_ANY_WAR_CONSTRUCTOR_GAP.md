# Optimized-away constructor state
SGuildWar(BYTE) is inline only (DWARF guild.h140). Retained DeclareWar080ae600 and ReserveWar080aed80 inline contexts zero time/score/map and set type, but both overwrite state before observation. Supplied reference uses GUILD_WAR_RECV_DECLARE; that initializer is NOT called binary-proven here. No production constructor body has been added.

The parent explicitly approved TEST ONLY sguildwar_lifecycle_fixture.cpp after inspecting this gap. All five named fields are set by under_any_war_regression.cpp before actual UnderAnyWar reads them. Existing CGuild lifecycle fixture remains test-only. Tests are staged, not compiled/run.

Integration: under_any_war_regression.cpp exports void CheckGuildWarLookup(); call it from existing guild_level_regression main, add this TU and sguildwar_lifecycle_fixture.cpp to that source group. Reuse full actual guild/map graph and established boundaries; no separate synthetic war executable is required.
