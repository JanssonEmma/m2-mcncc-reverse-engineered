#include <map>
#include <vector>
#define private public
#include "../../game/src/skill.h"
#undef private
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <new>
static void Check(CSkillManager& m,DWORD key,CSkillProto* expected)
{
    const std::map<DWORD,CSkillProto*> before=m.m_map_pkSkillProto;
    assert(m.Get(key)==expected);
    assert(m.m_map_pkSkillProto==before); // Includes missing lookup no-insert and null value preservation.
}
int main()
{
    assert(sizeof(DWORD)==4);
    void* a=::operator new(1);void* b=::operator new(1);
    // Opaque pointer tokens ONLY. No CSkillProto object is constructed, reduced,
    // read, deleted or claimed recovered. Lookup does not dereference its values.
    CSkillProto* first=static_cast<CSkillProto*>(a);CSkillProto* second=static_cast<CSkillProto*>(b);
    {
        CSkillManager m;assert(CSkillManager::instance_ptr()==&m&&m.m_map_pkSkillProto.empty());
        Check(m,0,0);Check(m,0xffffffffUL,0);
        m.m_map_pkSkillProto[0]=first;m.m_map_pkSkillProto[1]=0;
        m.m_map_pkSkillProto[0x7fffffffUL]=second;m.m_map_pkSkillProto[0x80000000UL]=first;m.m_map_pkSkillProto[0xffffffffUL]=second;
        const DWORD keys[]={0,1,2,0x7ffffffeUL,0x7fffffffUL,0x80000000UL,0x80000001UL,0xfffffffeUL,0xffffffffUL};
        CSkillProto* values[]={first,0,0,0,second,first,0,0,second};
        for(unsigned repeat=0;repeat<3;++repeat)
            for(unsigned i=0;i<sizeof(keys)/sizeof(keys[0]);++i)Check(m,keys[i],values[i]);
        m.m_map_pkSkillProto[0x80000000UL]=second;Check(m,0x80000000UL,second);
        m.m_map_pkSkillProto.erase(0);Check(m,0,0);
        m.m_map_pkSkillProto.clear(); // Mandatory before narrow TEST-ONLY destructor.
    }
    assert(!CSkillManager::instance_ptr());
    ::operator delete(a);::operator delete(b);
    std::puts("skill manager pointer lookup passed: full manager map/ctor, unsigned keys, no insertion; prototype lifecycle NOT tested");
}

