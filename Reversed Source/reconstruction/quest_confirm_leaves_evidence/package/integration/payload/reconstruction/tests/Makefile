# Match the actual compiler output name so existing Windows executables are reusable.
ifeq ($(OS),Windows_NT)
EXEEXT := .exe
CHILD_EXIT_PLATFORM_OBJECTS := child_exit_crt_lifetime.o
CHILD_EXIT_PLATFORM_LDFLAGS := -Wl,--wrap=main
else
EXEEXT :=
endif
RUNTIME_TESTS = quest_confirm_leaves_regression quest_predicate_diagnostics_regression quest_yield_bridges_regression quest_state_lifecycle_regression pc_state_consumers_regression quest_event_flags_regression quest_lua_queries_regression pc_reward_producers_regression pc_build_cancel_regression pc_quest_save_regression pc_quest_info_regression quest_registration_regression pc_quest_storage_regression quest_manager_indices_regression pc_state_helpers_regression pc_flags_timers_regression safebox_closed_regression quest_pc_lookup_regression affect_packet_leaves_regression attribute_tail_regression weighted_attribute_regression skill_factory_regression blend_item_regression locale_regions_regression poly_parser_eval_regression item_addon_regression locale_default_regression poly_owning_lifecycle_regression item_attribute_regression skill_lookup_regression locale_transfer_regression banword_locale_regression quickslot_chain_regression horse_methods_regression item_id_regression locale_combat_data_regression locale_foundation_regression autogive_closed_regression skill_power_regression item_lookup_regression exp_data_regression exp_next_regression stop_muyeong_regression character_save_regression charlog_regression guild_level_regression polymorph_helpers_regression target_walking_regression level_pk_regression stat_primitives_regression party_dependency_regression stat_data_regression item_accessory_setters_regression locale_parser_regression core_regression event_regression heartbeat_regression vector_regression runtime_regression data_regression logging_regression gost_regression memcpy_regression signal_regression grid_regression attribute_regression tea_regression utils_regression utils_numeric_regression utils_strings_regression buffer_regression buffer_manager_regression socket_regression fdwatch_regression desc_output_unit desc_output_integration desc_packet_integration traffic_regression packet_info_regression packet_tables_regression input_regression game_time_regression desc_input_integration desc_flush_regression locale_regression desc_control_regression character_name_regression desc_manager_regression profiler_regression entity_regression sectree_attribute_regression horse_regression character_manager_regression mob_regression quest_flag_regression character_initialize_regression character_creation_regression item_regression item_persistence_regression character_inventory_regression item_ownership_regression character_stat_regression movement_regression text_loader_regression motion_file_regression motion_build_regression character_update_regression item_audit_regression asyncsql_regression asyncsql_connection_regression asyncsql_worker_regression dev_log_regression item_socket_expiry_regression

CXX ?= c++
CXXFLAGS ?= -std=c++03 -Wall -Wextra -pedantic
CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic
SOURCES = core_regression.cpp ../../game/src/FSM.cpp ../../game/src/event_queue.cpp

ITEM_SOCKET_FLAGS = -Isql_shim -I../dependencies/mysql-4.1.3/include -include sql_socket_platform.h
ITEM_SOCKET_SOURCES = ../../game/src/log.cpp dev_log_test_impl.cpp item_socket_support.cpp
ITEM_SOCKET_OBJECTS = item_sql_runtime.o item_sql_semaphore.o item_sql_api.o item_socket_config.o
.PHONY: check check-abi clean
check: $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))
	./quest_confirm_leaves_regression$(EXEEXT)
	./quest_predicate_diagnostics_regression$(EXEEXT)
	./quest_yield_bridges_regression$(EXEEXT)
	./quest_state_lifecycle_regression$(EXEEXT)
	./pc_state_consumers_regression$(EXEEXT)
	./quest_event_flags_regression$(EXEEXT)
	./quest_lua_queries_regression$(EXEEXT)
	./pc_reward_producers_regression$(EXEEXT)
	./pc_build_cancel_regression$(EXEEXT)
	./pc_quest_save_regression$(EXEEXT)
	./pc_quest_info_regression$(EXEEXT)
	./quest_registration_regression$(EXEEXT)
	./pc_quest_storage_regression$(EXEEXT)
	./quest_manager_indices_regression$(EXEEXT)
	./pc_state_helpers_regression$(EXEEXT)
	./pc_flags_timers_regression$(EXEEXT)
	./safebox_closed_regression$(EXEEXT)
	./quest_pc_lookup_regression$(EXEEXT)
	./affect_packet_leaves_regression$(EXEEXT)
	./attribute_tail_regression$(EXEEXT)
	./weighted_attribute_regression$(EXEEXT)
	./skill_factory_regression$(EXEEXT)
	./blend_item_regression$(EXEEXT)
	./locale_regions_regression$(EXEEXT)
	./poly_parser_eval_regression$(EXEEXT)
	./item_addon_regression$(EXEEXT)
	./locale_default_regression$(EXEEXT)
	./poly_owning_lifecycle_regression$(EXEEXT)
	./item_attribute_regression$(EXEEXT)
	./skill_lookup_regression$(EXEEXT)
	./locale_transfer_regression$(EXEEXT)
	./banword_locale_regression$(EXEEXT)
	./quickslot_chain_regression$(EXEEXT)
	./horse_methods_regression$(EXEEXT)
	./item_id_regression$(EXEEXT)
	./locale_combat_data_regression$(EXEEXT)
	./locale_foundation_regression$(EXEEXT)
	./autogive_closed_regression$(EXEEXT)
	./skill_power_regression$(EXEEXT)
	./item_lookup_regression$(EXEEXT)
	./exp_data_regression$(EXEEXT)
	./exp_next_regression$(EXEEXT)
	./stop_muyeong_regression$(EXEEXT)
	./character_save_regression$(EXEEXT)
	./charlog_regression$(EXEEXT)
	./guild_level_regression$(EXEEXT)
	./polymorph_helpers_regression$(EXEEXT)
	./target_walking_regression$(EXEEXT)
	./level_pk_regression$(EXEEXT)
	./stat_primitives_regression$(EXEEXT)
	./party_dependency_regression$(EXEEXT)
	./stat_data_regression$(EXEEXT)
	./item_accessory_setters_regression$(EXEEXT)
	./locale_parser_regression$(EXEEXT)
	./core_regression$(EXEEXT)
	./event_regression$(EXEEXT)
	./heartbeat_regression$(EXEEXT)
	./vector_regression$(EXEEXT)
	./runtime_regression$(EXEEXT)
	./data_regression$(EXEEXT)
	./logging_regression$(EXEEXT)
	./gost_regression$(EXEEXT)
	./memcpy_regression$(EXEEXT)
	./signal_regression$(EXEEXT)
	./grid_regression$(EXEEXT)
	./attribute_regression$(EXEEXT)
	./tea_regression$(EXEEXT)
	./utils_regression$(EXEEXT)
	./utils_numeric_regression$(EXEEXT)
	./utils_strings_regression$(EXEEXT)
	./buffer_regression$(EXEEXT)
	./buffer_manager_regression$(EXEEXT)
	./socket_regression$(EXEEXT)
	./fdwatch_regression$(EXEEXT)
	./desc_output_unit$(EXEEXT)
	./desc_output_integration$(EXEEXT)
	./desc_packet_integration$(EXEEXT)
	./traffic_regression$(EXEEXT)
	./packet_info_regression$(EXEEXT)
	./packet_tables_regression$(EXEEXT)
	./input_regression$(EXEEXT)
	./game_time_regression$(EXEEXT)
	./desc_input_integration$(EXEEXT)
	./desc_flush_regression$(EXEEXT)
	./locale_regression$(EXEEXT)
	./desc_control_regression$(EXEEXT)
	./character_name_regression$(EXEEXT)
	./desc_manager_regression$(EXEEXT)
	./profiler_regression$(EXEEXT)
	./entity_regression$(EXEEXT)
	./sectree_attribute_regression$(EXEEXT)
	./horse_regression$(EXEEXT)
	./character_manager_regression$(EXEEXT)
	./mob_regression$(EXEEXT)
	./quest_flag_regression$(EXEEXT)
	./character_initialize_regression$(EXEEXT)
	./character_creation_regression$(EXEEXT)
	./item_regression$(EXEEXT)
	./item_persistence_regression$(EXEEXT)
	./character_inventory_regression$(EXEEXT)
	./item_ownership_regression$(EXEEXT)
	./character_stat_regression$(EXEEXT)
	./movement_regression$(EXEEXT)
	./text_loader_regression$(EXEEXT)
	./motion_file_regression$(EXEEXT)
	./motion_build_regression$(EXEEXT)
	./character_update_regression$(EXEEXT)
	./item_audit_regression$(EXEEXT)
	./asyncsql_regression$(EXEEXT)
	./asyncsql_connection_regression$(EXEEXT)
	./asyncsql_worker_regression$(EXEEXT)
	./dev_log_regression$(EXEEXT)
	./item_socket_expiry_regression$(EXEEXT)

check-abi:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_registration_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_manager_indices_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only pc_state_helpers_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only pc_flags_timers_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_pc_lookup_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only attribute_tail_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only weighted_attribute_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only skill_factory_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only blend_item_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only poly_parser_eval_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only item_addon_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only poly_owning_lifecycle_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only item_attribute_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only skill_lookup_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quickslot_chain_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-invalid-offsetof -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only horse_methods_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only item_id_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only locale_foundation_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ITEM_SOCKET_FLAGS) -Wno-long-long -m32 -fsyntax-only autogive_closed_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-invalid-offsetof -m32 -fsyntax-only skill_power_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only item_lookup_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only exp_data_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only stat_leaves_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only character_save_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SQL_TEST_FLAGS) -Wno-long-long -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only charlog_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-invalid-offsetof -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only guild_level_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only polymorph_helpers_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only target_walking_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only level_pk_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only stat_primitives_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only party_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only stat_data_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only locale_translation_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only dev_log_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SQL_TEST_FLAGS) -Wno-long-long -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only item_audit_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -Wno-invalid-offsetof -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only character_update_abi.cpp text_loader_abi.cpp motion_abi.cpp affect_abi.cpp item_abi.cpp character_abi.cpp mob_abi.cpp character_bases_abi.cpp quest_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-invalid-offsetof -m32 -fsyntax-only input_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only packet_info_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-invalid-offsetof -m32 -D_USE_32BIT_TIME_T -fsyntax-only traffic_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only abi_layout.cpp
	$(CC) -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -m32 -fsyntax-only socket_abi.c
	$(CC) -Ifdwatch_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -m32 -fsyntax-only fdwatch_abi.c
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -Wno-invalid-offsetof -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -D_USE_32BIT_TIME_T -fsyntax-only desc_abi.cpp

core_regression$(EXEEXT): $(SOURCES) ../../game/src/FSM.h ../../game/src/state.h ../../game/src/event_queue.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o $@

