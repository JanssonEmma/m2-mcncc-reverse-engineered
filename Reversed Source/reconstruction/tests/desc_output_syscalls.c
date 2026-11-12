// Syscall boundary for the real descriptor/buffer/socket/fdwatch/utils integration.
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/event.h>
#include "socket_hooks.h"
#include "../../libthecore/include/memcpy.h"
int socket_test_errno;
static char read_data[4096];
static int read_result, read_error, read_calls, read_ready;
static int input_error_calls, read_error_calls, proceed_warnings, allow_clock;
static long clock_ticks;
static int fixed_clock, sync_error_line, entity_error_line, sector_error_calls, sector_error_line;
int desc_test_sector_error_calls(void) { return sector_error_calls; }
int desc_test_sector_error_line(void) { return sector_error_line; }
int desc_test_entity_error_line(void) { return entity_error_line; }
static unsigned long fixed_millis;
void desc_test_set_millis(unsigned long value) { fixed_clock=1; fixed_millis=value; allow_clock=1; }
int desc_test_sync_error_line(void) { return sync_error_line; }
void desc_test_recv(const char* data,int result,int error) {
    assert(result <= (int)sizeof(read_data));
    if(result>0) memcpy(read_data,data,(size_t)result);
    read_result=result; read_error=error; read_calls=0; read_ready=1;
}
int desc_test_recv_calls(void) { return read_calls; }
int desc_test_input_errors(void) { return input_error_calls; }
int desc_test_proceed_warnings(void) { return proceed_warnings; }
void desc_test_allow_clock(void) { allow_clock=1; }

