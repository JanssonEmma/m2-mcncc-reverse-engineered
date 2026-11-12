// vim:ts=8 sw=4
#ifndef __INC_LOCK_H__
#define __INC_LOCK_H__

#ifdef _WIN32
#include "../../libthecore/include/typedef.h"
typedef CRITICAL_SECTION lock_t;
#else
#include <pthread.h>
typedef pthread_mutex_t lock_t;
#endif

class CLock
{
    public:
	CLock();
	~CLock();

	void	Initialize();
	void	Destroy();
	int	Trylock();
	void	Lock();
	void	Unlock();

    private:
	lock_t	m_lock;
	bool	m_bLocked;
};
#endif
