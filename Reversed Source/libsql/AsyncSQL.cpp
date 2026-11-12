#include "AsyncSQL.h"
#include <cstring>
#include <cerrno>
#include <chrono>
#include <system_error>

#ifdef _WIN32
#include <process.h>
#else
#include <sys/time.h>
#endif

namespace
{
    void GetQueryTime(timeval* time)
    {
        using namespace std::chrono;
        const auto elapsed = steady_clock::now().time_since_epoch();
        const auto wholeSeconds = duration_cast<seconds>(elapsed);
        time->tv_sec = static_cast<long>(wholeSeconds.count());
        time->tv_usec = static_cast<long>(duration_cast<microseconds>(elapsed - wholeSeconds).count());
    }
}

namespace
{
    class MySQLLibrary
    {
        public:
            MySQLLibrary()
            {
                initialized = mysql_library_init(0, nullptr, nullptr) == 0;
                if (!initialized)
                {
                    std::fputs("mysql_library_init failed\n", stderr);
                    return;
                }

                const auto version = mysql_get_client_version();
                const char* description = mysql_get_client_info();
                ready = version == LIBMYSQL_VERSION_ID &&
                        description && !std::strstr(description, "MariaDB");
                if (!ready)
                {
                    std::fprintf(stderr, "Oracle MySQL client matching headers (%d) required, found %s (%lu)\n",
                                 LIBMYSQL_VERSION_ID, description ? description : "unknown", version);
                }
            }

            ~MySQLLibrary()
            {
                if (initialized)
                    mysql_library_end();
            }

            bool IsReady() const { return ready; }

        private:
            bool initialized = false;
            bool ready = false;
    };

    bool InitializeMySQLLibrary()
    {
        static const MySQLLibrary library;
        return library.IsReady();
    }
}

// 081d5cd0: MYSQL is the first by-value member of the old nonvirtual class.
MYSQL* CAsyncSQL::GetSQLHandle()
{
    return &m_hDB;
}

// 081d4e30: queue even before connection; the ID wraps in the target's 32 bits.
void CAsyncSQL::AsyncQuery(const char* query)
{
    SQLMsg* message = new SQLMsg;
    message->m_pkSQL = &m_hDB;
    m_iMsgCount = static_cast<int>(static_cast<unsigned int>(m_iMsgCount)+1U);
    message->iID = m_iMsgCount;
    message->stQuery = query;
    PushQuery(message);
}

// 081d50b0: wake the worker while the queue mutex is still held.
void CAsyncSQL::PushQuery(SQLMsg* message)
{
    std::lock_guard<AsyncSQLMutex> lock(*m_mtxQuery);

    m_queue_query.push(message);
    m_sem.Release();
}

// 081d2170/081d3110: only these scalars are assigned after member construction.
// MYSQL, pipe descriptors and the server thread ID deliberately retain storage.
CAsyncSQL::CAsyncSQL()
{
    m_iMsgCount = 0;
    m_hThread = {};
    m_mtxQuery = 0;
    m_mtxResult = 0;
    m_bEnd = false;
    m_iQueryFinished = 0;
    m_bConnected = false;
    m_iCopiedQuery = 0;

    m_aiPipe[0] = -1;
    m_aiPipe[1] = -1;

    memset(&m_hDB, 0, sizeof(m_hDB));

    m_ulThreadID = 0;
}

CAsyncSQL::~CAsyncSQL()
{
    Destroy();
}

// 081d44a0: the original clears the query pointer twice, leaving result dangling.
void CAsyncSQL::Destroy()
{
    Quit();
    CloseClientHandle();

    if (m_mtxQuery)
    {
        delete m_mtxQuery;
        m_mtxQuery=0;
    }

    if (m_mtxResult)
    {
        delete m_mtxResult;
        m_mtxResult=0;
    }
}

void CAsyncSQL::Quit()
{
    if (!m_hThread)
        return;

    m_bEnd = true;
    m_sem.Release();
#ifdef _WIN32
    WaitForSingleObject(m_hThread, INFINITE);
    CloseHandle(m_hThread);
#else
    pthread_join(m_hThread, nullptr);
#endif
    m_hThread = {};
}

