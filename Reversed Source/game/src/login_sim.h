#ifndef MCNCC_LOGIN_SIM_H
#define MCNCC_LOGIN_SIM_H

#include "../../common/tables.h"
#include <map>
#include <vector>

struct CLoginSim {
    CLoginSim();
    void AddPlayer(DWORD playerID);
    bool IsCheck();
    void SendLogin();
    void SendLoad();
    void SendLogout();
    bool IsDone();

    TPacketGDAuthLogin auth;
    TPacketGDLoginByKey login;
    TPlayerLoadPacket load;
    TLogoutPacket logout;
    std::vector<DWORD> vecID;
    unsigned int vecIdx;
    bool bCheck;
};

extern std::map<DWORD, CLoginSim*> g_sim;
extern std::vector<TPlayerTable> g_vec_save;

#endif
