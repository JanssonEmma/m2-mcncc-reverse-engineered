# Match the actual compiler output name so existing Windows executables are reusable.
ifeq ($(OS),Windows_NT)
EXEEXT := .exe
else
EXEEXT :=
endif
RUNTIME_TESTS = horse_methods_regression item_id_regression locale_combat_data_regression locale_foundation_regression autogive_closed_regression skill_power_regression item_lookup_regression exp_data_regression exp_next_regression stop_muyeong_regression character_save_regression charlog_regression guild_level_regression polymorph_helpers_regression target_walking_regression level_pk_regression stat_primitives_regression party_dependency_regression stat_data_regression item_accessory_setters_regression locale_parser_regression core_regression event_regression heartbeat_regression vector_regression runtime_regression data_regression logging_regression gost_regression memcpy_regression signal_regression grid_regression attribute_regression tea_regression utils_regression utils_numeric_regression utils_strings_regression buffer_regression buffer_manager_regression socket_regression fdwatch_regression desc_output_unit desc_output_integration desc_packet_integration traffic_regression packet_info_regression packet_tables_regression input_regression game_time_regression desc_input_integration desc_flush_regression locale_regression desc_control_regression character_name_regression desc_manager_regression profiler_regression entity_regression sectree_attribute_regression horse_regression character_manager_regression mob_regression quest_flag_regression character_initialize_regression character_creation_regression item_regression item_persistence_regression character_inventory_regression item_ownership_regression character_stat_regression movement_regression text_loader_regression motion_file_regression motion_build_regression character_update_regression item_audit_regression asyncsql_regression asyncsql_connection_regression asyncsql_worker_regression dev_log_regression item_socket_expiry_regression

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
	$(RM) $(addsuffix $(EXEEXT),$(RUNTIME_TESTS))
	$(RM) character_creation_regression character_initialize_regression quest_flag_regression mob_regression character_manager_regression horse_regression horse_utils.o sectree_attribute_regression entity_regression profiler_regression desc_manager_regression character_name_regression desc_control_regression locale_regression desc_flush_regression desc_input_integration input_pong_test.o input_regression game_time_regression game_time_impl.o packet_tables_regression packet_info_regression traffic_regression desc_packet_integration desc_tea.o core_regression event_regression heartbeat_regression vector_regression runtime_regression data_regression logging_regression gost_regression memcpy_regression signal_regression grid_regression attribute_regression tea_regression utils_regression utils_numeric_regression utils_strings_regression buffer_regression buffer_manager_regression socket_regression fdwatch_regression desc_output_unit desc_output_integration utils_strings_impl.o buffer_manager_core.o fdwatch_test_impl.o desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o

DESC_PACKET_SOURCES = ../../game/src/constants.cpp ../../game/src/char_skill.cpp ../../game/src/char_battle.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp character_fixture.cpp quest_flag_fixture.cpp ../../game/src/horse_rider.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp ../../game/src/FSM.cpp ../../game/src/char_manager.cpp ../../game/src/desc_client.cpp ../../game/src/desc_manager.cpp ../../game/src/input.cpp ../../game/src/input_login.cpp ../../game/src/input_main.cpp ../../game/src/input_auth.cpp ../../game/src/packet_info.cpp ../../game/src/main.cpp ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/TrafficProfiler.cpp ../../game/src/sequence.cpp ../../game/src/buffer_manager.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp
DESC_PACKET_OBJECTS = desc_tea.o movement_duration_impl.o

DESC_TEST_HEADERS = ../../game/src/config.h ../../game/src/packet.h ../../game/src/protocol.h ../../game/src/TrafficProfiler.h ../../game/src/utils.h ../../game/src/desc.h ../../common/tables.h desc_fixture.h ../../libthecore/include/buffer.h ../../libthecore/include/fdwatch.h ../../libthecore/include/socket.h ../../libthecore/include/typedef.h

desc_output_unit$(EXEEXT): desc_output_unit.cpp desc_fixture.cpp character_unused_clock.cpp ../../game/src/desc.cpp $(DESC_TEST_HEADERS) $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_output_unit.cpp desc_fixture.cpp character_unused_clock.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o $(LDFLAGS) -o $@

desc_output_syscalls.o: desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c desc_output_syscalls.c -o $@

desc_fdwatch.o: ../../libthecore/src/fdwatch.c ../../libthecore/include/fdwatch.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h posix_shim/sys/time.h
	$(CC) -Ifdwatch_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c ../../libthecore/src/fdwatch.c -o $@

desc_socket.o: ../../libthecore/src/socket.c ../../libthecore/include/socket.h socket_hooks.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Isocket_shim -Iposix_shim -include socket_hooks.h $(CPPFLAGS) $(CFLAGS) -c ../../libthecore/src/socket.c -o $@

