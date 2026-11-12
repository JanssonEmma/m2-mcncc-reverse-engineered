#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/affect.h"
#include "desc_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <climits>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
extern "C" int desc_test_point_errors(void);
extern "C" int desc_test_point_value(void);
extern "C" int desc_test_point_maximum(void);
extern "C" int desc_test_point_line(void);
extern "C" const char* desc_test_point_name(void);
int main() {
    desc_test_allow_clock(); quest::CQuestManager quests; CHARACTER character;
    character.m_stName="stat-test";
    for(int i=0;i<255;++i) {
        character.SetRealPoint(static_cast<BYTE>(i),i*3-2000);
        character.m_pointsInstant.points[i]=i-500;
        assert(character.GetRealPoint(static_cast<BYTE>(i))==i*3-2000 && character.GetPoint(static_cast<BYTE>(i))==i-500);
    }
    assert(!desc_test_point_errors());
    character.SetRealPoint(254,-2147483647L-1); character.m_pointsInstant.points[254]=2147483647;
    assert(character.GetRealPoint(254)==(-2147483647L-1) && character.GetPoint(254)==2147483647);
    character.m_pointsInstant.points[0]=-2147483647L-1;
    assert(character.GetPoint(0)==(-2147483647L-1) && character.GetLimitPoint(0)==-2147483647);
    DescOutputProbe descriptor;
    struct Case { BYTE type; bool attached; long value; long expected; } cases[]={
        {17,false,-1,0},{17,false,0,0},{17,false,249,249},{17,false,250,250},{17,false,251,250},
        {17,true,169,169},{17,true,170,170},{17,true,171,170},{17,true,2147483647,170},
        {19,false,-2147483647L-1,0},{19,false,249,249},{19,false,251,250},
        {19,true,-1,0},{19,true,199,199},{19,true,200,200},{19,true,201,200},
        {62,false,999,999},{65,true,-123,-123},{114,false,13,13},{114,false,14,13},
        {115,true,-20,-20},{115,true,50,13},{63,false,50,50},{64,true,-10,-10}
    };
    for(unsigned i=0;i<sizeof(cases)/sizeof(cases[0]);++i) {
        const Case& test=cases[i]; character.BindDesc(test.attached ? &descriptor : 0);
        character.m_pointsInstant.points[test.type]=test.value;
        assert(character.GetLimitPoint(test.type)==test.expected);
        assert(character.m_pointsInstant.points[test.type]==test.value); // A limit read never stores the clamped result.
    }
    assert(!desc_test_point_errors());
    character.m_pointsInstant.points[63]=51;
    assert(character.GetPoint(63)==51 && desc_test_point_errors()==1 && desc_test_point_line()==2738);
    assert(desc_test_point_value()==51 && desc_test_point_maximum()==50 && !std::strcmp(desc_test_point_name(),"stat-test"));
    assert(character.GetLimitPoint(63)==50 && desc_test_point_errors()==2 && desc_test_point_line()==2784);
    character.m_pointsInstant.points[114]=51;
    assert(character.GetPoint(114)==51 && desc_test_point_errors()==2);
    assert(character.GetLimitPoint(114)==13 && desc_test_point_errors()==3 && desc_test_point_maximum()==50);
    for(int i=0;i<4;++i) { character.SetPart(static_cast<BYTE>(i),static_cast<WORD>(60000+i)); assert(character.GetPart(static_cast<BYTE>(i))==60000+i); }
    character.m_pointsInstant.bBasePart=255;
    assert(character.GetOriginalPart(0)==255 && character.GetOriginalPart(3)==60003);
    assert(!character.GetOriginalPart(1) && !character.GetOriginalPart(2) && !character.GetOriginalPart(4) && !character.GetOriginalPart(255));
    character.BindDesc(0); assert(character.GetOriginalPart(0)==60000 && character.GetOriginalPart(3)==60003);
    assert(!character.FindAffect(10));
    CAffect first={10,17,1,2,3,4},second={10,19,5,6,7,8},third={11,17,9,10,11,12},zero={10,0,0,0,0,0};
    character.m_list_pkAffect.push_back(&first); character.m_list_pkAffect.push_back(&second);
    character.m_list_pkAffect.push_back(&third); character.m_list_pkAffect.push_back(&zero);
    assert(character.FindAffect(10)==&first && character.FindAffect(10,17)==&first && character.FindAffect(10,19)==&second);
    assert(character.FindAffect(11,17)==&third && !character.FindAffect(11,19) && !character.FindAffect(12));
    assert(character.FindAffect(10,0)==&first); // Apply0 is a wildcard rather than a search specifically for apply0.
    character.m_list_pkAffect.pop_front(); assert(character.FindAffect(10)==&second && character.FindAffect(10,17)==0);
    assert(character.m_list_pkAffect.size()==3); character.m_list_pkAffect.clear();
    std::puts("character stats passed: old speed/point caps, diagnostics, appearance fallback and ordered affect lookup");
}
