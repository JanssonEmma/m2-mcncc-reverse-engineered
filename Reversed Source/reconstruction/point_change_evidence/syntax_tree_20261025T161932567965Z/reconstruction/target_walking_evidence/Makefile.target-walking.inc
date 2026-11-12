# Append after candidate integration; preserves all existing check recipes.
RUNTIME_TESTS += target_walking_regression
.PHONY: check-target-walking check-target-walking-abi clean-target-walking-fixture
check: check-target-walking
check-target-walking: target_walking_regression$(EXEEXT)
	./target_walking_regression$(EXEEXT)
check-abi: check-target-walking-abi
check-target-walking-abi: target_walking_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only target_walking_abi.cpp

TARGET_WALKING_SOURCES = ../../game/src/item.cpp ../../game/src/item_manager.cpp ../../game/src/sectree.cpp ../../libgame/src/attribute.cc desc_fixture.cpp ../../game/src/desc.cpp $(DESC_PACKET_SOURCES) $(ITEM_SOCKET_SOURCES)
TARGET_WALKING_OBJECTS = $(DESC_PACKET_OBJECTS) target_walk_desc_output_syscalls.o desc_fdwatch.o desc_socket.o desc_utils.o buffer_manager_core.o $(ITEM_SOCKET_OBJECTS)
target_walking_regression$(EXEEXT): target_walking_regression.cpp $(TARGET_WALKING_SOURCES) $(TARGET_WALKING_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/char_manager.h ../../game/src/packet.h ../../common/length.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long target_walking_regression.cpp $(TARGET_WALKING_SOURCES) $(TARGET_WALKING_OBJECTS) $(LDFLAGS) -o $@

target_walk_desc_output_syscalls.o: target_walk_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c target_walk_desc_output_syscalls.c -o $@

clean: clean-target-walking-fixture
clean-target-walking-fixture:
	$(RM) target_walk_desc_output_syscalls.o target_walking_abort_*.txt