void CAsyncSQL::ReturnQuery(const char* query,void* userData)
{
    SQLMsg* message= new SQLMsg;
    message->m_pkSQL = &m_hDB;
    m_iMsgCount = static_cast<int>(static_cast<unsigned int>(m_iMsgCount) +1U);
    message->iID = m_iMsgCount;
    message->stQuery = query;
    message->bReturn = true;
    message->pvUserData = userData;
    PushQuery(message);
}

void CAsyncSQL::PushResult(SQLMsg* message)
{
    std::lock_guard<AsyncSQLMutex> lock(*m_mtxResult);
    m_queue_result.push(message);
}

bool CAsyncSQL::PopResult(SQLMsg** message)
{
    std::lock_guard<AsyncSQLMutex> lock(*m_mtxResult);
    
    if (m_queue_result.empty())
        return false;

    *message = m_queue_result.front();
    m_queue_result.pop();
    
    return true;
}

bool CAsyncSQL::PeekQuery(SQLMsg** message)
{
    std::lock_guard<AsyncSQLMutex> lock(*m_mtxQuery);
    if (m_queue_query.empty())
    {
        return false;
    }
    *message = m_queue_query.front();
    
    return true;
}

// The old id argument is unused; popping does not destroy the message.
bool CAsyncSQL::PopQuery(int)
{
    std::lock_guard<AsyncSQLMutex> lock(*m_mtxQuery);
    if (m_queue_query.empty())
        return false;
    m_queue_query.pop();
    
    return true;
}

bool CAsyncSQL::PeekQueryFromCopyQueue(SQLMsg** message)
{
    if (m_queue_query_copy.empty())
        return false;
    *message = m_queue_query_copy.front();
    return true;
}

INT CAsyncSQL::CopyQuery()
{
    std::lock_guard<AsyncSQLMutex> lock(*m_mtxQuery);
    if (m_queue_query.empty())
        return -1;
    
    while(!m_queue_query.empty())
    {
        m_queue_query_copy.push(m_queue_query.front());
        m_queue_query.pop();
    }
    const INT count = static_cast<INT>(m_queue_query_copy.size());
    
    return count;
}

bool CAsyncSQL::PopQueryFromCopyQueue()
{
    if (m_queue_query_copy.empty())
        return false;
    m_queue_query_copy.pop();
    return true;
}

int CAsyncSQL::GetCopiedQueryCount()
{
    return m_iCopiedQuery;
}

void CAsyncSQL::ResetCopiedQueryCount()
{
    m_iCopiedQuery=0;
}

void CAsyncSQL::AddCopiedQueryCount(int count)
{
    m_iCopiedQuery.fetch_add(count, std::memory_order_relaxed);
}

DWORD CAsyncSQL::CountQuery()
{
    if (m_mtxQuery)
    {
        std::lock_guard<AsyncSQLMutex> lock(*m_mtxQuery);
        const auto count = static_cast<DWORD>(m_queue_query.size());
        return count;
    }

    return static_cast<DWORD>(m_queue_query.size());
}

DWORD CAsyncSQL::CountResult()
{
    if (m_mtxResult)
    {
        std::lock_guard<AsyncSQLMutex> lock(*m_mtxResult);
        const auto count = static_cast<DWORD>(m_queue_result.size());
        return count;
    }

    return static_cast<DWORD>(m_queue_result.size());
}

// 081d54b0: this method is genuinely empty in the supplied target.
void CAsyncSQL::DumpQueryQueue(FILE*)
{
}

int CAsyncSQL::CountQueryFinished()
{
    return m_iQueryFinished;
}

void CAsyncSQL::ResetQueryFinished()
{
    m_iQueryFinished=0;
}

// 081d4510: thread entry only runs the worker after a successful connection.
#ifdef _WIN32
unsigned int __stdcall AsyncSQLThread(void* argument)
#else
void* AsyncSQLThread(void* argument)
#endif
{
    CAsyncSQL* sql = static_cast<CAsyncSQL*>(argument);

    if (mysql_thread_init())
    {
        std::fputs("mysql_thread_init failed\n", stderr);
#ifdef _WIN32
        return 0;
#else
        return nullptr;
#endif
    }

    sql->ChildLoop();
    sql->CloseClientHandle();

    mysql_thread_end();
#ifdef _WIN32
    return 0;
#else
    return nullptr;
#endif
}

