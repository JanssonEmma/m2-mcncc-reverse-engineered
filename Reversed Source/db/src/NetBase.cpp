#include "stdafx.h"
#include "NetBase.h"

LPFDWATCH CNetBase::m_fdWatcher = NULL;

CNetBase::CNetBase()
{
}

CNetBase::~CNetBase()
{
}

CNetPoller::CNetPoller()
{
}

CNetPoller::~CNetPoller()
{
	Destroy();
}

bool CNetPoller::Create()
{
	sys_log(1, "NetPoller::Create()");

	if (m_fdWatcher)
		return true;

	m_fdWatcher = fdwatch_new(512);

	if (!m_fdWatcher)
	{
		Destroy();
		return false;
	}

	return true;
}

void CNetPoller::Destroy()
{
	sys_log(1, "NetPoller::Destroy()");

	if (m_fdWatcher)
	{
#ifndef _WIN32
		close(m_fdWatcher->kq);
#endif
		fdwatch_delete(m_fdWatcher);
		m_fdWatcher = NULL;
	}

	thecore_destroy();
}
