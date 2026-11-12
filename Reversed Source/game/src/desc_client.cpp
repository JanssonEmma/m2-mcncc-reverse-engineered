// Partial old client descriptor: lifecycle entry, type/retry and DB wire output.
#include "desc_client.h"
#include "config.h"
#include "db.h"
#include "desc_manager.h"
#include "guild_manager.h"
#include "input.h"
#include "p2p.h"
#include "party.h"
#include <cstring>
#include <vector>
#include "protocol.h"
#include "utils.h"
#include "../../libthecore/include/socket.h"
#include "../../libthecore/include/log.h"
CLIENT_DESC::CLIENT_DESC() : m_iPhaseWhenSucceed(0), m_bRetryWhenClosed(false) {}
CLIENT_DESC::~CLIENT_DESC() { Destroy(); }
BYTE CLIENT_DESC::GetType() { return 1; }
void CLIENT_DESC::SetRetryWhenClosed(bool retry) { m_bRetryWhenClosed=retry; }
void CLIENT_DESC::DBPacketHeader(BYTE header,DWORD handle,DWORD size) {
    const TPacketDBHeader frame = {
        header, static_cast<std::uint32_t>(handle), static_cast<std::uint32_t>(size)
    };
    buffer_write(m_lpOutputBuffer, &frame, sizeof(frame));
}
void CLIENT_DESC::DBPacket(BYTE header,DWORD handle,const void* data,DWORD size) {
    sys_log(1,"DB_PACKET: header %d handle %d size %d buffer_size %d",
            static_cast<unsigned int>(header), static_cast<int>(handle), static_cast<int>(size),
            buffer_size(m_lpOutputBuffer));
    DBPacketHeader(header,handle,size);
    if(data) buffer_write(m_lpOutputBuffer,data,size);
}

// ELF object083258e4,size4; pointer type corroborated by reference/calls.
CLIENT_DESC* db_clientdesc=0;
// Original BSS object at 083258e8, adjacent to db_clientdesc.
CLIENT_DESC* g_pkAuthMasterDesc=0;
// Original 083258ec and 083258f0 BSS objects.
CLIENT_DESC* g_NetmarbleDBDesc = nullptr;
CLIENT_DESC* g_TeenDesc = nullptr;

void CLIENT_DESC::Packet(const void* data,int size) { buffer_write(m_lpOutputBuffer,data,size); }
void CLIENT_DESC::Setup(LPFDWATCH watcher,const char* host,WORD port) {
    m_lpFdw=watcher; m_stHost=host; m_wPort=port;
    m_lpOutputBuffer=buffer_new(0,0x100000);
    m_lpInputBuffer=buffer_new(0,0x100000);
    m_iMinInputBufferLen=0x100000; m_sock=-1;
}
bool CLIENT_DESC::Connect(int phase) {
    if(phase) m_iPhaseWhenSucceed=phase;
    if(static_cast<int>(static_cast<unsigned int>(get_global_time())-static_cast<unsigned int>(m_LastTryTime))>2) {
        m_LastTryTime=get_global_time();
        if(m_sock==-1) {
            sys_log(0,"SYSTEM: Trying to connect to %s:%d",m_stHost.c_str(),static_cast<unsigned int>(m_wPort));
            m_sock=socket_connect(m_stHost.c_str(),m_wPort);
            if(m_sock!=-1) {
                sys_log(0,"SYSTEM: connected to server (fd %d, ptr %p)",m_sock,this);
                fdwatch_add_fd(m_lpFdw,m_sock,this,1,0);
                fdwatch_add_fd(m_lpFdw,m_sock,this,2,0);
                SetPhase(m_iPhaseWhenSucceed); return true;
            }
            SetPhase(7);
        }
    }
    return false;
}

