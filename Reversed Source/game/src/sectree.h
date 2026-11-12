// Complete old sector data declaration; lifecycle entry and direct traversal recovered.
#ifndef MCNCC_GAME_SECTREE_H
#define MCNCC_GAME_SECTREE_H
#include "entity.h"
#include <cstdint>
#include "../../libthecore/include/typedef.h"
class CAttribute;
struct sectree_coord { unsigned int x:16; unsigned int y:16; };
typedef sectree_coord SECTREE_COORD;
union sectreeid { std::uint32_t package; SECTREE_COORD coord; };
typedef sectreeid SECTREEID;
static_assert(sizeof(SECTREEID) == 4, "2009 sector map key");

enum {
    ATTR_BLOCK = 1 << 0,
    ATTR_WATER = 1 << 1,
    ATTR_BANPK = 1 << 2,
    ATTR_OBJECT = 1 << 7,
};
class SECTREE {
    friend class SECTREE_MAP;
    friend class SECTREE_MANAGER;
public:
    SECTREE();
    ~SECTREE();
    void Destroy();
    void Initialize();
    SECTREEID GetID();
    bool InsertEntity(LPENTITY entity);
    void RemoveEntity(LPENTITY entity);
    void IncreasePC();
    void DecreasePC();
    void BindAttribute(CAttribute* attribute);
    CAttribute* GetAttributePtr() { return m_pkAttribute; }
    DWORD GetAttribute(long x,long y);
    bool IsAttr(long x,long y,DWORD flag);
    void CloneAttribute(SECTREE* source);
    int GetEventAttribute(long x,long y);
    void SetAttribute(DWORD x,DWORD y,DWORD attribute);
    void RemoveAttribute(DWORD x,DWORD y,DWORD attribute);
    template<class Func> void ForEachEntity(Func func) {
        ENTITY_SET::iterator it = m_set_entity.begin();
        while (it != m_set_entity.end())
            func(*it++);
    }
    template<class Func> void ForEachAround(Func& func) {
        LPSECTREE_LIST::iterator tree=m_neighbor_list.begin();
        while(tree!=m_neighbor_list.end()) {
            SECTREE* sector=*tree++;
            ENTITY_SET::iterator entity=sector->m_set_entity.begin();
            while(entity!=sector->m_set_entity.end()) func(*entity++);
        }
    }
private:
    SECTREEID m_id;
    ENTITY_SET m_set_entity;
    LPSECTREE_LIST m_neighbor_list;
    int m_iPCCount;
    bool isClone;
    CAttribute* m_pkAttribute;
};
#endif
