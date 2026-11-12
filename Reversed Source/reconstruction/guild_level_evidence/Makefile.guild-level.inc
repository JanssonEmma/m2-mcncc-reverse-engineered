# Append after full candidate source/data/accessor integration. Existing recipes stay intact.
RUNTIME_TESTS += guild_level_regression
.PHONY: check-guild-level check-guild-level-abi clean-guild-level-observer
check: check-guild-level
check-guild-level: guild_level_regression$(EXEEXT)
	./guild_level_regression$(EXEEXT)
check-abi: check-guild-level-abi
check-guild-level-abi: guild_level_abi.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-invalid-offsetof -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only guild_level_abi.cpp

# Full real shared graph; keep the existing full-data guild lifecycle fixture test-only.
GUILD_LEVEL_SOURCES = $(filter-out ../../game/src/guild.cpp guild_lifecycle_fixture.cpp,$(TARGET_WALKING_SOURCES)) ../../game/src/guild.cpp guild_lifecycle_fixture.cpp
GUILD_LEVEL_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) guild_level_desc_output_syscalls.o
guild_level_regression$(EXEEXT): guild_level_regression.cpp $(GUILD_LEVEL_SOURCES) $(GUILD_LEVEL_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/guild.h ../../game/src/char.h ../../game/src/packet.h ../../common/tables.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long guild_level_regression.cpp $(GUILD_LEVEL_SOURCES) $(GUILD_LEVEL_OBJECTS) $(LDFLAGS) -o $@

guild_level_desc_output_syscalls.o: guild_level_desc_output_syscalls.c desc_output_syscalls.c socket_hooks.h fdwatch_shim/assert.h fdwatch_shim/sys/event.h socket_shim/sys/socket.h socket_shim/errno.h
	$(CC) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(CPPFLAGS) $(CFLAGS) -c guild_level_desc_output_syscalls.c -o $@
clean: clean-guild-level-observer
clean-guild-level-observer:
	$(RM) guild_level_desc_output_syscalls.o
