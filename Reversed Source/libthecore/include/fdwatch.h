// Recovered kqueue interface; see reconstruction/fdwatch-recovery.md.
#ifndef MCNCC_LIBTHECORE_FDWATCH_H
#define MCNCC_LIBTHECORE_FDWATCH_H
#include "typedef.h"
#ifndef __WIN32__
#include <sys/event.h>
#include <sys/time.h>
#endif
typedef struct fdwatch FDWATCH;
typedef struct fdwatch* LPFDWATCH;
enum EFdwatch {
    FDW_NONE = 0, FDW_READ = 1, FDW_WRITE = 2,
    FDW_WRITE_ONESHOT = 4, FDW_EOF = 8
};
#ifdef __WIN32__
struct fdwatch {
    fd_set rfd_set;
    fd_set wfd_set;
    socket_t* select_fds;
    int* select_rfdidx;
    int nselect_fds;
    fd_set working_rfd_set;
    fd_set working_wfd_set;
    int nfiles;
    void** fd_data;
    int* fd_rw;
};
#else
typedef struct kevent KEVENT;
typedef struct kevent* LPKEVENT;
typedef int KQUEUE;
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
#endif
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
void fdwatch_forget_closed_fd(LPFDWATCH watcher, socket_t descriptor);
void fdwatch_clear_event(LPFDWATCH watcher, socket_t descriptor, unsigned int event_index);
int fdwatch_check_event(LPFDWATCH watcher, socket_t descriptor, unsigned int event_index);
int fdwatch_get_ident(LPFDWATCH watcher, unsigned int event_index);
int fdwatch_get_buffer_size(LPFDWATCH watcher, socket_t descriptor);
void* fdwatch_get_client_data(LPFDWATCH watcher, unsigned int event_index);
#ifdef __cplusplus
}
#endif
#endif
