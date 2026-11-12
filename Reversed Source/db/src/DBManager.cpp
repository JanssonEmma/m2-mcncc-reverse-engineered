#include "stdafx.h"
#include "DBManager.h"
#include <cassert>
#include <memory>

extern std::string g_stLocale;

CDBManager::CDBManager()
{
    Initialize();
}

CDBManager::~CDBManager()
{
    Destroy();
}

void CDBManager::Initialize()
{
    for (int i = 0; i < SQL_MAX_NUM; ++i)
    {
        m_mainSQL[i] = NULL;
        m_directSQL[i] = NULL;
        m_asyncSQL[i] = NULL;
    }
}

void CDBManager::Destroy()
{
    Clear();
}

void CDBManager::Clear()
{
    for (int i = 0; i < SQL_MAX_NUM; ++i)
    {
        if (m_mainSQL[i])
        {
            delete m_mainSQL[i];
            m_mainSQL[i] = NULL;
        }

        if (m_directSQL[i])
        {
            delete m_directSQL[i];
            m_directSQL[i] = NULL;
        }

        if (m_asyncSQL[i])
        {
            delete m_asyncSQL[i];
            m_asyncSQL[i] = NULL;
        }
    }

    Initialize();
}

void CDBManager::Quit()
{
    for (int i = 0; i < SQL_MAX_NUM; ++i)
    {
        if (m_mainSQL[i])
            m_mainSQL[i]->Quit();

        if (m_asyncSQL[i])
            m_asyncSQL[i]->Quit();

        if (m_directSQL[i])
            m_directSQL[i]->Quit();
    }
}

SQLMsg * CDBManager::PopResult()
{
    SQLMsg * p;

    for (int i = 0; i < SQL_MAX_NUM; ++i)
        if (m_mainSQL[i] && m_mainSQL[i]->PopResult(&p))
            return p;

    return NULL;
}

SQLMsg * CDBManager::PopResult(eSQL_SLOT slot)
{
    SQLMsg * p;

    if (m_mainSQL[slot] && m_mainSQL[slot]->PopResult(&p))
            return p;

    return NULL;
}
int CDBManager::Connect(int iSlot, const char * db_address, int db_port, const char * db_name,
                       const char * user, const char * pwd)
{
    if (!db_address || !db_name || iSlot < 0 || iSlot >= SQL_MAX_NUM)
        return false;

    if (db_port < 0 || db_port > 65535)
    {
        sys_err("Invalid MySQL port %d for SQL slot %d", db_port, iSlot);
        return false;
    }

    if (m_directSQL[iSlot] || m_mainSQL[iSlot] || m_asyncSQL[iSlot])
    {
        sys_err("SQL slot %d is already configured", iSlot);
        return false;
    }

    const auto port = static_cast<unsigned int>(db_port);
    auto direct = std::make_unique<CAsyncSQL2>();
    if (!direct->Setup(db_address, user, pwd, db_name, g_stLocale.c_str(), true, port))
        return false;

    auto main = std::make_unique<CAsyncSQL2>();
    if (!main->Setup(db_address, user, pwd, db_name, g_stLocale.c_str(), false, port))
        return false;

    auto async = std::make_unique<CAsyncSQL2>();
    if (!async->Setup(db_address, user, pwd, db_name, g_stLocale.c_str(), false, port))
        return false;

    m_directSQL[iSlot] = direct.release();
    m_mainSQL[iSlot] = main.release();
    m_asyncSQL[iSlot] = async.release();
    return true;
}

SQLMsg * CDBManager::DirectQuery(const char * c_pszQuery, int iSlot)
{
    return m_directSQL[iSlot]->DirectQuery(c_pszQuery);
}

extern int g_query_count[2];

void CDBManager::ReturnQuery(const char * c_pszQuery, int iType, DWORD dwIdent, void * udata, int iSlot)
{
    assert(iSlot < SQL_MAX_NUM);
    CQueryInfo * p = new CQueryInfo;

    p->iType = iType;
    p->dwIdent = dwIdent;
    p->pvData = udata;

    m_mainSQL[iSlot]->ReturnQuery(c_pszQuery, p);
    ++g_query_count[0];
}

void CDBManager::AsyncQuery(const char * c_pszQuery, int iSlot)
{
    assert(iSlot < SQL_MAX_NUM);
    m_asyncSQL[iSlot]->AsyncQuery(c_pszQuery);
    ++g_query_count[1];
}

unsigned long CDBManager::EscapeString(void *to, const void *from, unsigned long length, int iSlot)
{
    assert(iSlot < SQL_MAX_NUM);
    return mysql_real_escape_string_quote(m_directSQL[iSlot]->GetSQLHandle(),
                                          static_cast<char*>(to), static_cast<const char*>(from),
                                          length, '\'');
}

void CDBManager::FlushQueryQueue(int iSlot, const char* filename)
{
    assert(iSlot < SQL_MAX_NUM);

    FILE* fp = fopen(filename, "w");
    if (!fp)
    {
        _sys_err(__FUNCTION__, 176, "cannot open file %s", filename);
        return;
    }

    m_mainSQL[iSlot]->DumpQueryQueue(fp);
    fclose(fp);

    CAsyncSQL2 sql;
    if (!m_directSQL[iSlot] || !sql.Setup(m_directSQL[iSlot], true))
    {
        _sys_err(__FUNCTION__, 186, "Cannot create sql connection when flushing queries");
        return;
    }

    constexpr int bufferSize = 32768;
    const auto buffer = std::make_unique_for_overwrite<char[]>(bufferSize);

    fp = fopen(filename, "r");
    if (!fp)
        return;
    while (fgets(buffer.get(), bufferSize, fp))
    {
        if (!strchr(buffer.get(), '\n') && !feof(fp))
        {
            int next = fgetc(fp);
            if (next != '\n' && next != EOF)
            {
                while (next != '\n' && next != EOF)
                    next = fgetc(fp);
                _sys_err(__FUNCTION__, __LINE__, "Skipping an oversized SQL line");
                continue;
            }
            if (ferror(fp))
                break;
        }

        char* terminator = strrchr(buffer.get(), ';');
        if (!terminator)
        {
            _sys_err(__FUNCTION__, __LINE__, "Skipping an SQL line without a terminator");
            continue;
        }
        *terminator = '\0';
        delete sql.DirectQuery(buffer.get());
    }

    fclose(fp);
}

bool CDBManager::SetLocale(const char* locale)
{
    for (int i = 0; i < SQL_MAX_NUM; ++i)
    {
        if (m_directSQL[i] && !m_directSQL[i]->SetLocale(locale))
            return false;
    }

    for (int i = 0; i < SQL_MAX_NUM; ++i)
    {
        if (m_mainSQL[i])
            m_mainSQL[i]->SetLocale(locale);
        if (m_asyncSQL[i])
            m_asyncSQL[i]->SetLocale(locale);
    }
    return true;
}
