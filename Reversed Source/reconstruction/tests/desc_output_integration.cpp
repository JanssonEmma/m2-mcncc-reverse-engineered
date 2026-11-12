// Real ProcessOutput + real C modules; only lifecycle and OS are fixtures.
#include "desc_fixture.h"
#include <cassert>
#include <cerrno>
#include <climits>
#include <cstring>
int max_bytes_written, current_bytes_written, total_bytes_written;
extern "C" {
void desc_test_capacity(int);
void desc_test_send_reset(void);
void desc_test_send_step(int, int);
int desc_test_send_calls(void);
int desc_test_errors(void);
int desc_test_changes(void);
const char* desc_test_written(void);
}
static void writable(LPFDWATCH watcher, DESC* client, int capacity, bool rearm) {
    desc_test_capacity(capacity);
    if (rearm) fdwatch_add_fd(watcher, 5, client, FDW_WRITE, 1);
    assert(fdwatch(watcher, 0) == 1);
    assert(fdwatch_get_client_data(watcher, 0) == client);
    assert(fdwatch_check_event(watcher, 5, 0) == FDW_WRITE);
}
int main() {
    DescOutputProbe client;
    LPFDWATCH watcher = fdwatch_new(16);
    LPBUFFER output = buffer_new(0, 64);
    client.Attach(0, -1, output);
    assert(client.ProcessOutput() == 0); // Empty output does not dereference watcher.
    client.Attach(watcher, 5, output);
    buffer_write(output, "abcdefghij", 10);
    fdwatch_add_fd(watcher, 5, &client, FDW_READ, 0);
    writable(watcher, &client, 4, true);
    assert(desc_test_changes() == 2 && watcher->fd_rw[5] == 5);
    desc_test_send_reset();
    desc_test_send_step(-1, EAGAIN); desc_test_send_step(2, 0); desc_test_send_step(2, 0);
    total_bytes_written = INT_MAX - 1;
    assert(client.ProcessOutput() == 0 && desc_test_send_calls() == 3);
    assert(buffer_size(output) == 6 && !memcmp(buffer_read_peek(output), "efghij", 6));
    assert(!memcmp(desc_test_written(), "abcd", 4));
    assert(max_bytes_written == 4 && current_bytes_written == 4 && total_bytes_written == INT_MIN + 2);
    assert(watcher->nkqevents == 1 && watcher->fd_rw[5] == 7);
    assert(watcher->kqevents[0].filter == EVFILT_WRITE && watcher->kqevents[0].flags == 17);
    assert(watcher->fd_data[5] == &client);
    writable(watcher, &client, 0, false);
    desc_test_send_reset();
    assert(client.ProcessOutput() == 0 && !desc_test_send_calls());
    assert(buffer_size(output) == 6 && !watcher->nkqevents && watcher->fd_rw[5] == 5);
    writable(watcher, &client, -1, true);
    assert(client.ProcessOutput() == 0 && !desc_test_send_calls() && buffer_size(output) == 6);
    writable(watcher, &client, 3, true);
    desc_test_send_step(-1, EINTR);
    assert(client.ProcessOutput() == -1 && desc_test_errors() == 1);
    assert(buffer_size(output) == 6 && current_bytes_written == 4 && !watcher->nkqevents);
    writable(watcher, &client, 100, true);
    desc_test_send_reset(); desc_test_send_step(2, 0); desc_test_send_step(4, 0);
    assert(client.ProcessOutput() == 0 && desc_test_send_calls() == 2);
    assert(!memcmp(desc_test_written(), "efghij", 6) && !buffer_size(output));
    assert(max_bytes_written == 6 && current_bytes_written == 10 && total_bytes_written == INT_MIN + 8);
    assert(output->write_point_pos == 0 && output->read_point == output->mem_data);
    assert(!watcher->nkqevents && client.ProcessOutput() == 0 && desc_test_send_calls() == 2);
    buffer_delete(output); fdwatch_delete(watcher);
    puts("descriptor output integration passed: real watcher/buffer/socket/utils, retries, counters and rearming");
    return 0;
}
