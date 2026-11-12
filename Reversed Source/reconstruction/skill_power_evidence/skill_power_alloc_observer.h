#ifndef MCNCC_SKILL_POWER_ALLOC_OBSERVER_H
#define MCNCC_SKILL_POWER_ALLOC_OBSERVER_H
#include <cstddef>
std::size_t skill_test_allocations();
std::size_t skill_test_deallocations();
bool skill_test_live(const void*);
std::size_t skill_test_size(const void*);
#endif