desc_utils.o: ../../libthecore/src/utils.c ../../libthecore/include/utils.h ../../libthecore/include/memcpy.h posix_shim/sys/time.h
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c ../../libthecore/src/utils.c -o $@

desc_output_integration$(EXEEXT): desc_output_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_TEST_HEADERS) $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_output_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(LDFLAGS) -o $@

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

desc_packet_integration$(EXEEXT): desc_packet_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_TEST_HEADERS) $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_packet_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(LDFLAGS) -o $@

traffic_regression$(EXEEXT): traffic_regression.cpp ../../game/src/TrafficProfiler.cpp ../../game/src/TrafficProfiler.h ../../common/singleton.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) traffic_regression.cpp ../../game/src/TrafficProfiler.cpp $(LDFLAGS) -o $@

packet_info_regression$(EXEEXT): packet_info_regression.cpp ../../game/src/packet_info.cpp ../../game/src/packet_info.h ../../libthecore/include/utils.h
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long packet_info_regression.cpp ../../game/src/packet_info.cpp $(LDFLAGS) -o $@

packet_tables_regression$(EXEEXT): packet_tables_regression.cpp ../../game/src/packet_info.cpp ../../game/src/input_udp.cpp ../../game/src/packet_info.h
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long packet_tables_regression.cpp ../../game/src/packet_info.cpp ../../game/src/input_udp.cpp $(LDFLAGS) -o $@

input_regression$(EXEEXT): input_regression.cpp desc_fixture.cpp character_unused_clock.cpp input_pong_test.o ../../game/src/input.h ../../game/src/desc.cpp ../../game/src/packet_info.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long input_regression.cpp desc_fixture.cpp character_unused_clock.cpp input_pong_test.o ../../game/src/desc.cpp $(filter-out ../../game/src/input.cpp ../../game/src/main.cpp,$(DESC_PACKET_SOURCES)) $(DESC_PACKET_OBJECTS) buffer_manager_core.o $(LDFLAGS) -o $@

game_time_impl.o: ../../game/src/utils.cpp ../../game/src/utils.h game_time_hooks.h
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -include game_time_hooks.h -c ../../game/src/utils.cpp -o $@

game_time_regression$(EXEEXT): game_time_regression.cpp game_time_impl.o ../../game/src/utils.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) game_time_regression.cpp game_time_impl.o $(LDFLAGS) -o $@

desc_input_integration$(EXEEXT): desc_input_integration.cpp desc_fixture.cpp ../../game/src/input.cpp ../../game/src/input.h ../../game/src/desc.cpp ../../game/src/packet_info.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_input_integration.cpp desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(LDFLAGS) -o $@

desc_flush_regression$(EXEEXT): desc_flush_regression.cpp desc_fixture.cpp character_unused_clock.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_flush_regression.cpp desc_fixture.cpp character_unused_clock.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) buffer_manager_core.o $(LDFLAGS) -o $@

input_pong_test.o: ../../game/src/input.cpp ../../game/src/input.h pong_hooks.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long -include pong_hooks.h -c ../../game/src/input.cpp -o $@

locale_regression$(EXEEXT): locale_regression.cpp ../../game/src/locale_service.cpp ../../game/src/config.cpp ../../game/src/locale.cpp ../../game/src/locale_service.h ../../game/src/locale.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) locale_regression.cpp ../../game/src/locale_service.cpp ../../game/src/config.cpp ../../game/src/locale.cpp $(LDFLAGS) -o $@

desc_control_regression$(EXEEXT): desc_control_regression.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long desc_control_regression.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(LDFLAGS) -o $@

character_name_regression$(EXEEXT): character_name_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc character_fixture.h ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/char.h ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_name_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp $(LDFLAGS) -o $@

desc_manager_regression$(EXEEXT): desc_manager_regression.cpp ../../game/src/desc_manager.cpp ../../game/src/desc_manager.h ../../common/stl.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) desc_manager_regression.cpp ../../game/src/desc_manager.cpp $(LDFLAGS) -o $@

profiler_regression$(EXEEXT): profiler_regression.cpp ../../game/src/profiler.h ../../common/stl.h
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long profiler_regression.cpp $(LDFLAGS) -o $@

entity_regression$(EXEEXT): entity_regression.cpp character_fixture.cpp ../../game/src/horse_rider.cpp ../../game/src/FSM.cpp ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/sectree.cpp ../../libgame/src/attribute.cc entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp ../../game/src/entity.h ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long entity_regression.cpp character_fixture.cpp ../../game/src/horse_rider.cpp ../../game/src/FSM.cpp ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/sectree.cpp ../../libgame/src/attribute.cc entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp $(LDFLAGS) -o $@

