// Complete 2009 CLIENT_DESC members; method bodies recovered separately.
#ifndef MCNCC_GAME_DESC_CLIENT_H
#define MCNCC_GAME_DESC_CLIENT_H
#include "desc.h"
class CLIENT_DESC : public DESC {
public:
    CLIENT_DESC();
    virtual ~CLIENT_DESC();
    virtual BYTE GetType();
    virtual void Destroy();
    virtual void SetPhase(int phase);
    bool Connect(int iPhaseWhenSucceed = 0);
    void Setup(LPFDWATCH fdw, const char* host, WORD port);
    void SetRetryWhenClosed(bool retry);
    bool IsRetryWhenClosed();
    void DBPacketHeader(BYTE header, DWORD handle, DWORD size);
    void DBPacket(BYTE header, DWORD handle, const void* data, DWORD size);
    void Packet(const void* data, int size);
protected:
    int m_iPhaseWhenSucceed;
    bool m_bRetryWhenClosed;
};
extern CLIENT_DESC* db_clientdesc;
#endif
