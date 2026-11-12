// Test-only descriptor fixture wrapper: preserve all existing full lifecycle
// boundaries and intercept only the external assertion reporting function.
#define fw_assert_fail helper_base_assert_fail
#include "desc_fixture.cpp"
#undef fw_assert_fail
extern "C" void helper_test_assert(const char*,const char*,int);
extern "C" void fw_assert_fail(const char* condition,const char* file,int line) {
    helper_test_assert(condition,file,line);
    helper_base_assert_fail(condition,file,line);
}
