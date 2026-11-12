#include <map>
#include <set>
#include <list>
#define private public
#include "../../game/src/sectree.h"
#undef private
#include "../../libgame/include/attribute.h"
#include "../../game/src/char.h"
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>
extern "C" { void* (*thecore_memcpy)(void*,const void*,size_t)=std::memcpy; }
// Explicit fixture for sector entity/attribute ownership; production destructor remains pending.
SECTREE::~SECTREE() {}
int main() {
    CAttribute cells(128,128),other(128,128);
    SECTREE sector,clone; assert(!sector.GetAttributePtr() && !sector.GetID().package);
    sector.m_id.package=0xabcd1234UL; SECTREEID id=sector.GetID();
    assert(id.coord.x==0x1234 && id.coord.y==0xabcd); id.package=0;
    assert(sector.GetID().package==0xabcd1234UL);
    sector.BindAttribute(&cells); assert(sector.GetAttributePtr()==&cells && !sector.isClone);
    sector.SetAttribute(1,2,0x123401UL);
    assert(sector.GetAttribute(50,100)==0x123401UL && sector.IsAttr(50,100,1) && !sector.IsAttr(50,100,2));
    assert(sector.GetEventAttribute(50,100)==0x1234);
    assert(sector.GetAttribute(6450,6500)==0x123401UL);
    sector.RemoveAttribute(1,2,1); assert(sector.GetAttribute(50,100)==0x123400UL);
    sector.SetAttribute(0,0,0x55); assert(sector.GetAttribute(-1,-1)==0x55 && sector.GetAttribute(-6401,-6401)==0x55);
    sector.SetAttribute(127,127,0x80000080UL);
    assert(sector.GetAttribute(6399,6399)==0x80000080UL && sector.GetEventAttribute(6399,6399)==0x800000);
    clone.CloneAttribute(&sector); assert(clone.isClone && clone.GetAttributePtr()==&cells);
    clone.SetAttribute(1,2,2); assert(sector.IsAttr(50,100,2));
    clone.BindAttribute(&other); assert(clone.isClone && clone.GetAttributePtr()==&other);
    clone.Initialize(); assert(!clone.isClone && !clone.GetAttributePtr() && sector.GetAttributePtr()==&cells);
    std::puts("sector attributes passed: cell/world coordinates, signed truncation, bit masks, event bits and shared clones");
}

// Explicit unused entity/scheduler boundary for this isolated attribute slice.
bool CEntity::IsType(int) { std::abort(); }
BYTE CHARACTER::GetCharType() { std::abort(); }
void CHARACTER::StartStateMachine(int) { std::abort(); }
void CHARACTER::StopStateMachine() { std::abort(); }
extern "C" void _sys_err(const char*,int,const char*,...) { std::abort(); }
