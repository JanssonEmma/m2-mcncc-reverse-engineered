#include "../../game/src/constants.h"
#include "locale_combat_golden.h"
#include <cassert>
#include <cstring>
#include <cstdio>
template<class A,class B> struct LcSame {enum {value=0};};
template<class A> struct LcSame<A,A> {enum {value=1};};
template<class T,int N> void ConstArray(T& a,const unsigned char (&gold)[N]) {
    typedef char exact[(LcSame<T,const int[N/4]>::value)?1:-1];
    (void)sizeof(exact);assert(sizeof(a)==N);assert(std::memcmp(a,gold,N)==0);
}
template<class T> void MutableConstPointer(T&) {
    typedef char exact[(LcSame<T,const int*>::value)?1:-1];(void)sizeof(exact);
}
int main() {
    assert(sizeof(int)==4);
    assert(!aiPercentByDeltaLev && !aiPercentByDeltaLevForBoss && !aiChainLightningCountBySkillLevel);
    ConstArray(aiPercentByDeltaLev_euckr,golden_aiPercentByDeltaLev_euckr);
    ConstArray(aiPercentByDeltaLevForBoss_euckr,golden_aiPercentByDeltaLevForBoss_euckr);
    ConstArray(aiPercentByDeltaLev_gb2312,golden_aiPercentByDeltaLev_gb2312);
    ConstArray(aiPercentByDeltaLev_sjis,golden_aiPercentByDeltaLev_sjis);
    ConstArray(aiChainLightningCountBySkillLevel_euckr,golden_aiChainLightningCountBySkillLevel_euckr);
    ConstArray(aiChainLightningCountBySkillLevel_gb2312,golden_aiChainLightningCountBySkillLevel_gb2312);
    MutableConstPointer(aiPercentByDeltaLev);MutableConstPointer(aiPercentByDeltaLevForBoss);
    MutableConstPointer(aiChainLightningCountBySkillLevel);
    aiPercentByDeltaLev=aiPercentByDeltaLev_sjis;
    aiPercentByDeltaLevForBoss=aiPercentByDeltaLev_gb2312;
    aiChainLightningCountBySkillLevel=aiChainLightningCountBySkillLevel_gb2312;
    assert(aiPercentByDeltaLev[15]==100 && aiPercentByDeltaLevForBoss[30]==180 &&
           aiChainLightningCountBySkillLevel[40]==5);
    aiPercentByDeltaLev=0;aiPercentByDeltaLevForBoss=0;aiChainLightningCountBySkillLevel=0;
    std::puts("locale combat data: six exact arrays824bytes, constness/bounds and original null mutable pointers passed");
}
