#include "matrix_for_runup.h"
#include "db.h"
#include "config.h"
#include "locale_service.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <memory>

MatrixDBManager::MatrixDBManager() : m_bIsConnect(false) {}
MatrixDBManager::~MatrixDBManager() {}

// 080cfd20. Connection settings come from MATRIX_SQL.
bool MatrixDBManager::InitMatrix(const char* host, const char* user, const char* password, const char* database)
{
    if (!Connect(host, user, password, database) || !IsConnected())
        return false;
    std::fprintf(stderr, "Matrix DB Successed to connect %s\n", g_stLocale.c_str());
    return true;
}

// 080cfdd0
bool MatrixDBManager::GainMatrix(const char* login, char* cardId, char* matrix)
{
    char query[1024];
    std::sprintf(query, "select card_id FROM `mat_user` WHERE username='%s'", login);
    SQLMsg* message = DirectQuery(query);
    if (!message) {
        sys_log(0, "cannot get the MATRIX");
        return false;
    }
    if (!message->Get()->iNumRows) {
        std::puts(query);
        sys_log(0, "[GainMatrix]DirectQuery failed( %s )", query);
        delete message;
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(message->Get()->pSQLResult);
    if (!row || !row[0]) {
        delete message;
        return false;
    }
    std::strncpy(cardId, row[0], 31);
    cardId[31] = '\0';
    delete message;

    for (int i = 0; i < 8; i += 2) {
        matrix[i] = "0123456789ABCDEF"[number_ex(0, 5, "matrix_for_runup.cpp", 0x52+i)+10];
        matrix[i+1] = static_cast<char>('1'+number_ex(0, 7, "matrix_for_runup.cpp", 0x53+i));
    }
    if (test_server) {
        sys_log(0, "Matrix Runup %s[%s]\n [%c %c] [%c %c] [%c %c] [%c %c]", login, cardId,
                matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6], matrix[7]);
    }
    return true;
}

bool MatrixDBManager::CheckMatrix(const char* cardId, const char* matrix, const char* answer)
{
    if (std::strlen(cardId) > 31 || std::strlen(matrix) != 8 || std::strlen(answer) != 4)
        return false;
    char escapedID[63];
    mysql_real_escape_string(m_sql_direct.GetSQLHandle(), escapedID, cardId, std::strlen(cardId));
    std::unique_ptr<SQLMsg> message(DirectQuery(
        "SELECT card_str FROM `mat_id` WHERE card_id='%s'", escapedID));
    if (!message || !message->Get() || !message->Get()->pSQLResult || !message->Get()->iNumRows)
        return false;

    MYSQL_ROW row = mysql_fetch_row(message->Get()->pSQLResult);
    if (!row || !row[0] || std::strlen(row[0]) < 48)
        return false;
    for (unsigned index = 0; index < 4; ++index) {
        const int cardRow = matrix[2 * index] - 'A';
        const int cardCol = matrix[2 * index + 1] - '1';
        if (cardRow < 0 || cardRow >= 6 || cardCol < 0 || cardCol >= 8)
            return false;
        const char value = answer[index];
        const char upper = value >= 'a' && value <= 'z' ? value - ('a' - 'A') : value;
        if (upper != row[0][cardRow * 8 + cardCol])
            return false;
    }
    return true;
}

// 080d05e0
bool MatrixDBManager::Connect(const char* host, const char* user, const char* password, const char* database)
{
    if (m_sql.Setup(host, user, password, database, g_stLocale.c_str(), false))
        m_bIsConnect = true;
    if (!m_sql_direct.Setup(host, user, password, database, g_stLocale.c_str(), true))
        _sys_err("Connect", 0x13d, "cannot open direct sql connection to host %s character set %s",
                 host, g_stLocale.c_str());
    return m_bIsConnect;
}

// 080d0730
SQLMsg* MatrixDBManager::DirectQuery(const char* format, ...)
{
    char query[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(query, sizeof(query), format, args);
    va_end(args);
    return m_sql_direct.DirectQuery(query);
}

// 080d06e0
void MatrixDBManager::Query(const char* format, ...)
{
    char query[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(query, sizeof(query), format, args);
    va_end(args);
    m_sql.AsyncQuery(query);
}

// 080d0780
bool MatrixDBManager::IsConnected()
{
    return m_bIsConnect;
}

// 080d0790
unsigned long MatrixDBManager::EscapeString(void* output, const void* input, unsigned long length)
{
    return mysql_real_escape_string(m_sql_direct.GetSQLHandle(), static_cast<char*>(output),
                                    static_cast<const char*>(input), length);
}

// 080d07c0
void MatrixDBManager::ReturnQuery(int type, DWORD ident, void* data, const char* format, ...)
{
    char query[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(query, sizeof(query), format, args);
    va_end(args);

    auto* info = new CReturnQueryInfo;
    info->iQueryType = QUERY_TYPE_RETURN;
    info->iType = type;
    info->dwIdent = ident;
    info->pvData = data;
    m_sql.ReturnQuery(query, info);
}

// 080d0840
SQLMsg* MatrixDBManager::PopResult()
{
    SQLMsg* message;
    return m_sql.PopResult(&message) ? message : nullptr;
}

// 080d0870
void MatrixDBManager::Process()
{
    while (std::unique_ptr<SQLMsg> message{PopResult()}) {
        auto* info = static_cast<CQueryInfo*>(message->pvUserData);
        if (!info)
            continue;
        switch (info->iQueryType) {
        case QUERY_TYPE_RETURN:
            AnalyzeReturnQuery(message.get());
            break;
        case QUERY_TYPE_FUNCTION:
            static_cast<CFuncQueryInfo*>(info)->f(message.get());
            break;
        case QUERY_TYPE_AFTER_FUNCTION:
            static_cast<CFuncAfterQueryInfo*>(info)->f();
            break;
        }
    }
}

// 080d09c0
void MatrixDBManager::AnalyzeReturnQuery(SQLMsg* message)
{
    std::unique_ptr<CReturnQueryInfo> info(static_cast<CReturnQueryInfo*>(message->pvUserData));
    _sys_err("AnalyzeReturnQuery", 0x1ac, "FATAL ERROR!!! Unhandled return query id %d", info->iType);
}