event_regression$(EXEEXT): event_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/event.h ../../game/src/event_queue.h ../../libthecore/include/typedef.h ../../libthecore/include/heart.h ../../libthecore/include/main.h ../../libthecore/include/log.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) event_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@

heartbeat_regression$(EXEEXT): heartbeat_regression.c ../../libthecore/src/heart.c ../../libthecore/src/utils.c ../../libthecore/include/heart.h ../../libthecore/include/utils.h ../../libthecore/include/log.h posix_shim/sys/time.h posix_shim/sys/select.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) heartbeat_regression.c utils_link_stubs.c ../../libthecore/src/heart.c ../../libthecore/src/utils.c $(LDFLAGS) -o $@

vector_regression$(EXEEXT): vector_regression.cpp ../../game/src/vector.cpp ../../game/src/vector.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) vector_regression.cpp ../../game/src/vector.cpp $(LDFLAGS) -lm -o $@

runtime_regression$(EXEEXT): runtime_regression.c runtime_hooks.h ../../libthecore/src/main.c ../../libthecore/include/main.h ../../libthecore/include/heart.h ../../libthecore/include/utils.h ../../libthecore/include/log.h ../../libthecore/include/typedef.h posix_shim/unistd.h posix_shim/sys/time.h
	$(CC) -Iposix_shim -include runtime_hooks.h $(CPPFLAGS) $(CFLAGS) runtime_regression.c ../../libthecore/src/main.c $(LDFLAGS) -o $@

data_regression$(EXEEXT): data_regression.cpp ../../game/src/crc32.cpp ../../game/src/crc32.h ../../game/src/file_loader.cpp ../../game/src/file_loader.h ../../libthecore/include/typedef.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) data_regression.cpp ../../game/src/crc32.cpp ../../game/src/file_loader.cpp $(LDFLAGS) -o $@

logging_regression$(EXEEXT): logging_regression.c logging_hooks.h ../../libthecore/src/log.c ../../libthecore/src/utils.c ../../libthecore/include/log.h ../../libthecore/include/utils.h ../../libthecore/include/memcpy.h posix_shim/sys/stat.h posix_shim/dirent.h posix_shim/sys/time.h posix_shim/sys/select.h
	$(CC) -Iposix_shim -include logging_hooks.h $(CPPFLAGS) $(CFLAGS) logging_regression.c utils_link_stubs.c ../../libthecore/src/utils.c $(LDFLAGS) -o $@

utils_regression$(EXEEXT): utils_regression.c utils_process_hooks.h ../../libthecore/src/utils.c ../../libthecore/src/signal.c ../../libthecore/include/utils.h
	$(CC) -Iposix_shim -Isignal_shim -include utils_process_hooks.h $(CPPFLAGS) $(CFLAGS) utils_regression.c ../../libthecore/src/utils.c ../../libthecore/src/signal.c $(LDFLAGS) -o $@

utils_numeric_regression$(EXEEXT): utils_numeric_regression.c utils_numeric_support.c ../../libthecore/src/utils.c ../../libthecore/include/utils.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) utils_numeric_regression.c utils_numeric_support.c ../../libthecore/src/utils.c $(LDFLAGS) -o $@

utils_strings_impl.o: utils_strings_hooks.h ../../libthecore/src/utils.c ../../libthecore/include/utils.h
	$(CC) -Iposix_shim -include utils_strings_hooks.h $(CPPFLAGS) $(CFLAGS) -c ../../libthecore/src/utils.c -o $@

utils_strings_regression$(EXEEXT): utils_strings_regression.c utils_numeric_support.c utils_strings_unused.c utils_strings_impl.o ../../libthecore/include/utils.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) utils_strings_regression.c utils_numeric_support.c utils_strings_unused.c utils_strings_impl.o $(LDFLAGS) -o $@

gost_regression$(EXEEXT): gost_regression.c ../../libthecore/src/gost.c ../../libthecore/include/crypt.h ../../libthecore/include/typedef.h
	$(CC) $(CPPFLAGS) $(CFLAGS) gost_regression.c ../../libthecore/src/gost.c $(LDFLAGS) -o $@

memcpy_regression$(EXEEXT): memcpy_regression.c ../../libthecore/src/memcpy.c ../../libthecore/include/memcpy.h ../../libthecore/include/log.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -mmmx -msse memcpy_regression.c $(LDFLAGS) -o $@

clean:
	$(RM) quest_registration_desc_output_syscalls.o
	$(RM) quest_manager_indices_desc_output_syscalls.o
	$(RM) pc_flags_timers_desc_output_syscalls.o
	$(RM) quest_pc_lookup_desc_output_syscalls.o
	$(RM) attribute_tail_desc_output_syscalls.o
	$(RM) weighted_desc_output_syscalls.o
	$(RM) blend_desc_output_syscalls.o
	$(RM) item_addon_desc_output_syscalls.o
	$(RM) item_attribute_desc_output_syscalls.o
	$(RM) $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))
	$(RM) character_creation_regression character_initialize_regression quest_flag_regression mob_regression character_manager_regression horse_regression horse_utils.o sectree_attribute_regression entity_regression profiler_regression desc_manager_regression character_name_regression desc_control_regression locale_regression desc_flush_regression desc_input_integration input_pong_test.o input_regression game_time_regression game_time_impl.o packet_tables_regression packet_info_regression traffic_regression desc_packet_integration desc_tea.o core_regression event_regression heartbeat_regression vector_regression runtime_regression data_regression logging_regression gost_regression memcpy_regression signal_regression grid_regression attribute_regression tea_regression utils_regression utils_numeric_regression utils_strings_regression buffer_regression buffer_manager_regression socket_regression fdwatch_regression desc_output_unit desc_output_integration utils_strings_impl.o buffer_manager_core.o fdwatch_test_impl.o desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o

DESC_PACKET_SOURCES = ../../game/src/constants.cpp ../../game/src/char_skill.cpp ../../game/src/char_battle.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp character_fixture.cpp quest_flag_fixture.cpp ../../game/src/horse_rider.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp ../../game/src/FSM.cpp ../../game/src/char_manager.cpp ../../game/src/desc_client.cpp ../../game/src/desc_manager.cpp ../../game/src/input.cpp ../../game/src/input_login.cpp ../../game/src/input_main.cpp ../../game/src/input_auth.cpp ../../game/src/packet_info.cpp ../../game/src/main.cpp ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/TrafficProfiler.cpp ../../game/src/sequence.cpp ../../game/src/buffer_manager.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp ../../game/src/banword.cpp ../../game/src/cmd_gm.cpp
DESC_PACKET_OBJECTS = desc_tea.o movement_duration_impl.o

DESC_TEST_HEADERS = ../../game/src/config.h ../../game/src/packet.h ../../game/src/protocol.h ../../game/src/TrafficProfiler.h ../../game/src/utils.h ../../game/src/desc.h ../../common/tables.h desc_fixture.h ../../libthecore/include/buffer.h ../../libthecore/include/fdwatch.h ../../libthecore/include/socket.h ../../libthecore/include/typedef.h

desc_output_unit$(EXEEXT): desc_output_unit.cpp desc_fixture.cpp character_unused_clock.cpp ../../game/src/desc.cpp $(DESC_TEST_HEADERS) $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o item_socket_config.o desc_rng_only.o desc_rng_os_boundary.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_output_unit.cpp desc_fixture.cpp character_unused_clock.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o item_socket_config.o desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

desc_output_syscalls.o: desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c desc_output_syscalls.c -o $@

desc_fdwatch.o: ../../libthecore/src/fdwatch.c ../../libthecore/include/fdwatch.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h posix_shim/sys/time.h
	$(CC) -Ifdwatch_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c ../../libthecore/src/fdwatch.c -o $@

desc_socket.o: ../../libthecore/src/socket.c ../../libthecore/include/socket.h socket_hooks.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Isocket_shim -Iposix_shim -include socket_hooks.h $(CPPFLAGS) $(CFLAGS) -c ../../libthecore/src/socket.c -o $@

desc_utils.o: ../../libthecore/src/utils.c ../../libthecore/include/utils.h ../../libthecore/include/memcpy.h posix_shim/sys/time.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c ../../libthecore/src/utils.c -o $@

desc_output_integration$(EXEEXT): desc_output_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_TEST_HEADERS) $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o item_socket_config.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_output_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o item_socket_config.o $(LDFLAGS) -o $@

fdwatch_test_impl.o: fdwatch_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h posix_shim/sys/time.h ../../libthecore/src/fdwatch.c ../../libthecore/include/fdwatch.h ../../libthecore/include/log.h ../../libthecore/include/typedef.h
	$(CC) -Ifdwatch_shim -Iposix_shim -include fdwatch_hooks.h $(CPPFLAGS) $(CFLAGS) -c ../../libthecore/src/fdwatch.c -o $@

fdwatch_regression$(EXEEXT): fdwatch_regression.c fdwatch_test_impl.o ../../libthecore/include/fdwatch.h
	$(CC) -Ifdwatch_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) fdwatch_regression.c fdwatch_test_impl.o $(LDFLAGS) -o $@

socket_regression$(EXEEXT): socket_regression.c socket_hooks.h socket_shim/sys/socket.h socket_shim/netinet/in.h socket_shim/arpa/inet.h socket_shim/netdb.h socket_shim/errno.h socket_shim/unistd.h socket_shim/fcntl.h posix_shim/sys/time.h ../../libthecore/src/socket.c ../../libthecore/include/socket.h ../../libthecore/include/typedef.h ../../libthecore/include/memcpy.h ../../libthecore/include/log.h
	$(CC) -Isocket_shim -Iposix_shim -include socket_hooks.h $(CPPFLAGS) $(CFLAGS) socket_regression.c ../../libthecore/src/socket.c $(LDFLAGS) -o $@

buffer_regression$(EXEEXT): buffer_regression.c ../../libthecore/src/buffer.c ../../libthecore/include/buffer.h ../../libthecore/include/typedef.h ../../libthecore/include/memcpy.h ../../libthecore/include/log.h ../../libthecore/include/utils.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) buffer_regression.c $(LDFLAGS) -o $@

buffer_manager_core.o: ../../libthecore/src/buffer.c ../../libthecore/include/buffer.h ../../libthecore/include/typedef.h ../../libthecore/include/memcpy.h ../../libthecore/include/log.h ../../libthecore/include/utils.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c ../../libthecore/src/buffer.c -o $@

buffer_manager_regression$(EXEEXT): buffer_manager_regression.cpp buffer_manager_core.o ../../game/src/buffer_manager.cpp ../../game/src/buffer_manager.h ../../common/singleton.h ../../libthecore/include/buffer.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) buffer_manager_regression.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o $(LDFLAGS) -o $@

