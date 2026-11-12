# Optional bounded data/API gates after complete candidate integration.
RUNTIME_TESTS += locale_combat_data_regression
.PHONY: check-locale-combat-data check-locale-foundation-abi
check: check-locale-combat-data
check-locale-combat-data: locale_combat_data_regression$(EXEEXT)
	./locale_combat_data_regression$(EXEEXT)
check-abi: check-locale-foundation-abi
check-locale-foundation-abi: locale_foundation_abi.cpp ../../game/src/constants.h ../../game/src/locale_service.h ../../game/src/config.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -m32 -fsyntax-only locale_foundation_abi.cpp
locale_combat_data_regression$(EXEEXT): locale_combat_data_regression.cpp locale_combat_golden.h ../../game/src/constants.cpp ../../game/src/constants.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) locale_combat_data_regression.cpp ../../game/src/constants.cpp $(LDFLAGS) -o $@