static int descriptor = 5, available, send_calls, send_count, send_index;
static int results[16], errors[16], submitted_count, error_calls;
static struct kevent submitted[32];
static char written[128];
static int written_size,chat_command_logs;
int desc_test_chat_command_logs(void) { return chat_command_logs; }
void* (*thecore_memcpy)(void*, const void*, size_t) = memcpy;
void desc_test_capacity(int value) { available = value; }
void desc_test_send_reset(void) { send_calls = send_count = send_index = written_size = error_calls = 0; }
void desc_test_send_step(int result, int error) {
    assert(send_count < 16); results[send_count] = result; errors[send_count++] = error;
}
int desc_test_send_calls(void) { return send_calls; }
int desc_test_errors(void) { return error_calls; }
int desc_test_changes(void) { return submitted_count; }
const char* desc_test_written(void) { return written; }
int kqueue(void) { return 71; }
int kevent(int queue, const struct kevent* changes, int count, struct kevent* events,
           int capacity, const struct timespec* wait) {
    assert(queue == 71 && count <= 32 && capacity == 16 && wait);
    submitted_count = count; memcpy(submitted, changes, count * sizeof(*changes));
    memset(&events[0], 0, sizeof(*events));
    events[0].ident = descriptor; events[0].filter = EVFILT_WRITE; events[0].data = available;
    return 1;
}
ssize_t sk_send(int fd, const void* data, size_t length, int flags) {
    int result;
    assert(fd == descriptor && !flags && send_index < send_count);
    ++send_calls; result = results[send_index]; socket_test_errno = errors[send_index++];
    if (result > 0) {
        assert((size_t)result <= length && written_size + result < (int)sizeof(written));
        memcpy(written + written_size, data, result); written_size += result;
    }
    return result;
}
char* sk_strerror(int error) { (void)error; return "fixture error"; }
void sk_perror(const char* message) { (void)message; assert(!"unexpected perror"); }
static int motion_error_calls;
int desc_test_motion_errors(void) { return motion_error_calls; }
static int point_error_calls,point_error_value,point_error_maximum,point_error_line;
static char point_error_name[64];
int desc_test_point_errors(void) { return point_error_calls; }
int desc_test_point_value(void) { return point_error_value; }
int desc_test_point_maximum(void) { return point_error_maximum; }
int desc_test_point_line(void) { return point_error_line; }
const char* desc_test_point_name(void) { return point_error_name; }
static int inventory_error_calls;
int desc_test_inventory_errors(void) { return inventory_error_calls; }
void (*desc_test_event_force_error)(void)=0;
void _sys_err(const char* function, int line, const char* format, ...) {
    if(desc_test_event_force_error && !strcmp(function,"event_process") && line==211 && !strcmp(format,"EVENT BUG! pointer may invalid! create in %s:%d")) { desc_test_event_force_error(); return; }
    if(!strcmp(function,"GetMoveMotionSpeed") && line==2601) { ++motion_error_calls; return; }
    if((!strcmp(function,"GetPoint") && line==2738) || (!strcmp(function,"GetLimitPoint") && line==2784)) {
        va_list args; va_start(args,format);
        strncpy(point_error_name,va_arg(args,const char*),63); point_error_name[63]=0;
        point_error_value=va_arg(args,int); point_error_maximum=va_arg(args,int); va_end(args);
        point_error_line=line; ++point_error_calls;
        /* The target supplies only these three values; never format/read a nonexistent fourth argument. */
        return;
    }
    if((!strcmp(function,"GetItem") && line==212) || (!strcmp(function,"GetWear") && line==350) || (!strcmp(function,"SetItem") && line==229)) { ++inventory_error_calls; return; }
    if((!strcmp(function,"DecreasePC") && line==103) || (!strcmp(function,"InsertEntity") && line==139)) { ++sector_error_calls; sector_error_line=line; return; }
    if(!strcmp(function,"UpdateSectree") && line==120) { entity_error_line=line; return; }
    if(!strcmp(function,"SyncTime") && (line==565 || line==603)) { sync_error_line=line; return; }
    if(!strcmp(function,"ProcessInput") && (line==216 || line==231)) { ++input_error_calls; return; }
    if(!strcmp(function,"socket_read") && line==58) { ++read_error_calls; return; }
    if(!strcmp(function,"buffer_read_proceed") && line==179) { ++proceed_warnings; return; }
    if(strcmp(function,"socket_write")) fprintf(stderr,"unexpected descriptor dependency error %s:%d %s\n",function,line,format);
    assert(!strcmp(function, "socket_write") && line == 111 && !strcmp(format, "write to desc error"));
    ++error_calls;
}
void sys_log(int level, const char* format, ...) { (void)level; assert(format); if(!strcmp(format,"SNED COMMAND %s %s")) ++chat_command_logs; }
// All unused boundaries fail rather than reaching the host OS.
int sk_socket(int a, int b, int c) { (void)a; (void)b; (void)c; assert(0); return -1; }
ssize_t sk_recv(int fd,void* target,size_t capacity,int flags) {
    assert(fd==descriptor && !flags && read_ready && read_calls==0);
    ++read_calls; read_ready=0; socket_test_errno=read_error;
    if(read_result>0) { assert((size_t)read_result<=capacity); memcpy(target,read_data,(size_t)read_result); }
    return read_result;
}
ssize_t sk_recvfrom(int a, void* b, size_t c, int d, struct sockaddr* e, socklen_t* f)
{ (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; assert(0); return -1; }
int sk_bind(int a, const struct sockaddr* b, socklen_t c) { (void)a; (void)b; (void)c; assert(0); return -1; }
int sk_listen(int a, int b) { (void)a; (void)b; assert(0); return -1; }
int sk_accept(int a, struct sockaddr* b, socklen_t* c) { (void)a; (void)b; (void)c; assert(0); return -1; }
int sk_connect(int a, const struct sockaddr* b, socklen_t c) { (void)a; (void)b; (void)c; assert(0); return -1; }
static int expected_close=-1, close_calls;
void desc_test_expect_close(int fd) { expected_close=fd; close_calls=0; }
int desc_test_close_calls(void) { return close_calls; }
int sk_close(int fd) { assert(fd==expected_close); expected_close=-1; ++close_calls; return 0; }
int sk_setsockopt(int a, int b, int c, const void* d, socklen_t e)
{ (void)a; (void)b; (void)c; (void)d; (void)e; assert(0); return -1; }
int sk_getsockopt(int a, int b, int c, void* d, socklen_t* e)
{ (void)a; (void)b; (void)c; (void)d; (void)e; assert(0); return -1; }
int sk_fcntl(int a, int b, ...) { (void)a; (void)b; assert(0); return -1; }
unsigned int sk_inet_addr(const char* value) { (void)value; assert(0); return 0; }
unsigned short sk_htons(unsigned short value) { (void)value; assert(0); return 0; }
struct hostent* sk_gethostbyname(const char* value) { (void)value; assert(0); return NULL; }
int fork(void) { assert(!"unexpected fork"); return -1; }
long random(void) { assert(!"unexpected RNG"); return 0; }
int gettimeofday(struct timeval* value, struct timezone* zone)
{ assert(allow_clock && !zone);
  if(fixed_clock) { unsigned long raw=fixed_millis+0x43d4db10UL; value->tv_sec=raw/1000; value->tv_usec=(raw%1000)*1000; }
  else { value->tv_sec=100; value->tv_usec=++clock_ticks*1000; }
  return 0; }
int select(int n, fd_set* r, fd_set* w, fd_set* e, struct timeval* t)
{ (void)n; (void)r; (void)w; (void)e; (void)t; assert(!"unexpected select"); return -1; }

int usleep(unsigned int usec) { (void)usec; assert(!"unexpected terminal sleep"); return 0; }
