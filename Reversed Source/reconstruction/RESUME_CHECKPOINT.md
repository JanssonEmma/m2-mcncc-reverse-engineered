# Metin2 reconstruction restart — checkpoint 112

Checkpoint 112 restored the original CHARACTER shop and exchange setters. 2 focused runtime suites passed. The registry has 120 suites. The last full run remains checkpoint 103.

Evidence, source guards and focused proof are in reconstruction/shop_state_evidence.

The next shop dependencies for AssignTriggers are CShopManager::Get, GetByNPCVnum, and the full CShop::AddGuest path. Saved StartShopping and warp callback evidence is under work/spawn_shop_warp. Full spawn, quest startup and server linking remain in scope.

Current scope excludes Monarch. Batch related focused tests. Keep the original ELF unexecuted and unmodified.
Shop continuation. work/shop_guest/HANDOFF.md records the complete lookup and AddGuest evidence, original owner layouts, and the three packed packet candidates. The 32-bit layout probe passed. Resume with genuine shop lifetime and guest packet tests. No source changes or builds are pending.
Shop lifetime closure audit now saved in work/shop_guest/lifecycle-calls.json and lifecycle-ghidra.json. Continue with the complete owners, constructors, Broadcast, owning Destroy/teardown, guest registration and original packet tests. Allowance was 96% used at the start of this continuation.

Source restoration batch 113. Restored shop.h, shop.cpp, shop_manager.h, shop_manager.cpp, the original packed shop records in packet.h and common/tables.h, and the Character shop/exchange accessors. Recovered methods include shop construction and teardown, Broadcast, AddGuest, manager construction and teardown, Destroy and both lookups. Source byte guards and original evidence are in work/shop_guest/source-applied.json and HANDOFF.md. This batch has no build or runtime receipt. Checkpoint 112 remains the last validated baseline. Per current user scope, defer builds/tests until the full source is restored and focus eventual tests on data. Next restore StartShopping and the shop trigger, then remaining shop operations and spawn dependencies.
Source batch 114 restored StartShopping in shop_manager.cpp and AssignTriggers/OnClickShop in original trigger.cpp. The three-slot trigger table and locale key were checked against the ELF. StartShopping preserves the original owner store after AddGuest, including when AddGuest returns false. Source hashes and decompiles are in work/spawn_shop_warp. Builds and runtime tests remain deferred. Next recover the other shop operations and SetProto dependencies ApplyPoint/ComputePoints and warp callbacks.
Source batch 115 restored shop creation, item placement and pricing, guest removal, item-update packets, manager initialization, PC shop create/find/destroy, and StopShopping. Original evidence and before/after hashes are in work/shop_guest. The table advances only after successful slot placement. The original NPC and PC index ownership and the empty PC update branch were preserved. Builds and runtime tests remain deferred. Next recover CShop::Buy and the manager Buy/Sell paths, then return to remaining spawn dependencies.
Source batch 116 restored CShop::Buy, manager Buy/Sell, CanHandleItem, GetShopBuyPrice and GoldBarLog. Monarch treasury transfers were omitted as instructed. The original price and tax arithmetic, other-empire markup, guest/position checks, item moves and log calls remain. Remaining source dependencies include PointChange, item creation and removal, voucher use and the sold-item registry. Complete original decompiles, exact locale bytes and source hashes are in work/shop_guest. Builds and runtime tests remain deferred.
Source batch 117 restored CHARACTER::PointChange, ApplyPoint, ComputeBattlePoints, ComputePoints, ComputeAffect, RefreshAffect and CItem::ModifyPoints in their original translation units. Evidence is in work/point_change, work/stat_apply_scc and work/stat_compute_scc. Source hashes and rollback copies are in work/stat_source_batch. Continue with the staged item lifecycle under work/point_change/auto_give, including set_count/removal.cpp.candidate, then the staged affect and mount/polymorph dependencies. Unresolved callees include StartMuyeongEvent, MountVnum, SetPolymorph, AutoGiveItem and quest LevelUp. Monarch remains excluded. Builds and tests remain deferred.
Source batch 118 restored item creation and destruction, stack count changes, removal from the character, unequipping, wear-slot updates, AutoGiveItem, both RemoveAffect overloads, and the accessory expiration/degrade chain. Source was placed in the original item.cpp, item_manager.cpp, char_item.cpp and char_affect.cpp with corresponding declarations. Evidence comes from work/point_change/auto_give, work/stat_apply_scc and work/stat_affect_scc. Source hashes and originals are in work/item_lifecycle_source_batch. Next recover ITEM_MANAGER::RemoveItem and CHARACTER::CheckMaximumPoints, followed by the saved Muyeong and mount/polymorph bodies. Monarch stays excluded. Builds and tests remain deferred.
Source batch 119 restored ITEM_MANAGER::RemoveItem, safebox removal, movement and teardown, CHARACTER::CheckMaximumPoints, SetPolymorph, MountVnum and StopRiding in their original translation units. Original decompiles for RemoveItem and CheckMaximumPoints, source hashes and rollback bytes are in work/removal_source_batch. StopRiding still needs real HorseSummon and quest Unmount implementations. Continue with the staged horse and quest wrappers in work/mount_polymorph, then the Muyeong callback in work/stat_affect_scc. Builds and tests remain deferred. Monarch remains excluded.
Source batch 120 restored HorseSummon and its dead-horse event, the Muyeong scheduler and callback, quest Unmount and LevelUp, NPC all-event dispatch, script execution and Lua state resumption. Evidence and source hashes are in work/horse_source_batch, work/stat_affect_scc/muyeong-source and work/quest_unmount_source_batch. Muyeong preserves the original dangling temporary set reference and still needs the full ComputeSkill and battle_is_attackable bodies. Quest dependencies include PC::EndRunning and GotoConfirmState. Saved new Ghidra exports for VCardUse and StartWarpNPCEvent are in work/horse_source_batch/next-ghidra.json. Builds and tests remain deferred. Monarch stays excluded.
Batch 120 also restored CHARACTER_MANAGER::SpawnMob, CHARACTER::SetProto, Show and Goto, and the ore expiration callback. Existing spatial lookups were reused. See work/spawn_source_batch for the guarded source batch and evidence. Continue with StartWarpNPCEvent and its callback, StartAffectEvent, mob attribute application and movement dependencies.
Source batch 121 restored warp NPC scheduling, sector scan and proximity checks, the original IsHack portal guard and limit, affect scheduling and periodic recovery/expiry, premium and quest flag access, mob attribute application with both original ELF tables, and the battle, arena, castle and guild attack checks. Ghidra exports and source hashes are in work/warp_affect_source, work/mob_attribute_source_batch and work/battle_source_batch. Pending warp dependencies include WarpSet and CMapLocation, with the original WarpSet export saved. Pending combat dependencies include CPVPManager::CanAttack and full ComputeSkill. Monarch stays excluded. Builds and tests remain deferred.
Batch 121 also restored quest confirmation scheduling and resume, PC::EndRunning, reward and deferred state-change processing, and state event dispatch. See work/quest_completion_source_batch. The new transfer evidence includes both CMapLocation::Get overloads, Stop and WarpEnd under work/warp_affect_source.
Source batch 122 restored WarpSet, Stop, the packed 15-byte warp packet, CMapLocation lookup and insertion, and sector map-index lookup. Also restored MonsterChat, IsGuardNPC and quest::ScriptToString. Guards are in work/warp_transfer_source and work/monster_chat_source_batch. The original WarpEnd and P2P send/connector exports are staged for the receiving side. Builds and tests remain deferred. Monarch remains excluded.
Batch 122 also restored CPVPManager::CanAttack, PvP lookup, timing and map lifetime, and the character PK/alignment/killer-mode methods. See work/pvp_source_batch. Fight registration, agreement and deletion remain separate source work.
Source batch 123 restored WarpEnd, the 33-byte peer login and 4-byte setup records, P2P broadcast, boot and connector/acceptor registration, VCardUse with the original 116-byte DB record, PvP duel registration/agreement/cleanup including GiveUp, and quest Input/Select/SelectItem/Resume. Evidence and guarded source receipts are in work/warp_arrival_source, work/vcard_source_batch, work/pvp_duel_source and work/quest_resume_source_batch. Next restore RecvLogin/RecvLogout, peer disconnect cleanup and remaining P2P lookup/count methods from fresh exports in work/warp_arrival_source. Builds and tests remain deferred. Monarch stays excluded.
Source batch 124 restored peer disconnect cleanup, name and PID lookups, host/port matching and user counts, with the original 48-byte CCI field layout from DWARF. Receipts and complete RecvLogin/RecvLogout C++ translations are in work/p2p_receive_source. The two receive translations are staged pending the guild, party and messenger manager callback owners. Builds and tests remain deferred. Monarch remains excluded.
Batch 124 also restored ten original ComputeSkill dependencies and the complete item sale registry in itemlog.cpp, including the original index behavior. Evidence is in work/skill_dependencies_source and work/itemlog_source_batch. Skill map insertion preserves the original initialization of bUsed alone. Startup wiring for InitItemLog belongs with main.cpp. The guild, party and messenger peer callback exports are in work/p2p_receive_source/manager-callbacks-ghidra.json.
Batch 124 also restored quest login/logout, player/server timer dispatch, no-wait login dispatch and suspended-script cleanup. Four source paths and fourteen original exports are recorded in work/quest_lifecycle_dispatch_batch.
Source batch 125 restored original CInputP2P construction/type/dispatch, login/logout/setup entrypoints, peer warp and transfer handlers, the character exit/warp location helpers, and the party peer callbacks with original packed PartyAdd. Evidence and source hashes are in work/p2p_input_source and work/party_p2p_source. Other packet handlers remain separate original source work. The original missing-party-member insertion leaves some scalar fields indeterminate. Monarch stays excluded. Builds and tests remain deferred.
Batch 125 also restored FindPosition and the original MessengerManager peer callbacks, account list query, mobile data and notifications. Work receipts are in work/p2p_input_source and work/messenger_source_batch. Messenger still depends on DBManager::FuncQuery dispatch and gm_get_level. P2P receive apply is staged in work/p2p_receive_source/apply_receive.py.
Batch 125 integrated guild peer callbacks and both full P2P_MANAGER receive methods after restoring party and messenger owners. Guild source and evidence are in work/guild_p2p_source_batch and final peer receive guards in work/p2p_receive_source. Remaining upstream gaps include guild owner construction and teardown, messenger async DB dispatch, and the other CInputP2P handlers. Original ExitToSavedLocation calls WarpSet with the warp fields before clearing the exit fields; this behavior is preserved.
Source batch 126 restored peer relay and notice, disconnect, shout, messenger add/remove/mobile, guild war map, chat block, forked map and billing login ping handlers. Also restored duplicate-login delayed disconnect and callback with exact original locale bytes. Source hashes and exports are in work/p2p_handlers_source. Remaining original callees include notice/shout broadcasts, language conversion, messenger list mutations, AddAffect, billing and forked-map storage. Monarch stays excluded and builds/tests remain deferred.
Batch 126 also restored SetForkedMap in questlua_forked.cpp, DBManager callback query registration and three-way result dispatch, guild manager startup/teardown and guild mark/image/util owners, plus SetGuildWarMapIndex. Receipts are in work/p2p_handlers_source, work/db_query_dispatch_source_batch and work/guild_mark_lifecycle. Pending DB AnalyzeReturnQuery has fourteen original cases plus default saved in full. Mark virtual operations and other peer callees remain in scope.
Batch 126 also restored attack rating and bonuses, arrow/melee/magic damage, weapon damage extraction and mob damage helpers. Exact binary constants and x87 evidence are in work/damage_source. GetMarriageBonus remains a real missing dependency. Cross-compiler intermediate floating-point parity is unverified.
Source batch 127 restored peer shout and notice broadcasts, the original empire text conversion routines and 10,052-byte table layout, original replacement bytes, and locale conversion table loading. Binary exports and source guards are in work/peer_broadcast_source. Table loader startup wiring remains separate. Monarch stays excluded. Builds and tests remain deferred.
Batch 127 also integrated original CLoginData, DB login-data and billing helpers, AddAffect and original packet records, poison/fire cleanup, battle end and movement sync. Binary evidence and source guards are in work/db_login_data_source_batch and work/add_affect_source. Auth prepare, billing owner and AnalyzeReturnQuery remain open. Original guild symbol packet added with source guard in work/peer_broadcast_source.
Batch 127 also integrated fourteen original guild mark virtual operations, symbol send/load/save, mark index and allocation, CRC and block compression helpers. See work/guild_mark_operations. Old unchecked malformed-file and zero-size symbol paths were preserved. Remaining source includes auth/database return handling, guild and messenger lifecycle dependencies, full ComputeSkill, and server main.
Source batch 128 restored peer friend add/remove and mobile updates, and original billing expiration, billing checks and auth-login send. Source receipts and binary evidence are in work/messenger_mutation_source and work/db_auth_billing_source_batch. Corrected the original mark packet tag and nonconst DESC::GetHandle signature from DWARF. FlushBilling, LoginPrepare and AnalyzeReturnQuery remain open. Monarch stays excluded. Builds and tests remain deferred.
Batch 128 also restored the original splash-affect operator, movement and mining cancellation hooks, fight initiation and victim/state helpers in work/splash_affect_source. Full ComputeSkill still needs splash damage and broad combat runtime.
Batch 128 also restored start, destroy, idle, usage, ShutdownOnFatalError and Metin2Server_Check in main.cpp. Source hashes and original evidence are in work/server_main_source. The main entry point, heartbeat, io_loop and manager boot dependencies remain open. The original server binary was not executed.
Source batch 129 restored original GM authority and host registry, exact 104-byte admin record, and client CRC/version loading and checks, including delayed disconnect and exact version mismatch locale. Receipts and Ghidra exports are in work/gm_source_batch and work/client_version_source. Original parser handling of malformed or empty files was preserved. Monarch stays excluded. Builds and tests remain deferred.
Batch 129 also restored FlushBilling with the original forced and capped normal paths, and ten skill/combat effect routines including poison, fire, immunity, good-affect removal and chain-lightning count. Receipts are in work/db_flush_source_batch and work/skill_damage_effect_source. Full CHARACTER::Damage remains pending.
Batch 129 also restored the original server io_loop and UDP processor, including dispatch thresholds and the original discarded accumulator behavior. Evidence and source guards are in work/server_loop_source. Heartbeat, top-level main and auth LoginPrepare remain follow-on work.
Batch 129 integrated LoginPrepare, original matrix authentication data and lookup/coordinate generation, and LoginFailure. See work/db_login_prepare_source_batch. CheckTeleBlock and CheckPasspod remain genuine direct dependencies.
Batch 129 also integrated the original heartbeat pulse and shutdown flow, packed 1818-byte player table and login/count/load records, and the original CLoginSim owner declaration. See work/server_loop_source. Main entry point and original login simulation implementations remain open.
Source batch 130 restored the remaining peer guild chat and Santa transfer handlers, the Santa respawn callback and random mob placement, and original SQL-backed CheckTeleBlock/CheckPasspod. Binary exports and guards are in work/seasonal_peer_source and work/db_legacy_auth_checks_source_batch. No service was contacted. Monarch remains excluded. Builds and tests remain deferred.
Batch 130 progress estimate. 75 of 120 original non-Monarch DWARF game-server .cpp filenames that also exist in the supplied reference are present in the reconstruction (62.5%). File presence includes partial files, excludes original-only names and is not a code coverage metric. Rough source completion estimate is about half. Full main, combat, DB dispatch and other module bodies remain open.
Batch 130 also integrated FuncSplashDamage and ChainLightningEvent from work/splash_damage_source. Remaining dependencies include CHARACTER::Damage, Sync and CParty::GetNextOwnership. Startup agent is preparing original main and missing stack owners under work/server_main_source or its follow-on directory. No builds or tests were run.
Source batch 131 restored exchange construction, cancellation, gold offers, item removal and inventory validation, original item exchange flag setter and 48-byte exchange packet. Evidence/guards in work/exchange_source_batch. AddItem and Accept decompiles are saved and remain to be integrated with Done and CheckSpace. Builds and tests deferred. The separate DB server and database assets are a later project milestone.
Batch 131 also integrated original main entry and WriteVersion (work/server_entry_source), Sync and party ownership plus quest attribute dispatch (work/sync_party_source). Startup still depends on original owner implementations. Sync still calls pending full Dead. The CExchange friend for future Accept may be applied with that body.
Batch 131 restored login-key lookup, creation and expiry in desc_manager.cpp (work/db_login_key_source_batch). Expiry saves the value before erase instead of reading the erased node. Remaining DB return handler work is open.
Source batch 132 restored exchange item offers, space checking, commit and accept logic, and eleven character safebox/mall methods. Binary exports and guards are in work/exchange_source_batch and work/char_safebox_source_batch. ExchangeStart and client input wiring remain pending. Damage and Dead exports are saved in work/char_damage_source. Builds and tests remain deferred.
Batch 132 also integrated ExchangeStart in its original exchange.cpp and recovered damage packet emission, last-attacked state, mob HP/drain and death-blow helpers, plus stone-skin/death-blow state predicates. Receipts in work/exchange_source_batch and work/char_damage_source. Remaining trade work is client packet wiring. Damage and Dead bodies remain pending.
Source batch 133 restored CInputMain::Exchange in input_main.cpp from the original 7-byte packet path. Full Analyze dispatch and quest GiveItemToPC remain pending. Evidence and source guards are in work/exchange_source_batch. Continue reference-first with binary verification of changed control flow, constants and data layouts. Builds and tests remain deferred.
Source batch 134 restored GiveItemToPC, target lookup and original 112-byte TargetInfo, NPC target-event dispatch and Lua condition evaluation. Reference branch structure was compared to original Ghidra exports, with event slot 17, target pointer/layout and signatures checked against the binary. Guards and exports are in work/quest_target_source. Client Analyze dispatch and target creation/deletion remain pending. Monarch remains excluded. Builds and tests remain deferred.
Source batch 135 restored target creation/replacement, deletion, per-player logout, target lookup and event processing with original arrive/die quest callbacks and distance-based scheduling. The three target packet layouts were checked against original DWARF (43/13/5 bytes). Original exports and source guards are in work/target_lifecycle_source. Next restore questlua_target bindings and target logout callers, then resume client Analyze and broader game-server dependencies. Builds and tests remain deferred.
Source batch 136 restored the original questlua_target.cpp bindings and registration table. The ELF table at 08230ab4 verifies aliases npc/pc, function addresses and order. x87 conversion uses truncation; old result counts, including target_id invalid argument pushing zero then returning zero, are preserved. Source guards and exports in work/questlua_target_source. The startup Lua initialization caller and character disconnect target cleanup still need integration. Builds and tests remain deferred.
Source batch 137 restored the original character Disconnect sequence, including target and messenger cleanup, peer logout, item and affect persistence, and quest LogoutPC/DisconnectPC. Also restored character delayed-save flush and the original 26-byte peer logout record. Ghidra exports and guarded source hashes are in work/disconnect_source. Remaining direct dependencies include SaveReal, DeathPenalty, arena and marriage logout, war/wedding map setters, guild logout and session logging. Lua target startup registration also remains pending. Monarch stays excluded. Builds and tests remain deferred.
Source batch 138 restored CreatePlayerProto with original 1818-byte serialization, playtime/alignment and unique-item helpers, session LoginLog and guild member logout. Ghidra evidence and guarded source hashes are in work/player_save_source. SaveReal is exported but still needs quest GetPCForce/PC::Save and the marriage save owner. Remaining disconnect dependencies include arena/marriage logout and war/wedding map membership. Builds and tests remain deferred.
Source batch 139 restored SaveReal using existing quest persistence and original marriage lookup/save/update with the DWARF 48-byte owner and 13-byte DB packet. Evidence and hashes in work/save_real_source. Marriage logout remains open. Standing workflow for all follow-on tasks: start from the supplied compiling reference, batch-map original ELF symbols and DWARF to source, compare signatures/calls/branches/constants/layout, decompile changed or uncertain paths, and retain compact evidence and hashes. Use independent subagents selectively for non-overlapping batches. Builds and tests remain deferred.
Source batch 140 restored marriage logout, partner clearing, and cancellation of the proximity event. Compared the reference path with saved original marriage exports and checked the two leaf methods in Ghidra. Original early return when both players are offline is preserved. Receipts in work/marriage_logout_source. Arena logout and war/wedding membership still remain. Builds and tests deferred.
Source batch 141 restored arena logout, duel teardown, observer return warps and event cancellation. Original return positions and locale bytes came from the ELF; source receipts in work/arena_logout_source. Recovery-event implementation remains pending together with war/wedding memberships. Builds and tests remain deferred.
Source batch 142 restored original recovery scheduling and callback, SP distribution, killer-mode expiry, and target selection/expiry with horse and mob HP display. Recovery percent data matched the ELF table [1,5,5,5,5,5,5,5,5,5]. Preserved the original poisoned-player return of three pulses. Ghidra exports and guarded hashes are in work/recovery_source. Next address war and wedding membership, client dispatch, or the remaining damage closure. Builds and tests remain deferred.
Source batch 143 restored wedding map membership, character transfer/removal, and local darkness, snow and music notifications. Binary verification established the original 36-byte owner, level threshold and music command length semantics. Evidence and guarded hashes in work/wedding_membership_source. Wedding map creation/teardown and guild war membership remain open. Builds and tests deferred.
Source batch 144 restored wedding-map and manager lifetime, map lookup, end notices and guest reward, timed exit and private-map teardown. The 2009 constructor leaves m_isMusic indeterminate, which is preserved. Original event info comes from the zeroing allocator. Evidence and guarded hashes are in work/wedding_teardown_source. Private-map destruction implementation and wedding map creation/request remain pending. Builds and tests remain deferred.
Source batch 145 restored private-map and sector teardown, including original entity destruction order and shared clone-attribute ownership. Also restored WeddingManager::End and IsWeddingMap. The original removes the map registry entry before deleting the map and performs entity cleanup inside sector destruction. Evidence and source guards are in work/private_map_source. Full original map copy/Build, CreatePrivateMap and wedding create exports are saved there for the next batch. Builds and tests deferred.
Source batch 146 restored sector-map construction and clone, neighbor list building, and private-map creation with original index scan and map 107–109 limits. The eight neighbor offsets were extracted in original order from ELF .rodata. Evidence in work/private_map_source and guarded hashes in work/private_map_create_source. Wedding creation/request and regen integration remain next. Builds and tests remain deferred.
Source batch 147 restored wedding instance creation and DB-ready reply, with original map 81, one-shot NPC regen path and 12-byte DB record. Also restored the original regen owner declaration from DWARF (64 bytes x86). Full regen_do/read_line exports and source guards are in work/wedding_create_source. Regen token parsing and spawning, including dungeon registration, remain pending. Builds and tests deferred.
Source batch 148 restored original regen tokenization and record parsing in regen.cpp. Compared against reference and verified original differences, including only space/tab/LF delimiters, quoted tokens, comment handling, and original time suffix and coordinate conversion. Receipts are in work/regen_parser_source; regen_do/read_line are in work/wedding_create_source. Next restore spawn dispatch and dungeon registration with genuine group/range paths. Builds and tests remain deferred.
Source batch 149 restored original ranged spawn retries, forced stone spawn motion, aggression, regen association and stone ownership. Evidence and guards are in work/regen_spawn_source. Original SpawnGroup, SpawnGroupGroup and SetDungeon exports are saved there. Group mob owner, party creation/link and dungeon member/count implementation remain pending for full regen spawn integration. Builds and tests remain deferred.
Source batch 150 restored original mob group data layouts, weighted group selection, SpawnGroup and SpawnGroupGroup. Original 20/28-byte owner layouts and selection RNG were verified. Evidence and hashes in work/mob_group_source and work/regen_spawn_source. Group loading, dungeon attachment and NPC party CreateParty/Join/Link still require implementation. Builds and tests deferred.
Source batch 151 restored LoadGroup and LoadGroupGroup using the existing text-file loader and verified original branch behavior from work/mob_group_source. Group-group files parse the optional probability but accumulate unit weights, as the original did. Original numeric conversion and first missing numbered member behavior preserved. Guards in work/mob_group_load_source. Startup initialization and NPC party/dungeon dependencies remain pending. Builds and tests deferred.
Source batch 152 restored CMobManager::Initialize and RebindMobProto in mob_manager.cpp. Original table import updates the first three skills, inserts both lookup keys, registers race and quest NPC IDs, loads both locale group files, and rebinds by the character PID index. Existing quest registration was verified against the original and reused. Evidence, backups and hashes are in work/mob_initialize_source. Next restore dungeon attachment, NPC party creation/join/link and regen dispatch. Builds and tests deferred.
Source batch 153 restored regen_do, dungeon spawn dispatch, scheduled callbacks and the original CDungeon owner fields and regen list lifecycle. DWARF and source hashes are in work/dungeon_regen_source. The original four-map exclusion and fixed-position spawn precedence were preserved. CHARACTER::SetDungeon and its dungeon membership/count dependencies remain open, along with NPC party and map regen. Builds and tests deferred.
Source batch 154 restored CHARACTER::SetDungeon, original dungeon member/party/monster accounting, empty-dungeon expiry, elimination exit/warp callbacks, regen handoff, map-indexed dungeon lifecycle and area lookup. Evidence and hashes in work/dungeon_members_source. NPC party creation/join/link and map regen remain pending. Builds and tests deferred.
Source batch 155 restored NPC and PC party creation, Join/P2PJoin, Link, member registration, character party state, linking packets and the periodic update callback. Original NPC VID and PC PID paths were preserved. Evidence and source hashes are in work/party_link_source, including queued teardown exports. Next restore party quit/unlink/destroy and PC periodic Update, then map regen. Builds and tests deferred.
Source batch 156 restored party quit, unlink, deletion/destruction, teardown packets and role point application/removal. Original packet widths, locale key and role constants were recovered. Caches preserve fields across the original post-erase and post-delete paths. Evidence and source hashes are in work/party_teardown_source and work/party_link_source. Remaining dependencies include CParty::Update and CHARACTER destruction and state transitions. Map regen remains pending. Builds and tests deferred.
Source batch 157 restored map-wide and one-shot regen loading, periodic spawn callbacks, exception rectangles, reset/free, test-server spawn totals and NPC position indexing. One-shot map spawns skip storing the original temporary record pointer to avoid dangling references. Evidence and hashes in work/map_regen_source. World startup still needs SECTREE_MANAGER::Build and its asset loaders, and character destruction needs regen count release. Builds and tests deferred.
Source batch 158 restored SECTREE_MANAGER::Build and original Setting.txt, Town.txt, server_attr, sector grid and dungeon area loaders. Per-map regen is wired in original regen/npc/boss/stone order. Parsing now bounds reads and skips invalid index rows instead of reusing stale parse state. Evidence and source hashes in work/world_assets_source. Full DB boot invocation remains pending, along with character teardown and party Update. Builds and tests deferred.
Source batch 159 restored original CInputDB frame processing and staged the post-table startup sequence in input_db.cpp, including item ID, ordered drop files, world map build and post-map hooks. Frame parsing checks the full nine-byte header and rejects negative lengths. Boot table parsing, Analyze dispatch, descriptor hookup and remaining drop/building/castle/PCBang dependencies are open. Saved full Boot/Analyze/Process evidence in work/db_boot_source. Builds and tests deferred.
Source batch 160 restored version-5 CInputDB::Boot table parsing and boot-only Analyze dispatch (0x2b), shared land/object records, and the original refine recipe import. Tables are bounded by the DB frame size; optional legacy trailing bytes are opaque and the terminal 0xffff is validated. FinishBoot is now called from the parser. Other Analyze cases, descriptor hookup, item and building initialization bodies and drop assets remain pending. Source hashes in work/db_boot_tables_source, original boot evidence in work/db_boot_source. Builds and tests deferred.
Source batch 161 restored item prototype initialization, reverse refine lookup, quest item registration, live item prototype rebinding, original name prefix lookup, and etc drop file parsing. Live vnums are captured before invalidating prototype storage. The original name match is case-insensitive and the probability scale is 10000. Source hashes in work/item_boot_source; item initialize evidence in work/db_boot_tables_source. Common, monster and group drop loaders and building boot remain open. Builds and tests deferred.
Source batch 162 restored common drop file loading and shared rank records. Original parser reads four rank columns into six rank buckets, rounds probability at scale 10000, falls back to numeric item vnums, and sorts by ending level. Bounded item name copying replaces the original unchecked field copy. Evidence and hashes in work/common_drop_source. Monster, special and group drop loaders and building boot remain open. Builds and tests deferred.
Source batch 163 restored ReadDropItemGroup and the original 24-byte x86 group owner with 12-byte entries. Loader appends to existing mob groups, stops at the first missing numbered row and rounds probability at scale 10000. Rows missing required fields fail cleanly. Evidence and hashes in work/drop_group_source. Special and monster drop loaders and building boot remain pending. Builds and tests deferred.
Source batch 164 restored ReadMonsterDropItemGroup and its kill, drop, limit and thiefgloves owner records. Kill weights are cumulative; other probabilities are rounded at scale 10000. Duplicate keys preserve the first group and release the unused allocation. Incomplete rows return failure. Saved original decompile in work/drop_group_source and source guards in work/monster_drop_source. Remaining work includes special drop loading, runtime drop selection, and building boot. Builds and tests deferred.
Source batch 165 restored ReadSpecialDropItemFile and its 28-byte x86 owner. The original uses one weighted group type, six exact pseudo-item names, and reverse indexing for group IDs below 30000. The later quest and attribute group modes are absent in this binary. Incomplete rows fail and temporary groups are freed on error or duplicate. Evidence and source hashes in work/special_drop_source. Next restore runtime selection and building boot. Builds and tests deferred.
Source batch 166 restored original single weighted special reward selection, group lookup and CHARACTER::GiveItemFromSpecialItemGroup in char_item.cpp. Item, point, debuff, poison and hostile spawn outcomes preserve original output/reference behavior. Empty or unselectable groups return false. Chest item-use dispatch and regular monster drop creation remain pending. Evidence and hashes in work/special_reward_source. Builds and tests deferred.
Source batch 167 restored monster item rate selection with premium and NewCIBN overtime, mob drop and polymorph prototype accessors, kill-group weighted selection, and GetDropPerKillPct. Null characters use the base rate and empty groups fail selection safely. Full CreateDropItem decompile and helper exports saved in work/monster_reward_source. Verified base roll range 4000000 in assembly, divided by three on distribution test servers, with an original division-by-zero branch on privilege plus unique-item flag equal to -100. Full generation and event dispatch remain pending. Builds and tests deferred.
Source batch 168 restored CreateDropItem for common, monster, level-limited, glove, etc, metin, horse and lotto drops, DB money logs, and the privilege lookup it calls. Original roll order and constants verified using work/monster_reward_source. The division-by-zero privilege combination now returns failure. Remaining dependencies include CreateQuestDropItem, privilege lifecycle, distribution test config parsing and the combat reward caller. Source hashes in work/create_drop_source. Builds and tests deferred.
Source batch 169 restored CreateQuestDropItem and the original character-stone event configuration and drop routine. Preserved event roll order, level and rank gates, and the binary New Year, Valentine, White Day item IDs. The drop routine reuses the existing character-stone state and setter, including the recovered defaults. Saved evidence and hashes in work/quest_drop_source. Quest SetEventFlag hookup and combat reward integration remain open. Builds and tests deferred.
Source batch 170 restored DB event flag packet 0x69, quest flag updates, monster/player rates, CRC and test settings, client broadcasts, Christmas spawns, and the character-stone event hook. Both firework position tables matched the ELF and share the same source implementation. Malformed flag packets fail length or termination checks. Evidence and hashes in work/event_flags_source. DB descriptor hookup, other DB dispatch and combat reward integration remain open. Builds and tests deferred.
Source batch 171 restored outbound descriptor factory/retry, DB and P2P phase processor selection, one-time boot request, setup/login replay packets, and connector socket teardown. DWARF sizes 24/58/90 are enforced in source. Original DB guild-war teardown dependency, Netmarble/teen phases and other Analyze cases remain open. Source guards in work/db_descriptor_source. Builds and tests deferred.
Parallel handoffs for the next batches are under work/db_dispatch_next, work/building_next and work/reward_next. DB startup dispatch identifies map location 0xfe, P2P announcement 0xff and time 0x5a.
Batch 171 also restored DB startup map locations, P2P announcements, and time updates from the parallel dispatch handoff. Source hashes in work/db_startup_dispatch_source and original evidence in work/db_dispatch_next. The handler bounds payloads before use and rejects unterminated hosts.
Source batch 172 restored DB notices, login failure replies, billing expiry/check, and money log dispatch. It also restored LogManager::MoneyLog and ground item placement plus 300-second expiry. DB payloads are bounded before use. Evidence and source hashes are in work/db_service_dispatch_source, work/db_dispatch_next and work/reward_next. Builds and tests deferred.
Batch 172 also restored exact movable-position search (161 offsets), gold amount/drop rates and kill logging, and building boot imports and finalization. Source receipts in work/reward_next and work/building_next. Remaining dependencies include building attribute rasterization and guild/NPC hooks, combat RewardGold/DistributeExp/Reward, and the remaining DB login/player dispatch.
Source batch 173 restored DB AuthLogin (0x7a) and LoginAlready (0x22) dispatch. Auth response offsets 0/1/5 verified by original DWARF. Duplicate login paths disconnect a local session or send the 26-byte P2P packet, bounding the wire name. Evidence in work/db_dispatch_next and source receipt in work/db_login_replies_source. LoginSuccess needs descriptor account binding and the player list packet. Builds and tests deferred.
Batch 173 also restored LoginSuccess (0x1e), account binding, location resolution with original empire fallback coordinates, and character-list response. Uses the original 32-bit account/player ABI. Input DB success/descriptor receipts in work/db_login_replies_source and work/db_dispatch_next. PlayerLoad (0x23) and its character/map dependencies remain open. Combat gold rewards and building occupancy were deferred when child sessions lost authentication.
Source batch 174 restored PlayerLoad (0x23), map and recall queries, SetPlayerProto, main/points/skill packets, gold reward distribution and sector occupancy. Source evidence and hashes in work/player_location_source, work/db_dispatch_next, work/reward_next and work/building_occupancy. Points packet zeroes the original two uninitialized fields. Gold rewards guard a zero jackpot divisor and seed the first drop position from the victim. PCBang IP lookup implementation, combat EXP/Reward, guild building NPC hooks and further DB dispatch remain open. Builds and tests deferred.
Source batch 175 restored PCBang IPv4 lookup/refresh and DB return query cases 11/12, player create/delete replies 0x25 through 0x29, combat EXP attribution/party shares, and guild building NPC and altar effects. PCBang update cycle/minimum play time defaults verified as 10/10. Malformed IPs are rejected. Other return query cases, PCBang playtime logging, GiveExp, combat Reward/Dead, and China event config parsing remain open. Source evidence and hashes in work/pcbang_source, work/db_dispatch_next, work/reward_next and work/building_guild_npc. Builds and tests deferred.
Source batch 176 restored PCBang playtime logging on disconnect using the original minimum time and registered IP checks. Verified SQL literal and LogManager argument widths at ELF 08078970/081fa400. Source receipt in work/pcbang_log_source. Builds and tests deferred.
Batch 176 also restored quest NPC and party kill dispatch, GiveExp and its marriage/rate helpers, combat Reward, building land and object runtime cleanup, and safebox/mall replies 0x2d through 0x30 and 0x33. Evidence and source receipts in work/quest_kill_source, work/reward_next, work/combat_reward_source, work/building_runtime and work/db_dispatch_next. Reward uses a per-call drop vector, GiveExp guards a level-zero marriage calculation, and building deletion erases the actual VID. Death still needs to call Reward. Builds and tests deferred.
Batch 176 also restored the lotto SQL return handler for query case 3, using the existing character lookup, item grant, and socket setters. Evidence and receipt in work/db_dispatch_next. Builds and tests deferred.
Batch 176 connected building create, delete and land update to original DB packets 0x8c–0x8e. The handlers validate payload sizes and copy to aligned local records. Evidence and source receipts in work/building_runtime and work/building_db_dispatch.
Batch 176 connected DB empire selection (0x31) and character rename (0x79), including client replies and the refreshed character list after empire selection. The start-coordinate lookup bounds the empire index. Evidence and receipt in work/db_dispatch_next.
Batch 176 restored original mob revive state and party reviver checks. Saved Dead call-site evidence in work/reward_next. Dead suppresses reward on instant flag 0x10 or qualifying guild-war kills, spawns a death replacement before Reward(false), and otherwise calls Reward(!IsRevive()). Dead and its event lifecycle are still open.
Source batch 177 restored building land list and coordinate lookup, original five-function building Lua registration and DB requests, NPC position client packet (5+34-byte wire layout), and login character select/delete/rename and empire request dispatch. NPC packet rejects lists exceeding the wire size. Rename bounds the slot and terminates the name before validation. DB QuestLoad 0x2c restores flags and the phase-delayed quest login event, guarding short/unterminated rows and using the active PID on a zero-row reply. Evidence and source receipts in work/building_client_land, work/building_quest_lua, work/npc_positions, work/login_requests and work/db_dispatch_next. Login Analyze is partial, and Entergame and VM startup still need to call the restored helpers. Builds and tests deferred.
Source batch 178 restored password login, LoginByKey, DB country range/exception updates, descriptor user counts and original 32768 user-limit default, arena membership/observer helpers, and dead_event plus DeathPenalty. Login requests use bounded copies of client fields and user counting bounds the empire slot. Death event guards access after party Quit destroys a leader party. Evidence and receipts in work/login_password, work/db_dispatch_next, work/arena_enter_game and work/reward_next. Entergame and full Dead integration remain open. Config parser still needs the original user_limit token. Builds and tests deferred.
Batch 178 also restored ReviveInvisible, sync ownership, stone cleanup and dungeon death counters and hooks. The stone cleanup reacquires its child iterator after recursive death, and dungeon counters initialize to zero. Evidence and receipt in work/reward_next. Hooks await Dead integration.
Source batch 179 restored login CRC/version dispatch, guild names and active war packets, character greeting and periodic save event (120 seconds at default tick rate). Greeting treats configured text as literal. Save cancellation runs in DestroyCharacter pending the original CHARACTER destructor; it must move into that destructor during restoration. The save event config token remains open. Receipts and binary evidence in work/db_dispatch_next, work/guild_enter_game, work/enter_lifecycle and work/login_password. Builds and tests deferred.
Batch 179 also wired client character creation 0x04 to DB 0x05 with original stats and coordinates, added guild initial grade/member/skill/info packets and member limit/land helpers, and restored the speed-check event, SQL logging and descriptor teardown. Speed check uses the original 60-second interval and threshold 50 and suppresses duplicate event creation. Entry event teardown currently runs in DestroyCharacter. Character creation rejects invalid empire values and bounds names; guild member packet bounds the wire name. LoginMember still requires the SendEnemyGuild/TouchGuild dependency. Receipts and evidence in work/db_dispatch_next, work/guild_enter_game and work/enter_lifecycle.
Source batch 180 restored war-map lookup, OX event admission and shutdown/participant warp, original login privilege notices, stun and affect cleanup, and the locale-specific item-drop penalty. Item-drop recovery preserves the original POSIX shuffle and adds null guards. OX quiz administration, war-map ownership, full Entergame and Dead remain open. Evidence in work/enter_map and work/reward_next. Builds and tests deferred.
Batch 180 also restored the five original login guild mark and symbol requests around the existing libguild backend. Symbol upload bounds the declared length and returns only bytes beyond the fixed header, avoiding the original double consumption in the frame parser. Startup mark data and handshake routing remain open. Evidence and receipt in work/db_dispatch_next.
Batch 180 also restored arena duel death and ready-event transitions, guild LoginMember and TouchGuild asynchronous load, and guild mark/symbol boot loading. Arena restores the original empty observer helper and adds payload, descriptor and duplicate timer guards. Guild loading initializes member counters and bounds short skill blobs. Evidence and receipts in work/reward_next, work/guild_enter_game and work/db_dispatch_next.
Source batch 181 restored and wired Entergame (client 0x0a), including placement, world phase, saved subsystem registration, original time/channel/teen packets, premium and version handling, and arena/OX/instance routing. Marriage login links partners and starts the original proximity timer. War-map team, state and score packet helpers are restored; instance attachment still needs IncMember, DecMember and SetWarMap bodies. Handshake mark routing and four text commands are restored; admin auth and remaining commands are pending. Recall fallback retains the current position on a missing map record, marriage sends guard missing descriptors, and HackLog escapes SQL fields. Evidence and receipts in work/npc_positions, work/enter_game, work/marriage_login, work/db_dispatch_next and work/war_map_membership. Builds and tests deferred.
Batch 181 also restored original forked-war locale table loading and map, Sungzi, boss, regen and per-empire kill helpers in the existing questlua_forked source. Castle death and full Dead integration remain open. Evidence and receipt in work/reward_next.
Source batch 182 restored war-map membership and SetWarMap, flag spawning, timeout/scoring and DB war-end requests. War map creation and ownership still remain. Descriptor manager now sweeps closed descriptors, retains retrying connectors and destroys inbound peers before outgoing links. Inbound P2P socket acceptance and DESC_P2P setup/phase/teardown are restored. P2P setup only takes ownership of the socket after both buffers allocate, to avoid double closing on failure. Evidence and receipts in work/war_map_membership and work/desc_lifecycle. Builds and tests deferred.
Batch 182 also wired normal Login3 authentication through original 0x6f, DB query case 2, and LoginPrepare. The recovered path validates account state and restores premium fields, date block and auth Pong. MFA answer handling, Japan Netmarble and billing continuation remain pending. Japan/billing branches require those handlers before allowing login. Evidence and receipt in work/db_dispatch_next.
Batch 182 also restored inbound client DESC::Setup, original CRC handshake generation and 60-second ping timer, including locale keys, initial phase, and time sync. Client socket ownership starts after successful buffer allocation. AcceptDesc still needs the original IP ban and admin-list policies before wiring. Three-way war death scoring and outcome helper is restored, pending invocation from Dead. Evidence and receipts in work/desc_lifecycle and work/reward_next.
Batch 182 added war map manager startup metadata for normal and flag maps, with original default and CIBN coordinates. The startup call runs immediately after config_init pending reconstruction of that function. Metadata is owned by the manager and duplicate registration is safe. Active map creation and destruction remain open. Evidence in work/war_map_membership.
Source batch 183 restored normal inbound client admission, original 4096-socket limit and admin exemption, auth-only BANIP, and the original first-octet/XOR-mask match. The BANIP loader skips malformed/oversize rows and closes its input file. It runs after config_init until the original AUTH_SERVER config branch is restored. Both Matrix MFA responses and DB login continuation are restored, with typed, consumed challenges. Active war private map creation, timed exit, flag bases and shutdown draw are restored; guild DB start/end and shutdown call sites remain pending. Evidence and receipts in work/accept_client, work/db_dispatch_next and work/war_map_membership. Builds and tests deferred.
Source batch 184 restored quest manager initialization with the original 20 event IDs, NPC script index, notarget script, and locale guild-delay flags. InitializeLua is declared and its 2009 evidence is saved, but the VM and registration bodies remain open. NPC loader guards empty names. Castle boot, resident coordinate tables, persistence, and frog/guard death handlers were restored. Tower/siege lifecycle and full Dead wiring remain open. Receipts in work/quest_startup and work/reward_next. Builds and tests deferred.
Batch 184 also restored guild DB score packet 0x62 and war score broadcast, Passpod response and login continuation, and quest VM file loading (settings, quest library, locale, state scan, reverse state index and code cache). The VM loader still needs the Lua initialization/registration caller. Passpod requires an explicitly configured trusted IPv4 host via MCNCC_PASSPOD_HOST, with no default network destination. No network calls performed. Evidence and receipts in work/db_dispatch_next, work/war_map_membership and work/quest_startup.
Batch 184 also restored castle tower death and siege lifecycle, timed rewards, two stone waves, Japan loser warp and P2P siege notices. Siege shutdown cancels both timers and spawning guards failed allocations. Tower death is ready for Dead integration. Evidence and receipt in work/reward_next.
Source batch 185 wired Lua VM startup, original libraries, building/quest/target/global registration, all 14 q bindings, and recovered lookup, time and start-position globals. Remaining VM tables and globals still need recovery. Guild DB war lifecycle opcode 0x61 now dispatches original states 1–9, including private maps, quest letters and notices; private indices normalize by 10000. Guild field/instance kill scoring and flag drop are restored and ready for Dead. Evidence and receipts in work/quest_vm, work/db_dispatch_next, work/war_map_membership and work/guild_kill. Builds and tests deferred.
Batch 185 also restored full CHARACTER::Dead with verified combat reward, arena, guild war, castle and three-way hooks, player penalties, notifications, and scheduled cleanup, plus CloseMyShop. Affect resend guards a missing descriptor, and the shop sign payload is initialized. Evidence and receipt in work/reward_next. Builds and tests deferred.
Source batch 186 restored player, named and server Lua timers, callbacks, server registration and cancel paths. Duplicate server timers are canceled and callbacks handle cancellation from within the running Lua handler. Also integrated all eight game and 17 NPC Lua bindings, plus ten original guild Lua bindings with ranking and war-entry dependencies. Five guild bindings and other VM tables remain open. Evidence and receipts in work/quest_timers, work/db_dispatch_next and work/questlua_guild_source. Builds and tests deferred.
Batch 186 also restored and registered all 17 original horse bindings and the rider accessor. Horse HP/stamina percentage paths guard zero denominators, and character-specific bindings guard missing quest context. Evidence and receipt in work/reward_next.
Batch 186 also restored guild get_warp_war_list for active non-field wars. Four guild bindings remain. Evidence and receipt in work/questlua_guild_source.
Source batch 187 restored and registered all nine original affect Lua bindings, both guild ranking string bindings, and original BGM, OX quiz, PC/NPC lookup and quest-state globals. OX quiz preserves the old top-argument result and guards empty calls and null question text. Missing quest-context lookup returns no match. Two guild bindings remain on reserved-war data. Evidence and receipts in work/quest_globals, work/db_dispatch_next and work/questlua_guild_source. Builds and tests deferred.
Source batch 188 finished all 15 guild Lua bindings, including reserved-war DB ingress and betting lookups, and wired all 18 item, 15 marriage, and eight OX bindings in original VM order. OX recovers quiz loading, notices, timer stages, effects, elimination/warps, and rewards; it guards invalid quiz indices, owns one active quiz timer, and preserves malformed-call stack behavior where possible. Marriage/wedding DB reply lifecycle remains open. Evidence and receipts in work/questlua_guild_source, work/db_dispatch_next, work/reward_next and work/quest_ox. Builds and tests deferred.
Source batch 189 restored original Lua set_state and set_quest_state, P2P/global notice, map notice and script packets, map regen, and map kill bindings. Added guards for absent strings/context and script payloads beyond the 16-bit wire size. Evidence and source receipt in work/quest_map_globals. Builds and tests deferred.
Batch 189 also registered the original direct warp_to_village binding with a missing-context guard. Delayed bulk warp callbacks remain open.
Source batch 190 restored marriage/wedding DB replies 0x96–0x9c with packed payload validation and record cleanup, and five delayed quest bulk warp globals with map, time and empire filters. Bulk warps share an event callback and guard invalid empire indices and absent quest context. Evidence and source scripts in work/reward_next and work/quest_map_globals. Builds and tests deferred.
Batch 190 also restored and registered all 12 original party and five arena Lua bindings. Party restores proximity update and experience tables; arena restores map/duel creation, timeout, observer admission and teardown. The old arena stack return and duplicate timeout packet are retained. Evidence and receipts in work/db_dispatch_next and work/questlua_arena_source.
Source batch 191 restored and registered all 28 original forked/three-way-war Lua bindings, plus the original print, under_han, guild lookup and local/P2P chat-block globals. The libthecore Hangul sources and KStbl layout follow the reference; all 2350 raw table entries matched the 2009 ELF. Hangul functions bound malformed input and the Lua chat-block reply guards missing context. Evidence and scripts in work/reward_next and work/quest_misc_globals. Builds and tests deferred.
Batch 191 also restored and registered 61 of 134 original pc Lua entries, covering binary-backed getters and aliases for character stats, inventory/equipment, map, guild, dungeon, skill, mount, marriage, GM and premium. Remaining 73 original entries are open. Evidence and receipts in work/questlua_pc_source.
Source batch 192 restored and registered 22 of 45 original dungeon Lua bindings and their server leaves. Also restored find_pc_cond, add_goto_info, named goto routing, map base lookup and command argument parsing. Goto alias matching bounds each name segment, input is bounded, and the original command emits the syntax message on successful named warp (confirmed branch at 080be740). The command dispatcher still needs restoration. Evidence and receipts in work/quest_goto_find and work/db_dispatch_next. Builds and tests deferred.
Batch 192 also integrated 22 original PC Lua mutator names, bringing the table to 83 of 134. The batch adds quest flags, gold/SP/alignment, appearance and skill group, mount/polymorph, exit, and horse revive. Evidence and receipts in work/questlua_pc_source.
Source batch 193 restored the 2009 command dispatcher metadata/order and position/locale/GM gates, wired Lua command(), and connected recovered goto and event flag commands. Remaining command handlers and client chat dispatch are open; recognized missing handlers receive the original unknown-command reply. Command input is bounded and excluded monarch rows are omitted. Also restored all seven privilege Lua globals and DB request/reply storage, plus 13 additional dungeon bindings (35 of 45). Evidence and receipts in work/command_dispatch, work/reward_next and work/db_dispatch_next. Builds and tests deferred.
Batch 193 also added 11 original PC travel and selection bindings, bringing the table to 94 of 134. The goto dispatcher reuses the existing event flag listing implementation. Evidence and receipts in work/questlua_pc_source.
Source batch 194 restored client slash command routing, chat censorship/empire conversion, party/guild/shout routes, and partial game/dead input dispatch. Character manager update restores five-second chat resets, state pulse handling and hourly kill logs. Chat bounds the payload and waits for the trailing sequence byte. Added all 10 remaining dungeon bindings (45/45), 10 PC reward/inventory entries (104/134), native rod/pick refinement and fishing data initialization/selectors. Other game packet routes and fishing action gameplay remain open. Evidence and receipts in work/client_chat, work/db_dispatch_next, work/questlua_pc_source and work/reward_next. Builds and tests deferred.
Batch 194 wired both rod/pick Lua refinement globals. They preserve the original item when replacement allocation fails and validate ownership. Fishing startup bounds the original final out-of-record used-result write. Evidence in work/reward_next.
Batch 194 also restored 12 PC skill, scroll, reward and teleport bindings (116 of 134). Original map-name lookup and teleport return quirks are retained. Evidence and receipts in work/questlua_pc_source.
Source batch 195 wired original client quickslot add/delete/swap, sit/stand, warp completion and quest answer, input, confirmation, button/info and item selection. Restored NPC quest button/info lookup and execution. Added DB ItemLoad 0x2a with original 52-byte rows, equipment and unique expiry; payload bounds and failed-equipment cleanup are guarded. Also restored six PC Lua bindings (122/134) for stat reset, guild leave/disband, lotto and polymorph books. Evidence and receipts in work/client_quest_routes, work/db_dispatch_next and work/questlua_pc_source. Builds and tests deferred.
Source batch 196 restored original client whisper in game and dead phases, local/peer relay and empire conversion, block replies, and mute checks. Whisper validates the declared size and waits for the sequence byte, bounds the target and message, and sets relay only before sending. Restored fishing cast, bite and reel gameplay through opcode 82, plus three PC empire bindings (125/134). Fishing requires a baited rod until the item-use path is restored. Evidence and receipts in work/client_whisper, work/reward_next and work/questlua_pc_source. Builds and tests deferred.
Batch 196 also restored client position synchronization and target selection, including building VID lookup. Sync validates lengths and retains complete sequence framing. Building target replies initialize the original unused HP byte. Added original PC name change and DB cache invalidation (126/134). Evidence in work/client_whisper and work/questlua_pc_source.
Batch 196 also connected original inventory drop, counted drop and pickup routes, including stack/socket matching, gold and party pickup. Placement failure rolls back the item and pickup checks map/ownership before fallback. Inventory move remains open. Evidence and receipt in work/db_dispatch_next.
Source batch 197 restored client movement opcode 0x07, original 24-byte movement broadcast, stamina stop, speed timer handling and skill-motion authorization with the complete binary tables. Inventory move opcode 0x0d now handles inventory relocation, splits/merges and wear-to-inventory; wear destinations still need original equip/swap checks. Added three PC monster attraction bindings (129/134). Evidence and receipts in work/client_move, work/db_dispatch_next and work/questlua_pc_source. Builds and tests deferred.
Batch 197 also restored fishing item use/give routes for rod equip/unequip, bait, raw fish, campfire and grilling. General item use/give and occupied-slot swaps remain open. Evidence and receipt in work/reward_next.
Source batch 198 restored the original client attack request, CHARACTER::Attack control flow and battle melee/hit dispatch, including castle gating, range, timed event interruption and weapon resistance. Attack still requires the pending speed/affect, ranged/skill and Damage bodies. Restored inventory equipment destination, swaps, equip/unequip restrictions and rollback. Evidence and receipts in work/combat_attack and work/db_dispatch_next. Builds and tests deferred.
Source batch 199 restored aggro accumulation and target switching, party move/fight/aggro messages, cached party leader access, premium-aware user damage rates, and the original normal attack effects and attack-speed timing. Animation speed loader and original config defaults restored. Damage and ranged/skill execution are still in recovery. Evidence and receipts in work/combat_aggro and work/reward_next. Builds and tests deferred.

Source batch 200 restored CHARACTER::Damage, ranged attack packets and Shoot, FlyTarget and arrow use, plus quest-manager click dispatch, NPC click events and chat menus. Damage excludes Monarch and guards null diagnostics, zero maximum HP and orphaned WAEGU drops. Four PC Lua bindings for equipment refine and mining are live (133/134). The final grandmaster binding depends on skill leveling. Evidence, source receipts and rollback copies are in work/db_dispatch_next, work/reward_next, work/combat_attack and work/questlua_pc_source. Character/client click entry points and ComputeSkill are in recovery. Builds and tests remain deferred.
Batch 200 also connected client opcode 0x1a to CHARACTER::OnClick, private-shop guest handling, quest click dispatch and NPC trigger fallback. The shop owner assignment follows the original unconditional store after AddGuest. Original click evidence and rollback copies are in work/combat_attack and work/reward_next. Saved both UseSkill decompiles in work/combat_attack for the next skill batch.
Batch 200 completed the 134-entry original PC Lua binding table by restoring grandmaster book learning, shared SkillLevelUp, skill read timers and the three original book tables. Evidence and source guards are in work/questlua_pc_source. Compilation and runtime validation remain deferred.

Source batch 201 restored ComputeSkill, ComputeSkillAtPosition, UseSkill and ComputeCooltime, plus client opcode 0x34. Restored the original outer UseItem validation and quest use events, moved existing fishing handlers under UseItemEx and connected original skill and forget books (types 17 and 22). Other UseItemEx branches are still open. UseItem rechecks ownership after quest scripts and captures the item ID before destruction for logging. Skill-book counters guard malformed indices, and position-targeted skill diagnostics use defined varargs. Evidence and rollback files are in work/combat_attack, work/db_dispatch_next, work/reward_next and work/questlua_pc_source. Builds and tests remain deferred.
Batch 201 also recovered the UseItemEx level and CIBN seed preamble, ordinary quest items with arena restrictions, the original no-op acknowledgment types and equipment toggle branches, replacing the earlier rod-only approximation. Unique subtypes 7 and 10 remain open. Full 2009 UseItemEx decompile and item-use source guards are in work/combat_attack; equipment receipt is in work/db_dispatch_next.

Source batch 202 recovered standard, immediate and continuous potion use, including arena restrictions, seed timing and dungeon and guild-war accounting. Restored special skill books, polymorph items and practice, hairstyle changes, blend consumables and Metin socketing. Client opcode 0x3c now passes the destination cell to the restored item-use entry point. Blend use bounds-checks the apply index before accessing the original table. Evidence and source rollback files are in work/combat_attack, work/questlua_pc_source, work/reward_next and work/db_dispatch_next. Remaining branches include additional item affects, sockets, unique items and special consumables. Builds and tests remain deferred.

Source batch 203 recovered timed abilities, affects, random stones, PC-bang restrictions, unique-item effects and Silk Botary shop-price DB exchange. Restored gift boxes, locked treasure boxes and keys, Metin detachment, recall scrolls and confirmed special consumable vnums for rewards, stat/aggro reset, polymorph removal and VCards. Unique abilities reuse the same verified effect table. Recall scroll creation checks allocation and placement before decrementing a stack. Evidence and source guards are under work/combat_attack, work/reward_next, work/db_dispatch_next and work/questlua_pc_source. Refinement preview/confirmation, attribute items and recipes are in recovery. Builds and tests remain deferred.
Saved the complete original CInputMain::MyShop and CHARACTER::OpenMyShop decompiles in work/combat_attack/my-shop203-0.json and my-shop203-1.json for the player-shop listing packet and creation path.

Source batch 204 restored private shop creation and client opcode 0x37, including listing checks, shop permit consumption, price-list update and sign broadcast. Packet parsing waits for the sequence byte and bounds the shop sign; the gold sum is widened and zero-count price division is guarded. Recipe crafting, cleanse and invisibility/mirror cooldown are live. Normal refinement gameplay and the preview helpers are in source; activating client confirmation still requires scroll gameplay. Evidence and source guards are in work/combat_attack, work/questlua_pc_source and work/reward_next. Builds and tests remain deferred.
Batch 204 also restored socket cleaning, attribute rerolls and additions, accessory drilling and insertion, and CanPutInto. Evidence and source guards are in work/db_dispatch_next. Subtype 24 acknowledges valid destinations without changing them, matching the original jump table.

Source batch 205 connected original client shop buy, sell and end handling (0x32), safebox checkin and checkout, mall checkout and safebox move. Variable shop requests wait for the sequence byte. Storage transfers restore the original slot if placement fails and only log successful transfers. Recovered 29 additional special consumable IDs for restoration, effects, fireworks, holiday buffs, shop window, alignment, hair dye and rare attributes. Evidence and guards are in work/combat_attack and work/questlua_pc_source. Scroll refinement and remaining special item IDs are in progress. Builds and tests remain deferred.
Batch 205 also restored scroll refinement gameplay and enabled subtype 2 and client confirmation opcode 0x60. Original locale-specific odds, downgrade/destruction and fee ordering are retained. Malformed overlapping recipes and replacement grid mismatches are rejected. Evidence and source guards are in work/reward_next.
Batch 205 also restored the shared original ITEM_USE arena and additional-effect gates, known success/no-op subtypes and unknown outer type diagnostics. Live scroll subtype 2 and confirmation remain connected. Evidence in work/db_dispatch_next.

Source batch 206 connected the original client hack report, party removal and party parameter packets. Restored clam, Moonlight reward, horse skill book and four lucky bags. The supplied rebuilt DB tree is under db; checked the shop-price update/request/response IDs (108/109/157), five-byte owner/count header and eight-byte entries against DB ClientManager and DBProtocol. Game shop creation and reply parsing now respect the DB's 40-entry cap. Hack reports pass raw text to the existing escaping DB logger and preserve escaped diagnostics. Remaining branches include party invitation/roles/skills, Messenger, guild client dispatch and the final treasure map/marriage items. Evidence is in work/combat_attack, work/db_dispatch_next and work/questlua_pc_source. Builds and tests remain deferred.

Source batch 207 restored client guild founding (0x51), cooldowns, the original guild/grade inserts, guild-name broadcast, and outgoing DB creation/skill/member packets (28/19/21), cross-checked against rebuilt DB. Names are escaped in direct SQL and missing insert results abort creation. The game-side DB membership reply is still in recovery. Restored party invites and their 10-second timeout, client Messenger and friend authorization, both treasure maps and marriage warp. The invite timer allocates both IDs and the lotto payload initializes the extra field read by the existing callback. Monarch remains excluded. Evidence and source guards are in work/combat_attack, work/db_dispatch_next, work/reward_next and work/questlua_pc_source. Builds and tests remain deferred.

Source batch 208 restored game-side guild load (100), skill update/power/persistence (56), and skill-availability (102) DB packets with guarded framing, cross-checked against the rebuilt DB. Client guild opcode 0x50 is connected with verified framing for subcommands 0–14. Its member removal, Japan-only deposit and withdrawal request behavior are live; remaining subcommand bodies are still in recovery. Evidence and rollback files are in work/combat_attack and work/building_next. Guild experience/recharge and DB membership reply remain open. Builds and tests remain deferred.
Batch 208 also connected guild member-add DB reply 0x5c and the original roster/link and local/peer login updates. Party role/state and skill request packets 0x4b/0x4c now route into original quota, summon and state behavior. The original HealParty body is empty. Party DB reply 0x3f and its P2P party lifecycle remain open. Evidence and rollback copies are in work/reward_next and work/db_dispatch_next.

Source batch 209 restored guild comment add/delete/refresh and exact 5+80*n response framing. Restored guild experience level-up and save, both region experience tables extracted from the ELF, power refill, ladder update request and DB replies 57/58. The original guild SkillRecharge callback is empty. Invalid stored guild levels are bounded before indexing the tables. Evidence and rollback files are in work/combat_attack and work/building_next. Builds and tests remain deferred.
Batch 209 also restored inbound cross-channel party creation, deletion, join/quit, roles and member levels through DB packets 0x3b–0x3f and 0x41. Evidence and guards are in work/db_dispatch_next.

Source batch 210 restored guild member removal and disband replies, client grade and general-role changes and DB grade relay, guild ladder and war records, skill level-up and its DB serializer, and guild bank balance/withdrawal replies (0x67/0x68). Checked ladder and withdrawal wire shapes against the rebuilt DB. Disband-safe load callbacks resolve guilds by ID, and the skill serializer initializes its save byte. Evidence and rollback files are in work/combat_attack, work/building_next, work/reward_next and work/db_dispatch_next. Remaining guild branches include client invitation, EXP offer, skill use, SP charge and DB member-data relay. Builds and tests remain deferred.

Source batch 211 restored original skillup/gskillup/pvp commands in cmd_general.cpp, guild invite/answer, member-data DB relay, EXP offering, guild skill gameplay, and SP charge. All original client guild 0x50 subcommands 0–14 now have source handlers. The guild skill path bounds-checks skill slots and handles stale remote targets; EXP offerings guard signed bounds. Evidence and rollback files are in work/combat_attack, work/building_next, work/reward_next and work/db_dispatch_next. Builds and tests remain deferred.

Source batch 212 restored SMS client routing and game-to-DB submission/notification records, stat/stat- commands, safebox close/load/password change, mall open/close and private shop close. Mall distance and timing match the ELF. Storage DB request widths were checked against the rebuilt DB. Evidence and rollback files are in work/combat_attack, work/building_next and work/reward_next. No SMS was sent or queued. Original opcode inventory confirms the main, login and auth switches have routes; dead-phase Hack/SMS and startup CONFIG recovery are still in progress. Builds and tests remain deferred.
Batch 212 also restored dead-phase Hack and SMS and the original missing-character diagnostic. All 52 original nondefault main-phase opcodes are routed in source; the audit checks dispatch coverage, not end-to-end behavior. Source audit and guards in work/reward_next.

Source batch 213 restored riding startup, horse ride/return/feed, original empty mount command and unmount handling, plus restart-here/town with arena, dungeon, guild war and three-way war paths. Horse food text handles missing item prototypes. Added CONFIG startup foundations (network globals, IPv4 discovery, user-count state and account DB startup methods); full config_init is still in recovery. Source evidence and guards are in work/combat_attack, work/building_next and work/db_dispatch_next. Builds and tests remain deferred.
Batch 213 also restored walk/run, PK mode and block settings with six original quest flags. Source evidence and guards are in work/building_next.

Source batch 214 restored the startup cube.txt loader in original cube.cpp/cube.h, including section order, NPC/material/reward records, percent and validation. Malformed orphan/unfinished sections are discarded safely. Cube crafting commands and UI still need recovery. Restored DB affect loading (0x32) with owned deferred records and descriptor checks, and the original timed logout/quit/phase-select countdown and cancellation. Source evidence and guards are in work/combat_attack, work/reward_next and work/building_next. Builds and tests remain deferred.
Batch 214 also restored DB admin reload reply 0x9e. It validates both tables before replacement and refreshes all online characters so removed admins lose cached privileges. The original GM reload command remains unbound. Evidence in work/reward_next.

Source batch 215 restored two-pass startup CONFIG parsing, locale and skill-power setup, command privileges and original startup ordering. Startup logs omit passwords and Hong Kong auth uses configured MATRIX_SQL in place of the original embedded credentials. Restored Cube UI/crafting and logging, including invalidating inventory references on removal, checking combined material counts and guarding failed reward delivery. Added DB billing repair/login replies and ungroup, observer exit and equipment inspection commands. Evidence and source guards are in work/combat_attack, work/building_next, work/reward_next and work/db_dispatch_next. Builds and tests remain deferred.

Source batch 216 restored GM reload commands and quest reload, graceful character shutdown, memory usage reporting, party request/accept/deny and DB VCard redemption. Quest reload defers while a quest coroutine is active, avoiding invalidating suspended Lua state. VCard uses a conditional atomic update to prevent duplicate credit; a concurrent balance change can reject redemption. Visual Studio project and filter entries now match all 126 current game/src C++ files. Evidence and source guards are in work/combat_attack, work/db_dispatch_next, work/building_next and work/reward_next. The game-side DB prototype reload reply is still in recovery. Builds and tests remain deferred.

Source batch 217 restored all 26 original emotion command rows with binary table flags, target consent/distance and nine-byte chat framing, plus character-in-view lookup. Restored guild war declaration/refusal commands, money-table startup lifecycle and optional connection globals, and DB proto reload (0x78) with validation of all four table arrays before mutation. Existing motion rebuild may retain stale records on repeat reload; auditing original semantics next. The Visual Studio project and filters each match all 127 game C++ files. Evidence and rollback files are in work/combat_attack, work/building_next, work/db_dispatch_next and work/reward_next. Builds and tests remain deferred.

Source batch 218 restored original Teen phase 12 and its handshake, notices, disconnect and login-time messages, with guarded stream parsing. Restored mobile registration/auth and the hair affect status command with 72 original point formats. Mobile registration code was restored without sending or queuing any messages. The motion audit confirmed original first-record retention on reload; restored its walk/run logging precision without changing ownership behavior. Evidence and guards are in work/db_dispatch_next, work/combat_attack and work/reward_next. Builds and tests remain deferred.

Source batch 219 restored GM notices, user counts, disconnect, transfer and shutdown countdown, plus all nine original OX event commands. Restored optional Netmarble phase 11 and its JP account lookup/create continuation, with zeroed padding, duplicate login rejection and bounded reply handling. No server was started or external connection made. Visual Studio source and filter parity rechecked. Evidence and rollback files are in work/combat_attack, work/building_next and work/db_dispatch_next. Builds and tests remain deferred.

Source batch 220 restored GM warp (including ten original town coordinates), item creation, level reset, kill, weapon attribute/socket commands, ordinary guild-land build/delete, and billing timer SQL result QID 8. Warp guards coordinate overflow, item creation checks placement, and building totals duplicate material requirements before spending. Evidence and rollback files are in work/combat_attack, work/building_next and work/db_dispatch_next. GM walls/reset and roulette are still in recovery. Builds and tests remain deferred.
Batch 220 also restored standalone roulette.cpp/.h and original no-op GM command. The module stays dormant as in the original. Invalid prize draws and repeated loads are guarded. The DWARF source-unit audit and receipt are in work/reward_next.

Source batch 221 restored GM mob/group/random-group spawning, both wall construction/deletion styles and building reset, and DB-string/greeting result QID 1. Mob spawning preserves original random bounds and draw order and rejects negative iteration counts. Non-GM reset is constrained to the guild plot already checked by the outer command. Evidence and rollback files are in work/combat_attack, work/building_next and work/db_dispatch_next. Builds and tests remain deferred.

Source batch 222 restored original aggressive/coward/map mob spawn, character and item purge, arena GM commands, safebox size query result QID 0 and Lua mob.spawn/spawn_group. The purge PC exclusion is confirmed by the original x86 type-byte comparison against 6. Spawn loops guard negative counts and arena rejects self-duels before cleanup. Evidence and rollback files are in work/combat_attack, work/building_next, work/db_dispatch_next and work/reward_next. Builds and tests remain deferred.

Source batch 223 restored GM user listing, player cache flush, subskill reset, four bulk stat commands and all six GM horse commands. Restored quest Lua member chat/readiness and guild-war cleanup when the DB connection closes. User listing is bounded, horse status guards missing horses, and logout clears the quest member context. Evidence and rollback files are in work/combat_attack, work/building_next, work/db_dispatch_next and work/reward_next. Highscore Lua and its async DB handlers are in progress. Builds and tests remain deferred.

Source batch 224 restored GM quest flag listing/set/delete and affect inspection/removal. Restored inline quest highscore.register/show and DB QID 4/5 result paths. Original QID 4 always replaces the score after a successful result regardless of requested ordering, and original QID 5 is an empty callback. SQL board text is escaped. Evidence and source guards are in work/combat_attack, work/db_dispatch_next and work/reward_next. Builds and tests remain deferred.
Batch 224 also connected the original GM itemlog command to the existing item-sale audit logger, whose ring indexing matches the binary. Evidence in work/building_next.

Source batch 225 restored GM skill level/group/point commands, cooldown disable, invisibility, mount test, polymorph and polymorph-item creation, plus original guild war list/end/cancel/status utilities. Invalid guild IDs are rejected for mutation commands and GM item placement checks rollback. Evidence and guards are in work/combat_attack and work/building_next. Builds and tests remain deferred.
Batch 225 also restored the character delayed-save heartbeat and movable/random position helpers in the sector manager. Evidence in work/db_dispatch_next. Core state-machine and entity visibility encoding are next gameplay gaps.
Batch 225 also confirmed exact registration parity for 68 inline quest globals and three logging globals and restored the find_pc_cond diagnostics for invalid arguments. Audit in work/reward_next.

Source batch 226 restored character visibility insert/remove and shop-sign, movement and walk-mode updates. DWARF confirmed the 35/54/5-byte record sizes and field offsets; the original reciprocal walk condition checks the viewer's walking flag or exhausted stamina. Also restored horse death/level synchronization, GM quest state tools and original mob AI flags, speed toggles and coward escape. Main AI state bodies are still in recovery. Evidence and rollback files are in work/combat_attack, work/reward_next, work/building_next and work/db_dispatch_next. Builds and tests remain deferred.

Source batch 227 restored horse follow/return and chase positioning, movement-state interpolation and stamina handling, mob skill lookup/cooldown and delayed splash events, and the original GM empire/guild privilege and chat-block commands. Individual splash events use separate queue keys and cancel on character removal. Zero-duration motion completes immediately, moderation duration parsing guards overflow, and the guild privilege command retains the original empty ADMIN_QUEST script. Evidence and source guards are in work/db_dispatch_next, work/combat_attack, and work/building_next. Normal idle/battle/flag AI states and character teardown remain under recovery. The rebuilt DB source at PROJECT_ROOT/Reversed Source/db is available for game-side interface comparisons. Builds and tests remain deferred.

Source batch 228 restored mob battle AI with victim selection, summon and party support, chase/leash logic, and mob skill/normal attack timing. Restored ClearTarget and ClearItem teardown prerequisites, pending the Destroy lifecycle, plus the original GM rod/pick refinement, max-pick and fishing simulation commands. The simulation caps iterations at one million. Rechecked UseMobSkill conversion in x86 instructions and corrected cooldown and splash coordinates to truncate toward zero. Evidence and source guards are in work/db_dispatch_next, work/reward_next, work/building_next and work/combat_attack. Idle and flag state recovery continues. Builds and tests remain deferred.

Source batch 229 restored monster and Metin stone idle handlers and nearest-victim selection. The StateIdle dispatcher still needs the NPC branch. Restored GM forgetme, aggregate, attract_ranger, pull_monster, observer and effect, including the original pull movement and sync sequence. Matched x86 truncation in the AI movement, horse follow, battle and pull conversions. Character Destroy is staged but held pending regen-record lifetime handling because quest clear_regen can free records while spawned characters are alive. The original DestroyCharacter manager was rechecked and does not erase delayed-save entries; normal Disconnect flushes them before removal. Evidence and guards are in work/db_dispatch_next, work/reward_next and work/building_next. Builds and tests remain deferred.

Source batch 230 restored the StateIdle dispatcher and NPC idle logic, including guard targeting, wandering and Santa map transfers. State updates verify that the character remains registered before writing the next tick after a callback, since horse expiry and local Santa warps can destroy the character. Restored the 2009 destructor and teardown order, with regen backlinks invalidated before dungeon/global records are freed. Existing test fixtures keep their own destructor via a fixture-only Makefile define. Also restored the GM weaken, stat_reset, state, stun, slow and Xmas event commands. Flag/base AI and forced teardown with pending saves remain to audit. Builds and tests remain deferred.

Source batch 231 restored guild-war flag and flag-base callbacks, carrier effects, capture scoring and timed flag reset. Verified flag, base and horse prototype FSM bindings against SetProto in the original. Removed the obsolete manager event-cancellation helper now that the destructor owns cleanup. Forced deletion drops stale delayed-save entries and suppresses teardown requeue as a lifetime safeguard. Normal disconnect retains its save path, and the original empty manager Destroy was confirmed in assembly. Restored GM console, guild creation/deletion, safebox page count, forced marriage breakup and password incomplete-command warnings. Removed a byte-equivalent duplicate do_inputall body. Next gameplay route is ItemGive. Builds and tests remain deferred.

Source batch 232 restored the four remaining named war-map methods found in the original ELF and reference comparison. The original begin and timeout setters assign their event pointers directly, and event creation uses those methods. Also restored GM reset, greset, advance, book, respawn and socketitem; the advance command bounds levels to 0 through 99. War-map decompilation and rollback files are in work/combat_attack, and the command stage is in work/building_next. ItemGive still routes through the partial fishing handler while the generic 2009 NPC item-give behavior is recovered. Builds and tests remain deferred.

Source batch 233 replaced the fishing-only ItemGive handler with the 2009 NPC handoff route. Recovered item handling and range checks, refine NPC behavior, tiered horse feed and revival, campfire grilling, and quest take-item event 16. The original seven source files were retained. Before/after hashes and rollback copies are in work/db_dispatch_next. A symbol scan identified the next candidate group in CInputMain and the four original CHARACTER_MANAGER map broadcast helpers. Builds and tests remain deferred.

Source batch 234 restored the original GM threeway_info, threeway_myinfo, siege and frog commands. Threeway status reads the existing event counters and counts descriptor-backed characters by empire. Siege uses the existing peer packet 25, and frog placement uses the first free castle resident slot. Source changes span the existing command, castle and forked quest files. No server actions were executed. The active CInputMain packet audit and map broadcasts remain under recovery. Builds and tests remain deferred.

Source batch 235 restored the CInputMain orphan-descriptor close, post-move client-version check and original ScriptButton and test-server chat diagnostics. The original and live dispatcher case sets matched; helper-only symbol differences generally map to inline code, and Roulette has no dispatcher case in the original. Restored the map-wide and empire-restricted three-way war script broadcasts in both death branches. Restored GM private-map warp and inventory inspection with bounded inventory output. Evidence and rollback are in work/db_dispatch_next and work/reward_next; GM staging is in work/building_next. Builds and tests remain deferred.

Source batch 236 restored the dormant CInputMain::Roulette source method at original 081025f0, including the original item-ID/gold check, item/cash branches and localized diagnostics. Packet dispatch still has no Roulette route, matching the original. Binary audit verified all 73 client and 24 peer registration records for ID, base size, name and sequencing, and original non-Monarch DB packet ID coverage. Case coverage is distinct from handler behavior, with DB boot/player load prioritized next. Evidence and rollback for Roulette are in work/combat_attack. Builds and tests remain deferred.

Source batch 237 restored the original GiveRandomSkillBook method and routed lucky bags 30093–30096 through it, retaining safeguards for incomplete skill tables. Restored GM weapon addon testing, detail/monster log toggles, PC Bang diagnostics and both original command aliases. Detail logging uses a CHARACTER member to keep the private flag encapsulated. DB behavior comparison remains underway. Builds and tests remain deferred.

Source batch 238 restored the original GM set command, including prefix-first field selection, additive point changes, gold logging and issuer-relative alignment behavior. Restored CParty::ForEachMemberPtr and wired HasReviverInParty through its returned functor, keeping the guard for unlinked party slots. The game/DB source audit found normal-path agreement for boot v5 table sizing and player, item, quest and affect load records. Login-success diagnostics and deeper character-select and boot checks remain under review. Builds and tests remain deferred.

Source batch 239 restored /saveati and the original map-attribute TGA export, including 32-bit BGRA palette, top-origin header, traversal order and trailing zero fill for missing sectors. Added a DB boot guard rejecting mismatched frame lengths and a 0x71a player-record size assertion. These are source checks beyond the original malformed-frame behavior; normal boot and player-load ordering matched the original and rebuilt DB producer. Restored out-of-line GetMobDamageRate in char_manager.cpp and confirmed premium user damage selection. GM table exclusions are the Monarch rows, legacy unauthenticated hardcoded DB control, and the intentional null-write command. No exporter, builds or tests were run.

Source batch 240 restored the original named CInputMain ItemDrop, ItemDrop2, ScriptAnswer and Warp entry points and connected their packet cases. Kept explicit wire reads and the observer gate, and matched the original ScriptAnswer log format and answer threshold. Quest confirmation and selection were compared and already match normal gameplay. Login random-key methods were identified as an active unresolved descriptor dependency and are being recovered. Binary exports and rollback copies for the input methods are in work/combat_attack. Builds and tests remain deferred.

Source batch 241 restored descriptor MakeRandomKey and GetRandomKey in desc_manager.cpp. Login success now has its original RNG and insert-only handle mapping implementation, and lookup writes only on a hit. Restored CLand and building CManager teardown and land-local object lookup methods. Building teardown unregisters global indices before deleting owned objects and runs explicitly while character and event managers are still available. Stages and evidence are in work/reward_next and work/building_next. Builds and tests remain deferred.

Source batch 242 restored the original named CInputMain item pickup, quickslot add/delete/swap and position methods. Their existing packet cases call the recovered methods with the same observer restriction, byte offsets and stand/sit semantics. Decompilation and backups are in work/combat_attack. Normal guild and marriage DB replies matched original field and call order on source inspection. The next descriptor gap is FindByCharacterName and its original GM callers. Builds and tests remain deferred.

Source batch 243 restored the original case-sensitive FindByCharacterName descriptor lookup and connected /state, /disconnect and /kill. Recovered named QuestInputString, QuestConfirm, ScriptButton and ScriptSelectItem methods in input_main.cpp and routed their packet cases through them. Confirm uses a normalized local value without mutating the incoming buffer, and selection diagnostics match the original signed formatting. The sector audit matched all 35 manager and six sector-map signatures. Descriptor matrix packet helper recovery is next. Evidence and rollback are in work/reward_next and work/combat_attack. Builds and tests remain deferred.

Source batch 244 restored the descriptor Runup quiz sender and routed the Hong Kong authentication challenge through it. It preserves the 10-byte packet and 8-byte quiz limit. Restored the original named Attack, Target and ItemMove handlers with observer checks and explicit wire reads. Those were the final three named CInputMain definitions missing from the original ELF symbol inventory. Building target responses retain zeroed health. No authentication requests, builds or tests were run. Evidence and rollback are in work/reward_next and work/combat_attack.

Source batch 245 restored original dormant CHARACTER::ResetSkill and ComputePassiveSkill in char_skill.cpp. Reset retains the 13 original subskill records and sends the recalculated skill state. Passive evaluation uses x86-confirmed truncation and adds a missing-prototype guard. Neither method had an observed runtime caller in the original and neither was newly wired. The symbol inventory identified active guild member grade/general persistence as a separate gap, with SaveMember recovery underway. Builds and tests remain deferred.

Source batch 246 restored CGuild::SaveMember and calls from guild member grade/general changes and experience donations, with original notification order and nonzero general-byte normalization. Recovered dormant CHARACTER motion encoding/sending, the 11-byte motion record, allowed-gold thresholds, siege NPC classification, and dungeon CountRealMonster. Dormant methods retain their original lack of live routes. Evidence and rollback are in work/db_dispatch_next, work/combat_attack and work/building_next. Builds and tests remain deferred.

Source batch 247 restored the original named CHARACTER party-join condition methods and routed requests and invites through them, preserving the distinction between the empire check at request/invite and mutable conditions at acceptance. Restored dormant CParty::IsNearLeader and SetExpCentralizeCharacter in party.cpp without adding runtime routes. A missing PID leaves the EXP target unchanged; a present unlinked member can clear it. Evidence and rollback are in work/combat_attack and work/building_next. Builds and tests remain deferred.
Batch 247 also restored the original named async guild P2P grade callback. It takes the grade from the SQL row and retains a guild-ID relookup before applying the result.

Source batch 248 restored the original named guild admission verifier, its ordered penalty/membership/capacity codes, and capacity diagnostics. Invite and accept use the shared verifier and keep existing recipient messages. Fixed the guild grade callback declaration to use the existing forward-declared SQL type. Recovered dormant CountEmptyInventory, which totals prototype sizes across 90 inventory cells. Builds and tests remain deferred.

Source batch 249 restored the original __OpenPrivateShop method name in char_item.cpp and its three observed callers (silk bundle request/reply and item 50200). Existing outfit check and localized response matched the binary. Restored dormant CParty::IsRole. Verified all five binary ForEachNearMember instantiations against the existing template, including by-value functor return and nonnull near-member filtering. Builds and tests remain deferred.
Batch 249 also restored the original forced guild comment refresh from insert, delete and manual refresh, retaining the deferred guild lookup and validating SQL row width.

Source batch 250 restored dormant CHARACTER::Say with the original 6-byte script header and bounded packet length. Restored dormant guild GetTotalLevel and AdvanceLevel, plus NotifyGuildMaster in the active war-declaration path. A DWARF inventory comparison covered 174 compilation units. Five original basenames are absent, including three Monarch units, minilzo.c and start_position.cpp. The latter two require owner/dependency review before source parity can be claimed. Builds and tests remain deferred.

Source batch 251 restored the original STeamData initializer and routed both war-map teams through it. Restored the original OnKill diagnostic metadata and the dormant CInputLogin::MatrixAnswer no-op. Confirmed that the binary and local reference both identify miniLZO as version 1.08 dated Jul 12 2002, and found no miniLZO source or link dependency in the game project. miniLZO and start_position source-owner stages are in progress. Builds and tests remain deferred.
Batch 251 also restored miniLZO 1.08 from the version-matched local reference and registered it in the Visual Studio project and filters. The wrapper uses the library ABI macros for initialization and work memory. No build was run.

Source batch 252 restored ten original named CInputDB entry points for building object and land changes (0x8c–0x8e) and marriage/wedding events (0x96–0x9c). The original packet IDs now route through those methods while retaining explicit wire reads, malformed-frame rejection and subsystem calls. Source guards and rollback files are in work/db_dispatch_next. start_position.cpp ownership is still being staged. Builds and tests remain deferred.

Source batch 253 restored start_position.cpp and its original five globals from the ELF, moving existing users to the restored owner. Updated Visual Studio project and filters and login-focused Makefile source lists. All non-Monarch basenames in the 174-unit DWARF audit are now represented. Restored twelve named DB party and guild handlers with existing parsers and wire checks. Restored original Teen handshake and main method boundaries while retaining guarded partial/coalesced stream handling. Builds and tests remain deferred.

Source batch 254 restored original named DB handlers for safebox/mall loads, safebox size and password replies, empire/guild/character privileges, and money logging. Existing packet sizes, bounds, auth-server exclusion and downstream calls were preserved. Both Visual Studio XML source inventories register all 133 game C/C++ units once, but the MakeFileProj has no NMake build command, so this verifies IDE registration only. The focused Makefile contains the restored start_position source for all input_login link lists. Builds and tests remain deferred.

Source batch 255 restored named CInputDB Notice, BillingExpire, BillingCheck and MyshopPricelistRes entry points for packets 0x01, 0x81, 0x84 and 0x9d. The dispatcher uses them with existing bounded notice text, billing login termination, count validation and shop list bounds. Original paths and metadata were compared against the ELF; binary exports and rollback files are in work/combat_attack. Remaining guild/war wrappers are being staged. Builds and tests remain deferred.
Batch 255 also restored eight named guild DB routes for grades, war state and scoring, reservations and bets, and bank changes.

Source batch 256 restored named CInputDB handlers for time synchronization, country-IP blocks and block exceptions. Packet IDs 0x5a, 0xab and 0xac use the restored methods, preserving explicit wire reads, length checks and bounded login termination. Trigger and item-manager paths are being compared for original behavior gaps. Evidence and rollback are in work/combat_attack. Builds and tests remain deferred.
Batch 256 also returned the active mob victim finder to its original trigger.cpp owner and restored dormant idle hooks. The item drop audit found the original active special drops and lotto callback represented in source.

Source batch 257 restored the final five named non-Monarch CInputDB handlers from the ELF comparison: duplicate login, auth reply, event flags, map locations and peer setup. Restored the out-of-line destructor. A fresh ELF-to-live definition scan found all 75 non-Monarch named CInputDB methods, all 49 CInputMain methods, all 16 CInputLogin methods, and all six CInputTeen methods. The remaining 10 CInputDB names are Monarch-only. This verifies name presence, not full behavior or signatures. The game/DB payload audit and remaining item-manager source work continue. Builds and tests remain deferred.
Batch 257 also restored dormant file lottery helpers, keeping their original one-shot load, six-digit draw and append behavior. No callers were added.

Batch 258 audited suspicious empty source methods against the 2009 ELF. ComputeSkillPoints, DistributeHP, building CObject::Save, PvP Disconnect, party HealParty, Messenger Initialize/Destroy and Netmarble Analyze are intentional no-ops. CItem construction and dungeon/target manager destructors match base/container-only behavior. The game/DB producer sweep found no additional in-scope normal payload mismatch. It identified a separate DB-side oversized MyShop SQL row issue; a bounded producer patch was staged in work/db_dispatch_next/apply_db_pricelist_count_bound.py and left outside the game-source pass. Builds and tests remain deferred.

Source batch 259 restored original building DrawWall and FindObjectByVnum. The existing wall-block route uses the original quantized wall cursor. Restored five dormant utility functions for dice, decisions and filename extensions. A DWARF name scan against live source identified further named helpers, with castle, guild-war and quest Lua differences prioritized. Evidence and rollback are in work/building_next and work/combat_attack. Builds and tests remain deferred.
Batch 259 also restored the named guild-war map/type helpers and the original war-over announcement helper in their existing callers.

Source batch 260 restored original castle map/resident slot and count helpers, and original CHARACTER mob damage bounds, default mob size, leadership and guild-refine predicates. Existing melee damage, party, quest leadership and refinement routes use the recovered helpers. Restored dormant battle distance and CanFight definitions without new callers. Evidence and rollback are in work/building_next, work/db_dispatch_next and work/combat_attack. Builds and tests remain deferred.

Source batch 261 restored original argument splitting, whitespace skipping, linear interpolation and highest-array-element helpers. The active command tokenizer now uses the restored skip_spaces prefix/suffix rule. highest_array_element returns the last index on ties, matching the binary. Evidence and rollback are in work/combat_attack. Quest and privilege helper batches remain under recovery. Builds and tests remain deferred.
Batch 261 also restored the dormant empire, guild and character privilege removal methods. The active DB expiry path continues to send zero values through Give methods, as in the original.

Source batch 262 restored original Three-Way War spawn and kill-counter helpers and the dormant PID death-count and boss-reset helpers. Restored command, duration and gauge utilities; interpret_command now uses original first_cmd parsing. Gauge width and fill are bounded for the static buffer. The active chat-block command retains its existing overflow-checked duration parser. Builds and tests remain deferred.

Source batch 263 restored original marriage engagement predicates and wired marriage-add, Lua and empire-change callers. Existing proximity event timing matched the binary. Restored original ore conversion, fraction/yield, mining chance and pick practice helpers. Kept fixed level-zero chance input and two fraction RNG rolls verified in the original. No changes to active normal-input mining behavior were intended. Builds and tests remain deferred.

Source batch 264 restored four dormant original CItem helpers: item-use packet encoding, socket creation, polymorph classification, and prototype apply lookup. The item-use record is 14 packed bytes with x86-confirmed offsets. Invalid socket indexes are guarded in release builds. The dungeon cleanup, delayed jump and exit callbacks already match the 2009 names and scheduling. Builds and tests remain deferred.

Source batch 265 restored the original animation loading/diagnostic helpers and named load_one path, retaining existing attack timing behavior and subtype mapping. Recovered dormant HackCRCLog with original table/field layout and existing log-layer SQL escaping conventions. No callers were added to the CRC logger. Builds and tests remain deferred.

Source batch 266 restored original summon-item and map predicates in char_item.cpp and reused the zone predicate in both local and peer WarpToPID routes. The recognized items and blocked map IDs match the original and the prior normal-input behavior. Binary exports and rollback files are in work/combat_attack. File-loader and quest timer/event comparisons are in progress. Builds and tests remain deferred.

Source batch 267 restored the original CMemoryFileLoader API and owner files with bounded memory reads. The class had no external callers in the original. Restored named item socket-copy and item sex-check helpers on the existing split/drop and use routes, preserving all three per-slot updates and the race/anti-flag rule. Builds and tests remain deferred.

Source batch 268 restored the five named quest warp callbacks, their original payload layouts and diagnostics, plus separate named and unnamed timer entry points. Restored shared GET_SEX with original race/default behavior and routed item sex checks, gifts, emotion checks and Lua get_sex through it. Builds and tests remain deferred.

Source batch 269 restored original block-country exception add/delete helpers and diagnostic metadata. Recovered dormant NewPlayerTable including the level-35 China event branch, original RNG order and starting positions, with a guard for invalid empire values. Character creation continues through NewPlayerTable2 as observed in the binary. Save, stun and descriptor ping callbacks were compared and already match the original paths. Builds and tests remain deferred.
Batch 269 also restored is_string_true in the active mark_server configuration path. The original CONFIG key inventory is represented in the current parser.

Source batch 270 restored the original named CalculateConsume and CalculateConsumeSP in char_item.cpp. Recall and marriage-ring routes now share the original 30 percent requirement and HP floor behavior, retaining widened multiplication for valid extreme point values. Binary exports and rollback are in work/combat_attack. Cube and channel-service login comparisons are underway. Builds and tests remain deferred.

Source batch 271 restored the original Cube cleanup, recipe validation, reward selection and diagnostic helpers. Reward selection keeps both original random draws, and crafting retains the rebuild's ownership and duplicate-material guards. Restored original channel-service login branch and helper without changing the existing login validator, which rejects the bracket prefix and keeps it unreachable on the normal network path. Builds and tests remain deferred.

Source batch 272 restored dormant delayed item-warp and timewarp callbacks with original default countdown, event-flag handling, HP change and warp order. The 2009 ELF has no scheduler references to these callbacks, so no call route was added. Restored the duplicate-login peer packet's full 25-byte name field and bounded receiver reads to it. Map locations, peer registration, auth replies and quest login matched valid-input behavior. Builds and tests remain deferred.

Source batch 273 restored the original named party level-check helper in the active join predicate. It preserves the 15-level Canadian and 30-level default limits. Restored the original command dollar-escaping helper name while retaining the buffer-edge check that prevents writing a partial escape. Builds and tests remain deferred.

Source batch 274 restored original USER_COUNT web-admin IP checks, the unconditional five-second OX close callback, and the dormant fishing command helper without registering it. Also restored named character-stone drop and chance helpers in their existing item drop route, preserving original brackets and RNG ordering. No remote admin command access was added. Builds and tests remain deferred.

Source batch 275 restored original named mob approach and emotion eligibility helpers in their existing combat and command callers. Verified original 80/90 percent follow distances, test-server diagnostics, wedding-map bypass and two unique-item IDs. Exchange and quest Lua behavioral audits are in progress. Builds and tests remain deferred.

Source batch 276 restored the dormant PrintHexa locale diagnostic with the original 16-byte row layout, offset prefix, uppercase hex, legacy multibyte display ranges, and stderr output. No caller was added, matching original xrefs. Evidence and rollback are in work/combat_attack. Shop and quest Lua audits continue. Builds and tests remain deferred.

Source batch 277 restored 14 original C++ names behind active PC Lua bindings and five dormant PC helpers. Lua registration names are unchanged. Restored the named mob_skill_hit_event callback and its scheduler route. Retained the existing safe cleanup-before-compute order, which differs from the original erase-after-compute order. Builds and tests remain deferred.

Source batch 278 restored the original guild experience threshold helper and routed guild point changes through it, preserving the YMIR and other locale tables. Shop purchase, sale, and exchange flows matched original normal transaction behavior on source and binary comparison. Existing packet sequence bounds, listing size/gold guards, and item lifetime handling remain intentional differences. Builds and tests remain deferred.

Source batch 279 restored the original party_request_event and party_invite_event names and TPartyJoinEventInfo owner structure in char.cpp, retaining both ten-second scheduler routes and the existing stale-request guard. Compared guest/leader PID ordering, denial packets and diagnostics against 2009. Netmarble and matrix/quest evidence comparisons continue. Builds and tests remain deferred.

Source batch 280 restored the dormant skill_gwihwan_event with original random threshold, faction spawn fallback and diagnostics. Restored active Netmarble connector/result names and result order while keeping one outstanding request per login. Restored original matrix-card codec and coordinate helper names, unsigned column parameter and dormant encoder. Existing malformed-input checks and secret-free diagnostics are retained. No authentication traffic, builds or tests were run.

Source batch 281 restored the original named oxevent_timer callback and the quiz scheduler route. Retained the rebuild's per-event phase and clearing of the expired event pointer rather than the original static phase. Binary audit identified active missing GetGameTimeIP/GetGameTime billing helpers and a force-NOBILL fallback in live auth; mapping the original multi-result queries is next. Builds and tests remain deferred.

Source batch 282 restored the original billing GetGameTimeIP and GetGameTime result helpers with field validation, and confirmed existing libsql support for multi-result statements. Login/refresh query handlers 6/7/9 and QID 2 continuation are still being recovered, so billed login is not yet restored. Also restored the original GM affect helper and dormant guildlevel command body without registering a guild-level mutation. Builds and tests remain deferred.

Source batch 283 restored original game billing result dispatcher cases 6/7/9 for initial IP, initial account and repair. The handlers retain request ownership and require a live authenticated descriptor for initial login. The QID 2 query producer and QID 9 refresh producer still need integration, so normal billed login remains incomplete. Evidence and staging are in work/db_dispatch_next. Builds and tests remain deferred.

Source batch 284 restored the original char_battle.cpp ownership of GetProtege and GetNearestVictim and removed the second Cube_close definition. The canonical cube.cpp implementation is now listed as a dependency for the focused combat targets. Verified the already-integrated Runup quiz sender in desc.cpp, its declaration and the Hong Kong authentication call in db.cpp. Combat staging is in work/building_next and the Runup evidence is in work/reward_next. Billing query producers remain open. Builds and tests remain deferred.

Source batch 285 removed the duplicate CHARACTER::IsEquipUniqueItem definition in char.cpp. The single canonical char_item.cpp definition matches the original DWARF owner at 081808e0. Verified the live definition count after applying work/building_next/apply_unique_item_owner.py. The quest PC alias audit found the original registrations and active behavior present; the gold-bar audit retains its existing null guard. Billing producer and scroll-refinement helper recovery are still in progress. Builds and tests remain deferred.

Source batch 285 also restored CItem::GetRefineFromVnum in item.cpp and routed the scroll-refinement downgrade through the original method, verified at ELF addresses 08168370 and 081725fd. Guarded source backups are in work/reward_next. No builds or tests.

Source batch 286 restored the original out-of-line building::CObject::SetVID at 08106c10, building::CLand::GetData at 081076b0, and empty building::CManager::GracefulShutdown at 08108b00. Checked GetData call sites for mutable land pointers. Reviewed wall construction, load/finalize and guild effects against the binary. Staging is in work/building_next. Builds and tests remain deferred.

Source batch 287 restored COXEventManager::ClearQuiz out of line in OXEvent.cpp. The reference and 2009 ELF disassembly at 08087b40 agree that the helper clears each level before clearing the outer quiz list. Existing quest and GM reload callers use the recovered method. Builds and tests remain deferred.

Source batch 288 restored fixed-width wire fields for the original 15-byte client warp and 33-byte peer login records. WarpSet and WarpEnd now send the size of the packed record. Both sizes are asserted in packet.h. Source audit confirmed other peer login senders and readers use the shared declaration. The original start-position data and map-location behavior matched the current source. Stage in work/building_next. Builds and tests remain deferred.

Source batch 288 also restored the original PickupItem party fallback at 0817f27e. When no linked party member matches the item owner, the picker now gets the same inventory fallback used when the owner has no space. This can let a party picker collect a reserved drop in that case. Retained the existing map check and the original party-branch return value. Source hashes and backup in work/reward_next. Builds and tests deferred.

Batch 289 audited the original billing query producers. The complete ReturnQuery xref list contains no producers for QIDs 6, 7 or 9. Original successful QID 2 with billing enabled exits before LoginPrepare and does not enqueue billing SQL. CInputDB::BillingRepair at 080ebd80 also does not enqueue a query. The only GameTime and GameTimeIP SELECT literals found in the ELF are the QID 8 two-column queries at 082219c0 and 08221ae0. Do not infer a missing schema or fabricate producers. Current QID 2 still returns NOBILL; the exact original early-exit placement is being checked. Also confirmed CGrid::Print diagnostic bytes, signed cell loads and traversal against ELF 081f50d0. No builds or tests.

Source batch 290 restored the original billing-enabled QID 2 exit after account binding, ConnectAccount and matrix-code assignment. Valid credentials log success and leave the client without a login response, matching the 2009 binary. QID 8 matched the original on valid SQL results, including the 600-second rollover and expiry notification. Also restored packed 24-byte movement and 3-byte header plus 12-byte element position-sync wire layouts with fixed-width fields. Senders now use the actual packed sizes. Stages in work/db_dispatch_next and work/building_next. Builds and tests remain deferred.

Source batch 291 restored CArena::RemoveObserver out of line in arena.cpp. The original ELF at 080bd250 looks up the PID and erases the found iterator, leaving an absent PID alone. Verified the existing arena-exit caller resolves to the restored declaration. Builds and tests remain deferred.

Source batch 291 also restored original fixed-width client login3 keys and matrix challenge coordinates. The 65-byte login3 and 9-byte challenge records are asserted. DB result and Netmarble adapters now copy keys individually across the wire/native boundary. Shared game-to-DB auth packets remain a separate audit. Stage in work/building_next. No builds or tests.

Source batch 292 recovered the original PC Bang table-refresh diagnostics in game/src/db.cpp QID 11, and added result-shape and null-field guards in QIDs 11/12. Preserved the original behavior for valid results. Existing explicit MATRIX_SQL configuration remains in use for MatrixDBManager. Stage in work/db_dispatch_next. Builds and tests remain deferred.

Source batch 292 also restored the shared game-to-DB auth login record to the original 115 bytes, verified at SendAuthLogin 0810d920. Game and DB endpoints convert all four keys between the packed wire fields and their native storage. The existing DB length validation uses the shared record size. Stage in work/building_next. Builds and tests remain deferred.

Source batch 292 restored DropGold save and success after gold item creation, including placement failure, per original ELF 0817e2d0. Gold deduction still requires successful placement. Retained cleanup of the unplaced item as an intentional difference. Stage and rollback bytes in work/reward_next. Builds and tests remain deferred.

Source batch 293 restored seven original MatrixDBManager methods in matrix_for_runup.cpp: Query, IsConnected, EscapeString, ReturnQuery, PopResult, Process, and AnalyzeReturnQuery. The recovery reuses the shared SQL result and callback types. Original return IDs have no handlers and are logged and released. Retained explicit MATRIX_SQL configuration, excluding the original hardcoded connection credentials. Stage in work/db_dispatch_next. Builds and tests remain deferred.

Source batch 294 restored the original CItem::GetGold in item.cpp. Reference and ELF 08167ed0 agree on flag 0x08, count fallback at a zero price, and unsigned count-per-gold division. No direct calls were found in the original ELF, so existing shop pricing callers were left alone. Builds and tests remain deferred.

Source batch 294 restored 67-byte GD login-by-key and 9-byte GD player-load records from original senders 080f2570 and 080f29b0. Client key parsing uses the original four-byte fields and both endpoints handle their native key storage explicitly. Stage in work/building_next. DB callback ownership audit confirmed the original leaked callback holders; an optional lifetime fix is staged in work/db_dispatch_next and remains separate from source parity. Builds and tests remain deferred.

Source batch 294 also restored the cube recipe methods and original recipe-row then selected-slot material consumption order. Existing aggregate preflight stays in place. Fully consumed slots are cleared in the local selection before SetCount can destroy the item, so duplicate recipe rows cannot revisit a freed stack. Stage in work/reward_next. Builds and tests remain deferred.

Source batch 295 restored original character-delete (44 bytes), empire-select (5), game/DB rename (29), and client rename reply (30) layouts across the game and DB. Also restored the named MatrixDBManager::InitMatrix startup path for the existing configuration trigger. It takes explicit MATRIX_SQL settings in place of historical embedded credentials. Stages in work/building_next and work/db_dispatch_next. Builds and tests remain deferred.

Source batch 296 restored the original out-of-line ANI constructor, load, load_one, print_attack_speed and attack_speed definitions in ani.cpp. The target symbol table confirms all five owners and signatures. Existing parser bounds and table behavior are retained. Builds and tests remain deferred.

Source batch 297 restored the original first-match FN_find_cube helper at 080fc060 and Cube_delete_item clear-before-notification order at 080fd100. Existing item removal clears inventory-backed cube selections, and material consumption still clears exhausted slots before SetCount. The rebuild retains NPC identity and distance validation and sends success only after AutoGiveItem succeeds. Stage and rollback in work/reward_next. No builds or tests.

Source batch 298 restored the original 106-byte shared quest row and four-byte quest-load count for empty and nonempty payloads. Game and DB now share the record for load/save and the game keeps bounded parsing. Also restored the original GetServerLocation(TAccountTable&, BYTE) symbol at 080e7000 and shared start coordinates in character routing, empire select and warp fallback. Verified original map notice call sites are Monarch-only; the other empire notice helper has no code callers and remains unclassified for dormant recovery. Stages in work/building_next and work/db_dispatch_next. Builds and tests remain deferred.

Source batch 299 restored the original 48-byte client exchange wire record verified at exchange_packet 08064310. Packet arguments and sockets now use explicit four-byte fields. The sender encodes each native socket separately and initializes all unused fields. This prevents host-width truncation of the item attributes and socket data. Builds and tests remain deferred.

Source batch 300 restored fixed-width client inventory item update/set/set2 packets (36/40/44 bytes) and fixed native socket serialization in CHARACTER::SetItem and CSafebox::Add. Original CSafebox::Add at 08096e70 sends 40 bytes and copies 21 attribute bytes. This batch covers client inventory/safebox only. Builds and tests remain deferred.

Source batch 301 restored libgame CTargaImage and reconnected the GM map attribute exporter (080b7ce0) to it. The Visual Studio library and focused link inputs include targa.cc. The original whole-image buffer costs four bytes per pixel and preserves the missing-sector zero tail. Retained allocation and write error checks. Stage and rollback in work/reward_next. No builds or tests.

Source batch 302 restored shared item persistence wire shapes verified in SaveSingleItem, ItemLoad, SafeboxLoad and MallLoad. Uses 52-byte item rows, four-byte counts, 11-byte storage headers and eight-byte destroy payloads. Updated game/DB socket conversions and DB cache SQL formatting. Checked live helper names and SetSockets parameter type after integration. CGrid and CAttribute audited operations matched original behavior. Stage in work/building_next. Builds and tests remain deferred.

Source batch 303 restored the original fixed-width client ground-item add (21 bytes), remove (5), and ownership (30) records verified at 081665e0, 08166680, 08167f50 and 08167fd0. Ownership messages zero-fill unused name bytes. Legacy SQL ChildLoop was audited against the ELF and matched the queue, retry and shutdown behavior. Builds and tests remain deferred.

Source batch 304 restored fixed-width client point-change (17 bytes) and damage (10 bytes) packets. Original PointChange at 0812e340 sends 17 bytes on both direct and broadcast paths; original SendDamagePacket at 0814d8c0 sends 10 bytes to both recipients. The live senders use the packed record sizes. Legacy SQL direct, locale and multi-result behavior matched original. Builds and tests remain deferred.

Source batch 304 also restored the original shop packet layouts: 4-byte header, 42-byte listing item, 1684-byte start and 43-byte update payload, plus 11-byte shared listing and 449-byte DB boot record. Socket words are encoded individually and empty slots are zero-filled. Restored original Passpod Connect/Disconnect and diagnostics using the operator-configured host, with strict HTTP parsing and no raw authentication logging. Stages in work/building_next and work/reward_next. No builds, tests or network requests.

Source batch 305 corrected both shop socket loop bounds to derive from their packed destination arrays. The previous source used ITEM_SOCKET_MAX_NUM, declared only on the DB side. Verified MyShop opcode 55 is sequenced: CPacketInfo::Set adds one byte and CInputProcessor::Process reads data[length-1] after Analyze adds the listing body. Retained the MyShop trailing-byte guard to wait for a complete frame and prevent an out-of-bounds read. Builds and tests remain deferred.

Source batch 306 restored 21-byte affect rows, 22/6-byte client add/remove and 25/9-byte DB add/remove. DB affect loads now encode four-byte PID and count words. Client send sizes verified at 0813d2d0 and 0813d350; DB shapes match the original-format game parser. DB SQL formatting follows the fixed-width fields. Valid peer packet behavior audit found no verified divergence. Builds and tests deferred.

Source batch 307 restored original client party add/update/link/unlink/remove/parameter sizes (30/21/9/9/5/2 bytes) and the seven-byte guild symbol header. Both record definitions and sender lengths follow the ELF/DWARF. Stage in work/building_next. Builds and tests remain deferred.

Source batch 307 also restored original CGuild::UseSkill format arguments for insufficient power, wrong-channel teleport and test-server success feedback. Call stack arguments verified at 0808eb55, 0808edfd and 0808ef23. Existing skill effects and cooldown behavior preserved. Stage and rollback in work/reward_next. No builds or tests.

Source batch 308 restored the 10-byte CreateFly and 74-byte quest confirmation wire records with explicit 32-bit fields. Binary send sizes verified at 08147740 and 08136310. CreateFly uses sizeof so the entire target VID is sent on hosts with 64-bit long. Builds and tests remain deferred.

Source batch 308 also restored the 12 original game-side boot record-size diagnostics. The refine and banword records were cross-checked against original Boot at 53 and 25 bytes. Diagnostics use live structure sizes so any residual width difference remains visible. Stage in work/db_dispatch_next. Builds and tests remain deferred.

Source batch 308 also restored fixed-width shared party create/delete/add/remove/state/level records across game and DB, including DB replay and game frame checks. Stage in work/building_next. Builds and tests remain deferred.

Source batch 309 restored the original nine-byte GC chat header verified in CHARACTER::ChatPacket at 0812fe70. Shared senders now agree on the packed header size. ChatPacket and MonsterLog clamp formatted lengths to the actual destination capacity, preserving ordinary output while avoiding the original unchecked formatted length on oversized text. Rechecked live party DB record use after integration. Builds and tests remain deferred.

Source batch 310 restored fixed-width character update (35 bytes), combat target (6), and quest target create/update/delete (43/13/5) packets. Original spawn/additional/remove sizes already matched. Also restored both 13-byte handshake records and the 7-byte UDP grant, confirmed at 0813a6d0 and 0813a550 and by the client registration. Builds and tests remain deferred.

Source batch 311 restored the original 30-byte peer relay header. DESC::Packet at 0813a0b1 writes 30 bytes and the existing peer registry and receive path already expect the signed 32-bit length at offset 26. The sender now agrees with those boundaries on wide-long hosts. Builds and tests remain deferred.

Source batch 312 restored the original ten-byte client PvP record. The original CPVP::Packet and CPVPManager::SendList at 0819c1c0 and 0819dbc0 send ten bytes on all audited branches. Explicit VID widths let both existing sizeof senders emit the original frame. Login record assertions remain staged pending fixed-width fields for the account and login-success structures. Builds and tests remain deferred.

Source batch 313 restored five original shared guild DB records: skill update (22 bytes), EXP update (8), ladder delta (8), skill use (12), and skill availability (9). The game and DB endpoints now use the same fixed-width records. Retained the initialized zero save byte in game skill updates. Stage in work/building_next. Builds and tests remain deferred.

Source batch 314 restored the original account (333 bytes), account character (63), client login success (329), DB/client creation success (64/65), and logout (48) records. Game and DB adapters retain native map lookup and cast IDs at affected SQL formatting boundaries. Also restored nine-byte money logs and 37-byte event flags verified at 08110620 and 081a5df0. The game receivers already decode these original sizes. Builds and tests remain deferred.

Source batch 315 restored the original 13-byte marriage update and 12-byte wedding-ready shared wire layouts. Game senders now use packed record sizes and DB validation and forwarding use those same types. The existing game receive layouts already use the original fixed widths. Builds and tests remain deferred.

Source batch 315 also restored shared packed guild war state (18 bytes) and score (16) records. Game sends/receives and DB validation/forwarding now use these types, matching original RequestEndWar, RequestDeclareWar and SendGuildWarScore sizes. Stage in work/building_next. Builds and tests remain deferred.

Source batch 316 restored 255-byte mob prototypes and five-byte mob skill rows from original DWARF, updating affected DB and motion diagnostics. Also restored the original 42-byte safebox/mall load request verified at 08133670 and adjusted the DB SQL and diagnostic types. Both game senders already use the packed request size. Builds and tests remain deferred.

Source batch 317 aligned DB marriage-add (62 bytes), marriage-remove, wedding-request and wedding-end (eight bytes each) with the original fixed-width game records. Also restored the original unsigned-long arguments in guild ChargeSP and OfferExp feedback. Audited guild war and treasury behavior had no confirmed gameplay gap. Builds and tests remain deferred.

Source batch 318 restored shared game/DB guild membership records (8-byte guild pair, 9-byte add request, 14-byte update, 41-byte add response) and the four-byte guild create/load ID. Verified original RequestAddMember, RequestRemoveMember, disband and LevelChange lengths; unified game parsing and DB validation around the shared declarations. Guild observer warp and quest guild entry/disband/leave checks matched the original on reviewed paths. Builds and tests remain deferred.

Source batch 318 also restored the original 154-byte item prototype and five-byte limit/apply records from DWARF. Updated DB prototype and quest diagnostics at the affected call sites. Prototype socket fields and native item storage remain distinct. Builds and tests remain deferred.

Source batch 319 restored the original packed 69-byte item attribute and rare-attribute boot record. DWARF confirms the index and probability fields at offsets 33 and 37, five signed values at 41, and set limits at 61. DB boot diagnostics retain matching argument types. Builds and tests remain deferred.

Source batch 319 also restored the original 1475-byte skill boot record from DWARF and the Boot size check. Fixed seven unsigned fields and signed lMaxHit at their original four-byte widths. CSkillManager consumes the fields by value; DB boot logging uses matching argument types. Builds and tests remain deferred.

Source batch 320 restored the original VCard (116 bytes) and billing expiry (36 bytes) shared game/DB records. The game receivers already expect these layouts; the shared serializers and DB validation now agree with them on hosts with eight-byte long. Verified VCard DB diagnostics use the correct native logging type. Builds and tests remain deferred.

Source batch 321 restored the original 53-byte boot refine recipe and eight-byte material entry. Original Boot checks 0x35 and CRefineManager::Initialize copies 53 bytes at 0806a2c0. Both game and DB share the packed record, and the DB log argument types match. Builds and tests remain deferred.

Source batch 322 restored the original six DB privilege request and broadcast widths for empire, guild and character. Checked against fixed-width game request records and original receive offsets, including the four-byte guild/PID and duration/time fields. Builds and tests remain deferred.

Source batch 322 also restored DWARF-backed building land (36 bytes), material (8), prototype (96), object (40), and create request (32) records. Game and DB agree on four-byte object deletion and eight-byte land update. Staging and original field offsets in work/building_next. Builds and tests remain deferred.

Source batch 323 restored the original DB safebox password-change request width (18 bytes, confirmed at 080aa63b) and the packed five-byte size-change request. DB validation uses those types and SQL formatting matches their four-byte account IDs. Builds and tests remain deferred.

Source batch 323 also restored the original 1818-byte player record, embedded 10-byte horse record and 1871-byte character creation request from DWARF and the original sender. Adjusted SQL and logging boundaries for fixed-width fields. Stage in work/db_dispatch_next. Builds and tests remain deferred.

Source batch 324 restored all five original guild treasury request and reply layouts. Deposit, withdrawal, balance and give messages use eight bytes; the withdrawal result uses nine. DWARF confirms unsigned four-byte guild IDs and signed four-byte amounts. Both endpoints now use the shared types, preserving existing withdrawal result behavior. Stage and evidence in work/building_next. Builds and tests remain deferred.

Source batch 325 restored the four-byte DB heartbeat player-count record. Original heartbeat sends opcode 10 with a single four-byte word; game serialization and DB length validation now agree through the shared declaration. Builds and tests remain deferred.

Source batch 326 restored original four-byte billing check counts and login keys in both directions. Checked original CheckBilling at 0810d710 and existing game response parser. DB validation and response encoding now agree with the fixed widths; native login-data lookup and expiry decisions are preserved. Builds and tests remain deferred.

Source batch 326 also aligned MyShop price-list DB validation and encoding with the 4-byte request, 5-byte header and 8-byte entries already used by the game. Fixed guild reservation deletion to send exactly the four-byte ID. Original DWARF confirms reservation/bet records already match 46/43 bytes; setup and live forwarding now share the bet record. Stages in work/db_dispatch_next and work/building_next. Builds and tests remain deferred.

Source batch 327 restored the original 29-byte DB block-chat request: 25-byte character name and signed 32-bit duration. Verified original do_block_chat sends 29 bytes at 080c5395, matching the current game encoder. DB validation and affect persistence now interpret the same width. Builds and tests remain deferred.

Source batch 328 restored original billing login and repair encoding in DB. Both routes use four-byte counts, login entries are five bytes, and repair entries are 51 bytes. Confirmed game decoders at 080ebd80 and 080ebf90. Both DB repair producers now share the record; all EncodeDWORD callers were audited. Stage in work/db_dispatch_next. Builds and tests remain deferred.

Source batch 328 also restored the shared nine-byte DB peer frame header: opcode byte, four-byte handle, four-byte payload size. Verified original DB PeekPacket and EncodeHeader at 0804ca30 and 0804cb10. Both endpoints use the shared header and retain partial-frame handling. Setup and boot section counts already match original widths. Builds and tests remain deferred.

Source batch 329 restored the original 30-byte DB highscore request. Original QUERY_HIGHSCORE_REGISTER at 0806baf0 reads the score at +4, signed direction at +8, and 21-byte board at +9. DB packet validation now uses those fixed widths and SQL/log format arguments match. Dungeon and quest event audit found no additional verified gameplay mismatch; notes in work/reward_next/DUNGEON_EVENT_PARITY.md. Builds and tests remain deferred.

Source batch 330 restored original scalar DB decode and validation widths: four bytes for item and player cache flush, MyShop lookup and billing count; eight bytes for marriage engagement cancellation. Original DB handlers 08058440, 08059520 and 08060f20 confirm word reads. Game cache flush and marriage cancellation already use fixed-width payloads. Source stage in work/building_next. Builds and tests remain deferred.

Source batch 331 restored five original out-of-line CDungeon methods for revive, potion/revive flags and mob/stone kill counters, with original non-const signatures verified at 08185020 through 08185060. Existing quest and restart callers use mutable dungeon owners. The Lua registration audit matched all 435 in-scope registrations; the excluded Mgmt entries are Monarch-only. Builds and tests remain deferred.

Source batch 331 also restored the original inert CArena observer packet and chat hooks at 080bd2d0/080bd2e0. Duel timeout now routes observer feedback through the original no-op hook, preserving duelist notifications. Original timeout callback xrefs and staged edits in work/db_dispatch_next. Builds and tests remain deferred.

Source batch 332 restored CItem::RemoveFlag and AddFlag at 081667a0 and 081667b0. Reference and ELF agree on clearing and setting bits in m_lFlag without other state changes. Builds and tests remain deferred.

Source batch 333 restored CHARACTER::Command, which directly delegates to the existing command interpreter as verified at 08129ce0. No direct original callers were found. Builds and tests remain deferred.

Source batch 333 also restored the original arena observer chat calls in ready and death paths, including the distinct opponent-missing locale key at 08203ae0. The calls go through the original inert observer hook, so live spectator output remains silent. Stage in work/reward_next. Builds and tests remain deferred.

Source batch 334 restored PCBang_CPlayTimeWriter ownership and original Log callback via the logout wrapper at 08124e60. Short-session and unknown-IP diagnostics now run only on test servers, matching the original. Successful SQL playtime logging and bounded IP parsing remain. Builds and tests deferred.

Source batch 335 restored the original CPrivManager::RequestRemoveGuildPriv and RequestRemoveEmpirePriv symbols. ELF 08188c80 and 08188c90 both return immediately. The named hooks are restored with their original empty bodies. Builds and tests remain deferred.

Source batch 335 also restored original guild mark image pixel widths, CRC/block/index storage, and four-byte symbol file fields. ELF transfer sizes and DWARF field layouts are recorded in the staged libguild work. The public mark interfaces retain native argument types. Builds and tests remain deferred.

Source batch 336 restored CQuestManager::CanEndQuestAtState with the original signature and unconditional false return confirmed at 081a4d00. Existing quest state transitions are unaffected. Builds and tests remain deferred.

Source batch 336 also restored the two dormant CMoneyTableBySkill methods. Initialize copies 99 entries (396 bytes in the original); GetLimitMoney uses level minus one because the first table entry is four bytes past this in the original. No direct original callers or data loader were found, and the rebuild adds none. Builds and tests remain deferred.

Source batch 337 restored the dormant CPasspod::IConv method. DWARF return DIE 0x24208a6 confirms bool and ELF 0815c350 returns true without touching output. The guild mark audit confirmed the rebuild intentionally counts the symbol upload header once; the original counts it twice. Builds and tests remain deferred.

Source batch 338 restored the original Guild::DeleteMarkManager API in libguild. The ELF body at 0807ab10 null-checks and dispatches the virtual destructor, equivalent to the restored delete expression. Original CGuildManager uses its own direct delete, so no callers were changed. Builds and tests remain deferred.

Source batch 338 also restored the original IP::Read owner at 0815b9f0 and both ban-list endpoint callers. The rebuild retains strict IPv4 validation and the existing mask and bucket behavior. Stage in work/db_dispatch_next. Builds and tests remain deferred.

Source batch 339 restored original file placement of ten movement and AI methods across char.cpp and char_state.cpp based on ELF line information. The guard target scan now uses the original named FuncFindGuardVictim, capturing the guard origin once at scan start. Existing movement validation guards were retained. Stage in work/building_next. Builds and tests remain deferred.

Source batch 340 restored ElemSet::remove at 0811b120 and routed __delete through it. Pointer lookup, size accounting and payload free follow the original; std::list erasure safely unlinks the tracking node. Source review confirmed the moved movement methods retain their required includes. Builds and tests remain deferred.

Source batch 341 restored the named original NPartyExpDistribute::FPartyTotaler and FPartyDistributor helpers. Original 08155fc0 calls both ForEachOnlineMember instantiations, with distributor operator at 08157590. The recovered source uses the returned totaler and retains existing experience formulas and empty-party guards. Builds and tests remain deferred.

Source batch 341 also restored original shared parse_token use across both config passes. The caller keeps its existing rejection of separator-free lines and supplies a second terminator when a line ends at the separator, because parse_token starts two bytes after it. Stage in work/db_dispatch_next. Builds and tests remain deferred.

Source batch 342 restored original marriage::CManager::Initialize and Destroy at 080b12e0/080b12f0. Reference and ELF agree that Initialize returns true and Destroy does nothing. No new startup or shutdown callers were added. Horse and polymorph active-path audit found no verified gameplay divergence. Builds and tests remain deferred.

Source batch 343 restored the original four-byte sector map key verified by DWARF. LoadAttribute and SaveAttributeToImage initialize lookup keys, and the map no longer sees indeterminate high bits on LP64. Source stage and DWARF in work/building_next. Compilation-unit inventory found only the known C-to-C++ event filename variants and Monarch-only sources absent from the live non-Monarch tree. Builds and tests remain deferred.

Source batch 344 restored the original empty GD115 /reload a request at the rebuilt DB boundary. Original game do_reload sends zero bytes, while the available original DB ReloadAdmin reads a supplied IP pointer. The rebuild uses each recipient peer setup IP for the empty request and keeps the explicit-IP form. Per-recipient scoping is a reconstruction choice for the original binaries disagreeing on this request. Builds and tests remain deferred.

Source batch 345 restored original party EXP arithmetic and damage-share rounding. The party bonus wraps the 32-bit product before signed division; the centralized share and damage-share products retain the original wider intermediate precision. Evidence at 08156180, 081562da and 0815333f through 081534f8. For 7 raw XP and leading damage 5 of 6, the original leading share is 5 rather than the prior 6. Uses long double to model the original x87 intermediate on supported compilers. Builds and tests remain deferred.

Source batch 346 restored the original out-of-line CUBE_DATA constructor used by the cube recipe loader. Both original constructor variants (080fc180 and 080fc1e0) construct the three empty vectors and set the percent to zero. The source keeps those initialization semantics. Builds and tests remain deferred.

Source batch 346 also restored ordinary one-shot regen spawn origin/rotation via SetRegen(nullptr), preserving the no-retained-temporary policy and original dungeon one-shot exclusion. Retained a quota guard to avoid unsigned wraparound. GiveExp now uses original wrapped 32-bit products before signed division for level penalties and modifiers; all four level-penalty tables matched the binary. Stages in work/building_next and work/reward_next. Builds and tests remain deferred.

Source batch 347 restored original ELF source ownership of two summon helpers in char.cpp and three alignment methods in char_battle.cpp. Alignment addition reuses that file own 32-bit wrapping helper. Death penalty branch order, call flags and all 100 original percentage entries matched in the parallel audit. Builds and tests remain deferred.

Source batch 348 restored original translation unit ownership of three stamina methods and SkillLevelPacket (ELF line entries 08134260, 08134380, 08134390 and 0806afb0). Also restored original wider intermediates for IncRegenCount statistics at 080f6690, avoiding float truncation before the original double store. The implementation uses long double to model x87 intermediates where the compiler provides it. Builds and tests remain deferred.

Source batch 349 returned CHARACTER::SendEquipment to its original char.cpp owner (ELF 08135e60, source line 5982). Verified the existing layout against the original 0x2d7-byte send and added a size assertion. Builds and tests remain deferred.

Source batch 350 restored CHARACTER::IsHack to its original char.cpp owner (ELF 08136830, line 6348). Reuses char.cpp fixed-width arithmetic for the original cooldown comparisons. The shop, exchange, storage, cube and refine guards and diagnostics were preserved. Builds and tests remain deferred.

Source batch 351 restored original melee and arrow mob-multiplier rounding from ELF 080d6d87, 080d6d3a and 080d71fd. The integer damage operand now rounds to float before wider multiplication. Evasion and critical/penetration marriage additions retain the original 32-bit wraparound. The rebuild uses long double for the wide intermediate where supported. Builds and tests remain deferred.

Source batch 352 restored explicit 32-bit wraparound for event deadline creation, reset, recurrence and remaining time. Original instructions at 0804cf62, 0804d153, 0804d308 and 0804d38a add/subtract four-byte pulse words. Queue ordering and delay policy stay as recovered. Also checked all eight VCXPROJ source lists against their live files, with no missing entries. Quest persistence audit found no verified valid-data mismatch. Builds and tests remain deferred.

Source batch 353 restored original 32-bit subtraction and multiplication before signed division by 100 for normal-hit weapon resistance in battle_hit (ELF 080d7471 through 080d7487). Reuses battle.cpp fixed-width helpers. DB source and security review are complete per the owner. Builds and tests remain deferred.

Source batch 354 restored the original UseArrow replacement path in char_battle.cpp. ELF 08153a9b tail-calls EquipItem for the next stack, so the recovered code now uses the normal equip checks, side effects, and quickslot handling. Game-side DB frame, handshake, and boot audits found no verified valid-packet difference. Builds and tests remain deferred.

Source batch 355 restored the original cube material traversal on an exact stack match (FN_remove_material 080fbfd0, strict greater-than exit at 080fc020). The next eligible matching stack receives the original same-count update/save when present. Retained early pointer clearing, cumulative duplicate-row material checks and exchange checks. Original archery selection, shot handling, target queue cleanup and FlyTarget packet audit found no additional supported-input difference. NoticeInMap was confirmed Monarch-only by all four callers. NoticeToMyEmpireInMap had no direct caller. Builds and tests remain deferred.

Source batch 356 restored dungeon unique-spawn diagnostics in SpawnUnique and SpawnMoveUnique (ELF 08183600 and 08183090). The error reports the configured source-area midpoint, with the map base applied. Spawn placement is unchanged. Map boot order, 161 movable offsets and eight sector-neighbor offsets matched in the static audit. Builds and tests remain deferred.

Source batch 357 restored Cube_make success notification before AutoGiveItem (ELF 080fce64 then 080fce8c). Reward draws and failed-roll packet/log were verified. The existing null-grant guard still sends a failure response, after the restored success notification if item creation fails. Builds and tests remain deferred.

Source batch 358 returned nine war and ladder methods to guild_war.cpp, as confirmed by original ELF line mappings and reference placement. Function bodies were preserved and the ownership rescan shows no remaining CGuild placement mismatches. Builds and tests remain deferred.

Source batch 359 restored the original pointer-sized recovery event allocation in char.cpp (ELF 0812d200/0812d20f), retaining four bytes on i386 while using the actual record size on wider hosts. The recovery diagnostic now passes an int to its %d field. Recovery timing and HP/SP paths matched in the audit. Moved CanDoCube from cube.cpp to char_item.cpp based on original line information and reference placement, preserving the recovered body. Builds and tests remain deferred.

Source batch 360 restored record-sized event allocations for ore expiry, warp NPC, affects, Muyeong, delayed disconnect, accessory expiry and Santa spawn. Each original record remains four bytes on i386. Wider hosts reserve space for the actual pointer or native integer. No builds or tests.

Source batch 361 restored the global CMapLocation singleton that the original static initializer constructs at 0811e7f0. The two Get overloads now take output references (ELF 0811e5e0 and 0811e660), with all four callers updated. Definitions stay in map_location.cpp to match the supplied source layout. The map-location packet and first-insert-wins behavior matched in the audit. No builds or tests.

Source batch 362 replaced literal i386 allocation lengths with actual event record sizes for fire, poison, chain lightning, quest confirmation, wedding and quest targets. This preserves the i386 sizes and covers wider fields on other hosts. Santa respawn uses a four-byte map index, verified by original allocation/store at 0818ae10 and callback at 0818ac80. No builds or tests.

Source batch 363 restored the event-info ID 32-bit rollover (ELF 0804cc7b), with original signed map-key interpretation. Static audit matched payload zeroing, queue tombstones, reset, cancel and callback ownership. The singleton census found no other missing active instances after CMapLocation. No builds or tests.

Source batch 364 restored the original private WORD sector version initialized to 0x0300 and external linkage for the zero-initialized regen_list and regen_exception_list (ELF 0823ec3c, 08322a08, 08322a0c). Placed the exception record declaration in regen.h as in the reference. Original nation, start, character creation and arena globals matched. No builds or tests.

Source batch 365 restored the OX quiz 32-bit pulse product at 08087fe9 before the signed scheduling clamp. Uninterrupted quiz phases, notices, 10/5-second cadence, answer checks and audience warp matched. Existing per-event phase, timer cancellation and handle clearing safeguards remain deliberate. No builds or tests.

Source audit 366 verified unchanged data against the original ELF: all 64 JobInitialPoints values (0823f5e0), all eight creation position values (082401a0), all 36 China event starter item records (0823fd60), the 0x74f character-create DB request, and the success and failure response widths. Poison and fire audit verified all six rank rates, nine level proc chances, original tick order and three-second recurrence. No source difference was found for the supported inputs checked. No builds or tests.

Source batch 367 restored the reference-named map attribute flags ATTR_BLOCK, ATTR_WATER, ATTR_BANPK and ATTR_OBJECT and their uses across spawn, movement, combat, campfire and building occupancy. Original masks and region check behavior were verified against the binary. Character selection and rename supported paths matched the original. No builds or tests.

Source batch 368 restored the original 32-bit shared millisecond clock (ELF 081b94d7/081b94e7), fixing widened timestamps compared against stored 32-bit skill deadlines on LP64. UseSkill now uses original truncation and 32-bit seconds-to-ms multiplication (08072094, 080722b2, 080725b9). No builds or tests.

Source batch 369 restored get_float_time signed 32-bit epoch subtraction (ELF 081b9493/081b9499) and uses long double to model the original x87 intermediate where available. Existing host LONG_MIN/LONG_MAX assertions reflect prior host-width behavior and need review in the later data-test pass. Game-side login reply framing and 329-byte selection layout matched in the static audit. No builds or tests.

Source batch 370 restored 32-bit wraparound in original combat and movement clocks, including character defaults, guild-war death notice, horse movement, summon death window, and Santa/play-time deadlines. Item equip timing already matched unsigned 1500ms comparison in ELF 0817f726-0817f743. Character save defaults and flags matched in the parallel static audit. No builds or tests.

Source batch 371 restored sector-neighbor traversal for dungeon ExitAll (081851c0), JumpAll (081827b0), and the JumpToEliminateLocation fallback (081858c0). Overlapping lists can revisit players as in the 2009 binary. Dungeon notice, kill and purge retain direct traversal. No builds or tests.

Source batch 372 restored neighboring-sector traversal in dungeon ExitAllToStartPosition (ELF 08185460), reusing the existing helper for warp and exit paths. Saved-location behavior matched. The original 45-entry dungeon Lua API lacks the newer reference item-group gates. Parallel item-load audit confirmed deferred placement retries and corrective saves. Existing whole-grid and invalid-slot safeguards remain. No builds or tests.

Source batch 373 restored StateMove interpolation rounding (ELF 08080265-080802f7) and client Move timer ratio rounding (080ff53c). Wide arithmetic uses long double where supported. Also restored quickslot clearing before the safebox item-set on check-in (08100240). Existing defensive inventory rollback remains, and initial safebox/mall row placement matched. No builds or tests.

Source batch 374 restored thecore_msleep unsigned four-byte division and signed wrapped timeval microseconds from ELF 081b93c8-081b93de. The legacy use of the full duration for the microsecond field is preserved. Safebox move/merge/password audit found no supported-input difference. No builds or tests.

Source batch 375 restored six original dungeon Lua spawn traces in questlua_dungeon.cpp. The audit verified spawn defaults and found original dungeon_new_jump (08094082-080940a2) passes privateMap/x/y to WarpSet. The reconstruction keeps reference x/y/privateMap ordering to satisfy the callee contract. No builds or tests.

Source batch 376 restored four-byte map IDs and 32-bit pulse multiplication for dungeon elimination timers (ELF 081856a0), plus the original missing-dungeon diagnostic. The audited exit priority, jump-before-regen order and one-shot filename clearing matched. Supported exchange offers and personal shop listing paths also matched. No builds or tests.

Source batch 377 restored signed 32-bit VID formatting in damage detail chat (0814e174), wrapped stun recovery drains (0814814a/0814817e), and wrapped timed revival HP delta (0814836b). Damage, stun, death and revival packet order matched the original. Existing descriptor guard retained. No builds or tests.

Source batch 378 restored original ETargetTypes position/VID names and NPC::OnTarget condition logging/evaluation for empty conditions (081b13a0), with bounded logging. Restored original successful-proc stat re-reads for HP/SP steal and recovery, pre-roll Japan threshold and post-fly descriptor sample in Damage. Quest target timing and packet layouts, poison/stun/slow order, and 106-byte quest save rows and flush timing matched. No builds or tests.

Source batch 379 restored original critical/penetration descriptor sampling after fetching chance points, and penetration defense re-reads after the test-server chat. The bonus addition now wraps as in the 2009 ELF. Peer warp/transfer payloads and handoff order matched in the parallel audit. No builds or tests.

Source batch 380 restored original server timer set/loop/clear argument conversion over the full unsigned 32-bit key range and associated diagnostics. Existing duplicate-event and callback self-cancel guards remain. Restored successful guild skill level-up log after online broadcasts (0808e700). Supported SP charge and game-side DB skill update matched. No builds or tests.

Source batch 381 restored target watcher pre-advance before clearing and sending updates (ELF 081323aa-081323d8) and German locale suppression of global guild war declarations after both updates (0809b9d0). Target HP/effect packets and guild war map/accept paths matched. Building target packet HP remains zero-initialized where the original left it unwritten. No builds or tests.

Source batch 382 restored 32-bit delay multiplication for all five quest map/village warp schedulers (08059330) and two original quest diagnostics. Movement replay divides angle by five then truncates to signed short and sends its low byte (081285b0-081285da). Verified FDIVR uses memory angle divided by loaded 5.0. Invalid conversion emits low byte zero. No builds or tests.

Source batch 383 restored CWarMap::SetEnded cleanup order. ELF 0816b860-0816b8d8 destroys flags and bases before the reset timer cancellation at 0816b8eb. Draw, reward calculation, and delayed teardown matched. No builds or tests.

Source batch 384 restored wrapped 32-bit HP and SP differences for full and 50-HP respawns (ELF 080a95af/080a95d9, 080a93fb/080a94e0). Town, in-place, guild-war and three-way revival ordering matched, and original war-map startup, spectator counts, spawn data and entry/exit packet order matched. No builds or tests.

Source batch 385 restored four-byte guild presence PIDs (ELF 0808c2b0/0808c380), eliminating a 12-byte LP64 send with an advertised eight-byte size. Restored quest spawn angle rounding and wider radial intermediates (08053330/080538e0) using double as an approximation of x87, and original setstate trace. No builds or tests.

Source batch 386 restored Reward map lookup after the kill quest and per ground drop. Restored original 32-bit contribution sum/10% comparison, special gold scaling and HP/SP recovery products with original stat re-reads. Drop placement order, 10-second ownership and 300-second expiry matched. No builds or tests.

Source batch 387 restored UDP state checker unsigned 32-bit key/index, exact 9-byte request and 10-byte response, and four-byte UDP handshake parsing. On LP64 the native fields previously read beyond the request and omitted state in the reply. The TCP scalar write scan found no further verified DWORD mismatch. No builds or tests.

Source batch 388 restored d.spawn_group quest coordinate and radius scaling by 100 (08096130), single-mob radial rounding using double as x87 approximation, and original low-32 vnum conversion. Existing count limits remain, and rectangles outside signed world coordinate range are rejected. No builds or tests.

Source batch 389 restored RewardGold threshold evaluation before RNG (081487c4-081487e9), original rank and bonus re-reads, and 32-bit arithmetic for chance, scaling, total gold and item counts. Existing zero privilege divisor guard remains. Gold pile counts and scatter RNG order matched. No builds or tests.

Source batch 390 restored the d.spawn_group Lua count conversion using the low 32 bits of the signed-64 truncation (08096339-0809634c). Retained no spawn for a negative low word and existing coordinate guards. Original rectangle and first successful leader matched. Initialized the otherwise discarded UDP byte counter to avoid the original uninitialized read. UDP dispatch and state thresholds matched. No builds or tests.

Source batch 391 restored race-5001 on-hit gold expiry after both successful and failed placement (0814f545/0814f54d), post-roll coordinate sampling with original 32-bit offsets, and damage HP/SP snapshots after the special branch. Retained zero-max-HP guard and initialized Z. Party gold split and ordinary-only autoloot matched. No builds or tests.

Source batch 392 restored four-byte wrapping in ground item expiry and ownership timer products (081665b4/08168047) and pickup coordinate subtraction (08166d79/08166d7c). Reused existing arithmetic helpers and retained the null-character guard. No builds or tests.

Source audit 393 verified dungeon spawn member order (08184360, 0818db00, 081319c0) and peer setup/reconnect (080ea330, 0811d960/0811d870, 0811c060, 0813f370). Spawn visibility precedes attachment/counting, and link precedes aggression. Peer roster/setup order, identity updates, and retry timing matched for supported data. No builds or tests.

Source batch 394 restored original symbolic warp/goto types and named goto spawn constant and buffer extent. Warp NPC parsing retains grammar and scaling while discarding unused label safely. Restored signed 32-bit unique expiry time comparison (08168545) and wrapped countdown/start delays (081684a0/0816854f/08168625). Countdown, wall-clock polling and unequip behavior matched. Guild skill-info legacy 22 advertised/21 sent confirmed and preserved. No builds or tests.

Source batch 395 restored dungeon unique HP percent precision as observed by Lua (08183f20 and 08094d27-08094d33). Return type is double instead of source float to avoid narrowing on modern ABIs. Preserves original IEEE infinity/NaN for zero max HP and zero for a missing unique key. HP/defense setter behavior and supported guild bank paths matched. No builds or tests.

Source batch 396 restored SetSockets(const long*) (08167d70), original signed 32-bit timeout socket and diagnostic argument (08168935/08168979), and signed i386 remainder division before accessory timer cancellation on unequip (08168bb1). Socket save and degradation ordering matched. No builds or tests.

Source batch 397 restored d.set_unique low-32 VID conversion (08095a05-08095a31) using the dungeon Lua DWORD reader. Kill/purge and first non-PC alias removal on death matched. Outgoing guild DB membership, treasury, cooldown and war packet sizes matched; the unused skill save byte remains zero-initialized. No builds or tests.

Source batch 398 restored original post-callback owner reads in CItem::EquipTo (0816798b/08167996) and post-equip item vnum read in CHARACTER::EquipItem (0817f828). Equip/unequip timers, points, packet and save order matched; no normal valid-item behavior difference demonstrated for these reads. No builds or tests.

Source batch 399 restored dungeon Lua flag numeric behavior: d.setf truncates, d.setqf rounds, and both write INT_MIN on invalid signed-32 conversion as original FISTP. Restored per-member trace. Character/item DB payload widths and equipment stat effect ordering matched. No builds or tests.

Source batch 400 restored signed 32-bit skill-book cooldown comparison, fresh deadline/clock reads for denial chat, wrapped grandmaster next-read addition and original level re-reads for chances/requirements. Kept the lower chance-table index clamp. Party invite/answer and timeout behavior matched. No builds or tests.

Source batch 401 restored signed 32-bit coordinate wrapping when multiplying dungeon JumpAll and elimination fallback tiles by 100 (081827c2 and 081859a0). Solo/party save-exit and online warp routing matched. Retained d.join leader validation before allocation and d.new_jump corrected WarpSet argument order. No builds or tests.

Source batch 402 restored zeroing of each offline 21-byte roster status in SendPartyInfoAllToOne (08142d00), removing stale or uninitialized affect bytes. Online HP/affect layout and party distance threshold matched. No builds or tests.

Source batch 403 restored HP/SP skill cost truncation and wrapped negation, current resource re-read after polynomial evaluation, and the original low-32 cooldown zero test/addition and diagnostic argument. Preserved cost-before-cooldown ordering. Skill level packet layout and denial text matched. No builds or tests.

Source audit 404 confirmed original party skill and experience mode behavior. CInputMain::PartyUseSkill 081013d0 gates to leader, HealParty 081434a0 is empty, Summon 081434b0 uses the 12 offsets at 082280f0 with matching RNG and Show/Stop order. EXP distributor 08157590 uses the same weighted/equal modes and 5000 range. Existing absent-leader and zero-weight guards retained. No builds or tests.

Source batch 405 restored GetDeltaByDegree separate sine/cosine radian rounding (08164d34/08164d37) and horse skill 137 angle difference rounding (08076398-080763bb). Uses long double for wider products where available. Status effect order and messenger add/remove P2P widths matched. No builds or tests.

Source batch 406 restored DWARF-named TPacketGCDungeon (4 bytes), TPacketGCDungeonDestPosition (8 bytes) and FSendDestPosition. Preserved the split BufferedPacket/Packet send and near-party-member filtering. Dungeon area parsing, map lifecycle and selected dungeon routing matched. No builds or tests.

Source batch 407 restored original rounding points for Normalize, DotProduct, direction scaling and angle delta, using long double for x87 intermediates where available. XY direction subtracts in four-byte signed coordinates. Movement duration, messenger initial list/login, and dungeon destination routing matched. No builds or tests.

Source batch 408 audited original dungeon teardown 081825a0 and retained first empty-set schedule, including an absent member. Added a deliberate guard against overwriting a live teardown timer and leaving an orphaned callback with a dangling dungeon pointer. Party attachment/unlink and messenger authorization/logout ordering matched. No builds or tests.

Source batch 409 restored 32-bit handshake clock bias and retry math, including acceptance across counter rollover. Restored movement-driven exchange distance deltas and race-2191 teleport offset truncation and wrapped additions (080808ee/080808f3 and 080807c2/080807d8). Preserved zero-duration movement guard. No builds or tests.

Source audit 410 verified dungeon monster and stone count transitions, elimination timing and resurrection attachment. Original SetDungeon 081319c0 decrements and checks old dungeon before replacing the pointer, Dead 0814af70 removes only the unique key, and dead_event 08148280/Destroy 081274f0 perform final detach. Original party revival does not attach the replacement to the dungeon; direct resurrection-vnum replacement does. Current source matches these paths. No builds or tests.

Source batch 411 restored localized shout denial level formatting (080fe29a) and original aggro scaling before integer truncation (08154611/41/79) plus 32-bit candidate distance deltas. Uses long double to model x87 intermediate where supported. Aggro tie-break, target delay, and party propagation matched. No builds or tests.

Source batch 412 restored current selected-stone lookup before each moving-group member attachment (0818d700), while preserving the initial snapshot for spawn motion. Group RNG and regeneration exception behavior matched. No builds or tests.

Source batch 413 restored original four-byte multiplication before signed division in stone HP wave selection (0807dc0a-0807dc1a). All ten thresholds/radii, wave RNG, stone attachment, and child death/detach matched. Zero-max-HP and child erase guards retained. Whisper packet/routing matched with existing delayed relay-target assignment. No builds or tests.

Source batch 414 restored the original hit-attribution descriptor and SP drain reads and max-aggro store before target relookup. Restored escaping the full original 85-byte shout log field, including padding. No builds or tests.

Session stop after batch 414 at user request. All three parallel audits completed. Pending integration: work/reward_next/apply_damage_ledger_arithmetic.py --apply (DistributeExp 32-bit damage/distance arithmetic and nearest-victim float distance), and work/building_next/apply_regen_timer_width.py --apply (original 32-bit row time and four timer products). Both stages passed their guards and were not applied. Empire-language conversion tables and startup order matched. Resume by reviewing those two stages against live source before applying. No builds or tests.

Source batch 415 integrated both prepared patches from the batch 414 handoff. DistributeExp uses original 32-bit coordinate and damage arithmetic (08153732/08153747/081537b4), and GetNearestVictim stores the best distance as float (08153dd6-08153dea). Regen uses a uint32 row time and the original signed low-32 pulse product at all four scheduling sites (0807a0c1/0807a554/0807a279/0807a7df). Both guarded checks passed against live source. No builds or tests.

Source batch 416 restored attack-rating precision at skill call sites. ELF 080d6098 returns through x87; direct and position skills spill to double (0806fccf/0806e020), while splash spills to float (08075a14), as do melee/arrow. Use long double for the helper return where supported, preserving each original caller rounding boundary. Cross-channel presence, guild war map updates and party status paths matched in the parallel audits. No builds or tests.

Source batch 417 restored LoadMapRegion town and empire spawn coordinate scaling and addition in signed low-32 arithmetic (080b5060). Map recall fallback and user-count reporting matched in static comparisons. No builds or tests.

Source batch 418 restored Ambush skill 31 dagger coefficients at 08076c53-08076c7c and 08076ce9: rear YMIR +1.0, rear other locales +0.5, non-rear other locales +0.5. Front YMIR and Stealth bases already matched. DB and peer notice routing matched for valid packets. No builds or tests.

Source batch 419 restored original random map location clamp widths at GetRandomLocation (080b63c0): signed wrapped distance, unsigned side selection and sector lookup, signed attribute/output coordinates. Preserved 100 attempts and duplicate-region retry. MyShop price list widths and supported per-item price calculations matched. No builds or tests.

Source batch 420 restored original NPC sale unsigned 32-bit arithmetic and Ambush rear-angle precision. Shop sale preserves original count times unit price wrapping and unsigned gold sum. GetDegreeDelta keeps x87-style precision (08164da2-08164da9) for the direct 35-degree skill check (08076bd2-08076bdd); the Follow caller retains its float spill. No builds or tests.

Source batch 421 restored original CShop::Buy 32-bit surcharge and tax products (080d37d0), and signed 32-bit sector coordinate reduction plus dword attributes (080a7830). Existing attribute indexing behavior was preserved. No builds or tests.

Source batch 422 restored 32-bit LoadSetting dimension scaling (080b42c0) and SaveAttributeToImage traces (080b7ce0). Exchange wire and shop listing/update layouts matched. No builds or tests.

Source batch 423 restored the VCard ID argument type for both original %lu log sites, retaining the four-byte DB wire field. Exchange transfer and save order matched. The inherited consumed-VCard pointer lifetime behavior remains unchanged. No builds or tests.

Source batch 424 restored the free-angle building occupancy radian conversion (080b744f-080b7472): signed 32-bit angle, wide division and multiplication by the original float constant, then one float rounding before trig. Uses long double where supported. Subsequent rasterizer spills remain unverified. No builds or tests.

Source batch 425 restored PickupItem (0817ec10) SetCount update/save when a matching inventory stack is exactly full at 200. Overfull records retain the existing protection. Gold pickup and party split matched. No builds or tests.

Source batch 426 restored ComputeSkill range comparison (08071c1b-08071c27) using the original unsigned 32-bit +50 grace and exact promotion rather than rounding the threshold to float. The rejection log reports the configured base range (08071cad) and passes the correct %lu type. Skill motion tables and cooldown branches matched. No builds or tests.

Source batch 427 restored original width-product float spills and wider height arithmetic for seven rotated occupancy corner values (080b7484-080b75d3), using long double where available. Final corner and scanline still under audit. Restored signed 32-bit vnum and int count to original AutoGiveItem diagnostic. Item routing matched. No builds or tests.

Source batch 428 restored StateBattle motion-to-ticks precision and fallback width (08081043/08081063-08081071), and restored rotated occupancy final corner and scanline rounding around original x87 spills (080b766c-080b768a and cell callback). Final corner reuses the prior width spill and wide height. Cross-libm runtime parity remains unverified. No builds or tests.

Source batch 429 restored DWARF-named NPC position record and header in packet.h, with verified 34/5 byte sizes and original fields. Preserved packet byte layout, empty-map suppression, oversize guard and split sending. No builds or tests.

Source batch 430 restored original unsigned 32-bit scheduling and comparisons in StartStateMachine/UpdateStateMachine (08130330-08130424), preventing LP64 widening across pulse rollover. Retained existing callback destruction and zero-duration movement guards. Potion and recall normal-data behavior matched. No builds or tests.

Source batch 431 restored original ForAttrRegion signed 32-bit alignment, end adjustment, wrapped span difference before division and signed rotation modulo 360 (080b7ac0). Ordinary geometry and traversal are unchanged. No builds or tests.

Source batch 432 restored original heartbeat 32-bit pulse increment (080a1aa6), MoveItem merge/split count update ordering (0817e490), and LoadDungeon 32-bit center plus/minus radius (080b4780). Preserved safe allocation/rollback, overfull protection and malformed row handling. All guarded checks passed. No builds or tests.

Source batch 433 restored DropItem 0817ddf0 ordering: clear quickslot before inventory removal/update, flush partial source before ground visibility. Prepared allocation and map validation before side effects; retained count rollback and kept split log after successful placement. Private map allocation and valid attribute-file geometry matched. No builds or tests.

Source batch 434 restored original guard victim type filter (08082104-08082118): descriptorless PC or monster, excluding ordinary NPCs. Gold drop ordering and fields matched; retained existing cleanup of failed placements. No builds or tests.

Source batch 435 restored guard selection and leash dword coordinate deltas (08082187-0808219e, 08133002-08133018). Guard range, scheduling, stun and reward suppression matched after batch 434. Ordinary warps, mob group order and equipment effects also matched. No builds or tests.

Source batch 436 restored StateBattle post-kill obstacle probes with dword coordinate wrap, original single float destination spill before rotation and signed-32 Goto truncation (08080b24-08080c42). Replacement victim skips the battle-type lookup as in the original. Refinement normal paths matched. No builds or tests.

Source batch 437 restored MAP_ALLOW byte tokenization and per-token atoi (080d961b-080d9719), preventing an invalid token or numeric suffix from stopping later entries. Restored UseMobSkill low-32 timing product before division, x87 signed truncation sentinel and wrapped offset adds (08073ea6/08073fd2/0807412e-0807414f). Retained unique event keys for cancellable overlapping casts. Quest item callbacks matched. No builds or tests.

Source batch 438 restored Follow interception cosine/approach/lead rounding (081330ee-08133120, 08133434-08133456) and ITEM_ID_RANGE quoted two-argument parsing, original uint32 bounds and reversed-range diagnostic (080d8300). Kept missing-argument validation. Hair and polymorph active paths matched. No builds or tests.

Source batch 439 restored Follow integer-squared distances (081330bf/0813314f/08133339), projected floating distance and original coordinate rescaling/truncation (08133495-0813353b). Large squared distances can wrap and yield NaN as in the original. Item expiration/count paths matched for supported data. No builds or tests.

Source batch 440 restored residual projected Follow distance arithmetic (08133495-081334d0): multiply each signed wrapped dword delta by its exact wide delta. Only signed boundary crossings differ from batch 439. Terrain masks, horse obstacle checks and equipment slot rules matched in static audits. No builds or tests.

Source batch 441 restored original signed dword span and midpoint arithmetic for GetCenterPositionOfMap (080b5944-080b596c), used by GM and cross-peer quest warps. Verified ordinary recall and location validation already match. No builds or tests.

Source batch 442 restored extended heading return precision (08164ccf-08164cef/08164d17) and original direct int32 truncation for close-range monster reposition (081333dd). Existing item lround path retains its float boundary pending separate audit. Quest reward and exchange persistence order matched. No builds or tests.

Source batch 443 restored StoneDetect direct extended-to-int truncation (081758dd-0817591e and 081762c1-08176302), and horse skill 137 heading precision through rotation subtraction before fmod (08076398). Other skill float boundaries retained. No builds or tests.

Source batch 444 restored signed low-32 coordinate math and target name formatting for SpawnNameMob/SpawnGotoMob (081844d0/08184630). Existing spawn path and target parser retained. No builds or tests.

Source batch 445 restored 32-bit midpoint and base arithmetic for unique-spawn failure logs and direction RNG before moving-unique map lookup. Kept null-map guard. PullMonster now uses original dword deltas and direct signed truncation after adding the float offset to the exact position (08156e0b-08156eda). No builds or tests.

Source batch 446 restored StoneDetect signed 32-bit squared distance and original unordered-distance routing to the farthest bucket (08175855/08176239). The source guard and read-back passed. No build or tests.

Session handoff: the map-bounds audit finished. Pending patch: USER_HOME/Documents/Codex/2026-10-23/i-h/work/building_next/apply_map_region_bounds.py (dry-run passed; not applied). LoadMapRegion (080b5060) and BuildSectreeFromSetting (080b44a0) use wrapped 32-bit bounds and the asymmetric Y fringe formula. The original debug log prints stored unsigned 16-bit sector keys. Dungeon area parser (080b4780) matched supported rows. Resume by reviewing and applying the staged map patch.

Source batch 447 restored wrapping signed 32-bit map bounds in LoadMapRegion (080b5060) and BuildSectreeFromSetting (080b44a0), including the existing asymmetric Y fringe formula. Sector debug logging now uses the stored 16-bit keys. The staged map patch was applied after the dry run and verified by source read-back. No builds or tests.

Source batch 448 restored wrapping dword regen offsets before sorting bounds in regen_load (0807a560), regen_load_in_file (0807a380) and regen_do. The NPC map marker midpoint wraps the endpoint sum and base subtraction (0807a810-0807a838). Source guard and read-back passed. No builds or tests.

Source batch 449 restored low-dword signed products in item drop event, mob rate, premium, common drop and kill-count group calculations (0819347a/08193627/0819365a/08193754/08194523). PC Bang point is re-read on the taken branch and uses the original dword modifier (081946b2-081946d3). Source guard and read-back passed. Special item vnums and empire text conversion matched in static review. No builds or tests.

Source batch 450 restored unsigned dword mob-group IDs, keys and members (080f6bb0/080f7060), normal-hit stun apply point and value (080d72d2-080d72e3), and x87 truncation to low dword for four item drop loaders (08196a66/08195ca1/08198782/08197632/08197b4e/08197ee8), including the zero low dword for integer-indefinite conversion. Restored the original nonconst CFSM::IsState signature (08050310); transition behavior matched. No builds or tests.

Source batch 451 restored four-byte CAttribute terrain storage and unsigned default cells (081f51c0-081f5a9d), preserving DWORD-facing interfaces, and low-dword safebox merge counts (08097675-080976b5). Safebox and mall transfer packet paths matched. Source guards and read-back passed. No builds or tests.

Source batch 452 restored wrapping dword position deltas and resistance subtraction for ranged hits (08156bac/08156bb2 and 08156712). Skill and death stun payloads, SP regeneration branch selection, and ordinary map-location lookup matched on static review. The source guard and read-back passed. No builds or tests.

Source batch 453 restored signed fishing table weights and wrapping dword cumulative sums (080821e0/080824cb), with the original binary search on signed cumulative weights and the existing safe no-result fallback. Source guard and read-back passed. No builds or tests.

Source batch 454 restored the four global skill-book tables (0821b0e0/0821b120/0821b160/0821b1a0) in constants.cpp and routed the existing callers to their original symbols. All 40 values matched the existing local tables. Restored the unreferenced 100-entry exp_table_taiwan symbol (0821a5e0) without assigning it to an existing locale. ELF extraction and source read-back passed. No builds or tests.

Source batch 455 restored dword miss-weight and ordinary catch-length arithmetic and original rare catch rounding (08082b23/08083415/08083492-080834c4). Equipped Metin socket IDs now use unsigned dword lookup keys in ModifyPoints (08167420). Audited normal hit effects and PvP gates matched. Source guards and read-back passed. No builds or tests.

Source batch 456 restored ten ELF-backed global constants in constants.cpp for rank gold, guild EXP, socket quantities/probabilities, attribute chance and party bonuses. Active callers use the original symbols and previously embedded values were checked against the ELF. Monster splash motion loading now adds the time offset and negates Y at dword width (0811483b-08114855). Source guards and read-back passed. No builds or tests.

Source batch 457 restored the missing Chain Lightning hit counter advance (08075b96), after setting the polynomial chain variable from the pre-hit counter (08075b3d) and before damage evaluation and the max-hop check (08075df8-08075e08). A direct ELF comparison matched all values of the six locale EXP tables, level-delta tables, party EXP distribution, death EXP loss, chain caps and polymorph power tables. Source guard and read-back passed. No builds or tests.

Source batch 458 restored writable CHN_aGuildWarInfo and KOR_aGuildWarInfo globals (0823fca0/0823fd00) with original caller selection, and the original unreferenced BattleTypeStats data (0823f6e0). Guild war values matched the previous local tables. No builds or tests.

Source batch 459 normalized item and mob IDs to unsigned dword in common, drop, monster and special item group readers (08195aa0/08198440/08196b40/08195f80), including map keys and prototype fallbacks. The signed special-group threshold is preserved. Follow-up Chain Lightning scheduling/selection and game DB boot, reload and item placement matched supported data. Source guard and read-back passed. No builds or tests.

Source batch 460 restored writable aArroundCoords and c_apszEmpireNames/c_apszPrivNames tables at 0823f760/0823fc68/0823fc78, and connected movable-position and privilege-name callers. Verified all 161 coordinate pairs and nine string literals byte-for-byte against the ELF. Existing locale translation and bounds checks were retained. Source guard and read-back passed. No builds or tests.

Source batch 461 restored mining::info, mining::fraction_info, mining::PickGradeAddPct, and mining::SkillLevelAddPct as writable globals (0823fea0/0823ff40/0823ffc0/08240000). All 117 data words matched the prior source; ore rows now store three 32-bit IDs as in the ELF. Refinement and audited direct/positional skill effect sequences matched ordinary data. No builds or tests.

Source batch 462 restored unsigned dword elapsed time for guild-war scoring and timeout (0816b670/0816b090), and signed dword potion reward products and total (0816a720/0816afb0). War spawn locations and OX answer areas matched. Source guard and read-back passed. No builds or tests.

Source batch 463 restored the five-second guild flag respawn delay (0816c0eb-0816c109) and the original mining::OreDrop(CHARACTER*, DWORD) symbol and dword coordinate addition (080ef2f0/080ef3a5/080ef3d7). Ore roll and placement order matched. Source guards and read-back passed. No builds or tests.

Source batch 464 restored mining::mining_event linkage (080efdc0) and the signed interpretation of the wrapped dword event delay (080effe5-080efff1). Runtime lookup and pick practice order matched. Source guard and read-back passed. No builds or tests.

Source batch 465 restored the signed 32-bit shop slot price and low-dword multiplication/storage and packet conversions (080d33d0). Ordinary buy, sell and shop-opening behavior matched. Accessory socket degradation and ownership transitions also matched. Source guards and read-back passed. No builds or tests.

Source batch 466 restored guild-building NPC rotation precision and dword base addition in CObject::RegenNPC (08106fcd-081070e9). Original float spills, extended sum/subtraction and signed x87 truncation are explicit. At one degree and local offset (-645, 510), the arithmetic model gives the original X offset -635 instead of -636. Fishing data matched all 155 timing weights and 37 zero-initialized records. Audited boss skill and ordinary drop reward behavior matched, with existing overlapping-callback and zero-divisor guards preserved. No builds or tests.

Source batch 467 restored dword coordinate deltas in FuncFindMobVictim (0813d213-0813d21e) and the signed dword comparison after doubling HP for building preference (0813d020-0813d02e). Target eligibility and distance thresholds were retained. Source guards and read-back passed. No builds or tests.

Source batch 468 restored signed dword variadic arguments for special-group gold and EXP quest notices (0815db50). Both locale strings matched the ELF. Quest drop and experience branch ordering, arena transitions and horse stamina recovery matched supported cases. Source guard and read-back passed. No builds or tests.

Source batch 469 restored wrapped dword extent and coordinate arithmetic in SpawnMobRandomPosition (0818c9c0) and the unsigned dword blocked-spawn log timestamp (0818d010). Terrain, RNG order and retry behavior matched. Source guard and read-back passed. No builds or tests.

Source batch 470 restored the type-2 view-insert bypass before coordinate evaluation and explicit dword deltas (0805155e-08051583). The null-sectree diagnostic now supplies signed 32-bit coordinates to the original %d fields (080510da-08051110). Source guards and read-back passed. No builds or tests.

Source batch 471 restored unsigned dword map/marker registry and sector-package lookup keys (080b41f0/080b3730/080b6f70/080b68f0), retaining public signatures. The test-server out-of-range melee notice now names the attacker and uses signed dword coordinates (080d5d64-080d5dbd). Character persistence matched the original valid-data paths. Source guards and read-back passed. No builds or tests.

Source batch 472 restored the original rotated building bounds rounding and dword base arithmetic (08107b90), including an ordinary edge placement difference. Restored signed dword item flags (08166700/081667a0/081667b0) so high-bit flags select the original inventory packet at 08171117. Restored external quest timer callback symbols (081ad9d0/081adae0); timer product spill and rint already matched. Observer and audited ordinary combat paths matched. Source guards and read-back passed. No builds or tests.

Source batch 473 restored external linkage for the five quest warp event callbacks (080589b0/08058d60/08059200/08059cb0/0805a000), and for the eight OX Lua functions plus end_oxevent callback (0813c710-0813cc60). OX five-second event scheduling matched. The absent management registration was confirmed to contain only excluded functionality. Source guards and read-back passed. No builds or tests.

Source batch 474 restored signed dword socket storage in CItem::SetSockets and SetSocket (08167d70/08167db0). Newly written high-bit IDs now behave the same before and after DB reload. Client update and save ordering are retained, and the item log receives the original unsigned coordinate bits. Source guard and read-back passed. No builds or tests.

Source batch 475 restored signed-byte wall length/size and wrapping coordinate geometry in DrawWall and RequestCreateWallBlocks (08109b30/0810a350). Original repeat count is clamped to 0-3 while raw signed size determines origin. Ordinary supported layouts matched. Kill rewards and party loot rotation also matched on static audit. Source guard and read-back passed. No builds or tests.

Source batch 476 restored immediate approval after SetKillerMode for eligible same-empire PvP attacks (0819d6b3/0819d746), signed dword building land containment (08109080/081076c0), and unsigned metin duplicate prototype lookup for high-bit socket IDs. CPoly random operations preserve the original extended intermediates and FISTP bounds (081bc290/081bc320); RNG scaling and call count are unchanged. Source guards and read-back passed. No builds or tests.

Source batch 477 restored the external timed_event symbol (080a8ef0), fixed-width PID in its payload, and the original message-before-decrement order (080a8f3d-080a8f42). Safe PID resolution, current-event checks and descriptor guard are retained. Source guards and read-back passed. No builds or tests.

Source batch 478 restored signed dword firework seller base-plus-tile coordinates in xmas ProcessEventFlag (0818a470). The placement table and six normal positions matched. PvP follow-up, fishing, and mining checks found no further supported-data divergence. Source guard and read-back passed. No builds or tests.

Source batch 479 restored the check_speedhack_event symbol (08137210) with the existing threshold, logging, disconnect and retry behavior. SaveSingleItem now passes signed dword arguments to the original ITEM_SAVE diagnostic; delayed persistence and packet fields matched. Source guards and read-back passed. No builds or tests.

Source batch 480 restored external dungeon exit/jump callbacks (08185630/08185590) and party_update_event (08141200) with original dword leader payload. Restored dormant do_threeway_war_regen (080c7160) without command registration, matching its map lookup and empty string lifecycle. DB affect loading matched for valid records. Source guards and read-back passed. No builds or tests.

Source batch 481 restored dormant do_b1 through do_b7 building-effect routines (080c1d00-080c2ca2) without command-table registration, as in the original. Spawn, affect, and RNG ordering were compared. Null spawns are guarded and follow-up actions resolve the saved VID rather than dereferencing an expired pointer. Source guard and read-back passed. No builds or tests.

Source batch 482 restored external save_event (0812fc50) and ping_event (08139570) symbols. Verified original save/flush order and ping/pong/phase/time-sync branch ordering. The guild mark family is present in libguild, where earlier game-only symbol search missed it. Source guards and read-back passed. No builds or tests.

Source batch 483 restored unsigned dword guild and mark ID ingress for persistence reload (0807b150/0807bef0) and the missing DBManager::CountQuery inline forwarding used by the original shutdown SQL-drain path. Source guards and read-back passed. No builds or tests.

Open source gap: dormant CHARACTER::__Profile__Damage (0814f710) is a distinct older combat body with profiler scopes Attr/mapAttr/Rest. No direct original call or jump was found. Restore it as a separate body rather than delegating to active Damage. Static branch audit is saved at USER_HOME/Documents/Codex/2026-10-23/i-h/work/reward_next/profile_damage_audit.md.

Source batch 484 restored DBManager::CountQueryResult and ResetQueryResult inline forwarding to CAsyncSQL, matching 2009 DWARF member names and the supplied reference bodies. Current source has no callers. Guard and source read-back passed. No builds or tests.

Source batch 485 restored standalone NoticeToMyEmpireInMap (0818ee90), filtering connected character entities by empire and sending chat type 2 with the original literal %s. No original direct callers were found; no dispatch path was added. Source guards and read-back passed. No builds or tests.

Source batch 486 restored four local castle siege helpers (08118670/081187e0/08119900/08119960), with original start-map table lookup and existing failed-spawn guard. Monarch hiring hooks remain excluded. Source guards and read-back passed. No builds or tests.

Source batch 487 restored dormant CHARACTER::__Profile__Damage (0814f710) as a separate combat body with Attr/mapAttr/Rest accumulation, original older critical and penetration rules, half mana shield, 90% cross-empire damage, hit recovery and WAEGU RNG ordering. Ordinary Damage remains the reflection target and no new dispatch was introduced. Guard and source read-back passed. No builds or tests.

Source batch 488 restored the local FN_random_stone selector (081189e0), original stone event routing (08118a50), and wrapping dword tile conversion in shared siege stone/tower placement. The seven vnums and RNG line were checked. Source guard and read-back passed. No builds or tests.

Source batch 489 restored dword cube recipe IDs (080fc760) and original material helper names (080fbf80/080fbfd0) with adapted signatures and existing duplicate-row/exchange safeguards. Item addon roll now retains the original x87 caller precision and cache semantics (0816e8a5-0816e8cb/0814748f-0814754b), sharing RNG state with the existing float API. Independent profiled damage audit found no correction for valid data; all original start/arena/create position tables and nation strings matched. Source guards and read-back passed. No builds or tests.

Source batch 490 restored signed 32-bit IPv4 address storage in CMapLocation::SLocation, matching original Insert/Get dword stores (0811e777-0811e7a4 and 0811e6c2-0811e6cb). The public long reference API and wire bits are retained. Source guards and read-back passed. No builds or tests.

Source batch 491 restored signed dword marriage elapsed and point accumulation, plus double-precision debug formatting (080aff00/080b07e0/080b0866-080b087a). Wedding creation/end and participant handling matched. Item attribute rolls, monster targeting callbacks and SP recovery matched audited valid-data paths. Source guard and read-back passed. No builds or tests.

Source batch 492 restored signed dword wrapping for three-way war player, monster and boss counters (080a4cc0/080a4d00/080a4d30). Negative counters retain their original winner-selection semantics. Affect and equipment/socket audits matched ordinary data. A source definition-name index now cross-references 2653 ELF global method entries in work/source-definition-index/global-method-name-candidates.tsv. 2625 have candidate definitions by name. Of 28 without candidates, 14 are confirmed tagger misses (EVENTFUNC or defaulted constructors) and 14 are excluded Monarch functions or helpers. Name matches do not verify scope, signature or behavior. Source guard and read-back passed. No builds or tests.

Source batch 493 restored quest namespace ownership for quest_setstate, quest_coroutine_yield, quest_no_send, quest_get_current_quest_index and RegisterQuestFunctionTable (08169640-08169940). The registration now defines the external symbol called by questlua.cpp startup. The updated source index separates scope-tagged matches, event macros and name-only candidates. Source guards and read-back passed. No builds or tests.

Source batch 494 restored unsigned dword arena set/score fields and wrapped signed event delays (080bb820/080bc440). Verified original arena observer-send methods are empty; ordinary duel score progression and timings matched. Item update packet, DB proto reload and combat movement/skill affect audits matched valid paths. Source guards and read-back passed. No builds or tests.

Source batch 495 restored 41 original externally named quest globals for timers, notices, warps, lookups and starting positions, retaining private helper linkage. Fixed original Boot normal/rare attribute %lu diagnostic arguments without altering 69-byte row parsing. Updated definition index reports 2621 scope-tagged candidates, 8 name-only, 10 event macro and 14 excluded entries among 2653 global method entries. Source guards and read-back passed. No builds or tests.

Source batch 496 restored signed dword leader-plus-offset coordinates in party summons (081434b0) with all 12 offsets, search order and RNG unchanged. Restored external marriage::SendLoverInfo and near_check_event (080afc90/080b05d0). Original combat movement and skill affect audit matched. Source guards and read-back passed. No builds or tests.

Source batch 497 restored the external CUBE_DATA constructor and original callable signatures for can_make_item, reward_value and remove_material (080fc180-080fc370), with corresponding character slot accessor. Duplicate ingredient totals and exchange guards remain in force. Restored original-width ApplyValue and flag diagnostic arguments in DB affect loading. Shop audit matched normal non-Monarch behavior. Source guards and read-back passed. No builds or tests.

Source batch 498 restored signed dword coordinate deltas for party ground-gold count and distribution (08181680/08181720), retaining inclusive 5000 range and recipient order. Dungeon jump and normal AddToGround matched. Source guard and read-back passed. No builds or tests.

Source batch 499 restored FN_check_item_count(CItem**, DWORD, int) and its signed dword count over all 15 slots (080fbf80). Cube recipe matching retains cumulative duplicate requirements and excludes items in exchange. Source guard and read-back passed. No builds or tests.

Source batch 500 restored original dword socket equality for PickupItem and MoveItem (0817ec10/0817e490) and unsigned dword marriage timestamps in the DB notice diagnostic. Alignment, safebox and mall valid paths matched. Source guards and read-back passed. No builds or tests.

Source structure audit after batch 500: compared all 174 distinct original DWARF compilation unit basenames with the live game/lib source tree. 169 have exact basename counterparts. event.c and event_queue.c map to event.cpp and event_queue.cpp and are present in the game project. The three remaining units are Monarch and management registration containing only Monarch bindings. No missing in-scope translation unit was found by this basename audit; file presence does not verify bodies, linkage or behavior.

Source batch 501 restored dword socket comparisons for AutoGiveItem and item-versus-prototype defaults (0817fb20/08170df0), sharing the low-dword equality used by pickup and manual stacking. Original ground timers, target display, billing and lotto valid-data paths matched. Building target packets retain deterministic zero for the original uninitialized HP byte. Source guard and read-back passed. No builds or tests.

Source batch 502 restored local FN_point_string/FN_hair_affect_string (080acd20/080ad0a0) with locale lookup on hair bonus descriptions. All 72 mapping literals matched the ELF jump table. Bounded formatting and a defined double argument for the two decimal formats replace unsafe original behavior. Campfire positions now add the float offset to the exact signed dword position before truncating (0817c000-0817c1a1). Netmarble QID13 success queues the follow-up lookup after disconnect, matching original order. Source guards and read-back passed. No builds or tests.

Source batch 503 restored external shutdown_event (080a8c20) and named SendDisconnectFunc/DisconnectFunc (080ad740/080ad6d0). The original client-set lookup is limited to the zero and minus-ten countdown branches; recipient filters and ordering match. Source guards and read-back passed. No builds or tests.

Source batch 504 restored x87 division/truncation for fishing cast direction and low-dword cast and bite delays (08083070/08082ff3). Restored original placement of the QID7 START diagnostic before validation guards; stale replies remain safely guarded. Source guards and read-back passed. No builds or tests.

Source batch 505 restored extended intermediate precision and truncation for all six party role formulas (08143910, including 08143b24). Attacker skill power 3 yields 11 rather than 12. Source guard and read-back passed. No builds or tests.

Source batch 506 restored signed dword coordinate deltas for party near-leader checks and periodic near flags (081445f6-08144607/08144140-08144155), preserving strict 5000 range and dungeon behavior. VCard SQL log now formats original unsigned dword coordinate bits (08110670). Source guards and read-back passed. No builds or tests.

Source batch 507 restored dword addition for fishing rod skill and bait socket (080831c1) and original function-local static fish-use probability table s_acc_prob (0823d960). Ordinary values and thresholds matched. Source guard and read-back passed. No builds or tests.

Source batch 508 restored parse_time_str(const char*) linkage (08147560) and the original interpolation intermediate precision before the float API boundary (081471a0). All eight case variants of time suffixes matched the original jump table. ItemLog normalizes the four unsigned dword values before text/SQL formatting (080784e0). Normal inventory move and item grant audit matched. Source guards and read-back passed. No builds or tests.

Source batch 509 restored signed dword pick experience increment before SetSocket (080ef5e7). Mining event, ore-drop RNG, original pre-fee material consumption, and mining table values matched. Source guard and read-back passed. No builds or tests.

Source batch 510 restored the packet.h owner and original names for the 10-byte dig motion packet; only the packet count narrows to a byte before the full RNG int reaches CreateMiningEvent (08130157/0813017b). CharLog now formats original unsigned dword fields (080785a0). Restored original one_argument null diagnostic location (08146f6e); quoted token behavior matched. Combat stat audit matched valid paths. Source guards and read-back passed. No builds or tests.

Source batch 511 restored original const signatures for string_dice, get_filename_ext and is_filename_ext (08146d00/08146dc0/08146df0), including const return type confirmed in DWARF. text_gauge uses original low-dword product (08147291) while preserving bounded output. CubeLog formats original unsigned dword values. Source guards and read-back passed. No builds or tests.

Source batch 512 restored FuncFindMobVictim original class scope, field order, visitor bool return and GetVictim method (0813d122/0813d223/0813d233/0813d016). Target selection remains equivalent for valid data; null sector guard retained. Source guard and read-back passed. No builds or tests.

Source batch 513 restored original unsigned dword audit formatting at GoldBarLog, LoginLog and PCBangLoginLog. The original format strings and input API remain in use. Static audit of get_time_string, first_cmd and last_cmd matched normal return strings and token handling. Source guards and read-back passed. No builds or tests.

Source batch 514 restored the unrounded motion speed quotient (0812da53), scale-before-float boundary in GetMoveSpeed (0812dbc4-0812dbdb), and retained the raw quotient float boundary in CalculateMoveDuration (0812dc67). Kept the original public float signature through a private precise helper. Attack, PK timing and audited reward paths matched for normal data. Source guards and read-back passed. No builds or tests.

Source batch 515 restored unsigned dword movement-duration storage (0812dca8), and original signed/unsigned dword arguments for movement diagnostics (0812dcc2-0812dd35). Verified the non-PC log precedes the motion clock sample and retained that order. Source guards and read-back passed. No builds or tests.

Source batch 516 restored the original fatal GetNewID precondition (08192239) in release as well as debug. Restored the external chase helper symbol (08080160) and the idle timer dword product (08080432). Ordinary movement matched; existing same-tick zero-duration protection is retained. Source guards and read-back passed. No builds or tests.

Source batch 517 restored the original half-pulse horse state timer and signed dword rider deltas and destination/midpoint coordinates (08081887-08081b70). Restored fatal upper bounds in CItem::GetValue and CItem::SetSocket in release builds, matching the original abort paths. Source guards and read-back passed. No builds or tests.

Source batch 518 restored signed dword delay normalization before event creation clamping (0804ceb0) and before callback result diagnostics and rescheduling (0804d296-0804d2b0). Public long signatures and the already verified wrapped deadline calculation are retained. Source anchors and read-back passed. No builds or tests.

Source batch 519 restored the guild privilege zero fallback via GetPrivByGuild and original empire lookup helpers (08188efd-08188f52). Missing guild privileges now participate in the maximum as zero, including when signed penalties exist. Source guard and read-back passed. No builds or tests. Staged for review: work/reward_next/apply_sync_owner_clock_precision.py and work/db_dispatch_next/apply_item_vid_counter_width.py.

Source batch 520 restored the extended clock intermediate for sync-owner initialization and the three-second comparison (08126dec-08126ef8/08130967-0813097f/081b9470), retaining float storage and the public float getter. Two existing isolated link stubs track the added getter. Item manager VID counter now wraps in 32 bits as in CreateItem (081922e0). Both staged changes from batch 519 are integrated. Source guards and read-back passed. No builds or tests.

Source batch 521 restored the comma-space separator and double-precision hours derived from signed dword remaining time in quest empire/guild privilege descriptions (08054820/08054eb0). Static audit matched ordinary item removal, wear and grid paths (08166a30/08171300/081713d0/081714d0/081716e0) and quickslot operations (08050660/08050740/08050940) in the checked branches. Source guards and read-back passed. No builds or tests.

Additional static audit after batch 521: exchange ordinary start, packet, offer, space, transfer and accept branches matched (08064310/080643e0/080647a0/08064a50/08064b80/08064c80/08064d00/08064e30/080654a0). Both trees retain a potential dangling exchange slot after synchronous VCard consumption (90008/90009); flag for separate safety review. Refine recipe initialization, lookup and refine information ordinary paths matched (0806a2c0/0806a3f0/081735e0), including the 60-byte packet. No builds or tests.

Static audit after batch 521: ordinary SetItem grid/owner/window ordering and 40/44-byte packet selection (08170fb0) and special item group RNG/reward effects (08180a10) matched. SetItem retains deliberate stale Cube selection cleanup. Party skill, heal no-op, summon and all 12 offsets, removal, loot rotation, proximity and periodic update matched on audited normal paths (081013d0/081434a0/081434b0/08135f80/082280f0/08100b60/08143870/081445d0/08141200). heart_idle verified ordinary sleep and lag cutoff order (081bc0f0). No builds or tests.

Source batch 522 restored low-dword missed-pulse multiplication, addition and increment before signed lag handling in heart_idle (081bc153/081bc24e-081bc259). Normal pulse timing and lag cutoff order are retained. Source anchors and read-back passed. No builds or tests.

Source batch 523 restored division of the full signed integer pulse and rate before rounding to the public float in thecore_time (081b9949-081b9951). Audited ordinary item drop rate/table selection, RNG order and reward routing matched (08193400/081934e0), with the current zero-divisor protection retained. Source anchor and read-back passed. No builds or tests.

Static audit after batch 523: ordinary monster dialogue and ScriptToString (08134c40 and six callers, 080527f0) matched. Audited horse stamina, health and recovery, grade, and guild mount race paths matched (080c8390/080c8480/080c84f0/080c8640/080c8700/080c8930/080c8950/080c89b0/0816e060/0816df10). No source changes or builds/tests.

Source batch 524 restored the watchdog tick counter low-dword increment before the shutdown check (081b98a8-081b98b9), preserving the volatile signed API and one read/write. Safebox ordinary routes matched (08096e70/08097090/080973b0/08100240/08100430/08100700); both trees remove the source stack on partial transfer, flagged for separate safety review. Quest drop event order and CharStone/skill-level rules matched audited ordinary paths (08194bf0/08194820/08194790). Source anchors and read-back passed. No builds or tests.

Source batch 525 restored four dword game profiler counters (08322cf8, 16 bytes) and signed dword formatting for all five idle timing values (080a1a70). Ordinary shop slots, pricing, gating, stock sale, tax, inventory and logs matched audited paths (080d33d0/080d37d0/080d2a20/080d2b10). Original high-value NPC rejection retains the already created item in both trees, flagged for separate safety review. Source anchors and read-back passed. No builds or tests.

Static audit after batch 525: ordinary Follow/Return (08132f00/08132df0), guard/monster/NPC/Santa idle (0807f470/0807fb70), entity visibility (08050ad0/08050b80/08050d30/08051540), Sync attribute event order (0812d450), walking and stamina (081341a0/08134260-081343a0) matched. Ordinary DB safebox request, distance, save/close order and password commands (08133670/081335f0/080971d0/08133ad0/080aa510/080aa690) matched, including 42-byte request and 11-byte save. Source unchanged; no builds or tests.

Static audit after batch 525: ordinary non-Monarch DB Boot metadata and initialization/finishing order match (080e83f0, corrected entry address). ReloadAdmin (080ed020) deliberately retains refresh of removed live admins. Duel/PvP invite, accept, win/revenge, death resolution, attack predicates and expiration match checked paths (0819c9b0/0819c370/0819d390/0819d490/0819dda0). GiveUp (0819d1a0) is dormant in the original, with no direct caller found. No source changes, builds or tests.

Static audit after batch 525: both SECTREE_MAP::Find overloads, the two candidate-only entries in the historical function coverage ledger, match the current source at 080b3730/080b3780. Verified exact unsigned dword key lookup, null on miss, unsigned coordinate division by 6400, low-16 packing of each quotient and delegation. This is current static source evidence independent of the historical test-mapping ledger. No builds or tests.

Source batch 526 restored dword timeval addition and carry in timeadd (081b8ca9-081b8ce4). Wrapped microseconds are interpreted as signed before normalization, and the second count wraps on carry. Source anchor and read-back passed. timediff edge comparison remains a separate follow-up. No builds or tests.

Source batch 527 restored timediff signed dword operand comparison and wrapped seconds/microseconds subtraction with borrow based on the operands (081b8c23-081b8c92). Reused the signed bit conversion in timeadd. Wider hosts now reproduce the original signed 32-bit timeval boundary in these helpers. Ordinary miscellaneous drop loader, special item group lookup and item manager teardown matched audited paths (08196870/08198c80/08191c50). Source anchors and read-back passed. No builds or tests.

Source batch 528 restored time_str normalization to the original signed dword timestamp before localtime (081b8d03-081b8d14), retaining the host time_t signature and formatting. Drop and monster-drop group parsers matched ordinary field requirements, duplicate policies, name fallback and probability/count rules (08198440/08196b40). Source anchor and read-back passed. No builds or tests.

Source batch 529 restored quest NPC lock/unlock PID ownership (08121250/081211d0) and both range checks with dword coordinate deltas and unrounded Lua double times 100 (08120f81-08120f9a/081210db-081210ee). OX timer replacements now inherit the original static phase (08322c9d), and empty attendee checks preserve existing misses (08088081). Existing null/pointer guards retained. MyShop and DB safebox-size ordinary paths matched (081741c0/08174150/081740e0/0810e110/081340d0). Source guards and read-back passed. No builds or tests.

Static audit after batch 529: npc_kill and npc_purge (08120dd0/08120e60) clear the current player quest NPC ID before killing or destroying the separately resolved NPC, matching live ordinary paths. DB privilege packet handlers and mutation leaves matched ordinary field layout, signed values, expiry, cache replacement and notices (080eb8c0/080eb940/080eb9c0/08188a90/08188640/08188950); current index guards retained. No source changes or builds/tests.

Source batch 530 restored Lua result count one without pushing a value for OX close, end, end_force, give_item and the invalid quiz path. With arguments the binding returns the last stack argument under the bundled Lua 5.0 convention. The end_oxevent callback retains its zero result. Evidence: ox-lua-return-original.txt in the building_next staging folder. Source anchors and read-back passed. No builds or tests.

Source batch 531 restored original Dungeon Lua spawn angle and distance float spills followed by extended trig-product truncation (08095d0e/08095d30/08095d47-08095d88). Witness radius 200.3 with angle/radius draws 361/995 changes the x offset from -127 to the original -128 coordinate units. Adjacent quest PID/VID paths matched (08162b40/08162c90/0815f3b0/080f90e0/08095a70/08096130). Source anchor and read-back passed. No builds or tests.

Static audit after batch 531: DB Time and clock helpers match signed dword wrap and diagnostics (080eb530/08146c40/08146c20). MapLocations matches the byte count and map list semantics of 50-byte rows (080ea260). DB P2P matches peer deduplication, phase 9 and field assignment order (080ea330). Existing packet validation retained. No source changes, builds or tests.

Source batch 532 restored overlapping OX Quiz scheduling (08088000/08088008) and the unpushed Lua return of _add_ox_quiz (080583ae). The reconstruction tracks every pending timer to retain safe Destroy cleanup and erases completed timers individually. Adjacent dungeon spawn paths and DB guild ladder/skill paths matched audited ordinary behavior. Source anchors and read-back passed. No builds or tests.

Static audit after batch 532: production main calls OXEvent_manager.Destroy after the idle loop and before quest teardown and event_destroy. Pending OX timer handles are removed when completed, or individually cancelled and cleared during Destroy. Ordinary dungeon unique HP and Lua boundary (08183f20/08094d2c), unique death ordering and elimination/regen/warp routes (081856a0/08185820/081858c0/081869c0) matched. No source changes, builds or tests.

Source batch 533 restored Lua stack push order and original qword-to-dword/byte truncation in item.select, item.select_cell and item.next_refine_vnum (080f0400/080f02d0/080f0ef0). Character null guard retained. DB wedding and guild-war reserve/bet paths matched audited ordinary behavior (080ecd90/080ece00/080ecea0/080ecf30/080eca70/080ecad0/080ecb30). Source guards and read-back passed. No builds or tests.

Source batch 534 restored signed dword Lua item socket values, flag masks and value index conversion with INT32_MIN for out-of-range numbers (080f0680/080f0760/080f0910/080f0a40). Restored original wedding notice/exit visitor names and loop structure (0816fa90/0816fac0/08170760/081707c0), retaining verified behavior. Xmas transitions, arena observer no-ops and DB guild exp/member/grade (080ea990/080eabb0/080eab30) matched audited paths. Source guards and read-back passed. No builds or tests.

Source batch 535 restored quest affect slot FISTP word then byte narrowing, signed dword values and durations, and signed conversion of remove type (08069220/08069610/080698b0/08069af0/08069480). Marriage manager constructor variants (080b0b00/080b0cc0) resolve to marriage.cpp and match singleton/member default initialization at source level. Account DB, DBManager SQL routes and DB building packet wrappers matched audited paths (08110c60/08110c70/08110cd0/08110cf0/0810c870/0810c8c0/0810c920/0810c950/0810c9d0/080ebc60/080ebcc0/080ebd20). Source guards and read-back passed. No builds or tests.

Source batch 536 restored ANI timing parser 1024-byte fgets chunks and literal space/tab/CR/LF token delimiters (08239db8), low-dword truncation/zero fallback and signed minimum (080a8182-081b9260). Both end_oxevent callbacks (080cac80/0813cad0) independently match CloseEvent and zero return. DB guild load/member/disband paths matched ordinary behavior (080ea490/080eaa30/080eaab0/080eac50/0809ac20/0809ae20/0808b360/0808b760). Source guards and read-back passed. No builds or tests.

Source batch 537 restored building Lua land coordinate read order y/x/map and signed dword conversion with INT32_MIN on invalid values (080851e9/0808520d/08085232). Other building bindings use the low dword of signed qword conversion as in the original (080852a0/080853e0/08085520/080857b0). Existing SQL result fallback retained. Arena bindings matched ordinary behavior. Source guards and read-back passed. No builds or tests.

Static audit after batch 537: CFSM::Update, IsState and GotoState match the checked 2009 transition paths (080502c0/08050310/08050330). End callback precedes rereading the pending state, pending is cleared before begin callback, active is reread before state body, and GotoState always returns true while skipping a redundant pending assignment. No source changes, builds or tests.

Source batch 538 restored original party.setf signed dword truncation and party.setqf name construction before rint and dword conversion (08146200/08146450). Cinematic bindings update the party before rereading the script and restore descriptor addresses in the original fanout diagnostics (08145930/08145770/081469a0/08146880). Oversized packet guard retained. DB LoginAlready, character delete replies and empire selection match audited ordinary paths (080e9fe0/080e78f0/080e7970/080ea140). Source guards and read-back passed. No builds or tests.

Source batch 539 restored the private ANI speed table to 64 dword entries with 32-byte race rows (080a8230/080a8270), retaining the 1000 defaults, 1500 bow default and public DWORD API. Source anchors and read-back passed. No builds or tests.

Source batch 540 restored quest game_drop_item coordinate snapshot, original numeric conversion/jitter and expiry even on failed ground placement (080b2df0). Deterministic Z and null character guard retained. Restored virtual CFSM update dispatch (0818de95-0818de9e) and original external linkage for six ANI helpers (080a7e90/080a7ef0/080a7f50/080a80c0/080a8440/080a84a0). Billing/login and character-create DB paths matched ordinary data (0810d870/0810cd80/08110960/0810cba0/0810cc10/0810da80/080e7620/080e75e0). Source guards and read-back passed. No builds or tests.

Static audit after batch 540: fishing event ordinary PID lookup, rod check, step increment/cap, handle clear, failure and bait reset match (08082f00-08082fc7); source retains a deliberate stale-event identity guard. DB SetEventFlag packet and manager routes match checked name/value widths, prior-value update, day-mode broadcast and seasonal routing (080ebbc0/081a5e60). No source changes, builds or tests.

Static audit after batch 540: marriage quest wedding entry, double-push false result, status/time and row order match checked paths (080f92a0/080f9590/080f9ca0/080fa340/080fa420/080fa670/080b0950/080b0730/0816fc10). Horse level recomputation, percentage and rider queries match ordinary input (080e3980/080e3ad0/080e3bd0/080e3d30/080e38b0), with existing zero-max guard retained. No source changes or builds/tests.

Quest DB static audit: QuestLoad and quest_login_event match ordinary qualified flag loading and one-second login retry by descriptor phase (080e9850/080e96f0). The current empty-row PID fallback is retained. No source changes, builds or tests.

Source batch 541 restored fishing::Take STOP/practice for nonpositive step, practice-only for unhandled Compute statuses, and stored socket readback for fish length consumers (080834e0/0808364e-08083662). Ordinary OreRefine checks, material/fee order and roll matched. Source guards and read-back passed. No builds or tests.

Static audit after batch 541: DB login-success/rename checked packet lengths, original logout handle and character/empire/phase update order (080e7260/080e79b0); deliberate zeroing of unused packet bytes remains. ANI original strtof, exact float-by-1000 product, low dword extraction and zero fallback match the current parser (080a8043-080a80a4). No source changes, builds or tests.

Source batch 542 restored Lua argument 2 before argument 1 and original signed dword conversion for OX Quiz and GiveItem (0813c950/0813cc60), including byte level/count and low-dword item vnum. Forked map and counters matched ordinary audited behavior. Source guards and read-back passed. No builds or tests.

Source batch 543 restored signed dword elapsed marriage time to the Lua result (080fa460-080fa474). Wedding lifecycle and bonuses matched checked ordinary cases; literal-safe shout remains. OX CheckAnswer bounds, cheer/effect dispatch and WarpToAudience matched ordinary paths (08088060/08088590). DB MoneyLog/Notice payload and routing matched (080eba40/080ec0c0/080c04e0/080c7e40), retaining short-frame bounds. Source anchor and read-back passed. No builds or tests.

Source batch 544 restored q clock/counter rounding followed by signed dword conversion (08169300/081694a0), title-before-name ownership (081690d0), and q.setstate Lua name reread and ownership after coroutine validation (08169640). Existing quest context guards retained. Source guards and read-back passed. No builds or tests.

Source batch 545 restored low-dword map selection after qword Lua truncation (080967d0), extended multiplication before pc.change_alignment truncation (0815fcaa-0815fcbc, 0.3 yields 2), and wrapped signed dword cumulative item-group weights for special/monster groups (08195f80/08196b40). Source guards and read-back passed. No builds or tests.

Static audit after batch 545: CMotion constructor variants and duration/vector/empty accessors match logical member initialization and reads (08114340/08114380/08114d10/08114d20/08114d30), using native std::string layout. Item destroy event, equipped/real-time unique expiry and ownership timer/30-byte notice match ordinary paths (08166560/08166540/08168450/08168580/08168680/08167fd0). Existing packet zero-fill retained. No source changes or builds/tests.

Source batch 546 restored extended intermediate precision and unsigned dword EXP base in pc.give_exp_perc (0816118e-081611b6), yielding 299 on base 100000 at Lua 0.3 percent as in the ELF. CMotionSet::Load dist/sec diagnostic now divides before rounding to double (081142eb-08114317). Entity initialization, PacketView and observer paths matched checked behavior. Source guards and read-back passed. No builds or tests.

Source batch 547 restored 32-bit motion key shifts and low-dword normalization at CMotionSet insertion and lookup, retaining public DWORD signatures (081142f5-08114332 and original one-word key ABI). Ground-item placement, pickup distance and equipment transitions matched audited ordinary behavior (08166d60/08166c20/08166bc0/08167870/08167a70). Source anchors and read-back passed. No builds or tests.

Source batch 548 restored position-scroll exact int32 distance comparison against the unrounded square of the stored binary32 radius (08162141/081621ce-081621e8). Radius sqrt(5) and distance squared 5 now match the original true result. Source anchor and read-back passed. No builds or tests.

Source batch 549 restored arena Lua set-count read before membership/dismount (08078d59), observer argument order map/X/Y, and unsigned dword map conversion on AddArena/AddObserver (080790c0/0807928f). Arena slot selection and duel-list shape matched ordinary data. Source guards and read-back passed. No builds or tests.

Source batch 550 restored pc.warp optional map read before IsHack and separate y/x reads for test log and warp (0815cdc0), plus right-to-left argument reads for pc.set_warp_location (0815c750). All ten teleport town records, local warp and guild spectator warp matched audited ordinary data (08163870/0822a0b4). Source guards and read-back passed. No builds or tests.

Source batch 551 restored original wrapped dword read deadlines in the standard, special and horse skill-book branches of UseItemEx (081746b0). Single clock sample and delay/locale/RNG rules retained. Source guards and read-back passed. No builds or tests.

Source batch 552 restored dword coordinate subtraction in safebox distance and signed wrapped pulse difference/product in safebox request cooldown (081335f0-08133614/081336ee-081336fd). Reused existing target-width character arithmetic. Exchange AddGold/CheckSpace/Done and safebox destruction/stacking matched audited branches. Source guards and read-back passed. No builds or tests.

Source batch 553 restored signed low-dword HP/SP recall item cost product before division (08174252-08174263/081742ef-08174300), and original HP remainder comparison and capped consumption path (0817427e-08174286). Ordinary values remain equivalent. Source guards and read-back passed. No builds or tests.

Source batch 554 restored pc.set_skill_level vnum/level reads before character resolution (08163c80) and original ore/diamond argument and character/NPC/item resolution order (081633a0/08163210). Equip refinement, skill reset, master check and inventory query matched audited ordinary paths. Source guards and read-back passed. No builds or tests.

Source batch 555 restored signed low-dword potion bonus and six instant/gradual HP/SP scaling products before division (UseItemEx 081746b0). Gradual recovery cap, branch order and item consumption retained. Source guards and read-back passed. No builds or tests.

Source batch 556 restored polymorph BookUpgrade socket 2 reread after writing socket 1 (0816f1e7), and signed dword cooldown addition in standard and scroll refine (081719e0/08172480). Normal fee paths and audited empire/name change and stat reset matched; name SQL escaping and result guards retained. Source guards and read-back passed. No builds or tests.

Source batch 557 restored dungeon getf numeric fallback after logging invalid names and setf/setqf read/context order (080934e0/08093300/080930c0). Null-name safety retained. d.join still checks party leadership before private map allocation to avoid the original unused-map leak (080963d0). Corrected batch 538 diagnostic: CINEMASEND_TRY includes DESC address, successful CINEMASEND includes only character name (082286a2-082286c4). Source guards and read-back passed. No builds or tests.

Source batch 558 restored unsigned dword cycle/current/last arithmetic in the attribute reroll cooldown (UseItemEx 081746b0), including wrapped deadline comparison, saved minute bits and signed dword diagnostics. Source guards and read-back passed. No builds or tests.

Static UDP audit after batch 558: CInputUDP::Process matches the >4-byte loop guard, packet registry and available-length checks, negative Analyze termination and consumed accounting, and logs unknown headers before dumping the complete original datagram (081401e0-08140312). Corrected stale input_udp.cpp header comment that still described CInputUDP as unresolved. No behavioral edits, builds or tests.

Source batch 559 restored same-count SetCount packet/save for present optional mix materials (UseItemEx 081746b0 subtype 23), retaining transactional validation and clearing subsequent aliases before stack deletion. Timed mob skill now applies the hit before erasing its event-map entry (08073d1a/08073d30), revalidating caster VID and event identity; collision-safe scheduling retained. UDP StateChecker count thresholds, response and failure diagnostics matched (08140050-08140182). Source guards and read-back passed. No builds or tests.

Source batch 560 restored named player/server timer duration reads before loading the timer script, including before the optional server argument (08056500/080567c0/08056d80/08056b70). Quest target lifecycle and packet builders match audited paths (08105800/08105ce0/08105f30/08104f90/08105010/08105080). Source guards and read-back passed. No builds or tests.

Source batch 561 restored signed low-dword coordinate deltas for NPC item handoff distance, retaining inclusive range 2000 (08180160). Refine/horse/fishing/quest handoff checked paths matched (08180520). Pending separate review: d.new_jump original WarpSet(newMap,arg2,arg3) order (08093ec0/08094082-080940a2) differs from functional newer WarpSet(arg2,arg3,newMap); guarded stage work/reward_next/apply_quest_dungeon_legacy_new_jump.py not applied pending callsite impact review. Source guard and read-back passed. No builds or tests.

Source batch 562 restored true defaults in dungeon potion/revive queries for absent context or invalid arguments (08094660/08094710) and wrapped dword coordinate subtraction after the original monster aggregation roll (08156c88-08156ca9). Audited metin attachment and hair item paths match. Legacy d.new_jump call-site search is still in progress (shell session 82948); original order remains staged. Source guards and read-back passed. No builds or tests.

Source batch 563 restored original d.new_jump WarpSet(newMap, arg2, arg3) argument order (08094082-080940a2; WarpSet 081328a0). The search finished with no literal d.new_jump calls in scanned .lua/.quest/.script/.txt files under both supplied trees. Compiled/external scripts were outside the search. This intentional 2009 parity restoration can break external quests expecting the newer corrected warp order. Source guard and read-back passed. No builds or tests.

Source batch 564 restored signed dword MIN and wrapped subtraction in UseArrow (08153a29-08153a45), retaining ordinary bounded count behavior and replacement ordering. Polymorph item routing, mastery duration and bonus precedence matched audited paths (08181120/0816ed70/0816ec30). Source anchor and read-back passed. No builds or tests.

Source batch 565 restored 32-bit world-to-region building coordinate subtraction and land-master center arithmetic at boot (08106a38-08106a44/0810956d-0810959d). Global quest warps now read map/destination/X/Y/delay in original order before filtered context lookup, and village diagnostics precede reads/allocation (08059330/08059e20/0805a160/08058ab0/08058e60). Source guards and read-back passed. No builds or tests.

Source batch 566 restored two dword VID identity fields and low-dword character VID lookup, preserving the public DWORD API and full ID/CRC comparison (0818c780/0818c7d0). FindEquipCell matched audited ordinary slot, class, limit, marriage and paired unique rules (08166e60). Source anchors and read-back passed. Pending staged quest state result patch work/reward_next/apply_quest_state_lookup_result.py. No builds or tests.

Source batch 567 restored get_quest_state no-PC Lua result count one without push and full quest status key in the diagnostic (08057f10). Horse offline stamina elapsed and health deadlines now use original unsigned dword arithmetic (080c84f0/080c8930/080c8950). Source guards and read-back passed. Pending staged OX attendee reward log work/building_next/apply_oxevent_attender_reward_log.py. No builds or tests.

Source batch 568 restored OXEVENT_REWARD item logging after each online attendee grant attempt (08088917-08088952), including failed grants. Restored recall position lookup before stored-position selection for VNUM 22000 and signed dword distance inputs (0817d5f0), retaining initialized recall fallback. Pending staged mob spawn precision work/reward_next/apply_quest_mob_spawn_precision.py. Source guards and read-back passed. No builds or tests.

Source batch 569 restored the global mob.spawn and mob.spawn_group float angle/radial spills before exact trig-product truncation (08053330/080538e0/0805362b/08053beb). Push the first successful spawn VID at success, independently of its numeric value, or zero when no spawn succeeded. Existing guards retained. Source anchors and read-back passed. No builds or tests.

Static audit after batch 569: dungeon_spawn_mob unlike global mob.spawn stores the first nonzero VID and pushes once after the loop (08095c67-08095c9a), matching live. Radius threshold 1.0 and zero-count default match. Source intentionally bounds negative counts, avoiding the original decrement-to-0xffffffff wrap path. Scroll refinement probability, outcome, fee/save order and socket transfer/notifications match ordinary data (08172480/08171750/08171810/081718e0); material/destination safeguards retained. No source changes, builds or tests.

Source batch 570 restored original Lua BGM/goto argument read order and dword map/coordinate conversions (08058100/0805817a/080581e0), retaining null-name safeguards. Logging/shop sale and OX/quest target reward routes matched audited ordinary behavior (080888a0/081a3f90/081a3720); SQL escaping retained. Source guards and read-back passed. No builds or tests.

Source batch 571 restored original HitOnce rejection log with no payload and three signed dword fields for success (0806acd0/0806adc9). Hit/cooldown and UseSkill field ordering matched audited paths. Client item use/drop/move/pickup packet widths and observer gating matched (080fe300/080fe320/080fe350/080fe390/080fe3d0/080fe410). Source guards and read-back passed. No builds or tests.

Source batch 572 restored dungeon elimination argument checks, dungeon lookup and signed dword delay conversion order (08093a60). Warp-at-eliminate reads optional regen filename before dungeon lookup and then y/x/map/delay as in the ELF (08093b70). Shared truncation also preserves the dword indefinite for dungeon flags. Checked regen/clear/purge/kill/exit paths matched. Source guards and read-back passed. No builds or tests.

Source batch 573 restored numeric-type-only validation and low-dword VID conversion after signed qword truncation in npc.is_near_vid (08121028/0812106e-08121076). Missing NPC/target safety and range thresholds retained. Source guard and read-back passed. No builds or tests.

Source batch 574 restored npc.open_shop signed dword conversion and original second NPC lookup on success (081206a0), preserving null guards. Applied only the shop part of staged patch because the VID part was already integrated in batch 573. Scroll detachment and refinement NPC lookup match ordinary paths (08173a00/081365b0). Source guards and read-back passed. No builds or tests.

Source batch 575 restored fatal CEntity destruction precondition in release as well as debug (081ac89f/081ac8d6). CEntity Destroy and type leaves match target (081acb50/081acb70/081acb80/081acb90). Updated stale entity file header. Quest-item callbacks and arena/special shop restrictions matched ordinary audited paths (081746b0/081a3da0). Pending staged Cube NPC distance patch work/building_next/apply_cube_npc_distance_width.py. Source guards and read-back passed. No builds or tests.

Source batch 576 restored signed dword Cube NPC coordinate deltas (080fc480-080fc4b0) with existing craft distance/reward guards retained. Safebox Add/Remove/Move diagnostics now pass original signed dword positions/counts (08096e70/08097090/080973b0). Safebox/mall set packets matched 40-byte serialization. Pending staged PC quest item reward arguments work/reward_next/apply_quest_pc_item_reward_args.py. Source guards and read-back passed. No builds or tests.

Source batch 577 restored signed dword reward VNUM conversion, positive count reread and original label ordering for PC give/give-or-drop quest bindings (0815ddb0/0815e260). Gold-bar null guard retained. Prototype VNUM summary now formats original signed dword bits (08191cf0). Equipment inspection matched original populated packet layout; zero-fill of unused entries retained. Source guards and read-back passed. No builds or tests.

Source batch 578 restored low-dword X zero test and signed dword coordinates for war flag and flag-base spawning (0816bbb0/0816bd40). Ordinary war-map score/count/lifecycle, forked death counters/parser/classification and quest target routes matched audited behavior (080a5160-080a5470/080a4430/080a5b70-080a5f00/081899b0-0818a1c0). Source guards and read-back passed. No builds or tests.

Source batch 579 restored fatal war-map team index guards in AddFlagBase/AddFlag/RemoveFlag/IsFlagOnBase/GetGuildID in release builds as in the original assert paths (0816bbb0/0816bd40/0816bee0/0816bf80/0816a600). Valid index behavior and ended-map early returns retained. Shop opening/stock update serialization matched (080d4590/080d49a0), retaining zero-fill for sold-slot fields. Source anchors and read-back passed. No builds or tests.

Source batch 580 restored dword war-map pulse products and signed map normalization (0816ca20), game.set_event_flag and game.set_safebox_level signed dword conversions (080b28f0/080b2c00), and unsigned dword wallet bits in exchange overflow diagnostics (080fec80). Safebox descriptor/capacity guards and exchange/MyShop packet layouts retained. Source guards and read-back passed. No builds or tests.

Source batch 581 restored fatal null AddToStateList precondition in release as in the original assert path (0818e192/0818e222). Verified existing duplicate rejection and unique insertion order. Source anchor and read-back passed. No builds or tests.

Source batch 582 restored quest target event raw dword VID lookup and wrapped coordinate deltas (0810577x/0810578a-081057b6), plus unsigned dword wallet formatting in shop sale overflow log (080d2b10). Source guards and read-back passed. Pending staged polymorph book Lua arguments work/reward_next/apply_quest_polymorph_book_args.py. No builds or tests.

Source batch 583 restored pc.give_polymorph_book slot reads 4/3/2/1 and signed word then byte conversion of book level/level limit (08163d50/08163e6d/08163e9b). Shared the equivalent existing empire word conversion. Adjacent mount/book upgrade ordinary paths matched. Source guards and read-back passed. No builds or tests.

Source batch 584 restored source stack count update before destination socket copy in MoveItem split (0817e490), retaining preallocation and rollback on failed placement. PC economy/equipment Lua ordinary paths matched audited cases (0815f780/0815fd10/0815d670/08161600/081627c0/0815dc90/08162230/08162360/0815db50), including chance 100 RNG bypass and refine allocation failure return. Source guard and read-back passed. No builds or tests.

Source batch 585 restored ground-drop split quickslot clear, source debit and source save flush before destination socket copy (0817ddf0). Existing allocation/preflight/rollback guards retained. Guild Lua betting/reservation/warp paths matched (080d1420/080d1840/080d17c0/080d1c30/080d1640/080d1190), as did war map opponent lookup (0816a690). Source guard and read-back passed. No builds or tests.

Source batch 586 restored wrapped dword elapsed/remaining arithmetic for delayed recall (081743c4/081743fa) and original signed coordinate log plus socket reread before immediate WarpSet (08173f83/08173f8d). Refine fees, royalty, detachment and ordinary recall map gate matched. Source guards and read-back passed. No builds or tests.

Source batch 587 restored signed byte quiz level display and unsigned dword VID/zero formatting in OX cheer packets (08087aab/080882c3/081f7d84), retaining deterministic padding. Checked CWarMap GetTeamIndex (0816a5c0), quest manager event dispatch and inventory VNUM helpers matched ordinary inputs. Current reload/logout safety retained; original shared blend orphan noted separately. Source guards and read-back passed. No builds or tests.

Static audit after batch 587: DISTANCE_APPROX 08051610 matches signed comparison after wrapped negation, low dword weighted sum 246*maximum + 102*minimum and arithmetic right shift by eight, including INT_MIN negation wrap. Item prototype name lookup first prefix/order and unchanged output on miss match (08193120/081931b0); reverse refine, scroll detachment and gold sharing match audited normal paths (08198c30/08173a00/0817e9d0). No source changes, builds or tests.

Quest NPC dispatcher static audit: Login then Letter order and return, button/info, cached ExecuteEventScript event assignment, target prefix/verb/condition first match, and generated chat selection match checked paths (081b1970/081b2130/081b1f40/081b2a10/081b13a0/081b22e0). No source changes, builds or tests.

Source batch 588 restored signed dword wrap in player three-way war death decrement (080a5172/0814b233) and killer count increment (0814c0e8), using existing arithmetic helpers. Forked Lua map/path numeric handling matched inspected paths. Source guards and read-back passed. No builds or tests.

Source batch 589 restored dword wrap for dungeon live monster count and mob/stone kill counters (08131a9a/08131b48/08184ff0), retaining log and elimination order. DB safebox/mall reply paths (080e9d40/080e9f60/080e9e70/080e9ea0/080e9ef0), deferred item persistence (081928a0/081928f0/08192980/08192b80/08191c60) and quest flag persistence/rewards (08060770/08061140) matched audited ordinary behavior. Source guards and read-back passed. No builds or tests.

Source batch 590 restored fixed dword quest NPC identity/owner storage and low-dword setter (08134f39), retaining DWORD accessor API. Item manager RemoveItem/DestroyItem matched audited detach, quickslot, delayed-save, last-owner DB delete and map erase order (08192c20/08192da0). Source anchors and read-back passed. No builds or tests.

Source batch 591 restored signed dword Lua attendee count and wrapped end-event pulse delay in OX bindings (0813ca7f/0813cb73). Quest coroutine cache, selection/resume and script packet handling matched audited paths (081a7380/081a2190/081a22a0/081a23f0/0805c1d0/0805cd10/081a4f50). Existing native pointer cache representation retained. Source guards and read-back passed. No builds or tests.

Source batch 592 restored GetQuestStateIndex rint then signed dword conversion with INT_MIN for unrepresentable or NaN values (081a5240/081a5382). Item prototype initialization/rebind matched ordinary paths (08191cf0); existing snapshot before vector replacement retained. Pending staged FishingPractice socket-width/reread patch work/building_next/apply_fishing_practice_socket_width.py. Source guards and read-back passed. No builds or tests.

Source batch 593 restored signed dword socket comparison and wrapped increment in FishingPractice (08082cf0), rereading socket and max after progress chat for completion (08082e19-08082e2a). MyShop price response/order (080ecfc0/081740e0), item ID allocation (081921c0/08192250), and global quest map/character helper paths matched audited data. Source guards and read-back passed. No builds or tests.

Source batch 594 restored unsigned low-dword NPC shop selector in StartShopping (080d2781), completing the signed Lua conversion to unsigned shop-ID lookup boundary from batch 574. Zero still selects by NPC race. Audited guild Lua war betting/reservation behavior matched. Source guard and read-back passed. No builds or tests.

Source batch 595 restored SetSkillNextReadTime(DWORD, int) as named by original ELF (0806acb0), with direct dword store. Existing skill array bounds are retained; all current production callers already supply signed dword values. Audited ordinary skill-book, sector lifecycle, party quest, horse quest and safebox resize paths matched. Source anchors and read-back passed. No builds or tests.

Source batch 596 restored the signed result of the original 32-bit defense-grade subtraction in CDungeon::UniqueSetDefGrade (08183058). Guarded application and read-back passed. Character DB create/delete reply packet layouts and mutation order, wedding and PvP lifecycle paths, and dungeon jump/exit/count bindings matched audited valid inputs. Existing coordinate preflight and literal wedding command formatting retained. No builds or tests.

Static audit after batch 596: original GetSkillNextReadTime (0806ac90) declares return type time_t (DWARF 0x3d03e1 -> typedef 0x3b158b), confirming the current declaration. Six DB party handlers 080eb290 through 080eb4b0 and quest teleport, lottery, rename and event flag paths matched on inspected valid inputs. Marked 11 rows in work/source-definition-index/static-audit-candidates.tsv to avoid re-audit. No source edits, builds or tests.

Source batch 597 restored low-dword save/ping config cycle products (080d9410/080d9446). Original config token references were enumerated from .rodata, and the selected products were verified by disassembly. Billing expiry/repair and affect-load DB handlers matched inspected valid records. Source anchors and read-back passed. No builds or tests.

Source batch 598 restored ComputePoints then SkillLevelPacket at the end of ClearSkill and ClearSubSkill with initialized skill data (080746e3/080746eb and 080748d4/080748dc). Original Lua wrappers dispatch directly to these methods. Existing null guards retained. Source guards and read-back passed. No builds or tests.

Source batch 599 restored x87 signed-dword truncation including INT_MIN indefinite in ComputePassiveSkill (0806d133-0806d14b), reusing TruncateSkill. ClearSkill and ClearSubSkill now preserve original wrapped dword point deltas and read order (080743c0-080743de/080748f2-080748f9). Parallel static checks matched inspected DB building replies, aggro/ranger/stat reset, resistance, character update/reward rates and exchange lifecycle. Source anchors and read-back passed. No builds or tests.

Source batch 600 restored descriptor reread after the skill level copy (0806aff2-0806b00b) and asserted the original 0x5fb wire size. SetSkillLevel thresholds and ResetOneSkill refund/reset/log/refresh sequence matched audited inputs. P2P BlockChat, ForkedMapInfo and LoginPing matched inspected valid payloads. Source anchors and read-back passed. No builds or tests.

Static gap audit after batch 600: original DB dispatch adds only excluded Monarch IDs a0/a1/a2/a3/a5/a6/a7/a8/a9/aa; P2P case sets match (1-23 and 25). Quest strong-symbol gaps resolve to EVENTFUNC definitions or excluded Monarch modules. NoticeInMap and castle helper gaps are Monarch-only by direct callers. The strong free-symbol scan surfaced do_hammer_of_tor (080a8790), which sends DB opcode 0x72 with a money payload and special source-IP handling, and do_mhh (080c78b0), which writes to address zero. Leave both out of the live source. Remaining free-symbol candidates need macro and callsite classification before treating them as missing. No source changes, builds or tests.

Source batch 601 restored the file-local guild deposit cap helper (080fd750, active call at 081015be), named GuildDepositLimit to avoid the reserved original identifier. GM OX close now uses the original low-dword five-second pulse product (080cadc9) like the existing Lua close path. Original global and quest-namespaced end_oxevent symbols are both defined. DB boot, item-load and quest-load call targets had live definitions. Source guards and read-back passed. No builds or tests.

Source batch 602 restored guild skill SP cost and cooldown x87 signed-dword truncation (0808eb03/0808ebe3) and wrapped power debit (0808ecb4/0808ed33). Current ordinary path order retained. DB map discovery/peer registration and hair, polymorph, recall and horse status matched inspected regular paths. Source anchors and read-back passed. No builds or tests.

Source batch 603 restored signed comparison after wrapped dword guild power addition (0808f1bc-0808f1d0) and original dword refill subtraction on guild level-up (0808f326). Save and packet order matched and was preserved, as were guild level bounds. DB full-frame accounting, PC quest character change/start/block/saved exit and shop paths matched inspected normal traffic. The safer DB full-header guard is retained. Source guards and read-back passed. No builds or tests.

Source batch 604 restored guild skill point read before copying levels (0808cdba-0808cddd), and player ID rereads before the contribution save and after it for the DB packet (0808d9fb/0808da1a). Guild skill level-up and experience contribution packet sequences otherwise matched audited data. Parallel equipment, dungeon unique and DB privilege/duplicate login paths matched inspected inputs. Existing swap recovery and bounded login copy retained. Source anchors and read-back passed. No builds or tests.

Source batch 605 restored direct progression to the guild EXP update packet after a negative adjustment exceeds stored EXP (0808f4c2-0808f4db). Existing guild level bounds still apply. Parallel ranged action, billing/voucher and quest context/item/highscore audits found no new ordinary-data differences. Source guard and read-back passed. No builds or tests.

Source batch 606 restored buffered header/count and 4+25+51 guild comment field writes, flushing the final content through DESC::Packet (0808e165/0808e180/0808e236/0808e268/0808e2cb/0808e2f6). Empty results still flush promptly and bounded SQL/row handling remains. Guild DB state handlers matched audited valid records. Source anchors and read-back passed. No builds or tests.

Source batch 607 restored bounded string copy for the guild grade-name notification (0808d16c), retaining deterministic zero padding for the local update whose original strcpy was at 0808d437. Renaming to a shorter name no longer copies stale bytes past its terminator. Original grade auth notification width and grade membership DB updates matched. Source guard and read-back passed. No builds or tests.

Source batch 608 restored header/PID buffering and final value send for guild member grade and general notifications (0808e53c/554/56c, 0808e64c/664/67c), preserving subsequent save and DB relay. DB grade SQL and client broadcasts matched inspected valid data; quest journal packet assembly also matched. Source anchors and read-back passed. No builds or tests.

Source batch 609 restored signed dword LogoutMember diagnostic PID (0808bc86), avoiding host-width variadic mismatch. Guild remote presence mutation and notifications at 0808b930/0808bb50 and local logout flow matched inspected normal paths. DB country range/exception updates matched. Source guard and read-back passed. No builds or tests.

Source batch 610 restored separate P2P guild-chat header (6 bytes) and payload (92 bytes) broadcasts after the local chat (0808f842-0808f8bb). Existing zero padding and 85-byte text cap retained. Receiver waits for full 98-byte frame and reads text at offset 12. Parallel P2P session, NPC click and quest kill dispatch audits matched normal data. Source guard and read-back passed. No builds or tests.

Source batch 611 restored two dword guild invitation event fields (0809037f/08090383) and wrapped ten-second pulse product (080903b5-080903b8). Kept allocation for both fields and safe guild-name logging; ELF requests four bytes then writes two dwords and passes the guild object to percent-s. P2P transfer and dungeon spawn normal paths matched audited data. Source anchors and read-back passed. No builds or tests.

Source batch 612 restored signed dword elapsed comparisons for guild withdrawal/disband penalties (0809096a/0809098b and 080909e4/08090a02), keeping separate clock reads. DB empire select/rename, NPC idle/escape, and quest timer callbacks matched inspected normal inputs. Source anchors and read-back passed. No builds or tests.

Source batch 613 restored original guild invitation TEMP_BUFFER sequence (4-byte header, 4-byte ID, 13-byte name) and descriptor reread after assembly (08090423-080904af). Existing descriptor and timer allocation guards retained. InviteAccept cancellation, erasure and revalidation matched. DB guild war switch/score and equipment/marriage bonus audits matched inspected normal data. Source guard and read-back passed. No builds or tests.

Source batch 614 restored explicit guild grade-list buffer size before payload lookup (0808be80/0808be8a). Guild info 4+35-byte sequence and member bonus bounds matched. Numeric ELF comparison work/building_next/audit_numeric_game_constants.py matched 49 arrays, 67,076 values including 1,540 gameplay values and 65,536 sequence bytes. DB war reservation and quest journal item-selection paths matched. Source guard and read-back passed. No builds or tests.

Source batch 615 restored original TEMP_BUFFER write sequence for guild list rows: packed 13-byte member fields, name bytes, then padding (0808bfb9/0808bfdf/0808c002 and full-list analog). Both list sends read size before data. Retained bounded names and zero padding. DB add/member updates and supported quest registration/event key order matched. Source anchors and read-back passed. No builds or tests.

Source batch 616 restored read timing for the 16-bit current/max guild power packet fields (0808f06f/0808f086) and size-before-data order for member removal (0808bd74/0808bd7e). DB login outcome routes matched. Separate ELF record scan matched 1,095 fields across 12 gameplay tables (work/building_next/audit_struct_game_constants.py). Source anchors and read-back passed. No builds or tests.

Source batch 617 restored explicit signed dword withdrawal timestamp read before flag-key construction during guild disband (0808db0e-0808db41). Guild member removal and DB add/remove request paths matched inspected normal data. Character routing fallback and quest reward queue/PC predicates also matched audited inputs. Source guard and read-back passed. No builds or tests.

Source batch 618 restored buffered guild creation name header and ID followed by name flush (FGuildNameSender 08090c0d/08090c25/08090c3d). Guild create SQL, skill synchronization and master add order matched audited normal data; SQL safeguards retained. Quest rod and pick refinement matched. Source guard and read-back passed. No builds or tests.

Source batch 619 restored the emotion_types record table float extra_delay field and final newline sentinel (ELF 0823ea40), with unchanged command selection because the original reads the other fields only. LoadGuildData SQL conversions and polymorph book paths matched inspected valid data. Source guard and read-back passed. No builds or tests.

Source batch 620 restored original signed dword emote distance deltas, reading Y before X (080988c6-080988e3), and packet size before data (080989dc/080989e6). DB login outcome and dungeon purge/kill paths matched inspected valid data. Source anchors and read-back passed. No builds or tests.

Source batch 621 restored unsigned dword emote consent keys and packet VIDs with victim/actor reads at formatting (08098639/08098684 and 08098934/0809894c). Static command table audit checked 201 live rows against ELF; differences were excluded Monarch and deliberately disabled debug/crash commands. Source anchors and read-back passed. No builds or tests.

Source batch 622 restored do_emotion command-name lookup through the live command table, matching local and client names until the newline sentinel (0809877c-080987f5); restored missing-emotion log (08098cc2). Current invalid command index guard retained. Original nine-byte chat header verified and deterministic empire byte retained. All 21 emote data rows matched the original. Source anchors and read-back passed. No builds or tests.

Source batch 623 restored global CHARACTER_CanEmotion(CHARACTER&) symbol (080986b0); wedding map and unique item checks matched. Static comparison confirmed all 25 active command-to-emote mappings after batch 622. DB quest hydration and deferred login and quest button/info paths matched audited inputs. Source guards and read-back passed. No builds or tests.

Source batch 624 restored early GM check and bounded GM-only argument copy before dollar escaping (08089a76/08089ebb-08089edf). Exact do_cmd and prefix matching confirmed. Retained empty-token rejection and safe dollar escaping at buffer boundary. DB item hydration, NPC identity and item lookup/allocator paths matched audited data. Source anchor and read-back passed. No builds or tests.

Source batch 625 restored branch-local GM reads in command privilege checks, skipping the second read for ordinary zero-privilege commands (08089cae-08089cbc), and original unknown-position diagnostic function/line (08089c08-08089c18). Hong Kong/NewCIBN command thresholds checked. DB building object and war scoring paths matched audited data. Source guards and read-back passed. No builds or tests.

Source batch 626 restored BroadcastNotice five-byte header write followed by the terminated text, and size-before-data before P2P send (080c069a/080c06b0/080c06b8/080c06c2). Local notice still follows P2P. DB setup/bootstrap and quest special item rewards matched inspected normal paths. Source guards and read-back passed. No builds or tests.

Source batch 627 restored dword map-ID equality in map notice recipient filtering (080c7edd), preserving descriptor and literal formatting guards. Local and map chat modes matched. Command timer/observer/equipment and DB writer framing matched inspected normal inputs. Source guard and read-back passed. No builds or tests.

Source batch 628 restored signed dword base division and wrapped addition/scaling for numeric GM goto (080be857-080be8de); signed decimal notice uses the original dword values. Original named goto success-to-syntax-message branch at 080be740 confirmed and retained. DB connector lifecycle matched audited paths. Source guard and read-back passed. No builds or tests.

Source batch 629 restored 64-byte cap on pipe-delimited intermediate goto aliases (080be4c0/080be4cf) and signed dword named-warp scaling (080be5f8-080be635). Kept bounded alias comparison rather than original unterminated intermediate copies and unbounded final strcpy. DB billing/block exception and quest NPC handler selection matched inspected paths. Source guards and read-back passed. No builds or tests.

Source batch 630 restored original MINMAX call for the nine GM event rate flags (080c16ea); original literals and 0..1000 bounds matched. DB flag application, Xmas flag transitions and six firework coordinates matched original data. Source guard and read-back passed. No builds or tests.

Source batch 631 restored signed dword division for local-player warp coordinates and wrapped dword scaling before notice/WarpSet (080be9f9-080bea7e). Retained numeric input range guard; original parses optional z but does not pass it to WarpSet. Peer map-center route and DB time synchronization matched inspected paths. Source guards and read-back passed. No builds or tests.

Source batch 632 restored local GM transfer reads in map/Y/X order with signed dword coordinates (080be13a-080be152). Remote 34-byte packet and quest map-local warps matched inspected data. Source guard and read-back passed. No builds or tests.

Source batch 633 restored modulo-dword GM mob random rectangle and group 500-unit rectangle arithmetic (080bf418-080bf4b6, 080bf060/080bf180). Original RNG order and post-spawn flags retained. DB result dispatch and safebox size load matched inspected paths. Source guards and read-back passed. No builds or tests.

Source batch 634 restored signed low-dword Y-before-X distance deltas for GM purge and weaken (080c8139-080c814d, 080c8212-080c8227), and wrapped HP delta toward 10 (080c8259). Existing target filters and distance gates retained. DB greeting and quest experience grants matched ordinary data. Source guards and read-back passed. No builds or tests.

Source batch 635 restored item_purge original int slot loop 0..108 and per-present-item manager lookup after GetItem (080bfec0-080bff1d); remove then quickslot order confirmed. GM item count uses original MINMAX (080bf033). Quest confirmation, movement sync and outbound DB logout matched inspected inputs. Source guards and read-back passed. No builds or tests.

Source batch 636 restored GM stat signed dword cap arithmetic, capped-branch reread, and live point reread after SetRealPoint (080c6c4b-080c6d12). Original point notification flags made explicit; level command uses original MINMAX (080c3620). DB character save and quest target runtime matched inspected paths. Source guards and read-back passed. No builds or tests.

Source batch 637 restored advance atoi/MINMAX sequence (080c0fae-080c0fc7) and skill-group default zero before atoi for empty text (080c328c-080c32da). Reset/clear sequences matched. Affect persistence/cleanup and multi-root quest NPC loading matched inspected paths. Source guards and read-back passed. No builds or tests.

Source batch 638 restored atoi/unsigned dword numeric GM skill lookup (080c30d3/080c2f01) and zero default before atoi for empty set_skill_point input, with signed dword real-point reread (080c31d9-080c323b). DB event flag request and periodic affects matched inspected normal paths. Kept explicit cancellation of duplicate server timers because original can leave an untracked callback (081a7c70). Source guards and read-back passed. No builds or tests.

Source batch 639 restored GM set value rereads for money logging, application, and confirmation (080c0c89/080c0cba/080c0cf6), and wrapped caller-alignment subtraction (080c0e1d-080c0e3e). All nine original field names verified. Recovery, grandmaster skill training and safebox checkout DB write matched inspected normal data. Source guard and read-back passed. No builds or tests.

Source batch 640 restored original signed dword type/value/duration and unsigned flag in GM affect diagnostics (080c64e3), plus point-before-type atoi order and low-dword lookup (080c6509/080c6514). Existing failed-removal loop guard retained. Source guards and read-back passed. No builds or tests.

Source batch 641 restored original GM state diagnostic coordinate reads Y/Z/X (080c0067-080c006d), map/Y/X dword sector lookup (080c00c6-080c00f2), and fresh X/Y reads for attribute lookup (080c0121/080c0124). User count DB update and cross-quest state setter matched audited inputs. Player shop retains wide overflow-safe price totals. Source guard and read-back passed. No builds or tests.

Source batch 642 restored separate DBPacketHeader and raw Packet calls for GM flush (080c794d/080c796d), retaining the fixed four-byte PID and null descriptor check. Billing queue transitions, quest level-up/dismount hooks, and shop buy/sell ordinary paths matched audited data. Source guard and read-back passed. No builds or tests.

Source batch 643 restored bound-character checks after descriptor lookup in GM dc and kill (080c0974-080c097d and 080c0a50), preserving original missing-player paths and destroy/death calls. Source guards and read-back passed. No builds or tests.

Source batch 644 restored refinable helmet acceptance (armor subtype 1) at armor smiths 20045 and 20075 alongside subtypes 0/2 (081802f0-08180339). Remaining NPC item handoff routing and DB reload requests matched inspected valid paths. Source guard and read-back passed. No builds or tests.

Source batch 645 restored contiguous 40-byte refine preview material copy (08173752-0817376a), keeping tower zero materials and 60-byte packet. Client refine routing, quest refine replacements and normal/downgrade persistence matched inspected valid paths. Destination preflight remains to preserve the old item on placement failure. Source guard and read-back passed. No builds or tests.

Source batch 646 restored original dword refine recipe key in lookup/log and end comparison after logging (0806a3f9-0806a45b). Initialize 53-byte stride and first-wins duplicate handling match. DB ELF 0806f85c-0806f8a9 confirms original and live five-nonzero-material row retains count zero. No change to DB. Source guard and read-back passed. No builds or tests.

Source batch 647 restored signed share calculation before GetRefineGuild in PayRefineFee (08136727-0813673b). Guild discount, foreign empire multiplier and deposit-before-wallet deduction matched original. DB shop ingest and outer item use matched inspected normal data. Ownership revalidation after quest callbacks retained. Source guard and read-back passed. No builds or tests.

Source batch 648 restored low-dword cost negation in standard and scroll refine money logs (08171ddc, 08172097, 081728a9, 08172b95/08172de1). Tower quest flag consumption and nonzero free-refine material skip matched original. DB banword/attribute ingest and quest guild-war entry matched inspected data. Source guard and read-back passed. No builds or tests.

Source batch 649 restored global pointer signatures for NotifyRefineSuccess/Fail (08171810/081718e0), and original log input read sequence with dword player/item IDs. Client command strings and success/failure order matched. Rare attribute DB load, inventory swap and quest appearance/skill group matched inspected normal inputs. Source guards and read-back passed. No builds or tests.

Source batch 650 restored global TransformRefineItem(CItem*, CItem*) and signed dword socket interpretation/copy (08171750-08171808). Accessory expiry setup, gem compaction and final attribute copy retained. DB skill loader and dungeon party joins matched audited valid paths. Source guards and read-back passed. No builds or tests.

Source batch 651 restored original DB mob damage multiplier atof-to-float rounding (DB ELF 0806ff90/0806ff9b). Gameserver attribute transfer matches the original 21-byte copy then destination Save (081877c0/08187900); asserted the original size. Accessory decay and quest guild/storage prompts matched inspected paths. Source guards and read-back passed. No builds or tests.

Source batch 652 restored all three building rotation conversions in DB InitializeObjectTable using atof followed by float storage (DB ELF 08073400-08073421). Source guards and read-back passed. No builds or tests.

Static audit after batch 652: repository-local path resolution verified with project_paths.py. The 2009 ELF matches current building and DB boot loaders, FSM state transitions, mob damage multiplier rounding in melee/arrow damage, wedding lifecycle, and the reviewed arena/party placeholder bodies. The dormant profiled damage routine is already present in char_battle.cpp from batch 487. Prioritize original symbols demonstrably absent from current definitions before spending credits on broad re-audits. No builds or tests.

Source batch 653 restored the original dormant DB packet statistics helpers Add/Reset (0805c340/0805c3b0) and both global instances. No live callers were added. CGrid::Print now explicitly interprets cell bytes as signed, matching ELF movsbl at 081f512a. The non-Monarch dungeon and quest Lua definition scan found no missing bodies. Source guards and read-back passed. No builds or tests.

Source batch 654 restored the DB test-server item-cache flush diagnostic (08059273-080592a0) and explicit 32-bit unsigned arithmetic for number_ex (081b92e9-081b92ff). The full signed interval uses the RNG draw directly to avoid modulo zero. Static comparisons of arena and guild war branches and the resolved numeric gameplay tables found no further deltas. Source guards and read-back passed. No builds or tests.

Source batch 655 restored the original x87 intermediate arithmetic in gauss_random_extended (08147492-0814749f and 08147514-0814754b). The fresh result retains its float rounding and the cached path retains the extended arithmetic until its double API boundary. Static audits matched unique item timers, DB item ownership and six mixed gameplay tables. Source guards and read-back passed. No builds or tests.

Source batch 656 restored signed 32-bit microsecond inputs in get_dword_time and get_float_time_extended (081b9487/081b94ca) and unsigned 32-bit DB wedding deadline arithmetic and comparisons (DB ELF 0808e4f4/0808e787/0808e9f7/0808eb1e). Reviewed movement timing, Chain Lightning, Eunhyung and skill cooldown matched inspected original paths. Source guards and read-back passed. No builds or tests.

Source batch 657 restored wrapped 32-bit year/day arithmetic in tm_calc (081b913d/081b915e/081b91ae/081b91c7/081b9210) and the original fishing timing-roll-before-table-read order (08083253-08083260), retaining bounds checks. Reviewed wedding event delays, ordinary DB privilege and reservation deadlines, and mining leaves matched. Source guards and read-back passed. No builds or tests.

Source batch 658 restored original highest_array_element value caching and single load per visited entry (081471ce-081471e9), preserving last-index ties. Restored fishing length range reads before the ordinary and rare RNG draws (08083356-08083362/0808344b-08083486), with the final ordinary minimum read preserved. Quest rewards and ordinary DB player persistence matched audited paths. Source guards and read-back passed. No builds or tests.

Source batch 659 retained extended precision across the cached Gaussian return into the item bonus roll (08147492/0814749f/0816e8aa-0816e8cb), restored signed dword x87 truncation for DB guild war power and handicap (0807f6c8/0807f8db/0807f9ea), and restored fishing length division before the float display rounding (0808391c-08083935). Santa reward counters matched audited paths. Source guards and read-back passed. No builds or tests.

Source batch 660 restored ordered logarithm evaluation and extended division in CPoly::Eval (DB ELF 080c0a87-080c0ab0), and the guild war payout multiplication using the stored double ratio and x87-width intermediate where supported (DB ELF 08081a5a-08081ac1). Reviewed ordinary consumable effects, stone rewards and fishing outcomes matched. Source guards and read-back passed. No builds or tests.

Source batch 661 restored extended intermediate division for CSC, SEC and COT in CPoly::Eval before the double stack store (DB ELF 080c09ee/080c0a52). Guild ranking tie handling and quest kill/login event dispatch matched audited original paths. Source guards and read-back passed. No builds or tests.

Source batch 662 restored x87-width intermediate arithmetic for four CPoly operators (DB ELF 080c08c0/080c08d1/080c0914/080c0944), wrapped guild ladder point addition with negative clamp (0807e544-0807e54b), and the gradual potion fullness comparison and HP/SP read order (08178dcc-08178f47). Quest item grant paths matched the original. Source guards and read-back passed. No builds or tests.

Source batch 663 restored uint32 storage for DB guild war and bet scores, matching the original ADD width and subsequent comparisons (0807dde0-0807dde2). Reviewed immediate potions and dungeon counters/completion matched ordinary gameplay. Source guards and read-back passed. No builds or tests.

Source batch 664 restored extended division of the unsigned dword guild bet amounts before the binary64 ratio store (DB ELF 08081a20-08081a5a). Quest state transition ordering matched inspected paths. Source guard and read-back passed. No builds or tests.

Source batch 665 restored wrapped 32-bit handicap score subtraction before signed comparison (DB 080817b1/08081be9) and the Metin stone drop chance width and RNG order (081942d3-08194308). Source guards and read-back passed. No builds or tests.

Source batch 666 restored scaled chance calculation before the random draw for generic, glove and miscellaneous drops (081939db-08193a17, 08193e21-08193e5d, 081943d0-0819440b). Guild bet/refund and quest party paths matched audited behavior. Source guards and read-back passed. No builds or tests.

Source batch 667 restored the mob kill-group drop chance calculation before RNG (08194523-08194554) and explicit low-dword addition of rare drop rate bonuses (08194730/0819473f). Special item group rewards and DB item socket persistence matched audited paths. Source guards and read-back passed. No builds or tests.

Source batch 668 restored the original boss-table decision before level delta evaluation and fresh mob rank read for common drops (081934f2-0819356a, 0819474e-08194783). Retained existing stone snapshot for the later drop branch. GiveExp matched the audited bonus sequence. Source guard and read-back passed. No builds or tests.

Source batch 669 restored wrapped signed 32-bit drop chance arithmetic for Christmas socks and moon drops, including chance evaluation before the moon draw (0819581f-08195859/0819596a-0819597e). Source guards and read-back passed. No builds or tests.

Source batch 670 restored Mars and three-skill drop arithmetic and read order (08195115-08195186/0819530c-08195329) and extended guild ladder percentage multiplication before truncation (DB ELF 0807c5a9-0807c5e2). Seasonal item tables matched. Source guards and read-back passed. No builds or tests.

Source batch 671 restored event-local rank and locale reads for seasonal drops and the original DB guild war countdown clock width and minute conversion (0807ffa9-0807ffde). Source guards and read-back passed. No builds or tests.

Source batch 672 restored the live killer level read for Mars drop eligibility after earlier event processing (0819506f-0819507c). Quest scheduler timing matched inspected paths. Source guards and read-back passed. No builds or tests.

Source batch 673 restored signed 32-bit DB guild war, wait-start and skill deadline storage and wrap (0807b7e1/0807b8c4/0807b9ca). Includes original signed epoch boundary behavior. Source guards and read-back passed. No builds or tests.

Source batch 674 restored original character-stone drop diagnostic level snapshots and name lookup order (081948a0-081948d2/0819498f-081949c7). Drop selection and rates matched. DB marriage writes matched audited current-day data. No builds or tests.

Source batch 675 restored original live victim race, item and stone type reads in ordinary drops (08193911/08193add/08193b2f/08193d35/08193f27/08193f37/0819438c/08194428). Game utility definitions were verified against ELF names and a stale file comment was corrected. Monster idle movement and DB party state matched audited paths. No builds or tests.

Source batch 676 restored independent row copies for the generic, level and glove drop loops (081939b0/08193bf4/08193def). DB building updates and monster victim selection matched audited paths. No builds or tests.

Source batch 677 restored the original null mob kill-drop group check before dereference and continued level-drop processing (08193b0a-08193b12). Monster aggro branches matched the inspected original. Source guard and read-back passed. No builds or tests.

Source batch 678 restored missing-set and successful-flush DB diagnostics for FlushItemCacheSet (08058a00-08058a2c/08058a94). Persistence ordering matched. No builds or tests.

Source batch 679 restored queued and direct DB item cache save diagnostics (08058bf7/08058cc0). Mob skill timing matched ordinary paths, with live distinct event keys retained for overlapping casts. No builds or tests.

Source batch 680 restored live-end traversal of common drops (0819372c) using an index and per-row copy to survive vector reallocation. Only the boot loader writes the list in recovered production paths. Also restored original DB item cache creation, hit and owner-transfer diagnostics (08058d64/08058e80/08058e34). No builds or tests.

Source batch 681 restored the DB shop price-list cache flush diagnostic after queuing writes and before clearing the dirty flag (0805073b-08050740). Party role bonus arithmetic matched the audited original. No builds or tests.

Source batch 682 restored original coordinate read order and signed low-dword spawn positions for special item group monster and group rewards (08180c32-08180c8b/08180e1a-08180ed3). Source guard and read-back passed. No builds or tests.

Source batch 683 restored the single signed 32-bit slowdown value shared by the special reward diagnostic and affect (08180aa0/08180d2a-08180d75). Player level-up stat and reward order matched. No builds or tests.

Source batch 684 restored the original player cache timeout diagnostic (08059034/080590a2-080590c0), reading data without refreshing the cache timestamp before flush. No builds or tests.

Source batch 685 restored low-dword HP drain limit and negation (08180d91-08180dcd) and the original cache flush limit diagnostic (DB ELF 08059166). Ordinary reset stat paths matched; exact class rereads remain for follow-up. Source guards and read-back passed. No builds or tests.

Source batch 686 restored fresh job reads for ResetPoint HP and SP growth bounds after the level change, upper then lower for each roll (08134578/0813458f/081345d0/081345e7). DB item-load and cache setup matched. No builds or tests.

Source batch 687 restored combat poison RNG before reading the level adjustment (08063ef0-08063ef5). Treasure poison bypasses this draw and matched the original. No builds or tests.

Source batch 688 restored ResetPoint modulo-32 level, growth, stat and resource arithmetic and explicit level-step/stat read order (081343f0/081345c1/08134614-08134675). DB price cache order matched. Source guards and read-back passed. No builds or tests.

Source batch 689 restored SP recovery fullness and cap evaluation before rereading the recovery pool (0813d4af-0813d4eb/0813d69f). DB quest flag persistence matched. Source guards and read-back passed. No builds or tests.

Source batch 690 restored skill-level rereads in ComputePassiveSkill and IsLearnableSkill (0806d0a2/0806d0d3 and 0806b0ed/0806b16b). Skill table text matched valid original data. No builds or tests.

Source batch 691 restored NPC recovery maximum HP reads before each mob table lookup (0812cfa0/0812cfae/0812cfeb/0812cff4). Player HP affect recovery matched. No builds or tests.

Source batch 692 restored the revive roll before the mob table threshold read (0813802f-08138046) and current descriptor lookup per loaded affect (0813e1e4), retaining a null guard. DB item award values matched. No builds or tests.

Source batch 693 restored fifteen original skill book locale keys and the GM_READ_SUCCESS and GM_READ_FAIL action names (0806bab0). No builds or tests.

Source batch 694 restored sixteen original skill book cooldown locale keys (08073910), including fallback. No builds or tests.

Source batch 695 restored the exact CP949 locale key for the +3 refine stone rejection (0817a566/0822f340). All nine grandmaster skill messages already matched. No builds or tests.

Source batch 696 restored original DB player load, save, create and delete dispatch diagnostics and log levels (0805c545/0805c587/0805c5c9/0805c620). Existing validation retained. No builds or tests.

Source batch 697 restored original Forked War DeadCount test-server formats and flag-before-count reads (080a54f5/080a54fa/080a556c/080a5571). DB ordinary item ID allocation matched; current exhaustion guard retained. No builds or tests.

Source batch 698 aligned the DB affect load row bound with the original 32-bit wire length and eight-byte prefix (08068d90). Ordinary skill book gameplay matched. No builds or tests.

Source batch 699 restored clock and max-SP read order in DistributeSP (081477c0-081477e1/081478f5-08147967). Ordinary amounts and effect thresholds matched. No builds or tests.

Source batch 700 restored unsigned 32-bit player cache key conversions at three logout and update boundaries (0806c009/0806c040/08060fa0), preserving high-bit IDs on LP64. No builds or tests.

Source batch 701 restored original low-dword gold scatter coordinate addition and offset order (08148b58/08148b76/08148dba/08148dd8/08149116/0814911e). No builds or tests.

Source batch 702 restored live EXP double-bonus and PC bang point reads (08153057/0815306a/08152bff/08152efd). Other DB cache lookup boundaries audited use unsigned keys. No builds or tests.

Source batch 703 restored the original skill power read order and deferred float narrowing in cast and compute paths (08071f26/08071f75/0806dc00/0806f855). Highscore persistence matched. No builds or tests.

Source batch 704 restored the signed weighted party EXP divisor, signed 64-bit truncation then low-dword share, and low-dword range deltas (08157673-0815768f). No builds or tests.

Source batch 705 restored x87 signed dword conversion for damage-weighted EXP and wrapping bonus addition (081533be/081534f8). No builds or tests.

Source batch 706 restored 32-bit unsigned SQL socket formatting for safebox awards (08055990), preventing LP64 sign expansion. Ordinary skill cast gates and charges matched. No builds or tests.

Source batch 707 restored low-dword values for item award IDs, vnums, counts and sockets at DB ingestion (08089d7e-08089dce). No builds or tests.

Source batch 708 restored original EXP scaling-before-cap and marriage partner-before-level evaluation (08152c62-08152ca6/08152cf9-08152d04). No builds or tests.

Source batch 709 restored per-recipient marriage point reads for the original test-server message (080b0845-080b08d0). No builds or tests.

Ongoing documentation requirement: update the repository-root README.md with each integrated reconstruction batch, alongside both restart notes. Keep source milestones and build/test milestones distinct.

Source batch 710 restored the original clock-before-marriage-time read order (080aff35/080aff3c). Marriage point arithmetic and caps matched. No builds or tests.

Source batch 711 restored live love point reads after proximity notifications (080b04e7/080b04fa). No builds or tests.

Source batch 712 restored original existing-row safebox and mall size diagnostics (080566db-080566f0). No builds or tests.

Source batch 713 restored the original combat item scatter order and wrapped 32-bit position arithmetic (08149d68-08149da3/0814a1f8). No builds or tests.

Source batch 714 restored one-based null loot row diagnostics (08149ad9/08149c8a). No builds or tests.

Source batch 715 restored the DB test-server diagnostic for cached item saves (08058524). No builds or tests.

Source batch 716 restored original test-server item cache SQL diagnostics and enqueue order (0804fe08/0804fea4/0805013f/08050170). Combat loot ownership matched audited behavior. No builds or tests.

Source batch 717 restored a fresh position snapshot for party gold distribution (0817eb2a/0817eb3f). No builds or tests.

Source batch 718 restored signed dword wrap in DB money log aggregation (08089566). No builds or tests.

Source batch 719 restored original dungeon group spawn upper bounds one unit beyond the center (0818486a/08184874), retaining range checks. No builds or tests.

Source batch 720 restored original SpawnGotoMob map lookup and spawn-before-name order (08184630), retaining the missing-map guard and dword coordinate arithmetic. Adjacent dungeon, party gold and DB money log paths matched. No builds or tests.

Source batch 721 restored unsigned 32-bit guild ranking IDs and persisted wager guild/amount values at DB load (0807b660/08080a60). No builds or tests.

Source batch 722 restored original fixed destination snapshot before dungeon party warp iteration (081823a7-081823d3/08185e35-08185e52). AutoGiveItem matched audited behavior. No builds or tests.

Source batch 723 restored unsigned 32-bit guild treasury values at SQL output (0807e840). Dungeon group transfer matched; duplicate destruction guard retained. No builds or tests.

Source batch 724 restored wrapped signed 32-bit guild treasury deposits, withdrawals and rollback (0807ea06/0807eab0/0807ebd4). Loot group loading matched. Dungeon potion and revive flags remain an original initialization gap. No builds or tests.

Source batch 725 restored 32-bit tile-to-world coordinate arithmetic in dungeon mob spawning (081843e4-081843fa). No builds or tests.

Source batch 726 restored original 32-bit guild win/loss/draw counter wrap (0807bf03/0807bfd3/0807c0a3). No builds or tests.

Source batch 727 restored the original item slot reread after GoldBarLog (0817fa11), with null and requested-vnum guards. DB guild value conversions matched audited paths. No builds or tests.

Source batch 728 restored original low-dword area coordinate arithmetic for four dungeon spawn paths (0818423d/08183741/081832d0/08184c90). No builds or tests.

Source batch 729 restored SpawnUnique draws before the map base reads in original Y/X order (081836de-08183747). DB hot-backup behavior matched. No builds or tests.

Source batch 730 restored the live source stack count read before transfer (0817e994), guarding against a now-oversized transfer before mutation. DB shutdown ordering matched. No builds or tests.

Source batch 731 restored moving unique spawn and destination map-base read order (0818320e-081832d6/08183422-08183435). No builds or tests.

Source batch 732 restored live map and position reads immediately before full and split item drops (0817e06d-0817e093), retaining rollback. No builds or tests.

Source batch 733 restored moving group direction draw and map-base read order (08184db6/08184c33/08184c71-08184cf8). DB player binary field persistence matched. No builds or tests.

Source batch 734 restored original Y-before-X map-base reads after both area spawn rolls (0818423a/08184243). DB player-load order matched, retaining peer validation. Pickup retains full-width counts to avoid truncating oversized stacks. No builds or tests.

Source batch 735 restored original character type checks at dungeon detach and attach (08131b34/08131a80). No builds or tests.

Source batch 736 restored original 32-bit player ID formatting for empire relocation SQL (08057472/080574b4). Inventory equip/swap matched ordinary behavior. No builds or tests.

Source batch 737 restored unsigned 32-bit MAX(item ID) values at both DB range ingestion points (0805ee74/0805bbf0). Dungeon regen and inventory paths matched reviewed behavior. No builds or tests.

Source batch 738 restores 32-bit bounds and coordinate-read order in CHARACTER_MANAGER::SpawnMoveGroup. Verified against the original disassembly and checked patch application. No build or runtime run.

Source batch 739 restores the original coordinate reads before RNG calls and count-before-vnum reads in OreDrop (Ghidra 080ef2f0). Reviewed NPC spawning, bow damage, attack rating and DB shop price reload paths matched for the behaviors checked. No build or runtime run.

Source batch 740 restores original pick experience read order and 32-bit player IDs in DB marriage reload (080ef630 and 0808cca9/0808ccba). Bounded wall placement and magic damage comparisons found no gameplay arithmetic mismatch. No build or runtime run.

Source batch 741 restores mining skill-bonus read order (080ef4e0), 32-bit marriage timestamps (0808cd48/0808d1bf), and 32-bit attack reposition distance and clock-read order (081347be-081347ef). Terrain lookups matched the reviewed behavior. No build or runtime run.

Source batch 742 restores max-pick experience read order (080efc20), Follow leash timeout and distance evaluation (08132f5c-08132fa3, 08133560-081335a4), and 32-bit guild withdrawal timestamps (0807888c/08078b93). No build or runtime run.

Source batch 743 restores current-position reads for Follow prediction and clamp (08133495/081334a7, 081334ed/08133510). All 117 dwords in the four mining tables matched the game ELF at 0823fea0, 0823ff40, 0823ffc0 and 08240000. NPC roaming and reviewed DB persistence paths matched. No build or runtime run.

Source batch 744 restores signed 32-bit practice socket reads (0816efc0) and signed 32-bit guild rematch deadline comparisons (0807c3de-0807c3f7). Horse behavior matched reviewed paths. StateMove zero-duration arrival guard retained after original comparison to avoid invalid coordinates. No build or runtime run.

Source batch 745 restores polymorph chance input read order (0816ede0-0816edf8) and clock-before-start-time order in StateMove (0808024c/08080251). The zero-duration movement guard remains in place. Guild reservation recovery matched reviewed paths. No build or runtime run.

Source batch 746 restores the mob vnum reread after applying polymorph (0816ed70) and 32-bit coordinate addition for the idle NPC destination, midpoint and Goto checks (0807f470). Reviewed quest gold/SP and guild war reconnect paths matched. No build or runtime run.

Source batch 747 restores original 32-bit monster idle movement coordinates for destination, midpoint and Goto (0807fe37/0807feb4/0807fefd). NPC and monster idle paths share the coordinate addition. Quest refinement and guild payout matched reviewed paths. No build or runtime run.

Source batch 748 restores CowardEscape path sample and destination addition at 32 bits (0807d961-0807da28), using the existing idle movement arithmetic. Quest teleport and DB quest/affect reload matched reviewed paths. No build or runtime run.

Source batch 749 restores StateHorse current Y/X reads for the midpoint check and final movement call (08081ade/08081af9 and 08081b61/08081b72). Base horse rider stubs and the reviewed quest scroll/mastery paths matched. No build or runtime run.

Source batch 750 restores the original horse wander distance draw before heading read (08081a20/08081a26) and per-item prototype startup diagnostics (DB 08071220). Reviewed horse persistence and quest paths matched. No build or runtime run.

Source batch 751 restores NPC and monster idle distance draws before heading reads (0807f732/0807f741 and 0807fdaf/0807fdb5) and skill prototype reload and per-row diagnostics (DB 080719ef/080718b2). Reviewed fishing and quest item loading matched. No build or runtime run.

Source batch 752 restores original DB reload and per-row diagnostics for normal and rare item attributes (080721cf/0807271f and 08072094/080725e4). No build or runtime run.

Source batch 753 restores 32-bit coordinate subtraction in monster and NPC idle leader-distance checks (0807fce8/0807fcee and 0807f674 vicinity). No build or runtime run.

Source batch 754 restores the original shop identifier and item count startup diagnostic (DB 080705d7). Reviewed shop data loading and polymorph chance, duration, and bonuses matched. No build or runtime run.

Source batch 755 restores extended precision where available for the pursuit threshold (08080e1e) and original mob prototype startup diagnostics (DB 080701d8/08070117). Reviewed quest NPC proximity and seasonal counters matched. No build or runtime run.

Source batch 756 restores battle cooldown evaluation order (08080eaa-08080edd) and building object reload and per-row diagnostics (DB 080735ac/08073468). No build or runtime run.

Source batch 757 restores the original 0-80 apply ID range for the three quest affect add variants (08069331/08069721/080699c1), with the separate 1-254 direct point range. No build or runtime run.

Source batch 758 restores original DB building object prototype reload and row diagnostics (08073244/080730cd). Reviewed quest affect removal, monster skill recovery ticks and object NPC placement matched. No build or runtime run.

Source batch 759 restores monster skill power scaling with a single float rounding and original input order (08073e07-08073e36) plus DB land reload and row diagnostics (08072bcf/08072a4e). Reviewed quest item getters and land decoding matched. No build or runtime run.

Source batch 760 restores the immediate monster splash skill-level reread (0807408f-080740b1) and original DB refine recipe reload and row diagnostics (0806f995/0806f89f). Reviewed marriage quest bindings and recipe decoding matched. No build or runtime run.

Source batch 761 restores clock-before-cooldown reads in CanUseMobSkill (08073c6b/08073c70 and 08073c79/08073c7e) and original DB item prototype reload diagnostics (08071393). Reviewed horse quest behavior matched. No build or runtime run.

Source batch 762 restores the original clock-before-expiry read order in HitOnce (0806ad9e/0806ada3) and DB banword load count diagnostic (08071c8d). Quest state bindings matched the reviewed paths. No build or runtime run.

Source batch 763 restores DB item ID range startup diagnostics (0805eb8f/0805ed3a). Reviewed party quest bindings, arena duel lifecycle, unique expiry, HP percentage, ranged resistance and building material parsing matched ordinary inputs. No build or runtime run.

Source batch 764 restores original DB localization load count, selected region and name column diagnostics (0805f05c/0805f337). Arena observer and accessory expiration behavior matched the reviewed paths. No build or runtime run.

Source batch 765 restores DB startup diagnostics for successfully parsed guild power and handicap formulas (0807b315/0807b2c4). Reviewed arena Lua bindings and ground-item timers matched ordinary paths. No build or runtime run.

Source batch 766 restores stun from normal attacks without an HP modifier (080d72db-080d72e3). Reviewed building quest bindings matched ordinary gameplay. No build or runtime run.

Source batch 767 restores the original DB guild rank row diagnostic (0807b6e1). The rank calculation already matched. No build or runtime run.

Source batch 768 restores the point-before-name read order in test-server attack-effect diagnostics (080d78e0). Slow and poison parameters, OX quest bindings, and DB guild reservation payouts matched reviewed paths. No build or runtime run.

Source batch 769 restores the mana shield strength reread after SP consumption (0814e5bb-0814e5e5), guarding against a zero divisor if the shield disappears. Reviewed quest target bindings and DB highscore/logout diagnostics matched. No build or runtime run.

Source batch 770 restores the second mall defense point read before damage reduction in both original paths (0814dc06-0814dc32 and 0814f96a-0814f990). Reviewed NPC quest and price-list loading matched. No build or runtime run.

Source batch 771 restores the mana burn chance reread after the RNG draw (0814db60/0814e6b7-0814e6c5). Quest event flags and item drops matched reviewed paths. No build or runtime run.

Source batch 772 restores signed 32-bit hot-backup elapsed comparison (DB 08085af7-08085b0a), retaining the original epoch versus tick clock inputs. No build or runtime run.

Source batch 773 restores the damage bonus reread and 32-bit addition (0814e603/0814e63d-0814e668) and the DB signed 32-bit logout cache cutoff (0806bfdc-0806bfe1). Reviewed mining quest bindings matched. No build or runtime run.

Source batch 774 restores 32-bit subtraction for the capped damage defense factor (0814dbc5-0814dbd2). Reviewed mount and horse quest bindings matched. No build or runtime run.

Source batch 775 restores DB cache timestamps and signed 32-bit elapsed comparisons for flush and expiry (0805901b-0805902e/080590e0/080591ef/080592f7). No build or runtime run.

Source batch 776 restores the original one-third damage snapshot and shield strength reread after SP consumption in the normal damage path (0814e520-0814e5e5). Division is guarded for zero and signed overflow. Reviewed quest alignment and DB timer storage matched. No build or runtime run.

Source batch 777 restores the original test-server item cache deletion diagnostic (DB 0804fc3c) and explicitly stores the fixed-width update timestamp. Reviewed quest polymorph paths matched. No build or runtime run.

Source batch 778 restores original first-hit damage entry creation and recovery only for existing attacker entries in normal Damage (0814e080-0814e11d). No build or runtime run.

Source batch 779 restores the test-server player cache flush diagnostic (DB 08050379). Aggro scaling, target switching, and horse stamina paths matched the reviewed original behavior. No build or runtime run.

Source audit after batch 779: recovery scheduling, horse offline recovery, gold and experience payout, and DB shop price and item cache paths matched reviewed ordinary behavior. No source changes, build or runtime run.

Source audit after batch 779: WAEGU boss gold drops, HP/SP steal and melee reflection (0814f440-0814f706, 0814e961-0814eb98, 0814ed4a), mining completion and OreDrop (080efdc0/080ef2f0), and DB item cache set flush (080588b0) matched reviewed ordinary behavior. No source changes, build or runtime run.

Source audit after batch 779: monster pull/attract/forget helpers, arrow handling, death experience penalty, and ordinary DB building create/delete and land update matched reviewed behavior. Recovery event recheck matched prior audit. No source changes, build or runtime run.

Source audit after batch 779: symbol-to-source matching found no missing in-scope names in the scanned game quest/sectree/dungeon/forked-war methods or DB client manager/item award/guild manager/guild war reservation classes. This verifies name coverage for the scanned classes. Polymorph book grants and guild skill polynomial calculations matched the reviewed branches. No source changes, build or runtime run.

Source audit after batch 779: equipment point modifiers, accessory degradation, affect expiration, quest event timers, DB item award load/taken handling, and world start/create/arena/Santa tables matched the original in reviewed paths. No source changes, build or runtime run.

Source audit after batch 779: local game symbol scan found no missing ordinary gameplay names. Remaining DB ItemAwardManager, PlayerHB, CMoneyLog, CPrivManager and marriage manager exported methods were found in source; wedding time ordering matched. AlterToMagicItem, GetGold, CheckItemUseLevel and DistanceValid matched audited item branches. No source changes, build or runtime run.

Source batch 780 restores the guild state reread before refine notifications (08171d8d/0817200c/081720b1) and the original unsigned 32-bit love-point log display (DB 0808d857). Arena duel list matched reviewed behavior. No build or runtime run.

Source batch 781 restores the original DB marriage reload count and per-row diagnostics (0808cc34/0808cc5e/0808cef6). Reviewed refinement fees, preview, socket transfer and arena placement matched. No build or runtime run.

Source batch 782 restores original locale reads for scroll refinement probability and for failures without a downgrade (08173031/08172dae). Reviewed dungeon elimination and marriage creation matched. No build or runtime run.

Source batch 783 restores the scroll refinement level rereads for chance tables and test diagnostics (08173126/0817315b/08173190/08173206/0817323c). Wedding expiry uses the copied PID after erasing the wedding entry, preserving cleanup without a dangling reference (DB 0808e720). No build or runtime run.

Source batch 784 restores accessory socket grade reads after drilling and insertion RNG (0817b2a5/0817b60b-0817b625). Reviewed building reconstruction, guild NPC respawn and wedding replay matched. No build or runtime run.

Source audit after batch 784: socket cleaning and Metin insertion (0817aa26-0817ab40/0817bb42-0817be3f), guild building bonuses (08106df0/08106ed0) and DB party level updates (08077880) matched reviewed ordinary behavior. Existing item lifetime guard retained. No source changes, build or runtime run.

Source batch 785 restores the attribute count read after rolling in both item attribute add paths (0817b03d/0817b8b1), with a probability table bounds check. Reviewed arena attack rules matched. No build or runtime run.

Source audit after batch 785: item attribute reroll and selection (08187540/08187060/0817ab7e), dungeon notice delivery (08185310), and DB affect save/remove (0806b980/0806ba40) matched reviewed ordinary behavior. No source changes, build or runtime run.

Source audit after batch 785: accessory socket timers and unequip handling (081689a0/08168a90/08168b70) and ordinary shop pricing/restrictions (080d2b10/080d37d0) matched. CreatePlayerSaveQuery uses signed historical PID formatting (080675b0) only differing above INT32_MAX, as with quest saves. Kept current unsigned-schema-compatible behavior. No source changes, build or runtime run.

Source audit after batch 785: Cube crafting reward/material ordering (080fcd70), unique item expiration and unequip (08168450/08168580/08168680), and DB money log accumulation and flush (08089390) matched the reviewed behavior. Existing recipe and item lifetime checks retained. No source changes, build or runtime run.

Source audit after batch 785: rare attribute eligibility and assignment (08187a20), item addon roll and application order (0816e880), Santa respawn (0818ac80/0818ae10), and DB highscore signed score and tie handling (0806bd64-0806bd96) matched the original paths reviewed. No source changes, build or runtime run.

Source batch 786 restores independent locale reads at the skill-book EXP gate and progression branch (0806bb3f/0806bb48 and 0806bfcf/0806bfd8). Reviewed dungeon quest flags and DB guild decoding matched. No build or runtime run.

Source batch 787 restores original per-row DB event flag load diagnostics (0808b42d). All 27 party EXP table values and reviewed skill-book timing matched. No build or runtime run.

Source batch 788 restores current skill level reads at mastery rolls (0806cab9/0806cb8f) and DB changed event flag logging after query submission (0808b7fc). All 40 skill-book progression table values matched the game ELF. No build or runtime run.

Source audit after batch 788: GetSkillPower, SetSkillLevel and ComputePassiveSkill (08073350/0806b020/0806d070), all 15 poison table entries and GetPoisonDamageRate (081f7910/0823b460/08063b20), and DB PutPlayerCache hot-backup ordering (08067910) matched reviewed behavior. No source changes, build or runtime run.

Source batch 789 restores cooldown, victim VID, and skill-use record lookup order in targeted UseSkill (080722bc/080722d6). All 82 chain-lightning target table values and ordinary DB item row decoding matched. No build or runtime run.

Source audit after batch 789: Chain Lightning bounce selection and hit order (0806d1f0/08075430), all 70 stone drop table values (0821b420), and DB safebox gold persistence (08057030) matched the reviewed original paths. No source changes, build or runtime run.

Source batch 790 restores original 32-bit DB usage aggregation counters (0805d57e/0805d639/0805d640). Reviewed skill eligibility and resets matched, as did all 17 mob enchantment and resistance mapping entries. No build or runtime run.

Source batch 791 restores original movement speed lookup before base duration arithmetic (0812dc6e-0812dc95). All 17 item attribute probability values and the DB usage log output matched reviewed behavior. No build or runtime run.

Source batch 792 restores a shared wrapped 32-bit deadline for the DB guild war start queue and diagnostic (0807e40c-0807e439). Reviewed movement paths and all 15 accessory socket table values matched. No build or runtime run.

Source audit after batch 792: Attack dispatch and nearest-victim selection (08147ac0/08153cf0), all 100 party EXP distribution weights (0821ab20), and DB guild war reservation timing (0807f430) matched reviewed behavior. No source changes, build or runtime run.

Source batch 793 restores the single mounted horse armor read in ComputeBattlePoints (0812c0cb-0812c0e0). All 372 horse stat values and reviewed DB guild skill cooldown paths matched. No build or runtime run.

Source audit after batch 793: ComputePoints and adjacent armor/magic calculations (0812c520), 78 mining fraction/grade/skill values (0823ff40/0823ffc0/08240000, recheck), and in-range DB item ID allocation (0805ef80) matched. The reconstruction retains the exhaustion check at the unsigned 32-bit boundary. No source changes, build or runtime run.

Source audit after batch 793: all 155 fishing timing probabilities (0823c0e0), ResetPoint stat and HP/SP formulas (081343f0), and DB FlushPlayerCacheSet order (0806c040) matched the reviewed original. No source changes, build or runtime run.

Source audit after batch 793: blend socket rolls (080e6430), marriage bonus and point progression (080b00f0/080aff00), all ten Metin wave thresholds and stage guards (0807dbf0), and DB guild member count and average decoding (0807f0a0/0807f270) matched reviewed behavior. No source changes, build or runtime run.

Source batch 794 restores explicit signed 32-bit score broadcast values and unsigned 32-bit diagnostic display in DB UpdateScore (0807db20). Existing uint32 accumulation already matches original wrapping. Metin spawn radii and war-map potion and kill scoring matched. No build or runtime run.

Source audit after batch 794: all 124 level-difference table values (0821a920/0821a9a0/0821aa20/0821aaa0), war-map score/timeout/reward paths (0816b670/0816b090/0816afb0), and DB reserve-war countdown and queued start (0807ff40) matched reviewed behavior. No source changes, build or runtime run.

Source audit after batch 794: all 600 values of six locale EXP curves (08219f60-0821a90f), DropGold and ItemProcess_Hair (0817e2d0/08180fc0), and DB guild ladder gain, loss and persistence (0807e500/0807c5bc-0807c609) matched reviewed original behavior. No source changes, build or runtime run.

Source audit after batch 794: all 35 item socket chance and capacity values (0821ade0/0821ae40/0821ae60), quest percentage EXP and reviewed party shares (08161060/08060fd0), and DB player part, skill-group and alignment loading (08068550) matched. No source changes, build or runtime run.

Source batch 795 restores the successful DB item ID range diagnostic and original signed 32-bit remaining-count display (0805c086). All 100 EXP loss table entries and reviewed castle siege tower cleanup matched. No build or runtime run.

Source audit after batch 795: all 41 polymorph power values (0821b540), horse health decay and 32-bit deadline updates (080c8950), and DB item-range collision diagnostics (0804c980/0804c9ef) matched the reviewed original. No source changes, build or runtime run.

Source batch 796 restores the DB item cache set creation diagnostic after insertion (08058870-08058899). All 42 guild EXP table values matched. No build or runtime run.

Source batch 797 corrects batch 757: the original quest apply wrappers decrement the input byte before the unsigned comparison against 80 (08069319/08069331, 08069700/08069721, 080699a0/080699c1), accepting IDs 1-81. Direct-point limits unchanged. No build or runtime run.

Source batch 798 restores original DB shop price cache update diagnostics (08050597-080505c8). Reviewed quest affect checks and all six monster-rank gold probabilities matched. No build or runtime run.

Source batch 799 restores original DB shop price cache hit, miss and load-result diagnostics (0805baa3/0805bb38/08056f05). Reviewed shop placement and visibility, and all 72 CN/KR item drop penalty values matched. No build or runtime run.

Source batch 800 restores the original DB shutdown cache flush diagnostic (080542ba). The raw pointer startup diagnostic remains omitted. Reviewed inventory grid behavior and all 32 battle-type modifiers matched. No build or runtime run.

Source batch 801 restores original DB boot packet size, version and gameplay record size diagnostics (080547ac-0805490f). All 64 initial job stat and growth values and reviewed fish grilling behavior matched. No build or runtime run.

Source audit after batch 801: all 36 Chinese/Korean guild war settings (0823fca0/0823fd00), CalcAttBonus race priority and percentage stacking (080d61e0), and DB item load cache order and diagnostics (08068c50/08068cf8) matched reviewed original behavior. No source changes, build or runtime run.

Source audit after batch 801: all 161 nearby position offset pairs (0823f760), ordinary fishing practice (08082cf0), and DB guild war cancellation order (08080910) matched reviewed behavior. No source changes, build or runtime run.

Source audit after batch 801: all 32 skill point mapping entries (0823f260), pick refinement (080ef720), and DB guild war draw handling (0807c4b0) matched reviewed original behavior. Existing owner checks retained. No source changes, build or runtime run.

Source audit after batch 801: dungeon stone and wooden door defaults (08183b90/08183bc0), quest item drop conversion, offsets and expiry (080b2df0), and DB item award polling (08089ba0) matched the reviewed original. No source changes, build or runtime run.

Source audit after batch 801: dungeon party destination and recipient selection (081821d0/080947c0), PC::Reward item quantities and EXP cap (08061140), and DB player load result diagnostics (08068b00/08068b7c) matched reviewed behavior. No source changes, build or runtime run.

Source batch 802 restores original guild skill, experience and member update diagnostics (DB 080789a3/08078a03/08078a7a). Reviewed dungeon region parsing and recall item behavior matched ordinary inputs. No build or runtime run.

Source batch 803 restores five original DB guild membership diagnostics (08077f7d/08078029/080780bf/08078832/08078aea). Reviewed private map allocation and movement stamina matched. No build or runtime run.

Source batch 804 restores original DB guild ladder update and skill request diagnostics (08079399/08079429). Reviewed map region lookup and movement stamina recovery matched. No build or runtime run.

Source batch 805 restores the original DB guild skill availability diagnostic (080794f3). Reviewed map midpoint arithmetic and normal and skill attack effects matched. No build or runtime run.

Source batch 806 restores eight DB guild war state diagnostics (08078df0). NPC map position insertion matched the original. No build or runtime run.

Source audit after batch 806: named dungeon monster spawn (081844d0), reviewed horse feeding, stamina and skill-level synchronization, and DB guild treasury diagnostics (0805a570/0805a5e0/0805a650 and 0807e869-0807ebc3) matched. No source changes, build or runtime run.

Source batch 807 preserves the selected DB guild wait-start entry across priority queue pop. Reviewed dungeon destination marker and polymorph paths matched. No build or runtime run.

Source batch 808 zero-initialized the DB queued guild war start record. The original Game dispatch uses the guild IDs to start the war and receives initial scores separately. CDungeon::SetUnique and reviewed item equip requirements matched original behavior. Ghidra confirmed the original game ELF with 7,238 recognized functions. No build or runtime run.

Source batch 809 restored the original read order for CHARACTER::GetMobAttackRange (0812bd54–0812bd71) and CHARACTER::GetPremiumRemainSeconds (081363a3–081363b2). Reviewed dungeon regeneration, kill and purge, accessory bonuses, and horse quest paths matched for ordinary gameplay. Disassembly at 080e3980 confirms truncation before clamping horse level. No build or runtime run.

Source audit after batch 809 found source bodies for all 103 checked original dungeon and quest-dungeon symbols and no missing ordinary item or character stat methods in the reviewed classes. Static comparisons matched quest coroutine transitions and cache (0805c1d0, 0805cb90–0805d000, 081a7380), target bindings (081899b0–0818a1c0), OX event bindings (0813c780–0813cc60), and ordinary arena round transitions (080bc440). Next, prioritize link dependencies and startup integration over repeated body presence scans. No source changes, build, or runtime run.

Source batch 810 restored config startup CMD tokenization through two_arguments (config_init 080d8300, CMD path 080d9f74–080da0a2). Quoted tokens now reach the existing privilege lookup without quote delimiters. The reviewed main initialization sequence matched the original. No build or runtime run.

Source batch 811 restored original AUTH_SERVER argument parsing (config_init at 080d8300). The quoted master keyword and quoted host and port now reach the same configuration branches as unquoted tokens. The startup DB table order, game boot table consumption, blend loader, and locale conversion loaders matched the reviewed ordinary paths. No build or runtime run.

Source batch 812 restored game boot asset path preparation before loader invocation (080e8c2b–080e8d88) and the fresh map base read before Build (080e8f5a). The five drop loader calls retain the original order. All supplied SQL skill power rows have 45 space-separated entries; both the binary and reconstructed loader consume the first 41 for those rows. Reviewed motion lists and cube recipes matched normal inputs. No build or runtime run.

Source and project audit after batch 812: the Game Visual Studio project covers all 132 C++ files and the one C file. Every translation unit listed by the nine repository Visual Studio projects resolves to an existing path. Game uses a Makefile configuration with no NMake build command or production Game Makefile. Reviewed map index loading (080b5c80), ordinary and special drop groups, building finalization (08109450), and motion initialization (08113440) matched ordinary original behavior. No source changes, build, or runtime run.

Source batch 813 restored const input in building::CManager::LoadObjectProto (08108b10; ELF symbol _ZN8building8CManager15LoadObjectProtoEPKNS_12SObjectProtoEi). Declaration and definition agree, and the existing Boot caller remains compatible. Reviewed material validation and building finalization matched the original. No build or runtime run.

Source batch 814 restored the original const input to building::CLand::PutData (081076c0; ELF symbol _ZN8building5CLand7PutDataEPKNS_5SLandE). Declaration and definition agree, and the existing LoadLand call remains compatible. Reviewed ordinary building placement matched the original. No build or runtime run.

Source batch 815 restored CItem::PutAttribute(const int*) (081874d0; ELF symbol _ZN5CItem12PutAttributeEPKi) and removed four casts from callers. Also fixed an invalid string member call left in batch 811 after AUTH_SERVER host changed to a character buffer. Declaration, definition and call sites were checked statically. No build or runtime run.

Source batch 816 restored building::CManager::UpdateLand recipient traversal (08108ec0). The client iterator is acquired before packet preparation and advanced before each send. Map filtering and packet sizes remain 3 plus 24 bytes. Reviewed map, sector and regen parameter lists matched the ELF. No build or runtime run.

Batch 816 also restored the original non-const CItem::IsEquipped() declaration (DWARF linkage name _ZN5CItem10IsEquippedEv). All three current production call sites use mutable items. No build or runtime run.

Source batch 817 restored ELF parameter types for CreatePlayerProto (08128bb0, reference), SetPlayerProto (0812a680, pointer to const), and UpdateAggrPointEx (08154500, reference). Declarations, definitions, and source call sites were reconciled. No build or runtime run.

Source batch 818 restored the DWARF return type const CSpecialItemGroup* for ITEM_MANAGER::GetSpecialItemGroup and updated GiveItemFromSpecialItemGroup. The reviewed target marker behavior matched. No build or runtime run.

Source batch 819 restored CHARACTER::UseSilkBotaryReal(const TPacketMyshopPricelistHeader*) at 08174150. Game and DB now share the same five-byte header. The game dispatcher still verifies packet length and entry count before calling the handler. Reviewed fishing catch logic matched the original. No build or runtime run.

Source batch 819 also restored ITEM_MANAGER::GetSpecialGroupFromItem(DWORD) from DWARF and its inlined lookup in CItem::GetSpecialGroup at 08168780. No build or runtime run.

Source batch 820 restored evaluation order in CItem::GetRefineFromVnum (08168370). The item VNUM is read before the item manager singleton and passed to the existing reverse lookup at 08198c30. OX event answer handling and manager API parameters matched the original. No build or runtime run.

Batch 820 also restored SetPlayerProto skill array installation before the copy hook (0812a680), preserving allocation before deletion so allocation failure keeps the previous array. No build or runtime run.

Source batch 821 restored CSpecialItemGroup::IsEmpty and GetOneIndex from DWARF and the original weighted lookup. SelectItem retains the empty and out-of-range guards. Reviewed arena round transitions matched. No build or runtime run.

Source batch 822 restored both CInputDB::PlayerLoad map fallbacks to use g_start_map (080e7a60). The denied-map path rereads descriptor empire before selecting the return map and coordinates. The reviewed arena timeout path matched the original. No build or runtime run.

Source batch 823 restored DWARF-indexed special item group accessors and integrated them in GiveItemFromSpecialItemGroup (08180a68–08180aa3). Selection uses one roll for nonempty groups, validates the index, and reads vnum, count and rare chance before reward dispatch. Reviewed OX quiz flow matched. No build or runtime run.

Source batch 824 restored QuestLoad post-Build descriptor lookup and live deferred-login PID (080e9850), keeping the null guard. Also restored DWARF-confirmed CMobItemGroup::GetOneIndex with one roll for nonempty groups and guarded caller selection. Reviewed fishing grill, safebox, and mall paths matched. No build or runtime run.

Source batch 825 restored live character PID reads in quest_login_event (080e96f0). The scheduled PID still selects the character; the callback reads that character ID again for logging and Login, matching the original. Reviewed ore refinement and level-limited drops matched normal behavior. No build or runtime run.

Source batch 826 restored shared g_start_map and g_nation_name use in the three-way war logic recovered from CHARACTER::Dead (0814af70). Evacuation captures the shared map value when scheduled, and announcements localize the shared empire names. Reviewed dungeon monster counting and accessory socket compatibility matched. No build or runtime run.

Source batch 827 restored the two distinct event flag lookups around logging in CQuestManager::SetEventFlag (081a5e60). Reviewed quest PC flag persistence, item ground removal and ownership expiry matched. No build or runtime run.

Source batch 828 restored signed PID diagnostics in QuestButton (081a3a20) and QuestInfo (ELF CQuestManager::QuestInfo). Existing safe localized format handling remains in place. Quest resume and guild war observer flow matched ordinary original behavior. No build or runtime run.

Source audit after batch 828 confirmed ordinary quest state transitions, current NPC and item helpers, item initialization and prototype flags, and dungeon return fallback (08186c20). No source changes, build, or runtime run. Continue with unverified gameplay branches rather than rechecking these helpers.

Source batch 829 corrected GetLimitPoint diagnostic arguments (0812e120). Static comparison confirmed the original limit values and exposed a missing point-type argument in the diagnostic. The reconstruction supplies that argument so the original format has matching values. Reviewed party level bounds and movement speed arithmetic matched. No build or runtime run.

Batch 829 also corrected the same missing point-type argument in the sibling GetPoint diagnostic.

Source batch 830 restored MainCharacterPacket branch-local reads after the BGM diagnostic and current descriptor lookup at send (08129f70). All three packet layouts and the mobile notice remain unchanged. Reviewed quest rewards and party bonus calculations matched. No build or runtime run.

Source batch 831 restored the original FFindReviver state fields and HasReviverInParty return field (08137f90 and 08137170). x86 disassembly confirms return of the flag byte from the functor result, despite the decompiler showing false. Existing null guard and roll order remain. Reviewed polymorph power, motion mode and party proximity matched. No build or runtime run.

Source batch 832 restored the post-log dungeon member field reads in CHARACTER::SetDungeon (081319c0). Party, solo player, and monster additions use the assigned member field. Reviewed pick refinement and dungeon group placement matched normal original behavior. No build or runtime run.

Source batch 833 restored the late job read in GiveRandomSkillBook (08134810), after item award and the job-specific roll. The existing partial-table safeguard is retained. Reviewed dungeon removal, exit location and party gold distribution matched ordinary original behavior. No build or runtime run.

Source batch 834 restored the time_t parameter in SetSkillNextReadTime (DWARF at 0806acb0). Kept the fixed 32-bit timestamp in the packed player record and the existing slot bounds check. Reviewed dungeon member count transitions and skill book reward selection matched. No build or runtime run.

Source batch 835 restored GetMobSkill prototype snapshot and count ordering (08073bb0). The original mutable return is still unresolved against the current const CMob pointer and const CMobManager::Get API; this batch retains the const result and bounds. Reviewed item pricing and party roles matched. No build or runtime run.

Batch 835 type audit resolved GetMobSkill: raw DWARF 0x3d087e and 0x1ead5c3 give pointer to const TMobSkillInfo. CMobManager::Get (0x18326e3/0x1832704), m_pkMobData (0x1eaa421) and SetProto (0x1eaaa9d) also confirm pointers to const CMob. This supersedes the mutable-return inference in the preceding entry. Reviewed skill eligibility and cooldown matched. No source changes, build, or runtime run.

Source batch 836 restored the order of skill prototype ID reads in SkillLevelDown, keeping guards before the later array accesses. Reviewed monster cooldown reset, immediate and timed skill execution, and random stone rewards matched. No build or runtime run.

Source batch 837 restored SkillLevelUp prerequisite and mastery ordering and the alternate normal mastery transition (0806c6e0), plus ComputePassiveSkill diagnostic read ordering (0806d070). Existing skill ID and formatting guards remain. Normal book item handling and party summoning matched. No build or runtime run.

Source batch 838 restored ResetOneSkill log input reads before the logger lookup (08074910). Full and secondary skill reset, skill-forget item flow, and arena cleanup matched the checked original paths. No build or runtime run.

Source batch 839 restored ComputeSkill target range rounding to float after the 32-bit addition. Ability items now read duration before amount (08174a02–08174a2e). Dungeon empty-instance timer matched ordinary behavior, with the existing duplicate callback guard. No build or runtime run.

Source batch 840 restored the single sector lookup in ChainLightningEvent fallback target traversal (0806d1f0). Skill power calculations and ordinary delayed map travel matched the reviewed original paths. No build or runtime run.

Source batch 841 restored point polynomial input order in ComputeSkill and ComputeSkillAtPosition. ComputeSkill rereads the mounted skill type after polynomial setup (0806f94d), retaining the early riding guard. Timed consumables reread duration, amount, apply and applicable affect ID after eligibility checks and validate the new apply index (08179b70–08179bc0 and 08175261–081752bf). Automatic warp NPC scheduling matched. No build or runtime run.

Source batch 842 restored CDungeon::Join destination capture before SaveExitLocation (08182290), retaining the missing-map guard. Reviewed chain lightning limits, used skill mastery and accessory degradation matched. No build or runtime run.

Source batch 843 restored evaluation order for timed nonattack effects in ComputeSkill (0806f710). The locale check precedes the Chunkeon ID and mastery checks, and duration SP cost is evaluated before the primary affect fields are read. No build or runtime run.

Source batch 844 restored the original CItem inline GetWearFlag, GetAntiFlag, GetImmuneFlag, GetRefinedVnum and GetRefineSet APIs from DWARF and the reference. The corresponding equipment and refinement paths call the accessors, which return zero when the prototype is absent. No build or runtime run.

Source batch 845 restored seven DWARF-confirmed Chain Lightning and Eunhyung inline CHARACTER helpers and routed the live movement and skill callers. Preserved initialization and exclusion-set update order and 32-bit counter arithmetic. Arena and item accessor candidates remain staged for subsequent review. No build or runtime run.

Source batch 846 restored nine original CArena helpers and four indexed CItem socket/attribute accessors with live callers. The arena map list uses mutable arena pointers for the original nonconst getters. Item call sites use fixed or bounded indices. No build or runtime run.

Source batch 847 restored the DWARF-confirmed CItem::IsStackable and used it in item creation, movement, splitting and pickup while retaining the existing flag check and guards. No build or runtime run.

Source batch 848 restored FWarpToDungeon (08185dc0) for solo and party entry, retaining map guards and exit-save order. Restored four DWARF-confirmed character timing accessors and integrated movement, combat, and equipment callers. Rebased the equipment timing hunk onto batch 847. No build or runtime run.

Source batch 849 restored CItem::SetFlag(long) from DWARF and the reference. Item initialization, prototype binding, and flag add/remove now use the setter with existing 32-bit normalization. No build or runtime run.

Source batch 850 restored FExitDungeon through CParty::ForEachNearMember (08145000) and the DWARF-confirmed CHARACTER::GetAIFlag (DIE 0x1eaa7c5), with existing party and AI call sites. No build or runtime run.

Source batch 851 restored IsPosition(int) and GetStateDuration() (DWARF DIEs 0x1eaaeea and 0x1eaa981), wired into IsDead, CanBeginFight and UpdateStateMachine. Preserved the callback lifetime guard. The shop DWARF audit found no missing ordinary method bodies. No build or runtime run.

Source batch 852 restored FExitDungeonToStartPosition (08186c20) at the active dungeon exit. The shared traversal supplies filtered CHARACTER pointers to the helper and retains the empire recall, map-zero warp and saved-location fallback. No build or runtime run.

Source batch 853 restored rod socket read order in RefinableRod (08084230) and GetFishingLevel (08083170). Both use the recovered GetSocket accessor; eligibility checks and 32-bit arithmetic remain. No build or runtime run.

Source batch 854 restored FWarpToPosition::operator()(CEntity*) (081869c0) in WarpMapPlayers, used by JumpAll and JumpToEliminateLocation. Preserved sector traversal, the player filter, tile conversion and same-map Show then Stop versus cross-map WarpSet. No build or runtime run.

Source batch 855 restored DetermineFish locale and selected fish ID read order (080828e0). Existing selection and exclusion logic stays in place. Next verified named helper is FPurgeSectree at 08186b10 for CDungeon::Purge. No build or runtime run.

Source batch 856 restored FPurgeSectree (08186b10) to CDungeon::Purge. DetermineFish now reads the selected vnum separately inside each enabled locale branch (08082a38–08082a6c). Existing filters, purge traversal and branches remain. No build or runtime run.

Source batch 857 restored global POLYMORPH_BONUS_TYPE and public GetBonusType(DWORD) from DWARF DIEs 0x279020f and 0x27927ba, and routed the existing bonus selection through the original constants. No build or runtime run.

Source batch 858 restored the named dungeon callbacks FKillSectree, FNotice and FExitDungeon from DWARF and the reference. Existing notice formatting, entity filters, map guards and sector traversals are retained. Party.cpp uses a separate translation-unit-local FExitDungeon. No build or runtime run.

Source batch 859 restored SetRandomHP, SetRandomSP, GetRandomHP and GetRandomSP (DWARF DIEs 0x1eab0f2, 0x1eab110, 0x1eab12e and 0x1eab14b). Wired into level changes, point recalculation, resets, and player record save/load. Preserved RNG and write order and record width. No build or runtime run.

Source batch 860 restored FCountMonster (DWARF DIE 0x28e9b7b; CDungeon::CountRealMonster at 08185070). One mutable counter is passed by reference across the original per-sector neighbor traversal, retaining the same entity filter and map fallback. No build or runtime run.

Source batch 861 restored GetDropMetinStoneVnum and GetDropMetinStonePct (DWARF DIEs 0x1ead1ee and 0x1ead20b). The drop branch retains its initial VNUM guard, chance arithmetic, roll, and VNUM reread on success. Next verified source gap: quest::FSetQuestFlag at 08096a60. No build or runtime run.

Source batch 862 restored CHARACTER::IsStone (DIE 0x1eaacbc) in drop and dungeon kill paths, and quest::FSetQuestFlag (DIE 0x8e17f0; traversal 08096a60) at dungeon_set_quest_flag. Preserved descriptor and PC checks and the false flag-update argument. No build or runtime run.

Source batch 863 integrated IsStone at seven additional active call sites: prototype setup, dungeon membership, combat logging and behavior, and idle-state handling. Existing guards and monster-first membership checks remain. No build or runtime run.

Source batch 864 restored FPartyChat and FPartyClearReady (08146aa0 and 08146810) and CDungeon::CheckDestroy (DWARF 0x28b022f; inlined at 081825a0). The chat uses a fixed format and dungeon cleanup retains the reconstruction guard against duplicate timers. No build or runtime run.

Source batch 865 restored quest::FCinematicSender and FRunCinematicSender (081469a0 and 08146880) at their active party quest call sites. Both share the existing guarded packet-send routine. No build or runtime run.

Source batch 866 restored the original nonconst CHARACTER::IsMonster() (DWARF DIE 0x1eaac9f). Thirteen verified type checks use it across character, combat, and AI paths. No build or runtime run.

Source batch 867 restored the party FSetQuestFlag callback (DWARF 0x229d14c; traversal 08146b30 and solo branch 08146625). Internal linkage avoids collision with the dungeon callback of the same name. Existing descriptor and quest PC checks and the false SetFlag argument remain. No build or runtime run.

Source batch 868 restored the original nonconst GetTarget, IsWarp and IsGoto APIs (DWARF 0x1ead979, 0x1eaad13 and 0x1eaad30) at five active gameplay call sites. Existing target fallback and event and descriptor guards remain. No build or runtime run.

Source batch 869 restored FPartyAlignmentCompute (DWARF 0x2371150, traversal 08155040, callback 081550b8) in the character death path. The first pass counts nearby online members and the second applies the signed share. A zero count falls back to the killer. No build or runtime run.

Source batch 870 restored nonconst CHARACTER::IsBlockMode(BYTE) (DWARF 0x1eab860) at four social gameplay call sites, retaining the existing masks and evaluation order. No build or runtime run.

Source batch 871 restored the three nonconst CHARACTER killing-count methods (DWARF 0x1eae4a3, 0x1eae4bc, 0x1eae4d5), integrating clear and increment in initialization and three-way war scoring. Increment retains the existing modulo-32 arithmetic. No build or runtime run.

Source batch 872 restored nonconst CHARACTER::SetQuestBy(DWORD), GetQuestBy() and GetAffectContainer() (DWARF 0x1eadf11, 0x1eadf2f, 0x1eac13e). The two quest NPC callers and the death-time effect traversal use the original APIs. No build or runtime run.

Source batch 873 restored nonconst CHARACTER::IsWalking(), SetWalking(bool) and ResetWalking() (DWARF 0x1eaba12, 0x1eaba2f, 0x1eaba6b) at six movement and stamina call sites. The walk packet still carries the current mode byte. No build or runtime run.

Source batch 874 restored nonconst CHARACTER::SetStamina, GetStamina, SetMaxStamina and GetMaxStamina (DWARF 0x1eab006, 0x1eab024, 0x1eab0b7, 0x1eab0d5) across active character stat, movement, regeneration, item and player record paths. Arithmetic and walking transitions are preserved. No build or runtime run.

Source batch 875 restored nonconst CHARACTER::SetExp(DWORD) and SetGold(long) (DWARF 0x1eaae33 and 0x1eaca9b). Character setup, point changes, player loading and the experience distribution reset use the original setters. No build or runtime run.

Source batch 876 restored nonconst CHARACTER::SetPotionLimit(int) and GetPotionLimit() (DWARF 0x1eae2c1, 0x1eae2df). Character initialization, arena rounds and potion eligibility use the original APIs. Both decrements retain their placement relative to item consumption. No build or runtime run.

Source batch 877 restored nonconst CHARACTER::SetCubeNpc(LPCHARACTER) (DWARF 0x1eae6aa and 0x18dab56). Crafting open and close use the setter at the existing state transitions. No build or runtime run.

Source batch 878 restored ResetChangeAttackPositionTime, SetChangeAttackPositionTime, SetPartyRequestEvent and GetWeddingMap (DWARF 0x1ead885, 0x1ead89e, 0x1eac2d1, 0x1eae0ac). Existing timer arithmetic, request identity and cancellation guards, and wedding membership cleanup remain. No build or runtime run.

Source batch 879 restored nonconst CHARACTER::IsPC, IsNPC, IsDoor and IsBuilding (DWARF 0x1eaac65, 0x1eaac82, 0x1eaacd9, 0x1eaacf6). Marriage bonus and combat AI use IsNPC and IsBuilding at the verified type checks. IsPC tests descriptor presence. No build or runtime run.

Source batch 880 restored nonconst CHARACTER::SetSP(int) and SetMaxSP(int) (DWARF 0x1eaafcb and 0x1eab07c). Point changes, monster setup and player loading use the recovered setters. No build or runtime run.

Source batch 881 restored nonconst CHARACTER::IsWarping(), GetMyShopTime() and IsAlive() (DWARF 0x1eae12d, 0x1eae4f2 and 0x1eacd3e). Five active item and shop checks use the warp and cooldown APIs. No build or runtime run.

Source batch 882 restored GetUseSeedOrMoonBottleTime(), SetUseSeedOrMoonBottleTime() and IsPCBang() (DWARF 0x1eae3ca, 0x1eae3e7, 0x1eae18e). The item cooldown keeps separate pulse reads and the success update. Experience and drops use IsPCBang. SetPCBang return behavior remains unverified. No build or runtime run.

Source batch 883 restored nonconst CHARACTER::DetailLog() (DWARF 0x1eadd3f) and its active diagnostic command caller. The method toggles the existing detail-log flag. No build or runtime run.

Source batch 884 restored nonconst CHARACTER::IsOverTime(e_overtime) (DWARF 0x1eae702). The character click handler uses the original comparisons for three- and five-hour regional notices. No build or runtime run.

Source batch 885 restored nonconst CHARACTER::SetKillCount(int), GetKillCount() and DecKillCount() (DWARF 0x1eae44f, 0x1eae46d and 0x1eae48a). Battle, restart, diagnostics and forked quests use the original remaining-deaths APIs. The decrement retains 32-bit wrapping. No build or runtime run.

Source batch 886 restored nonconst CHARACTER::GetRefineTime(), SetRefineTime(), GetExchangeTime() and SetExchangeTime() (DWARF 0x1eae394, 0x1eae3b1, 0x1eae400 and 0x1eae41d). Refine and exchange updates sample the pulse separately for each call, and the cooldown checks preserve evaluation order. No build or runtime run.

Source batch 887 restored nonconst CHARACTER::GetSkillPowerByLevel(int, bool) (DWARF 0x1eae36d and 0x3d0792). GetSkillPower at 08073350 confirms the clamp to 0-40 and the skill group read before GetJob. The active caller retains its unique-item, guild and overflow branches. No build or runtime run.

Source batch 888 restored nonconst CHARACTER::IsItemLoaded() and SetItemLoaded() (DWARF 0x1eac447 and 0x1eac464). Inventory loading uses the methods at the existing duplicate-load guard and completion point. No build or runtime run.

Source batch 889 restored SetMallLoadTime(int), GetMallLoadTime(), CancelSafeboxLoad(), SetOpenSafebox(bool), GetSafeboxLoadTime() and SetSafeboxLoadTime() (DWARF 0x1eadaa2, 0x1eadac0, 0x1eada89, 0x1eae319, 0x1eae337 and 0x1eae354). Storage state transitions preserve their clock reads and existing guards. No build or runtime run.

Static coverage review after source batch 889: all 21 fishing and 19 mining ELF symbols checked have source definitions with matching parameter shapes. DWARF name comparison found no further confirmed omissions with active callers in CItem, CParty or CSkillProto. The next comparison should use full signatures and targeted body checks. No source change or build.

Static signature and record audit after batch 889: owner DWARF comparisons of CItem, CParty and CSkillProto found no confirmed signature mismatches after accounting for aliases, overloads and compiler parameters. CParty::SMember and TSkillUseInfo member names match. SSkillTable DIE 0x14d1037 spells its 32-bit fields DWORD and long; fixed-width declarations retain the 1475-byte packed boot record. Fishing DetermineFish and CShop::RemoveGuest match the reviewed original paths. No source change or build.

Source batch 890 restored FGuildCompare (0809eb60) and quest::FBuildLuaGuildWarList (080d1c30). Ranking uses the original comparison keys and read order. The Lua callback looks up the guild for each pair and appends eligible wars at one-based indices. No build or runtime run.

Source batch 891 restored FWarpAllToVillage, FWarpAllToVillageExceptMyEmpire and FWarpInMap (0805d2c0, 0805d3b0, 0805d4e0) through the shared character traversal. Village diagnostics read map index before name. The unfiltered map warp omits the empire read. Existing empire bounds are retained. No build or runtime run.

Source batch 892 restored FEmpireChatPacket (08103570) and ChatPacketCharacter_func (party traversal 08103400). Local chat captures the sender map for the delivery pass. The existing message bounds check and packet parser remain intact. No build or runtime run.

Source batch 893 restored FuncMatchChatEvent (081b5f10) and FuncMissChatEvent (MatchingQuest 081b40f0). Both walk scripts by index and resolve accepted entries again after IsScriptTrue, with a bounds check if evaluation changed the list. Empty conditions use the existing zero-length success path. No build or runtime run.

Source batch 894 restored FuncAggregateMonster, FuncAttractRanger and FuncPullMonster (08156c10, 08156ce0, 08156db0). Ranger attraction rereads the current victim and attack range at the original decision points. Existing coordinate arithmetic and movement update order are retained. No build or runtime run.

Source batch 895 restored quest::FBuildLuaWeddingMapList and marriage::CManager::for_each_wedding (080fa670). Traversal returns the callback by value. Lua rows preserve the active-wedding gate, two IDs, two names and one-based indexing, with the counter advanced after insertion. No build or runtime run.

Source batch 896 restored FuncFindChrForFlag (08081d10) and FuncFindChrForFlagBase (08081e50). Pickup reads the owner per candidate after affect checks. Capture rereads the owner for logging, both comparisons and scoring, retaining existing eligibility guards. No build or runtime run.

Source batch 897 restored FuncDeadSpawnedByStone (for_each 08137eb0, ClearStone 081322a0). The callback calls Dead before clearing the stone pointer. The current traversal erases by key because a child stone can unlink itself during Dead and invalidate the original iterator. No build or runtime run.

Source batch 898 restored FuncWeaken (080c81e0; command call at 080c3aa2). It computes distance before applying the all option and preserves the character-type filter and HP adjustment. No build or runtime run.

Source batch 899 restored NPartyPickupDistribute::FPartyMoneyTotaler, FPartyMoneyDistributor and FPartyFindOwnership (08181680, 08181720 and 08181870). Gold logging reads Y, X and PID after PointChange and before resolving the logger. The existing range, payout and ownership fallback remain. Coordinate fields retain unsigned 32-bit storage for wrapped deltas. No build or runtime run.

Source batch 900 restored FSendUserCount, FExitGuildWar, FPacket and FRemoveFlagAffect (traversals 0816cc80, 0816cd00, 0816cdb0, 0816ce20). Count messages retain original 30-byte buffers and send order. Packet delivery keeps the existing descriptor guard. No build or runtime run.

Source batch 901 restored FuncPurge (080c8100, DWARF 0xfaed61). It retains distance evaluation, command logging before the NPC check, and destruction through the character manager. No build or runtime run.

Source batch 902 restored FuncForgetMyAttacker (08154260), FGuildNameSender (0809d1d0) and FSendWarList (0809d590). Attacker reset retains the direct two-field VID comparison. Guild name delivery captures the name pointer once and both broadcasts preserve packet order and descriptor checks. No build or runtime run.

Source batch 903 restored FuncClearSync (traversal 08137dd0). The callback asserts each character and clears its sync owner without unlinking it during traversal. ClearSync still releases its own owner first and clears the list last. No build or runtime run.

Source batch 904 restored FGuildChat (traversal 08091550) for local delivery to online guild members. The existing literal format string and chat type are retained. No build or runtime run.

Source batch 905 restored FuncShout (traversal 0811cd30). Delivery checks for a character, then GM level and empire, and sends the existing type 6 message. No build or runtime run.

Source batch 906 restored FuncPacketView and its FuncPacketAround base (original traversal 081acd80). Sector, observer, excluded-recipient and descriptor checks are preserved, with self-send still evaluated after the view traversal. No build or runtime run.

Source batch 907 restored FSendChangeGrade (Boost holder 080929d0). Both existing post-query callers hold the same eight-byte TPacketGuild by value and relay it through DBPacket with the original header. No build or runtime run.

Source batch 908 restored FuncWho (original traversal 0815a3b0). Local counts are updated before reading empire. The existing bounds check and later peer totals are retained. No build or runtime run.

Source batch 909 restored the single leader pointer read in CParty::IsPositionNearLeader (081445d0). Both coordinate deltas use that leader; wrapped arithmetic and the strict 5000 cutoff remain. No build or runtime run.

Source batch 910 restored six nonconst building accessors: CObject SetLand, GetVnum, GetGroup and GetNPC, and CLand GetID and GetOwner (DWARF 0x1a9d796, 0x1a9d7cf, 0x1a9d7eb, 0x1a9d86c, 0x1a9d9b2, 0x1a9d9eb). Existing CObject GetID also matches the nonconst original. Object prototype fallbacks and all lookup guards are retained. No build or runtime run.

Source batch 911 restored nonconst CObject::GetVID() (DWARF 0x1a9d737). The three VID callers already use mutable objects. Two wall cleanup locals now match the nonconst GetID() restored in batch 910. No build or runtime run.

Source batch 912 restored npc_is_near read order (08120f20): current character, current NPC, Lua distance, then proximity calculation. The existing default, null checks and distance precision remain. No build or runtime run.

Source batch 913 restored npc_is_quest evaluation order (08120d20). It resolves the current quest name and index before reading NPC quest ownership. Existing NPC and current PC guards are retained. No build or runtime run.

Source batch 914 wired npc_is_pc (08120840) to the recovered CHARACTER::IsPC() descriptor check, retaining the null NPC guard. Local objdump confirmed the descriptor load. npc_open_shop and CLand::PutData matched their reviewed paths. No build or runtime run.

Source batch 915 restored SetWarMap pointer use (08131b60): read the previous map once, remove the member, store the incoming map and use that argument for registration. No build or runtime run.

Source batch 916 restored the leader lookup before the fresh nearby-member count and clamp in ComputePartyBonusExpPercent (081449d5–081449ed). No build or runtime run.

Source batch 917 restored the single party pointer read and signed 32-bit delay result in party_update_event (08141231 and 0814124e). The leader and descriptor guards remain. No build or runtime run.

Source batch 918 restored 32-bit timer arithmetic at CParty::Link (08142401–0814241d) and shares the delay calculation with party_update_event. No build or runtime run.

Source batch 919 restored CWarMap::UpdateUserCount read order (0816a7f1–0816a853). It reads team IDs and accumulated counts in team order, then snapshots the observer count for both messages. No build or runtime run.

Source batch 920 restored SummonToLeader position reads (08143680–08143692 and 08143767): each candidate reads Y, X and current map, and Show reads the map again after random selection. The existing noninserting member lookup and guards remain. No build or runtime run.

Source batch 921 restored the GetSkillPowerByLevel path in CParty::ComputeRolePoint (08143a85–08143ab2). The original skill-group read precedes GetJob. Role formulas are unchanged. No build or runtime run.

Source batch 922 restored 32-bit point deltas in CParty::ComputeRolePoint (0814394f–08143a4b and 08143b4e–08143d8a), preserving the separate comparison and fresh point read. No build or runtime run.

Source batch 923 restored CParty::Update clock sequencing, live leader dungeon reads, the pre-callback near flag and conditional EXP bonus store (08143e5e–081440fb). No build or runtime run.

Source batch 924 restored the direct member traversal and fresh character read in CParty::ChatPacketToAllMember (081419e0–08141a23). The descriptor guard and literal chat format remain. No build or runtime run.

Source batch 925 restored the low-dword VNUM comparison in CountMemberByVnum (08144708). The existing member filters and byte count are retained. No build or runtime run.

Source batch 926 restored the member pointer snapshot and wrapped role counters in CParty::SetRole (08143200, 081432ac, 081432ff). Existing role bounds and update guards remain. No build or runtime run.

Source batch 927 restored the 32-bit role-counter decrement in CParty::P2PQuit (08141ff2). It still runs after logging and before the player lookup. No build or runtime run.

Source batch 928 restored the 32-bit member-count diagnostic arithmetic in CParty::P2PJoin and P2PQuit (08141b8d and 08141fba). The local join log reads the name before the leader ID (08141b22–08141b27) and rereads the name for the member record. No build or runtime run.

Source batch 929 restored the CParty::Link diagnostic read order (08142287 and 08142291): character name, leader PID, log, then member pointer assignment. No build or runtime run.

Source batch 930 restored the live role reads in CParty::Unlink (081425ff and 081425bf). Existing dungeon and pointer cleanup order remains. No build or runtime run.

Source batch 931 restored party status logging reads at 08142c5e–08142c7a: recipient VID and name, then source VID and name. The original signed 32-bit diagnostic format and post-log recipient reload are preserved. No build or runtime run.

Source batch 932 restored the diagnostic read order in CParty::SendPartyInfoAllToOne (08142da5–08142dc7). It reads the recipient fields first, then reloads the linked member for source fields. No build or runtime run.

Source batch 933 restored CDungeon::SpawnGroup upper bounds to the map-relative center (0818486a, 08184874). Existing coordinate range checks and the first-success result are retained. No build or runtime run.

Source batch 934 restored signed 32-bit SpawnMobRange diagnostic arguments in the original VID, Y, X, name read order (0818d647–0818d681). The downstream range accepts equal endpoints and preserves both random draws. No build or runtime run.

Source batch 935 restored the four wrapped 32-bit group spawn bounds (0818dc2b, 0818dc5b, 0818dc88 and 0818dcba), with each coordinate read before its random draw. No build or runtime run.

Source batch 936 restored random spawn coordinate sequencing (0818ca73–0818cab7). Each axis reads the extent, draws its offset, then reads the map base before adding with 32-bit wrap. No build or runtime run.

Source batch 937 restored signed 32-bit arguments for the SpawnMob missing-sector, blocked and ban-PK diagnostics (0818d010). No build or runtime run.

Source batch 938 initializes dungeon potion and revive usage flags to false in Initialize, matching the supplied newer reference. The 2009 ELF does not initialize these flags. Both Lua argument guards already match the original (08094660, 08094710). No build or runtime run.

Source batch 939 restored the direct Lua number read in dungeon_select (080967e8–08096808), retaining the original low 32 bits of the truncated 64-bit value and existing zero-map behavior. No build or runtime run.

Source batch 940 restored dungeon_spawn_group Lua read order (0809614e–08096365), retaining the coordinate, radius and count limits before spawning. No build or runtime run.

Source batch 941 restored the missing-dungeon return in dungeon_spawn_group (08096237–08096241, 08096377–08096390). After numeric type checks, an absent dungeon returns one Lua zero before conversions. Active dungeon spawn limits remain. No build or runtime run.

Source batch 942 restored dungeon_spawn_unique argument sequencing and separate signed log conversion (08095690). It validates types first and rereads arguments after the dungeon lookup. No build or runtime run.

Source batch 943 restored Lua argument sequencing for dungeon_spawn_move_unique and dungeon_spawn_move_group (080954b0, 08095310), including separate signed diagnostic and DWORD spawn conversions. No build or runtime run.

Source batch 944 restored argument order 2 then 1 for logging and spawning in both dungeon door bindings (080950f0, 08095200). No build or runtime run.

Source batch 945 restored the Lua read sequences for named and waypoint mobs (08095fb0, 08095e20), preserving coordinate bounds. No build or runtime run.

Source batch 946 restored dungeon_spawn_mob Lua type checks, missing-dungeon return and optional argument reads (08095a70–08095ddb). Existing coordinate and count bounds remain. GhidraMCP was unreachable on port 8080, so evidence came from the original ELF via objdump. No build or runtime run.

Source batch 947 restored dungeon_spawn argument reads and separate signed diagnostic and DWORD placement conversions (08095830–08095936). No build or runtime run.

Source batch 948 restored Lua argument reads in six unique-monster bindings (08094920, 08094a50, 08094b80, 08094ed0, 08094fe0, 08095980), preserving the signed setter range checks. Both unique getters already matched. No build or runtime run.

Source batch 949 restored dungeon_join (080963d0), dungeon_jump_all (080943f0) and dungeon_set_dest (080947c0). Retained integer range checks and destroy the newly created dungeon if no current character exists. No build or runtime run.

Source batch 950 restored new dungeon jump sequencing (08093ec0 and 08094180). The reconstruction destroys the newly created instance if the character is absent or the coordinate range checks reject the inputs. No build or runtime run.

Source batch 951 restored explicit dungeon quest flag key assembly order (080930fa–08093169). The string guard stays in place. Static comparison confirmed the elimination Lua wrappers, notice and ordinary dungeon flag paths. No build or runtime run.

Source batch 952 restored signed 32-bit map index conversion for the Lua result (080936e6). The 45-entry registration table and sentinel match the original (081fdc2c). Static exit, kill-all and purge checks matched normal contexts. No build or runtime run.

Source batch 953 restored JoinParty registration and map lookup order (08182346–081823d3). The missing-map guard remains after registration. No build or runtime run.

Source batch 954 restored maximum-then-current HP reads (08183f5b–08183f6d) and the unique enemy pointer reload before the defense change (0818305f–0818307b). The HP setters matched. No build or runtime run.

Source batch 955 restored dungeon party counter insertion and 32-bit increment/decrement (08182670–081827a1). Zero-count removal and member cleanup remain in original order. No build or runtime run.

Source batch 956 restored the empty dungeon teardown delay calculation (0818260c–0818262a). The pending event guard remains. CheckEliminated exit and warp scheduling matched. No build or runtime run.

Source batch 957 restored SpawnRegen map setting read order (081849ab–081849c3). ClearRegen retains character reference invalidation before deleting records. No build or runtime run.

Source batch 958 verified Initialize field offsets (08182046–08182055) against the potion, revive and kill getters (08185030–0818506a) and restored the original write order. The reconstruction retains deterministic defaults for the kill counters and usage flags. Kill counting and dead-character cleanup matched. No build or runtime run.

Source batch 959 restored SpawnMob map lookup before logging and Y-before-X base reads in SpawnMob and SpawnNameMob (0818438a–081843f1, 08184530–08184539). Missing-map guards remain. No build or runtime run.

Source batch 960 restored the midpoint evaluation order in the SpawnUnique failure diagnostic (0818376f–081837b3). The message takes X then Y. No build or runtime run.

Source batch 961 restored SpawnMoveUnique destination-first short circuit (081830d1–08183155) and failure midpoint order (081834e6–08183513). No build or runtime run.

Source batch 962 restored the SpawnMoveGroup destination-first short circuit (08184b4e–08184bcc). No build or runtime run.

Source batch 963 corrects batch 933: SpawnGroup LEA instructions at 0818486a and 08184874 add one to the center via a scale-1 index, and 081848ad/081848b4 pass those upper bounds to CHARACTER_MANAGER::SpawnGroup. Existing coordinate range checks and first-success return remain. CountRealMonster matched. No build or runtime run.

Source batch 964 restored JumpToEliminateLocation field reads for a destination dungeon (081858f2–0818590d) and the map fallback (08185994–0818599d). No build or runtime run.

Source batch 965 made the original signed 32-bit delay and destination map stores explicit in both elimination setters (081857e0, 08185820), including on hosts with wider long. No build or runtime run.

Source batch 966 restored SendDestPositionToParty diagnostic read order (0818221a–0818223e). The normal destination packet path matched. No build or runtime run.

Source batch 967 restored outer sector iterator advancement before callbacks in KillAll (08184e58) and Purge (08184f58). Existing missing-map guards remain. No build or runtime run.

Source batch 968 restored the OX quiz timer ten- and five-second pulse arithmetic (08087cfd–08087d05, 08087dbf–08087dc4). No build or runtime run.

Source batch 969 restored ShowQuizList locale and signed quiz level read order (08087a87–08087ad2). No build or runtime run.

Source batch 970 restored OX Quiz delay preparation and fresh quiz lookups (08087f00–08088032). Level bounds and tracked timer ownership remain. No build or runtime run.

Source batch 971 restored CheckAnswer VID read before cheer RNG (0808826c–0808828f) and attendee iterator advancement after the branch callbacks (08088151–08088165, 0808837c–08088387). No build or runtime run.

Source batch 972 restored phase-local OX event manager access (08087bf6–08087c20, 08087d3f–08087d5e) and one answer read for scoring and announcement (08087d3f–08087e43). Timer ownership tracking is retained. No build or runtime run.

Source batch 973 restored OX Lua event manager access before status updates after script load or forced close (0813c82e–0813c843, 0813cbd7–0813cbe9). No build or runtime run.

Source batch 974 restored the forced OX command manager reload between CloseEvent and SetStatus (080cb346–080cb357). No build or runtime run.

Source batch 975 restored manager reads before quiz clear and after announcements in do_oxevent_open (080ca9ce–080ca9f3, 080caae4–080caaf9). Existing path and Lua-state checks remain. No build or runtime run.

Source batch 976 restored do_oxevent_quiz manager and argument conversion order (080caf7f–080cafaf). No build or runtime run.

Source batch 977 restored the OX attendee command count read before localization (080cb2a5–080cb2bb). No build or runtime run.

Source batch 978 restored OX reward reporting order and 32-bit total (080cb43a–080cb47e), using objdump. Defer Ghidra-dependent work until Ghidra is available again. No build or runtime run.

Source batch 979 restored the OX door manager reload before SetStatus (080cb145–080cb15a), verified via objdump. No build or runtime run.

Source batch 980 restored the regular OX close manager reload before FINISH (080cad79–080cad8b), verified via objdump. No build or runtime run.

Source batch 981 restored the guild member update order: level, offered experience, grade (0808f74b–0808f75c). Static ELF comparison only.

Source batch 982 restored guild AddMember diagnostic reads in offer, level, job, grade and PID order (0808b433–0808b468). Static ELF comparison only.

Source batch 983 restored guild experience carry at level 20: only clear experience when it reaches the level-20 threshold (0808f2ab–0808f2d0). Static ELF comparison only.

Source batch 984 restored the experience underflow path's unchanged guild level (0808f4c2–0808f4db). Successful updates still bound the level before indexing the experience table. Static ELF comparison only.

Source batch 985 restored the original CGuild::UpdateSkill byte-pointer signature (0808f100), matching the reference and existing caller. Skill copy and power calculations matched the ELF. Static comparison only.

Source batch 986 restored the non-const CGuild::GetTotalLevel signature and 32-bit accumulation (0808f9f0–0808fa3e). The newer reference declares this method const. Static comparison only.

Source batch 987 restored GetMasterCharacter's master PID snapshot before character manager lookup (0808f93c–0808f94f). Static ELF comparison only.

Source batch 988 restored the guild general counter update before storing the member flag (0808e4db–0808e4e8, 0808e592) with explicit 32-bit arithmetic. Static ELF comparison only.

Source batch 989 restored local guild logout iteration: notify the current member before advancing (0808bcd8–0808bce0). Static ELF comparison only.

Source batch 990 restored job-before-name diagnostic reads in the guild roster (0808c250–0808c283). Local and cross-core guild login/logout call order matched. Static ELF comparison only.

Source batch 991 made the original unsigned 32-bit skill index explicit in GetSkillLevel (0808f0e5–0808f0f6). Static ELF comparison only.

Source batch 992 restored the unsigned 32-bit index in CGuild::SkillUsableChange (0808fb7c–0808fba0). Static ELF comparison only.

Source batch 993 restored unsigned 32-bit skill indexing and prototype lookup in SkillLevelUp (0808e70c–0808e750). Static ELF comparison only.

Source batch 994 restored 32-bit skill ID handling in CGuild::UseSkill (0808e912–0808e9dc), retaining the skill array bounds and authorization checks. Static ELF comparison only.

Source batch 995 restored SkillLevelUp diagnostic reads in type, level and VNUM order (0808e84e–0808e888). Static ELF comparison only.

Source batch 996 restored the guild skill debug power snapshot before localization (0808eed3–0808eeef). Static ELF comparison only.

Source batch 997 restored the shared guild power snapshot for the insufficient-power check and message (0808eafa–0808eb55). Static ELF comparison only.

Source batch 998 made the ChargeSP capacity reads explicit (0808fa80–0808fa8e) and widened the subtraction before retaining the nonnegative capacity clamp. Static ELF comparison only.

Source batch 999 restored the current-power comparison before storing the computed maximum in ComputeGuildPoints (0808f0c0–0808f0d5). Static ELF comparison only.

Source batch 1000 restored ChangeGuildPower store-then-clamp order (0808f1b6–0808f1d0). The 32-bit sum and subsequent save and notification calls are preserved. Static ELF comparison only.

Source batch 1001 tied guild grade name validation and escaping to the nine-byte grade field and rejects null names. The ELF length check was reviewed at 0808d340. The existing eight-byte usable limit is retained for storage and packet compatibility. Static comparison only.

Source batch 1002 restored invalid guild grade diagnostics in ChangeGradeName and ChangeGradeAuth (0808d32a, 0808d587). Rank 1 returns silently and existing guards remain. Static ELF comparison only.

Source batch 1003 checked the guild rank-name notification against the 14-byte ELF packet and made the nine-byte wire field explicit. Existing termination and padding are retained. Static comparison only.

Source batch 1004 restored the grade-name row snapshot before authorization conversion in __P2PUpdateGrade (0808cfca–0808cffe). Existing row, rank and name bounds are retained.

Source batch 1005 restored the guild ID read before the building manager lookup in HasLand (0809009c–080900ae). The 39-byte guild info packet matched. Static ELF comparison only.

Source batch 1006 restored the unsigned 32-bit guild war map index store and the opponent-before-guild diagnostic reads (080af0d0–080af0f4). Static ELF comparison only.

Source batch 1007 restored the 32-bit guild war score store and explicit size-before-data reads for both score broadcasts (080adf85–080adfe0). Static ELF comparison only.

Source batch 1008 restored the fresh 32-bit guild war score read after logging in GetWarScoreAgainstTo (080ae08f–080ae09a). Static ELF comparison only.

Source batch 1009 restored the 32-bit guild war start timestamp store (080ae791–080ae796). State, announcement and entry prompt order matched. Static ELF comparison only.

Source batch 1010 restored the guild war end notification (state 7) before erasing the opponent record (080aeec6–080aef36) and moved the self check before lookup. Static ELF comparison only.

Source batch 1011 restored ReserveWar lookup before construction and the reservation log (080aed36–080aed6c, 080aee39). Existing reservations retain their type. Static ELF comparison only.

Source batch 1012 restored the guild war map index reread after saving the character exit location (080af243–080af260). Existing entry guards remain. Static ELF comparison only.

Source batch 1013 restored GuildWarEntryAsk map index rereads after logging and before spawn-position lookup (080af37f–080af3bb). Static ELF comparison only.

Source batch 1014 restored the Y-before-X reads in the missing war-map spawn diagnostic (0816c875–0816c89a). Normal spawn lookup and map creation matched; existing map ownership handling remains.

Source batch 1015 restored war reward calculation order (0816afc2–0816b015): winner potion share, base war price, then loser potion share. Arithmetic remains 32-bit. UpdateScore and CheckScore victory rules matched. Static ELF comparison only.

Source batch 1016 restored the second-then-first score reads and 32-bit winning guild return in GetWinnerGuild (0816a6f9–0816a717). Timeout gameplay matched. Static ELF comparison only.

Source batch 1017 restored end-event payload initialization order in CWarMap::SetEnded (0816b90f–0816b912). Ten-second scheduling matches. Static ELF comparison only.

Source batch 1018 restored the single war-map pointer snapshot in war_end_event (08169a8f–08169ad3), reused for clearing and destroying the same map. Static ELF comparison only.

Source batch 1019 restored the war-map index reread after logging in DestroyWarMap (0816cac0–0816cae0). The index is retained through map deletion for private-map cleanup. Static ELF comparison only.

Source batch 1020 restored team-two then team-one ID reads in CWarMap::Draw (0816b042–0816b053). ExitAll and OnShutdown traversal matched. Static ELF comparison only.

Source batch 1021 restored the UsePotion guild-before-prototype check and the 32-bit potion price read before team lookup (0816a737–0816a750). Static ELF comparison only.

Source batch 1022 restored 32-bit war map team member count arithmetic in AppendMember and RemoveMember (0816a7c1, 0816a7d6). Joiner insertion precedes increment as in the ELF. Static comparison only.

Source batch 1023 restored explicit 32-bit observer count updates before logging (0816ab2a, 0816ae28). Static ELF comparison only.

Source batch 1024 restored the 32-bit opponent ID returned by GetGuildOpponent (0816a6cf). Team matching and missing-guild handling are preserved. Static ELF comparison only.

Source batch 1025 restored 32-bit war-map team ID storage and lookup (08169f60, 0816a5c6–0816a5d4). Static ELF comparison only.

Source batch 1026 restored 32-bit accumulated war-map participant IDs and count return (0816a77a, 0816a7a2–0816a7bc). Static ELF comparison only.

Source batch 1027 restored the 32-bit guild ID return in CWarMap::GetGuildID (0816a61e). Flag reset and respawn order matched the ELF. Static comparison only.

Source batch 1028 restored guild-ID-before-spawn-pointer diagnostic reads for AddFlag and AddFlagBase (0816bc5c–0816bc8f, 0816be04–0816be32). Existing spawn failure guards remain. Static ELF comparison only.

Source batch 1029 restored 32-bit flag base coordinate comparisons and base-before-flag reads (0816bfae–0816bfcb). Existing index and null checks remain. Static ELF comparison only.

Source batch 1030 restored second-before-first guild ID reads in the war timeout announcement diagnostic (0816af19–0816af37). Static ELF comparison only.

Source batch 1031 restored the 32-bit war-map start tick store and the original GetType call before flag setup (0816a14f–0816a16b). Start timer and flag spawn order matched. Static ELF comparison only.

Source batch 1032 restored CheckScore score reads and tie-before-threshold evaluation (0816b6b6–0816b6d9), plus size-before-data reads in SendScorePacket (0816b552–0816b591). Static ELF comparison only.

Source batch 1033 restored per-map locale checks in LoadWarMapInfo (0816c60a, 0816c699). All four map IDs and 24 X/Y coordinates match the ELF. Zero Z and duplicate-entry ownership guards are retained. Static comparison only.
Source batch 1034 restored OX CloseEvent return coordinates for empire IDs outside 1–3 and the Y-before-X lookup for valid IDs (080886fd–08088727). Static ELF comparison only.

Source batch 1035 corrected the earlier dungeon SpawnGroup upper-bound interpretation. The original uses base plus the requested coordinate as the inclusive upper bound on each axis (0818485a–08184877). Static ELF comparison only.

Source batch 1036 restored signed 32-bit land master spawn coordinates in CLand::ClearLand (08109a32–08109a60). Static ELF review also confirmed the checked normal paths in building reconstruction, OX entry and scoring, and dungeon group spawning.

Source batch 1037 restored the signed 32-bit pickaxe refinement stage and post-roll log field reads (080ef798–080ef80a, 080ef980–080ef9d1). The reconstruction retains its owner validation. Static ELF comparison only.

Source batch 1038 restored separate fishing catch-length reads for the test notice, high score and catch log, plus the catch log field read order (08083789–0808381e, 0808391c–0808393b). Static ELF comparison only.

Source batch 1039 restored the grilling name snapshot before locale lookup (0808419a–080841c0). The original file inventory was cross-checked against game and library paths. Apparent gameplay gaps resolve to existing library or FSM sources and excluded Monarch registrations. File presence does not establish complete function coverage. Static ELF comparison only.

Source batch 1040 restored CHARACTER_MANAGER::NoticeInMap (0818ecf0), with direct per-sector traversal and descriptor filtering. Static ELF comparison only. The original item-use call at 08178cc2 still needs to be mapped to its reconstructed branch. Three other original callers are in the excluded Monarch area.

Source batch 1041 restored the fishing random draw and search for valid rows with nonpositive totals (0808284c, 08082a07). Invalid row indices and one-past search results still return zero. The pending NoticeInMap item-use caller at 08178cc2 was identified as the excluded Monarch heal branch. All four direct original callers belong to Monarch. Static ELF comparison only.

Source batch 1042 restored value-before-type reads when shifting item attributes and logging rare attribute additions (08187748–0818774c, 08187cb0–08187cf5). Static ELF comparison only.

Batch 1043 audit verified the original dungeon_set_dest x87 conversion (08094842–080948a6). The source keeps the existing range guard because the supplied reference client uses signed coordinate subtraction and abs on the destination marker. Valid coordinate handling matches the ELF. The original client was outside this audit.

Source batch 1044 restored the original 32-bit mob lookup and signed socket values in CPolymorphUtils::GiveBook (0816f040). The book grant still precedes mob validation. Static ELF comparison only.

Source batch 1045 restored marriage proximity checks after the diagnostic, the original near/far notification branches, and the previous love-point snapshot (080b04b7–080b05bf). Descriptor guards remain. Static ELF comparison only.

Source batch 1046 checked wedding entry and exit against the ELF (0816faf0, 0816fb70, 0816fd60). SendLocalEvent now passes the generated music command as literal text, as the existing broadcast does. This is an intentional departure from the original format argument. Static comparison only.

Source batch 1047 restored the love-point-before-spouse reads in marriage login (080b02f0, 080b0320) and filename-before-enabled Lua argument conversion for wedding music (080f9fbd–080f9fd0). Static ELF comparison only.

Source batch 1048 restored signed 32-bit arena observer coordinate scaling and Y-before-X reads after SaveExitLocation (080bce95–080bcec3). Static review also confirmed duel startup and the five arena Lua bindings.

Source batch 1049 restored the post-log riding flag read in horse_is_riding (080e36a0–080e36c3). The existing null guard remains. Static ELF comparison only.

Source batch 1050 restored current-before-maximum HP and SP reads at duel startup (080bb932–080bb9e9). Static ELF comparison only.

Source batch 1051 restored per-notification locale lookups during arena timeout and missing-opponent handling (080bb55a–080bb632, 080bb6d3). This avoids retaining a pointer to the shared locale buffer across another lookup. Static ELF comparison only.

Source batch 1052 restored per-send locale lookups during arena readiness and literal zero return coordinates outside empire IDs 1–3 (080bb179, 080baf98, 080bbd6a). Static ELF comparison only.

Source batch 1053 restored signed 32-bit arena round-restart placement, scaling Y and X before reading each player map index (080bab95, 080bac07). Static ELF comparison only.

Source batch 1054 restored per-recipient winner name and locale reads and per-send round transition localization in CArena::OnDead, plus current-before-maximum refill reads in the round restart (080bac4b–080bad29). Static ELF comparison only.

Source batch 1055 restored current-before-maximum arena cleanup refill reads and captured the empire before clearing the arena pointer (080bbb98–080bbc4b, 080bbc81–080bbd27). Static ELF comparison only.

Source batch 1056 restored rare percentage, count and VNUM read order for weighted mob drops (081945c8–081945de). Static ELF review confirmed the checked mob group and item group selection paths.

Source batch 1057 restored the motion diagnostic read order (08114c64–08114cbb). Paths without a slash use the whole filename as a deliberate fallback. Mob skill loading and mob group weight parsing matched the checked ELF paths.

Source batch 1058 restored the movement-duration diagnostic field order (0812dcce–0812dd35). Static ELF comparison only.

Source batch 1059 restored movement-speed displacement-before-duration and missing-motion race-before-name reads (0812da3c–0812da99). Static ELF comparison only.

Source batch 1060 restored Y-before-X character position reads in SetRotationToXY (0812d3bf–0812d3e8). Static ELF comparison only.

Source batch 1061 restored the stamina-start notification read order: half-consumption check, current stamina, then pulse rate (081342ad–081342fa). Static ELF comparison only.

Source batch 1062 restored the mounted motion fallback walking-state reread and motion load displacement-before-duration order (0812db33–0812db64, 081142d6–081142e6). Static ELF comparison only.

Source batch 1063 restored StateMove interpolation field reads: start X then destination X, followed by destination Y then start Y (0808028f–080802f7). Static ELF comparison only.

Source batch 1064 restored movement deadline evaluation in EncodeInsertPacket: read duration and start time, add as uint32, then read the clock and subtract (081283b7–081283ce). Static ELF comparison only.

Source batch 1065 restored CalculateMoveDuration coordinate snapshots and reuse (0812dc1b–0812dc40). Static ELF comparison only.

Source batch 1066 restored the player recovery elapsed-period calculation before DistributeSP (0812d086–0812d0ac). The poison delay and NPC recovery checks matched the reviewed ELF paths.

Source batch 1067 restored the movable-position lookup field order for ground gold drops (0814864b–08148670): Y, X, then map index. Static ELF comparison only.

Source batch 1068 restored separate mob table reads for the maximum and minimum gold roll bounds in all three RewardGold branches (08148952, 08148c86, 08148ec6). Static ELF comparison only.

Source batch 1069 restored the 32-bit experience rollover when DistributeExp credits the spawning stone (0815329d). Static ELF comparison only.

Source batch 1070 restored separate centralized party EXP recipient reads for presence, range and payout, with the original coordinate read order (0815607d, 0815614b–081561cf). Static ELF comparison only.

Source batch 1071 restored the original party EXP payout range read order in FPartyDistributor (081575a2–081575b9). The weighted split and highest-damage share rounding matched the checked ELF paths.

Source batch 1072 restored damage entry snapshot and iterator advancement before attacker lookup in DistributeExp (081531c0–081531e2). Static comparison also confirmed XP pool consumption before the zero-total early return.

Source batch 1073 restored party EXP totaler coordinate reads (08156fa2–08156fb4). The inclusive distance cutoff and locale-dependent weight rules matched the ELF.

Source batch 1074 restored the attacker range coordinate read order in DistributeExp (08153720–08153741), preserving the inclusive 5000 cutoff and wrapped differences.

Source batch 1075 restored the WAEGU gold branch position and roll bound read order, and the boss scatter X offset store before the Y draws (0814a05b–0814a0be, 081490b7). Static ELF comparison only.

Source batch 1076 restored the Y-before-X coordinate snapshots for party gold distribution (0817eb2a–0817eb3f). The original recipient still receives the remainder after the group payouts.

Source batch 1077 restored the regular reward itemDrop gate before its position snapshot (08149514–08149569) and separate mob-table reads for the boss gold diagnostic (08148fa0–08148fb9). Static ELF comparison only.

Source batch 1078 restored member Y-before-X reads for party gold counting and payout range checks (081816f0, 0818174c). The signed inclusive 5000 cutoff matched the ELF.

Source batch 1079 restored killer-before-victim name reads for the quest-drop diagnostic (08194c18–08194c48). The checked seasonal rates matched the ELF.

Source batch 1080 restored live common-drop row access and level-drop VNUM-before-count reads (0819377b, 08193c68–08193c78). Static ELF comparison only.

Source batch 1081 restored VNUM-before-count reads when creating thief glove drops (08193e82–08193ea3). Static ELF comparison only.

Source batch 1082 restored VNUM-before-count reads for regular mob drop-group rows (08193a35–08193a48). The Metin stone drop path matched the checked ELF.

Source batch 1083 restored parsing of the sixth common-drop column with atoi (08195c40), while retaining the original four-field stored record. Common and ETC drop parsing and probability scaling matched the reviewed ordinary data paths.

Source batch 1084 restored the original MINMAX call for kill-group rarity (08196fbe–08196fd7). Static review confirmed numbered rows 1–255, the regular drop default count, and monster probability conversion.

Source batch 1085 restored treasure gold and EXP log snapshots after PointChange: X, Y, then player ID before resolving the logger (08180ada–08180aee, 08180bb9–08180bcd). Static ELF comparison only.

Source batch 1086 restored the item reward name read before localization for treasure and gift box notices (0817d180, 0817d1d1). The checked key and chest consumption sequence matched the ELF.

Source batch 1087 restored explicit key-before-chest value reads (0817cbb3–0817cbc8). Gift-box count and consumption matched the checked ELF.

Source batch 1088 restored campfire placement coordinate evaluation in original Y-before-X order (0817c000–0817c039). Static ELF comparison only.

Source batch 1089 restored separate campfire position reads for water and spawning, with map reread after rotation RNG (0817c091–0817c1f3), and blend duration/value snapshots before effect lookup (0817d443–0817d465). Static ELF comparison only.

Source batch 1090 made campfire expiry delay and item consumption explicitly 32-bit (0817c226–0817c258). Retained the reconstruction's VID-based event payload and spawn-success guard.

Source batch 1091 restored the polymorph ball skill reread for nonzero duration, separate from the gate and attack bonus reads (08181364, 08181458, 08181389). Static ELF comparison only.

Source batch 1092 restored the 32-bit polymorph race read from socket zero (0818113d–0818115c). Skill-book consumption matched the checked ELF paths.

Source batch 1093 normalized the mount and polymorph race arguments to the original unsigned 32-bit width before comparison and storage (08135b10, 08136010). Static ELF comparison only.

Source batch 1094 restored horse departure rotation with the original extended angle addition before float rounding, and original coordinate read order (0816de3b–0816dec3). Static ELF comparison only.

Source batch 1095 restored horse Show Z-before-map reads and Lua mount VNUM conversion before the negative-duration fallback (0816db82–0816dbad, 081619d2–08161a9e). Static ELF comparison only.

Source batch 1096 restored the horse health maximum snapshot before reading current health and the death diagnostic consume-before-regen event reads (080c89c9–080c89ee, 080c8a3c–080c8a5a). Horse status grades and decay timing matched the checked ELF paths.

Source batch 1097 restored the horse stamina maximum snapshot before reading current stamina (080c8546–080c856b). Static ELF comparison only.

Source batch 1098 restored post-update stamina snapshots before delay calculation in both horse stamina callbacks (080c868f–080c8693, 080c873e–080c8756). Static ELF comparison only.

Source batch 1099 restored horse revival stat reads from a single level snapshot before either stat is stored (080c83ee–080c8420). The original stamina timer creation paths matched in static review.

Source batch 1100 restored SetHorseLevel stat snapshots and clearing the health decay deadline before storing stamina and health (080c8ac8–080c8af1). Static ELF comparison only.

Source batch 1101 restored the current-before-maximum reads in the Lua horse health and stamina percentage queries (080e3bef, 080e3d4f). Existing null and denominator guards remain. Static ELF comparison only.

Source batch 1102 restored sequential potion recipe ingredient deductions, bottle lookup after ingredients, and reading the recipe chance after the roll (0817a76a–0817a959). Missing bottles return failure and aliased stacks retain lifetime guards.

Source batch 1103 restored the Cube_make random draw before reading the recipe success percentage (080fce34–080fce3c). Static ELF comparison only.


Source batch 1104 aligned shared potion ingredient deduction and shortage notice (0817a7e9, 0817a836), continuous potion duration and magnitude (0817a5ac-0817a6b5), Cube diagnostic name and cell (080fcff7, 080fd13e), and 32-bit land owner updates (08108196-081081a5). Static ELF comparison only. Ghidra work remains deferred.
Source batch 1105 restored 32-bit lookup keys for building object groups (0810796b-0810798f) and guild land ownership (081091dc-081091df). Static ELF comparison only. Ghidra work remains deferred.

Batch 1105 also restored immediate potion effect reads, post-effect bottle type and count-before-limit sequencing (08179d0e-08179ffb), plus Cube distance coordinate reads (080fc494-080fc4ab).

Source batch 1106 restored the 32-bit land object VNUM lookup (081079cb-081079ee) and Cube success and failure log read order (080fce91-080fcedf, 080fcf3a-080fcf7d). Static ELF comparison only.
Source batch 1107 restored the 32-bit persistent land object ID lookup (08107922-08107930) and the fishing practice notice maximum-before-progress reads (08082dd4-08082ddb). Static ELF comparison only.

Batch 1107 also restored 32-bit wrapping potion-limit decrements (08178f00, 08179e23).

Source batch 1108 restored 32-bit building VID lookups (08107a92-08107aa0, 08109702-08109710) and the fishing start VID-before-rotation snapshot (080830db-080830e7). Static ELF comparison only.

Source batch 1109 restored 32-bit land and building prototype lookups (08109042-08109050, 08108da2-08108db0) and signed nonzero horse percentage division before clamping (080e3c17, 080e3d77). The zero guard remains. Static ELF comparison only.

Batch 1109 also restored the 32-bit ID used when removing a land object (08107ad9-08107aec).

Source batch 1110 restored 32-bit object IDs in the building manager removal path (08109689-081096e8) and post-roll fishing rod refinement log snapshots (08084314-0808433f). Static ELF comparison only.

Source batch 1111 restored the 32-bit guild master and player ID comparison for horse race selection (0816df23-0816df31). Static ELF comparison only.

Batch 1111 also restored 32-bit occupancy-bound arithmetic when showing building objects (08106c20-08106d2c). The fishing timing table matched all 155 ELF values (0823c0e0).

Batch 1111 also restored horse-feed food name lookups after inventory counting and feeding, including the separate Korean suffix lookup (080a8a57-080a8ae2, 080a8bb0).

Source batch 1112 restored 32-bit building occupancy removal bounds (08106890-0810690d) and localized fishing simulation row read order (08083cbe-08083cc3). Static ELF comparison only.

Batch 1112 also restored signed 32-bit building occupancy rotation conversion for showing and removing objects.

Source batch 1113 restored horse stat updates using live deltas without preclamping (080c62b6-080c62f8) and horse state report read order (080c5f64-080c6036). Parsing remains bounded to signed 32-bit inputs and zero maxima display zero percent. Static ELF comparison only.

Batch 1113 also restored 32-bit wall-block VNUM subtraction and comparison (0810a630-0810a676).

Source batch 1114 restored the fishing table tab-error diagnostic line ending (0808274e-08082759). Short successful reloads matched the original zero-fill behavior. Static ELF comparison only.

Batch 1114 also restored 32-bit VNUM arithmetic for wall block creation (0810a3bb-0810a3d3). Wall geometry and block count matched the ELF.

Source batch 1115 restored map-relative land lookup snapshots and signed 32-bit map keys (081090d8-0810911c), plus guild NPC spawn coordinate snapshots (0810707e-081070e4). Static ELF comparison only.

Source batch 1116 restored 32-bit wedding armor VNUM checks (0816d67d-0816d68d) and branch-specific mining refinement log snapshots (080ef89a-080ef8b7, 080efa67-080efa89). Static ELF comparison only.

Batch 1116 also restored 32-bit horse summon race forwarding (0816da97-0816dadd) and land list map filtering (08109886).

Source batch 1117 restored land owner update logging from the same 32-bit snapshot sent to the DB (081081d0-0810821e). Static ELF comparison only.

Batch 1117 also restored wrapping wall center arithmetic (08109cc9, 08109cd5) and ore drop log prototype-before-count reads (080ef415-080ef418).

Source batch 1118 restored 32-bit wrapping offsets for wall placement and test-server corner markers (08109cc0-0810a34f). Static ELF comparison only.

Batch 1118 also restored pc_polymorph character lookup before Lua race and duration conversion (08161826-081618bc).

Source batch 1119 restored 32-bit building VID storage before object registration (08106c13-08106c19, 081078bc). Static ELF comparison only.

Source batch 1120 restored Lua polymorph book character, argument and current item lookup order (08163db8-08163ed1, 08163fbb-08163ff0). Static ELF comparison only.

Batch 1120 also restored deletion log IDs from the 32-bit request key (08108083-081080d8) and polymorph level subtraction before reading the book socket (0816edee-0816edf3).

Source batch 1121 restored the original 32-bit polymorph bonus race key (0816ec3f-0816ec78). The existing nonpositive-chance RNG skip matched the ELF (0816edff-0816ee25). Static ELF comparison only.

Source batch 1122 restored 32-bit blend item VNUM storage and lookup (080e64f0-080e6506). Blend selection and independent effect rolls matched the ELF. Static ELF comparison only.

Batch 1122 also restored the 32-bit map comparison in marriage_in_my_wedding (080fa384-080fa386), retaining the missing-record guard.

Source batch 1123 restored the 32-bit wedding map lookup key (08170252-08170260). Blend record layout matched the 48-byte ELF record. Static comparison only.

Source batch 1124 restored unsigned 32-bit wedding map classification (08170213-0817022a). Static ELF comparison only.

Batch 1124 also restored the wedding end event's map snapshot before the stage check (0816f5d2).

Source batch 1125 restored 32-bit wedding recall map division and the destination map reread after SaveExitLocation, followed by Y and X (080b09a4-080b0a12). Static ELF comparison only.

Batch 1125 also restored 32-bit spouse comparisons for wedding guest gifts (0816fa2e-0816fa39).

Source batch 1126 restored wedding NPC placement and diagnostic coordinate reads (081703f2-081704bd) and Lua join ID conversion before character capture (080f9356-080f93b0). Static ELF comparison only.

Source batch 1127 restored the accessory downgrade test-server diagnostic owner and name reads before localization (08168943-0816896b). Static ELF comparison only.

Source batch 1128 restored accessory socket result log reads after the result notice (0817b2e8-0817b332, 0817b65f-0817b6a9). Static ELF comparison only.

Source batch 1129 restored unique item expiry log name and ID capture order (081684b0-081684dc). Static ELF comparison only.

Source batch 1130 restored 32-bit target ID formatting in accessory socket operations (0817b256, 0817b503) and the distinct logger lookup order on success and failure (0817b2fd, 0817b3b8, 0817b665, 0817b73c). Static ELF comparison only.

Source batch 1131 restored the signed int promotion in the item attribute diagnostic (08187302-08187324) and corrected the PutAttribute const pointer ABI comment. Static ELF comparison only.

Batch 1131 also restored signed 32-bit stone cleaner socket comparisons (0817aa50-0817aa69). Source ID remains captured before stack consumption for lifetime safety.

Source batch 1132 restored CLEAN_SOCKET post-consumption log field order and 32-bit target ID formatting (0817aa9a-0817aafc). The source ID is reread only when SetCount reports the item survived. Static ELF comparison only.

Source batch 1133 restored RemoveItem audit field order and 32-bit count formatting (08192c37-08192c8d). Static ELF comparison only.

Batch 1133 also restored the 32-bit special item group lookup key (08198c92-08198ca0) and signed 32-bit clock division before the attribute cooldown comparison and persistence (0817adbd-0817ade0, 0817aed9).

Source batch 1134 restored the 32-bit reverse refinement VNUM lookup key (08198c42-08198c50). Static ELF comparison only.

Batch 1134 also restored 32-bit item count storage and zero-count lifecycle branching (081668a9-081668b4).

Source batch 1135 restored 32-bit item ID and VID lookup keys (08192ff2-08193000, 08193042-08193050). Item count initialization matched the ELF. Static comparison only.

Batch 1135 also normalized item ID creation and ID/VID setters to keep registration and lookup keys consistent with i386.

Source batch 1136 restored 32-bit prototype VNUMs in CreateItem and RealNumber, and the original DWORD VID counter declaration with explicit 32-bit increment wrapping. Static ELF and DWARF comparison only.

Batch 1136 also bounds prototype search to valid vector indices. Empty tables and misses above the highest VNUM return no match without reading past the table.

Source batch 1137 restored the original RealNumber midpoint sequence, including duplicate VNUM selection (081930e0-08193113), with an index guard before each prototype access.

Batch 1137 also restored signed 32-bit flag storage in SetFlag, consistent with item initialization and prototype assignment (08166393, 08166719-0816671c).

Source batch 1138 restored 32-bit lookup keys for mob prototypes, mob groups and weighted mob group selection (080f6542, 080f6b72, 080f6ac2). Static ELF comparison only.

Batch 1138 also restored 32-bit last-owner PID assignment (08166b57-08166b5d) and the original const-vector GetTable overload (DWARF DIE 0x2b419da).

Source batch 1139 restored 32-bit mob regeneration statistics keys at IncRegenCount entry (080f66c2, 080f6870, 080f6904). Static ELF comparison only.

Batch 1139 also restored original inline GetVIDMap, GetVecProto and GetMemberCount APIs using DWARF declarations and matching reference bodies.
Roulette uses GetVIDMap again (inline DWARF DIEs 0x72ac6e, 0x72acc3, 0x72ad4a).

Source batch 1140 restored CMobGroup::Create and AddMember and their LoadGroup call sites (080f7268, 080f729d, 080f74c4), using original DWARF and reference bodies.

Batch 1140 also restored CMobGroupGroup construction, AddMember and GetMember at their original loader and selection sites (080f6d0e, 080f6e55, 080f6ae7). The loader retains weight 1 per accepted member.

Source batch 1141 restored the original declaration-only CItem::IsTwohanded API (DWARF _ZN5CItem11IsTwohandedEv and reference item.h). The original ELF and reference contain no function body or call site.

Batch 1141 also restored the declaration-only CMobManager::Destroy API (DWARF DIE 0x1832644 and reference mob_manager.h). No definition was found.

Batch 1141 also restored the protected CItem::EquipEx declaration and public CMobManager::RebindMobProto access, based on original DWARF and the reference headers.

Source batch 1142 restored the original public CItem::SetValue(DWORD, long) declaration (DWARF _ZN5CItem8SetValueEml). No original definition was found.

Source batch 1143 restored CMobItemGroup::GetOne and CSpecialItemGroup::Contains from DWARF and reference headers. GetOne bounds-checks the result; CreateDropItem retains its nullable selection path and single roll.

Source batch 1144 restored the mining::GetFractionCount header declaration. The existing implementation and OreDrop calls match the exported original symbol at 080ef260.

Source batch 1145 restored 32-bit mining vein and raw ore lookup keys (080ef200, 080ef230). DWARF confirmed the checked fishing and mining implementation helpers belong in their translation units.

Source batch 1146 restored the original 32-bit fish ID and elapsed-time inputs in fishing::Compute (080831dd-080831f4). Static ELF comparison only.

Source batch 1147 restored signed 32-bit blend apply, duration and magnitude reads (0817d443-0817d44b) while retaining the apply-index guard.

Batch 1147 also restored the 32-bit rod practice comparison in RefinableRod (08084271).

Source batch 1148 restored fishing level capture before reading the selected fish ID on the bite path (0808362c-08083649). Fish length draws and arithmetic matched the ELF.

Batch 1148 also restored the 32-bit bait socket check before fishing starts (08130218-0813021b).

Source batch 1149 restored the original 32-bit bait replacement check and item name capture before localization (08179696-081796c5). Static ELF comparison only.

Source batch 1150 restored hair item VNUM capture before job lookup (08180ff3-08180ffb). All 13 ore lookup rows matched the ELF.

Source batch 1151 restored the 32-bit skill prototype lookup key (080e59a2-080e59b0). Static ELF comparison only.

Batch 1151 also restored live wear-flag reads after equipment eligibility checks in FindEquipCell (08166f08-08166fa7).

Source batch 1152 restored EquipTo immunity scanning via the current owner and the original occupied-slot reread (08167959-0816795f, 081679c7-081679d5). Static ELF comparison only.

Source batch 1153 restored Unequip diagnostic field reads and original 32-bit item ID formatting (08167a92-08167ad2). Its immunity scan matched the ELF.

Batch 1153 also restored 32-bit immunity masks at SetImmuneFlag and IsImmune (08064165-0806416b), preserving the original DWORD declarations.

Source batch 1154 restored the declaration-only TSkillUseInfo::ResetHitCount API. Original DWARF records the declaration with no emitted body or inline use.

Batch 1154 restored the occupied-slot reread in EquipItem (0817f78e-0817f7a6). Existing swap eligibility guards remain.

Source batch 1155 restored non-const CHARACTER GetPosition and GetRotation signatures from original DWARF (0x1eaaf0c, 0x1eab703). Existing game call sites use mutable characters. Static comparison only.

Source batch 1156 restored the occupied-slot diagnostic reread and name/VNUM capture order in EquipTo (081679e7-08167a13). Swap failure recovery is retained as an explicit compatibility difference.

Source batch 1157 restored original non-const stat, mount and polymorph getters from CHARACTER DWARF, and the single prototype snapshot in the EquipTo invalid-slot diagnostic (08167890-081678af). Static source and ELF comparison only.

Source batch 1158 restored FindEquipCell limit value capture before the limit type and character stat lookup (08166ec6-08166ed8). Static ELF comparison only.

Batch 1158 also restored non-const GetParty, GetGuild, GetSkillGroup and GetEmpire signatures from original CHARACTER DWARF.

Source batch 1159 restored the 32-bit motion set lookup key (08113382-08113390). Static ELF comparison only.

Batch 1159 also restored original public motion iterator aliases (DWARF 0x1be4edc, 0x1be4ee7, 0x1c05b2d).

Source batch 1160 restored the original non-const GetLastShoutPulse and GetQuestNPCID signatures (DWARF 0x1eaad4d and 0x1eade83). Current callers use mutable characters.

Source batch 1161 restored signed 32-bit coordinate comparisons and destination writes in CHARACTER::Goto (0812d828-0812d93c). Static ELF comparison only.

Batch 1161 restored marriage::TMarriage::IsOnline and its seven source call sites from original DWARF (0xd7f366) and the reference body.

Source batch 1162 restored 32-bit affect type semantics across insertion, lookup and removal (0813d3af, 0813d3da, 0813e397-0813e49b), and signed 32-bit coordinates in Move (0812dd4f-0812dd88). Static ELF comparison only.

Source batch 1163 restored TAffectFlag::IsSet, Set and Reset at their original inline sites (0813eb6a, 0813e974, 0813e8e1), with bounds checks for the two 32-bit words. AddAffect normalizes value, duration, flags and SP cost before applying the original predicates and stores (0813e4aa-0813e4b9).

Source batch 1164 restored the TAffectFlag zero and two-word constructors using original DWARF and reference bodies. The original DWORD member declarations are retained, with constructor inputs normalized to 32 bits.

Batch 1164 also restored the constructor call in CHARACTER::Initialize (08126f0a-08126f20) and typed AddAffect diagnostic arguments to match their format and original widths.

Source batch 1165 restored TAffectFlag copy assignment at the original initialization site (08126f18-08126f20), copying the two low 32-bit words.

Batch 1165 also restored the signed 32-bit clock comparison for hair affect expiration (0813dac4-0813dad3).

Source batch 1166 restored TAffectFlag equality and inequality at the four original inline sites (0813d950, 0813dbeb, 0813e238, 0813e7df).

Batch 1166 also restored the three original affect enums from DWARF (0x20f8e92, 0x20f9021, 0x20f912a).

Source batch 1167 reconnected recovered affect enum names to their original type and flag checks in char_affect.cpp. Numeric values, premium ranges and operation order are unchanged.

Source batch 1168 restored the signed 32-bit SP cost snapshot for the affordability check and the separate debit reread in ProcessAffect (0813db95-0813dba5, 0813dd1c-0813dd34).

Source batch 1169 restored the shared SendAffectAddPacket declaration in affect.h and its use from char_battle.cpp (0813d350, 0814b6cd).

Source batch 1170 restored FindAffect iterator advancement before matching type and apply point (0813d3d8-0813d3f8). Static ELF comparison only.

Batch 1170 restored the shared SendLog declaration in cmd.h and replaced four local declarations (ELF 080c05f0; fishing loader calls at 08082765 and adjacent sites).

Source batch 1171 restored ClearAffect descriptor and removal field capture order (0813d8ea, 0813d9df-0813d9fa). Static ELF comparison only.

Source batch 1172 restored separate SaveAffect type reads for filtering, logging and serialization (0813ddca, 0813de17, 0813de42), and the original load_affect_login_event_info record name (DWARF 0x21182df). Static comparison only.

Batch 1172 restored the original FindLandByGuild iterator advancement and CLand::GetData lookup (081091c6, 081091d7, 081091df).

Source batch 1173 restored the original ClearLand GetData call after resetting ownership and its use for the land NPC map and coordinates (081099dc-08109a66). The existing missing-region guard remains. Static ELF comparison only.

Batch 1173 also restored the mirror item cooldown's flag-before-clock order and signed 32-bit elapsed time, with an independent clock read for the success timestamp (0817a368-0817a3c6).

Source batch 1174 restored the original refine recipe diagnostic field reads in cost, probability and recipe ID order (0806a300-0806a32c). Static ELF comparison only.

Batch 1174 also restored the refine cooldown flag reread after the clock and signed 32-bit strict comparison in DoRefine and DoRefineWithScroll (08171acb, 08173502). FindLand now advances its iterator before inspecting the saved land entry (08109106-08109117).

Source batch 1175 restored horse skill book signed 32-bit cooldown and wait message resampling (08176f8c-08176fa9, 0817728e-081772a3), the original short-circuit skill cap reads (08176fbe-08177078), and the original mining probability MINMAX calls (080ef53a, 080ef565). Static ELF comparison only.

Source batch 1176 restored SendLandList iterator advancement before reading the saved land entry and independent size reads for logging, header and payload (081097c3-0810987c). Static ELF comparison only.

Batch 1176 also restored both stone detectors' 32-bit attempt increment and socket rereads for distance, retry limit and heading (0817583c-081758dd, 08176220-081762ad).

Source batch 1177 restored the original marriage_engage_to name, player ID and manager read order before RequestAdd (080f8d3a-080f8d57). Static ELF comparison only.

Batch 1177 also restored the original 32-bit arena observer keys (080bd522) and the full-refill items' stat read order and 32-bit subtraction (081768a4, 0817685a, 0817680b).

Source batch 1178 restored the 32-bit arena EndDuel PID comparison (080bd36a-080bd38a) and alignment item locale and stat read order with wrapped negation (081780ab-081780f7). Static ELF comparison only.

Source batch 1179 restored 32-bit player ID comparisons in CArenaMap::IsMember (080bcf8c, 080bcfcb-080bcfd2), preserving observer lookup priority. Static ELF comparison only.

Batch 1179 also restored the hair dye cooldown threshold's 32-bit arithmetic and live item ID check (08178283-081782c4). The consumed item is not accessed after its count changes.

Source batch 1180 restored 32-bit arena duelist storage and PID handling in CanAttack, OnDisconnect and both OnDead paths (080bb820, 080bc300, 080bc3c0, 080bc440, 080bd040). Static ELF comparison only.

Source batch 1181 restored quest affect apply ID 80 as the maximum (08069331, 08069721, 080699c1) and rare attribute add audit ordering after the success notice (0817855b-081785e9), retaining the pre-consumption source ID for lifetime safety. Static ELF comparison only.

Source batch 1182 restored 32-bit arena map IDs in registry keys, stored map records and the same-map combat check (080ba3bc, 080ba484, 080bc1f0, 080bcee9, 080bcf32). Static ELF comparison only.

Batch 1182 also restored quest item drops applying the X offset before rolling Y (080b2ed3-080b2f07) and the Moonlight box audit field read order (08175e5b-08175e70).

Source batch 1183 restored UseItemEx live prototype reads after the locale check and test-server diagnostic (081746dd-0817477e). Static ELF comparison only.

Source batch 1184 restored separate guild power comparisons for the two reserve-war Lua fields (080d191f, 080d195e) and original arena list coordinate reads (080ba816-080ba858). Static ELF comparison only.

Source batch 1185 restored the original building quest land record accessor (0808549a) and seed/moon bottle cooldown pulse and rate sampling with 32-bit arithmetic (0817d4f0-0817d509, 0817d556-0817d578). Static ELF comparison only.

Source batch 1186 restored reserved guild war diagnostic reads in handicap, destination guild, source guild and reservation ID order (080d18bd-080d18d7). Static ELF comparison only.

Batch 1186 also restored the target name and PID reads before target manager resolution for the position and VID quest bindings (08189b9b-08189ba8, 08189e21-08189e2e).

Source batch 1187 restored the ready-event presence snapshot before duelist ID reset in CArena::Clear (080ba32c-080ba33e). Static ELF comparison only.

Batch 1187 also restored target delete, clear and ID lookup argument reads before target manager resolution (08189fed-0818a012, 0818a0f0-0818a110, 0818a261-0818a286).

Source batch 1188 restored 32-bit target manager player keys and quest event identifiers, including lookup and deletion comparisons (08105800, 08105ce0, 08105de0, 08105eb6). Static ELF comparison only.

Source batch 1189 restored nonstandard skill book zero-value routing through LearnSkillByBook (0817c7e1-0817c7ff), retaining the zero-socket handling for standard and forget books. Static ELF comparison only.

Batch 1189 also restored 32-bit storage for the building object VID (08106c19), keeping the public getter and setter signatures.

Source batch 1190 restored the owner-first 32-bit map comparison for VID quest targets (08105787-0810578d). Static ELF comparison only.

Batch 1190 also restored mining skill book requirement sampling (08177985-08177acc) and matched building VID diagnostic arguments to their unsigned-long format after narrowing storage.

Source batch 1191 restored the character-coordinate read before the map base in pc_get_local_x and pc_get_local_y (0815e8d8-0815e8db, 0815e9b8-0815e9bb). Static ELF comparison only.

Batch 1191 also restored the original low 32-bit building rotation conversion, including zero for invalid signed 64-bit conversions (08106b5b, 08106b6d).

Source batch 1192 restored separate combo book skill reads, language book chance sampling before eligibility, and the map scroll Lua name read before manager lookup (0817778e, 081777b6, 081777d1, 081775ef-08177619, 08161dee). Static ELF comparison only.

Batch 1192 also restored chance sampling before the skill cap for value, fixed and test skill books (08176c09, 08177ca1, 08177e36).

Source batch 1193 restored localization before the reward count read for quest special-item group gold and experience notices (0815dc29-0815dc4c). Static ELF comparison only.

Batch 1193 also restored Lua slot-before-value conversion in pc_set_part (081614e7-08161544) and iterator advancement before object unregistration in CLand::Destroy (08107648-08107666).

Source batch 1194 restored 32-bit shop price-list command values (081740ec-08174115) and explicit X-before-Y land master coordinate reads in CLand::PutData (0810778b-08107799). Static ELF comparison only.

Source batch 1195 restored the distinct land bounds comparison orders for PutData (0810779b-081077bb) and FindLand (08109140-08109160), retaining inclusive boundaries and wrapped coordinate arithmetic. Static ELF comparison only.

Batch 1195 also restored 32-bit unique equipment comparisons (08180912, 0818093b, 0818099f, 081809c5) and quest percentage XP label sampling around reward logging (081610e6-08161249).

Source batch 1196 restored Lua amount-before-label sampling and fresh amount and label reads around reward logging in pc_give_exp (08160e37-08160f8f). Static ELF comparison only.

Batch 1196 also restored signed 32-bit event time sampling before division in StopUniqueExpireEvent (081686d1-081686dc).

Source batch 1197 restored signed 32-bit unique expiry socket reads in scheduling and both expiry modes (0816847d, 08168545, 081685ce). Static ELF comparison only.

Batch 1197 also restored the quest XP amount reread in pc_give_exp2 (08160c45-08160c4d) and the original land pointer reuse in building reconstruction (08106a05-08106a5b).

Source batch 1198 restored the ground item expiry diagnostic name read order (08166491-081664d2). Static ELF comparison only.

Batch 1198 also restored FindObject and its null-result check in CLand::DeleteObject (08107aec), plus character-before-item label reads in quest reward diagnostics (0815df0f, 0815e399).

Source batch 1199 restored 32-bit VID diagnostics in RequestDeleteObjectByVID (08108141, 0810816d) and flag-before-price order in CItem::GetGold (08167ed6-08167f03). Static ELF comparison only.

Batch 1199 also restored quest reward diagnostic reads in level, player ID, quest name and table postfix order (08160ee8-08160f12).

Source batch 1200 restored the original gold-bar quest reward audit read order (0815e494-0815e4c4). Static ELF comparison only.

Batch 1200 also restored the original 12-byte SetSockets copy hook before Save (08167d92), using a packed-width temporary when host long is wider.

Source batch 1201 restored 32-bit CreateSocket occupancy (08167cc2), numeric Lua inventory argument order (0815dcd0, 0815ea91), and unsigned 32-bit land dimension diagnostics (08108e70-08108ea2). Static ELF comparison only.

Source batch 1202 restored 32-bit socket occupancy reads in GetSocketCount and its AddSocket caller (08168118, 0816813f). Static ELF comparison only.

Batch 1202 also restored accessory expiry 32-bit socket gates (08168ab8-08168abf) and quest item removal count and diagnostic read ordering (0815ed04-0815ed49, 0815effb-0815f009).

Source batch 1203 restored the original count-before-VNUM material diagnostic reads in LoadObjectProto (08108c39-08108c41). Static ELF comparison only.

Batch 1203 also restored the alignment Lua amount read before character resolution (0815fc98-0815fcb4) and separate X/Y trigonometric evaluation in building NPC placement (08106ff3-08107055).

Source batch 1204 restored the original armor removal appearance read before resolving the SetPart owner (081675fd, 081675dc, 08167541). Static ELF comparison only.

Batch 1204 also restored the zero-amount short circuit in pc_change_sp (0815f82c-0815f85b) and 32-bit VNUM lookup in FindSpecifyItem (0817f87b, 0817f8cc).

Source batch 1205 restored the original 32-bit VNUM comparison and count accumulator in CountSpecifyItem (0817f94d, 0817f963). Static ELF comparison only.

Batch 1205 also restored the Lua gold amount read before current character resolution (0815fd28-0815fd3c) and shared Y/X position snapshots for building occupancy removal (08106887-081068d1).

Source batch 1206 restored item Y, item X, character Y, character X read order in DistanceValid (08166d73-08166d90). Static ELF comparison only.

Batch 1206 restored signed 32-bit building placement inputs (08106c80-08106ca4) and original Lua key reads in quest flag writes and deletion (08160711, 081608a1, 08160a8c).

Source batch 1207 restored Y-before-X coordinate reads for ground item sectree lookup and failure logging (08166c89-08166c90, 08166ce6-08166cf2). Static ELF comparison only.

Batch 1207 also restored cross-quest flag argument order (081601a9-081601cc, 0815ff85-0815ff9d) and item-use packet field order and VNUM reread (08166772-08166799).

Source batch 1208 restored Lua key reads before quest PC lookup (081603ac, 081604f1) and Y-before-X landmaster placement during FinalizeBoot (0810956d-081095a1). Static ELF comparison only.

Source batch 1209 restored recipient-before-item name reads in both ground item encoder diagnostics (0816663b-08166640, 081666be-081666c3). Static ELF comparison only.

Batch 1209 also restored the wall deletion ID snapshot and independent test-server rule (0810a2e5-0810a30b, 0810a348).

Source batch 1210 restored the single attribute weight read per roll step (08187510-08187518) and player ID reads before marriage manager lookup (08162e65, 08162f25, 08162fe5). Static ELF comparison only.

Batch 1210 also restored one-read attribute candidate weights and level cap (081872c9, 08187428), and Y-before-X wall placement reads (08109bf5-08109bfe).

Source batch 1211 restored separate lottery reward player ID reads and table postfix resolution before the DB manager lookup (0816297f-081629c9). Static ELF comparison only.

Batch 1211 restored a single rare attribute cap read (08187c80-08187c89).

Source batch 1212 restored the quest point reset character reread after level capture (0815f2a3-0815f2bb). Static ELF comparison only.

Batch 1212 also restored the item update attribute copy hook (08166834) and Lua argument reads before quest timer context lookup (08056411, 08057052).

Source batch 1213 restored iterator advancement before land deletion in building manager cleanup (0810ab20-0810ab7e). Static ELF comparison only.

Batch 1213 also restored the original item save socket and attribute copy hooks (08192a28-08192a64) and 32-bit building creation inputs (08107c5b, 08107fbb).

Source batch 1214 restored named quest timer argument and context order (08056c43-08056cf3, 08056e53-08056f00). Cancel a newly created event if the quest PC is unavailable. Static ELF comparison only.

Batch 1214 restored the socket and attribute copy hooks for both populated inventory item packet formats (08171165, 0817117c, 081711f9, 08171215).

Source batch 1215 restored the quest manager lookup before Lua timer arguments in server timer cancellation (08056a89-08056b02). Static ELF comparison only.

Batch 1215 restored live same-quest state reads (08057d29-08057dab), the guild snapshot for building reconstruction (080857ff), and the refine materials copy hook (0817376a).

Source batch 1216 restored Lua keys before item and mob metadata lookups (08053fc3, 08054093, 08054160) and the safebox socket copy hook (08096f89). Static ELF comparison only.

Source batch 1217 restored shop socket copy hooks before attribute copies in AddGuest and BroadcastUpdateItem (080d47b7, 080d4ad6). Static ELF comparison only.

Batch 1217 restored Lua player name and VID reads before character manager lookup (0805521c, 08055580).

Source batch 1218 restored the exchange socket copy hook (080643ad) and guild name conversion before quest guild lookup (08054fc0-08054feb). Static ELF comparison only.

Source batch 1219 restored the Lua VNUM read before character manager lookup for quest NPC search (080556a8-080556dc). Static ELF comparison only.

Source batch 1220 restored the packed socket copy source in ItemLoad (080eae91-080eaea5) and Lua filter reads before the conditional player search resolves its character (0805535f-080553e1). Static ELF comparison only.

Source batch 1221 restored map regeneration base Y-before-X reads (08059c0d-08059c25). Static ELF comparison only.

Source batch 1222 restored single item type reads for magic attribute thresholds (08168215, 0816831b) and the X/Y/PID read order in fishing rod refinement result logging (08084426-0808442c). Static ELF comparison only.

Source batch 1223 restored single addon and socket configuration reads in CreateItem (08192486, 081924df), unsigned 32-bit Lua start values and literal zero for invalid empire coordinates (0805a340, 0805a3b0, 0805a430). Static ELF comparison only.

Source batch 1224 restored the signed 32-bit Lua clock result (08053e46-08053e58). Static ELF comparison only.

Batch 1224 restored signed 32-bit Lua playtime conversion (0815f367) and owner-before-name reads in the duplicate item diagnostic (081927b5-081927d2).

Source batch 1225 restored signed 32-bit Lua global coordinates before division and signed 32-bit current map indices (0815e752, 0815e7bc-0815e7c6, 0815e83c-0815e846). Static ELF comparison only.

Batch 1225 restored prototype row and VNUM rereads after quest registration in ITEM_MANAGER::Initialize (08191e21-08191e70).

Source batch 1226 restored the current character snapshot before Lua argument validation in pc_get_part (081613c8-081613d7). Missing character and invalid argument return zero. Static ELF comparison only.

Source batch 1227 restored the signed 32-bit map result and per-result empire reads in pc_get_start_location (0815cd01-0815cd46). Static ELF comparison only.

Source batch 1228 restored target VNUM capture before accessory subtype selection (08168d16) and signed 32-bit war map Lua output (08161f59). Static ELF comparison only.

Source batch 1229 restored the current quest PC read between key and value in flag setters, after the key in deletion (08160711-0816073b, 081608a1-081608cb, 08160a8c-08160aac), and the single item name read in CreateSocket diagnostics (08167cca-08167cfc). Static ELF comparison only.

Source batch 1230 restored original character snapshots in the Lua part and skill-group setters (081614a4, 08161699-081616b0), retaining argument checks and avoiding a missing-character dereference. Static ELF comparison only.

Source batch 1231 restored completion of signed alignment conversion before current character lookup (0815fcbc-0815fcc9). Static ELF comparison only.

Source batch 1232 restored the quest PC snapshot before percentage XP logging (081611c2-08161275) and NPC shop flag-before-price reads (080d3675-080d3695). Static ELF comparison only.

Source batch 1233 restored the quest player snapshot before fixed XP reward logging (08160edf-08160f8f). Static ELF comparison only.

Source batch 1234 restored early character snapshots in quest item rewards and reused the captured quest PC for item delivery and logging (0815ddcc-0815dfb9, 0815e279). Static ELF comparison only.

Source batch 1235 restored the character snapshot before Lua slot conversion in pc_get_equip_refine_level (08162249-08162258), with a zero result for missing character context. Static ELF comparison only.

Source batch 1236 restored early character snapshots for quest position scroll and equipment refinement bindings (08161fc9, 08162379), with missing-context guards. Static ELF comparison only.

Source batch 1237 restored the character snapshot before Lua validation in pc_enough_inventory (0815dca9-0815dd17), with a false result for missing character context. Static ELF comparison only.

Source batch 1238 restored the original 32-bit mount VNUM backing field (0813601f-08136027), preserving the public signatures. Static ELF comparison only.

Batch 1238 also restored character capture before Lua empire conversion (081642e8-08164322).

Source batch 1239 restored 32-bit polymorph race storage (08135b34-08135b44) and early character capture in the premium-time Lua binding (081640bf-081641de). Static ELF comparison only.

Source batch 1240 restored live shop slot reads after socket and attribute copies (080d4adc-080d4b02), with bounds and missing-item guards. Static ELF comparison only.

Batch 1240 also restored character capture before grand-master skill argument validation (0815c63c-0815c6ed).

Source batch 1241 restored the original party quest flag name construction order (08146485-081464ef). Static ELF comparison only.

Source batch 1242 restored the Lua cinematic payload read after party update and before script construction (081457f0-08145801). Static ELF comparison only.

Source batch 1243 restored signed 32-bit Lua item value conversion (080f0b95-080f0b9b). Static ELF comparison only.

Batch 1243 restored the owner and diagnostic field snapshots in SaveSingleItem (0819298c-081929a6).

Source batch 1244 restored Lua mask conversion before the item flag read (080f09d2-080f09ef). Static ELF comparison only.

Source batch 1245 restored Y-before-X coordinate reads for the initial horse wander target (08081a6a, 08081a7e). Static ELF comparison only.

Batch 1245 also restored Lua guild ID reads before manager resolution in guild_name, guild_get_name and guild_level (080d103c, 080d1383, 080d10fb).

Source batch 1246 restored the skip-save flag read before the item name in the destruction diagnostic (08192e90-08192ec4). Static ELF comparison only.

Batch 1246 also restored descriptor and login capture before the Lua reservation ID in guild_is_bet (080d16f5-080d1719).

Source batch 1247 restored the original floating-point invalid flag on out-of-range OX quest numeric conversions (0813c9d0, 0813ccce). Static ELF comparison only.

Source batch 1248 restored map-before-player-ID reads and the second manager lookup in arena_start_duel (08078d9a-08078de3). Static ELF comparison only. Ghidra-dependent work is deferred until Ghidra is available.

Source batch 1249 restored arena_is_in_arena lookup order (08079393-080793b0) and the arena_start_duel manager reload before StartDuel (08078e21). Static ELF comparison only.

Source batch 1250 restored the arena conversion invalid flag (08078d72, 08079344) and signed 32-bit Santa gift counters (08120ac0, 08120c30). Static ELF comparison only.

Source batch 1251 restored NPC proximity coordinate read order (08120f64-08120f76, 081210be-081210c9) and the horse quest signed 32-bit conversion before clamping (080e39f3-080e39fa). Static ELF comparison only.

Source batch 1252 restored marriage quest identifier conversion (080f8d04, 080f9374, 080f9397) and the player snapshot before NPC lookup in kill and purge (08120de5, 08120e75). Static ELF comparison only.

Source batch 1253 restored upper-before-lower reads in GetFishLength (08083356-08083362) and GetFractionCount (080ef2cb-080ef2d9), plus the Grill prototype snapshot (08084163-0808419d). Static ELF comparison only.

Batch 1253 also restored the player and partner ID reads before manager access in marriage_remove (080f8e5b-080f8e89).

Source batch 1254 restored the missing marriage diagnostic read order (080f8ecd-080f8eda, 080f9006-080f9013 and equivalent wedding paths) and the clock read before the marriage timestamp (080fa460-080fa465). Static ELF comparison only.

Batch 1254 also restored the fishing timing RNG draw before the time-type read (0808324e-08083260), retaining the bounds check before table access.

Source batch 1255 restored the floating-point invalid flag on invalid rare fish length conversion (080834c4). Static ELF comparison only.

Batch 1255 also restored partner ID resolution before character manager access in marriage_find_married_vid (080f911e-080f914c).

Source batch 1256 restored the invalid floating-point flag on target quest numeric conversions (08189ad9, 08189b1d, 08189b91, 08189c65, 08189e17, 08189eea). Static ELF comparison only.

Batch 1256 also restored post-lookup Lua conversions for wedding dark, client command, music and snow (080f9932, 080f9b62, 080f9fbd, 080fa202), retaining null-map checks.

Source batch 1257 restored the mining weight read (080ef2a0-080ef2ab) and fishing practice socket reads before prototype limits (08082d60-08082d6f, 08082e16-08082e25). Static ELF comparison only.

Batch 1257 also restored the Lua reads before the matching map origin coordinate reads in target_pos (08189abc-08189ac4, 08189af7-08189aff).

Source batch 1258 restored the floating-point invalid flag for invalid NPC proximity VID conversions (0812106e). Static ELF comparison only.

Batch 1258 also restored the event step check before final rod practice in fishing::Take (08083579-08083596).

Source batch 1259 restored the fishing step guard for unexpected Compute results (08083651, 08083662) and the invalid floating-point flag in item quest numeric conversion (080f0704, 080f033a, 080f046f). Static ELF comparison only.

Source batch 1260 restored the Lua ID read before item manager access (080f0453-080f0467) and the item VNUM read before the base VNUM in UseFish (08083f37-08083f46). Static ELF comparison only.

Batch 1260 also restored affect type resolution before character lookup in affect_remove (080694de, 08069509-0806950e).

Source batch 1261 restored the invalid floating-point flag in affect quest slot and signed numeric conversion (080692d6, 0806931e, 080693b2, 080693e4, 080694d1). Static ELF comparison only.

Batch 1261 also restored the character snapshot before affect diagnostics (080692dc, 080696d1, 08069971, 08069bac), the selected item reread before socket retrieval (080f0715), and the 32-bit fishing step increment (08082f87-08082f90).

Source batch 1262 restored the mining event character manager lookup between player and vein resolution (080efde6-080efe05). Static ELF comparison only.

Batch 1262 also restored the inclusive apply ID 81 limit in the quest affect bindings (08069331, 08069721, 080699c1).

Source batch 1263 restored the map index write on replacement quest targets (0810599b, 08105a8d) and the invalid flag for three-way war Lua numeric conversions (080a5c12, 080a5db5, 080a5e3f, 080a5e9f). Static ELF comparison only.

Batch 1263 also restored the fishing table backup and rollback copy hook (08082217, 08082791, 080827db) with the staged parser retained.

Source batch 1264 restored 32-bit target map comparisons (0810551e, 08105875) and the fishing tab diagnostic newline (08082759). Corrected batch 1263: 0810599b writes arg2 at +0x5c; only creation writes map at +0x60 (08105a8d). Static ELF comparison only.

Source batch 1265 restored the three-way war diagnostic map and path rereads (080a5b44, 080a5c54, 080a5c9a). Static ELF comparison only.

Batch 1265 also restored the 32-bit quest target ID increment on creation and replacement (08105974-0810597f, 08105a3b-08105a49).

Source batch 1266 restored the fishing simulation species diagnostic read order (08083cb0-08083cc3). Static ELF comparison only.

Batch 1266 also restored the replacement name and ID reads before the character fields in the final rod refinement audit (0808440f-0808442c).

Source batch 1267 restored quest target delete packet field reads (081059c1-081059ce, 08105db8-08105dbd) and separate owner ID reads before die callback and deletion (08105656, 08105696). Static ELF comparison only.

Batch 1267 also restored the current PC snapshot before the Lua thread comparison in quest_setstate (081696e3-081696e9).

Source batch 1268 restored the owner ID read before the quest index in the target arrival callback (08105581-081055b9). Static ELF comparison only.

Batch 1268 also restored Lua rounding before the current PC lookup for the quest clock and counter setters (08169354-08169359, 081694f4-081694f9).

Source batch 1269 restored Lua string construction before the current PC lookup for quest title, clock name, counter name and icon setters (0816905b, 0816928b, 0816942b, 081695cb). Static ELF comparison only.

Source batch 1270 restored clock and counter conversion after the current PC lookup (08169359-08169374, 081694f9-08169514) and the invalid flag in quest and game numeric conversions (080b2977, 080b2c4d, 080b2caa). Static ELF comparison only.

Source batch 1271 restored the quest manager capture before Lua validation in game event flag bindings (080b28f9-080b291c, 080b2a19-080b2a3c). Static ELF comparison only.

Source batch 1272 restored immediate Lua argument conversion before position capture for game_drop_item (080b2e18-080b2e61), and the invalid flag for party flag conversions (08146293, 08146564). Static ELF comparison only.

Batch 1272 also restored unsigned 32-bit Lua conversion of the nearby party member count (08145a76-08145a80).

Source batch 1273 restored cinematic party rereads (081459df, 0814588d), the reroll addon snapshot (08187572), and signed 32-bit quest drop map index (080b2f0a-080b2f1b). Static ELF comparison only.

Source batch 1274 restored the 32-bit party leader comparison and leader-first ID read (08145eb8-08145ebd). Static ELF comparison only.

Source batch 1275 restored the invalid floating-point flag for building quest conversions. ELF signed 32-bit stores: 080851e9, 0808520d, 08085232. Signed 64-bit stores: 08085302, 08085478, 080854cc, 080855a6, 080857e3. Static ELF comparison only.

Source batch 1276 made original Lua land ID, price and owner result widths explicit (0808525e-08085264, 08085327-08085359). Static ELF comparison only.

Source batch 1277 restored the Y-before-X candidate position reads for party loot ownership (081438a7-081438be) and the guild ID read before the building manager in building_reconstruct (08085809-0808581b). Static ELF comparison only.

Source batch 1278 restored the polymorph utility reload before book practice update (08181218, 08181237-0818124e). Static ELF comparison only.

Batch 1278 also restored altar NPC guild master ID and level read order (08107141-08107154, 08107165-0810717c).

Source batch 1279 restored long-time-before-base reads in GetExpBonusPercent (08144633-08144639). Static ELF comparison only.

Batch 1279 also restored the post-lookup altar VNUM check (08106e39-08106e8b) and 32-bit PID lookup in SetExpCentralizeCharacter (08144860).

Source batch 1280 restored 32-bit PID keys in party role reads and updates and the leader proximity query (08143330, 0814337c, 08144660). Static ELF comparison only.

Batch 1280 also restored the sector manager reload and occupancy region argument preparation in CObject::Show (08106cbb-08106d2c).

Source batch 1281 restored the party member and leader coordinate read order in IsPositionNearLeader (081445f0-08144607). Static ELF comparison only.

Batch 1281 also restored occupancy region field read order in CObject::Destroy (0810687c-081068e6).

Source batch 1282 restored the NPC snapshot before character manager access in CObject::Destroy (0810692d-08106945). Static ELF comparison only.

Batch 1282 also restored building boot position reads (0810963b-08109653) and the 32-bit IsRole lookup (0814337c-08143387). The address 0814337c in batch 1280 identifies IsRole; SetRole starts at 08143200.

Source batch 1283 restored 32-bit PID lookup keys for P2PSetMemberLevel (08142532-08142543) and SendPartyInfoOneToAll (08142b23-08142b40). Static ELF comparison only.

Batch 1283 also restored recall item diagnostic and warp coordinate reads (08173f45-08173f57, 08173f8d-08173f90).

Source batch 1284 restored the recall map lookup coordinate width and read order (08173ec2-08173ee3). The existing singleton accessor assertion remains. Static ELF comparison only.

Batch 1284 also restored the 32-bit player ID key in SummonToLeader (0814350e).

Source batch 1285 restored building coordinate conversion invalid flags (08106ce9, 081068e6, 08107038, 08107078, 081070bd, 08107d29-08107f22, 08109b68) and 32-bit party member keys for join, quit, membership and bonus removal (08141a70, 08141ee0, 08143430). Static ELF comparison only.

Source batch 1286 restored 32-bit party manager index keys at the container type, covering both insertion and lookup (SetPartyMember 08141110, P2PLogin 08140c20, P2PJoinParty 08140ce0). Static ELF comparison only.

Source batch 1287 restored the wall angle snapshot (08109bd0-08109be0) and stone detachment audit field reads (08173a70-08173a9c). Static ELF comparison only.

Source batch 1288 restored the two signed 32-bit socket scans in stone detachment (08173ada-08173b61) and 32-bit party member and leader storage (08103490, 08144f20, 08141a70). Static ELF comparison only.

Source batch 1289 restored the refinement information prototype snapshot (08173644-08173676) and ClearLand character manager capture before coordinate reads (08109a08-08109a79). Static ELF comparison only.

Source batch 1290 restored refinement eligibility and recipe prototype snapshots (08173682-081736ba, 081738b9-0817390f). Static ELF comparison only.

Source batch 1291 restored the land-load diagnostic field reads (08108e66-08108ea2). Static ELF comparison only.

Batch 1291 also restored object prototype diagnostic reads and the material VNUM snapshot before lookup (08108bcd-08108c18).

Source batch 1292 restored LoadObject diagnostic and runtime position field reads (0810924d-0810927f, 081093a0-081093b8). Static ELF comparison only.

Source batch 1293 restored ID-before-VNUM reads on the missing building prototype diagnostic (081093f1-08109417). Static ELF comparison only.

Source batch 1294 restored the 32-bit map comparison in UpdateLand recipient selection (08108f8c-08108f92). Static ELF comparison only.

Source batch 1295 restored invalid floating-point flags in dungeon quest integer conversion helpers (08093407, 08093b2d, 080967ff). Static ELF comparison only.

Batch 1295 also restored the guarded Metin stone wear flag read in the target type branches (0817bda2-0817bdb4).

Source batch 1296 restored the Metin stone target-type reread after the duplicate scan (0817bb55-0817bb77). Static ELF comparison only.

Source batch 1297 restored signed 32-bit socket eligibility comparison in UseMetinStone (0817bb7f-0817bb8a, 0817bbdc-0817bbe3). Static ELF comparison only.

Batch 1297 also restored the original 32-bit dungeon manager map key (Find at 081829d0-081829f6) consistently across insertion, lookup and removal.

Source batch 1298 restored Metin socket audit field reads and item manager reload before removal (0817bc66-0817bce2). Static ELF comparison only.

Batch 1298 also restored 32-bit storage for both dungeon map fields (constructor stores 08181905 and 08181911; unsigned declarations in the reference tree).

Source batch 1299 restored 32-bit dungeon warp destination storage (08185856, 0818585c, 08185862; signed fields in the reference) and the guarded occupied-slot reread for CountEmptyInventory (0817170c, 08171737). Static ELF comparison only.

Source batch 1300 restored Lua type validation before quest manager access in the dungeon flag setter and getter (0809339c-080933a5, 080934f8). Static ELF comparison only.

Source batch 1301 restored advance-before-check traversal in CDungeon::DeadCharacter (08183ff8-0818400b). Static ELF comparison only.

Source batch 1302 restored 32-bit warp callback fields and map comparison in FWarpToPosition (081869eb-08186a41). Static ELF comparison only.

Source batch 1303 restored party pickup notice read order for both recipients (0817f15b-0817f1fc). Static ELF comparison only.

Batch 1303 also restored direct pickup name reads before localization (0817edfc, 0817ef38, 0817f206) and the leader-first 32-bit PID comparison in dungeon_join (08096474-08096479).

Source batch 1304 restored the full-inventory pickup diagnostic field reads (0817ee79-0817eeae). Static ELF comparison only.

Source batch 1305 restored ground item audit prototype and field reads for direct pickup (0817ed68-0817edf7), party pickup (0817f0b9-0817f14a), and drop (0817e0fb-0817e184). Static ELF comparison only.

Source batch 1306 restored find/update versus direct insertion in CDungeon::SetFlag (08182089, 081820a1, 08182106). Static ELF comparison only.

Batch 1306 also restored the dungeon_jump_all map read between Lua X read and conversion (08094499-080944be), retaining coordinate guards.

Source batch 1307 restored split drop audit field order and the 51-byte hint limit (0817dfda-0817e068), retaining placement and rollback behavior. Static ELF comparison only.

Source batch 1308 restored inventory stack split audit reads, 32-bit total and 51-byte hint limit (0817e7fb-0817e844). Static ELF comparison only.

Source batch 1309 restored item-name-before-character-name reads in MoveItem diagnostics (0817e642-0817e651, 0817e72e-0817e73d). Static ELF comparison only.

Source batch 1310 restored stack merge diagnostic count width and name order (0817e911-0817e973) and the quest_set_another_title PC lookup after Lua string construction (08169138-08169193). Static ELF comparison only.

Source batch 1311 restored the Lua state name read before quest manager access in quest_setstate (0816965e, 08169667, 081696d2). Static ELF comparison only.

Batch 1311 also restored party flag insertion and unchanged-value handling (08144459-081444dc) and single descriptor reads for ground item add and remove (081665f2-08166633, 08166692-081666b6).

Source batch 1312 restored signed 32-bit OX entry coordinate comparisons and diagnostics (080876c2-080876f6). Static ELF comparison only.

Source batch 1313 restored 32-bit OX participant index keys and values (08087782, 080877ba, 080877ee, 08087810). Static ELF comparison only.

Source batch 1314 restored signed 32-bit coordinate comparisons in OX answer scoring (0808810a-08088131). Static ELF comparison only.

Source batch 1315 restored signed 32-bit OX return coordinates in COXEventManager::CloseEvent (0808870a, 08088720, 08088738). Static ELF comparison only. Ghidra-dependent work is deferred until Ghidra is available.

Source batch 1316 restored the shared unsigned 32-bit reward VNUM in COXEventManager::GiveItemToAttender (08088913-08088917, 0808892b-08088952). Static ELF comparison only.

Source batch 1317 restored the live unsigned 32-bit player ID read after the effect and attendee removal in COXEventManager::CheckAnswer (08088151-080881b1, 08088210). Static ELF comparison only.

Source batch 1318 restored disconnected OX attendee cleanup by key in both auxiliary indexes (0808839f-080884fa) and per-marker Y/X coordinate reads in CLand::RequestCreateWall (08109d62-08109e6d). Static ELF comparison only.

Source batch 1319 restored signed 32-bit VNUM and experience arguments in PC::GiveItem and PC::GiveExp reward diagnostics (08060ea6, 0806100f). Static ELF comparison only.

Source batch 1320 restored the quest experience diagnostic reads and initial cap inputs in PC::Reward (080611d8-08061245). Static ELF comparison only.

Source batch 1321 restored branch-specific next EXP table and queued reward reads in PC::Reward (08061245-080612ac). Static ELF comparison only.

Source batch 1322 restored queued item reward count and VNUM reads (080611a2-080611c9) and the fishing::Take entry step snapshot and time read order (080834ef-080834f5, 08083610-08083649). Static ELF comparison only.

Source batch 1323 restored canonical vertical gate rotation in CLand::RequestCreateWall, including positive zero for a negative-zero input (08109cb5, 0810a1c8-0810a214). Static ELF comparison only.

Source batch 1324 restored explicit object map traversal in RequestDeleteWall and RequestDeleteWallBlocks (0810a319-0810a329, 0810a684-0810a694). Static ELF comparison only.

Source batch 1325 restored quest timer conversion before allocation and the signed conversion invalid flag (081adced-081adcfa, 081addd0-081adddd). Static ELF comparison only.

Source batch 1326 restored eager rotated bounds conversions in CLand::RequestCreateObject (08107d29, 08107d86, 08107de7, 08107e56). Static ELF comparison only.

Source batch 1327 restored the dispatch manager and NPC-then-player reads in quest_timer_event (081adb17-081adb35). Static ELF comparison only.

Source batch 1328 restored server quest timer dispatch and cleanup argument order (081ad9f3-081ada04, 081ada40-081ada60). Static ELF comparison only.

Source batch 1329 restored castle village destination snapshots (081186b7-081186ea, 0811877d) and sequential Lua goto argument conversions (080581e0-080582a4). Static ELF comparison only.

Source batch 1330 restored the original 0x3ca3d70b default volume in quest::_add_bgm_info (080581a3-080581b3). Static ELF comparison only.

Source batch 1331 restored the signed 64-bit invalid flag in ReadLuaDword, verified across item and mob names, player and NPC lookups, and server timer arguments (08053fdf, 080540af, 080553cd, 0805559c, 080556c4, 0805673d, 08056adf). Static ELF comparison only.

Source batch 1332 restored 32-bit siege pulse update and threshold (08118841-08118862) and quest NPC map comparison width and read order (08055710-08055718). Static ELF comparison only. Retained stone timer cancellation after tracing the stale callback across a siege restart.

Source batch 1333 restored signed 32-bit castle siege callback return delays after notices or the loser warp (08118961-0811896c, 081189cf-081189da). Static ELF comparison only.

Source batch 1334 restored the normal castle frog spawn order and second free-slot lookup (0811946f-08119551). If that lookup finds no slot, discard the untracked frog. Static ELF comparison only.

Source batch 1335 restored FindSpecifyPC level, map and job reads (0818e978-0818e9ac, 0818e9f2-0818e9fd) and forced castle frog coordinate computation (08119628-0811968e). Static ELF comparison only.

Source batch 1336 restored ordered 32-bit guard spawn bounds and group VNUM (0811979c-08119830, 08119877-0811987d). Follow-up: verify group ownership and cleanup before moving guard slot selection after spawning (08119789, 08119856). Static ELF comparison only.

Source batch 1337 restored the separate map reads for diagnostics and selection in _find_pc_cond (08055462-08055486, 080553ed). Static ELF comparison only.

Source batch 1338 restored the unsigned 32-bit VNUM range checks for castle guards and towers (08119fb3-08119fc2, 0811a213-0811a222). Static ELF comparison only.

Source batch 1339 restored castle_cost_of_hiring_guard and its declaration from the original switch (08119fd0-0811a007, 0822313c-082231a3). Static ELF comparison only.

Source batch 1340 restored separate tower slot cleanup and surviving-tower scan (08119e40-08119e7f, 08119f14-08119f1e). Static ELF comparison only.

Source batch 1341 restored castle_is_my_castle (0811a1c0-0811a20f), mapping empires 1/2/3 to maps 181/183/182 and rejecting other empires. Static ELF comparison only.

Source batch 1342 restored the fishing table comparison read order (080821c6-080821d0). Static ELF comparison only.

Static coverage after batch 1342: all 372 signed 32-bit cells of c_aHorseStat match the ELF .data symbol (0823ec40, size 0x5d0). Arena observer send methods are genuine no-ops (080bd2d0, 080bd2e0). The emitted arena, marriage and wedding function names checked have source counterparts. No source edit was warranted for these checks.

Source batch 1343 restored the empire read order in both castle_can_attack comparison branches (0811a079-0811a086, 0811a0a5-0811a0b2). Static ELF comparison only.

Source batch 1344 restored signed 32-bit arena return coordinates (080bbc17, 080bbc30, 080bbc4b, and corresponding repeat paths). The initial start map, start position, arena return and create position arrays match ELF .data (08240140, 08240160, 08240180, 082401a0). Static ELF comparison only.

Source batch 1345 restored signed 32-bit Y/X village return reads in both quest traversal callbacks (0805d384-0805d39a, 0805d3f9-0805d40f). Static ELF comparison only.

Source batch 1346 restored the single-player quest village return read order (0805918b-080591a1) and per-observer character manager lookup in CArena::EndDuel (080bbd40-080bbd58). Static ELF comparison only.

Source batch 1347 restored FWarpInMap layout and construction (0805925c-0805926b, 0805d515-0805d522), arena observer registration (080bd512-080bd5d4), and the fishing miss-rate clamp call (08082af8). Static ELF comparison only.

Source batch 1348 restored destination field read order for the diagnostic and WarpSet in FWarpInMap (0805d515-0805d52c, 0805d54a-0805d557). Static ELF comparison only.

Source batch 1349 restored the fishing::Compute timing table clamp call (08083204-08083225). Static ELF comparison only.

Source batch 1350 restored explicit arena map-list traversal in CArenaMap::SendArenaMapListTo (080ba88d-080ba8a6). Static ELF comparison only.

Source batch 1351 restored the polymorph level allowance and ordered level reads (081812d7-0818131d) and separate arena timeout descriptor reads (080bb661-080bb68f), retaining null guards. Static ELF comparison only.

Source batch 1352 restored signed 16-bit refinement slot conversion side effects and timing before character lookup (08058437-08058464, 080584e7-08058514). Static ELF comparison only.

Source batch 1353 restored per-player potion limit reads in CArena::HandleReadyEvent (080bb09c-080bb111). Static ELF comparison only.

Source batch 1354 restored the replacement VNUM width in mining::RealRefinePick, including the downgrade from GetValue(4) (080ef9e5-080efa1e). Static ELF comparison only.

Source batch 1355 restored the replacement VNUM width in fishing::RealRefineRod (080843bb, 080845c2-080845fb). Static ELF comparison only.

Source batch 1356 restored the prototype fallback in successful fishing::RealRefineRod via CItem::GetRefinedVnum (08084377-08084389). Static ELF comparison only.

Source batch 1357 restored the pick prototype fallback (080ef812-080ef81f) and separate rod item manager lookups (08084389, 080843d6, 080845c9, 08084616). Static ELF comparison only.

Source batch 1358 restored B-name, A-name, B-score, A-score reads for each arena round score notice (080bc877-080bc94f, 080bcab5-080bcb90), preserving formatting and recipients. Static ELF comparison only.

Source batch 1359 restored the item manager capture and fresh factory lookup in pick refinement (080ef74f-080ef765, 080ef81f, 080ef86d, 080ef9ec, 080efa3a) and the final rod audit logger lookup (08084438, 08084678). Static ELF comparison only.

Source batch 1360 restored 32-bit Y/X coordinates for both duel start warps (080bb858-080bb890, 080bb8a4-080bb8dc). Static ELF comparison only.

Source batch 1361 restored signed 32-bit map index in mining OreDrop ground placement (080ef3df-080ef3f0). Static ELF comparison only.

Source batch 1362 restored branch-specific rod audit field reads and the logger lookup after the postfix (08084314-0808433f, 08084550-0808457b). Static ELF comparison only.

Source batch 1363 restored the signed 32-bit tool refinement chance comparisons (08084307-0808430e, 080ef701-080ef70b) and rod stage arithmetic (080842bd). Static ELF comparison only.

Source batch 1364 restored the single attacker map read in CArenaManager::CanAttack (080bc216-080bc244). Static ELF comparison only.

Source batch 1365 restored the Lua item-name reread before pc_count_item diagnostics (0815ebc6-0815ebf8). Static ELF comparison only.

Source batch 1366 restored the 32-bit map comparison and shared lookup key in CArenaManager::OnDead (080bc360-080bc390). Static ELF comparison only.

Source batch 1367 restored signed 32-bit coordinates and lower-X, lower-Y, upper-X, upper-Y checks in pc_have_map_scroll (08161ea7-08161ebf). Static ELF comparison only.

Source batch 1368 restored signed 32-bit position-scroll type comparisons (08161e89-08161e9f, 081621b8-081621bd) and the player A ID snapshot in CArenaMap::GetDuelList (080bbf8e-080bbfb5). Static ELF comparison only.

Source batch 1369 restored separate low-byte empire reads for the defense success and reward window notices (081188b9, 08118908). Static ELF comparison only.

Source batch 1370 restored the castle stone-wave pulse-rate read and 32-bit delay (08118b69-08118b8d), retaining timer cleanup. Static ELF comparison only.

Source batch 1371 restored 32-bit castle stone-wave count updates (08118b6f-08118b7e) and mining chance bonus arithmetic and order (080ef557-080ef571). Static ELF comparison only.

Source batch 1372 restored castle siege spawn placement-before-RNG order (081199b3-08119a17 and the stone callback). Castle frog positions and guard bounds match all 168 active ELF data values. Static ELF comparison only.

Source batch 1373 restored the initial siege delay width and stone event field initialization order (081192e8-08119307, 08119334-0811933b). Static ELF comparison only.

Source batch 1374 corrected fishing::Take elapsed-time scope and restored its 7000 ms timeout diagnostic (ELF .rodata 081fb480). Verified castle random stone and tower VNUMs and selection bounds (081189e0, 08119900) and siege rectangles (08240960). Static ELF comparison only.

Source batch 1375 restored the no-guild empty string branch in guild_around_ranking_string (080d0bee-080d0bf3). Static ELF comparison only.

Source batch 1376 restored the CArena constructor Clear call after event pointer and position initialization (080ba2ec-080ba310). Static ELF comparison only.

Source batch 1377 restored guild war list callback initialization before Lua table creation (080d17c6-080d17d6). Static ELF comparison only.

Source batch 1378 restored the guild war entry argument and guild read order (080d1242-080d1273) and per-map manager lookups in castle_siege (08119129, 0811914e, 08119170). Static ELF comparison only.

Source batch 1379 restored explicit START and END state handling in castle_siege (08119193-081191c0). Static ELF comparison only.

Source batch 1380 restored the low-byte empire value for both final castle tower notice branches (08119e9b, 08119ef4). Static ELF comparison only.

Source batch 1381 restored the START/END phase filter for castle_tower_die (08119d99-08119da9), retaining the empire guard. Static ELF comparison only.

Source batch 1382 restored the killer empire read before the siege phase in castle_tower_die (08119d8c-08119d99). All three tower notice texts match original bytes. Static ELF comparison only.

Source batch 1383 restored the single horse level read in CHorseRider::GetHorseGrade (080c8b18-080c8b36). Static ELF comparison only.

Source batch 1384 restored the wedding end log argument width (0816f94a-0816f96e) and wedding list callback initialization order (080f9226-080f9236). Static ELF comparison only.

Source batch 1385 restored the stored music state read in WeddingMap::SetMusic after filename assignment (0816fc49-0816fc6c). Static ELF comparison only.

Source batch 1386 restored wedding end map reads after map_allow_find and on End failure (080b1be7-080b1bfd, 080b1c4d onward). Static ELF comparison only.

Source batch 1387 restored the 32-bit wedding map field and assignment in WeddingReady (080b1a3b-080b1a4e). Reviewed all source consumers of TWeddingInfo. Static ELF comparison only.

Source batch 1388 restored unsigned 32-bit WeddingMap index storage and registry keys. Checked constructor and registry Find, insert and erase paths against the original 32-bit index representation (0816f650, 08170240). Static ELF comparison only.

Source batch 1389 restored the unsigned 32-bit wedding participant backing fields and reward comparisons (0816f6c0-0816f6c9, 0816fa34-0816fa39). Static ELF comparison only.

Source batch 1390 restored signed 32-bit map index interpretation at wedding private map teardown (08170554-0817056b). Static ELF comparison only.

Source batch 1391 restored signed 32-bit PID arguments for all three wedding map creation diagnostics (081702d2-081702f2, 0817044e-0817046e, 081704dd-081704eb). Static ELF comparison only.

Source batch 1392 restored step-before-map reads in wedding_end_event (0816f5cf-0816f5d6). Wedding map classification and the exit callback match the reviewed ELF paths. Static ELF comparison only.

Source batch 1393 restored the live member end check in WeddingMap::ShoutInMap (0816fbea-0816fbf7), retaining literal message formatting. Confirmed the wedding timer step starts zero through the event allocator. Static ELF comparison only.

Source batch 1394 restored 32-bit marriage bonus item VNUMs (080b0130, 080b0185, 080b019a), and verified all six bonus table rows (0823ebc0-0823ec37). Wedding teardown order matches. Static ELF comparison only.

Source batch 1395 restored the second-then-first 32-bit map comparison in TMarriage::IsNear (080b00d4-080b00dd). Static ELF comparison only.

Source batch 1396 restored the equipped spouse count in TMarriage::GetBonus (080b0148-080b0178, 080b018e, 080b01ee). Both eligible spouses are checked and the grade bonus is applied once. Static ELF comparison only.

Source batch 1397 restored unsigned 32-bit marriage record IDs and registry keys, with normalization at add, lookup, update, removal and wedding transitions (080b1320, 080b1410, 080b19c0, 080b1a60, and spouse login/logout). Static ELF comparison only.

Source batch 1398 restored signed 32-bit recurring delay arithmetic in near_check_event (080b05e1-080b05ee). The initial one-tick delay and event setup match the ELF (080b0600-080b0660). Static ELF comparison only.

Source batch 1399 restored unsigned 32-bit PID sorting for marriage update and removal requests (080b130a-080b1314, 080b1709, 080b1813). Static ELF comparison only.

Source batch 1400 restored the unsigned 32-bit PID comparison and corresponding name order in CManager::RequestAdd (080b1379-080b13a0). The packed 13-byte RequestUpdate layout and write order already match. Static ELF comparison only.

Source batch 1401 restored the shared 32-bit love-point delta in TMarriage::Update (080b07e9, 080b0808, 080b081a). Static ELF comparison only.

Source batch 1402 restored separate character manager reads for the two love-point diagnostic recipients (080b082e-080b0845, 080b0883-080b0897). Point cap arithmetic and display conversions match the reviewed ELF paths. Static ELF comparison only.

Source batch 1403 restored the two point-before-recipient reads in TMarriage::SetMarried (080b0764-080b077a, 080b0782-080b0798). Verified marriage grade clamp and thresholds (080afea0-080afee3). Static ELF comparison only.

Source batch 1404 restored the manager and second-then-first PID reads in TMarriage::RequestEndWedding (080b0aa3-080b0abd). Static ELF comparison only.

Source batch 1405 restored the marriage save manager and field read sequence (080b06bf-080b06ed). Static ELF comparison only.

Source batch 1406 restored the married flag read before assigning either spouse pointer in TMarriage::Login (080b02e0-080b02e4, 080b0310-080b0314). Static ELF comparison only.
