#ifndef __INC_METIN2_DBSERV_STDAFX_H__
#define __INC_METIN2_DBSERV_STDAFX_H__

#include "../../libthecore/include/typedef.h"
#include <assert.h>
#include <errno.h>
#ifdef _WIN32
#include <ws2tcpip.h>

inline void usleep(unsigned long microseconds)
{
    Sleep(microseconds / 1000);
}

inline unsigned int sleep(unsigned int seconds)
{
    Sleep(seconds * 1000);
    return 0;
}

#define strcasecmp _stricmp
#else
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "../../common/features.h"

#include "../../libthecore/include/buffer.h"
#include "../../libthecore/include/crypt.h"
#include "../../libthecore/include/fdwatch.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/memcpy.h"
#include "../../libthecore/include/signal.h"
#include "../../libthecore/include/socket.h"
#include "../../libthecore/include/utils.h"

#include "../../common/length.h"
#include "../../common/singleton.h"
#include "../../common/stl.h"
#include "../../common/tables.h"

#include "DBProtocol.h"

#ifndef sys_err
#define sys_err(...) _sys_err(__FUNCTION__, __LINE__, __VA_ARGS__)
#endif

#endif
