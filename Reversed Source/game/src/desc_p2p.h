#ifndef MCNCC_GAME_DESC_P2P_H
#define MCNCC_GAME_DESC_P2P_H

#include "desc.h"

class DESC_P2P : public DESC {
public:
    ~DESC_P2P() override;
    void Destroy() override;
    void SetPhase(int phase) override;
    bool Setup(LPFDWATCH watcher, socket_t socket, const char* host);
};

#endif
