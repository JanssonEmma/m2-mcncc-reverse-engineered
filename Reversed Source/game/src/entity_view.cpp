// Recovered mutual view graph operations; spatial refresh/reencode remain pending.
#include "entity.h"
#include "sectree.h"
#include "char.h"
#include "utils.h"
#include <cstdint>
#include "../../libthecore/include/log.h"
void CEntity::ViewCleanup() {
    ENTITY_MAP::iterator it=m_map_view.begin();
    while(it!=m_map_view.end()) {
        CEntity* entity=(it++)->first;
        entity->ViewRemove(this,false);
    }
    m_map_view.clear();
}
void CEntity::ViewInsert(LPENTITY entity,bool recursive) {
    if(this==entity) return;
    ENTITY_MAP::iterator it=m_map_view.find(entity);
    if(it==m_map_view.end()) {
        m_map_view.insert(std::make_pair(entity,m_iViewAge));
        if(!entity->m_bIsObserver) entity->EncodeInsertPacket(this);
        if(recursive) entity->ViewInsert(this,false);
    } else it->second=m_iViewAge;
}
void CEntity::ViewRemove(LPENTITY entity,bool recursive) {
    ENTITY_MAP::iterator it=m_map_view.find(entity);
    if(it!=m_map_view.end()) {
        m_map_view.erase(it);
        if(!entity->m_bIsObserver) entity->EncodeRemovePacket(this);
        if(recursive) entity->ViewRemove(this,false);
    }
}

void CEntity::ViewReencode() {
    if(m_bIsObserver) return;
    EncodeRemovePacket(this); EncodeInsertPacket(this);
    ENTITY_MAP::iterator it=m_map_view.begin();
    while(it!=m_map_view.end()) {
        CEntity* entity=(it++)->first;
        EncodeRemovePacket(entity);
        if(!m_bIsObserver) EncodeInsertPacket(entity);
        if(!entity->m_bIsObserver) entity->EncodeInsertPacket(this);
    }
}

// Old functor has one pointer; the5500 range is a constant, not a data field.
class CFuncViewInsert {
public:
    LPENTITY m_me;
    explicit CFuncViewInsert(LPENTITY me):m_me(me) {}
    void operator()(LPENTITY entity) {
        if (!entity->IsType(2)) {
            const auto dx = static_cast<std::int32_t>(static_cast<std::uint32_t>(entity->GetX()) -
                static_cast<std::uint32_t>(m_me->GetX()));
            const auto dy = static_cast<std::int32_t>(static_cast<std::uint32_t>(entity->GetY()) -
                static_cast<std::uint32_t>(m_me->GetY()));
            if (DISTANCE_APPROX(dx, dy) > 5500)
                return;
        }
        m_me->ViewInsert(entity);
        if(entity->IsType(0) && m_me->IsType(0) && m_me->GetDesc() && !entity->GetDesc()) {
            CHARACTER* character=static_cast<CHARACTER*>(entity);
            BYTE kind=character->GetCharType();
            if(kind!=3 && kind!=9) character->StartStateMachine(1);
        }
    }
};
void CEntity::UpdateSectree() {
    SECTREE* sector=m_pSectree;
    if(!sector) {
        if (IsType(0))
            _sys_err("UpdateSectree", 120, "null sectree name: %s %d %d",
                static_cast<CHARACTER*>(this)->GetName(),
                static_cast<std::int32_t>(m_pos.x), static_cast<std::int32_t>(m_pos.y));
        return;
    }
    m_iViewAge=static_cast<int>(static_cast<unsigned int>(m_iViewAge)+1U);
    CFuncViewInsert insert(this); sector->ForEachAround(insert);
    const bool changed=m_bObserverModeChange,observer=m_bIsObserver;
    if(changed || !observer) {
        ENTITY_MAP::iterator it=m_map_view.begin();
        while(it!=m_map_view.end()) {
            ENTITY_MAP::iterator current=it++;
            CEntity* entity=current->first;
            if(current->second<m_iViewAge) {
                entity->EncodeRemovePacket(this);
                m_map_view.erase(current);
                entity->ViewRemove(this,false);
            } else if(changed) {
                if(observer) EncodeRemovePacket(entity);
                else { entity->EncodeInsertPacket(this); EncodeInsertPacket(entity); entity->ViewInsert(this); }
            }
        }
    }
    if(changed) m_bObserverModeChange=false;
}
