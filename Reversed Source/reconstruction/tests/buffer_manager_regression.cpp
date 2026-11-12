// Links the actual manager and actual C buffer implementation.
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>
#include "../../game/src/buffer_manager.h"
#include "../../libthecore/include/memcpy.h"

extern "C" {
void* (*thecore_memcpy)(void*, const void*, size_t) = memcpy;
int MIN(int a, int b) { return a < b ? a : b; }
void sys_log(int level, const char* format, ...) { assert(level == 0 && format); }
void _sys_err(const char* function, int line, const char* format, ...)
{ (void)function; (void)line; (void)format; assert(!"unexpected buffer error"); }
}
int main()
{
    BUFFER_MANAGER* manager = BUFFER_MANAGER::instance_ptr();
    assert(manager && manager == &BUFFER_MANAGER::instance());
    assert(manager == &BUFFER_MANAGER::Instance());
    LPBUFFER first, second;
    {
        TEMP_BUFFER* a = new TEMP_BUFFER;
        TEMP_BUFFER* b = new TEMP_BUFFER;
        first = a->getptr(); second = b->getptr();
        assert(first != second && first->mem_size == 8192 && second->mem_size == 8192);
        a->write("packet", 6);
        assert(a->size() == 6 && !memcmp(a->read_peek(), "packet", 6));
        delete a; delete b;
    }
    {
        TEMP_BUFFER a;
        TEMP_BUFFER b;
        assert(a.getptr() == second && b.getptr() == first); // Cache is LIFO.
        assert(a.size() == 0 && b.size() == 0);
        assert(a.getptr()->read_point == a.getptr()->mem_data);
        std::vector<char> packet(9000, 'X');
        a.write(&packet[0], (int)packet.size());
        assert(a.size() == 9000 && a.getptr()->mem_size == 35192);
        assert(a.getptr() == second && !memcmp(a.read_peek(), &packet[0], packet.size()));
    } // b returned first, a (grown) last.
    {
        TEMP_BUFFER a;
        assert(a.getptr() == second && a.getptr()->mem_size == 35192 && a.size() == 0);
    }
    // A live buffer is outside the manager cache and remains valid across Destroy.
    {
        TEMP_BUFFER live;
        live.write("live", 4);
        manager->Destroy(); manager->Destroy();
        assert(live.size() == 4 && !memcmp(live.read_peek(), "live", 4));
    }
    manager->Destroy();
    {
        // Grow beyond C pool's retention limit. Manager still caches it until Destroy.
        TEMP_BUFFER large;
        std::vector<char> packet(200000, 'L');
        large.write(&packet[0], (int)packet.size());
        first = large.getptr();
        assert(first->mem_size > 131072);
    }
    {
        TEMP_BUFFER large;
        assert(large.getptr() == first && large.size() == 0);
    }
    manager->Destroy();
    {
        TEMP_BUFFER fresh;
        assert(fresh.size() == 0 && fresh.getptr()->mem_size <= 131072);
    }
    // More than one deque node on both 32-bit and 64-bit hosts.
    {
        std::vector<TEMP_BUFFER*> active;
        std::vector<LPBUFFER> pointers;
        for (int i = 0; i < 300; ++i) {
            TEMP_BUFFER* item = new TEMP_BUFFER;
            item->write(&i, sizeof(i));
            active.push_back(item); pointers.push_back(item->getptr());
        }
        for (int i = 0; i < 300; ++i) delete active[i];
        active.clear();
        for (int i = 299; i >= 0; --i) {
            TEMP_BUFFER* item = new TEMP_BUFFER;
            assert(item->getptr() == pointers[i] && item->size() == 0);
            active.push_back(item);
        }
        for (size_t i = 0; i < active.size(); ++i) delete active[i];
    }
    manager->Destroy(); manager->Destroy();
    assert(BUFFER_MANAGER::instance_ptr() == manager);
    puts("buffer manager regression passed: automatic singleton, reuse, growth and cache cleanup");
    return 0;
}
