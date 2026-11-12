// Recovered kqueue interface; see reconstruction/fdwatch-recovery.md.
#ifndef MCNCC_LIBTHECORE_FDWATCH_H
#define MCNCC_LIBTHECORE_FDWATCH_H
#include <sys/event.h>
#include <sys/time.h>
#include "typedef.h"
typedef struct fdwatch FDWATCH;
typedef struct fdwatch* LPFDWATCH;
typedef struct kevent KEVENT;
typedef struct kevent* LPKEVENT;
typedef int KQUEUE;
enum EFdwatch {
    FDW_NONE = 0, FDW_READ = 1, FDW_WRITE = 2,
    FDW_WRITE_ONESHOT = 4, FDW_EOF = 8
};
struct fdwatch {
    KQUEUE kq;
    int nfiles;
    LPKEVENT kqevents;
    int nkqevents;
    LPKEVENT kqrevents;
    int* fd_event_idx;
    void** fd_data;
    int* fd_rw;
};
#ifdef __cplusplus
extern "C" {
#endif
LPFDWATCH fdwatch_new(int nfiles);
void fdwatch_delete(LPFDWATCH watcher);
int fdwatch(LPFDWATCH watcher, struct timeval* timeout);
void fdwatch_register(LPFDWATCH watcher, int flag, int descriptor, int rw);
void fdwatch_clear_fd(LPFDWATCH watcher, socket_t descriptor);
void fdwatch_add_fd(LPFDWATCH watcher, socket_t descriptor, void* client_data, int rw, int oneshot);
void fdwatch_del_fd(LPFDWATCH watcher, socket_t descriptor);
void fdwatch_clear_event(LPFDWATCH watcher, socket_t descriptor, unsigned int event_index);
int fdwatch_check_event(LPFDWATCH watcher, socket_t descriptor, unsigned int event_index);
int fdwatch_get_ident(LPFDWATCH watcher, unsigned int event_index);
int fdwatch_get_buffer_size(LPFDWATCH watcher, socket_t descriptor);
void* fdwatch_get_client_data(LPFDWATCH watcher, unsigned int event_index);
#ifdef __cplusplus
}
#endif
#endif
