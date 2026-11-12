# Independent exact EXP data plus actual inline accessor integration.
RUNTIME_TESTS += exp_data_regression exp_next_regression
.PHONY: check-exp-data check-exp-data-abi
check: check-exp-data
check-exp-data: exp_data_regression$(EXEEXT) exp_next_regression$(EXEEXT)
	./exp_data_regression$(EXEEXT)
	./exp_next_regression$(EXEEXT)
check-abi: check-exp-data-abi
check-exp-data-abi: exp_data_abi.cpp exp_data_type_checks.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only exp_data_abi.cpp

exp_data_regression$(EXEEXT): exp_data_regression.cpp exp_data_type_checks.h exp_data_golden.h ../../game/src/constants.cpp ../../game/src/constants.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) exp_data_regression.cpp ../../game/src/constants.cpp $(LDFLAGS) -o $@

EXP_NEXT_SOURCES = $(TARGET_WALKING_SOURCES)
EXP_NEXT_OBJECTS = $(filter-out target_walk_desc_output_syscalls.o,$(TARGET_WALKING_OBJECTS)) desc_output_syscalls.o
exp_next_regression$(EXEEXT): exp_next_regression.cpp exp_data_type_checks.h exp_data_golden.h $(EXP_NEXT_SOURCES) $(EXP_NEXT_OBJECTS) $(DESC_TEST_HEADERS) ../../game/src/char.h ../../game/src/constants.h
	$(CXX) -Ifdwatch_shim -Isocket_shim -Iposix_shim $(ITEM_SOCKET_FLAGS) $(CPPFLAGS) $(CXXFLAGS) -Wno-long-long exp_next_regression.cpp $(EXP_NEXT_SOURCES) $(EXP_NEXT_OBJECTS) $(LDFLAGS) -o $@
