# Original configuration map-allow registry

The loaded x86 ELF SHA-256 was checked before extracting all four complete functions and the 64 direct callsites of SetEventFlag.

| Target | Address | ELF symbol | Instruction count |
|---|---|---|---|
| Map membership | 080d8030 | _Z14map_allow_findi | 22 |
| Map logging | 080d8080 | _Z13map_allow_logv | 27 |
| Map insertion | 080d80e0 | _Z13map_allow_addi | 48 |
| Map byte export | 080d81a0 | _Z14map_allow_copyPhi | 32 |

The original config.cpp owner is consistent with config_init and the reference owner. The private s_set_map_allows symbol is 12 bytes of original BSS at 0832325c and is logically std::set<int>. The reconstruction uses the native full STL type.

config_init at 080d8300 invokes map_allow_add at 080d96ab and 080d96d3. SetEventFlag at 081a5e60 invokes map_allow_find at 081a68ad. CLIENT_DESC::SetPhase at 0813f560 calls map_allow_copy at 0813f6a0. CGuild::RequestDeclareWar at 080ae260 calls map_allow_log at 080ae3f2. Membership also gates map and event consumers elsewhere.

The original disables membership checks on auth servers, while add still prints and inserts. This allows a repeated auth-mode insertion to use normal unique set insertion without taking the fatal branch. In non-auth mode the original writes its repeated spelling three times, prints Program Exit, and calls exit(1). The copy has an intentional original boundary where a nonempty set stores its first byte for any size, then keeps writing until count exceeds size or the set is exhausted. It advances the iterator before the byte store. The later reference's long pointer does not match the original byte store, symbol signature, or caller.

The staged locale_regression.cpp extends its existing real config graph. It exercises both auth modes, sorted signed ordering, modulo-256 byte narrowing, size -1, 0 and 1, guard bytes, empty input, missing lookup, auth-mode insertion and exact log observation. The fatal duplicate branch is supported by the full assembly and calls. The test does not invoke it.

Static expansion of the current test Makefile found 71 recipe commands which link direct config.cpp or its item_socket_config.o inclusion. Each already supplies sys_log from a test observer or logging fixture. item_socket_config.cpp currently renames g_bAuthServer to its unused config-side variant, while direct locale_regression links original g_bAuthServer. Any future feature relying on map_allow_find in that broad graph must account for that auth state explicitly.

SetEventFlag remains blocked on genuine ProcessEventFlag, SpawnMob, SECTREE_MANAGER ownership and CharStone drop-event state. The staged map cluster closes an authentic config dependency without altering SetEventFlag.
