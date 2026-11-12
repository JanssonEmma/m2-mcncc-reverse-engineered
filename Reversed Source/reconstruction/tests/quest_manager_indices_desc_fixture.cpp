// TEST ONLY assertion boundary wrapper. All other current descriptor boundaries
// are included unchanged; the shared desc_fixture.cpp itself is never edited.
#define fw_assert_fail quest_indices_base_assert_fail
#include "desc_fixture.cpp"
#undef fw_assert_fail
extern "C" int quest_indices_assert(const char*,const char*,int);
extern "C" void fw_assert_fail(const char* expression,const char* file,int line) {
    if(!quest_indices_assert(expression,file,line)) quest_indices_base_assert_fail(expression,file,line);
}
