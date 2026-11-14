#include "../../libthecore/include/utils.h"
#include "desc.h"
#include "char.h"
#include "desc_client.h"
#include "desc_p2p.h"
#include "config.h"
#include "ip_ban.h"
#include <cstring>
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/socket.h"
#include <arpa/inet.h>
// Partial old manager: initialization, registries and lookups.
#include "desc_manager.h"
#include "p2p.h"
#include "crc32.h"
#include "utils.h"
#include <cstdint>
#include <algorithm>

// 0815 descriptor registry teardown.
void DESC_MANAGER::DestroyDesc(DESC* desc, bool eraseFromSet)
{
    if (eraseFromSet)
        m_set_pkDesc.erase(desc);
    if (desc->GetHandshake())
        m_map_handshake.erase(desc->GetHandshake());
    if (desc->GetHandle())
        m_map_handle.erase(desc->GetHandle());
    else
        m_set_pkClientDesc.erase(desc);
    delete desc;
    --m_iSocketsConnected;
}

namespace {
struct FuncWho
{
    int& localUserCount;
    int* empireUserCount;

    void operator()(LPDESC desc)
    {
        if (!desc->GetCharacter())
            return;

        ++localUserCount;
        const BYTE empire = desc->GetEmpire();
        if (empire < 4)
            ++empireUserCount[empire];
    }
};
}

void DESC_MANAGER::UpdateLocalUserCount()
{
    m_iLocalUserCount = 0;
    std::fill_n(m_aiEmpireUserCount, 4, 0);
    FuncWho count{m_iLocalUserCount, m_aiEmpireUserCount};
    for (DESC* desc : GetClientSet())
        count(desc);
    for (int empire = 1; empire <= 3; ++empire)
        m_aiEmpireUserCount[empire] += P2P_MANAGER::instance().GetEmpireUserCount(empire);
}

void DESC_MANAGER::GetUserCount(int& total, int** empireUserCount, int& localCount)
{
    *empireUserCount = m_aiEmpireUserCount;
    total = P2P_MANAGER::instance().GetCount() + m_iLocalUserCount;
    localCount = m_iLocalUserCount;
}

DESC_MANAGER::DESC_MANAGER() : m_map_loginName(100) { Initialize(); }
DESC_MANAGER::~DESC_MANAGER()
{
    Destroy();
}

// 08158c40
void DESC_MANAGER::DestroyClosed()
{
    for (auto it = m_set_pkDesc.begin(); it != m_set_pkDesc.end(); ) {
        const auto entry = it++;
        DESC* desc = *entry;
        if (!desc->IsPhase(0))
            continue;

        if (desc->GetType() == 1 && static_cast<CLIENT_DESC*>(desc)->IsRetryWhenClosed()) {
            desc->Destroy();
            continue;
        }

        DestroyDesc(desc, false);
        m_set_pkDesc.erase(entry);
    }
}

// 08158540
void DESC_MANAGER::Destroy()
{
    for (auto it = m_set_pkDesc.begin(); it != m_set_pkDesc.end(); ) {
        const auto entry = it++;
        DESC* desc = *entry;
        if (desc->GetType() == 1 || desc->IsPhase(9))
            continue;

        DestroyDesc(desc, false);
        m_set_pkDesc.erase(entry);
    }

    for (auto it = m_set_pkDesc.begin(); it != m_set_pkDesc.end(); ) {
        const auto entry = it++;
        DestroyDesc(*entry, false);
        m_set_pkDesc.erase(entry);
    }
    m_set_pkClientDesc.clear();
    m_map_loginName.clear();
    m_map_handle.clear();
    Initialize();
}

void DESC_MANAGER::Initialize() {
    m_iSocketsConnected=0; m_iHandleCount=0; m_iLocalUserCount=0;
    for(int i=0;i<4;++i) m_aiEmpireUserCount[i]=0;
    m_bDisconnectInvalidCRC=false;
}
void DESC_MANAGER::ConnectAccount(const std::string& login,DESC* desc) {
    m_map_loginName.insert(std::make_pair(login,desc));
}
void DESC_MANAGER::DisconnectAccount(const std::string& login) { m_map_loginName.erase(login); }
// 08158e70
DESC* DESC_MANAGER::FindByCharacterName(const char* name)
{
    for (DESC* desc : m_set_pkDesc) {
        CHARACTER* character = desc->GetCharacter();
        if (character && std::strcmp(character->GetName(), name) == 0)
            return desc;
    }
    return nullptr;
}

DESC* DESC_MANAGER::FindByLoginName(const std::string& login) {
    DESC_LOGINNAME_MAP::iterator it=m_map_loginName.find(login);
    return it==m_map_loginName.end()?0:it->second;
}
DESC* DESC_MANAGER::FindByHandle(DWORD handle) {
    DESC_HANDLE_MAP::iterator it=m_map_handle.find(static_cast<int>(handle));
    return it==m_map_handle.end()?0:it->second;
}
DESC* DESC_MANAGER::FindByHandshake(DWORD handshake) {
    DESC_HANDSHAKE_MAP::iterator it=m_map_handshake.find(handshake);
    return it==m_map_handshake.end()?0:it->second;
}
const DESC_MANAGER::DESC_SET& DESC_MANAGER::GetClientSet() { return m_set_pkDesc; }

// 08159240
DWORD DESC_MANAGER::MakeRandomKey(DWORD handle)
{
    const DWORD key = thecore_random();
    m_map_handle_random_key.emplace(handle, key);
    return key;
}