sectree_attribute_regression$(EXEEXT): sectree_attribute_regression.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/sectree.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) sectree_attribute_regression.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc $(LDFLAGS) -o $@

horse_utils.o: ../../libthecore/src/utils.c
	$(CC) -Iposix_shim $(CPPFLAGS) $(CFLAGS) -ffunction-sections -fdata-sections -c ../../libthecore/src/utils.c -o $@

horse_regression$(EXEEXT): horse_regression.cpp ../../game/src/horse_rider.cpp ../../game/src/config.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp horse_utils.o
	$(CXX) -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long horse_regression.cpp ../../game/src/horse_rider.cpp ../../game/src/config.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp horse_utils.o $(LDFLAGS) -Wl,--gc-sections -o $@


mob_regression$(EXEEXT): mob_regression.cpp ../../game/src/mob_manager.cpp ../../game/src/mob_manager.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Iposix_shim mob_regression.cpp ../../game/src/mob_manager.cpp $(LDFLAGS) -o $@

quest_flag_regression$(EXEEXT): quest_flag_fixture.cpp quest_flag_regression.cpp ../../game/src/questmanager.cpp ../../game/src/questmanager.h ../../game/src/questnpc.h ../../game/src/questpc.h ../../game/src/quest.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) quest_flag_fixture.cpp quest_flag_regression.cpp ../../game/src/questmanager.cpp $(LDFLAGS) -o $@

character_initialize_regression$(EXEEXT): character_initialize_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc character_fixture.h ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/char.h ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_initialize_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp $(LDFLAGS) -o $@

character_manager_regression$(EXEEXT): character_manager_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc character_fixture.h ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/char.h ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_manager_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp $(LDFLAGS) -o $@

character_creation_regression$(EXEEXT): character_creation_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc character_fixture.h ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/char.h ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_creation_regression.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp ../../game/src/char_item.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp item_unused_database.cpp ../../game/src/locale.cpp ../../game/src/locale_service.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp $(LDFLAGS) -o $@

item_regression$(EXEEXT): item_regression.cpp item_unused_database.cpp ../../game/src/item_manager.cpp ../../game/src/item.cpp ../../game/src/char_item.cpp ../../game/src/item.h buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES) ../../game/src/locale.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_regression.cpp item_unused_database.cpp ../../game/src/item_manager.cpp ../../game/src/item.cpp ../../game/src/char_item.cpp quest_flag_fixture.cpp entity_packet_fixture.cpp ../../game/src/entity.cpp ../../game/src/entity_view.cpp ../../game/src/party.cpp party_lifecycle_fixture.cpp character_fixture.cpp ../../game/src/mob_manager.cpp ../../game/src/horse_rider.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc ../../game/src/char.cpp ../../game/src/crc32.cpp ../../game/src/questmanager.cpp ../../game/src/char_manager.cpp ../../game/src/buffer_manager.cpp buffer_manager_core.o ../../game/src/FSM.cpp ../../game/src/motion.cpp movement_duration_impl.o ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/locale_service.cpp ../../game/src/locale.cpp $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) ../../game/src/char_battle.cpp ../../game/src/input_login.cpp stat_primitives_unused_input_fixture.cpp ../../game/src/constants.cpp ../../game/src/char_skill.cpp $(LDFLAGS) -o $@

item_persistence_regression$(EXEEXT): item_persistence_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_persistence_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

character_inventory_regression$(EXEEXT): character_inventory_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_inventory_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

item_ownership_regression$(EXEEXT): item_ownership_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_ownership_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

character_stat_regression$(EXEEXT): character_stat_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/char_affect.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_stat_regression.cpp ../../game/src/char_affect.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

movement_regression$(EXEEXT): movement_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long movement_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

movement_duration_impl.o: movement_duration_impl.cpp ../../game/src/utils.cpp ../../game/src/utils.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c movement_duration_impl.cpp -o $@

text_loader_regression$(EXEEXT): text_loader_regression.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/text_file_loader.h ../../common/pool.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long text_loader_regression.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp $(LDFLAGS) -o $@

motion_file_regression$(EXEEXT): motion_file_regression.cpp ../../game/src/motion.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long motion_file_regression.cpp ../../game/src/motion.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp $(LDFLAGS) -o $@

motion_build_regression$(EXEEXT): motion_build_regression.cpp ../../game/src/motion.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long motion_build_regression.cpp ../../game/src/motion.cpp ../../game/src/text_file_loader.cpp ../../game/src/file_loader.cpp ../../game/src/mob_manager.cpp $(LDFLAGS) -o $@


character_update_regression$(EXEEXT): character_update_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES) guild_lifecycle_fixture.cpp
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long character_update_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) guild_lifecycle_fixture.cpp $(LDFLAGS) -o $@

