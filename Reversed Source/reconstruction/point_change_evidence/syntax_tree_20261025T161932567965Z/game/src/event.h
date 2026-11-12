// Recovered old event interface: raw pointers and numeric info IDs.
#ifndef MCNCC_EVENT_H
#define MCNCC_EVENT_H

#include "../../libthecore/include/typedef.h"

typedef struct event_info
{
    DWORD dwID;
    void* pvData;
} EVENTINFO, *LPEVENTINFO;

typedef struct event EVENT;
typedef struct event* LPEVENT;
typedef long (*TEVENTFUNC)(LPEVENT event);

struct SQueueElement;
struct event
{
    TEVENTFUNC func;
    DWORD dwInfoID;
    void* info;
    SQueueElement* q_el;
    char is_force_to_end;
    char is_processing;
    char file[64];
    int line;
};

#ifdef __cplusplus
extern "C" {
#endif
DWORD __AllocEventInfo(int size, void** data);
void* FindEventInfo(DWORD id);
LPEVENT event_create_ex(TEVENTFUNC func, DWORD infoID, long when, char* file, int line);
void event_cancel(LPEVENT* event);
void event_reset_time(LPEVENT event, long when);
void* event_info(LPEVENT event);
int event_process(int pulse);
long event_time(LPEVENT event);
void event_destroy();
int event_count();
#ifdef __cplusplus
}
#endif

// Convenience macros reconstructed to match the recovered calling interface.
// Original macro spelling/expansion is not present in the inspected metadata.
#define EVENTFUNC(name) long (name)(LPEVENT event)
#define event_create(func, infoID, when) \
    event_create_ex((func), (infoID), (when), const_cast<char*>(__FILE__), __LINE__)

#endif
