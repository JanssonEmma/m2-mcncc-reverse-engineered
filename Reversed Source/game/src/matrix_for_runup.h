#ifndef MCNCC_GAME_MATRIX_FOR_RUNUP_H
#define MCNCC_GAME_MATRIX_FOR_RUNUP_H

#include "../../common/singleton.h"
#include "../../libsql/AsyncSQL.h"

class MatrixDBManager : public singleton<MatrixDBManager> {
public:
    MatrixDBManager();
    virtual ~MatrixDBManager();
    bool InitMatrix(const char* host, const char* user, const char* password, const char* database);
    bool GainMatrix(const char* login, char* cardId, char* matrix);
    bool CheckMatrix(const char* cardId, const char* matrix, const char* answer);
    bool Connect(const char* host, const char* user, const char* password, const char* database);
    void Query(const char* format, ...);
    SQLMsg* DirectQuery(const char* format, ...);
    bool IsConnected();
    unsigned long EscapeString(void* output, const void* input, unsigned long length);
    void ReturnQuery(int type, DWORD ident, void* data, const char* format, ...);
    void Process();
    void AnalyzeReturnQuery(SQLMsg* message);

private:
    SQLMsg* PopResult();
    CAsyncSQL m_sql;
    CAsyncSQL m_sql_direct;
    bool m_bIsConnect;
};

#endif