signal_regression$(EXEEXT): signal_regression.c signal_hooks.h ../../libthecore/src/signal.c ../../libthecore/include/signal.h ../../libthecore/include/utils.h signal_shim/signal.h signal_shim/sys/time.h signal_shim/sys/wait.h
	$(CC) -Isignal_shim -include signal_hooks.h $(CPPFLAGS) $(CFLAGS) signal_regression.c ../../libthecore/src/signal.c $(LDFLAGS) -o $@

grid_regression$(EXEEXT): grid_regression.cpp ../../libgame/src/grid.cc ../../libgame/include/grid.h ../../libthecore/include/memcpy.h
	$(CXX) -I../.. $(CPPFLAGS) $(CXXFLAGS) grid_regression.cpp ../../libgame/src/grid.cc $(LDFLAGS) -o $@

attribute_regression$(EXEEXT): attribute_regression.cpp ../../libgame/src/attribute.cc ../../libgame/include/attribute.h ../../libthecore/include/typedef.h ../../libthecore/include/memcpy.h
	$(CXX) -I../.. $(CPPFLAGS) $(CXXFLAGS) attribute_regression.cpp ../../libgame/src/attribute.cc $(LDFLAGS) -o $@

tea_regression$(EXEEXT): tea_regression.c tea_oracle.inc ../../libthecore/src/tea.c ../../libthecore/include/crypt.h ../../libthecore/include/typedef.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -Wno-parentheses tea_regression.c ../../libthecore/src/tea.c $(LDFLAGS) -o $@

heartbeat_regression logging_regression: utils_link_stubs.c ../../libthecore/include/memcpy.h

desc_tea.o: ../../libthecore/src/tea.c ../../libthecore/include/crypt.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -Wno-parentheses -c ../../libthecore/src/tea.c -o $@

desc_packet_integration$(EXEEXT): desc_packet_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_TEST_HEADERS) $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o item_socket_config.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_packet_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o item_socket_config.o $(LDFLAGS) -o $@

traffic_regression$(EXEEXT): traffic_regression.cpp ../../game/src/TrafficProfiler.cpp ../../game/src/TrafficProfiler.h ../../common/singleton.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) traffic_regression.cpp ../../game/src/TrafficProfiler.cpp $(LDFLAGS) -o $@

packet_info_regression$(EXEEXT): packet_info_regression.cpp ../../game/src/packet_info.cpp ../../game/src/packet_info.h ../../libthecore/include/utils.h
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long packet_info_regression.cpp ../../game/src/packet_info.cpp $(LDFLAGS) -o $@

packet_tables_regression$(EXEEXT): packet_tables_regression.cpp ../../game/src/packet_info.cpp ../../game/src/input_udp.cpp ../../game/src/packet_info.h
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long packet_tables_regression.cpp ../../game/src/packet_info.cpp ../../game/src/input_udp.cpp $(LDFLAGS) -o $@

input_regression$(EXEEXT): input_regression.cpp desc_fixture.cpp character_unused_clock.cpp input_pong_test.o ../../game/src/input.h ../../game/src/desc.cpp ../../game/src/packet_info.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o item_socket_config.o desc_rng_only.o desc_rng_os_boundary.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long input_regression.cpp desc_fixture.cpp character_unused_clock.cpp input_pong_test.o ../../game/src/desc.cpp $(filter-out ../../game/src/input.cpp ../../game/src/main.cpp,$(DESC_PACKET_SOURCES)) $(DESC_PACKET_OBJECTS) buffer_manager_core.o item_socket_config.o desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

game_time_impl.o: ../../game/src/utils.cpp ../../game/src/utils.h game_time_hooks.h
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -include game_time_hooks.h -c ../../game/src/utils.cpp -o $@

game_time_regression$(EXEEXT): game_time_regression.cpp game_time_impl.o ../../game/src/utils.h desc_rng_only.o desc_rng_os_boundary.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) game_time_regression.cpp game_time_impl.o desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

desc_input_integration$(EXEEXT): desc_input_integration.cpp desc_fixture.cpp ../../game/src/input.cpp ../../game/src/input.h ../../game/src/desc.cpp ../../game/src/packet_info.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o item_socket_config.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_input_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o item_socket_config.o $(LDFLAGS) -o $@

desc_flush_regression$(EXEEXT): desc_flush_regression.cpp desc_fixture.cpp character_unused_clock.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o item_socket_config.o desc_rng_only.o desc_rng_os_boundary.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_flush_regression.cpp desc_fixture.cpp character_unused_clock.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o item_socket_config.o desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

input_pong_test.o: ../../game/src/input.cpp ../../game/src/input.h pong_hooks.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -include pong_hooks.h -c ../../game/src/input.cpp -o $@

locale_regression$(EXEEXT): locale_regression.cpp ../../game/src/locale_service.cpp ../../game/src/config.cpp ../../game/src/locale.cpp ../../game/src/locale_service.h ../../game/src/locale.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O2 -flto locale_regression.cpp ../../game/src/locale_service.cpp ../../game/src/config.cpp ../../game/src/locale.cpp $(LDFLAGS) -o $@

desc_control_regression$(EXEEXT): desc_control_regression.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o item_socket_config.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_control_regression.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o item_socket_config.o $(LDFLAGS) -o $@

character_name_regression$(EXEEXT): character_name_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc character_fixture.h ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/char.h ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -O2 -flto -Wno-long-long character_name_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

desc_manager_regression$(EXEEXT): desc_manager_regression.cpp ../../game/src/desc_manager.cpp ../../game/src/desc_manager.h ../../common/stl.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) desc_manager_regression.cpp ../../game/src/desc_manager.cpp $(LDFLAGS) -o $@

profiler_regression$(EXEEXT): profiler_regression.cpp ../../game/src/profiler.h ../../common/stl.h
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long profiler_regression.cpp $(LDFLAGS) -o $@

entity_regression$(EXEEXT): entity_regression.cpp character_fixture.cpp ../../game/src/horse_rider.cpp ../../game/src/FSM.cpp ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/sectree.cpp ../../libgame/src/attribute.cc entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp ../../game/src/entity.h ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -O2 -flto -Wno-long-long entity_regression.cpp character_fixture.cpp ../../game/src/horse_rider.cpp ../../game/src/FSM.cpp ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/sectree.cpp ../../libgame/src/attribute.cc entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

sectree_attribute_regression$(EXEEXT): sectree_attribute_regression.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/sectree.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) sectree_attribute_regression.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc $(LDFLAGS) -o $@

horse_utils.o: ../../libthecore/src/utils.c
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) -ffunction-sections -fdata-sections -c ../../libthecore/src/utils.c -o $@

horse_regression$(EXEEXT): horse_regression.cpp ../../game/src/horse_rider.cpp ../../game/src/config.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp horse_utils.o
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long horse_regression.cpp ../../game/src/horse_rider.cpp ../../game/src/config.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp horse_utils.o $(LDFLAGS) -Wl,--gc-sections -o $@


mob_regression$(EXEEXT): mob_regression.cpp ../../game/src/mob_manager.cpp ../../game/src/mob_manager.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Iposix_shim mob_regression.cpp ../../game/src/mob_manager.cpp $(LDFLAGS) -o $@

quest_flag_regression$(EXEEXT): quest_flag_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_flag_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o $(LDFLAGS) -o $@

character_initialize_regression$(EXEEXT): character_initialize_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc character_fixture.h ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/char.h ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -O2 -flto -Wno-long-long character_initialize_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

character_manager_regression$(EXEEXT): character_manager_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc character_fixture.h ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/char.h ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -O2 -flto -Wno-long-long character_manager_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

character_creation_regression$(EXEEXT): character_creation_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc character_fixture.h ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/char.h ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -O2 -flto -Wno-long-long character_creation_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

item_regression$(EXEEXT): item_regression.cpp item_unused_database.cpp ../../game/src/item_manager.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/char_item.cpp ../../game/src/item.h buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES) ../../game/src/locale.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -O2 -flto -Wno-long-long item_regression.cpp item_unused_database.cpp ../../game/src/item_manager.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/char_item.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/questpc.cpp ../../game/src/questevent.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/locale_service.cpp ../../game/src/locale.cpp $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp desc_rng_only.o desc_rng_os_boundary.o $(LDFLAGS) -o $@

item_persistence_regression$(EXEEXT): item_persistence_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_persistence_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

character_inventory_regression$(EXEEXT): character_inventory_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_inventory_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

item_ownership_regression$(EXEEXT): item_ownership_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_ownership_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

character_stat_regression$(EXEEXT): character_stat_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/char_affect.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_stat_regression.cpp ../../game/src/char_affect.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

movement_regression$(EXEEXT): movement_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long movement_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

movement_duration_impl.o: movement_duration_impl.cpp ../../game/src/utils.cpp ../../game/src/utils.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c movement_duration_impl.cpp -o $@

text_loader_regression$(EXEEXT): text_loader_regression.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/text_file_loader.h ../../common/pool.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long text_loader_regression.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp $(LDFLAGS) -o $@

motion_file_regression$(EXEEXT): motion_file_regression.cpp ../../game/src/motion.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long motion_file_regression.cpp ../../game/src/motion.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp $(LDFLAGS) -o $@

motion_build_regression$(EXEEXT): motion_build_regression.cpp ../../game/src/motion.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long motion_build_regression.cpp ../../game/src/motion.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp $(LDFLAGS) -o $@


character_update_regression$(EXEEXT): character_update_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES) guild_lifecycle_fixture.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_update_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) guild_lifecycle_fixture.cpp $(LDFLAGS) -o $@

SQL_TEST_FLAGS = -Isql_shim -I../dependencies/mysql-4.1.3/include -include sql_shim/mysql_platform.h
SQL_CONNECTION_FIXTURE = sql_connection_fixture.cpp
ITEM_AUDIT_SOURCES = ../../game/src/log.cpp ../../libsql/AsyncSQL.cpp ../../libsql/Semaphore.cpp ../../game/src/config.cpp ../../game/src/locale_service.cpp ../../game/src/locale.cpp
item_audit_regression$(EXEEXT): item_audit_regression.cpp $(SQL_CONNECTION_FIXTURE) $(ITEM_AUDIT_SOURCES) ../../libsql/AsyncSQL.h ../../libsql/Semaphore.h ../../game/src/log.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O2 -flto $(SQL_TEST_FLAGS) -Wno-long-long item_audit_regression.cpp $(SQL_CONNECTION_FIXTURE) $(ITEM_AUDIT_SOURCES) $(LDFLAGS) -o $@

asyncsql_regression$(EXEEXT): asyncsql_regression.cpp $(SQL_CONNECTION_FIXTURE) ../../libsql/AsyncSQL.cpp ../../libsql/Semaphore.cpp ../../libsql/AsyncSQL.h ../../libsql/Semaphore.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SQL_TEST_FLAGS) -Wno-long-long asyncsql_regression.cpp $(SQL_CONNECTION_FIXTURE) ../../libsql/AsyncSQL.cpp ../../libsql/Semaphore.cpp $(LDFLAGS) -o $@

