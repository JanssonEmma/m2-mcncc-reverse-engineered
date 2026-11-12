// Actual fdwatch.c with allocation/assert/kernel fixtures; no real descriptor poll.
#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../libthecore/include/fdwatch.h"

typedef struct allocation { void* value; size_t count, size; int live; } allocation;
static allocation memory[32];
static int allocated, allocation_calls, free_calls, fail_allocation_at, abort_expected, assert_expected;
static int abort_calls, assert_calls, perror_calls, queue_calls, queue_result, poll_calls, poll_result;
static int error_calls, error_line, log_calls;
static char error_function[64], error_format[160], error_message[256], last_log[160];
static void* freed[8];
static struct kevent response[16], changes[32];
static int response_count, change_count, max_events;
static struct timespec recorded_timeout;
static LPFDWATCH expected_watcher;
static jmp_buf escape;

void fw_assert_fail(const char* expression, const char* file, int line) {
    if (assert_expected) { ++assert_calls; longjmp(escape, 2); }
    fprintf(stderr, "unexpected assertion %s at %s:%d\n", expression, file, line); exit(2);
}
void* fw_calloc(size_t count, size_t size) {
    void* value;
    ++allocation_calls;
    if (allocation_calls == fail_allocation_at) return NULL;
    assert(allocated < 32);
    value = calloc(count ? count : 1, size ? size : 1);
    assert(value);
    memory[allocated].value = value; memory[allocated].count = count;
    memory[allocated].size = size; memory[allocated++].live = 1;
    return value;
}
void fw_free(void* value) {
    int i;
    for (i = 0; i < allocated; ++i) if (memory[i].value == value && memory[i].live) break;
    assert(i < allocated && free_calls < 8);
    freed[free_calls++] = value; memory[i].live = 0; free(value);
}
void fw_abort(void) { assert(abort_expected); ++abort_calls; longjmp(escape, 1); }
void fw_perror(const char* text) { assert(!strcmp(text, "malloc failure")); ++perror_calls; }
char* fw_strerror(int error) { assert(error == errno); return "fixture error"; }
void _sys_err(const char* function, int line, const char* format, ...) {
    va_list ap;
    ++error_calls; strcpy(error_function, function); error_line = line; strcpy(error_format, format);
    va_start(ap, format); vsnprintf(error_message, sizeof(error_message), format, ap); va_end(ap);
}
void sys_log(int level, const char* format, ...) {
    assert(level == 2); ++log_calls; strcpy(last_log, format);
}
int kqueue(void) { ++queue_calls; errno = EINVAL; return queue_result; }
int kevent(int queue, const struct kevent* submitted, int count,
           struct kevent* events, int capacity, const struct timespec* wait) {
    assert(expected_watcher && queue == expected_watcher->kq);
    assert(submitted == expected_watcher->kqevents && events == expected_watcher->kqrevents);
    assert(count <= 32 && response_count <= capacity && wait);
    ++poll_calls; change_count = count; max_events = capacity; recorded_timeout = *wait;
    memcpy(changes, submitted, (size_t)count * sizeof(*changes));
    if (poll_result != -1) memcpy(events, response, (size_t)response_count * sizeof(*events));
    return poll_result;
}
static void reset(void) {
    int i;
    for (i = 0; i < allocated; ++i) if (memory[i].live) free(memory[i].value);
    memset(memory, 0, sizeof(memory)); memset(response, 0, sizeof(response));
    allocated = allocation_calls = free_calls = fail_allocation_at = abort_expected = assert_expected = 0;
    abort_calls = assert_calls = perror_calls = queue_calls = poll_calls = poll_result = response_count = 0;
    error_calls = error_line = log_calls = change_count = max_events = 0;
    queue_result = 71; expected_watcher = NULL;
}
static LPFDWATCH create(void) { expected_watcher = fdwatch_new(8); assert(expected_watcher); return expected_watcher; }
static void event(int slot, unsigned int descriptor, short filter, unsigned short flags, int data) {
    response[slot].ident = descriptor; response[slot].filter = filter;
    response[slot].flags = flags; response[slot].data = data;
    if (response_count <= slot) response_count = slot + 1;
    poll_result = response_count;
}
static void check_allocation(void) {
    LPFDWATCH w; void* original[6]; int i;
    reset(); w = create();
    assert(queue_calls == 1 && allocation_calls == 6 && allocated == 6);
    assert(memory[0].count == 1 && memory[0].size == sizeof(FDWATCH));
    assert(memory[1].count == 16 && memory[1].size == sizeof(KEVENT));
    assert(memory[2].count == 16 && memory[2].size == sizeof(KEVENT));
    assert(memory[3].count == 8 && memory[3].size == sizeof(int));
    assert(memory[4].count == 8 && memory[4].size == sizeof(int));
    assert(memory[5].count == 8 && memory[5].size == sizeof(void*));
    assert(w->kq == 71 && w->nfiles == 8 && !w->nkqevents);
    assert(!w->fd_rw[7] && !w->fd_event_idx[7] && !w->fd_data[7]);
    original[0] = w->fd_data; original[1] = w->fd_rw; original[2] = w->kqevents;
    original[3] = w->kqrevents; original[4] = w->fd_event_idx; original[5] = w;
    fdwatch_delete(w);
    assert(free_calls == 6); for (i = 0; i < 6; ++i) assert(freed[i] == original[i]);
    // No close operation exists in the module; its queue descriptor remains open.
    reset(); queue_result = -1;
    assert(fdwatch_new(8) == NULL && !allocation_calls && error_line == 15);
    assert(!strcmp(error_function, "fdwatch_new") && !strcmp(error_message, "fixture error"));
    for (i = 1; i <= 6; ++i) {
        reset(); fail_allocation_at = i; abort_expected = 1;
        if (!setjmp(escape)) { fdwatch_new(8); assert(!"allocation failure returned"); }
        assert(abort_calls == 1 && perror_calls == 1 && !free_calls && allocated == i - 1);
    }
    reset();
}
static void check_registration(void) {
    LPFDWATCH w; int a, b, old_logs;
    reset(); w = create();
    w->kqevents[0].fflags = 123; w->kqevents[0].data = 456; w->kqevents[0].udata = &b;
    fdwatch_add_fd(w, 3, &a, FDW_READ, 0);
    assert(w->fd_rw[3] == FDW_READ && w->fd_data[3] == &a && w->nkqevents == 1);
    assert(w->kqevents[0].ident == 3 && w->kqevents[0].filter == EVFILT_READ);
    assert(w->kqevents[0].flags == EV_ADD && w->kqevents[0].fflags == 123);
    assert(w->kqevents[0].data == 456 && w->kqevents[0].udata == &b);
    assert(!strcmp(last_log, "FDWATCH_REGISTER fdw %p fd %d rw %d data %p"));
    old_logs = log_calls;
    fdwatch_add_fd(w, 3, &b, FDW_READ, 1);
    assert(w->nkqevents == 1 && log_calls == old_logs && w->fd_data[3] == &a);
    fdwatch_add_fd(w, 3, &b, FDW_WRITE, -1);
    assert(w->fd_rw[3] == 7 && w->fd_data[3] == &b && w->nkqevents == 2);
    assert(w->kqevents[1].filter == EVFILT_WRITE && w->kqevents[1].flags == 17);
    fdwatch_del_fd(w, 3);
    assert(w->nkqevents == 4 && !w->fd_rw[3] && !w->fd_data[3]);
    assert(w->kqevents[2].flags == EV_DELETE && w->kqevents[2].filter == EVFILT_READ);
    assert(w->kqevents[3].flags == EV_DELETE && w->kqevents[3].filter == EVFILT_WRITE);
    fdwatch_del_fd(w, 3); assert(w->nkqevents == 4);
    fdwatch_add_fd(w, 4, &a, FDW_READ | FDW_WRITE, 0);
    assert(w->fd_rw[4] == 3 && w->kqevents[4].filter == EVFILT_WRITE);
    fdwatch_add_fd(w, 4, &b, FDW_WRITE, 0); assert(w->fd_data[4] == &a && w->nkqevents == 5);
    fdwatch_clear_fd(w, 4); assert(!w->fd_rw[4] && !w->fd_data[4] && w->nkqevents == 5);
    fdwatch_add_fd(w, 8, &a, FDW_READ, 0);
    assert(error_line == 132 && !strcmp(error_message, "fd overflow 8") && w->nkqevents == 5);
    // Register is externally visible; any rw other than exactly READ selects WRITE.
    fdwatch_register(w, 0x10001, 5, 0);
    assert(w->kqevents[5].flags == 1 && w->kqevents[5].filter == EVFILT_WRITE);
    fdwatch_add_fd(w, 6, &a, FDW_READ, 1);
    assert(w->fd_rw[6] == (FDW_READ | FDW_WRITE_ONESHOT));
    assert(w->kqevents[6].filter == EVFILT_READ && w->kqevents[6].flags == 17);
    reset();
}
static void check_poll(void) {
    LPFDWATCH w; int a, i; struct timeval timeout = {2, 123456};
    reset(); w = create();
    fdwatch_add_fd(w, 3, &a, FDW_READ, 0);
    event(0, 3, EVFILT_READ, 0, 100); event(1, 4, EVFILT_WRITE, 0, 222);
    event(2, 4, EVFILT_WRITE, 0, 333); event(3, 8, EVFILT_WRITE, 0, 444);
    memset(w->fd_event_idx, 0x7f, 8 * sizeof(int));
    assert(fdwatch(w, &timeout) == 4);
    assert(change_count == 1 && max_events == 8 && !w->nkqevents);
    assert(recorded_timeout.tv_sec == 2 && recorded_timeout.tv_nsec == 123456);
    assert(timeout.tv_sec == 2 && timeout.tv_usec == 123456);
    assert(w->fd_event_idx[4] == 2 && w->fd_event_idx[3] == 0 && error_line == 79);
    assert(!strcmp(error_message, "ident overflow 8 nfiles: 8"));
    assert(fdwatch_get_buffer_size(w, 4) == 333 && fdwatch_get_buffer_size(w, 3) == 0);
    event(0, 4, EVFILT_WRITE, 0, 999); response_count = poll_result = 1;
    assert(fdwatch(w, NULL) == 1 && !recorded_timeout.tv_sec && !recorded_timeout.tv_nsec);
    assert(!w->fd_event_idx[3]);
    assert(fdwatch_get_buffer_size(w, 3) == 999); // Zero index aliases first event, no ident check.
    fdwatch_add_fd(w, 4, &a, FDW_WRITE, 0);
    for (i = 0; i < 8; ++i) w->fd_event_idx[i] = 6;
    poll_result = -1; response_count = 0;
    assert(fdwatch(w, NULL) == -1 && !w->nkqevents && change_count == 1);
    assert(w->fd_event_idx[4] == 6); // Kernel failure leaves previous result/index state.
    poll_result = 0;
    assert(fdwatch(w, NULL) == 0);
    for (i = 0; i < 8; ++i) assert(w->fd_event_idx[i] == 0);
    reset();
}
static void check_events(void) {
    LPFDWATCH w; int a, b, i;
    reset(); w = create(); w->fd_data[0] = &a; w->fd_rw[0] = FDW_READ;
    fdwatch_add_fd(w, 3, &b, FDW_READ, 0);
    event(0, 3, EVFILT_READ, 0, 12); assert(fdwatch(w, NULL) == 1);
    assert(fdwatch_get_ident(w, 0) == 3 && fdwatch_get_client_data(w, 0) == &b);
    assert(fdwatch_check_event(w, 4, 0) == 0 && fdwatch_check_event(w, 3, 0) == FDW_READ);
    w->kqrevents[0].flags = EV_ERROR;
    assert(fdwatch_check_event(w, 3, 0) == FDW_EOF);
    w->kqrevents[0].flags = EV_EOF; assert(fdwatch_check_event(w, 3, 0) == FDW_EOF);
    w->kqrevents[0].flags = 0; w->kqrevents[0].filter = -3;
    assert(fdwatch_check_event(w, 3, 0) == 0 && error_line == 200);
    w->kqrevents[0].filter = EVFILT_READ;
    fdwatch_clear_event(w, 4, 0); assert(fdwatch_get_ident(w, 0) == 3);
    fdwatch_clear_event(w, 3, 0); assert(fdwatch_get_ident(w, 0) == 0);
    assert(fdwatch_check_event(w, 3, 0) == 0 && fdwatch_get_client_data(w, 0) == &a);
    assert(fdwatch_check_event(w, 0, 0) == FDW_READ); // Clearing to0 can alias descriptor0.
    w->kqrevents[0].ident = 8; assert(fdwatch_get_client_data(w, 0) == NULL);
    w->kqrevents[0].ident = 3; w->kqrevents[0].filter = EVFILT_WRITE;
    fdwatch_add_fd(w, 3, &b, FDW_WRITE, 1);
    assert(fdwatch_check_event(w, 3, 0) == FDW_WRITE && w->fd_rw[3] == 5);
    assert(fdwatch_check_event(w, 3, 0) == FDW_NONE);
    fdwatch_add_fd(w, 3, &b, FDW_WRITE, 0);
    assert(w->fd_rw[3] == 7 && fdwatch_check_event(w, 3, 0) == FDW_WRITE && w->fd_rw[3] == 5);
    fdwatch_del_fd(w, 3); // One-shot consumed WRITE, so only READ deletion is submitted.
    assert(w->kqevents[w->nkqevents - 1].filter == EVFILT_READ);
    fdwatch_add_fd(w, 6, &b, FDW_READ, 1);
    w->kqrevents[1].ident = 6; w->kqrevents[1].filter = EVFILT_READ;
    assert(fdwatch_check_event(w, 6, 1) == FDW_READ && w->fd_rw[6] == 5);
    w->kqrevents[15].ident = 7; assert(fdwatch_get_ident(w, 15) == 7);
    // Every event-index API asserts capacity, not the most recent poll count.
    for (i = 0; i < 4; ++i) {
        assert_expected = 1;
        if (!setjmp(escape)) {
            if (i == 0) fdwatch_clear_event(w, 3, 16);
            if (i == 1) fdwatch_check_event(w, 3, 16);
            if (i == 2) fdwatch_get_ident(w, 16);
            if (i == 3) fdwatch_get_client_data(w, 16);
            assert(!"out of capacity returned");
        }
        assert_expected = 0;
    }
    assert(assert_calls == 4);
    reset();
}
int main(void) {
    check_allocation(); check_registration(); check_poll(); check_events();
    puts("fdwatch regression passed: allocation, registration, polling, one-shot state and event queries");
    return 0;
}
