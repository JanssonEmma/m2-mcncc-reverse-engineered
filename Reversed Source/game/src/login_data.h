#ifndef MCNCC_GAME_LOGIN_DATA_H
#define MCNCC_GAME_LOGIN_DATA_H

#include "../../libthecore/include/typedef.h"
#include <string>

class CLoginData {
public:
    CLoginData();
    void SetClientKey(const DWORD* clientKey);
    const DWORD* GetClientKey();
    void SetKey(DWORD key);
    DWORD GetKey();
    void SetLogin(const char* login);
    const char* GetLogin();
    void SetBillType(BYTE type);
    BYTE GetBillType();
    void SetBillID(DWORD id);
    DWORD GetBillID();
    void SetConnectedPeerHandle(DWORD handle);
    DWORD GetConnectedPeerHandle();
    void SetLogonTime();
    DWORD GetLogonTime();
    void SetIP(const char* ip);
    const char* GetIP();
    void SetRemainSecs(long seconds);
    long GetRemainSecs();
    void SetBilling(bool enabled);
    bool IsBilling();
    void SetDeleted(bool deleted);
    bool IsDeleted();
    void SetPremium(int* premiumTimes);
    int GetPremium(BYTE type);
    int* GetPremiumPtr();

private:
    // Original i386 CLoginData record is 100 bytes.
    DWORD m_dwKey;
    DWORD m_adwClientKey[4];
    BYTE m_bBillType;
    DWORD m_dwBillID;
    DWORD m_dwConnectedPeerHandle;
    DWORD m_dwLogonTime;
    long m_lRemainSecs;
    char m_szIP[16];
    bool m_bBilling;
    bool m_bDeleted;
    std::string m_stLogin;
    int m_aiPremiumTimes[9];
};

#endif
