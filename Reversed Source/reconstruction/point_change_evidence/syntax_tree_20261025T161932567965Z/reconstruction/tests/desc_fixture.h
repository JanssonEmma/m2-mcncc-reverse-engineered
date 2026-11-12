// Test-only lifecycle and protected-field access; never production definitions.
#ifndef MCNCC_TEST_DESC_FIXTURE_H
#define MCNCC_TEST_DESC_FIXTURE_H
#include "../../game/src/desc.h"
#include <cassert>
#include <cstring>

extern int desc_test_clock_calls;
extern int desc_test_phase_calls;
class DescOutputProbe : public DESC {
public:
    virtual ~DescOutputProbe() {
        // Detach borrowed resources and provide both owned buffers required by old teardown.
        m_lpOutputBuffer=buffer_new(0,1); m_lpInputBuffer=buffer_new(0,1); m_sock=-1; m_lpFdw=0;
        m_pkLoginKey=0; m_lpCharacter=0; m_pLogFile=0;
        m_pkPingEvent=0; m_pkDisconnectEvent=0;
        std::memset(&m_accountTable,0,sizeof m_accountTable);
    }
    virtual void SetPhase(int phase) { ++desc_test_phase_calls; DESC::SetPhase(phase); }
    void CheckInitial() {
        assert(m_pInputProcessor == 0);
        assert(m_lpFdw == 0);
        assert(m_sock == -1);
        assert(m_iPhase == 0);
        assert(m_dwHandle == 0);
        assert(m_iHandshakeRetry == 0);
        assert(m_wPort == 0);
        assert(m_LastTryTime == 0);
        assert(m_lpInputBuffer == 0);
        assert(m_iMinInputBufferLen == 0);
        assert(m_dwHandshake == 0);
        assert(m_lpOutputBuffer == 0);
        assert(m_lpCharacter == 0);
        assert(m_lpBufferedOutputBuffer == 0);
        assert(m_iCurrentSequence == 0);
        assert(m_dwMatrixCols == 0);
        assert(m_dwMatrixRows == 0);
        assert(m_pkLoginKey == 0);
        assert(m_pLogFile == 0);
        assert(m_dwProcCRC == 0);
        assert(m_pkPingEvent == 0);
        assert(m_bEncrypted == false);
        assert(m_pkDisconnectEvent == 0);
        assert(m_wP2PPort == 0);
        assert(m_bAdminMode == false);
        assert(m_bPong == true);
        assert(m_bMatrixTryCount == 0);
        assert(m_bCRCMagicCubeIdx == 0);
        assert(m_dwFileCRC == 0);
        assert(m_bHackCRCQuery == false);
        assert(m_dwBillingExpireSecond == 0);
        assert(m_dwLoginKey == 0);
        const unsigned char* account = reinterpret_cast<const unsigned char*>(&m_accountTable);
        for (size_t i=0;i<sizeof m_accountTable;++i) assert(account[i]==0);
        const unsigned char* udp = reinterpret_cast<const unsigned char*>(&m_UDPSockAddr);
        for (size_t i=0;i<sizeof m_UDPSockAddr;++i) assert(udp[i]==0);
    }
    void CheckReinitialize() {
        m_pInputProcessor = reinterpret_cast<CInputProcessor*>(this);
        m_lpFdw = reinterpret_cast<LPFDWATCH>(this);
        m_sock = 7;
        m_iPhase = 7;
        m_dwHandle = 7;
        m_iHandshakeRetry = 7;
        m_wPort = 7;
        m_LastTryTime = 7;
        m_lpInputBuffer = reinterpret_cast<LPBUFFER>(this);
        m_iMinInputBufferLen = 7;
        m_dwHandshake = 7;
        m_lpOutputBuffer = reinterpret_cast<LPBUFFER>(this);
        m_lpCharacter = reinterpret_cast<CHARACTER*>(this);
        m_lpBufferedOutputBuffer = reinterpret_cast<LPBUFFER>(this);
        m_iCurrentSequence = 7;
        m_dwMatrixCols = 7;
        m_dwMatrixRows = 7;
        m_pkLoginKey = reinterpret_cast<CLoginKey*>(this);
        m_pLogFile = reinterpret_cast<FILE*>(this);
        m_dwProcCRC = 7;
        m_pkPingEvent = reinterpret_cast<LPEVENT>(this);
        m_bEncrypted = true;
        m_pkDisconnectEvent = reinterpret_cast<LPEVENT>(this);
        m_wP2PPort = 7;
        m_bAdminMode = true;
        m_bPong = false;
        m_bMatrixTryCount = 7;
        m_bCRCMagicCubeIdx = 7;
        m_dwFileCRC = 7;
        m_bHackCRCQuery = true;
        m_dwBillingExpireSecond = 7;
        m_dwLoginKey = 7;
        std::memset(&m_accountTable, 0x5a, sizeof m_accountTable);
        std::memset(&m_UDPSockAddr, 0x5a, sizeof m_UDPSockAddr);
        std::memset(&m_SockAddr, 0x5a, sizeof m_SockAddr);
        m_dwHandshakeSentTime=123; m_dwClientTime=456; m_bP2PChannel=9;
        m_outtime=11; m_playtime=22; m_offtime=33;
        for(int i=0;i<4;++i) { m_adwEncryptionKey[i]=100+i; m_adwDecryptionKey[i]=200+i; }
        m_stHost="retained";
        m_stRelayName="retained";
        m_stP2PHost="retained";
        m_stClientVersion="retained";
        m_stMatrixCode="retained";
        m_Login="retained";
        m_stMatrixCardID="retained";
        m_stMatrixQuiz="retained";
        Initialize(); CheckInitial();
        const unsigned char* tcp = reinterpret_cast<const unsigned char*>(&m_SockAddr);
        for(size_t i=0;i<sizeof m_SockAddr;++i) assert(tcp[i]==0x5a);
        assert(m_dwHandshakeSentTime==123 && m_dwClientTime==456 && m_bP2PChannel==9);
        assert(m_outtime==11 && m_playtime==22 && m_offtime==33);
        for(int i=0;i<4;++i) { assert(m_adwEncryptionKey[i]==static_cast<DWORD>(100+i)); assert(m_adwDecryptionKey[i]==static_cast<DWORD>(200+i)); }
        assert(m_stHost=="retained");
        assert(m_stRelayName=="retained");
        assert(m_stP2PHost=="retained");
        assert(m_stClientVersion=="retained");
        assert(m_stMatrixCode=="retained");
        assert(m_Login=="retained");
        assert(m_stMatrixCardID=="retained");
        assert(m_stMatrixQuiz=="retained");
    }
    void CheckHelpers() {
        // Byte overflow is intentional: after 255 attempts the counter returns to zero.
        m_bMatrixTryCount=0;
        for (int i=1;i<=258;++i) assert(CheckMatrixTryCount()==((i%256)<3));
        Initialize();
        const int before=desc_test_clock_calls;
        Log("ignored %d", 1);
        assert(desc_test_clock_calls==before);
        m_pLogFile=std::tmpfile(); assert(m_pLogFile);
        const std::string payload(2048, 'x');
        Log("value=%d %s", -17, payload.c_str());
        assert(desc_test_clock_calls==before+1);
        time_t fixed=1700000000;
        const struct tm stamp=*std::localtime(&fixed);
        char prefix[80];
        std::sprintf(prefix,"%02d %02d %02d:%02d:%02d | value=-17 ",
            stamp.tm_mon+1,stamp.tm_mday,stamp.tm_hour,stamp.tm_min,stamp.tm_sec);
        std::rewind(m_pLogFile);
        char text[2200]; const size_t n=std::fread(text,1,sizeof(text),m_pLogFile);
        const std::string expected=std::string(prefix)+payload+"\n";
        assert(std::string(text,n)==expected);
        std::fclose(m_pLogFile); m_pLogFile=0;
    }
    void SetPacketState(int phase, bool encrypted=false) {
        m_iPhase=phase; m_bEncrypted=encrypted;
        for(int i=0;i<4;++i) m_adwEncryptionKey[i]=i+1;
    }
    int PacketPhase() const { return m_iPhase; }
    LPBUFFER BufferedOutput() const { return m_lpBufferedOutputBuffer; }
    bool HasRelay() const { return !m_stRelayName.empty(); }
    void ReleaseBuffered() {
        if(m_lpBufferedOutputBuffer) buffer_delete(m_lpBufferedOutputBuffer);
        m_lpBufferedOutputBuffer=0;
    }
    void Attach(LPFDWATCH watcher, socket_t socket, LPBUFFER output) {
        m_lpFdw = watcher;
        m_sock = socket;
        m_lpOutputBuffer = output;
    }
};
#endif
