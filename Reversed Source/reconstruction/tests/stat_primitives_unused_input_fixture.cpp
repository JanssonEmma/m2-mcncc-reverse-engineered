// TEST ONLY: isolated CHARACTER graphs do not exercise any input processor.
// RaceToJob remains real code in its original input_login.cpp translation unit.
// Full original classes are included; no reduced class/layout or application success mock.
#include "../../game/src/input.h"
#include <cstdlib>
CInputProcessor::~CInputProcessor() { std::abort(); }
bool CInputProcessor::Process(DESC*,const void*,int,int&) { std::abort(); }
int CInputLogin::Analyze(DESC*,BYTE,const char*) { std::abort(); }
// Use ONLY in isolated graphs without real input.cpp or desc_fixture.cpp.
// Never instantiate an input processor in those suites. Any accidental use aborts.
