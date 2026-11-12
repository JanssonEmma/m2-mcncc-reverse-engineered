#include "buffer_manager.h"
#include <cstddef>

BUFFER_MANAGER::BUFFER_MANAGER() {}
BUFFER_MANAGER::~BUFFER_MANAGER() { Destroy(); }

void BUFFER_MANAGER::Destroy()
{
    while (!buffer_stack.empty())
    {
        LPBUFFER buffer = buffer_stack.top();
        buffer_stack.pop();
        buffer_delete(buffer);
    }
}

LPBUFFER BUFFER_MANAGER::get()
{
    if (buffer_stack.empty())
        buffer_stack.push(buffer_new(NULL, 8192));
    LPBUFFER buffer = buffer_stack.top();
    buffer_stack.pop();
    return buffer;
}

void BUFFER_MANAGER::release(LPBUFFER buffer)
{
    buffer_reset(buffer);
    buffer_stack.push(buffer);
}

TEMP_BUFFER::TEMP_BUFFER() { buf = BUFFER_MANAGER::instance().get(); }
TEMP_BUFFER::~TEMP_BUFFER() { BUFFER_MANAGER::instance().release(buf); }
const void* TEMP_BUFFER::read_peek() { return buffer_read_peek(buf); }
void TEMP_BUFFER::write(const void* data, int size) { buffer_write(buf, data, size); }
int TEMP_BUFFER::size() { return (int)buffer_size(buf); }

// Target static initialization creates this manager before main and destroys it at exit.
static BUFFER_MANAGER buffer_manager;
