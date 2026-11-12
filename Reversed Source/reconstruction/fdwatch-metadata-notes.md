# fdwatch DWARF evidence

Target: `PROJECT_ROOT\Original Binaries\game`, SHA-256 `61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c`.
Read-only pyelftools extraction of the `fdwatch.c` compilation unit. No target execution.
`fdwatch-metadata.json` retains all subprogram, structure, scalar, typedef and variable DIEs in that unit, including offsets and complete relevant type chains. `extract_fdwatch.py` reproduces it with pyelftools available on PYTHONPATH.

## Emitted functions

All twelve have `DW_AT_external=true`; `fdwatch_register` must not be made static merely because newer supplied public header omits its declaration.

```c
LPFDWATCH fdwatch_new(int nfiles);                              /* 081b7070 */
void fdwatch_delete(LPFDWATCH fdw);                             /* 081b7190 */
int fdwatch(LPFDWATCH fdw, struct timeval *timeout);             /* 081b71f0 */
void fdwatch_register(LPFDWATCH fdw, int flag, int fd, int rw);   /* 081b7340 */
void fdwatch_clear_fd(LPFDWATCH fdw, socket_t fd);                /* 081b7410 */
void fdwatch_add_fd(LPFDWATCH fdw, socket_t fd, void *client_data,
                   int rw, int oneshot);                       /* 081b7430 */
void fdwatch_del_fd(LPFDWATCH fdw, socket_t fd);                  /* 081b7530 */
void fdwatch_clear_event(LPFDWATCH fdw, socket_t fd,
                        unsigned int event_idx);               /* 081b7580 */
int fdwatch_check_event(LPFDWATCH fdw, socket_t fd,
                       unsigned int event_idx);                /* 081b7600 */
int fdwatch_get_ident(LPFDWATCH fdw, unsigned int event_idx);     /* 081b7700 */
int fdwatch_get_buffer_size(LPFDWATCH fdw, socket_t fd);          /* 081b7760 */
void *fdwatch_get_client_data(LPFDWATCH fdw,
                             unsigned int event_idx);           /* 081b7790 */
```

`socket_t` is signed int, 4 bytes. Pointer size 4. `fdwatch_check_fd` is absent from this compilation unit and has no addressed record in the existing full functions.csv inventory. This bounded absence does not prove an implementation never existed in source.

## Layouts

`struct fdwatch` / FDWATCH: 32 bytes, fields exactly match supplied reference non-Windows structure. kq KQUEUE/int at0; nfiles int at4; kqevents LPKEVENT at8; nkqevents int at12; kqrevents LPKEVENT at16; fd_event_idx int* at20; fd_data void** at24; fd_rw int* at28.

`struct kevent` / KEVENT:20 bytes. ident uintptr_t/unsigned int32 at0; filter signed short16 at4; flags u_short/unsigned short16 at6; fflags u_int/unsigned int32 at8; data intptr_t/signed int32 at12; udata void* at16.

`struct timeval`:8 bytes. tv_sec signed long32 at0; tv_usec suseconds_t/signed long32 at4.

`struct timespec`:8 bytes. tv_sec time_t/signed int32 at0; tv_nsec signed long32 at4. The differing fundamental type of seconds is retained in metadata.

EFdwatch enum:4 bytes. FDW_NONE=0, FDW_READ=1, FDW_WRITE=2, FDW_WRITE_ONESHOT=4, FDW_EOF=8.

DWARF establishes these signatures and layouts, not behavioral semantics or runtime parity. OS event macro values require code or original platform headers; they are not enumeration records in this CU.
