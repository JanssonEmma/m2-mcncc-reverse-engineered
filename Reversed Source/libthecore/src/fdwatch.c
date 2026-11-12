// Reconstructed original kqueue backend; no select/Windows branch in the target.
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/fdwatch.h"
#include "../include/log.h"

LPFDWATCH fdwatch_new(int nfiles)
{
    LPFDWATCH watcher;
    int queue = kqueue();
    if (queue == -1)
    {
        _sys_err("fdwatch_new", 15, "%s", strerror(errno));
        return NULL;
    }
    watcher = (LPFDWATCH)calloc(1, sizeof(FDWATCH));
    if (!watcher) goto allocation_failure;
    watcher->kq = queue;
    watcher->nfiles = nfiles;
    watcher->nkqevents = 0;
    watcher->kqevents = (LPKEVENT)calloc((size_t)(nfiles * 2), sizeof(KEVENT));
    if (!watcher->kqevents) goto allocation_failure;
    watcher->kqrevents = (LPKEVENT)calloc((size_t)(nfiles * 2), sizeof(KEVENT));
    if (!watcher->kqrevents) goto allocation_failure;
    watcher->fd_event_idx = (int*)calloc((size_t)nfiles, sizeof(int));
    if (!watcher->fd_event_idx) goto allocation_failure;
    watcher->fd_rw = (int*)calloc((size_t)nfiles, sizeof(int));
    if (!watcher->fd_rw) goto allocation_failure;
    watcher->fd_data = (void**)calloc((size_t)nfiles, sizeof(void*));
    if (!watcher->fd_data) goto allocation_failure;
    return watcher;
allocation_failure:
    perror("malloc failure");
    abort();
    return NULL; // Unreachable with the real abort implementation.
}

void fdwatch_delete(LPFDWATCH watcher)
{
    free(watcher->fd_data);
    free(watcher->fd_rw);
    free(watcher->kqevents);
    free(watcher->kqrevents);
    free(watcher->fd_event_idx);
    free(watcher);
}

int fdwatch(LPFDWATCH watcher, struct timeval* timeout)
{
    struct timespec wait;
    int result, i;
    if (watcher->nkqevents)
        sys_log(2, "fdwatch: nkqevents %d", watcher->nkqevents);
    if (timeout)
    {
        wait.tv_sec = timeout->tv_sec;
        wait.tv_nsec = timeout->tv_usec;
    }
    else
        wait.tv_sec = wait.tv_nsec = 0;
    result = kevent(watcher->kq, watcher->kqevents, watcher->nkqevents,
                    watcher->kqrevents, watcher->nfiles, &wait);
    watcher->nkqevents = 0;
    if (result == -1)
        return -1;
    memset(watcher->fd_event_idx, 0, sizeof(int) * (size_t)watcher->nfiles);
    for (i = 0; i < result; ++i)
    {
        int descriptor = (int)watcher->kqrevents[i].ident;
        if (descriptor >= watcher->nfiles)
            _sys_err("fdwatch", 79, "ident overflow %d nfiles: %d",
                     descriptor, watcher->nfiles);
        else if (watcher->kqrevents[i].filter == EVFILT_WRITE)
            watcher->fd_event_idx[descriptor] = i;
    }
    return result;
}

void fdwatch_register(LPFDWATCH watcher, int flag, int descriptor, int rw)
{
    if (flag == EV_DELETE)
    {
        if (watcher->fd_rw[descriptor] & FDW_READ)
        {
            watcher->kqevents[watcher->nkqevents].ident = descriptor;
            watcher->kqevents[watcher->nkqevents].flags = flag;
            watcher->kqevents[watcher->nkqevents].filter = EVFILT_READ;
            ++watcher->nkqevents;
        }
        if (watcher->fd_rw[descriptor] & FDW_WRITE)
        {
            watcher->kqevents[watcher->nkqevents].ident = descriptor;
            watcher->kqevents[watcher->nkqevents].flags = flag;
            watcher->kqevents[watcher->nkqevents].filter = EVFILT_WRITE;
            ++watcher->nkqevents;
        }
    }
    else
    {
        watcher->kqevents[watcher->nkqevents].ident = descriptor;
        watcher->kqevents[watcher->nkqevents].flags = (unsigned short)flag;
        watcher->kqevents[watcher->nkqevents].filter = rw == FDW_READ ? EVFILT_READ : EVFILT_WRITE;
        ++watcher->nkqevents;
    }
}

void fdwatch_clear_fd(LPFDWATCH watcher, socket_t descriptor)
{
    watcher->fd_data[descriptor] = NULL;
    watcher->fd_rw[descriptor] = 0;
}

