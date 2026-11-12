// Target kevent/fdwatch model checks; compiled with the isolated test headers.
#include <stddef.h>
#include "../../libthecore/include/fdwatch.h"
#define CHECK(name, condition) typedef char name[(condition) ? 1 : -1]
CHECK(fdw_size, sizeof(FDWATCH) == 32);
CHECK(fdw_queue_offset, offsetof(FDWATCH, kq) == 0);
CHECK(fdw_files_offset, offsetof(FDWATCH, nfiles) == 4);
CHECK(fdw_changes_offset, offsetof(FDWATCH, kqevents) == 8);
CHECK(fdw_count_offset, offsetof(FDWATCH, nkqevents) == 12);
CHECK(fdw_results_offset, offsetof(FDWATCH, kqrevents) == 16);
CHECK(fdw_indexes_offset, offsetof(FDWATCH, fd_event_idx) == 20);
CHECK(fdw_client_offset, offsetof(FDWATCH, fd_data) == 24);
CHECK(fdw_rw_offset, offsetof(FDWATCH, fd_rw) == 28);
CHECK(kevent_size, sizeof(KEVENT) == 20);
CHECK(kevent_ident_offset, offsetof(KEVENT, ident) == 0);
CHECK(kevent_filter_offset, offsetof(KEVENT, filter) == 4);
CHECK(kevent_flags_offset, offsetof(KEVENT, flags) == 6);
CHECK(kevent_fflags_offset, offsetof(KEVENT, fflags) == 8);
CHECK(kevent_data_offset, offsetof(KEVENT, data) == 12);
CHECK(kevent_client_offset, offsetof(KEVENT, udata) == 16);
CHECK(fdw_timespec_size, sizeof(struct timespec) == 8);
CHECK(fdw_timespec_nsec_offset, offsetof(struct timespec, tv_nsec) == 4);
CHECK(fdw_timeval_size, sizeof(struct timeval) == 8);
CHECK(fdw_enum_values, FDW_NONE == 0 && FDW_READ == 1 && FDW_WRITE == 2
      && FDW_WRITE_ONESHOT == 4 && FDW_EOF == 8);
#undef CHECK
