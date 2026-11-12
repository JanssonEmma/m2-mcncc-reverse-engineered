#include "desc_p2p.h"
#include "input.h"
#include "p2p.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/socket.h"

DESC_P2P::~DESC_P2P()
{
    Destroy();
}

bool DESC_P2P::Setup(LPFDWATCH watcher, socket_t socket, const char* host)
{
    m_lpFdw = watcher;
    m_stHost.assign(host);
    m_lpOutputBuffer = buffer_new(nullptr, 0x100000);
    if (!m_lpOutputBuffer)
        return false;
    m_lpInputBuffer = buffer_new(nullptr, 0x100000);
    if (!m_lpInputBuffer)
        return false;

    m_sock = socket;
    fdwatch_add_fd(m_lpFdw, m_sock, this, FDW_READ, 0);
    m_iMinInputBufferLen = 0x100000;
    SetPhase(9);
    return true;
}

void DESC_P2P::Destroy()
{
    if (m_sock == -1)
        return;

    P2P_MANAGER::instance().UnregisterAcceptor(this);
    fdwatch_del_fd(m_lpFdw, m_sock);
    sys_log(0, "SYSTEM: closing p2p socket. DESC #%d", m_sock);
    socket_close(m_sock);
    m_sock = -1;
}

void DESC_P2P::SetPhase(int phase)
{
    static CInputP2P input;
    if (phase == 0)
        m_pInputProcessor = nullptr;
    else if (phase == 9) {
        sys_log(1, "PHASE_P2P");
        m_pInputProcessor = &input;
    } else {
        sys_err("DESC_P2P::SetPhase : Unknown phase");
    }
    m_iPhase = phase;
}
