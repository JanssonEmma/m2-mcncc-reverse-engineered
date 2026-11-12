#include "char_manager.h"
#include "desc.h"
#include "item.h"
#include "item_manager.h"
//080a6f10 constructor and080a71d0 initialization. Destructor/ownership pending.
#include "sectree.h"
#include "char.h"
#include "../../libthecore/include/log.h"
#include "../../libgame/include/attribute.h"
#include <cassert>
#include <cstdint>
SECTREE::SECTREE() { Initialize(); }
void SECTREE::Initialize() { m_id.package=0; m_pkAttribute=0; m_iPCCount=0; isClone=false; }

SECTREEID SECTREE::GetID() { return m_id; }
void SECTREE::BindAttribute(CAttribute* attribute) { m_pkAttribute=attribute; }
void SECTREE::CloneAttribute(SECTREE* source) { isClone=true; m_pkAttribute=source->m_pkAttribute; }
void SECTREE::SetAttribute(DWORD x,DWORD y,DWORD attribute) {
    assert(m_pkAttribute); m_pkAttribute->Set(static_cast<int>(x),static_cast<int>(y),static_cast<std::uint32_t>(attribute));
}
void SECTREE::RemoveAttribute(DWORD x,DWORD y,DWORD attribute) {
    assert(m_pkAttribute); m_pkAttribute->Remove(static_cast<int>(x),static_cast<int>(y),static_cast<std::uint32_t>(attribute));
}
DWORD SECTREE::GetAttribute(long x,long y) {
    assert(m_pkAttribute);
    const auto cellX = (static_cast<std::int32_t>(x) % 6400) / 50;
    const auto cellY = (static_cast<std::int32_t>(y) % 6400) / 50;
    return static_cast<std::uint32_t>(m_pkAttribute->Get(cellX, cellY));
}
bool SECTREE::IsAttr(long x,long y,DWORD flag) { return (GetAttribute(x,y)&flag)!=0; }
int SECTREE::GetEventAttribute(long x,long y) { return static_cast<int>(GetAttribute(x,y)>>8); }

void SECTREE::IncreasePC() {
    LPSECTREE_LIST::iterator it=m_neighbor_list.begin();
    while(it!=m_neighbor_list.end()) {
        SECTREE* tree=*it++;
        tree->m_iPCCount=static_cast<int>(static_cast<unsigned int>(tree->m_iPCCount)+1U);
    }
}
void SECTREE::DecreasePC() {
    LPSECTREE_LIST::iterator it=m_neighbor_list.begin();
    while(it!=m_neighbor_list.end()) {
        SECTREE* tree=*it++;
        tree->m_iPCCount=static_cast<int>(static_cast<unsigned int>(tree->m_iPCCount)-1U);
        if(tree->m_iPCCount<=0) {
            if(tree->m_iPCCount<0) {
                _sys_err("DecreasePC",103,"tree pc count lower than zero (value %d coord %d %d)",tree->m_iPCCount,static_cast<unsigned int>(tree->m_id.coord.x),static_cast<unsigned int>(tree->m_id.coord.y));
                tree->m_iPCCount=0;
            }
            ENTITY_SET::iterator entity=tree->m_set_entity.begin();
            while(entity!=tree->m_set_entity.end()) {
                CEntity* current=*entity++;
                if(current->IsType(0)) static_cast<CHARACTER*>(current)->StopStateMachine();
            }
        }
    }
}
bool SECTREE::InsertEntity(LPENTITY entity) {
    SECTREE* previous=entity->GetSectree();
    if(previous==this) return false;
    if(previous) previous->m_set_entity.erase(entity);
    entity->SetSectree(this);
    if(m_set_entity.find(entity)==m_set_entity.end()) m_set_entity.insert(entity);
    else _sys_err("InsertEntity",139,"entity %p already exist in this sectree!",entity);
    if(entity->IsType(0)) {
        if(entity->GetDesc()) { IncreasePC(); if(previous) previous->DecreasePC(); }
        else if(m_iPCCount>0) {
            CHARACTER* character=static_cast<CHARACTER*>(entity);
            BYTE kind=character->GetCharType();
            if(kind!=3 && kind!=9) character->StartStateMachine(1);
        }
    }
    return true;
}
void SECTREE::RemoveEntity(LPENTITY entity) {
    m_set_entity.erase(entity);
    entity->SetSectree(0);
    if(entity->IsType(0) && entity->GetDesc()) DecreasePC();
}

SECTREE::~SECTREE()
{
    Destroy();
}

void SECTREE::Destroy()
{
    if (!m_set_entity.empty()) {
        _sys_err("Destroy",0x22,"Sectree deleted, but some entitys remain.");
        ENTITY_SET::iterator it=m_set_entity.begin();
        while (it!=m_set_entity.end()) {
            LPENTITY entity=*it++;
            if (entity->IsType(0)) {
                CHARACTER* character=static_cast<CHARACTER*>(entity);
                const bool player=character->GetDesc()!=0;
                _sys_err("Destroy",0x2d,"Sectree destroying character: %s is_pc %d",character->GetName(),player);
                if (character->GetDesc()) {
                    character->GetDesc()->SetPhase(0);
                    character->Disconnect();
                } else CHARACTER_MANAGER::instance().DestroyCharacter(character);
            } else if (entity->IsType(1)) {
                CItem* item=static_cast<CItem*>(entity);
                _sys_err("Destroy",0x3b,"Sectree destroying Item: %s",item->GetName());
                ITEM_MANAGER::instance().DestroyItem(item);
            } else _sys_err("Destroy",0x40,"Unknown type: %d",entity->GetType());
        }
    }
    if (!isClone && m_pkAttribute) {
        delete m_pkAttribute;
        m_pkAttribute=0;
    }
}
