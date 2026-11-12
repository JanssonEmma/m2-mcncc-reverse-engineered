#ifndef MCNCC_GAME_PASSPOD_H
#define MCNCC_GAME_PASSPOD_H
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
struct fdwatch;
class CPasspod : public singleton<CPasspod> {
public:
    CPasspod();
    virtual ~CPasspod();
    int ConfirmPasspod(const char* account, const char* answer);
private:
    bool Connect(const char* host);
    void Disconnect();
    bool IConv(const char* input, char* output);

    socket_t m_sock;
    struct fdwatch* m_lpFDW;
};
#endif
