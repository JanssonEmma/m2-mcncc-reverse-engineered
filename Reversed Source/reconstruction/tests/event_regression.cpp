// Tests run reconstructed code with controlled heartbeat, logging and fatal hooks.
// They do not execute or establish equivalence against the original server.
#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>
#include "../../game/src/event.h"
#include "../../game/src/event_queue.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/log.h"

static HEART testHeart = {0, {0, 0}, {0, 0}, {0, 0}, 25, 0};
LPHEART thecore_heart = &testHeart;
static int errors;
static int fatals;
static int callbacks;
static int lastValue;
static std::string lastError;
static LPEVENT selfHandle;

extern "C" void sys_log(int, const char*, ...) {}
extern "C" void _sys_err(const char*, int, const char* format, ...)
{
    ++errors;
    lastError = format;
}
void ShutdownOnFatalError() { ++fatals; }
extern void FreeEventInfo(DWORD id); // Observed C++ helper, not public event API.

struct Info
{
    int calls;
    int value;
};

static Info* Allocate(DWORD& id)
{
    void* data = 0;
    id = __AllocEventInfo(sizeof(Info), &data);
    Info* info = static_cast<Info*>(data);
    assert(id != 0 && info != 0);
    assert(info->calls == 0 && info->value == 0); // calloc semantics.
    return info;
}

static long Once(LPEVENT current)
{
    assert(current->is_processing == 1);
    assert(current->q_el == 0 && event_time(current) == 0);
    assert(current->info == event_info(current));
    lastValue = static_cast<Info*>(current->info)->value;
    ++callbacks;
    return 0;
}

static long Repeat(LPEVENT current)
{
    Info* info = static_cast<Info*>(current->info);
    ++callbacks;
    return ++info->calls == 1 ? 5 : 0;
}

static long CancelSelf(LPEVENT current)
{
    const DWORD id = current->dwInfoID;
    event_cancel(&selfHandle);
    assert(selfHandle == 0 && current->is_force_to_end == 1);
    assert(FindEventInfo(id) == current->info); // Freed after callback returns.
    ++callbacks;
    return 3; // Suppressed by force flag, and emits the old EVENT BUG diagnostic.
}

static long ResetWhileRunning(LPEVENT current)
{
    event_reset_time(current, 99);
    assert(current->q_el == 0 && event_count() == 0);
    ++callbacks;
    return 0;
}

int main()
{
    DWORD id;
    Info* info = Allocate(id);
    assert(FindEventInfo(id) == info && FindEventInfo(0) == 0);
    info->value = 17;
    char origin[100];
    std::memset(origin, 'x', sizeof(origin));
    origin[99] = 0;
    LPEVENT current = event_create_ex(Once, id, -8, origin, 77);
    assert(current->info == 0 && current->line == 77);
    assert(std::strlen(current->file) == 63);
    assert(event_time(current) == 1 && event_count() == 1);
    assert(event_process(0) == 0);
    testHeart.pulse = 1;
    assert(event_process(1) == 1 && callbacks == 1 && lastValue == 17);
    assert(FindEventInfo(id) == 0 && event_count() == 0);

    // Finished event objects are reused in LIFO order with state reset.
    LPEVENT previous = current;
    Allocate(id);
    testHeart.pulse = 100;
    current = event_create(Repeat, id, 2);
    assert(current == previous && current->info == 0);
    assert(!current->is_processing && !current->is_force_to_end);
    assert(event_process(101) == 0);
    testHeart.pulse = 110;
    assert(event_process(110) == 1);
    assert(event_time(current) == 5 && !current->is_processing && current->info == 0);
    testHeart.pulse = 115;
    assert(event_process(115) == 1 && FindEventInfo(id) == 0);

    // Cancellation frees info immediately but leaves a queue tombstone.
    Allocate(id);
    testHeart.pulse = 120;
    current = event_create(Once, id, 1);
    previous = current;
    event_cancel(&current);
    assert(current == 0 && FindEventInfo(id) == 0 && event_count() == 1);
    DWORD reusedID;
    Allocate(reusedID);
    current = event_create(Once, reusedID, 2);
    assert(current == previous);
    testHeart.pulse = 121;
    assert(event_process(121) == 0 && FindEventInfo(reusedID) != 0);
    testHeart.pulse = 122;
    assert(event_process(122) == 1 && FindEventInfo(reusedID) == 0);

    // Reset accepts negative delay and keeps the old cancelled queue entry.
    Allocate(id);
    testHeart.pulse = 130;
    current = event_create(Once, id, 5);
    event_reset_time(current, -3);
    assert(event_time(current) == -3 && event_count() == 2);
    assert(event_process(130) == 1 && FindEventInfo(id) == 0);
    assert(event_count() == 1);
    testHeart.pulse = 135;
    assert(event_process(135) == 0 && event_count() == 0);

    Allocate(id);
    selfHandle = event_create(CancelSelf, id, 1);
    testHeart.pulse = 136;
    int oldErrors = errors;
    assert(event_process(136) == 1);
    assert(selfHandle == 0 && FindEventInfo(id) == 0 && event_count() == 0);
    assert(errors == oldErrors + 1 && lastError.find("EVENT BUG!") == 0);

    Allocate(id);
    current = event_create(ResetWhileRunning, id, 1);
    testHeart.pulse = 137;
    assert(event_process(137) == 1 && FindEventInfo(id) == 0);

    // Shutdown drains queued/pool objects, but preserves the info registry.
    Allocate(id);
    event_create(Once, id, 100);
    event_destroy();
    assert(event_count() == 0 && FindEventInfo(id) != 0);
    FreeEventInfo(id); // Explicit test cleanup for the observed shutdown behavior.
    assert(FindEventInfo(id) == 0);

    oldErrors = errors;
    event_cancel(0);
    assert(errors == oldErrors + 1 && lastError == "null pointer");
    LPEVENT empty = 0;
    event_cancel(&empty);
    assert(errors == oldErrors + 1);

    // Missing ID paths call the fatal hook. The test hook records and returns.
    current = event_create(Once, 0, 1);
    testHeart.pulse = 138;
    assert(event_process(138) == 1 && fatals == 1 && event_count() == 0);
    assert(lastError.find("cannot find event info") == 0);
    FreeEventInfo(0);
    assert(fatals == 2);
    event_destroy();

#if defined(__i386__) && defined(__GNUC__)
    assert(sizeof(DWORD) == 4 && sizeof(EVENTINFO) == 8);
    assert(sizeof(EVENT) == 88 && offsetof(EVENT, dwInfoID) == 4);
    assert(offsetof(EVENT, info) == 8 && offsetof(EVENT, q_el) == 12);
    assert(offsetof(EVENT, is_force_to_end) == 16);
    assert(offsetof(EVENT, is_processing) == 17);
    assert(offsetof(EVENT, file) == 18 && offsetof(EVENT, line) == 84);
    assert(sizeof(HEART) == 36 && offsetof(HEART, pulse) == 32);
#endif
    return 0;
}
