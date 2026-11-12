#ifndef __INC_METIN2_DB_DBMANAGER_H__
#define __INC_METIN2_DB_DBMANAGER_H__

#include "../../libsql/AsyncSQL.h"

#define SQL_SAFE_LENGTH(size) ((size) * 2 + 1)
#define QUERY_SAFE_LENGTH(size) (1024 + SQL_SAFE_LENGTH(size))

class CQueryInfo
{
public:
    int iType;
    DWORD dwIdent;
    void* pvData;
};

enum eSQL_SLOT
{
    SQL_PLAYER,
    SQL_ACCOUNT,
    SQL_COMMON,
    SQL_HOTBACKUP,
    SQL_MAX_NUM
};

class CDBManager : public singleton<CDBManager>
{
protected:
    void Initialize();
    void Destroy();

public:
    CDBManager();
    virtual ~CDBManager();

    void Clear();
    void Quit();

    int Connect(int iSlot, const char* host, int port, const char* database, const char* user, const char* password);

    void ReturnQuery(const char* query, int type, DWORD ident, void* data, int iSlot = SQL_PLAYER);
    void AsyncQuery(const char* query, int iSlot = SQL_PLAYER);
    SQLMsg* DirectQuery(const char* query, int iSlot = SQL_PLAYER);

    SQLMsg* PopResult();
    SQLMsg* PopResult(eSQL_SLOT slot);

    unsigned long EscapeString(void* to, const void* from, unsigned long length, int iSlot = SQL_PLAYER);

    DWORD CountReturnQuery(int i) { return m_mainSQL[i] ? m_mainSQL[i]->CountQuery() : 0; }
    DWORD CountReturnResult(int i) { return m_mainSQL[i] ? m_mainSQL[i]->CountResult() : 0; }
    DWORD CountReturnQueryFinished(int i) { return m_mainSQL[i] ? m_mainSQL[i]->CountQueryFinished() : 0; }
    DWORD CountReturnCopiedQuery(int i) { return m_mainSQL[i] ? m_mainSQL[i]->GetCopiedQueryCount() : 0; }

    DWORD CountAsyncQuery(int i) { return m_asyncSQL[i] ? m_asyncSQL[i]->CountQuery() : 0; }
    DWORD CountAsyncResult(int i) { return m_asyncSQL[i] ? m_asyncSQL[i]->CountResult() : 0; }
    DWORD CountAsyncQueryFinished(int i) { return m_asyncSQL[i] ? m_asyncSQL[i]->CountQueryFinished() : 0; }
    DWORD CountAsyncCopiedQuery(int i) { return m_asyncSQL[i] ? m_asyncSQL[i]->GetCopiedQueryCount() : 0; }

    void FlushQueryQueue(int iSlot, const char* filename);
    void ResetCounter()
    {
        for (int i = 0; i < SQL_MAX_NUM; ++i)
        {
            if (m_mainSQL[i])
            {
                m_mainSQL[i]->ResetQueryFinished();
                m_mainSQL[i]->ResetCopiedQueryCount();
            }

            if (m_asyncSQL[i])
            {
                m_asyncSQL[i]->ResetQueryFinished();
                m_asyncSQL[i]->ResetCopiedQueryCount();
            }
        }
    }

private:
    CAsyncSQL2* m_mainSQL[SQL_MAX_NUM];
    CAsyncSQL2* m_directSQL[SQL_MAX_NUM];
    CAsyncSQL2* m_asyncSQL[SQL_MAX_NUM];
    [[maybe_unused]] int m_quit; // Retained for the reconstructed class layout.

public:
    bool SetLocale(const char* locale);
};

#endif
