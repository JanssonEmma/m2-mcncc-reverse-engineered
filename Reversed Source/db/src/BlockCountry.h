#ifndef __INC_METIN_II_BLOCKCOUNTRY_H__
#define __INC_METIN_II_BLOCKCOUNTRY_H__

#include "Peer.h"

struct T_BLOCK_IP
{
    unsigned int ip_from;
    unsigned int ip_to;
    char country[51];
};

class CBlockCountry : public singleton<CBlockCountry>
{
public:
    CBlockCountry();
    ~CBlockCountry();

    bool Load();
    bool IsBlockedCountryIp(const char* user_ip);
    void SendBlockedCountryIp(CPeer* peer);
    void SendBlockException(CPeer* peer);
    void SendBlockExceptionOne(CPeer* peer, const char* login, BYTE cmd);
    void AddBlockException(const char* login);
    void DelBlockException(const char* login);

private:
    typedef std::vector<T_BLOCK_IP*> BLOCK_IP_VECTOR;
    typedef std::vector<const char*> BLOCK_EXCEPTION_VECTOR;

    BLOCK_IP_VECTOR m_block_ip;
    BLOCK_EXCEPTION_VECTOR m_block_exception;
};

#endif
