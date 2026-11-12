#include "desc_fixture.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include "../../libthecore/include/memcpy.h"
int max_bytes_written,current_bytes_written,total_bytes_written;
extern int desc_test_phase_calls;
static int times[8],time_count,time_index,blocks,sleeps,polls,checks,error_line,capacity,result;
static const char* last_log;
static int event_counts[4],events[4][2];
static void* clients[4][2];
static LPFDWATCH expected_watcher;
static void* expected_client;
extern "C" {
socket_t socket_connect(const char*,WORD) { std::abort(); }
void socket_close(socket_t) { std::abort(); }
void fdwatch_del_fd(LPFDWATCH,socket_t) { std::abort(); }
void printdata(const unsigned char*,int) { std::abort(); }
DWORD get_dword_time() { std::abort(); }
void* (*thecore_memcpy)(void*,const void*,size_t)=std::memcpy;
int MIN(int a,int b) { return a<b?a:b; }
void sys_log(int,const char* format,...) { last_log=format; }
void _sys_err(const char* function,int line,const char*,...) {
    assert(!std::strcmp(function,"FlushOutput") && (line==667 || line==687)); error_line=line;
}
int socket_read(socket_t,char*,size_t) { std::abort(); }
void socket_block(socket_t socket) { assert(socket==17); ++blocks; }
int socket_write(socket_t socket,const char*,size_t) { assert(socket==17); return result; }
int gettimeofday(struct timeval* value,struct timezone* zone) {
    assert(!zone && time_index<time_count); value->tv_sec=times[time_index++]; value->tv_usec=0; return 0;
}
int usleep(unsigned int usec) { assert(usec==250000); ++sleeps; return 0; }
int fdwatch(LPFDWATCH watcher,struct timeval* timeout) {
    assert(watcher==expected_watcher && timeout->tv_sec==0 && timeout->tv_usec==10000 && polls<4);
    return event_counts[polls++];
}
void* fdwatch_get_client_data(LPFDWATCH watcher,unsigned int index) {
    assert(watcher==expected_watcher && index<2); return clients[polls-1][index];
}
int fdwatch_check_event(LPFDWATCH watcher,socket_t socket,unsigned int index) {
    assert(watcher==expected_watcher && socket==17 && clients[polls-1][index]==expected_client);
    ++checks; return events[polls-1][index];
}
int fdwatch_get_buffer_size(LPFDWATCH watcher,socket_t socket) {
    assert(watcher==expected_watcher && socket==17); return capacity;
}
void fdwatch_add_fd(LPFDWATCH watcher,socket_t socket,void* client,int rw,int once) {
    assert(watcher==expected_watcher && socket==17 && client==expected_client && rw==2 && once==1);
}
}
static void Setup(DescOutputProbe& d,LPBUFFER output,int phase=5) {
    buffer_reset(output); buffer_write(output,"abc",3);
    d.SetPacketState(phase);
    blocks=sleeps=polls=checks=error_line=desc_test_phase_calls=time_index=0;
    last_log=0; capacity=3; result=0;
    std::memset(event_counts,0,sizeof event_counts);
    std::memset(events,0,sizeof events);
    std::memset(clients,0,sizeof clients);
    times[0]=times[1]=100; time_count=2;
}
int main() {
    DescOutputProbe d; FDWATCH watcher; LPBUFFER output=buffer_new(0,64);
    expected_watcher=&watcher; expected_client=&d; d.Attach(&watcher,17,output);
    d.FlushOutput(); assert(!blocks && !sleeps && !time_index);
    Setup(d,output);
    event_counts[0]=2; clients[0][0]=reinterpret_cast<void*>(1); clients[0][1]=&d;
    events[0][0]=8; events[0][1]=2;
    d.FlushOutput();
    assert(!buffer_size(output) && blocks==1 && sleeps==1 && polls==1 && checks==1);
    assert(!desc_test_phase_calls && !std::strcmp(last_log,"FLUSH SUCCESS"));
    // >10 seconds without a write event, equality10 still polls.
    Setup(d,output); times[1]=110; times[2]=111; time_count=3;
    d.FlushOutput();
    assert(polls==1 && time_index==3 && desc_test_phase_calls==1 && d.PacketPhase()==0);
    assert(buffer_size(output)==3 && sleeps==1);
    // A WRITE notification extends the deadline even when capacity is zero.
    Setup(d,output); capacity=0; times[2]=120; times[3]=121; time_count=4;
    event_counts[0]=1; clients[0][0]=&d; events[0][0]=2;
    d.FlushOutput();
    assert(polls==2 && time_index==4 && checks==1 && desc_test_phase_calls==1 && sleeps==1);
    Setup(d,output); event_counts[0]=-1;
    d.FlushOutput();
    assert(error_line==667 && !desc_test_phase_calls && d.PacketPhase()==5 && sleeps==1);
    Setup(d,output); event_counts[0]=1; clients[0][0]=&d; events[0][0]=8;
    d.FlushOutput(); assert(desc_test_phase_calls==1 && !error_line && sleeps==1);
    Setup(d,output); event_counts[0]=1; clients[0][0]=&d; events[0][0]=2; result=-1;
    d.FlushOutput(); assert(error_line==687 && desc_test_phase_calls==1 && buffer_size(output)==3);
    // A closed phase still performs one polling iteration after switching to blocking mode.
    Setup(d,output,0); d.FlushOutput();
    assert(polls==1 && blocks==1 && sleeps==1 && !desc_test_phase_calls);
    buffer_delete(output);
    std::puts("descriptor flush passed: event filtering, writes, failures, strict10/20-second bounds and terminal sleep");
}
