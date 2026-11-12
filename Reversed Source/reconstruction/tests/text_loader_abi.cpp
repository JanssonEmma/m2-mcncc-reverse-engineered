#include <cstddef>
#include <string>
#include <vector>
#include <map>
#define protected public
#include "../../game/src/text_file_loader.h"
#undef protected
#define SIZE(T,N) typedef char size_##T[(sizeof(T)==N)?1:-1]
#define OFFSET(T,M,N) typedef char off_##T##_##M[(offsetof(T,M)==N)?1:-1]
SIZE(D3DXVECTOR2,8);
OFFSET(D3DXVECTOR2,x,0);
OFFSET(D3DXVECTOR2,y,4);
SIZE(D3DXVECTOR3,12);
OFFSET(D3DXVECTOR3,x,0);
OFFSET(D3DXVECTOR3,y,4);
OFFSET(D3DXVECTOR3,z,8);
SIZE(D3DXVECTOR4,16);
OFFSET(D3DXVECTOR4,x,0);
OFFSET(D3DXVECTOR4,y,4);
OFFSET(D3DXVECTOR4,z,8);
OFFSET(D3DXVECTOR4,w,12);
SIZE(D3DXQUATERNION,16);
OFFSET(D3DXQUATERNION,x,0);
OFFSET(D3DXQUATERNION,y,4);
OFFSET(D3DXQUATERNION,z,8);
OFFSET(D3DXQUATERNION,w,12);
SIZE(D3DXCOLOR,16);
OFFSET(D3DXCOLOR,r,0);
OFFSET(D3DXCOLOR,g,4);
OFFSET(D3DXCOLOR,b,8);
OFFSET(D3DXCOLOR,a,12);
SIZE(D3DCOLORVALUE,16); OFFSET(D3DCOLORVALUE,r,0); OFFSET(D3DCOLORVALUE,g,4); OFFSET(D3DCOLORVALUE,b,8); OFFSET(D3DCOLORVALUE,a,12);
struct PoolABIProbe { int value; }; typedef CDynamicPool<PoolABIProbe> PoolABI; SIZE(PoolABI,24); OFFSET(PoolABI,m_nodes,4); OFFSET(PoolABI,m_pFreeList,8); OFFSET(PoolABI,m_pUsedList,12); OFFSET(PoolABI,m_nodeCount,16); OFFSET(PoolABI,m_stName,20);
