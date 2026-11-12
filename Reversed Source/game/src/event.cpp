// Original compilation unit: event.c, compiled as C++ with GCC 3.3.3.
// Reconstruction of game SHA-256 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <map>
#include <stack>
#include "event.h"
#include "event_queue.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/log.h"

extern void ShutdownOnFatalError();

std::stack<LPEVENT> s_pool_event;
static CEventQueue cxx_q;
static std::uint32_t s_event_info_count;

namespace {
std::int32_t EventSigned32(std::uint32_t bits)
{
    std::int32_t pulse;
    std::memcpy(&pulse, &bits, sizeof(pulse));
    return pulse;
}

int EventDeadline(int pulse, long delay)
{
    return EventSigned32(static_cast<std::uint32_t>(pulse) + static_cast<std::uint32_t>(delay));
}
}
std::map<int, LPEVENTINFO> g_event_info;

// 0x0804cc60
DWORD __AllocEventInfo(int size, void** data)
{
    LPEVENTINFO info = new struct event_info;
    info->dwID = static_cast<DWORD>(++s_event_info_count);
    info->pvData = std::calloc(1, size);
    *data = info->pvData;
    g_event_info[EventSigned32(s_event_info_count)] = info;
    return info->dwID;
}

// 0x0804cd30
void* FindEventInfo(DWORD id)
{
    std::map<int, LPEVENTINFO>::iterator it = g_event_info.find(EventSigned32(static_cast<std::uint32_t>(id)));
    return it == g_event_info.end() ? 0 : it->second->pvData;
}

// 0x0804cd80: this helper has C++ linkage in the target.
void FreeEventInfo(DWORD id)
{
    std::map<int, LPEVENTINFO>::iterator it = g_event_info.find(EventSigned32(static_cast<std::uint32_t>(id)));
    if (it == g_event_info.end())
    {
        _sys_err("FreeEventInfo", 47, "cannot find event info by id %lu", id);
        ShutdownOnFatalError();
    }
    else
    {
        sys_log(1, "FreeEventInfo: event free id %lu", id);
        std::free(it->second->pvData);
        delete it->second;
        g_event_info.erase(it);
    }
}

// 0x0804ceb0
LPEVENT event_create_ex(TEVENTFUNC func, DWORD infoID, long when, char* file, int line)
{
    sys_log(1, "EVENT CREATE id %lu [%s:%d]", infoID, file, line);
    when = EventSigned32(static_cast<std::uint32_t>(when));
    if (when < 1)
        when = 1;
    LPEVENT newEvent;
    if (s_pool_event.empty())
        newEvent = static_cast<LPEVENT>(std::malloc(sizeof(EVENT)));
    else
    {
        newEvent = s_pool_event.top();
        s_pool_event.pop();
    }
    newEvent->info = 0;
    newEvent->func = func;
    newEvent->dwInfoID = infoID;
    newEvent->q_el = cxx_q.Enqueue(newEvent, EventDeadline(thecore_heart->pulse, when));
    newEvent->is_processing = 0;
    newEvent->is_force_to_end = 0;
    std::strncpy(newEvent->file, file, 63);
    newEvent->file[63] = '\0';
    newEvent->line = line;
    return newEvent;
}

// 0x0804cff0
void event_cancel(LPEVENT* handle)
{
    if (!handle)
    {
        _sys_err("event_cancel", 96, "null pointer");
        return;
    }
    LPEVENT current = *handle;
    if (!current)
        return;
    if (current->is_processing)
    {
        current->is_force_to_end = 1;
        if (current->q_el)
            current->q_el->bCancel = true;
    }
    else if (!current->q_el)
        _sys_err("event_cancel", 117, "event already canceled ptr %p %s:%d",
                 current, current->file, current->line);
    else if (current->q_el->bCancel)
        _sys_err("event_cancel", 124,
                 "event already canceled and marked [q_el->bCancel] event %p %s:%d",
                 current, current->file, current->line);
    else
    {
        current->q_el->bCancel = true;
        FreeEventInfo(current->dwInfoID);
        s_pool_event.push(current);
    }
    *handle = 0;
}

// 0x0804d120: reset does not clamp the delay, unlike creation.
void event_reset_time(LPEVENT current, long when)
{
    if (!current->is_processing)
    {
        if (current->q_el)
            current->q_el->bCancel = true;
        current->q_el = cxx_q.Enqueue(current, EventDeadline(thecore_heart->pulse, when));
    }
}

// 0x0804d170
void* event_info(LPEVENT current)
{
    return FindEventInfo(current->dwInfoID);
}

// 0x0804d190
int event_process(int pulse)
{
    int count = 0;
    while (pulse >= cxx_q.GetTopKey())
    {
        TQueueElement* element = cxx_q.Dequeue();
        if (element->bCancel)
        {
            cxx_q.Delete(element);
            continue;
        }
        LPEVENT current = static_cast<LPEVENT>(element->pvData);
        current->q_el = 0;
        cxx_q.Delete(element);
        current->is_processing = 1;
        current->info = event_info(current);
        if (!current->info)
        {
            _sys_err("event_process", 199,
                     "cannot find event info by id %lu create in %s:%d",
                     current->dwInfoID, current->file, current->line);
            s_pool_event.push(current);
            ShutdownOnFatalError();
        }
        else
        {
            const auto next = EventSigned32(static_cast<std::uint32_t>(current->func(current)));
            current->info = 0;
            if (current->is_force_to_end == 1 && next != 0)
                _sys_err("event_process", 211,
                         "EVENT BUG! pointer may invalid! create in %s:%d",
                         current->file, current->line);
            if (next <= 0 || current->is_force_to_end)
            {
                FreeEventInfo(current->dwInfoID);
                s_pool_event.push(current);
            }
            else
            {
                current->q_el = cxx_q.Enqueue(current, EventDeadline(pulse, next));
                current->is_processing = 0;
            }
        }
        ++count;
    }
    return count;
}

// 0x0804d370
long event_time(LPEVENT current)
{
    if (!current->q_el)
        return 0;
    return EventSigned32(static_cast<std::uint32_t>(current->q_el->iKey) -
                         static_cast<std::uint32_t>(thecore_heart->pulse));
}

// 0x0804d390: queued event info is NOT freed by this function in the binary.
void event_destroy()
{
    TQueueElement* element;
    while ((element = cxx_q.Dequeue()) != 0)
    {
        if (!element->bCancel)
            s_pool_event.push(static_cast<LPEVENT>(element->pvData));
        cxx_q.Delete(element);
    }
    while (!s_pool_event.empty())
    {
        LPEVENT current = s_pool_event.top();
        s_pool_event.pop();
        std::free(current);
    }
}

// 0x0804d4a0: counts queue entries, including cancelled tombstones.
int event_count()
{
    return cxx_q.Size();
}