asyncsql_connection_regression$(EXEEXT): asyncsql_connection_regression.cpp $(SQL_CONNECTION_FIXTURE) ../../libsql/AsyncSQL.cpp ../../libsql/Semaphore.cpp ../../libsql/AsyncSQL.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SQL_TEST_FLAGS) -Wno-long-long asyncsql_connection_regression.cpp $(SQL_CONNECTION_FIXTURE) ../../libsql/AsyncSQL.cpp ../../libsql/Semaphore.cpp $(LDFLAGS) -o $@

asyncsql_worker_regression$(EXEEXT): asyncsql_worker_regression.cpp $(SQL_CONNECTION_FIXTURE) ../../libsql/AsyncSQL.cpp ../../libsql/Semaphore.cpp ../../libsql/AsyncSQL.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SQL_TEST_FLAGS) -Wno-long-long asyncsql_worker_regression.cpp $(SQL_CONNECTION_FIXTURE) ../../libsql/AsyncSQL.cpp ../../libsql/Semaphore.cpp $(LDFLAGS) -o $@

dev_log_regression$(EXEEXT): dev_log_regression.cpp dev_log_test_impl.cpp dev_log_hooks.h ../../game/src/dev_log.cpp ../../game/src/dev_log.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) dev_log_regression.cpp dev_log_test_impl.cpp $(LDFLAGS) -o $@

item_sql_runtime.o: ../../libsql/AsyncSQL.cpp ../../libsql/AsyncSQL.h sql_shim/mysql_platform.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SQL_TEST_FLAGS) -Wno-long-long -c ../../libsql/AsyncSQL.cpp -o $@
item_sql_semaphore.o: ../../libsql/Semaphore.cpp ../../libsql/Semaphore.h sql_shim/semaphore.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Isql_shim -c ../../libsql/Semaphore.cpp -o $@
item_sql_api.o: sql_connection_fixture.cpp sql_connection_fixture.h sql_shim/mysql_platform.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SQL_TEST_FLAGS) -DMCNCC_TEST_SQL_EXTERNAL_SYS_ERR -Wno-long-long -c sql_connection_fixture.cpp -o $@
item_socket_config.o: item_socket_config.cpp ../../game/src/config.cpp ../../game/src/config.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c item_socket_config.cpp -o $@

item_socket_expiry_regression$(EXEEXT): item_socket_expiry_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_socket_expiry_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

ifneq ($(EXEEXT),)
.PHONY: $(RUNTIME_TESTS)
$(RUNTIME_TESTS): %: %$(EXEEXT)
endif

locale_parser_regression$(EXEEXT): locale_parser_regression.cpp ../../game/src/locale.cpp ../../game/src/locale.hpp ../../game/src/locale_service.cpp ../../game/src/config.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O2 -flto locale_parser_regression.cpp ../../game/src/locale_service.cpp ../../game/src/config.cpp $(LDFLAGS) -o $@

item_accessory_setters_regression$(EXEEXT): item_accessory_setters_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) accessory_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_accessory_setters_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) accessory_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

accessory_desc_output_syscalls.o: accessory_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c accessory_desc_output_syscalls.c -o $@

stat_data_regression$(EXEEXT): stat_data_regression.cpp stat_data_golden.h ../../game/src/constants.cpp ../../game/src/constants.h ../../common/length.h ../../game/src/char.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) stat_data_regression.cpp ../../game/src/constants.cpp $(LDFLAGS) -o $@

party_dependency_regression$(EXEEXT): party_dependency_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) party_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long party_dependency_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) party_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

party_desc_output_syscalls.o: party_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c party_desc_output_syscalls.c -o $@

# Filter avoids a duplicate char_battle.cpp if parent adds it to the shared graph.
STAT_PRIMITIVES_BATTLE_SOURCE = $(filter-out $(DESC_PACKET_SOURCES),../../game/src/char_battle.cpp)
STAT_PRIMITIVES_SOURCES = $(STAT_PRIMITIVES_BATTLE_SOURCE) ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(ITEM_SOCKET_SOURCES)
STAT_PRIMITIVES_OBJECTS = $(DESC_PACKET_OBJECTS) stat_primitives_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS)

stat_primitives_regression$(EXEEXT): stat_primitives_regression.cpp stat_primitives_desc_output_syscalls.c $(STAT_PRIMITIVES_SOURCES) $(STAT_PRIMITIVES_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/input.h ../../game/src/packet.h ../../common/tables.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long stat_primitives_regression.cpp $(STAT_PRIMITIVES_SOURCES) $(STAT_PRIMITIVES_OBJECTS) $(LDFLAGS) -o $@

stat_primitives_desc_output_syscalls.o: stat_primitives_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c stat_primitives_desc_output_syscalls.c -o $@


level_pk_regression$(EXEEXT): level_pk_regression.cpp guild_lifecycle_fixture.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) level_pk_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long level_pk_regression.cpp guild_lifecycle_fixture.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) level_pk_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

level_pk_desc_output_syscalls.o: level_pk_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c level_pk_desc_output_syscalls.c -o $@

TARGET_WALKING_SOURCES = ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(ITEM_SOCKET_SOURCES)
TARGET_WALKING_OBJECTS = $(DESC_PACKET_OBJECTS) target_walk_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS)
target_walking_regression$(EXEEXT): target_walking_regression.cpp $(TARGET_WALKING_SOURCES) $(TARGET_WALKING_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/char_manager.h ../../game/src/packet.h ../../common/length.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long target_walking_regression.cpp $(TARGET_WALKING_SOURCES) $(TARGET_WALKING_OBJECTS) $(LDFLAGS) -o $@

target_walk_desc_output_syscalls.o: target_walk_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c target_walk_desc_output_syscalls.c -o $@


# Shared graph already includes real CHAR/CMob/quest/locale/point modules.
POLY_HELPER_SOURCES = $(filter-out desc_fixture.cpp ../../game/src/char_skill.cpp,$(TARGET_WALKING_SOURCES)) helper_desc_fixture.cpp ../../game/src/char_skill.cpp
POLY_HELPER_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) helper_desc_output_syscalls.o

polymorph_helpers_regression$(EXEEXT): polymorph_helpers_regression.cpp $(POLY_HELPER_SOURCES) $(POLY_HELPER_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/char_manager.h ../../game/src/packet.h ../../common/length.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long polymorph_helpers_regression.cpp $(POLY_HELPER_SOURCES) $(POLY_HELPER_OBJECTS) $(LDFLAGS) -o $@

helper_desc_output_syscalls.o: helper_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c helper_desc_output_syscalls.c -o $@

# Full real shared graph; keep the existing full-data guild lifecycle fixture test-only.
GUILD_LEVEL_SOURCES = under_any_war_regression.cpp sguildwar_lifecycle_fixture.cpp $(filter-out ../../game/src/guild.cpp guild_lifecycle_fixture.cpp,$(TARGET_WALKING_SOURCES)) ../../game/src/guild.cpp guild_lifecycle_fixture.cpp
GUILD_LEVEL_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) guild_level_desc_output_syscalls.o
guild_level_regression$(EXEEXT): guild_level_regression.cpp $(GUILD_LEVEL_SOURCES) $(GUILD_LEVEL_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/guild.h ../../game/src/char.h ../../game/src/packet.h ../../common/tables.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long guild_level_regression.cpp $(GUILD_LEVEL_SOURCES) $(GUILD_LEVEL_OBJECTS) $(LDFLAGS) -o $@

guild_level_desc_output_syscalls.o: guild_level_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c guild_level_desc_output_syscalls.c -o $@
clean: clean-guild-level-observer
clean-guild-level-observer:
	$(RM) guild_level_desc_output_syscalls.o

charlog_regression$(EXEEXT): charlog_regression.cpp $(SQL_CONNECTION_FIXTURE) $(ITEM_AUDIT_SOURCES) ../../libsql/AsyncSQL.h ../../libsql/Semaphore.h ../../game/src/log.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O2 -flto $(SQL_TEST_FLAGS) -Wno-long-long charlog_regression.cpp $(SQL_CONNECTION_FIXTURE) $(ITEM_AUDIT_SOURCES) $(LDFLAGS) -o $@

character_save_regression$(EXEEXT): character_save_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/char_manager.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_save_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o $(LDFLAGS) -o $@

stop_muyeong_regression$(EXEEXT): stop_muyeong_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o $(DESC_TEST_HEADERS) ../../game/src/char.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long stop_muyeong_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o $(LDFLAGS) -o $@

exp_data_regression$(EXEEXT): exp_data_regression.cpp exp_data_type_checks.h exp_data_golden.h ../../game/src/constants.cpp ../../game/src/constants.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) exp_data_regression.cpp ../../game/src/constants.cpp $(LDFLAGS) -o $@

EXP_NEXT_SOURCES = $(TARGET_WALKING_SOURCES)
EXP_NEXT_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o
exp_next_regression$(EXEEXT): exp_next_regression.cpp exp_data_type_checks.h exp_data_golden.h $(EXP_NEXT_SOURCES) $(EXP_NEXT_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/constants.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long exp_next_regression.cpp $(EXP_NEXT_SOURCES) $(EXP_NEXT_OBJECTS) $(LDFLAGS) -o $@

ITEM_LOOKUP_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o
item_lookup_regression$(EXEEXT): item_lookup_regression.cpp $(TARGET_WALKING_SOURCES) $(ITEM_LOOKUP_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/item_manager.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_lookup_regression.cpp $(TARGET_WALKING_SOURCES) $(ITEM_LOOKUP_OBJECTS) $(LDFLAGS) -o $@

skill_power_regression$(EXEEXT): skill_power_regression.cpp skill_power_alloc_observer.cpp skill_power_alloc_observer.h skill_power_api_checks.h ../../game/src/constants.cpp ../../game/src/constants.h ../../common/singleton.h ../../common/length.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) skill_power_regression.cpp skill_power_alloc_observer.cpp ../../game/src/constants.cpp $(LDFLAGS) -o $@

# Parent-only recipe, copy regression into reconstruction/tests. Do NOT compile
# AutoGiveItem/CreateItem yet: their actual app dependency frontier remains open.
# Assumes closed bodies placed into game/src/db.cpp, char_item.cpp and
# char_quickslot.cpp. If parent chooses other files, change only this variable.
AUTOGIVE_CLOSED_NEW_SOURCES = ../../game/src/db.cpp ../../game/src/char_quickslot.cpp
# Existing DESC_PACKET_SOURCES already includes full char_item.cpp. If it is
# updated globally to include char_quickslot.cpp, filter duplicate below.
AUTOGIVE_CLOSED_SOURCES = $(filter-out $(DESC_PACKET_SOURCES),$(AUTOGIVE_CLOSED_NEW_SOURCES)) ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(ITEM_SOCKET_SOURCES)
AUTOGIVE_CLOSED_OBJECTS = $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS)
autogive_closed_regression$(EXEEXT): autogive_closed_regression.cpp $(AUTOGIVE_CLOSED_SOURCES) $(AUTOGIVE_CLOSED_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long autogive_closed_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp $(AUTOGIVE_CLOSED_SOURCES) $(AUTOGIVE_CLOSED_OBJECTS) $(LDFLAGS) -o $@
# Add target to RUNTIME_TESTS + check recipe. O2 rebuild the same source graph
# using existing parent convention; strict32 compile ABI gate separately.
# All legacy CHAR executables need char_quickslot.cpp if char_item.cpp calls its
# methods; update shared source lists, not just this one target. Full DBManager
# real lifecycle introduces real SQL dependencies for its users only.

locale_combat_data_regression$(EXEEXT): locale_combat_data_regression.cpp locale_combat_golden.h ../../game/src/constants.cpp ../../game/src/constants.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) locale_combat_data_regression.cpp ../../game/src/constants.cpp $(LDFLAGS) -o $@

