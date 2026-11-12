// Scripted socket/watcher boundary with real recovered buffer and crypt modules.
#include "desc_fixture.h"
#include "../../game/src/desc_client.h"
#include <cassert>
#include <climits>
#include <cstring>
#include <cstdlib>
#include "../../libthecore/include/memcpy.h"
int max_bytes_written, current_bytes_written, total_bytes_written;
static int capacity, write_result, writes, rearmed;
static bool client_connect_mode;
static int client_connect_calls,client_connect_result,client_registrations;
static size_t requested;
static LPFDWATCH expected_watcher;
static socket_t expected_socket;
static void* expected_client;
extern "C" {
socket_t socket_connect(const char* host,WORD port) {
    assert(client_connect_mode && !std::strcmp(host,"127.0.0.1") && port==13000);
    ++client_connect_calls; return client_connect_result;
}
void socket_close(socket_t) { std::abort(); }
void fdwatch_del_fd(LPFDWATCH,socket_t) { std::abort(); }
void printdata(const unsigned char*,int) { std::abort(); }
int gettimeofday(struct timeval*,struct timezone*) { std::abort(); }
int usleep(unsigned int) { std::abort(); }
void socket_block(socket_t) { std::abort(); }
int fdwatch(LPFDWATCH,struct timeval*) { std::abort(); }
void* fdwatch_get_client_data(LPFDWATCH,unsigned int) { std::abort(); }
int fdwatch_check_event(LPFDWATCH,socket_t,unsigned int) { std::abort(); }
int socket_read(socket_t,char*,size_t) { std::abort(); }
DWORD get_dword_time() { return 1234; }
void sys_log(int, const char*, ...) {}
void _sys_err(const char*, int, const char*, ...) { std::abort(); }
void* (*thecore_memcpy)(void*, const void*, size_t) = std::memcpy;
int fdwatch_get_buffer_size(LPFDWATCH w, socket_t s) {
    assert(w == expected_watcher && s == expected_socket);
    return capacity;
}
void fdwatch_add_fd(LPFDWATCH w, socket_t s, void* client, int rw, int once) {
    assert(w == expected_watcher && s == expected_socket);
    if(client_connect_mode) { assert(client==expected_client && (rw==1 || rw==2) && once==0); ++client_registrations; return; }
    assert(client == expected_client && rw == FDW_WRITE && once == 1);
    ++rearmed;
}
int socket_write(socket_t s, const char* data, size_t n) {
    assert(s == expected_socket && data != 0);
    ++writes; requested = n;
    return write_result;
}
int MIN(int a, int b) { return a < b ? a : b; }
}
class ClientConnectionProbe : public CLIENT_DESC {
public:
    int phase_calls;
    ClientConnectionProbe():phase_calls(0) {}
    virtual void SetPhase(int phase) { ++phase_calls; m_iPhase=phase; }
    void LastTry(time_t value) { m_LastTryTime=value; }
    time_t LastTry() const { return m_LastTryTime; }
    int SuccessPhase() const { return m_iPhaseWhenSucceed; }
    int Phase() const { return m_iPhase; }
    void CheckSetup(LPFDWATCH watcher) { assert(m_lpFdw==watcher && m_stHost=="127.0.0.1" && m_wPort==13000 && m_iMinInputBufferLen==0x100000 && !buffer_size(m_lpInputBuffer) && !buffer_size(m_lpOutputBuffer) && m_sock==-1); }
    LPBUFFER Output() { return m_lpOutputBuffer; }
    void DetachSocket() { m_sock=-1; }
    void Encrypt() { m_bEncrypted=true; }
};
int main() {
    { DescOutputProbe lifecycle; lifecycle.CheckInitial();
      assert(lifecycle.GetType()==0); lifecycle.CheckReinitialize(); lifecycle.CheckHelpers(); }
    DescOutputProbe d;
    d.SetLoginKey(static_cast<DWORD>(71));
    assert(d.GetLoginKey()==71);
    CLoginKey key(99,&d);
    assert(key.m_dwExpireTime==0 && key.m_pkDesc==&d && static_cast<DWORD>(key)==99);
    d.SetLoginKey(&key);
    d.SetLoginKey(static_cast<DWORD>(72));
    assert(d.GetLoginKey()==99); // Attached key takes precedence over scalar fallback.
    key.m_dwKey=101; assert(d.GetLoginKey()==101);
    key.Expire(); assert(key.m_dwExpireTime==1234 && key.m_pkDesc==0);
    assert(d.GetLoginKey()==101); // Expiration leaves the key value and attached pointer.
    FDWATCH watcher;
    BUFFER b;
    char data[20] = "abcdefghijklmnop";
    std::memset(&b, 0, sizeof b);
    b.read_point = data; b.mem_data = data; b.mem_size = sizeof data;
    expected_watcher = &watcher; expected_socket = 17; expected_client = &d;
    d.Attach(&watcher, 17, &b);
    assert(d.ProcessOutput() == 0 && writes == 0);
    b.length = 10; capacity = 0;
    assert(d.ProcessOutput() == 0 && b.length == 10 && writes == 0);
    capacity = -1;
    assert(d.ProcessOutput() == 0 && writes == 0);
    capacity = 4; write_result = -1;
    assert(d.ProcessOutput() == -1 && b.length == 10 && rearmed == 0);
    assert(max_bytes_written == 0 && current_bytes_written == 0 && total_bytes_written == 0);
    write_result = 0;
    max_bytes_written = -1; // MAX is signed, as in the actual recovered utility.
    total_bytes_written = INT_MAX - 1;
    assert(d.ProcessOutput() == 0 && requested == 4 && b.length == 6 && rearmed == 1);
    assert(max_bytes_written == 4 && current_bytes_written == 4);
    assert(total_bytes_written == INT_MIN + 2);
    capacity = 100;
    assert(d.ProcessOutput() == 0 && requested == 6 && b.length == 0 && rearmed == 1);
    assert(max_bytes_written == 6 && current_bytes_written == 10);
    assert(d.ProcessOutput() == 0 && writes == 3);
    // Real client setup/connect logic at a scripted socket boundary.
    {
        ClientConnectionProbe client; client.Setup(&watcher,"127.0.0.1",13000); client.CheckSetup(&watcher);
        client.Encrypt(); client.Packet("raw",3);
        assert(buffer_size(client.Output())==3 && !std::memcmp(buffer_read_peek(client.Output()),"raw",3));
        client_connect_mode=true; expected_watcher=&watcher; expected_socket=5; expected_client=&client;
        client_connect_calls=client_registrations=0; client_connect_result=-1;
        client.LastTry(1700000000); assert(!client.Connect(8) && client.SuccessPhase()==8 && !client_connect_calls);
        client.LastTry(1699999998); assert(!client.Connect() && !client_connect_calls);
        client.LastTry(1699999997); assert(!client.Connect() && client_connect_calls==1 && client.Phase()==7 && client.phase_calls==1 && client.LastTry()==1700000000);
        client_connect_result=5; assert(!client.Connect() && client_connect_calls==1);
        client.LastTry(1699999997); assert(client.Connect() && client_connect_calls==2 && client.GetSocket()==5 && client.Phase()==8 && client.phase_calls==2 && client_registrations==2);
        client.LastTry(1699999997); assert(!client.Connect(9) && client.SuccessPhase()==9 && client_connect_calls==2 && client.LastTry()==1700000000);
        client.DetachSocket(); client_connect_mode=false;
    }

}
