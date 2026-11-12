// Partial API, complete old member layout. Unrecovered nonvirtual methods omitted.
// Target DESC is580 bytes with GCC3.3's four-byte std::string and FreeBSD32 types.
#ifndef MCNCC_GAME_DESC_H
#define MCNCC_GAME_DESC_H
#include <string>
#include <cstdio>
#include <ctime>
#include <netinet/in.h>
#include "../../common/tables.h"
#include "../../libthecore/include/buffer.h"
#include "../../libthecore/include/fdwatch.h"
#include "event.h"
#include "../../libthecore/include/utils.h"
class CInputProcessor;
class CHARACTER;
typedef CHARACTER* LPCHARACTER;
class DESC;
// Old 12-byte login-key record; inlined behavior corroborated in callers.
class CLoginKey {
public:
    CLoginKey(DWORD key, DESC* desc) : m_dwKey(key), m_dwExpireTime(0), m_pkDesc(desc) {}
    void Expire() { m_dwExpireTime = get_dword_time(); m_pkDesc = 0; }
    operator DWORD() const { return m_dwKey; }
    DWORD m_dwKey;
    DWORD m_dwExpireTime;
    DESC* m_pkDesc;
};
class DESC {
public:
    DESC();
    virtual ~DESC();
    virtual BYTE GetType();
    virtual void Destroy();
    virtual void SetPhase(int phase);
    bool Setup(LPFDWATCH watcher, socket_t socket, const sockaddr_in& peer, DWORD handle, DWORD handshake);
    int ProcessOutput();
    LPBUFFER GetOutputBuffer() { return m_lpOutputBuffer; }
    int ProcessInput();
    void FlushOutput();
    BYTE GetSequence();
    void SetNextSequence();
    void Handshake(DWORD handshake);
    void TrySyncTime(DWORD now, long delta);
    bool SyncTime(DWORD time, long delta, bool infinite_retry);
    void UDPGrant(const sockaddr_in& address);
    void SetSecurityKey(const DWORD* key);
    void AssembleCRCMagicCube(BYTE process_piece, BYTE file_piece);
    TAccountTable& GetAccountTable() { return m_accountTable; }
    void BindAccountTable(TAccountTable* table);
    void SendLoginSuccessPacket();
    void SendRunupMatrixizPacket(const char* quiz);
    const char* GetHostName() { return m_stHost.c_str(); }
    const char* GetClientVersion() { return m_stClientVersion.c_str(); }
    void SetClientVersion(const char* version) { m_stClientVersion = version; }
    WORD GetP2PPort() { return m_wP2PPort; }
    void DisconnectOfSameLogin();
    bool DelayedDisconnect(int seconds);
    friend long disconnect_event(LPEVENT event);
    void SetP2P(const char* host, WORD port, BYTE channel) {
        m_stP2PHost.assign(host);
        m_wP2PPort=port;
        m_bP2PChannel=channel;
    }
    DWORD GetHandshake() { return m_dwHandshake; }
    const DWORD* GetEncryptionKey() { return m_adwEncryptionKey; }
    const DWORD* GetDecryptionKey() { return m_adwDecryptionKey; }
    const sockaddr_in& GetAddr() { return m_SockAddr; }
    const sockaddr_in& GetUDPAddr() { return m_UDPSockAddr; }
    socket_t GetSocket() { return m_sock; }
    bool IsPhase(int phase) { return m_iPhase==phase; }
    CInputProcessor* GetInputProcessor() { return m_pInputProcessor; }
    void BufferedPacket(const void* data, int size);
    void Packet(const void* data, int size);
    void LargePacket(const void* data, int size);
    void Log(const char* format, ...);
    DWORD GetClientTime();
    BYTE GetEmpire();
    void SetRelay(const char* name);
    void BindCharacter(CHARACTER* ch);
    LPCHARACTER GetCharacter() { return m_lpCharacter; }
    void SetAdminMode();
    bool IsAdminMode();
    bool IsPong();
    void SetPong(bool pong);
    void SetMatrixCardRowsAndColumns(unsigned long rows, unsigned long cols);
    unsigned long GetMatrixRows();
    unsigned long GetMatrixCols();
    bool CheckMatrixTryCount();
    void SetPasspodPending(bool pending) { m_bPasspodPending = pending; }
    bool IsPasspodPending() const { return m_bPasspodPending; }
    void SetLoginKey(DWORD key);
    void SetLoginKey(CLoginKey* key);
    DWORD GetLoginKey();
    friend class DBManager;
    friend class CInputAuth;
    DWORD GetHandle() { return m_dwHandle; }
    void SetBillingExpireSecond(DWORD seconds);
    DWORD GetBillingExpireSecond();
protected:
    void Initialize();
    CInputProcessor* m_pInputProcessor;
    LPFDWATCH m_lpFdw;
    socket_t m_sock;
    int m_iPhase;
    DWORD m_dwHandle;
    std::string m_stHost;
    WORD m_wPort;
    time_t m_LastTryTime;
    LPBUFFER m_lpInputBuffer;
    int m_iMinInputBufferLen;
    DWORD m_dwHandshake;
    DWORD m_dwHandshakeSentTime;
    DWORD m_dwClientTime;
    int m_iHandshakeRetry;
    LPBUFFER m_lpBufferedOutputBuffer;
    LPBUFFER m_lpOutputBuffer;
    LPEVENT m_pkPingEvent;
    CHARACTER* m_lpCharacter;
    TAccountTable m_accountTable;
    struct sockaddr_in m_SockAddr;
    struct sockaddr_in m_UDPSockAddr;
    FILE* m_pLogFile;
    std::string m_stRelayName;
    bool m_bEncrypted;
    std::string m_stP2PHost;
    WORD m_wP2PPort;
    BYTE m_bP2PChannel;
    bool m_bAdminMode;
    bool m_bPong;
    int m_iCurrentSequence;
    DWORD m_dwMatrixRows;
    DWORD m_dwMatrixCols;
    BYTE m_bMatrixTryCount;
    bool m_bPasspodPending;
    CLoginKey* m_pkLoginKey;
    DWORD m_dwLoginKey;
    DWORD m_adwDecryptionKey[4];
    DWORD m_adwEncryptionKey[4];
    BYTE m_bCRCMagicCubeIdx;
    DWORD m_dwProcCRC;
    DWORD m_dwFileCRC;
    bool m_bHackCRCQuery;
    DWORD m_dwBillingExpireSecond;
    std::string m_stClientVersion;
    std::string m_stMatrixCode;
    std::string m_Login;
    int m_outtime;
    int m_playtime;
    int m_offtime;
public:
    LPEVENT m_pkDisconnectEvent;
protected:
    std::string m_stMatrixCardID;
    std::string m_stMatrixQuiz;
};
#endif
