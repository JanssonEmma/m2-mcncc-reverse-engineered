// Old cache API, absent from the newer reference. See buffer-manager-recovery.md.
#ifndef MCNCC_GAME_BUFFER_MANAGER_H
#define MCNCC_GAME_BUFFER_MANAGER_H
#include <stack>
#include "../../common/singleton.h"
#include "../../libthecore/include/buffer.h"
class TEMP_BUFFER;
class BUFFER_MANAGER : public singleton<BUFFER_MANAGER>
{
public:
    BUFFER_MANAGER();
    virtual ~BUFFER_MANAGER();
    void Destroy();
protected:
    LPBUFFER get();
    void release(LPBUFFER buffer);
    std::stack<LPBUFFER> buffer_stack;
    friend class TEMP_BUFFER; // Required by verified protected calls.
};
class TEMP_BUFFER
{
public:
    TEMP_BUFFER();
    ~TEMP_BUFFER();
    const void* read_peek();
    void write(const void* data, int size);
    int size();
    LPBUFFER getptr() { return buf; }
protected:
    LPBUFFER buf;
};
#endif
