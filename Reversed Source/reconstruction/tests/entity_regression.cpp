#include <map>
#include <set>
#include <list>
#define private public
#include "../../game/src/entity.h"
#include "../../game/src/sectree.h"
#undef private
#include <cassert>
#include <vector>
#include <cstdio>
#include "../../game/src/utils.h"
struct Action { CEntity* sender; CEntity* receiver; bool insert; Action(CEntity* s,CEntity* r,bool i):sender(s),receiver(r),insert(i) {} };
static std::vector<Action> actions;
class Probe : public CEntity {
public:
    bool change_observer_on_remove;
    Probe():change_observer_on_remove(false) {}
    ~Probe() { Destroy(); }
    virtual void EncodeInsertPacket(LPENTITY receiver) { actions.push_back(Action(this,receiver,true)); }
    virtual void EncodeRemovePacket(LPENTITY receiver) { actions.push_back(Action(this,receiver,false)); if(change_observer_on_remove) m_bIsObserver=true; }
    using CEntity::Initialize;
    using CEntity::Destroy;
    bool Changed() const { return m_bObserverModeChange; }
    bool Has(CEntity* entity) const { return m_map_view.count(entity)!=0; }
    std::size_t Size() const { return m_map_view.size(); }
    int Age(CEntity* entity) const { return m_map_view.find(entity)->second; }
    void Observer(bool value) { m_bIsObserver=value; }
    void CheckInitial(int type=-1) { assert(IsType(type) && GetType()==type && !m_iViewAge && !m_bIsDestroyed && !m_lpDesc && !m_pSectree && !m_lMapIndex && !m_bIsObserver && !m_bObserverModeChange && !m_pos.x && !m_pos.y && !m_pos.z && m_map_view.empty()); }
};
int main() {
    Probe a,b,c; a.CheckInitial(); b.CheckInitial();
    int token=1;
    a.SetXYZ(-2147483647L-1L,2147483647L,-9);
    assert(a.GetX()==(-2147483647L-1L) && a.GetY()==2147483647L && a.GetZ()==-9 && &a.GetXYZ()==&a.m_pos);
    PIXEL_POSITION position={7,-8,9}; a.SetXYZ(position);
    assert(a.GetX()==7 && a.GetY()==-8 && a.GetZ()==9);
    a.SetMapIndex(-17); assert(a.GetMapIndex()==-17);
    LPDESC desc=reinterpret_cast<LPDESC>(&token); a.BindDesc(desc); assert(a.GetDesc()==desc);
    LPSECTREE tree=reinterpret_cast<LPSECTREE>(&token); a.SetSectree(tree); assert(a.GetSectree()==tree);
    a.Initialize(); a.CheckInitial();
    a.SetType(9); assert(a.IsType(9) && !a.IsType(-1));
    a.ViewInsert(&a); assert(!a.Size() && actions.empty());
    a.m_iViewAge=7; a.ViewInsert(&b);
    assert(a.Has(&b) && b.Has(&a) && a.Age(&b)==7 && b.Age(&a)==0 && actions.size()==2);
    assert(actions[0].sender==&b && actions[0].receiver==&a && actions[0].insert);
    assert(actions[1].sender==&a && actions[1].receiver==&b && actions[1].insert);
    a.m_iViewAge=8; a.ViewInsert(&b); assert(a.Age(&b)==8 && a.GetViewAge()==8 && actions.size()==2);
    a.ViewRemove(&b); assert(!a.Size() && !b.Size() && actions.size()==4);
    a.ViewRemove(&b); assert(actions.size()==4);
    actions.clear(); b.Observer(true); assert(b.IsObserverMode()); a.ViewInsert(&b);
    assert(a.Has(&b) && b.Has(&a) && actions.size()==1 && actions[0].sender==&a);
    a.ViewRemove(&b); assert(actions.size()==2 && actions[1].sender==&a && !actions[1].insert);
    b.Observer(false); actions.clear();
    a.ViewInsert(&b,false); assert(a.Has(&b) && !b.Has(&a) && actions.size()==1);
    a.ViewInsert(&b,true); assert(!b.Has(&a) && actions.size()==1); // Existing entries only refresh age.
    a.ViewCleanup(); assert(!a.Size() && actions.size()==1); // No reciprocal entry to remove.
    a.ViewInsert(&b); a.ViewInsert(&c); actions.clear(); a.Destroy();
    assert(a.m_bIsDestroyed && !a.Size() && !b.Size() && !c.Size() && actions.size()==2);
    assert(!actions[0].insert && actions[0].sender==&a && !actions[1].insert && actions[1].sender==&a);
    a.Destroy(); assert(actions.size()==2);
    a.Initialize(3); a.CheckInitial(3);
    a.ViewInsert(&b); a.Initialize(4); a.CheckInitial(4);
    assert(b.Has(&a)); // Initialize clears only this map; cleanup is a separate operation.
    b.ViewCleanup(); assert(!b.Size());
    a.ViewInsert(&b); a.ViewInsert(&c); c.Observer(true); actions.clear();
    a.ViewReencode(); assert(actions.size()==7);
    assert(actions[0].sender==&a && actions[0].receiver==&a && !actions[0].insert);
    assert(actions[1].sender==&a && actions[1].receiver==&a && actions[1].insert);
    unsigned b_insertions=0,c_insertions=0;
    for(std::size_t i=0;i<actions.size();++i) {
        if(actions[i].sender==&b && actions[i].insert) ++b_insertions;
        if(actions[i].sender==&c && actions[i].insert) ++c_insertions;
    }
    assert(b_insertions==1 && !c_insertions);
    actions.clear(); a.Observer(true); a.ViewReencode(); assert(actions.empty());
    a.Observer(false); a.change_observer_on_remove=true; a.ViewReencode();
    assert(actions.size()==5 && !actions[0].insert && actions[1].insert && a.IsObserverMode());
    a.change_observer_on_remove=false; a.Observer(false);
    {
        SECTREE sector,other;
        Probe me,near,beyond,object;
        me.SetType(1); near.SetType(1); beyond.SetType(1); object.SetType(2);
        me.SetXYZ(0,0,0); near.SetXYZ(5724,0,0); beyond.SetXYZ(5725,0,0); object.SetXYZ(999999,999999,0);
        me.SetSectree(&sector); near.SetSectree(&sector); beyond.SetSectree(&sector); object.SetSectree(&other);
        sector.m_neighbor_list.push_back(&sector); sector.m_neighbor_list.push_back(&other);
        sector.m_set_entity.insert(&me); sector.m_set_entity.insert(&near); sector.m_set_entity.insert(&beyond);
        other.m_set_entity.insert(&object);
        assert(DISTANCE_APPROX(100,0)==96 && DISTANCE_APPROX(-100,-100)==135);
        assert(DISTANCE_APPROX(5724,0)==5500 && DISTANCE_APPROX(5725,0)==5501);
        me.UpdateSectree();
        assert(me.GetViewAge()==1 && me.Has(&near) && !me.Has(&beyond) && me.Has(&object));
        assert(me.Age(&near)==1 && me.Age(&object)==1 && near.Has(&me) && object.Has(&me));
        sector.m_set_entity.erase(&near); near.Observer(true); actions.clear(); me.UpdateSectree();
        assert(me.GetViewAge()==2 && !me.Has(&near) && !near.Has(&me) && actions.size()==2);
        assert(actions[0].sender==&near && !actions[0].insert && actions[1].sender==&me && !actions[1].insert);
        sector.m_neighbor_list.clear(); me.Observer(true); actions.clear(); me.UpdateSectree();
        assert(me.GetViewAge()==3 && me.Has(&object) && actions.empty()); // Unchanged observers retain stale entries.
        me.Observer(false); me.SetObserverMode(true);
        assert(me.GetViewAge()==4 && !me.Size() && !object.Has(&me) && !me.Changed());
        sector.m_neighbor_list.push_back(&sector); sector.m_neighbor_list.push_back(&other);
        near.Observer(false); sector.m_set_entity.insert(&near); me.SetObserverMode(false);
        assert(me.GetViewAge()==5 && me.Has(&near) && me.Has(&object) && !me.Changed());
        me.SetSectree(0); me.SetObserverMode(true);
        assert(me.GetViewAge()==5 && me.Changed() && me.IsObserverMode());
        me.SetObserverMode(true); assert(me.GetViewAge()==5 && me.Changed());
        me.SetSectree(&sector); me.SetObserverMode(false);
        assert(me.GetViewAge()==6 && !me.Changed());
        me.SetObserverMode(false); assert(me.GetViewAge()==6);
        sector.m_iPCCount=7; sector.isClone=true; sector.m_id.package=0x12345678UL;
        assert(sector.m_id.coord.x==0x5678 && sector.m_id.coord.y==0x1234);
        sector.Initialize(); assert(!sector.m_iPCCount && !sector.isClone && !sector.m_id.package && !sector.m_pkAttribute);
        assert(sector.m_neighbor_list.size()==2 && sector.m_set_entity.size()==3);
        me.ViewCleanup();
    }
    std::puts("entity views passed: reciprocal ordering, age refresh, observer suppression, one-sided graphs and cleanup");
}