// 081592a0
bool DESC_MANAGER::GetRandomKey(DWORD handle, DWORD* randomKey)
{
    const auto key = m_map_handle_random_key.find(handle);
    if (key == m_map_handle_random_key.end())
        return false;

    *randomKey = key->second;
    return true;
}

// 081592f0
DESC* DESC_MANAGER::FindByLoginKey(DWORD key)
{
    std::map<DWORD, CLoginKey*>::iterator login = m_map_pkLoginKey.find(key);
    return login != m_map_pkLoginKey.end() ? login->second->m_pkDesc : 0;
}

// 08159340
DWORD DESC_MANAGER::CreateLoginKey(DESC* desc)
{
    DWORD key;
    do {
        key = number_ex(1, 0x7fffffff, "desc_manager.cpp", 0x1b7);
    } while (m_map_pkLoginKey.find(key) != m_map_pkLoginKey.end());

    CLoginKey* login = new CLoginKey(key, desc);
    desc->SetLoginKey(login);
    m_map_pkLoginKey.insert(std::make_pair(key, login));
    return key;
}

// 08159420
void DESC_MANAGER::ProcessExpiredLoginKey()
{
    const unsigned int now = static_cast<unsigned int>(get_dword_time());
    std::map<DWORD, CLoginKey*>::iterator next = m_map_pkLoginKey.begin();
    while (next != m_map_pkLoginKey.end()) {
        std::map<DWORD, CLoginKey*>::iterator entry = next++;
        CLoginKey* login = entry->second;
        const DWORD expired = login->m_dwExpireTime;
        if (expired && now - static_cast<unsigned int>(expired) > 60000u) {
            m_map_pkLoginKey.erase(entry);
            delete login;
        }
    }
}

CLIENT_DESC* DESC_MANAGER::CreateConnectionDesc(struct fdwatch* watcher, const char* host, WORD port,
                                               int phase, bool retry)
{
    CLIENT_DESC* desc = new CLIENT_DESC;
    desc->Setup(watcher, host, port);
    desc->Connect(phase);
    desc->SetRetryWhenClosed(retry);
    m_set_pkDesc.insert(desc);
    m_set_pkClientDesc.insert(desc);
    ++m_iSocketsConnected;
    return desc;
}

namespace {
struct ValidIP {
    const char* prefix;
    BYTE network;
    BYTE mask;
};

const ValidIP admin_ip[] = {{"210.123.10", 128, 128}, {"\n", 0, 0}};

bool IsValidIP(const ValidIP* table, const char* host)
{
    for (; *table->prefix != '\n'; ++table) {
        for (int suffix = 255 - table->mask; suffix >= 0; --suffix) {
            char candidate[16];
            std::snprintf(candidate, sizeof(candidate), "%s.%d", table->prefix, table->network + suffix);
            if (!std::strcmp(candidate, host))
                return true;
        }
    }
    return false;
}
}

// Original inbound client admission and descriptor registration.
DESC* DESC_MANAGER::AcceptDesc(LPFDWATCH watcher, socket_t listener)
{
    sockaddr_in peer = {};
    const socket_t socket = socket_accept(listener, &peer);
    if (socket == -1)
        return nullptr;

    const char* host = inet_ntoa(peer.sin_addr);
    if (g_bAuthServer) {
        if (IsBanIP(peer.sin_addr)) {
            sys_log(0, "connection from %s was banned.", host);
            socket_close(socket);
            return nullptr;
        }
        if (test_server)
            sys_log(0, "you are good ip");
    }
    if (!IsValidIP(admin_ip, host) && m_iSocketsConnected >= 4096) {
        sys_err("max connection reached. MAX_ALLOW_USER = %d", 4096);
        socket_close(socket);
        return nullptr;
    }

    DESC* desc = new DESC;
    const DWORD handshake = CreateHandshake();
    const DWORD handle = ++m_iHandleCount;
    if (!desc->Setup(watcher, socket, peer, handle, handshake)) {
        socket_close(socket);
        delete desc;
        return nullptr;
    }

    m_map_handshake.emplace(handshake, desc);
    m_map_handle.emplace(handle, desc);
    m_set_pkDesc.insert(desc);
    ++m_iSocketsConnected;
    return desc;
}

DESC* DESC_MANAGER::AcceptP2PDesc(LPFDWATCH watcher, socket_t listener)
{
    sockaddr_in peer = {};
    const socket_t socket = socket_accept(listener, &peer);
    if (socket == -1)
        return nullptr;

    DESC_P2P* desc = new DESC_P2P;
    if (!desc->Setup(watcher, socket, inet_ntoa(peer.sin_addr))) {
        sys_err("DESC_MANAGER::AcceptP2PDesc : Setup failed");
        socket_close(socket);
        delete desc;
        return nullptr;
    }

    m_set_pkDesc.insert(desc);
    ++m_iSocketsConnected;
    P2P_MANAGER::instance().RegisterAcceptor(desc);
    return desc;
}

// 081586f0
DWORD DESC_MANAGER::CreateHandshake()
{
    DWORD handshake;
    do {
        const std::uint32_t input[] = {
            static_cast<std::uint32_t>(thecore_random() & 0xfffff),
            static_cast<std::uint32_t>(get_global_time())
        };
        handshake = get_crc32(reinterpret_cast<const char*>(input), sizeof(input));
    } while (!handshake || m_map_handshake.find(handshake) != m_map_handshake.end());
    return handshake;
}

void DESC_MANAGER::TryConnect()
{
    for (DESC* desc : m_set_pkClientDesc)
        static_cast<CLIENT_DESC*>(desc)->Connect();
}
