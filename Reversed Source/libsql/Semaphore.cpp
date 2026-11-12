#include "Semaphore.h"

#include <cstdio>
#include <climits>

#ifndef _WIN32
#include <cerrno>
#endif

CSemaphore::CSemaphore()
{
    Initialize();
}

CSemaphore::~CSemaphore()
{
    Destroy();
}

int CSemaphore::Initialize()
{
    Clear();
#ifdef _WIN32
    m_hSem = CreateSemaphoreW(nullptr, 0, LONG_MAX, nullptr);
    return m_hSem != nullptr;
#else
    m_hSem = new sem_t;

    if (sem_init(m_hSem, 0, 0) == -1)
    {
        std::perror("sem_init");

        delete m_hSem;
        m_hSem = nullptr;

        return 0;
    }

    return 1;
#endif
}

void CSemaphore::Clear()
{
    if (!m_hSem)
        return;

#ifdef _WIN32
    if (!CloseHandle(m_hSem))
        std::fprintf(stderr, "CloseHandle failed: %lu\n", GetLastError());
#else
    if (sem_destroy(m_hSem) == -1)
    {
        std::perror("sem_destroy");
        return;
    }

    delete m_hSem;
#endif
    
    m_hSem = nullptr;
}

int CSemaphore::Wait()
{
    if (!m_hSem)
        return 0;

#ifdef _WIN32
    return WaitForSingleObject(m_hSem, INFINITE) == WAIT_OBJECT_0;
#else
    int result;

    do
    {
        result = sem_wait(m_hSem);
    } while (result == -1 && errno == EINTR);

    if (result == -1)
    {
        std::perror("sem_wait");
        return 0;
    }

    return 1;
#endif
}

int CSemaphore::Release(int count)
{
    if (!m_hSem || count <= 0)
        return 0;

#ifdef _WIN32
    return ReleaseSemaphore(m_hSem, count, nullptr) != 0;
#else
    for (int i = 0; i < count; ++i)
    {
        if (sem_post(m_hSem) == -1)
        {
            std::perror("sem_post");
            return 0;
        }
    }

    return 1;
#endif
}

void CSemaphore::Destroy()
{
    Clear();
}
