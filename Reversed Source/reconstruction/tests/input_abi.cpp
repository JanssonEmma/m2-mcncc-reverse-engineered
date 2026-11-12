#include "../../game/src/input.h"
#include <cstddef>
class Probe : public CInputProcessor {
public:
    static void Check() {
        typedef char map_offset[(offsetof(Probe,m_pPacketInfo)==4)?1:-1];
        typedef char left_offset[(offsetof(Probe,m_iBufferLeft)==8)?1:-1];
        (void)sizeof(map_offset); (void)sizeof(left_offset);
    }
};
typedef char input_size[(sizeof(CInputProcessor)==12)?1:-1];
typedef char close_size[(sizeof(CInputClose)==12)?1:-1];

typedef char handshake_size[(sizeof(CInputHandshake)==16)?1:-1];
typedef char login_size[(sizeof(CInputLogin)==12)?1:-1];
typedef char main_size[(sizeof(CInputMain)==12)?1:-1];
typedef char dead_size[(sizeof(CInputDead)==12)?1:-1];
typedef char auth_size[(sizeof(CInputAuth)==12)?1:-1];
