// Complete old member declaration. Original GCC3.3 container ABI is required
// to reproduce target size128; current host STL has a different tree layout.
#ifndef MCNCC_GAME_DESC_MANAGER_H
#define MCNCC_GAME_DESC_MANAGER_H
#include <string>
#include <map>
#include <set>
#include <ext/hash_map>
#include "../../common/singleton.h"
#include "../../common/stl.h"
#include "../../libthecore/include/typedef.h"
class DESC;
class CLIENT_DESC;
class CLoginKey;
struct fdwatch;
class DESC_MANAGER : public singleton<DESC_MANAGER> {
public:
    typedef std::set<DESC*> DESC_SET;
    typedef std::set<CLIENT_DESC*> CLIENT_DESC_SET;
    typedef std::map<int, DESC*> DESC_HANDLE_MAP;
    typedef std::map<DWORD, DESC*> DESC_HANDSHAKE_MAP;
    typedef std::map<DWORD, DESC*> DESC_ACCOUNTID_MAP;
    typedef __gnu_cxx::hash_map<std::string, DESC*, stringhash> DESC_LOGINNAME_MAP;
    typedef std::map<DWORD, DWORD> DESC_HANDLE_RANDOM_KEY_MAP;
    DESC_MANAGER();
    virtual ~DESC_MANAGER();
    void Initialize();
    void Destroy();
    DWORD GetLocalUserCount() { return m_iLocalUserCount; }
    void UpdateLocalUserCount();
    void DestroyDesc(DESC* desc, bool eraseFromSet = true);
    DESC* AcceptP2PDesc(struct fdwatch* watcher, socket_t socket);
    DESC* AcceptDesc(struct fdwatch* watcher, socket_t socket);
    DWORD CreateHandshake();
    void TryConnect();
    void DestroyClosed();
    CLIENT_DESC* CreateConnectionDesc(struct fdwatch* watcher, const char* host, WORD port, int phase, bool retry);
    void ConnectAccount(const std::string& login, DESC* desc);
    void DisconnectAccount(const std::string& login);
    DESC* FindByLoginName(const std::string& login);
    DESC* FindByCharacterName(const char* name);
    DESC* FindByHandle(DWORD handle);
    DESC* FindByHandshake(DWORD handshake);
    const DESC_SET& GetClientSet();
    void SetDisconnectInvalidCRCMode(bool enabled) { m_bDisconnectInvalidCRC = enabled; }
    void GetUserCount(int& total, int** empireUserCount, int& localCount);
    DWORD MakeRandomKey(DWORD handle);
    bool GetRandomKey(DWORD handle, DWORD* randomKey);
    DESC* FindByLoginKey(DWORD key);
    DWORD CreateLoginKey(DESC* desc);
    void ProcessExpiredLoginKey();
private:
    bool m_bDisconnectInvalidCRC;
    DESC_HANDLE_RANDOM_KEY_MAP m_map_handle_random_key;
    DESC_SET m_set_pkClientDesc;
    DESC_SET m_set_pkDesc;
    DESC_HANDLE_MAP m_map_handle;
    DESC_HANDSHAKE_MAP m_map_handshake;
    DESC_LOGINNAME_MAP m_map_loginName;
    std::map<DWORD, CLoginKey*> m_map_pkLoginKey;
    int m_iSocketsConnected;
    int m_iHandleCount;
    int m_iLocalUserCount;
    int m_aiEmpireUserCount[4];
};
#endif