void fdwatch_add_fd(LPFDWATCH watcher, socket_t descriptor, void* client_data, int rw, int oneshot)
{
    int flag = EV_ADD;
    if (descriptor >= watcher->nfiles)
    {
        _sys_err("fdwatch_add_fd", 132, "fd overflow %d", descriptor);
        return;
    }
    if (watcher->fd_rw[descriptor] & rw)
        return;
    watcher->fd_rw[descriptor] |= rw;
    sys_log(2, "FDWATCH_REGISTER fdw %p fd %d rw %d data %p",
            (void*)watcher, descriptor, rw, client_data);
    if (oneshot)
    {
        sys_log(2, "ADD ONESHOT fd_rw %d", watcher->fd_rw[descriptor]);
        flag = EV_ADD | EV_ONESHOT;
        watcher->fd_rw[descriptor] |= FDW_WRITE_ONESHOT;
    }
    watcher->fd_data[descriptor] = client_data;
    fdwatch_register(watcher, flag, descriptor, rw);
}

void fdwatch_del_fd(LPFDWATCH watcher, socket_t descriptor)
{
    fdwatch_register(watcher, EV_DELETE, descriptor, 0);
    fdwatch_clear_fd(watcher, descriptor);
}

// Closing a descriptor removes its kqueue filters. Drop unsent registrations
// and already-returned events so descriptor reuse cannot target a new peer.
void fdwatch_forget_closed_fd(LPFDWATCH watcher, socket_t descriptor)
{
    if (!watcher || descriptor < 0 || descriptor >= watcher->nfiles)
        return;

    int retained = 0;
    for (int i = 0; i < watcher->nkqevents; ++i)
        if (watcher->kqevents[i].ident != (unsigned int)descriptor)
            watcher->kqevents[retained++] = watcher->kqevents[i];
    watcher->nkqevents = retained;
    for (int i = 0; i < watcher->nfiles; ++i)
        if (watcher->kqrevents[i].ident == (unsigned int)descriptor)
            watcher->kqrevents[i].ident = (unsigned int)-1;

    watcher->fd_event_idx[descriptor] = 0;
    fdwatch_clear_fd(watcher, descriptor);
}

void fdwatch_clear_event(LPFDWATCH watcher, socket_t descriptor, unsigned int event_index)
{
    assert(event_index < (unsigned int)(watcher->nfiles * 2));
    if (watcher->kqrevents[event_index].ident == (unsigned int)descriptor)
        watcher->kqrevents[event_index].ident = 0;
}

int fdwatch_check_event(LPFDWATCH watcher, socket_t descriptor, unsigned int event_index)
{
    LPKEVENT event;
    assert(event_index < (unsigned int)(watcher->nfiles * 2));
    event = &watcher->kqrevents[event_index];
    if (event->ident != (unsigned int)descriptor)
        return FDW_NONE;
    if (event->flags & EV_ERROR)
        return FDW_EOF;
    if (event->flags & EV_EOF)
        return FDW_EOF;
    if (event->filter == EVFILT_READ)
    {
        if (watcher->fd_rw[descriptor] & FDW_READ)
            return FDW_READ;
    }
    else if (event->filter == EVFILT_WRITE)
    {
        if (watcher->fd_rw[descriptor] & FDW_WRITE)
        {
            if (watcher->fd_rw[descriptor] & FDW_WRITE_ONESHOT)
                watcher->fd_rw[descriptor] &= ~FDW_WRITE;
            return FDW_WRITE;
        }
    }
    else
        _sys_err("fdwatch_check_event", 200,
                 "fdwatch_check_event: Unknown filter %d (descriptor %d)", event->filter, descriptor);
    return FDW_NONE;
}

int fdwatch_get_ident(LPFDWATCH watcher, unsigned int event_index)
{
    assert(event_index < (unsigned int)(watcher->nfiles * 2));
    return (int)watcher->kqrevents[event_index].ident;
}

int fdwatch_get_buffer_size(LPFDWATCH watcher, socket_t descriptor)
{
    int index = watcher->fd_event_idx[descriptor];
    if (watcher->kqrevents[index].filter == EVFILT_WRITE)
        return (int)watcher->kqrevents[index].data;
    return 0;
}

void* fdwatch_get_client_data(LPFDWATCH watcher, unsigned int event_index)
{
    int descriptor;
    assert(event_index < (unsigned int)(watcher->nfiles * 2));
    if (watcher->kqrevents[event_index].ident >= (unsigned int)watcher->nfiles)
        return NULL;
    descriptor = (int)watcher->kqrevents[event_index].ident;
    if (descriptor < 0)
        return NULL;
    return watcher->fd_data[descriptor];
}