SQL_TEST_FLAGS = -Isql_shim -I../dependencies/mysql-4.1.3/include -include sql_shim/mysql_platform.h
SQL_CONNECTION_FIXTURE = sql_connection_fixture.cpp
ITEM_AUDIT_SOURCES = ../../game/src/log.cpp ../../libsql/AsyncSQL.cpp ../../libsql/Semaphore.cpp ../../game/src/config.cpp ../../game/src/locale_service.cpp ../../game/src/locale.cpp
item_audit_regression$(EXEEXT): item_audit_regression.cpp $(SQL_CONNECTION_FIXTURE) $(ITEM_AUDIT_SOURCES) ../../libsql/AsyncSQL.h ../../libsql/Semaphore.h ../../game/src/log.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SQL_TEST_FLAGS) -Wno-long-long item_audit_regression.cpp $(SQL_CONNECTION_FIXTURE) $(ITEM_AUDIT_SOURCES) $(LDFLAGS) -o $@

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

item_socket_expiry_regression$(EXEEXT): item_socket_expiry_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_socket_expiry_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

ifneq ($(EXEEXT),)
.PHONY: $(RUNTIME_TESTS)
$(RUNTIME_TESTS): %: %$(EXEEXT)
endif

locale_parser_regression$(EXEEXT): locale_parser_regression.cpp ../../game/src/locale.cpp ../../game/src/locale.hpp ../../game/src/locale_service.cpp ../../game/src/config.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) locale_parser_regression.cpp ../../game/src/locale_service.cpp ../../game/src/config.cpp $(LDFLAGS) -o $@

item_accessory_setters_regression$(EXEEXT): item_accessory_setters_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) accessory_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long item_accessory_setters_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) accessory_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

accessory_desc_output_syscalls.o: accessory_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c accessory_desc_output_syscalls.c -o $@

stat_data_regression$(EXEEXT): stat_data_regression.cpp stat_data_golden.h ../../game/src/constants.cpp ../../game/src/constants.h ../../common/length.h ../../game/src/char.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) stat_data_regression.cpp ../../game/src/constants.cpp $(LDFLAGS) -o $@

party_dependency_regression$(EXEEXT): party_dependency_regression.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) party_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o ../../game/src/event.cpp ../../game/src/event_queue.cpp $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -DMCNCC_TEST_REAL_EVENTS -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long party_dependency_regression.cpp ../../game/src/event.cpp ../../game/src/event_queue.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) party_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

party_desc_output_syscalls.o: party_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c party_desc_output_syscalls.c -o $@

# Filter avoids a duplicate char_battle.cpp if parent adds it to the shared graph.
STAT_PRIMITIVES_BATTLE_SOURCE = $(filter-out $(DESC_PACKET_SOURCES),../../game/src/char_battle.cpp)
STAT_PRIMITIVES_SOURCES = $(STAT_PRIMITIVES_BATTLE_SOURCE) ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(ITEM_SOCKET_SOURCES)
STAT_PRIMITIVES_OBJECTS = $(DESC_PACKET_OBJECTS) stat_primitives_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS)

stat_primitives_regression$(EXEEXT): stat_primitives_regression.cpp stat_primitives_desc_output_syscalls.c $(STAT_PRIMITIVES_SOURCES) $(STAT_PRIMITIVES_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/input.h ../../game/src/packet.h ../../common/tables.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long stat_primitives_regression.cpp $(STAT_PRIMITIVES_SOURCES) $(STAT_PRIMITIVES_OBJECTS) $(LDFLAGS) -o $@

stat_primitives_desc_output_syscalls.o: stat_primitives_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c stat_primitives_desc_output_syscalls.c -o $@


level_pk_regression$(EXEEXT): level_pk_regression.cpp guild_lifecycle_fixture.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) level_pk_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS) $(ITEM_SOCKET_SOURCES)
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long level_pk_regression.cpp guild_lifecycle_fixture.cpp ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(DESC_PACKET_OBJECTS) level_pk_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_SOURCES) $(ITEM_SOCKET_OBJECTS) $(LDFLAGS) -o $@

level_pk_desc_output_syscalls.o: level_pk_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c level_pk_desc_output_syscalls.c -o $@

TARGET_WALKING_SOURCES = ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(ITEM_SOCKET_SOURCES)
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
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SQL_TEST_FLAGS) -Wno-long-long charlog_regression.cpp $(SQL_CONNECTION_FIXTURE) $(ITEM_AUDIT_SOURCES) $(LDFLAGS) -o $@

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
AUTOGIVE_CLOSED_SOURCES = $(filter-out $(DESC_PACKET_SOURCES),$(AUTOGIVE_CLOSED_NEW_SOURCES)) ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(ITEM_SOCKET_SOURCES)
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