# Compile COMPLETE real locale.cpp through an external-stdio observation TU.
# It replaces that source exactly once; no application function is stubbed.
LOCALE_FOUNDATION_SOURCES = $(filter-out ../../game/src/locale.cpp,$(TARGET_WALKING_SOURCES)) locale_foundation_locale_impl.cpp
LOCALE_FOUNDATION_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) locale_foundation_desc_output_syscalls.o
locale_foundation_regression$(EXEEXT): locale_foundation_regression.cpp locale_foundation_locale_impl.cpp $(LOCALE_FOUNDATION_SOURCES) $(LOCALE_FOUNDATION_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/locale_service.h ../../game/src/config.h ../../game/src/constants.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long locale_foundation_regression.cpp $(LOCALE_FOUNDATION_SOURCES) $(LOCALE_FOUNDATION_OBJECTS) $(LDFLAGS) -o $@

locale_foundation_desc_output_syscalls.o: locale_foundation_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c locale_foundation_desc_output_syscalls.c -o $@
clean: clean-locale-foundation-observer
clean-locale-foundation-observer:
	$(RM) locale_foundation_desc_output_syscalls.o

# Copy staged test files to reconstruction/tests; compile actual libthecore main.c
# with per-function/data sections so unrelated uncalled startup APIs are removed.
# This is NOT a new/thecore_shutdown replacement: original source store and real
# shutdowned global are retained. The full main.c object is compiled unchanged.
item_id_core.o: ../../libthecore/src/main.c ../../libthecore/include/main.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) -ffunction-sections -fdata-sections -c ../../libthecore/src/main.c -o item_id_core_full.o
	C:/Strawberry/c/bin/objcopy.exe --remove-section=.text$$thecore_init --remove-section=.pdata$$thecore_init --remove-section=.xdata$$thecore_init --remove-section=.text$$thecore_idle --remove-section=.pdata$$thecore_idle --remove-section=.xdata$$thecore_idle item_id_core_full.o $@
item_id_syscalls.o: item_id_syscalls.c desc_output_syscalls.c
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c item_id_syscalls.c -o $@
# item_id_desc_fixture.cpp is existing full desc_fixture.cpp with ONLY its fake
# thecore_heart/thecore_pulse definitions removed. All other fixture code intact.
# Actual main.c now owns those symbols. No descriptors/CHAR are instantiated by
# this test; ID tests use full actual ITEM_MANAGER constructor/destructor.
ITEM_ID_SOURCES = $(filter-out desc_fixture.cpp,$(TARGET_WALKING_SOURCES)) item_id_desc_fixture.cpp
ITEM_ID_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) item_id_syscalls.o item_id_core.o
item_id_regression$(EXEEXT): item_id_regression.cpp $(ITEM_ID_SOURCES) $(ITEM_ID_OBJECTS) ../../game/src/item_manager.h ../../game/src/config.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_id_regression.cpp $(ITEM_ID_SOURCES) $(ITEM_ID_OBJECTS) $(LDFLAGS) -Wl,--gc-sections -o $@
# Add to RUNTIME_TESTS/check and clean object names. Parent should mirror O2 flags
# for this recipe. Do not compile with NDEBUG: assertions are the test checks.
# check-abi addition (strict target ABI and public nonconst signatures):
# $(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only item_id_abi.cpp
# Optional independent child death invocation: ./item_id_regression --assert-zero
# Must terminate nonzero/abort; exit99 means assert disabled and is a failure.
# Host assertion diagnostics differ from original FreeBSD file/line/ABI; do not
# compare diagnostic text or force external assertion function to return.

# Closed old horse/helper integration, original source placements.
HORSE_METHOD_SOURCES = $(filter-out character_fixture.cpp ../../game/src/char_horse.cpp ../../game/src/guild.cpp guild_lifecycle_fixture.cpp,$(TARGET_WALKING_SOURCES)) horse_methods_character_fixture.cpp ../../game/src/char_horse.cpp ../../game/src/guild.cpp guild_lifecycle_fixture.cpp
HORSE_METHOD_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o

horse_methods_regression$(EXEEXT): horse_methods_regression.cpp $(HORSE_METHOD_SOURCES) $(HORSE_METHOD_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/packet.h ../../game/src/guild.h ../../game/src/horse-summon-detail.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long horse_methods_regression.cpp $(HORSE_METHOD_SOURCES) $(HORSE_METHOD_OBJECTS) $(LDFLAGS) -o $@


# Add source fixture + ABI gate under reconstruction/tests. Uses CURRENT parent's
# full DB/CHAR/item/SQL/descriptor graph; no new app mocks or SQL calls.
quickslot_chain_regression$(EXEEXT): quickslot_chain_regression.cpp $(AUTOGIVE_CLOSED_SOURCES) $(AUTOGIVE_CLOSED_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quickslot_chain_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp $(AUTOGIVE_CLOSED_SOURCES) $(AUTOGIVE_CLOSED_OBJECTS) $(LDFLAGS) -o $@
# Add runtime target/check line; O2 same graph with parent flags.
# Strict32 public nonconst signature gate:
# $(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quickslot_chain_abi.cpp
# Parentapply only adds two methods to EXISTING char_quickslot.cpp, already in
# shared source graph. No new production translation unit/link closure required.
# Do NOT integrate SetCount.cpp.candidate until full Unequip/removal/destruction
# behavior closes. No partial zero-owner paths and no substitutes.

# After complete class/record/helpers/validators integration.
.PHONY: check-banword-locale check-banword-locale-abi clean-banword-locale-observers
check-abi: check-banword-locale-abi
check-banword-locale-abi: banword_locale_abi.cpp banword_locale_api_checks.h ../../game/src/banword.h ../../common/tables.h ../../game/src/desc.h ../../game/src/cmd_gm.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -D_USE_32BIT_TIME_T -Wno-invalid-offsetof -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only banword_locale_abi.cpp
BANWORD_LOCALE_SOURCES = $(filter-out ../../game/src/banword.cpp ../../game/src/cmd_gm.cpp,$(TARGET_WALKING_SOURCES)) ../../game/src/banword.cpp ../../game/src/cmd_gm.cpp
BANWORD_LOCALE_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) banword_locale_desc_output_syscalls.o
banword_locale_regression$(EXEEXT): banword_locale_regression.cpp banword_locale_api_checks.h $(BANWORD_LOCALE_SOURCES) $(BANWORD_LOCALE_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/banword.h ../../game/src/locale_service.h ../../game/src/desc.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long banword_locale_regression.cpp $(BANWORD_LOCALE_SOURCES) $(BANWORD_LOCALE_OBJECTS) $(LDFLAGS) -o $@
banword_locale_desc_output_syscalls.o: banword_locale_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c banword_locale_desc_output_syscalls.c -o $@
clean: clean-banword-locale-observers
clean-banword-locale-observers:
	$(RM) banword_locale_desc_output_syscalls.o

# Canonical runtime-list/maincheck insertion is performed by prepare_apply.py.
.PHONY: check-locale-transfer check-locale-transfer-abi clean-locale-transfer-observers
check-locale-transfer: locale_transfer_regression$(EXEEXT)
	./locale_transfer_regression$(EXEEXT)
check-abi: check-locale-transfer-abi
check-locale-transfer-abi: locale_transfer_abi.cpp ../../game/src/locale_service.h ../../game/src/constants.h ../../game/src/config.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only locale_transfer_abi.cpp

# Actual constants.cpp only is compiler-instrumented; no production flag/API changes.
locale_transfer_constants_instrumented.o: ../../game/src/constants.cpp ../../game/src/constants.h ../../game/src/char.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fno-lto -finstrument-functions -c ../../game/src/constants.cpp -o $@
LOCALE_TRANSFER_SOURCES = $(filter-out ../../game/src/constants.cpp,$(TARGET_WALKING_SOURCES))
LOCALE_TRANSFER_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) locale_transfer_desc_output_syscalls.o locale_transfer_constants_instrumented.o
locale_transfer_regression$(EXEEXT): locale_transfer_regression.cpp $(LOCALE_TRANSFER_SOURCES) $(LOCALE_TRANSFER_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/locale_service.cpp ../../game/src/locale_service.h ../../game/src/constants.h $(CHILD_EXIT_PLATFORM_OBJECTS)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long locale_transfer_regression.cpp $(LOCALE_TRANSFER_SOURCES) $(LOCALE_TRANSFER_OBJECTS) $(CHILD_EXIT_PLATFORM_OBJECTS) $(CHILD_EXIT_PLATFORM_LDFLAGS) $(LDFLAGS) -o $@
locale_transfer_desc_output_syscalls.o: locale_transfer_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c locale_transfer_desc_output_syscalls.c -o $@
clean: clean-locale-transfer-observers
clean-locale-transfer-observers:
	$(RM) locale_transfer_desc_output_syscalls.o locale_transfer_constants_instrumented.o

# BEGIN bounded skill pointer lookup integration
# Pointer-only lookup suite: whole manager data/API + actual ctor/Get source.
# Explicit TEST-ONLY destructor ownership boundary asserts empty map. No fake
# CSkillProto/CPoly constructors/destructors/factory/points. Do not put fixture
# destructor in production or advertise prototype lifecycle/runtime proof.
skill_lookup_regression$(EXEEXT): skill_lookup_regression.cpp poly_unused_random.c ../../game/src/skill.cpp ../../libpoly/Poly.cc ../../libpoly/SymTable.cc ../../game/src/skill.h
	$(CXX) -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -O2 -flto skill_lookup_regression.cpp poly_unused_random.c ../../game/src/skill.cpp ../../libpoly/Poly.cc ../../libpoly/SymTable.cc $(LDFLAGS) -o $@
# Add runtime/check + normal/O2 parent conventions.
# Strict32 API gate:
# $(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only skill_lookup_abi.cpp
# No original old STL sizeof16 manager assertion against current host STL.


.PHONY: clean-skill-lookup
clean: clean-skill-lookup
clean-skill-lookup:
	$(RM) skill_lookup_regression$(EXEEXT)
# END bounded skill pointer lookup integration

item_attribute_regression$(EXEEXT): item_attribute_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) item_attribute_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES) 
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_attribute_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) item_attribute_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

