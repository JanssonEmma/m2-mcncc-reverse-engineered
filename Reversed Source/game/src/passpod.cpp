#include "passpod.h"
#include "../../libthecore/include/socket.h"
#include "../../libthecore/include/log.h"

#include <arpa/inet.h>
#include <cstdlib>
#include <string>

namespace {
std::string EncodeFormField(const char* value)
{
    constexpr char hex[] = "0123456789ABCDEF";
    std::string output;
    for (const unsigned char* c = reinterpret_cast<const unsigned char*>(value); *c; ++c) {
        if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') ||
            (*c >= '0' && *c <= '9') || *c == '-' || *c == '_' || *c == '.' || *c == '~') {
            output += static_cast<char>(*c);
        } else {
            output += '%';
            output += hex[*c >> 4];
            output += hex[*c & 15];
        }
    }
    return output;
}

int ParsePasspodResponse(const std::string& response)
{
    const auto separator = response.find("\r\n\r\n");
    if (separator == std::string::npos ||
        (response.compare(0, 13, "HTTP/1.0 200 ") && response.compare(0, 13, "HTTP/1.1 200 ")))
        return 1;

    const auto start = response.find_first_not_of("\r\n", separator + 4);
    if (start == std::string::npos)
        return 1;
    const auto end = response.find_first_of("\r\n", start);
    const auto status = response.substr(start, end == std::string::npos ? end : end - start);
    constexpr const char* responses[] = {
        "AUTH_SUCCESS",
        "AUTH_FAILURE:PASSPOD_ERROR",
        "AUTH_FAILURE:USER_NOT_FOUND",
        "AUTH_FAILURE:SYSTEM_NOT_FOUND",
        "AUTH_FAILURE:TOKEN_DISABLED",
        "AUTH_FAILURE:EMPTY",
    };
    for (int i = 0; i < 6; ++i) {
        if (status == responses[i])
            return i;
    }
    return 1;
}
}

CPasspod::CPasspod() : m_sock(-1), m_lpFDW(nullptr) {}

CPasspod::~CPasspod()
{
    Disconnect();
}

// 0815c2b0. The operator supplies the host for the legacy transport.
bool CPasspod::Connect(const char* host)
{
    if (m_sock != -1) {
        _sys_err("Connect", 0xa4, "Sock != INVALID_SOCKET ");
        return false;
    }

    m_sock = socket_connect(host, 8080);
    if (m_sock == -1) {
        _sys_err("Connect", 0xac, "Sock == INVALID_SOCKET ");
        return false;
    }
    return true;
}

// 0815c320
void CPasspod::Disconnect()
{
    if (m_sock != -1)
        socket_close(m_sock);
    m_sock = -1;
}

// 0815c350
bool CPasspod::IConv(const char*, char*)
{
    return true;
}

int CPasspod::ConfirmPasspod(const char* account, const char* answer)
{
    // Require an operator-selected trusted endpoint for the old HTTP transport.
    const char* host = std::getenv("MCNCC_PASSPOD_HOST");
    in_addr address;
    if (!host || inet_pton(AF_INET, host, &address) != 1)
        return 1;

    const std::string form = "username=" + EncodeFormField(account) +
        "&systemname=1001&passpod=" + EncodeFormField(answer) + "&algname=plaintext";
    const std::string request = std::string("POST /passpod-server/auth.s HTTP/1.0\r\nHost: ") + host +
        "\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: " +
        std::to_string(form.size()) + "\r\nConnection: Close\r\n\r\n" + form;

    if (!Connect(host))
        return 1;
    socket_lingeroff(m_sock);
    socket_timeout(m_sock, 2, 0);

    std::string response;
    if (!socket_write(m_sock, request.data(), request.size())) {
        char chunk[1024];
        while (response.size() < 4096) {
            const auto received = recv(m_sock, chunk, sizeof(chunk), 0);
            if (received <= 0)
                break;
            response.append(chunk, received);
        }
    }
    Disconnect();
    return ParsePasspodResponse(response);
}