void CLIENT_DESC::SetPhase(int phase)
{
    static CInputDB inputDB;
    static CInputP2P inputP2P;
    static CInputTeen inputTeen;
    static CInputNetmarble inputNetmarble;
    static bool sentBoot = false;
    static bool sentNetmarbleHandshake = false;
    if (m_iPhase == 11 && phase != 11)
        inputNetmarble.Reset();

    switch (phase) {
        case 0:
            m_pInputProcessor = 0;
            break;
        case 7:
            sys_log(1, "PHASE_CLIENT_DESC::CONNECTING");
            m_pInputProcessor = 0;
            break;
        case 8: {
            sys_log(1, "PHASE_DBCLIENT");
            if (!g_bAuthServer && !sentBoot) {
                sentBoot = true;
                TPacketGDBoot boot;
                for (int i = 0; i < 2; ++i)
                    boot.dwItemIDRange[i] = g_dwItemIDRange[i];
                std::memcpy(boot.szIP, g_szPublicIP, sizeof(boot.szIP));
                DBPacket(9, 0, &boot, sizeof(boot));
            }

            TPacketGDSetup setup = {};
            std::strncpy(setup.szPublicIP, g_szPublicIP, sizeof(setup.szPublicIP) - 1);
            setup.bAuthServer = g_bAuthServer;
            std::vector<TPacketLoginOnSetup> logins;
            if (!g_bAuthServer) {
                setup.bChannel = g_bChannel;
                setup.wListenPort = mother_port;
                setup.wP2PPort = p2p_port;
                map_allow_copy(setup.abMaps, sizeof(setup.abMaps));

                for (DESC* desc : DESC_MANAGER::instance().GetClientSet()) {
                    const TAccountTable& account = desc->GetAccountTable();
                    if (!account.id)
                        continue;

                    TPacketLoginOnSetup login = {};
                    login.dwID = account.id;
                    std::strncpy(login.szLogin, account.login, sizeof(login.szLogin) - 1);
                    std::strncpy(login.szSocialID, account.social_id, sizeof(login.szSocialID) - 1);
                    std::strncpy(login.szHost, desc->GetHostName(), sizeof(login.szHost) - 1);
                    login.dwLoginKey = desc->GetLoginKey();
                    const DWORD* key = desc->GetDecryptionKey();
                    for (int i = 0; i < 4; ++i)
                        login.adwClientKey[i] = key[i];
                    logins.push_back(login);
                }
                setup.dwLoginCount = logins.size();
            }
            std::vector<BYTE> payload(sizeof(setup) + logins.size() * sizeof(TPacketLoginOnSetup));
            std::memcpy(payload.data(), &setup, sizeof(setup));
            if (!logins.empty())
                std::memcpy(payload.data() + sizeof(setup), logins.data(), logins.size() * sizeof(TPacketLoginOnSetup));
            if (!g_bAuthServer) {
                sys_log(0, "DB_SETUP current user %d size %d", static_cast<int>(setup.dwLoginCount),
                        static_cast<int>(payload.size()));
                CPartyManager::instance().EnablePCParty();
            }
            DBPacket(255, 0, payload.data(), payload.size());
            m_pInputProcessor = &inputDB;
            break;
        }
        case 9:
            sys_log(1, "PHASE_P2P");
            m_pInputProcessor = &inputP2P;
            break;
        case 11:
            if (!sentNetmarbleHandshake) {
                inputNetmarble.SendFirstPacket(this);
                sentNetmarbleHandshake = true;
            }
            m_pInputProcessor = &inputNetmarble;
            break;
        case 12:
            inputTeen.SetStep(0);
            m_pInputProcessor = &inputTeen;
            break;
    }
    m_iPhase = phase;
}

bool CLIENT_DESC::IsRetryWhenClosed()
{
    return m_bRetryWhenClosed;
}

void CLIENT_DESC::Destroy()
{
    if (m_sock == -1)
        return;

    P2P_MANAGER::instance().UnregisterConnector(this);
    if (this == db_clientdesc) {
        CPartyManager::instance().DeleteAllParty();
        CPartyManager::instance().DisablePCParty();
        CGuildManager::instance().StopAllGuildWar();
        DBManager::instance().StopAllBilling();
    }
    fdwatch_del_fd(m_lpFdw, m_sock);
    sys_log(0, "SYSTEM: closing client socket. DESC #%d", m_sock);
    socket_close(m_sock);
    m_sock = -1;
}