item_attribute_desc_output_syscalls.o: item_attribute_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c item_attribute_desc_output_syscalls.c -o $@

# BEGIN actual poly owning lifecycle closure (no parser/Eval implementation)
poly_owning_lifecycle_regression$(EXEEXT): poly_owning_lifecycle_regression.cpp poly_unused_random.c ../../game/src/skill.cpp ../../game/src/skill.h ../../libpoly/Poly.cc ../../libpoly/Poly.h ../../libpoly/SymTable.cc ../../libpoly/SymTable.h ../../libpoly/Constants.h
	$(CXX) -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -O2 -flto poly_owning_lifecycle_regression.cpp poly_unused_random.c ../../game/src/skill.cpp ../../libpoly/Poly.cc ../../libpoly/SymTable.cc $(LDFLAGS) -o $@
.PHONY: clean-poly-owning
clean: clean-poly-owning
clean-poly-owning:
	$(RM) poly_owning_lifecycle_regression$(EXEEXT)
# END actual poly owning lifecycle closure

# Canonical first RUNTIME_TESTS and maincheck insertion are done by guarded preparation.
.PHONY: check-locale-default check-locale-default-abi
check-locale-default: locale_default_regression$(EXEEXT)
	./locale_default_regression$(EXEEXT)
check-abi: check-locale-default-abi
check-locale-default-abi: locale_default_abi.cpp ../../game/src/locale_service.cpp ../../game/src/locale_service.h ../../game/src/config.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -D_USE_32BIT_TIME_T -fsyntax-only locale_default_abi.cpp

# Test includes COMPLETE actual locale_service.cpp to call ORIGINAL file-static body.
# Filter only that separately compiled source; actual config globals come from
# item_socket_config.o's complete config.cpp inclusion (unrelated4names renamed).
LOCALE_DEFAULT_SOURCES = $(filter-out ../../game/src/locale_service.cpp,$(TARGET_WALKING_SOURCES))
LOCALE_DEFAULT_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o
locale_default_regression$(EXEEXT): locale_default_regression.cpp $(LOCALE_DEFAULT_SOURCES) $(LOCALE_DEFAULT_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/locale_service.cpp ../../game/src/locale_service.h ../../game/src/config.cpp ../../game/src/config.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long locale_default_regression.cpp $(LOCALE_DEFAULT_SOURCES) $(LOCALE_DEFAULT_OBJECTS) $(LDFLAGS) -o $@

item_addon_regression$(EXEEXT): item_addon_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) item_addon_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES) ../../game/src/utils.cpp
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_addon_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) item_addon_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

item_addon_desc_output_syscalls.o: item_addon_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c item_addon_desc_output_syscalls.c -o $@

# TEST ONLY exact COFF selection from complete actual utils.c (no objcopy retarget).
RNG_PROJECTION_PYTHON ?= python
desc_rng_full.o: ../../libthecore/src/utils.c ../../libthecore/include/utils.h ../../libthecore/include/memcpy.h posix_shim/sys/time.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) -fno-lto -ffunction-sections -fdata-sections -c ../../libthecore/src/utils.c -o $@
desc_rng_only.o: desc_rng_full.o project_rng.py
	$(RNG_PROJECTION_PYTHON) project_rng.py desc_rng_full.o $@ desc_rng_projection.proof.json --source ../../libthecore/src/utils.c
desc_rng_os_boundary.o: utils_link_stubs.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -fno-lto -UNDEBUG -c utils_link_stubs.c -o $@
# Standalone projection-call integrity gate; deliberately outside the 88-test registry.
rng_projection_smoke$(EXEEXT): rng_projection_smoke.c desc_rng_only.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -fno-lto -UNDEBUG rng_projection_smoke.c desc_rng_only.o $(LDFLAGS) -o $@
.PHONY: clean-desc-rng-projection
clean: clean-desc-rng-projection
clean-desc-rng-projection:
	$(RM) desc_rng_full.o desc_rng_only.o desc_rng_os_boundary.o desc_rng_projection.proof.json rng_projection_smoke$(EXEEXT)

# BEGIN complete original CPoly parser/evaluator
poly_parser_eval_regression$(EXEEXT): poly_parser_eval_regression.cpp poly_parser_eval_impl.cpp ../../libpoly/Poly.cc ../../libpoly/Poly.h ../../libpoly/SymTable.cc ../../libpoly/Constants.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) poly_parser_eval_regression.cpp poly_parser_eval_impl.cpp ../../libpoly/SymTable.cc $(LDFLAGS) -o $@
.PHONY: clean-poly-parser-eval
clean: clean-poly-parser-eval
clean-poly-parser-eval:
	$(RM) poly_parser_eval_regression$(EXEEXT)
# END complete original CPoly parser/evaluator

# Same-TU COMPLETE actual locale TU preserves original static linkage without a new API.
LOCALE_REGIONS_SOURCES = $(filter-out ../../game/src/locale_service.cpp,$(TARGET_WALKING_SOURCES))
LOCALE_REGIONS_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) locale_regions_desc_output_syscalls.o
locale_regions_regression$(EXEEXT): locale_regions_regression.cpp $(LOCALE_REGIONS_SOURCES) $(LOCALE_REGIONS_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/locale_service.cpp ../../game/src/locale_service.h ../../game/src/config.cpp ../../game/src/config.h $(CHILD_EXIT_PLATFORM_OBJECTS)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long locale_regions_regression.cpp $(LOCALE_REGIONS_SOURCES) $(LOCALE_REGIONS_OBJECTS) $(CHILD_EXIT_PLATFORM_OBJECTS) $(CHILD_EXIT_PLATFORM_LDFLAGS) $(LDFLAGS) -o $@
locale_regions_desc_output_syscalls.o: locale_regions_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c locale_regions_desc_output_syscalls.c -o $@
.PHONY: check-locale-regions check-locale-regions-abi clean-locale-regions
check-locale-regions: locale_regions_regression$(EXEEXT)
	./locale_regions_regression$(EXEEXT)
check-abi: check-locale-regions-abi
check-locale-regions-abi: locale_regions_abi.cpp ../../game/src/locale_service.cpp ../../game/src/locale_service.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -D_USE_32BIT_TIME_T -fsyntax-only locale_regions_abi.cpp
clean: clean-locale-regions
clean-locale-regions:
	$(RM) locale_regions_regression$(EXEEXT) locale_regions_desc_output_syscalls.o

blend_item_regression$(EXEEXT): blend_item_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) blend_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES) ../../game/src/utils.cpp ../../game/src/blend_item.cpp ../../game/src/blend_item.h blend_table_golden.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long blend_item_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) blend_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

blend_desc_output_syscalls.o: blend_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c blend_desc_output_syscalls.c -o $@

# Windows TEST HOST only: retain the real CRT DLL until its FLS callbacks finish.
child_exit_crt_lifetime.o: child_exit_crt_lifetime.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c child_exit_crt_lifetime.c -o $@
.PHONY: clean-child-exit-platform
clean: clean-child-exit-platform
clean-child-exit-platform:
	$(RM) child_exit_crt_lifetime.o

# BEGIN complete skill factory (actual parser and SendLog full graph)
SKILL_FACTORY_SOURCES = $(filter-out ../../game/src/skill.cpp ../../libpoly/Poly.cc ../../libpoly/SymTable.cc,$(BANWORD_LOCALE_SOURCES)) ../../libpoly/Poly.cc ../../libpoly/SymTable.cc skill_factory_impl.cpp
SKILL_FACTORY_OBJECTS = $(filter-out banword_locale_desc_output_syscalls.o,$(BANWORD_LOCALE_OBJECTS)) skill_factory_desc_output_syscalls.o
skill_factory_desc_output_syscalls.o: skill_factory_desc_output_syscalls.c desc_output_syscalls.c
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c skill_factory_desc_output_syscalls.c -o $@
skill_factory_regression$(EXEEXT): skill_factory_regression.cpp $(SKILL_FACTORY_SOURCES) $(SKILL_FACTORY_OBJECTS) ../../game/src/skill.cpp ../../common/tables.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long skill_factory_regression.cpp $(SKILL_FACTORY_SOURCES) $(SKILL_FACTORY_OBJECTS) $(LDFLAGS) -o $@
.PHONY: clean-skill-factory
clean: clean-skill-factory
clean-skill-factory:
	$(RM) skill_factory_regression$(EXEEXT) skill_factory_desc_output_syscalls.o
# END complete skill factory

weighted_attribute_regression$(EXEEXT): weighted_attribute_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) weighted_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES) 
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long weighted_attribute_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) weighted_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

weighted_desc_output_syscalls.o: weighted_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c weighted_desc_output_syscalls.c -o $@

attribute_tail_regression$(EXEEXT): attribute_tail_regression.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) attribute_tail_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES) 
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long attribute_tail_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_addon.cpp ../../game/src/item_attribute.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) attribute_tail_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

attribute_tail_desc_output_syscalls.o: attribute_tail_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c attribute_tail_desc_output_syscalls.c -o $@

# Complete actual current graph; reuse existing explicit unrelated lifecycle/dispatch fixtures.
AFFECT_PACKET_LEAF_SOURCES = $(filter-out ../../game/src/char_affect.cpp,$(TARGET_WALKING_SOURCES)) ../../game/src/char_affect.cpp
AFFECT_PACKET_LEAF_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o
affect_packet_leaves_regression$(EXEEXT): affect_packet_leaves_regression.cpp $(AFFECT_PACKET_LEAF_SOURCES) $(AFFECT_PACKET_LEAF_OBJECTS) $(DESC_TEST_HEADERS)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long affect_packet_leaves_regression.cpp $(AFFECT_PACKET_LEAF_SOURCES) $(AFFECT_PACKET_LEAF_OBJECTS) $(LDFLAGS) -o $@
.PHONY: check-affect-packet-leaves check-affect-packet-leaves-abi clean-affect-packet-leaves
check-affect-packet-leaves: affect_packet_leaves_regression$(EXEEXT)
	./affect_packet_leaves_regression$(EXEEXT)
check-abi: check-affect-packet-leaves-abi
check-affect-packet-leaves-abi: affect_packet_leaves_abi.cpp ../../game/src/packet.h ../../common/tables.h ../../game/src/char.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -m32 -fsyntax-only affect_packet_leaves_abi.cpp
clean: clean-affect-packet-leaves
clean-affect-packet-leaves:
	$(RM) affect_packet_leaves_regression$(EXEEXT)

quest_pc_lookup_regression$(EXEEXT): quest_pc_lookup_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_pc_lookup_desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_pc_lookup_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_pc_lookup_desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@

quest_pc_lookup_desc_output_syscalls.o: quest_pc_lookup_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c quest_pc_lookup_desc_output_syscalls.c -o $@