bool CAsyncSQL::Setup(CAsyncSQL* source,bool noThread)
{
    return Setup(
        source->m_stHost.c_str(),
        source->m_stUser.c_str(),
        source->m_stPassword.c_str(),
        source->m_stDB.c_str(),
        source->m_stLocale.c_str(),
        noThread,
        source->m_port
    );
}

bool CAsyncSQL::Setup(const char* host, const char* user, const char* password, const char* db, const char* locale, bool noThread, unsigned int port)
{
    if (!InitializeMySQLLibrary())
        return false;

    m_port = port;
    m_threaded = !noThread;
    m_stHost = host;
    m_stUser = user;
    m_stPassword = password;
    m_stDB = db;
    
    if (locale)
        m_stLocale = locale;

    if (noThread)
        return Connect();

    if (!m_mtxQuery)
        m_mtxQuery = new AsyncSQLMutex;
    if (!m_mtxResult)
        m_mtxResult = new AsyncSQLMutex;

    if (!Connect())
    {
        m_threaded = false;
        return false;
    }
#ifdef _WIN32
    m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, AsyncSQLThread, this, 0, nullptr));
    if (!m_hThread)
    {
        std::perror("_beginthreadex");
        CloseClientHandle();
        m_threaded = false;
        return false;
    }
#else
    const int result = pthread_create(&m_hThread, nullptr, AsyncSQLThread, this);

    if (result != 0)
    {
        std::fprintf(stderr,"pthread_create failed: %d\n", result);
        CloseClientHandle();
        m_threaded = false;
        return false;
    }
#endif
    
    return true;
}

bool CAsyncSQL::QueryLocaleSet()
{
    if (m_stLocale.empty())
        return true;

    if (mysql_set_character_set(&m_hDB, m_stLocale.c_str()))
    {
        _sys_err("QueryLocaleSet",82,"cannot set locale %s by 'mysql_set_character_set', errno %u %s", m_stLocale.c_str(), mysql_errno(&m_hDB), mysql_error(&m_hDB));
        return false;
    }
    
    return true;
}

bool CAsyncSQL::Connect()
{
    if (!InitializeMySQLLibrary())
        return false;

    if (m_clientInitialized)
        return m_bConnected;

    if (mysql_init(&m_hDB) == nullptr)
    {
        fprintf(stderr, "mysql_init failed\n");
        return false;
    }

    m_clientInitialized = true;

    if (!m_stLocale.empty())
    {
        if (mysql_options(
            &m_hDB,
            MYSQL_SET_CHARSET_NAME,
            m_stLocale.c_str()) != 0)
        {
            fprintf(
                stderr,
                "mysql_options(MYSQL_SET_CHARSET_NAME) failed: [%u] %s\n",
                mysql_errno(&m_hDB),
                mysql_error(&m_hDB));

            CloseClientHandle();
            return false;
        }
    }

    unsigned int sslMode = 0;
    if (mysql_options(
        &m_hDB,
        MYSQL_OPT_SSL_MODE,
        &sslMode) != 0)
    {
        fprintf(
            stderr,
            "mysql_options(MYSQL_OPT_SSL_MODE) failed: [%u] %s\n",
            mysql_errno(&m_hDB),
            mysql_error(&m_hDB));

        CloseClientHandle();
        return false;
    }

#ifdef _WIN32
    const char* unixSocket = nullptr;
#else
    const char* unixSocket = "/var/run/mysql/mysql.sock";
#endif

    if (mysql_real_connect(
        &m_hDB,
        m_stHost.c_str(),
        m_stUser.c_str(),
        m_stPassword.c_str(),
        m_stDB.c_str(),
        static_cast<unsigned int>(m_port),
        unixSocket,
        CLIENT_MULTI_STATEMENTS) == nullptr)
    {
        fprintf(
            stderr,
            "mysql_real_connect failed: [%u] %s\n",
            mysql_errno(&m_hDB),
            mysql_error(&m_hDB));

        CloseClientHandle();
        return false;
    }

    m_ulThreadID = mysql_thread_id(&m_hDB);
    m_bConnected = true;

    return true;
}

void CAsyncSQL::CloseClientHandle()
{
    if (!m_clientInitialized)
        return;

    mysql_close(&m_hDB);
    m_clientInitialized = false;
    m_bConnected = false;
}

