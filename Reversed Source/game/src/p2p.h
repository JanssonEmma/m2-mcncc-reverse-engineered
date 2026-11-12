#ifndef MCNCC_GAME_P2P_H
#define MCNCC_GAME_P2P_H
#include <set>
#include <ext/hash_map>
#include "../../common/singleton.h"
#include "../../common/stl.h"
#include "../../libthecore/include/typedef.h"

class DESC;
class CInputProcessor;
struct SPacketGGLogin;
typedef struct _CCI {
    char szName[25];
    DWORD dwPID;
    BYTE bEmpire;
    BYTE bMapIndex;
    DWORD dwTime;
    BYTE bChannel;
    DESC* pkDesc;
} CCI;

class P2P_MANAGER : public singleton<P2P_MANAGER> {
public:
    P2P_MANAGER();
    virtual ~P2P_MANAGER();
    void RegisterAcceptor(DESC* desc);
    void RegisterConnector(DESC* desc);
    void Boot(DESC* desc);
    void Send(const void* data, int size, DESC* except = 0);
    void FlushOutput();
    void UnregisterAcceptor(DESC* desc);
    void UnregisterConnector(DESC* desc);
    void EraseUserByDesc(DESC* desc);
    CCI* Find(const char* name);
    CCI* FindByPID(DWORD pid);
    bool IsExist(const char* host, WORD port);
    int GetCount();
    int GetEmpireUserCount(int index);
    void RecvLogin(DESC* desc, const SPacketGGLogin* packet);
    void RecvLogout(DESC* desc, const char* name);

private:
    CInputProcessor* m_pkInputProcessor;
    int m_iHandleCount;
    std::set<DESC*> m_set_pkPeers;
    __gnu_cxx::hash_map<std::string,CCI*,stringhash,std::equal_to<std::string>,std::allocator<CCI*> > m_map_pkCCI;
    __gnu_cxx::hash_map<DWORD,CCI*,__gnu_cxx::hash<DWORD>,std::equal_to<DWORD>,std::allocator<CCI*> > m_map_dwPID_pkCCI;
    int m_aiEmpireUserCount[4];
};
#endif
