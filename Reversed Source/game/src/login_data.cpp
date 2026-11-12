#include "login_data.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/memcpy.h"
#include "../../libthecore/include/utils.h"
#include <cstring>

// 08084c60 / 08084ce0
CLoginData::CLoginData()
    : m_dwKey(0), m_bBillType(0), m_dwBillID(0), m_dwConnectedPeerHandle(0),
      m_dwLogonTime(0), m_lRemainSecs(0), m_bBilling(false), m_bDeleted(false)
{
    std::memset(m_adwClientKey, 0, sizeof(m_adwClientKey));
    std::memset(m_szIP, 0, sizeof(m_szIP));
    std::memset(m_aiPremiumTimes, 0, sizeof(m_aiPremiumTimes));
}

void CLoginData::SetClientKey(const DWORD* clientKey)
{
    thecore_memcpy(m_adwClientKey, clientKey, sizeof(m_adwClientKey));
}

const DWORD* CLoginData::GetClientKey() { return m_adwClientKey; }
void CLoginData::SetKey(DWORD key) { m_dwKey = key; }
DWORD CLoginData::GetKey() { return m_dwKey; }
void CLoginData::SetBillType(BYTE type) { m_bBillType = type; }
DWORD CLoginData::GetBillID() { return m_dwBillID; }
void CLoginData::SetBillID(DWORD id) { m_dwBillID = id; }
BYTE CLoginData::GetBillType() { return m_bBillType; }
void CLoginData::SetConnectedPeerHandle(DWORD handle) { m_dwConnectedPeerHandle = handle; }
DWORD CLoginData::GetConnectedPeerHandle() { return m_dwConnectedPeerHandle; }
void CLoginData::SetLogonTime() { m_dwLogonTime = get_dword_time(); }
DWORD CLoginData::GetLogonTime() { return m_dwLogonTime; }

void CLoginData::SetIP(const char* ip)
{
    std::strncpy(m_szIP, ip, 15);
    m_szIP[15] = '\0';
}

const char* CLoginData::GetIP() { return m_szIP; }

void CLoginData::SetRemainSecs(long seconds)
{
    m_lRemainSecs = seconds;
    sys_log(0, "SetRemainSecs %s %ld type %u", m_stLogin.c_str(), m_lRemainSecs,
            static_cast<unsigned int>(m_bBillType));
}

long CLoginData::GetRemainSecs() { return m_lRemainSecs; }

void CLoginData::SetBilling(bool enabled)
{
    if (enabled) {
        sys_log(0, "BILLING: ON %s key %lu ptr %p", m_stLogin.c_str(), m_dwKey, static_cast<void*>(this));
        SetLogonTime();
    } else {
        sys_log(0, "BILLING: OFF %s key %lu ptr %p", m_stLogin.c_str(), m_dwKey, static_cast<void*>(this));
    }
    m_bBilling = enabled;
}

bool CLoginData::IsBilling() { return m_bBilling; }
void CLoginData::SetDeleted(bool deleted) { m_bDeleted = deleted; }
bool CLoginData::IsDeleted() { return m_bDeleted; }
void CLoginData::SetLogin(const char* login) { m_stLogin = login; }
const char* CLoginData::GetLogin() { return m_stLogin.c_str(); }

void CLoginData::SetPremium(int* premiumTimes)
{
    thecore_memcpy(m_aiPremiumTimes, premiumTimes, sizeof(m_aiPremiumTimes));
}

int CLoginData::GetPremium(BYTE type) { return type < 9 ? m_aiPremiumTimes[type] : 0; }
int* CLoginData::GetPremiumPtr() { return m_aiPremiumTimes; }
