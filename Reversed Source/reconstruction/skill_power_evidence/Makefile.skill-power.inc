# Integrate after complete constants.h/.cpp class/method candidates.
# Test translation units live in reconstruction/tests.
RUNTIME_TESTS += skill_power_regression
.PHONY: check-skill-power check-skill-power-abi
check: check-skill-power
check-skill-power: skill_power_regression$(EXEEXT)
	./skill_power_regression$(EXEEXT)
check-abi: check-skill-power-abi
check-skill-power-abi: skill_power_abi.cpp skill_power_api_checks.h ../../game/src/constants.h ../../common/singleton.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Wno-invalid-offsetof -m32 -fsyntax-only skill_power_abi.cpp

skill_power_regression$(EXEEXT): skill_power_regression.cpp skill_power_alloc_observer.cpp skill_power_alloc_observer.h skill_power_api_checks.h ../../game/src/constants.cpp ../../game/src/constants.h ../../common/singleton.h ../../common/length.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) skill_power_regression.cpp skill_power_alloc_observer.cpp ../../game/src/constants.cpp $(LDFLAGS) -o $@
