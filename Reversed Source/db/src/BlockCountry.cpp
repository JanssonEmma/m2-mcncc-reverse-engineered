#include "stdafx.h"
#include "SqlFormat.h"
#include "BlockCountry.h"
#include "DBManager.h"

namespace
{
enum
{
    BlockCountryHeader = 171,
    BlockExceptionHeader = 172,
    BlockExceptionAdd = 1,
    BlockExceptionDelete = 2
};

struct BlockCountryPacket
{
    unsigned int ip_from;
    unsigned int ip_to;
};
}

bool __is_blocked_country_ip(unsigned int ip, T_BLOCK_IP* block)
{
    return ip >= block->ip_from && ip <= block->ip_to;
}

CBlockCountry::CBlockCountry()
{
}

CBlockCountry::~CBlockCountry()
{
    for (BLOCK_IP_VECTOR::iterator it = m_block_ip.begin(); it != m_block_ip.end(); ++it)
        delete *it;

    m_block_ip.clear();
}

bool CBlockCountry::Load()
{
    char query[256];
    dbsql::sprintf(query, "SELECT IP_FROM, IP_TO, COUNTRY_NAME FROM iptocountry");
    SQLMsg* message = CDBManager::instance().DirectQuery(query, SQL_ACCOUNT);

    if (message->Get()->iNumRows == 0)
    {
        sys_err(" DirectQuery failed( %s )", query);
        delete message;
        return false;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(message->Get()->pSQLResult)) != NULL)
    {
        T_BLOCK_IP* block = new T_BLOCK_IP;
        block->ip_from = strtoul(row[0], NULL, 10);
        block->ip_to = strtoul(row[1], NULL, 10);
        strncpy(block->country, row[2], sizeof(block->country) - 1);
        block->country[sizeof(block->country) - 1] = '\0';
        m_block_ip.push_back(block);
    }
    delete message;

    dbsql::sprintf(query, "SELECT login FROM block_exception");
    message = CDBManager::instance().DirectQuery(query, SQL_ACCOUNT);
    if (message->Get()->iNumRows == 0)
    {
        sys_err(" DirectQuery failed( %s )", query);
        delete message;
        return true;
    }

    while ((row = mysql_fetch_row(message->Get()->pSQLResult)) != NULL)
    {
        const char* login = row[0];
        m_block_exception.push_back(strdup(login));
    }
    delete message;
    return true;
}

bool CBlockCountry::IsBlockedCountryIp(const char* user_ip)
{
    struct in_addr address;
#ifdef _WIN32
    if (InetPtonA(AF_INET, user_ip, &address) != 1)
#else
    if (inet_aton(user_ip, &address) == 0)
#endif
        return true;

    for (BLOCK_IP_VECTOR::iterator it = m_block_ip.begin(); it != m_block_ip.end(); ++it)
    {
        if (__is_blocked_country_ip(address.s_addr, *it))
            return true;
    }

    return false;
}

void CBlockCountry::SendBlockedCountryIp(CPeer* peer)
{
    for (BLOCK_IP_VECTOR::iterator it = m_block_ip.begin(); it != m_block_ip.end(); ++it)
    {
        BlockCountryPacket packet;
        packet.ip_from = (*it)->ip_from;
        packet.ip_to = (*it)->ip_to;
        peer->EncodeHeader(BlockCountryHeader, 0, sizeof(packet));
        peer->Encode(&packet, sizeof(packet));
    }
}

void CBlockCountry::SendBlockException(CPeer* peer)
{
    for (BLOCK_EXCEPTION_VECTOR::iterator it = m_block_exception.begin(); it != m_block_exception.end(); ++it)
    {
        SendBlockExceptionOne(peer, *it, BlockExceptionAdd);
    }
}

void CBlockCountry::SendBlockExceptionOne(CPeer* peer, const char* login, BYTE cmd)
{
    if (!peer || !login || (cmd != BlockExceptionAdd && cmd != BlockExceptionDelete))
        return;

    TPacketBlockException packet;
    packet.cmd = cmd;
    strncpy(packet.login, login, sizeof(packet.login) - 1);
    packet.login[sizeof(packet.login) - 1] = '\0';
    peer->EncodeHeader(BlockExceptionHeader, 0, sizeof(packet));
    peer->Encode(&packet, sizeof(packet));
}

void CBlockCountry::AddBlockException(const char* login)
{
    for (BLOCK_EXCEPTION_VECTOR::iterator it = m_block_exception.begin(); it != m_block_exception.end(); ++it)
    {
        if (strcmp(*it, login) == 0)
            return;
    }

    m_block_exception.push_back(strdup(login));
}

void CBlockCountry::DelBlockException(const char* login)
{
    for (BLOCK_EXCEPTION_VECTOR::iterator it = m_block_exception.begin(); it != m_block_exception.end(); ++it)
    {
        if (strcmp(*it, login) == 0)
        {
            free(const_cast<char*>(*it));
            m_block_exception.erase(it);
            return;
        }
    }
}
