// Complete old sector data declaration; lifecycle entry and direct traversal recovered.
#ifndef MCNCC_GAME_SECTREE_H
#define MCNCC_GAME_SECTREE_H
#include "entity.h"
#include "../../libthecore/include/typedef.h"
class CAttribute;
struct sectree_coord { unsigned int x:16; unsigned int y:16; };
typedef sectree_coord SECTREE_COORD;
union sectreeid { DWORD package; SECTREE_COORD coord; };
typedef sectreeid SECTREEID;
class SECTREE {
public:
    SECTREE();
    ~SECTREE();
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
