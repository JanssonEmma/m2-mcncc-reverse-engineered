#ifndef MCNCC_GAME_TARGET_H
#define MCNCC_GAME_TARGET_H
#include "../../common/singleton.h"
#include "event.h"
#include <cstdint>
#include <list>
#include <map>

class CHARACTER;

enum ETargetTypes
{
    TARGET_TYPE_POS = 1 << 0,
    TARGET_TYPE_VID = 1 << 1,
};

// 112-byte event record in the original x86 ABI.
struct TargetInfo {
    CHARACTER* pkChr;
    int iID;
    std::uint32_t dwPID;
    std::uint32_t dwQuestIndex;
    char szTargetName[33];
    char szTargetDesc[33];
    int iType;
    int iArg1;
    int iArg2;
    int iMapIndex;
    int iOldX;
    int iOldY;
    bool bSendToClient;
};
class CTargetManager : public singleton<CTargetManager> {
public:
    CTargetManager();
    virtual ~CTargetManager();
    TargetInfo* GetTargetInfo(DWORD pid, int type, int argument);
    void CreateTarget(DWORD pid, DWORD quest, const char* name, int type, int arg1,
                      int arg2, int map, const char* description=0, int send=1);
    void DeleteTarget(DWORD pid, DWORD quest, const char* name);
    LPEVENT GetTargetEvent(DWORD pid, DWORD quest, const char* name);
    void Logout(DWORD pid);
private:
    std::map<std::uint32_t, std::list<LPEVENT> > m_map_kListEvent;
    int m_iID;
};
#endif
