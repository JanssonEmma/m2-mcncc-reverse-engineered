#include "crc32.h"
#include "../../libthecore/include/log.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <string>
#include <vector>

namespace {
struct Allocation {
    void* pointer;
    unsigned size;
};

struct ElemSet {
    void remove(void* pointer);
    char filename[257];
    unsigned line;
    unsigned totalSize;
    std::list<Allocation> allocations;
};

// 0811b120
void ElemSet::remove(void* pointer)
{
    for (auto allocation = allocations.begin(); allocation != allocations.end(); ++allocation) {
        if (allocation->pointer != pointer)
            continue;
        totalSize -= allocation->size;
        std::free(allocation->pointer);
        allocations.erase(allocation);
        return;
    }
}

std::map<unsigned, ElemSet> sites;
}

// 0811a2c0
unsigned GetKey(const char* filename, int line)
{
    const auto wireLine = static_cast<std::uint32_t>(line);
    std::string key(filename);
    key.append(reinterpret_cast<const char*>(&wireLine), sizeof(wireLine));
    return get_crc32(key.data(), key.size());
}

// 0811a340
void print_memory_usage()
{
    std::vector<const ElemSet*> sorted;
    sorted.reserve(sites.size());
    for (const auto& entry : sites)
        sorted.push_back(&entry.second);
    std::sort(sorted.begin(), sorted.end(), [](const ElemSet* left, const ElemSet* right) {
        return left->totalSize > right->totalSize;
    });

    sys_log(0, "%-32s %-6s %-6s %s", "Memory Usage", "Line", "Count", "Size");
    for (const ElemSet* site : sorted) {
        sys_log(0, "%-32s %-6d %-6d %d", site->filename, static_cast<int>(site->line),
                static_cast<int>(site->allocations.size()), static_cast<int>(site->totalSize));
    }
}

// 0811a4e0
void* __new(unsigned size, const char* filename, int line)
{
    const unsigned key = GetKey(filename, line);
    auto result = sites.emplace(key, ElemSet{});
    ElemSet& site = result.first->second;
    if (result.second) {
        std::strncpy(site.filename, filename, 256);
        site.filename[256] = '\0';
        site.line = line;
    }

    site.allocations.push_front(Allocation{nullptr, size});
    void* pointer = std::malloc(size);
    site.allocations.front().pointer = pointer;
    site.totalSize += size;
    return pointer;
}

// 0811a630
void __delete(void* pointer, const char* filename, int line)
{
    const auto entry = sites.find(GetKey(filename, line));
    if (entry == sites.end())
        return;

    ElemSet& site = entry->second;
    site.remove(pointer);
}


