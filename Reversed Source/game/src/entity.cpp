// 2009 entity lifecycle, packet fanout and observer state.
#include "entity.h"
#include "desc.h"
#include "char.h"
#include <cassert>
#include <cstdlib>
CEntity::CEntity() { Initialize(-1); }
CEntity::~CEntity()
{
    if (!m_bIsDestroyed) {
        assert(m_bIsDestroyed);
        std::abort();
    }
}
void CEntity::Initialize(int type) {
    m_iType=type; m_iViewAge=0; m_bIsDestroyed=false;
    m_pos.x=m_pos.y=m_pos.z=0; m_map_view.clear();
    m_pSectree=0; m_lpDesc=0; m_lMapIndex=0;
    m_bIsObserver=false; m_bObserverModeChange=false;
}
void CEntity::Destroy() { ViewCleanup(); m_bIsDestroyed=true; }
void CEntity::SetType(int type) { m_iType=type; }
int CEntity::GetType() { return m_iType; }
bool CEntity::IsType(int type) { return m_iType==type; }

void CEntity::PacketAround(const void* data,int bytes,LPENTITY except) { PacketView(data,bytes,except); }
void CEntity::PacketView(const void* data,int bytes,LPENTITY except) {
    if(!m_pSectree) return;
    if(!m_bIsObserver) {
        for(ENTITY_MAP::iterator it=m_map_view.begin();it!=m_map_view.end();++it) {
            CEntity* entity=it->first;
            if(entity!=except && entity->m_lpDesc) entity->m_lpDesc->Packet(data,bytes);
        }
    }
    if(this!=except && m_lpDesc) m_lpDesc->Packet(data,bytes);
}

void CEntity::SetObserverMode(bool value) {
    if(m_bIsObserver!=value) {
        m_bIsObserver=value; m_bObserverModeChange=true;
        UpdateSectree();
        if(IsType(0)) static_cast<CHARACTER*>(this)->ChatPacket(5,"ObserverMode %d",static_cast<int>(m_bIsObserver));
    }
}
