# STAGED handoff only. Integrate actual fragments in ORIGINAL source placements first.
# Add HORSE_METHOD_SOURCES/HORSE_METHOD_OBJECTS after existing TARGET_WALKING groups.
HORSE_METHOD_SOURCES = $(filter-out character_fixture.cpp ../../game/src/char_horse.cpp ../../game/src/guild.cpp guild_lifecycle_fixture.cpp,$(TARGET_WALKING_SOURCES)) horse_methods_character_fixture.cpp ../../game/src/char_horse.cpp ../../game/src/guild.cpp guild_lifecycle_fixture.cpp
HORSE_METHOD_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o

horse_methods_regression$(EXEEXT): horse_methods_regression.cpp $(HORSE_METHOD_SOURCES) $(HORSE_METHOD_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/packet.h ../../game/src/guild.h ../../game/src/horse-summon-detail.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long horse_methods_regression.cpp $(HORSE_METHOD_SOURCES) $(HORSE_METHOD_OBJECTS) $(LDFLAGS) -o $@

# Add runtime binary to RUNTIME_TESTS and ./horse_methods_regression$(EXEEXT) to check.
# Add actual C++03 API/fixed packet gates to existing ABI/check recipe:
# $(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-invalid-offsetof -m32 -D_GLIBCXX_USE_CXX11_ABI=0 -fsyntax-only horse_methods_abi.cpp
# Add test artifacts to clean; no full-class modern STL sizeof gate.
#
# This leaf suite does not execute horse creation/Show/SetProto/affect callbacks.
# Existing event fixture is still an explicit unused boundary; do not claim event coverage.
# Do NOT link candidate-horse-summon.cpp/candidate-show-goto.cpp/candidate-spawn-mob.cpp
# or candidate-spatial-lookups.cpp into this focused closed-leaf target.
# Do NOT silently edit global character_fixture.cpp; only this dedicated copy excludes
# original abort definitions of GetMyHorseVnum/SendHorseInfo/ClearHorseInfo.