bool CAsyncSQL::RequestLocale(const std::string& locale)
{
    if (locale.empty())
        return true;

    {
        std::lock_guard<std::mutex> lock(m_localeMutex);
        m_requestedLocale = locale;
        m_hasRequestedLocale = true;
    }

    // Never call ApplyRequestedLocale while holding m_localeMutex.
    // For threaded connections the worker applies it before its next query.
    return m_threaded || ApplyRequestedLocale();
}

bool CAsyncSQL::ApplyRequestedLocale()
{
    std::lock_guard<std::mutex> lock(m_localeMutex);
    if (!m_hasRequestedLocale)
        return true;

    if (m_requestedLocale != m_stLocale)
    {
        m_stLocale.swap(m_requestedLocale);
        if (!QueryLocaleSet())
        {
            m_stLocale.swap(m_requestedLocale);
            return false;
        }
    }
    m_hasRequestedLocale = false;
    return true;
}

bool CAsyncSQL::ExecuteQuery(SQLMsg& message)
{
    if (!ApplyRequestedLocale())
    {
        message.uiSQLErrno = mysql_errno(&m_hDB);
        return false;
    }

    if (mysql_real_query(&m_hDB, message.stQuery.c_str(), message.stQuery.size()) == 0)
        return true;

    message.uiSQLErrno = mysql_errno(&m_hDB);
    return false;
}

void CAsyncSQL::DrainQueryResults()
{
    do
    {
        if (MYSQL_RES* result = mysql_store_result(&m_hDB))
            mysql_free_result(result);
    } while (mysql_next_result(&m_hDB) == 0);
}

SQLMsg* CAsyncSQL::DirectQuery(const char* query)
{
    SQLMsg* message = new SQLMsg;
    message->m_pkSQL = &m_hDB;
    m_iMsgCount = static_cast<int>(static_cast<unsigned int>(m_iMsgCount)+1U);
    message->iID = m_iMsgCount;
    message->stQuery=query;

    if (!ExecuteQuery(*message))
    {
        _sys_err("DirectQuery", 219, "MySQL query failed (error %u)", message->uiSQLErrno);
    }
    else
    {
        message->Store();
    }

    return message;
}

// 081d54c0: retain old retry pinning, separate timeval components and flush scope.
void CAsyncSQL::ChildLoop()
{
    timeval begin,end;
    GetQueryTime(&begin);

    while(!m_bEnd)
    {
        if (!m_sem.Wait())
        {
            std::fputs("CSemaphore::Wait failed in ChildLoop\n", stderr);
            break;
        }
        int count = CopyQuery();
        if (count<=0)
            continue;

        AddCopiedQueryCount(count);
        while(count--)
        {
            GetQueryTime(&begin);
            SQLMsg* message;
            if (!PeekQueryFromCopyQueue(&message))
                continue;

            const bool executed = ExecuteQuery(*message);
            if (!executed)
                _sys_err("ChildLoop", 483, "MySQL query failed (error %u)", message->uiSQLErrno);

            GetQueryTime(&end);
            const long long elapsedUs =
                (static_cast<long long>(end.tv_sec) - begin.tv_sec) * 1000000LL +
                (static_cast<long long>(end.tv_usec) - begin.tv_usec);
            if (elapsedUs > 200000)
                sys_log(0, "Slow MySQL query (%lld us)", elapsedUs);

            PopQueryFromCopyQueue();
            if (message->bReturn)
            { 
                if (executed)
                    message->Store();
                PushResult(message);
            }
            else
            {
                if (executed)
                    DrainQueryResults();
                delete message;
            }

            m_iQueryFinished.fetch_add(1, std::memory_order_relaxed);
        }
    }
    SQLMsg* message;
    while(PeekQuery(&message))
    {
        const bool executed = ExecuteQuery(*message);
        if (!executed)
            _sys_err("ChildLoop", 598, "MySQL query failed (error %u)", message->uiSQLErrno);

        PopQuery(message->iID);
        if(message->bReturn)
        {
            if (executed)
                message->Store();
            PushResult(message);
        }
        else
        {
            if (executed)
                DrainQueryResults();
            delete message;
        }
        m_iQueryFinished.fetch_add(1, std::memory_order_relaxed);
    }
}

// db 0x080bfab0: record the requested locale for subsequent queries.
bool CAsyncSQL2::SetLocale(const std::string& locale)
{
    return RequestLocale(locale);
}

