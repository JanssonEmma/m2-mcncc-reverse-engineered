// TEST ONLY: external C++ array allocation boundary, no application method mocks.
#include "skill_power_alloc_observer.h"
#include <new>
#include <cstdlib>
#include <cassert>
struct Entry { void* pointer; std::size_t size; bool live; };
static Entry entries[512];
static std::size_t count=0,freed=0;
void* operator new[](std::size_t bytes) throw(std::bad_alloc)
{
    void* p=std::malloc(bytes?bytes:1);
    if(!p) throw std::bad_alloc();
    assert(count<512);
    entries[count].pointer=p;entries[count].size=bytes;entries[count].live=true;++count;
    return p;
}
void operator delete[](void* p) throw()
{
    if(!p) return;
    for(std::size_t i=count;i!=0;--i)
        if(entries[i-1].pointer==p && entries[i-1].live) {
            entries[i-1].live=false;++freed;std::free(p);return;
        }
    // Every array allocation in this isolated executable passes the boundary above.
    assert(!"unexpected or duplicate delete[]");
}
std::size_t skill_test_allocations() { return count; }
std::size_t skill_test_deallocations() { return freed; }
bool skill_test_live(const void* p) {
    for(std::size_t i=count;i!=0;--i)
        if(entries[i-1].pointer==p) return entries[i-1].live;
    return false;
}
std::size_t skill_test_size(const void* p) {
    for(std::size_t i=count;i!=0;--i)
        if(entries[i-1].pointer==p) return entries[i-1].size;
    assert(!"unobserved allocation");return 0;
}
