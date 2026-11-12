# Append to reconstruction/tests/Makefile AFTER helper/data integration.
# RUNTIME_TESTS addition keeps clean correct. The phony dependency runs this
# test without replacing the existing check recipe or relying on parse order.
RUNTIME_TESTS += stat_primitives_regression
.PHONY: check-stat-primitives check-stat-primitives-abi
check: check-stat-primitives
check-stat-primitives: stat_primitives_regression$(EXEEXT)
	./stat_primitives_regression$(EXEEXT)

check-abi: check-stat-primitives-abi
check-stat-primitives-abi: stat_primitives_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only stat_primitives_abi.cpp

# Filter avoids a duplicate char_battle.cpp if parent adds it to the shared graph.
STAT_PRIMITIVES_BATTLE_SOURCE = $(filter-out $(DESC_PACKET_SOURCES),../../game/src/char_battle.cpp)
STAT_PRIMITIVES_SOURCES = $(STAT_PRIMITIVES_BATTLE_SOURCE) ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(ITEM_SOCKET_SOURCES)
STAT_PRIMITIVES_OBJECTS = $(DESC_PACKET_OBJECTS) stat_primitives_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS)

stat_primitives_regression$(EXEEXT): stat_primitives_regression.cpp stat_primitives_desc_output_syscalls.c $(STAT_PRIMITIVES_SOURCES) $(STAT_PRIMITIVES_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/input.h ../../game/src/packet.h ../../common/tables.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long stat_primitives_regression.cpp $(STAT_PRIMITIVES_SOURCES) $(STAT_PRIMITIVES_OBJECTS) $(LDFLAGS) -o $@

stat_primitives_desc_output_syscalls.o: stat_primitives_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c stat_primitives_desc_output_syscalls.c -o $@

clean: clean-stat-primitives-fixture
.PHONY: clean-stat-primitives-fixture
clean-stat-primitives-fixture:
	$(RM) stat_primitives_desc_output_syscalls.o