# Only complete closed original bodies. Full destructor/Remove/MoveItem are declarations until actual stat/item SCC closes.
SAFEBOX_CLOSED_SOURCES = $(TARGET_WALKING_SOURCES) ../../game/src/safebox.cpp ../../libgame/src/grid.cc
SAFEBOX_CLOSED_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) safebox_desc_output_syscalls.o
safebox_closed_regression$(EXEEXT): safebox_closed_regression.cpp $(SAFEBOX_CLOSED_SOURCES) $(SAFEBOX_CLOSED_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/safebox.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long safebox_closed_regression.cpp $(SAFEBOX_CLOSED_SOURCES) $(SAFEBOX_CLOSED_OBJECTS) $(LDFLAGS) -o $@
safebox_desc_output_syscalls.o: safebox_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c safebox_desc_output_syscalls.c -o $@
.PHONY: check-safebox-closed check-safebox-closed-abi clean-safebox-closed
check-safebox-closed: safebox_closed_regression$(EXEEXT)
	./safebox_closed_regression$(EXEEXT)
check-abi: check-safebox-closed-abi
check-safebox-closed-abi: safebox_closed_abi.cpp ../../game/src/safebox.h ../../common/tables.h ../../game/src/char.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -Wno-invalid-offsetof -m32 -fsyntax-only safebox_closed_abi.cpp
clean: clean-safebox-closed
clean-safebox-closed:
	$(RM) safebox_closed_regression$(EXEEXT) safebox_desc_output_syscalls.o


pc_flags_timers_regression$(EXEEXT): pc_flags_timers_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) pc_flags_timers_desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long pc_flags_timers_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) pc_flags_timers_desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@

pc_flags_timers_desc_output_syscalls.o: pc_flags_timers_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c pc_flags_timers_desc_output_syscalls.c -o $@

pc_state_helpers_regression$(EXEEXT): pc_state_helpers_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/questpc.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long pc_state_helpers_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@

quest_manager_indices_regression$(EXEEXT): quest_manager_indices_regression.cpp $(filter-out desc_fixture.cpp,$(TARGET_WALKING_SOURCES)) quest_manager_indices_desc_fixture.cpp $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_manager_indices_desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/questmanager.h desc_fixture.cpp
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_manager_indices_regression.cpp $(filter-out desc_fixture.cpp,$(TARGET_WALKING_SOURCES)) quest_manager_indices_desc_fixture.cpp $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_manager_indices_desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@

quest_manager_indices_desc_output_syscalls.o: quest_manager_indices_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c quest_manager_indices_desc_output_syscalls.c -o $@

# Complete PC storage methods with actual manager queries, flags and real event registry/scheduler.
PC_QUEST_STORAGE_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) pc_quest_storage_desc_output_syscalls.o
pc_quest_storage_regression$(EXEEXT): pc_quest_storage_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_QUEST_STORAGE_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/questpc.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long pc_quest_storage_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_QUEST_STORAGE_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@
pc_quest_storage_desc_output_syscalls.o: pc_quest_storage_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c pc_quest_storage_desc_output_syscalls.c -o $@
.PHONY: check-pc-quest-storage check-pc-quest-storage-abi clean-pc-quest-storage
check-pc-quest-storage: pc_quest_storage_regression$(EXEEXT)
	./pc_quest_storage_regression$(EXEEXT)
check-abi: check-pc-quest-storage-abi
check-pc-quest-storage-abi: pc_quest_storage_abi.cpp ../../game/src/questpc.h ../../game/src/questmanager.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -Wno-invalid-offsetof -fsyntax-only pc_quest_storage_abi.cpp
clean: clean-pc-quest-storage
clean-pc-quest-storage:
	$(RM) pc_quest_storage_regression$(EXEEXT) pc_quest_storage_desc_output_syscalls.o

quest_registration_regression$(EXEEXT): quest_registration_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_registration_desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/questmanager.h ../../game/src/config.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_registration_regression.cpp $(TARGET_WALKING_SOURCES) $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_registration_desc_output_syscalls.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@

quest_registration_desc_output_syscalls.o: quest_registration_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c quest_registration_desc_output_syscalls.c -o $@

# Complete original quest-info methods; use actual already-static BUFFER_MANAGER, real PC/events/network.
PC_QUEST_INFO_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) pc_quest_info_desc_output_syscalls.o
pc_quest_info_regression$(EXEEXT): pc_quest_info_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_QUEST_INFO_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/questpc.h ../../game/src/packet.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long pc_quest_info_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_QUEST_INFO_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@
pc_quest_info_desc_output_syscalls.o: pc_quest_info_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c pc_quest_info_desc_output_syscalls.c -o $@
.PHONY: check-pc-quest-info check-pc-quest-info-abi clean-pc-quest-info
check-pc-quest-info: pc_quest_info_regression$(EXEEXT)
	./pc_quest_info_regression$(EXEEXT)
check-abi: check-pc-quest-info-abi
check-pc-quest-info-abi: pc_quest_info_abi.cpp ../../game/src/questmanager.h ../../game/src/questpc.h ../../game/src/packet.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only pc_quest_info_abi.cpp
clean: clean-pc-quest-info
clean-pc-quest-info:
	$(RM) pc_quest_info_regression$(EXEEXT) pc_quest_info_desc_output_syscalls.o

PC_QUEST_SAVE_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) pc_quest_save_desc_output_syscalls.o
pc_quest_save_regression$(EXEEXT): pc_quest_save_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_QUEST_SAVE_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../common/tables.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long pc_quest_save_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_QUEST_SAVE_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@
pc_quest_save_desc_output_syscalls.o: pc_quest_save_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c pc_quest_save_desc_output_syscalls.c -o $@
.PHONY: check-pc-quest-save check-pc-quest-save-abi clean-pc-quest-save
check-pc-quest-save: pc_quest_save_regression$(EXEEXT)
	./pc_quest_save_regression$(EXEEXT)
check-abi: check-pc-quest-save-abi
check-pc-quest-save-abi: pc_quest_save_abi.cpp ../../common/tables.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only pc_quest_save_abi.cpp
clean: clean-pc-quest-save
clean-pc-quest-save:
	$(RM) pc_quest_save_regression$(EXEEXT) pc_quest_save_desc_output_syscalls.o

PC_BUILD_CANCEL_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) pc_build_cancel_desc_output_syscalls.o
pc_build_cancel_regression$(EXEEXT): pc_build_cancel_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_BUILD_CANCEL_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/questpc.h ../../game/src/quest.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long pc_build_cancel_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_BUILD_CANCEL_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@
pc_build_cancel_desc_output_syscalls.o: pc_build_cancel_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c pc_build_cancel_desc_output_syscalls.c -o $@
.PHONY: check-pc-build-cancel check-pc-build-cancel-abi clean-pc-build-cancel
check-pc-build-cancel: pc_build_cancel_regression$(EXEEXT)
	./pc_build_cancel_regression$(EXEEXT)
check-abi: check-pc-build-cancel-abi
check-pc-build-cancel-abi: pc_build_cancel_abi.cpp ../../game/src/questpc.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only pc_build_cancel_abi.cpp
clean: clean-pc-build-cancel
clean-pc-build-cancel:
	$(RM) pc_build_cancel_regression$(EXEEXT) pc_build_cancel_desc_output_syscalls.o

PC_REWARD_PRODUCERS_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) pc_reward_producers_desc_output_syscalls.o
pc_reward_producers_regression$(EXEEXT): pc_reward_producers_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_REWARD_PRODUCERS_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/questpc.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long pc_reward_producers_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_REWARD_PRODUCERS_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@
pc_reward_producers_desc_output_syscalls.o: pc_reward_producers_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c pc_reward_producers_desc_output_syscalls.c -o $@
.PHONY: check-pc-reward-producers check-pc-reward-producers-abi clean-pc-reward-producers
check-pc-reward-producers: pc_reward_producers_regression$(EXEEXT)
	./pc_reward_producers_regression$(EXEEXT)
check-abi: check-pc-reward-producers-abi
check-pc-reward-producers-abi: pc_reward_producers_abi.cpp ../../game/src/questpc.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only pc_reward_producers_abi.cpp
clean: clean-pc-reward-producers
clean-pc-reward-producers:
	$(RM) pc_reward_producers_regression$(EXEEXT) pc_reward_producers_desc_output_syscalls.o


