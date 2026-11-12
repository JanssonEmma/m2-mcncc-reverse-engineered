#include "input.h"
#include "config.h"
#include "db.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "../../libthecore/include/log.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>

struct NetmarbleJP_ConnectorInfo {
    DWORD desc_key;
    DWORD client_keys[4];
};

namespace {
constexpr int packetSize = 256;
constexpr int userInfoSize = 206;
constexpr int loginOffset = 12;
constexpr int passwordOffset = 44;

std::map<std::string, NetmarbleJP_ConnectorInfo> pendingLogins;

void WriteWord(char* target, std::uint32_t value)
{
    std::memcpy(target, &value, sizeof(value));
}

} // namespace

// 08111f90
bool NetmarbleJP_AppendConnector(const char* login, DWORD key, const DWORD* clientKeys)
{
    NetmarbleJP_ConnectorInfo connector{};
    connector.desc_key = key;
    std::memcpy(connector.client_keys, clientKeys, sizeof(connector.client_keys));
    pendingLogins[login] = connector;
    return true;
}

// 081121d0
bool NetmarbleJP_OnRecvResult(const NetmarbleJP_ConnectorInfo& connector, int header,
                             const char* login)
{
    DESC* desc = DESC_MANAGER::instance().FindByLoginKey(connector.desc_key);
    sys_log(0, "NETMARBLE_JP::FindLoginKey key(%lu) desc(%p) header(%d) login(%s)",
            connector.desc_key, static_cast<void*>(desc), header, login);
    if (!desc)
        return false;
    if (header != 0x41e) {
        LoginFailure(desc, header == 0x644 ? "WRONGPWD" : "NOTAVAIL");
        return false;
    }

    auto* request = new TPacketCGLogin3{};
    std::strncpy(request->login, login, sizeof(request->login) - 1);
    for (int i = 0; i < 4; ++i)
        request->adwClientKey[i] = static_cast<std::uint32_t>(connector.client_keys[i]);
    NetmarbleJP_QueryAccount(connector.desc_key, request);
    return false;
}

void NetmarbleJP_QueryAccount(DWORD key, TPacketCGLogin3* request)
{
    DBManager& db = DBManager::instance();
    char escapedLogin[61];
    db.EscapeString(escapedLogin, request->login, std::strlen(request->login));
    db.ReturnQuery(2, key, request,
                   "SELECT PASSWORD('...'), password, securitycode, social_id, id, status, availDt - NOW() > 0,"
                   "UNIX_TIMESTAMP(silver_expire),UNIX_TIMESTAMP(gold_expire),UNIX_TIMESTAMP(safebox_expire),"
                   "UNIX_TIMESTAMP(fish_mind_expire),UNIX_TIMESTAMP(marriage_fast_expire),"
                   "UNIX_TIMESTAMP(money_drop_rate_expire) FROM account WHERE login='%s'", escapedLogin);
}

bool NetmarbleJP_RequestLogin(DESC* desc, const TPacketCGLogin3& request)
{
    // Relay replies identify the login only. Keep one request per login in flight.
    if (pendingLogins.find(request.login) != pendingLogins.end())
        return false;
    const DWORD key = DESC_MANAGER::instance().CreateLoginKey(desc);
    sys_log(0, "InputAuth::Login : key %lu login %s", key, request.login);
    DWORD clientKeys[4];
    for (int i = 0; i < 4; ++i)
        clientKeys[i] = request.adwClientKey[i];
    NetmarbleJP_AppendConnector(request.login, key, clientKeys);

    char packet[packetSize]{};
    WriteWord(packet, 0x41d);
    WriteWord(packet + 4, userInfoSize);
    std::memcpy(packet + loginOffset, request.login, std::min<std::size_t>(16, std::strlen(request.login)));
    std::memcpy(packet + passwordOffset, request.passwd, std::min<std::size_t>(10, std::strlen(request.passwd)));
    sys_log(0, "NETMARBLE_JP::SendLoginPacket(%s:%d:%d)", request.login, userInfoSize, packetSize);
    g_NetmarbleDBDesc->Packet(packet, sizeof(packet));
    return true;
}

void CInputNetmarble::SendFirstPacket(DESC* desc)
{
    char packet[packetSize]{};
    WriteWord(packet, 1001);
    WriteWord(packet + 4, 8);
    WriteWord(packet + 8, 1022);
    WriteWord(packet + 12, 1);
    sys_log(0, "NETMARBLE_JP::SendFirstPacket(%d)", packetSize);
    desc->Packet(packet, sizeof(packet));
}

void CInputNetmarble::Reset()
{
    pendingLogins.clear();
}

// 08112400
bool CInputNetmarble::Process(DESC* desc, const void* raw, int bytes, int& consumed)
{
    const char* data = static_cast<const char*>(raw);
    m_iBufferLeft = bytes;
    while (m_iBufferLeft >= packetSize) {
        std::int32_t header;
        std::memcpy(&header, data, sizeof(header));
        const char* login = data + loginOffset;
        const char* end = static_cast<const char*>(std::memchr(login, 0, 16));
        const std::string name(login, end ? end : login + 16);
        sys_log(0, "NETMARBLE_JP::RecvResultPacket(%d:%s)", header, name.c_str());
        const auto found = pendingLogins.find(name);
        if (found != pendingLogins.end()) {
            NetmarbleJP_OnRecvResult(found->second, header, name.c_str());
            pendingLogins.erase(found);
        }

        data += packetSize;
        consumed += packetSize;
        m_iBufferLeft -= packetSize;
        sys_log(0, "NETMARBLE_JP::RecvResultPacketEnd(rest:%d)", m_iBufferLeft);
        if (desc->GetInputProcessor() != this) {
            _sys_err("Process", 0x87, "NETMARBLE_JP::Process - DIFF_TYPE");
            return false;
        }
    }
    return true;
}

BYTE CInputNetmarble::GetType()
{
    return 9;
}

int CInputNetmarble::Analyze(DESC*, BYTE, const char*)
{
    return 0;
}



