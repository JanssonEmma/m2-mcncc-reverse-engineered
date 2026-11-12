// Complete old semaphore data/API; POSIX behavior recovered from the target.
#ifndef MCNCC_SQL_SEMAPHORE_H
#define MCNCC_SQL_SEMAPHORE_H
#include <semaphore.h>
class CSemaphore {
    sem_t* m_hSem;
public:
    CSemaphore();
    ~CSemaphore();
    int Initialize();
    void Clear();
    void Destroy();
    int Wait();
    int Release(int count=1);
};
#endif