# Complete supplied Lua5.0.3 SDK C source dependency, not recovered original5.0 VM.
LUA_SDK_ROOT = ../../liblua
LUA_SDK_CORE_NAMES = lapi lcode ldebug ldo ldump lfunc lgc llex lmem lobject lopcodes lparser lstate lstring ltable ltests ltm lundump lvm lzio
LUA_SDK_AUX_NAMES = lauxlib lbaselib ldblib liolib lmathlib ltablib lstrlib loadlib
LUA_SDK_CORE_OBJECTS = $(addprefix $(LUA_SDK_ROOT)/src/mcncc_,$(addsuffix .o,$(LUA_SDK_CORE_NAMES)))
LUA_SDK_AUX_OBJECTS = $(addprefix $(LUA_SDK_ROOT)/src/lib/mcncc_,$(addsuffix .o,$(LUA_SDK_AUX_NAMES)))
LUA_SDK_CORE_LIBRARY = $(LUA_SDK_ROOT)/liblua_mcncc.a
LUA_SDK_AUX_LIBRARY = $(LUA_SDK_ROOT)/liblualib_mcncc.a
LUA_SDK_LIBRARIES = $(LUA_SDK_AUX_LIBRARY) $(LUA_SDK_CORE_LIBRARY)
LUA_SDK_HEADERS = $(wildcard $(LUA_SDK_ROOT)/include/*.h $(LUA_SDK_ROOT)/src/*.h)
LUA_SDK_AR ?= $(shell $(CC) -print-prog-name=ar)
$(LUA_SDK_ROOT)/src/mcncc_%.o: $(LUA_SDK_ROOT)/src/%.c $(LUA_SDK_HEADERS)
	$(CC) -I$(LUA_SDK_ROOT)/include -I$(LUA_SDK_ROOT)/src $(CPPFLAGS) $(CFLAGS) -c $< -o $@
$(LUA_SDK_ROOT)/src/lib/mcncc_%.o: $(LUA_SDK_ROOT)/src/lib/%.c $(LUA_SDK_HEADERS)
	$(CC) -I$(LUA_SDK_ROOT)/include -I$(LUA_SDK_ROOT)/src $(CPPFLAGS) $(CFLAGS) -c $< -o $@
$(LUA_SDK_CORE_LIBRARY): $(LUA_SDK_CORE_OBJECTS)
	$(LUA_SDK_AR) rcs $@ $(LUA_SDK_CORE_OBJECTS)
$(LUA_SDK_AUX_LIBRARY): $(LUA_SDK_AUX_OBJECTS)
	$(LUA_SDK_AR) rcs $@ $(LUA_SDK_AUX_OBJECTS)
# Every actualmanager source links real Lua APIs; archive at end of recipes, with explicit prerequisites.
override LDFLAGS += $(LUA_SDK_LIBRARIES) -lm
$(addsuffix $(EXEEXT),$(RUNTIME_TESTS)): $(LUA_SDK_LIBRARIES)
QUEST_LUA_QUERIES_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_lua_queries_desc_output_syscalls.o
quest_lua_queries_regression$(EXEEXT): quest_lua_queries_regression.cpp $(TARGET_WALKING_SOURCES) $(QUEST_LUA_QUERIES_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LUA_SDK_LIBRARIES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_lua_queries_regression.cpp $(TARGET_WALKING_SOURCES) $(QUEST_LUA_QUERIES_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@
quest_lua_queries_desc_output_syscalls.o: quest_lua_queries_desc_output_syscalls.c desc_output_syscalls.c
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c quest_lua_queries_desc_output_syscalls.c -o $@
.PHONY: check-quest-lua-queries check-quest-lua-queries-abi clean-quest-lua-queries
check-quest-lua-queries: quest_lua_queries_regression$(EXEEXT)
	./quest_lua_queries_regression$(EXEEXT)
# Public opaque API target32 only. Separate original internal-layout probe is known incompatible on MinGW.
check-abi: check-quest-lua-queries-abi
check-quest-lua-queries-abi: quest_lua_queries_public_abi.c $(LUA_SDK_HEADERS)
	$(CC) -I$(LUA_SDK_ROOT)/include -I$(LUA_SDK_ROOT)/src $(CPPFLAGS) $(CFLAGS) -m32 -fsyntax-only quest_lua_queries_public_abi.c
clean: clean-quest-lua-queries
clean-quest-lua-queries:
	$(RM) quest_lua_queries_regression$(EXEEXT) quest_lua_queries_desc_output_syscalls.o $(LUA_SDK_CORE_OBJECTS) $(LUA_SDK_AUX_OBJECTS) $(LUA_SDK_LIBRARIES)

QUEST_EVENT_FLAGS_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_event_flags_desc_output_syscalls.o
quest_event_flags_regression$(EXEEXT): quest_event_flags_regression.cpp $(TARGET_WALKING_SOURCES) $(QUEST_EVENT_FLAGS_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../common/tables.h ../../game/src/questmanager.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_event_flags_regression.cpp $(TARGET_WALKING_SOURCES) $(QUEST_EVENT_FLAGS_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@
quest_event_flags_desc_output_syscalls.o: quest_event_flags_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c quest_event_flags_desc_output_syscalls.c -o $@
.PHONY: check-quest-event-flags check-quest-event-flags-abi clean-quest-event-flags
check-quest-event-flags: quest_event_flags_regression$(EXEEXT)
	./quest_event_flags_regression$(EXEEXT)
check-abi: check-quest-event-flags-abi
check-quest-event-flags-abi: quest_event_flags_abi.cpp ../../common/tables.h ../../game/src/questmanager.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_event_flags_abi.cpp
clean: clean-quest-event-flags
clean-quest-event-flags:
	$(RM) quest_event_flags_regression$(EXEEXT) quest_event_flags_desc_output_syscalls.o

PC_STATE_CONSUMERS_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) pc_state_consumers_desc_output_syscalls.o
pc_state_consumers_regression$(EXEEXT): pc_state_consumers_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_STATE_CONSUMERS_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LUA_SDK_LIBRARIES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long pc_state_consumers_regression.cpp $(TARGET_WALKING_SOURCES) $(PC_STATE_CONSUMERS_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@
pc_state_consumers_desc_output_syscalls.o: pc_state_consumers_desc_output_syscalls.c desc_output_syscalls.c
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c pc_state_consumers_desc_output_syscalls.c -o $@
.PHONY: check-pc-state-consumers check-pc-state-consumers-abi clean-pc-state-consumers
check-pc-state-consumers: pc_state_consumers_regression$(EXEEXT)
	./pc_state_consumers_regression$(EXEEXT)
check-abi: check-pc-state-consumers-abi
check-pc-state-consumers-abi: pc_state_consumers_abi.cpp ../../game/src/questpc.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only pc_state_consumers_abi.cpp
clean: clean-pc-state-consumers
clean-pc-state-consumers:
	$(RM) pc_state_consumers_regression$(EXEEXT) pc_state_consumers_desc_output_syscalls.o

QUEST_STATE_LIFECYCLE_SOURCES = $(TARGET_WALKING_SOURCES) ../../game/src/questlua.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp
QUEST_STATE_LIFECYCLE_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_state_lifecycle_desc_output_syscalls.o
quest_state_lifecycle_regression$(EXEEXT): quest_state_lifecycle_regression.cpp $(QUEST_STATE_LIFECYCLE_SOURCES) $(QUEST_STATE_LIFECYCLE_OBJECTS) $(LUA_SDK_LIBRARIES) ../../game/src/questmanager.h ../../game/src/packet.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_state_lifecycle_regression.cpp $(QUEST_STATE_LIFECYCLE_SOURCES) $(QUEST_STATE_LIFECYCLE_OBJECTS) $(LDFLAGS) -o $@
quest_state_lifecycle_desc_output_syscalls.o: quest_state_lifecycle_desc_output_syscalls.c desc_output_syscalls.c
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c quest_state_lifecycle_desc_output_syscalls.c -o $@
.PHONY: check-quest-state-lifecycle check-quest-state-lifecycle-abi clean-quest-state-lifecycle
check-quest-state-lifecycle: quest_state_lifecycle_regression$(EXEEXT)
	./quest_state_lifecycle_regression$(EXEEXT)
check-abi: check-quest-state-lifecycle-abi
check-quest-state-lifecycle-abi: quest_state_lifecycle_abi.cpp ../../game/src/questmanager.h ../../game/src/packet.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_state_lifecycle_abi.cpp
clean: clean-quest-state-lifecycle
clean-quest-state-lifecycle:
	$(RM) quest_state_lifecycle_regression$(EXEEXT) quest_state_lifecycle_desc_output_syscalls.o

QUEST_YIELD_BRIDGES_SOURCES = $(TARGET_WALKING_SOURCES) ../../game/src/questlua.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp
QUEST_YIELD_BRIDGES_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_yield_bridges_desc_output_syscalls.o
quest_yield_bridges_regression$(EXEEXT): quest_yield_bridges_regression.cpp $(QUEST_YIELD_BRIDGES_SOURCES) $(QUEST_YIELD_BRIDGES_OBJECTS) $(LUA_SDK_LIBRARIES) ../../game/src/questmanager.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_yield_bridges_regression.cpp $(QUEST_YIELD_BRIDGES_SOURCES) $(QUEST_YIELD_BRIDGES_OBJECTS) $(LDFLAGS) -o $@
quest_yield_bridges_desc_output_syscalls.o: quest_yield_bridges_desc_output_syscalls.c desc_output_syscalls.c
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c quest_yield_bridges_desc_output_syscalls.c -o $@
.PHONY: check-quest-yield-bridges check-quest-yield-bridges-abi clean-quest-yield-bridges
check-quest-yield-bridges: quest_yield_bridges_regression$(EXEEXT)
	./quest_yield_bridges_regression$(EXEEXT)
check-abi: check-quest-yield-bridges-abi
check-quest-yield-bridges-abi: quest_yield_bridges_abi.cpp ../../game/src/questmanager.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_yield_bridges_abi.cpp
clean: clean-quest-yield-bridges
clean-quest-yield-bridges:
	$(RM) quest_yield_bridges_regression$(EXEEXT) quest_yield_bridges_desc_output_syscalls.o

QUEST_PREDICATE_DIAGNOSTICS_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_predicate_diagnostics_desc_output_syscalls.o
quest_predicate_diagnostics_regression$(EXEEXT): quest_predicate_diagnostics_regression.cpp $(TARGET_WALKING_SOURCES) $(QUEST_PREDICATE_DIAGNOSTICS_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LUA_SDK_LIBRARIES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_predicate_diagnostics_regression.cpp $(TARGET_WALKING_SOURCES) $(QUEST_PREDICATE_DIAGNOSTICS_OBJECTS) ../../game/src/event.cpp ../../game/src/event_queue.cpp $(LDFLAGS) -o $@
quest_predicate_diagnostics_desc_output_syscalls.o: quest_predicate_diagnostics_desc_output_syscalls.c desc_output_syscalls.c
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c quest_predicate_diagnostics_desc_output_syscalls.c -o $@
.PHONY: check-quest-predicate-diagnostics check-quest-predicate-diagnostics-abi clean-quest-predicate-diagnostics
check-quest-predicate-diagnostics: quest_predicate_diagnostics_regression$(EXEEXT)
	./quest_predicate_diagnostics_regression$(EXEEXT)
check-abi: check-quest-predicate-diagnostics-abi
check-quest-predicate-diagnostics-abi: quest_predicate_diagnostics_abi.cpp ../../game/src/questmanager.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_predicate_diagnostics_abi.cpp
clean: clean-quest-predicate-diagnostics
clean-quest-predicate-diagnostics:
	$(RM) quest_predicate_diagnostics_regression$(EXEEXT) quest_predicate_diagnostics_desc_output_syscalls.o


QUEST_CONFIRM_LEAVES_SOURCES = $(TARGET_WALKING_SOURCES) ../../game/src/questlua.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp
QUEST_CONFIRM_LEAVES_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) quest_confirm_leaves_desc_output_syscalls.o
quest_confirm_leaves_regression$(EXEEXT): quest_confirm_leaves_regression.cpp $(QUEST_CONFIRM_LEAVES_SOURCES) $(QUEST_CONFIRM_LEAVES_OBJECTS) $(LUA_SDK_LIBRARIES) ../../game/src/char.h ../../game/src/questmanager.h ../../game/src/packet.h
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long quest_confirm_leaves_regression.cpp $(QUEST_CONFIRM_LEAVES_SOURCES) $(QUEST_CONFIRM_LEAVES_OBJECTS) $(LDFLAGS) -o $@
quest_confirm_leaves_desc_output_syscalls.o: quest_confirm_leaves_desc_output_syscalls.c desc_output_syscalls.c
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -UNDEBUG -c quest_confirm_leaves_desc_output_syscalls.c -o $@
.PHONY: check-quest-confirm-leaves check-quest-confirm-leaves-abi clean-quest-confirm-leaves
check-quest-confirm-leaves: quest_confirm_leaves_regression$(EXEEXT)
	./quest_confirm_leaves_regression$(EXEEXT)
check-abi: check-quest-confirm-leaves-abi
check-quest-confirm-leaves-abi: quest_confirm_leaves_abi.cpp ../../game/src/char.h ../../game/src/questmanager.h ../../game/src/packet.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only quest_confirm_leaves_abi.cpp
clean: clean-quest-confirm-leaves
clean-quest-confirm-leaves:
	$(RM) quest_confirm_leaves_regression$(EXEEXT) quest_confirm_leaves_desc_output_syscalls.o
