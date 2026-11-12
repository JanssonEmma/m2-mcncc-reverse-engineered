// Reference-guided singleton; BUFFER_MANAGER instantiation checked against target.
#ifndef MCNCC_COMMON_SINGLETON_H
#define MCNCC_COMMON_SINGLETON_H
#include <assert.h>
template <typename T> class singleton
{
public:
    static T* ms_singleton;
    singleton()
    {
        assert(!ms_singleton);
        // Equivalent base adjustment without narrowing the host pointer to long.
        ms_singleton = static_cast<T*>(this);
    }
    virtual ~singleton()
    {
        assert(ms_singleton);
        ms_singleton = 0;
    }
    static T& instance() { assert(ms_singleton); return *ms_singleton; }
    static T& Instance() { assert(ms_singleton); return *ms_singleton; }
    static T* instance_ptr() { return ms_singleton; }
};
template <typename T> T* singleton<T>::ms_singleton = 0;
#endif
