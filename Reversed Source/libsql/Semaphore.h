// Complete old semaphore data/API; POSIX behavior recovered from the target.
#ifndef MCNCC_SQL_SEMAPHORE_H
#define MCNCC_SQL_SEMAPHORE_H

#ifndef _WIN32
#include <semaphore.h>
#else
#include <Windows.h>
#endif

class CSemaphore
{
    private:
    #ifdef _WIN32
        HANDLE m_hSem = nullptr;
    #else
        sem_t* m_hSem = nullptr;
    #endif
    public:
        CSemaphore();
        ~CSemaphore();

        CSemaphore(const CSemaphore&) = delete;
        CSemaphore& operator=(const CSemaphore&) = delete;

        int     Initialize();
        void    Clear();
        void    Destroy();
        int     Wait();
        int     Release(int count = 1);
};
#endif
