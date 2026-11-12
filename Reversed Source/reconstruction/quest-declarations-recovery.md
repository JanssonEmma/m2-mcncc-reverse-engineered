# Old quest declarations and event flags

quest.h, questpc.h, questnpc.h and questmanager.h retain complete old by-value data declarations for script/state records, RewardData, PC, NPC and CQuestManager. Target DWARF records their sizes as12/36/60/12,108,484 and216 bytes. NPC has20 event slots. The manager has compression_vector and lacks the newer other-PC stack fields. These declarations are not reduced event-flag-only classes.

questmanager.cpp currently implements GetEventFlag081a6bb0 only: exact std::string lookup, missing return0 and no insertion. The standalone suite checks absent/zero flags, signed extremes, case sensitivity, embedded NUL keys and updates. Character tests call this real lookup through ClearKillCount.

Lifecycle and Lua execution remain declaration-only production work. quest_flag_fixture.cpp provides explicit full-object manager construction/destruction for tests, with normal STL member lifetimes. PC/NPC destructor boundaries abort if reached; execution registries remain empty. These bodies are not production implementations. Other script-record constructors and nested quest-name hashing remain declarations pending recovery.

Structural checks verify complete supported script/state/reward record offsets under32-bit old-string compilation. Modern STL map/hash containers differ from GCC3.3, so full PC/NPC/manager ABI equality is not asserted. Full source layout and method dependencies require further recovery before a server link.
