#include <string>
#include <map>
#include <cstddef>
#define protected public
#include "../../game/src/motion.h"
#undef protected
#define SIZE(T,N) typedef char size_##T[(sizeof(T)==N)?1:-1]
#define OFFSET(T,M,N) typedef char off_##T##_##M[(offsetof(T,M)==N)?1:-1]
SIZE(D3DXVECTOR3,12); OFFSET(D3DXVECTOR3,x,0); OFFSET(D3DXVECTOR3,y,4); OFFSET(D3DXVECTOR3,z,8);
SIZE(CMotion,28); OFFSET(CMotion,m_isEmpty,0); OFFSET(CMotion,m_stFileName,4);
OFFSET(CMotion,m_fDuration,8); OFFSET(CMotion,m_isAccumulation,12); OFFSET(CMotion,m_vec3Accumulation,16);
// CMotionSet/manager use modern map internals: original12/16-byte whole sizes are not asserted.

SIZE(EMotionMode,4); SIZE(EPublicMotion,4);
