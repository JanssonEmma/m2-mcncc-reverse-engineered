#include "stdafx.h"
#include "Lock.h"

CLock::CLock() : m_lock{}, m_bLocked(false)
{
}

CLock::~CLock()
{
}

void CLock::Initialize()
{
	m_bLocked = false;
#ifdef _WIN32
	InitializeCriticalSection(&m_lock);
#else
	pthread_mutex_init(&m_lock, NULL);
#endif
}

void CLock::Destroy()
{
	assert(!m_bLocked && "lock didn't released");
#ifdef _WIN32
	DeleteCriticalSection(&m_lock);
#else
	pthread_mutex_destroy(&m_lock);
#endif
}

int CLock::Trylock()
{
#ifdef _WIN32
	return TryEnterCriticalSection(&m_lock) ? 0 : EBUSY;
#else
	return pthread_mutex_trylock(&m_lock);
#endif
}

void CLock::Lock()
{
#ifdef _WIN32
	EnterCriticalSection(&m_lock);
#else
	pthread_mutex_lock(&m_lock);
#endif
	m_bLocked = true;
}

void CLock::Unlock()
{
	assert(m_bLocked && "lock didn't issued");
	m_bLocked = false;
#ifdef _WIN32
	LeaveCriticalSection(&m_lock);
#else
	pthread_mutex_unlock(&m_lock);
#endif
}
