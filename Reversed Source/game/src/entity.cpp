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
namespace {
struct FuncPacketAround
{
    const void* m_data;
    int m_bytes;
    LPENTITY m_except;

    void operator()(LPENTITY entity)
    {
        if (entity == m_except)
            return;
        if (LPDESC desc = entity->GetDesc())
            desc->Packet(m_data, m_bytes);
    }
};

struct FuncPacketView : FuncPacketAround
{
    FuncPacketView(const void* data, int bytes, LPENTITY except)
        : FuncPacketAround{data, bytes, except}
    {
    }

    void operator()(const CEntity::ENTITY_MAP::value_type& entry)
    {
        FuncPacketAround::operator()(entry.first);
    }
};
}

void CEntity::PacketView(const void* data, int bytes, LPENTITY except)
{
    if (!m_pSectree)
        return;
    if (!m_bIsObserver) {
        FuncPacketView send(data, bytes, except);
        for (const auto& entry : m_map_view)
            send(entry);
    }
    if (this != except && m_lpDesc)
        m_lpDesc->Packet(data, bytes);
}

void CEntity::SetObserverMode(bool value) {
    if(m_bIsObserver!=value) {
        m_bIsObserver=value; m_bObserverModeChange=true;
        UpdateSectree();
        if(IsType(0)) static_cast<CHARACTER*>(this)->ChatPacket(5,"ObserverMode %d",static_cast<int>(m_bIsObserver));
    }
}
