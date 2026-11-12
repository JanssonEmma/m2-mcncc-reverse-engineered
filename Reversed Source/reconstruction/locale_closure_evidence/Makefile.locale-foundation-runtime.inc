# Append after complete locale foundation integration. Existing data/ABI gates remain.
RUNTIME_TESTS += locale_foundation_regression
.PHONY: check-locale-foundation clean-locale-foundation-observer
check: check-locale-foundation
check-locale-foundation: locale_foundation_regression$(EXEEXT)
	./locale_foundation_regression$(EXEEXT)

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
